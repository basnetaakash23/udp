//
//  dclient.c
//  
//
//  Created by AakashBasnet on 4/17/18.
//

//UDPClient.c

/*
 * gcc -o client UDPClient.c
 * ./client <server-ip>
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
//#define BUFLEN 512
#define MAX_LINE     (1000)
#define PORT 9930

void err(char *s)
{
    perror(s);
    exit(1);
}

int main(int argc, char** argv)
{
    char buffer[MAX_LINE];
    int offset = 0;
    struct sockaddr_in serv_addr;
    int sockfd, i, slen=sizeof(serv_addr);
    // char buf[BUFLEN];
    
    if(argc != 2)
    {
        printf("Usage : %s <Server-IP>\n",argv[0]);
        exit(0);
    }
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        err("socket");
    
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_aton(argv[1], &serv_addr.sin_addr)==0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }


    
//    while(1)
//    {
//        printf("\nEnter data to send(Type exit and press enter to exit) : ");
//        scanf("%[^\n]",buf);
//        getchar();
//        if(strcmp(buf,"exit") == 0)
//            exit(0);
//
//        if (sendto(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&serv_addr, slen)==-1)
//            err("sendto()");
//    }
    
    FILE* fp = fopen("practice_project_test_file_1","rb");
    fseek(fp, 0, SEEK_END);

    int file_length = ftell(fp);
    rewind(fp);
    char file_buffer[file_length];
    int num_of_items = fread(file_buffer, 1, file_length, fp);

    printf("%d. is the file length\n", num_of_items);

    // memcpy(buffer, &file_length, 4);
    // printf("It works up to here\n");
    // memcpy(buffer+4, file_buffer, num_of_items);

    // printf("It works up to here\n");
    
    // if(sendto(sockfd, buffer,MAX_LINE,0, (struct sockaddr*)&serv_addr, slen)==-1){
    //     err("sendto()");
    // }
    int x = 0;

    int frame_size = 20;
    char temp_buffer[frame_size];

    char temp_file[32];
    int sq_num = 0;
    int packet_num = 1;

    while(x < num_of_items){
        printf("Where is the error inside else statement?\n");


        
        if(num_of_items -x < frame_size){
            printf("%d last packet\n", num_of_items-x);
            
            printf("Where is the error inside else statement?");
            printf("Where is the error?\n");
            frame_size = num_of_items - x;
            sq_num = packet_num % 2;
            fseek(fp, x, SEEK_SET);
            fread(temp_buffer, 1, frame_size, fp);
            memcpy(temp_file, &frame_size, 4);
            // memcpy(temp_file+4, &seq_num, 4);
            memcpy(temp_file+4, temp_buffer, frame_size);
            if(sendto(sockfd, temp_file, 32 ,0, (struct sockaddr*)&serv_addr, slen)==-1){
                err("sendto()");
            }
            x = x + frame_size;
            packet_num++;
        }

        else{
            sq_num = packet_num % 2;
            printf("The value of x is %d\n", x);
            fseek(fp, x, SEEK_SET);
            fread(temp_buffer, 1, frame_size, fp);
            printf("Where is the seg fault?\n");
            memcpy(temp_file, &frame_size, 4);
            // memcpy(temp_file+4, &seq_num, 4);
            memcpy(temp_file+4, temp_buffer, frame_size);
            if(sendto(sockfd, temp_file, 32 ,0, (struct sockaddr*)&serv_addr, slen)==-1){
                err("sendto()");
            }
            x = x + frame_size;
            packet_num++;

        }
        

        // else{
        //     printf("Where is the error inside else statement?");
        //     sq_num = packet_num % 2;
        //     fread(temp_buffer, 1, frame_size, fp+x);
        //     memcpy(temp_file, &frame_size, 4);
        //     // memcpy(temp_file+4, &seq_num, 4);
        //     memcpy(temp_file+4, temp_buffer, frame_size);
        //     if(sendto(sockfd, temp_file, 32 ,0, (struct sockaddr*)&serv_addr, slen)==-1){
        //         err("sendto()");
        //     }
        //     x = x + frame_size;
        //     packet_num++;
        
        // }
    }

    printf("May be it receives error in the if statement\n");
    
    close(sockfd);
    return 0;
}

