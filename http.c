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
    // {"Date", "Wed, 16 Apr 97 22:54:42 GMT"},
    // {"Connection", "close"},  // default for 1.0
    // {"Content-Length", "7"},
    // {"Content-Type", "text/plain; charset=utf-8"},
};

int NUM_GLOBAL_HEADERS = sizeof(GLOBAL_HEADERS) / sizeof(header_t);


int sendresponsestartline(int sockfd, int code, char* reason) {
    int BUFSIZE = 128;
    char buffer[BUFSIZE];
    int status = snprintf(buffer, BUFSIZE, "HTTP/%s %d %s\r\n", "1.0", code, reason);
    if (status < 0 || status >= BUFSIZE) {
        return -1;
    }
    status = send(sockfd, buffer, status, 0);
    if (status < 0) {
        return status;
    } else {
        return 0;
    }
}


void sendheaders(int sockfd, header_t* headers, int numheaders) {
    int BUFSIZE = 1024;
    char buffer[BUFSIZE];

    // TODO: check for buffer overflow
    for (int i=0; i<numheaders; i++) {
        header_t h = headers[i];
        strncpy(buffer, h.name, BUFSIZE);
        strcat(buffer, ": ");
        strcat(buffer, h.content);
        strcat(buffer, "\r\n");
        send(sockfd, buffer, strlen(buffer), 0);
    }
}

