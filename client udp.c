/*
** client.c -- a datagram "client" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/time.h>


#define SERVERPORT "1234"    // the port users will be connecting to
#define BUF_SIZE 300
#define PACKET_NUM 5
#define HOST "127.0.0.1"

//The first 7 bytes of the packet
struct packet {

    unsigned short int start;
    unsigned short int client_id:8;
    unsigned short int type;
    unsigned short int segment_no:8;
    unsigned short int length:8;
} send_buf = {0xFFFF, 0x01, 0xFFF1, 0x0, 0xFF};

struct packetReject {
    unsigned short int start;
    unsigned short int client_id:8;
    unsigned short int type;
    unsigned short int code;
    unsigned short int segment_no:8;
    unsigned short int end;
} rej_packet = {0xFFFF, 0x0, 0xFFF3, 0x0, 0x0, 0xFFFF};

//The ending 2 bytes of the packet
struct endsign {
    unsigned short int end;
} end = {0xFFFF};

//Sending buffer
char buffer[BUF_SIZE];

//Recieving buffer
char recieving[BUF_SIZE];

//Payload buffer
char payload[256];
struct addrinfo hints, *servinfo, *p;
unsigned short int error_code;
unsigned short int reject_code;
int sockfd;

void sending(int, int, int);

void send_normal();

void send_error1();

void send_error2();

void send_error3();

void send_error4();

int main() {

    int rv;
    int numbytes;
    int packet_num = 0;
    struct timeval timeout;
    int count;
    int code;

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(HOST, SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");

        return 2;
    }

    //Set Recv timer
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));

    // Send
    for (packet_num = 0; packet_num < PACKET_NUM; packet_num++) {
        printf("Enter your payload:\n");
        bzero(payload, 255);
        gets(payload);
        send_buf.type = 0XFFF1;
        send_buf.length = strlen(payload);

        if (packet_num == 0) send_normal();
        else if (packet_num == 1) send_error1();
        else if (packet_num == 2) send_error2();
        else if (packet_num == 3) send_error3();
        else if (packet_num == 4) send_error4();


        rv = recvfrom(sockfd, recieving, BUF_SIZE - 1, MSG_WAITALL, p->ai_addr, &p->ai_addrlen);

        //Time out
        count = 0;
        while (rv == -1) {
            if (count == 3) {
                perror("No server response.\n\n");
                exit(1);
            }
            count++;
            printf("Packet %d . No response %d time\n", send_buf.segment_no, count);
            rv = recvfrom(sockfd, recieving, BUF_SIZE - 1, MSG_WAITALL, p->ai_addr, &p->ai_addrlen);
            if (rv > 0) break;
            sendto(sockfd, buffer, sizeof(send_buf) + strlen(payload) + 2, 0, p->ai_addr, p->ai_addrlen);

        }
        //only copy the front part 5 bytes
        memcpy(&send_buf, recieving, 5);
        printf("From receving packet's type: \"%#X\"\n", send_buf.type);
        if (send_buf.type == 0xFFF3) {
            //copy
            memcpy(&rej_packet, recieving, sizeof(rej_packet));

            if (rej_packet.code == 0xFFF4)
                printf("Reject Error: Packet out of sequence. %#X \n\n", rej_packet.code);

            else if (rej_packet.code == 0xFFF5)
                printf("Reject Error: Packet length does not match. %#X \n\n", rej_packet.code);

            else if (rej_packet.code == 0xFFF6)
                printf("Reject Error: End of packet lost. %#X \n\n", rej_packet.code);

            else if (rej_packet.code == 0xFFF7)
                printf("Reject Error: Duplicate packet. %#X \n\n", rej_packet.code);

        }

    }

    freeaddrinfo(servinfo);
    close(sockfd);

    return 0;
}


void send_normal() {
    printf("This is a normal pocket!\n");
    send_buf.type = 0XFFF1;
    send_buf.segment_no = 1;
    memset(buffer, 0, BUF_SIZE);
    memcpy(buffer, &send_buf, sizeof(send_buf));
    memcpy(buffer + sizeof(send_buf), payload, strlen(payload));
    memcpy(buffer + sizeof(send_buf) + strlen(payload), &end, 2);
    sendto(sockfd, buffer, sizeof(send_buf) + strlen(payload) + 2, 0, p->ai_addr, p->ai_addrlen);
}

//packet number is out of sequence
void send_error1() {
    printf("send a out-of-sequence pocket!\n");
    send_buf.segment_no = 5;
    memset(buffer, 0, BUF_SIZE);
    memcpy(buffer, &send_buf, sizeof(send_buf));
    memcpy(buffer + sizeof(send_buf), payload, strlen(payload));
    memcpy(buffer + sizeof(send_buf) + strlen(payload), &end, 2);
    sendto(sockfd, buffer, sizeof(send_buf) + strlen(payload) + 2, 0, p->ai_addr, p->ai_addrlen);
}

//payload length does not match
void send_error2() {
    printf("This is a wrong payload length pocket!\n");
    send_buf.segment_no = 3;
    send_buf.length = 0x33;
    memset(buffer, 0, BUF_SIZE);
    memcpy(buffer, &send_buf, sizeof(send_buf));
    memcpy(buffer + sizeof(send_buf), payload, strlen(payload));
    memcpy(buffer + sizeof(send_buf) + strlen(payload), &end, 2);
    sendto(sockfd, buffer, sizeof(send_buf) + strlen(payload) + 2, 0, p->ai_addr, p->ai_addrlen);
}

//end lost
void send_error3() {
    printf("This is a end-lost pocket!\n");
    send_buf.segment_no = 4;
    send_buf.length = 0xFF;
    memset(buffer, 0, BUF_SIZE);
    memcpy(buffer, &send_buf, sizeof(send_buf));
    memcpy(buffer + sizeof(send_buf), payload, strlen(payload));
//  memcpy(buffer + sizeof(send_buf) + strlen(payload), &end, 2);
    sendto(sockfd, buffer, sizeof(send_buf) + strlen(payload), 0, p->ai_addr, p->ai_addrlen);
}

//duplicated number
void send_error4() {
    printf("This is a duplicated pocket.\n");
    send_buf.segment_no = 4;
    memset(buffer, 0, BUF_SIZE);
    memcpy(buffer, &send_buf, sizeof(send_buf));
    memcpy(buffer + sizeof(send_buf), payload, strlen(payload));
    memcpy(buffer + sizeof(send_buf) + strlen(payload), &end, 2);
    sendto(sockfd, buffer, sizeof(send_buf) + strlen(payload) + 2, 0, p->ai_addr, p->ai_addrlen);

}



