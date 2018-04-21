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
#include "sendlib.c"
#define BUFLEN 1000
#define PORT 9930
#define LENGTH 32
#define HEADER_LENGTH 8

void err(char *str)
{
    perror(str);
    exit(1);
}

int main(void)
{   char *szPort;
    char *lossProbab;
    char *randomSeed;
    char *endptr;

    
    short port;
    char header_buffer[HEADER_LENGTH];
    struct sockaddr_in my_addr, cli_addr;
    int sockfd, i;
    int recv_result;
    socklen_t slen=sizeof(cli_addr);
    char buf[BUFLEN];
    char buffer[LENGTH];
    char ack_buffer[2];

    if(argc != 4)
    {
        printf("Incomplete Command Line arguments\n");
        exit(0);
        return -1;
    }
    szPort = argv[1];
    lossProbab = argv[2];
    randomSeed = argv[3];

    port = strtol(szPort, &endptr, 0);
    if(*endptr){
        printf("EchoCLNT: Invalid Port supplied.\n");
        exit(EXIT_FAILURE);
    }

    float loss_probability = atoi(lossProbab);
    int random_seed = atoi(randomSeed);
    
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
    // printf("Preparing to receive the header file\n");
    // while(1){
    //     printf("...........\n");
    //     recv_result = recvfrom(sockfd, header_buffer, HEADER_LENGTH, 0, (struct sockaddr*)&cli_addr, &slen);
    //     printf("Bytes received %d\n", recv_result);
    //     if(recv_result>0){
    //         char format[1];
    //         format[0] = header_buffer[0];
    //         uint8_t format_ = *format;     //format_ stores the 8 bit integer value of format


    //         uint8_t file_name_length = header_buffer[1]; //file name length has been copied
    //         char target_file[file_name_length+1];

    //         memcpy(target_file, header_buffer+2, file_name_length); //copies into the memory
    //         target_file[file_name_length] = '\0';    //target file will be stored as string


    //         char fileLength[4];                      //stores the file length for now 
    //         memcpy(filelength, header_buffer+2+file_name_length, 4);
    //         int file_Length = *fileLength;

    //         char ack[2] = ['1','\0'];

    //         int sent_ack = sendto(sockfd, ack, sizeof(ack), 0, (struct sockaddr*)&cli_addr, slen);



    //         break;
  
    //     }    
    // }
    
    /*Getting the format type*/

    printf("Preparing to send the files..................................................\n");
    int packet_length = 0;
    char filelength[4];
    char expectedsqnum[4];
    int bytes_sent;

    int packet_num = 0;
    int sq_num = 0;
    

    
    while(1){
        printf("...........\n");
        recv_result = recvfrom(sockfd, buffer, LENGTH, 0, (struct sockaddr*)&cli_addr, &slen);
        printf("Bytes received %d\n", recv_result);
        printf("Continue\n");
        if(recv_result>0){
            
            printf("Acknowledgement sent %d\n",bytes_sent);
            memcpy(filelength, buffer, 4);
            int file_length = *filelength;
            char file_content[file_length];
            printf("%d. is the file length\n", file_length);
            memcpy(expectedsq_num, buffer+4, 4);
            int expectedsq_num = *expectedsqnum;
            if(file_length < 20){
                break;
            }

            else{
                if(expectedsq_num == sq_num){
                memcpy(file_content, buffer+8, file_length);
                memcpy(buf+packet_length, file_content, file_length);
                packet_length = packet_length + file_length;
                packet_num = packet_num + 1;
                expectedsq_num = packet_num % 2;

                bytes_sent = lossy_sendto(loss_probability,random_seed,sockfd, &expectedsq_num, sizeof(ack_buffer), 0, (struct sockaddr*)&cli_addr, slen);
                
                }
            
            
            else{
                packet_length = packet_length - file_length;
                memcpy(file_content, buffer+8, file_length);
                memcpy(buf+packet_length, file_content, file_length);
                packet_length = packet_length + file_length;
                packet_num = packet_num + 1;
                expectedsq_num = packet_num % 2;
                bytes_sent = lossy_sendto(loss_probability,random_seed,sockfd, &expectedsq_num, sizeof(ack_buffer), 0, (struct sockaddr*)&cli_addr, slen);

                }

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

