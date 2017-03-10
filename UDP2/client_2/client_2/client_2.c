//
//  main.c
//  client_2
//
//  Created by Chang Liu on 2/25/17.
//  Copyright © 2017 Chang Liu. All rights reserved.
//

#/*
** client.c -- a datagram "client"
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVERPORT "1235"    // the port users will be connecting to
#define SIZE 300
#define HOST "127.0.0.1"


//Sending buffer
char buffer[SIZE];

//Recieving buffer
char rec_buf[SIZE];

struct addrinfo hints, *servinfo, *p;

//The packet format
struct packet {
    unsigned short int start;
    unsigned short int client_id:8;
    unsigned short int data_type;
    unsigned short int seg_num:8;
    unsigned short int length:8;
    unsigned short int tec:8;
    unsigned int sub_num;
    unsigned short int end;
} send_buf = {0xFFFF, 0x01, 0x0, 0x1, 0xE, 0x0, 0x0, 0xFFFF};


int main(int argc, char *argv[]) {
    int sockfd;
    int rv;
    
    struct timeval timeout;
    int count;
    
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    
    if ((rv = getaddrinfo(HOST, SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }
    printf("Secceed to bind socket.\n");
    
    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return -1;
    }
    
    //set timer
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));
    
    //send 3 subscriber numbers
    for (int i = 0; i < 3; i++) {
        count = 0;
        rv = -1;
        //subscriber_no = atoi(argv[1]);
        //technology = atoi(argv[2]);
        if (i == 0) {
            printf("1. Send subscriber number: 408-555-5555 (4G) Not exist.\n");
            send_buf.sub_num = 4085555555;
        } else if (i == 1) {
            printf("2. Send subscriber number: 408-554-6805 (4G) Paid\n");
            send_buf.sub_num = 4085546805;
        } else if (i == 2) {
            printf("3. Send subscriber number: 408-666-8821 (3G) Not paid\n");
            send_buf.sub_num = 4086668821;
        }
        
        send_buf.data_type = 0XFFF8;  //acc_Per
        
        //compele the packet format
        memset(buffer, 0, SIZE);
        memcpy(buffer, &send_buf, sizeof(send_buf));
        memset(rec_buf, 0, SIZE);
        sendto(sockfd, buffer, sizeof(send_buf), 0, p->ai_addr, p->ai_addrlen);
        
        //the client will resent 3 times if the timer is timeout
        rv = recvfrom(sockfd, rec_buf, SIZE - 1, MSG_WAITALL, p->ai_addr, &p->ai_addrlen);
        count = 0;
        while (rv == -1) {
            if (count == 3) {
                perror("No server response.\n\n");
                exit(1);
            }
            count++;
            printf("Packet %d No response %d time\n", send_buf.seg_num, count);
            rv = recvfrom(sockfd, rec_buf, SIZE - 1, MSG_WAITALL, p->ai_addr, &p->ai_addrlen);
            // if connected, send packet.
            if (rv > 0) break;
            sendto(sockfd, buffer, sizeof(send_buf), 0, p->ai_addr, p->ai_addrlen);
        }
        
        memcpy(&send_buf, rec_buf, sizeof(send_buf));
        
        if (send_buf.data_type == 0xFFFA) {
            printf("Reject: Subscriber does not exist on database - %#X \n\n", send_buf.data_type);
        } else if (send_buf.data_type == 0xFFF9) {
            printf("Reject: Subscriber has not paid - %#X \n\n", send_buf.data_type);
        } else if (send_buf.data_type == 0xFFFB) {
            printf("Succeed to access the network!\n");
            printf("The technology type is %dG \n\n", send_buf.tec);
        }
    }
    freeaddrinfo(servinfo);
    close(sockfd);
    
    return 0;
}

