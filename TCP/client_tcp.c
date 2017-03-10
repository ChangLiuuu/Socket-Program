//
//  main.c
//  UDP
//
//  Created by Chang Liu on 2/11/17.
//  Copyright © 2017 Chang Liu. All rights reserved.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 1234
#define REMOTE_IP "127.0.0.1"
#define BUFFER_SIZE 264
int main(){
    char packet1[BUFFER_SIZE];
    int inputLen;
    int socket_client = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;  //vip 4
    serv_addr.sin_addr.s_addr = inet_addr(REMOTE_IP);
    serv_addr.sin_port = htons(PORT);
    int n = connect(socket_client, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (n == -1) {
        perror("Failed to connect.\n");
        exit(1);
    }
    printf("Success to connect.");
    
    // input
    int startId = 0XFFFF;
    printf(startId);
    while (1) {
        if ((packet1[inputLen++] = getchar()) == '\n') {
            break;
        }
    }
    //send
    send(socket_client, packet1, BUFFER_SIZE, 0);
    
    //
   // close(socket_client);
    return 0;
}
