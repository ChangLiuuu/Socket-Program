//
//  main.c
//  UDP
//
//  Created by Chang Liu on 2/11/17.
//  Copyright © 2017 Chang Liu. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 1234
#define REMOTE_IP "127.0.0.1"

int main () {
    /*
     domain: protocol family witch will be used for commuication
     type  : specifies the communication semantics
     int socket(int domain, int type, int protocol);
     */
    
    int domain = AF_INET;  //ipv 4
    int type = SOCK_DGRAM; //socket type
    int protocol = 0; //default protol
    int socket_server = socket(domain, type, protocol); // create a socket filedescriptor
    
    if (socket_server == -1) {
        perror("Failed to create socket.\n");
        exit(1);   //exit caused by error
    }
    /* socket creates successful!, then bands.
     binding needs a structure, here is ipv4 structure
     man 7 ip
     struct sockaddr_in {
     sa_family_t    sin_family; address family: AF_INET
     in_port_t      sin_port;    port in network byte order
     struct in_addr sin_addr;   internet address
     };
     
     Internet address.
     struct in_addr {
     uint32_t       s_addr;     address in network byte order
     };
     */
    
    printf("socket create success!\n");
    struct sockaddr_in serv_addr; //define a structure
    serv_addr.sin_family = AF_INET; //ipv 4
    serv_addr.sin_port = htons(PORT); // port in network byte order
    serv_addr.sin_addr.s_addr = inet_addr(REMOTE_IP); //internet address
    //serv_addr.sin_addr.s_addr = INADDR_ANY; // "0.0.0.0"
    
    /*bind*/
    socklen_t addrlen = sizeof(serv_addr);
    //result: if bind successfully, return 0 or SOCKET_ERROR
    long result = bind(socket_server, (struct sockaddr*)&serv_addr, addrlen);
    if (result == -1) {
        perror("Failed to bind to the socket.\n");
        exit(1);
    }
    printf("bind success!\n");
    
    // listen() marks the socket referred to by sockfd as a passive socket
    // a socket that will just be used to accept incoming connection requests
    // int listen(int sockfd, int backlog)The backlog argument defines the maximum length to which the queue of pending connections for sockfd may grow.
    result = listen(socket_server, 1); // if backlog = SOMAXCONN, value is 128
    if (result == -1) {
        perror("Failed to listen socket.\n");
        exit(1);
    }
    printf("listening......\n");
    
    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    // accept can make a new connection used to writing and reading with clients
    struct sockaddr_in peeraddr; // the incoming tcpip address structure
    addrlen = sizeof(peeraddr);
    unsigned int connfd = accept(socket_server, (struct sockaddr*)&peeraddr, &addrlen);
    if (connfd == -1) {
        perror("Failed to accept. Exiting..\n");
        exit(1);
    }
    printf("accept success!\n");
        
    /*read data from client side*/
    char recv_buf[264];

    while (1) {
        // ssize_t read(int fd, void *buf, size_t count);
        result = read(connfd, recv_buf, sizeof(recv_buf));
        // read data successfully, and then send
        fputs(recv_buf, stdout);
        char *send_buf = "from Server: data had received, thanks!";
        result = write(connfd, send_buf, sizeof(send_buf));
        if (result == 0) {
            printf("nothing was writen to client!\n");
        } else if (result == -1) {
            printf("writen to client error!\n");
        } 
    }
}
