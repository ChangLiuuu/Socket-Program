//
//  main.c
//  server_2
//
//  Created by Chang Liu on 2/20/17.
//  Copyright © 2017 Chang Liu. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define MYPORT "1235"    // the port users will be connecting to
#define MAXBUFLEN 500
#define FILENAME "database.txt"


struct packet {
    unsigned short int header_id;
    unsigned short int client_id:8;
    unsigned short int packet_type;
    unsigned short int seg_num:8;
    unsigned short int length:8;
    unsigned short int tec:8;
    unsigned int sub_num;
    unsigned short int end;
} buffer = {0xFFFF, 0x01, 0x0, 0x1, 0xE, 0x0, 0x0, 0xFFFF};

struct database{
    unsigned int sub_no;
    int tech;
    int paid;
} subscribers[3];

//Recieving buffer
char buf[MAXBUFLEN];

struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr;
socklen_t addr_len;
char s[INET6_ADDRSTRLEN];

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    struct sockaddr_storage their_addr;
    char *filename;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }
        
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }
    
    freeaddrinfo(servinfo);
    printf("listener: waiting to recvfrom...\n");
    addr_len = sizeof their_addr;
    
    filename = FILENAME;
    FILE * file;
    char buf[MAXBUFLEN];
    char *data[3];
    char *original_num;
    char final_num[20];
    int user_no = 0;
    
    /*read database*/
    file = fopen (filename, "r");
    if (file == NULL){
        perror ("Error opening file");
        exit(-1);
    }
    
    while (fgets(buf,MAXBUFLEN,file) != NULL){
        //split " "
        original_num = strtok(buf, "\t");
        subscribers[user_no].tech = atoi(strtok(NULL, "\t"));
        subscribers[user_no].paid = atoi(strtok (NULL, "\t"));
        //split "-"
        data[0] = strtok(original_num, "-");
        data[1] = strtok(NULL, "-");
        data[2] = strtok (NULL, "-");
        
        //appends and gets number
        bzero(final_num, sizeof(final_num));
        strcat(final_num, data[0]);
        strcat(final_num, data[1]);
        strcat(final_num, data[2]);
        
        //get the subscriber number.
        // array to integer
        subscribers[user_no].sub_no = atoi(final_num);
        bzero(buf, MAXBUFLEN);
        user_no++;
    }
    fclose (file);
    
    while (1) {
        //receive
        int numbytes;
        bzero(buf, MAXBUFLEN);
        memset(&buffer, 0, sizeof(buffer));
        numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1 , 0,(struct sockaddr *)&their_addr, &addr_len);
        
        if (numbytes == -1) {
            perror("recvfrom error!\n");
            exit(1);
        }
        
        memcpy(&buffer, buf, sizeof(buffer));
        printf("The subscriber number is %u \n", buffer.sub_num);
        
        //printf("End of packet is %#X \n", buffer.end);
        
        //check
        int i = 0;
        // initialize packet_type.
        buffer.packet_type = 0xFFFA;
        
        for (i = 0; i < 3; i++) {
            if (buffer.sub_num == subscribers[i].sub_no) {
                buffer.tec = subscribers[i].tech;
                printf("Subscriber technology is  %dG \n", subscribers[i].tech);
                
                if (subscribers[i].paid == 1) {
                    buffer.packet_type = 0xFFFB;
                    printf("Subscriber can access the network! \n\n");
                } else {
                    buffer.packet_type = 0xFFF9;
                    printf("Subscriber has not paid! \n\n");
                }
            }
        }
        
        if (buffer.packet_type == 0xFFFA) {
            printf("Subscriber does not exist in database. \n\n");
        }
        
        bzero(buf, MAXBUFLEN);
        memcpy(buf, &buffer, sizeof(buffer));
        
        if (sendto(sockfd, buf, MAXBUFLEN - 1 , 0,(struct sockaddr *)&their_addr, addr_len) == -1) {
            perror("send error!");
            exit(1);
        }
        
    }
    
}


