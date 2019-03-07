#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

char* PORT = "8080";

int main() {

    int status;

    // get network info
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);  // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;      // use IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;      // fill in my IP for me

    if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // res now points to a linked list of 1 or more struct addrinfos

    // make a socket
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        // TODO: use errno for more error info
        fprintf(stderr, "socket creation error\n");
        exit(1);
    }

    // bind it to the port passed to getaddrinfo()
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        // TODO: use errno for more error info
        fprintf(stderr, "socket bind error\n");
        exit(1);
    };
    freeaddrinfo(res); // free the linked-list

    printf("Listening on port %s...", PORT);
    // listen on the port
    // TODO: pick a better number for backlog
    if (listen(sockfd, 5) == -1) {
        // TODO: use errno for more error info
        fprintf(stderr, "socket listen error\n");
        exit(1);
    }
    printf("foo\n");

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

    // receive from connection
    int numbytes;
    char buf[400];  // TODO: pull out to constant
    if ((numbytes = recv(incoming_fd, buf, 400-1, 0)) == -1) {
        perror("recv error");
        exit(1);
    }

    buf[numbytes] = '\0';  // end buffer string

    printf("client: received '%s'\n",buf);
}