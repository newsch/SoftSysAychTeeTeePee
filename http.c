#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#ifndef VERSION
#define VERSION "pre-release"
#endif

char* PORT = "8080";


typedef struct {
    char* name;
    char* content;
} header_t;

// HTTP Message https://tools.ietf.org/html/rfc2616#section-4
typedef struct {
    char* startline;
    header_t* headers;
    int numheaders;
    char* body;
} message_t;

// HTTP Request-Line https://tools.ietf.org/html/rfc2616#section-5.1
typedef struct {
    char method[8];
    char* requestUri;
    char* version;
} requestline_t;

typedef struct {
    requestline_t requestline;
    char** headers;
    char* body;
} request_t;

// HTTP Status-Line https://tools.ietf.org/html/rfc2616#section-6.1
typedef struct {
    char* version;
    int statuscode;
    char* reasonphrase;
} statusline_t;

typedef struct {
    char* version;
    int code;
    char* reason;

    header_t* headers;
    int numheaders;
    char* body;
} response_t;

header_t GLOBAL_HEADERS[] = {
    {"Server", "ATTP/" VERSION},
    {"Date", "Wed, 16 Apr 97 22:54:42 GMT"},
    // {"Connection", "close"},  // default for 1.0
    // {"Content-Length", "7"},
    {"Content-Type", "text/html; charset=utf-8"},
};

int NUM_GLOBAL_HEADERS = sizeof(GLOBAL_HEADERS) / sizeof(header_t);


// send an HTTP message over a socket
int sendmessage(int sockfd, message_t* msg) {
    int SENDBUFSIZE = 1024;  // in bytes
    char* buffer = malloc(SENDBUFSIZE);  // TODO: handle malloc errors
    // TODO: count how many bytes I'm adding so send can happen
    strcpy(buffer, msg->startline);
    strcat(buffer, "\r\n");
    for (int i=0; i<msg->numheaders; i++) {
        header_t h = msg->headers[i];
        strcat(buffer, h.name);
        strcat(buffer, ": ");
        strcat(buffer, h.content);
        strcat(buffer, "\r\n");
    }
    strcat(buffer, "\r\n");
    strcat(buffer, msg->body);
    send(sockfd, buffer, strlen(buffer), 0);  // TODO: don't send more than the buffer
    return 0;
}

// int parse_header(char* name, char* content, struct argp_state* state) {

// }


int sendresponse(int sockfd, response_t* rsp) {
    char startline[128];
    snprintf(startline, sizeof(startline), "HTTP/%s %d %s", rsp->version, rsp->code, rsp->reason);
    message_t msg = {
        startline,
        rsp->headers,
        rsp->numheaders,
        rsp->body
    };
    int response = sendmessage(sockfd, &msg);
    return response;
}

int sendcode(int sockfd, int code, char* reason) {
    response_t rsp = {
        "1.0",
        code,
        reason,
        GLOBAL_HEADERS,
        NUM_GLOBAL_HEADERS,
        "",
    };
    return sendresponse(sockfd, &rsp);
}

// send a file over a socket
void sendfile(int sockfd, char* filename) {
    // TODO: make mimetype from extension
    FILE* f = fopen(filename, "rb");

    int BUFSIZE = 100;
    char buffer[BUFSIZE];

    size_t bytesread;
    while ((bytesread = fread(buffer, sizeof(char), BUFSIZE, f))) {
        printf("Read %li bytes from file %s\n", bytesread, filename);
        send(sockfd, buffer, bytesread, 0);
    }
}

void handlefilerequest(int sockfd, char* filename) {
    sendcode(sockfd, 200, "OK");
    sendfile(sockfd, filename);
}

void return404(int sockfd) {
    message_t msg = {
        "HTTP/1.0 404 Not Found",
        NULL,
        0,
        ""
    };
    sendmessage(sockfd, &msg);
}

int main() {

    int status;

    // get network info
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof hints);  // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;      // use IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;      // fill in my IP for me

    if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // res now points to a linked list of 1 or more struct addrinfos

    // // make a socket
    int sockfd; // = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    // if (sockfd == -1) {
    //     // TODO: use errno for more error info
    //     fprintf(stderr, "socket creation error\n");
    //     exit(1);
    // }

    // // bind it to the port passed to getaddrinfo()
    // if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    //     // TODO: use errno for more error info
    //     fprintf(stderr, "socket bind error\n");
    //     exit(1);
    // };

    // loop through all the results and bind to the first we can
    int yes = 1;
    for(p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(res); // free the linked-list

    if (p == NULL)  {
        fprintf(stderr, "failed to bind\n");
        exit(1);
    }

    printf("Listening on port %s...\n", PORT);
    // listen on the port
    // TODO: pick a better number for backlog
    if (listen(sockfd, 5) == -1) {
        // TODO: use errno for more error info
        fprintf(stderr, "socket listen error\n");
        exit(1);
    }
    printf("foo\n");

    while (1) {
        // accept incoming connection (blocking?)
        struct sockaddr_storage their_addr;
        socklen_t addr_size;
        int incoming_fd;
        addr_size = sizeof(their_addr);
        incoming_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
        if (incoming_fd == -1) {
            // TODO: use errno for more error info
            fprintf(stderr, "accept error\n");
            exit(1);
        }
        printf("New connection");

        // receive from connection
        int numbytes;
        char buf[400];  // TODO: pull out to constant
        if ((numbytes = recv(incoming_fd, buf, 400-1, 0)) == -1) {
            perror("recv error");
            exit(1);
        }

        buf[numbytes] = '\0';  // end buffer string

        printf("client: received '%s'\n",buf);

        // send(incoming_fd, "foo", 3, 0);
        // sendcode(incoming_fd, 404, "Not Found");
        handlefilerequest(incoming_fd, "index.html");

        puts("Returned 404");
        close(incoming_fd);
        puts("Closed connection");
        // break;
    }

}