// send an HTTP message over a socket
int sendmessage(int sockfd, message_t* msg) {
    int SENDBUFSIZE = 1024;  // in bytes
    char* buffer = malloc(SENDBUFSIZE);  // TODO: handle malloc errors
    // TODO: count how many bytes I'm adding so send can happen without strlen
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
    send(sockfd, buffer, strlen(buffer), 0);
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

void send404(int sockfd) {
    sendresponsestartline(sockfd, 404, "Not Found");
    sendheaders(sockfd, &((header_t) {"Content-Length", "3"}), 1);
    send(sockfd, "\r\n404", 5, 0);
}

long getfilelength(FILE* fp) {
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    return size;
}

// send a file over a socket
void sendfile(int sockfd, FILE* fp) {

    int BUFSIZE = 100;
    char buffer[BUFSIZE];

    size_t bytesread;
    while ((bytesread = fread(buffer, sizeof(char), BUFSIZE, fp))) {
        printf("Read %li bytes from file.\n", bytesread);
        send(sockfd, buffer, bytesread, 0);
    }
}

// Translate a uri from a get request into a relative path
int resolvepath(char* resp, char* path) {
    size_t plen = strlen(path);
    if (path[0] == '/') {
        strcpy(resp, path + 1);
    } else {
        strcpy(resp, path);
    }
    if (path[plen - 1] == '/') {
        strcat(resp, "index.html");
    }
    #ifdef DEBUG
    printf("Changed \"%s\" to \"%s\"\n", path, resp);
    #endif
    return 0;
}

void sendfileresponse(int sockfd, char* filename) {
    char fn[sizeof(filename) / sizeof(char) + 16 * sizeof(char)];
    // TODO: make mimetype from extension
    // TODO: check for directory names
    resolvepath(fn, filename);

    FILE* fp = fopen(fn, "rb");
    if (!fp) {
        send404(sockfd);
        return;
    }

    long fsize = getfilelength(fp);
    char fsizestr[sizeof(long)];
    snprintf(fsizestr, sizeof(fsizestr) / sizeof(char), "%li", fsize);
    header_t contentheader = {"Content-Length", fsizestr};
    // TODO: add Date
    // TODO: add content-type

    // TODO: check statuses
    sendresponsestartline(sockfd, 200, "OK");
    sendheaders(sockfd, GLOBAL_HEADERS, NUM_GLOBAL_HEADERS);
    sendheaders(sockfd, &contentheader, 1);
    send(sockfd, "\r\n", 2, 0);

    sendfile(sockfd, fp);
}

// read into buffer until newline "\r\n"
int readline(int sockfd, char* buffer, int bufsize) {
    size_t buf_idx = 0;

    while (buf_idx < bufsize - 1 && 1 == read(sockfd, &buffer[buf_idx], 1)) {
        if (buf_idx > 0 &&
            '\n' == buffer[buf_idx] &&
            '\r' == buffer[buf_idx - 1])
        {
            buffer[buf_idx + 1] = '\0';
            break;
        }
        buf_idx++;
    }
    return (int) buf_idx;
}

int readrequestline(int sockfd, requestline_t* store, int lenM, int lenU, int lenV) {
    int BUFSIZE = 1024;
    char buffer[BUFSIZE];
    int numread = readline(sockfd, buffer, BUFSIZE);
    if (numread < 0) {
        return -1;
    } else if (numread >= BUFSIZE) {
        return -2;
    }
    printf("got:\n%s\n", buffer);

    // not thread-safe, per strtok definition
    // TODO: look into strtok_r
    char* method = strtok(buffer, " ");  // read method until first space
    char* uri = strtok(NULL, " ");  // get uri until second space
    char* newend = strtok(NULL, "/");  // remove HTTP/
    char* version = strtok(NULL, "\r\n");  // get version after HTTP/ until \r\n

    if (method == NULL || uri == NULL || version == NULL || newend == NULL) {
        return -1;
    }

    strncpy(store->method, method, lenM);
    strncpy(store->requestUri, uri, lenU);
    strncpy(store->version, version, lenV);

    if (store->method[lenM-1] != '\0' ||
        store->requestUri[lenU-1] != '\0' ||
        store->version[lenV-1] != '\0') {
        return -3;
    }

    return 0;
}

// int readheaderline(int sockfd, header_t* store, lenN, lenC) {
//     int BUFSIZE = 1024;
//     char buffer[BUFSIZE];
//     int numread = readline(sockfd, buffer, BUFSIZE);
//     if (numread == 2) {  // empty final line
//         return 1;
//     }

// }

// int parseuri(char* uri) {
//     // pattern from https://tools.ietf.org/html/rfc3986#appendix-B
//     const char pattern = "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?";
//     regex_t re;
//     int res = regcomp(&re, pattern, REG_NOSUB);  // ignore case and get success/fail
//     if (res != 0) {
//         return -1;
//     }
//     regfree(&re);

// }

int handlerequest(int sockfd) {
    char uri[128];
    char ver[8];
    requestline_t r = {
        "",
        uri,
        ver
    };
    int status = readrequestline(sockfd, &r, 8, 128, 8);
    #ifdef DEBUG
    printf("readrequestline returned %i\n", status);
    #endif
    if (status == -2) {
        sendcode(sockfd, 400, "Request start line too long");
        return -1;
    } else if (status < 0) {
        sendcode(sockfd, 500, "Internal Server Error");
        return -1;
    }
    #ifdef DEBUG
    printf("method: %s\nuri: %s\nver: %s\n", r.method, r.requestUri, r.version);
    #endif
    // sendcode(sockfd, 431, "Request Header Fields Too Large")
    sendfileresponse(sockfd, r.requestUri);
    return 0;
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
        // int numbytes;
        // char buf[400];  // TODO: pull out to constant
        // if ((numbytes = recv(incoming_fd, buf, 400-1, 0)) == -1) {
        //     perror("recv error");
        //     exit(1);
        // }

        // buf[numbytes] = '\0';  // end buffer string

        // printf("client: received '%s'\n",buf);
        handlerequest(incoming_fd);

        // send(incoming_fd, "foo", 3, 0);
        // sendcode(incoming_fd, 404, "Not Found");
        // sendfileresponse(incoming_fd, "index.html");

        puts("Finished Response");
        close(incoming_fd);
        puts("Closed connection");
        // break;
    }

}