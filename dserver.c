//
//  dserver.c
//  
//
//  Created by AakashBasnet on 4/17/18.
//

//UDPServer.c

/*
 *  gcc -o server UDPServer.c
 *  ./server
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define BUFLEN 1000
#define PORT 9930
#define LENGTH 32

void err(char *str)
{
    perror(str);
    exit(1);
}

int main(void)
{
    struct sockaddr_in my_addr, cli_addr;
    int sockfd, i;
    int recv_result;
    socklen_t slen=sizeof(cli_addr);
    char buf[BUFLEN];
    char buffer[LENGTH];
    char ack_buffer[2];
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        err("socket");
    else
        printf("Server : Socket() successful\n");
    
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1)
        err("bind");
    else
        printf("Server : bind() successful\n");
    
//    while(1)
//    {
//        if (recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &slen)==-1)
//            err("recvfrom()");
//        printf("Received packet from %s:%d\nData: %s\n\n",
//               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buf);
//    }
    int packet_length = 0;
    char filelength[4];
    int bytes_sent;

    
    while(1){

        recv_result = recvfrom(sockfd, buffer, LENGTH, 0, (struct sockaddr*)&cli_addr, &slen);
        printf("Bytes received %d\n", recv_result);
        printf("Continue\n");
        if(recv_result>0){
            ack_buffer[0] = 1;
            ack_buffer[1] = '\0';
            bytes_sent = sendto(sockfd, ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr*)&cli_addr, slen);
            printf("Acknowledgement sent %d\n",bytes_sent);
            memcpy(filelength, buffer, 4);
            int file_length = *filelength;
            char file_content[file_length];
            printf("%d. is the file length\n", file_length);
            memcpy(file_content, buffer+4, file_length);
            memcpy(buf+packet_length, file_content, file_length);
            packet_length = packet_length + file_length;
            if(file_length < 20){
                break;
            }
        }    
    }

    for(int i = 0; i < packet_length; i++){
           printf("%d. The character member is %d\n",i+1, buf[i]);
       }

    //creating a dummy target file and format type for now
    char format[2];
    format[0] = 1;
    format[1] = '\0';

    char* target_file = "output";

    //process_file(file_content, file_length, target_file, format);
    printf("Process file called\n");
    
    close(sockfd);
    return 0;
}

