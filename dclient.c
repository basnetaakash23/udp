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
#define HEADER_LENGTH 8

int create_packet();

void err(char *s)
{
    perror(s);
    exit(1);
}

typedef struct packet{
    char data[MAX_LINE];
}Packet;

typedef struct frame{
    int frame_kind;
    int seq_num;
    int ack;
    Packet packet;
}Frame;

int main(int argc, char** argv)
{
    short int port;

    char *szAddress;
    char *szPort;
    char *endptr;
    char *filepath;
    char *toFormat;
    char *toName;
    char *lossProbab;
    char *randomSeed;


    char buffer[MAX_LINE];
    char header_buffer[HEADER_LENGTH];
    char ack_buffer[2];
    int recv_result;
    int offset = 0;
    struct sockaddr_in serv_addr;
    int sockfd, i, slen=sizeof(serv_addr);
    // char buf[BUFLEN];
    
    if(argc != 8)
    {
        printf("Incomplete Command Line arguments\n");
        exit(0);
        return -1;
    }
    szAddress = argv[1];
    szPort = argv[2];
    filepath = argv[3];
    toFormat = argv[4];
    toName = argv[5];
    lossProbab = argv[6];
    randomSeed = argv[7];

    float loss_probability = atoi(lossProbab);
    printf("The loss probabiltiy is %f\n", loss_probability);

    int random_seed = atoi(randomSeed);
    printf("The value of random seed is %d\n", random_seed);

    port = strtol(szPort, &endptr, 0);
    if(*endptr){
        printf("EchoCLNT: Invalid Port supplied.\n");
        exit(EXIT_FAILURE);
    }
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        err("socket");
    
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
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


    uint8_t filename_length = strlen(toName);

    memcpy(header_buffer, toFormat, 1);                         //copying format information
    memcpy(header_buffer+1, &filename_length, 1);               //copying file name filename_length
    memcpy(header_buffer+2, toName, filename_length);           //copying toname value
    memcpy(header_buffer+2+filename_length, &file_length, 4);   //copying file length

    while(1){
        sendto(sockfd, header_buffer, HEADER_LENGTH ,0, (struct sockaddr*)&serv_addr, slen)
        char ack[2];
        int received_result = recvfrom(sockfd, ack, sizeof(ack_buffer), 0, (struct sockaddr*)&serv_addr, &slen);
        if(received_result>0){
            break;
        }

        else{
            continue;
        }

    }
    

    // memcpy(buffer, &file_length, 4);
    // printf("It works up to here\n");
    // memcpy(buffer+4, file_buffer, num_of_items);

    // printf("It works up to here\n");
    
    // if(sendto(sockfd, buffer,MAX_LINE,0, (struct sockaddr*)&serv_addr, slen)==-1){
    //     err("sendto()");
    // }
    int x = 0;
    int packet_num = 0;
    int size_of_frame = 0;

                    //
                                        
    while(x < num_of_items){
        printf("Where is the error inside else statement?\n");


        size_of_frame = create_packet(x, num_of_items, packet_num, fp);
        x = x + size_of_frame;
        packet_num++;
        printf("Did we reach here??\n");
        
    }

    printf("May be it receives error in the if statement\n");
    
    close(sockfd);
    return 0;
}

int create_packet(int current_index, int total_items, int packet_num, FILE* fp){
    int x = current_index;
    int num_of_items = total_items;
    int frame_size = 20;                //20 bytes
    char temp_buffer[frame_size];       //frame size
    char temp_file[32];                 
    int sq_num = 0;                    
     
    
    while(1){
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
                memcpy(temp_file+4, &sq_num, sizeof(int));
                memcpy(temp_file+8, temp_buffer, frame_size);

                if(sendto(sockfd, temp_file, 32 ,0, (struct sockaddr*)&serv_addr, slen)==-1){
                    err("sendto()");
                }
                recv_result = recvfrom(sockfd, ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr*)&serv_addr, &slen);
                if(recv_result > 0){
                    printf("Ack received\n");
                    break;
                }     
        }

        else{
                sq_num = packet_num % 2;
                printf("The value of x is %d\n", x);
                fseek(fp, x, SEEK_SET);
                fread(temp_buffer, 1, frame_size, fp); 
                printf("Where is the seg fault?\n");
                memcpy(temp_file, &frame_size, 4);
                // memcpy(temp_file+4, &seq_num, 4);
                printf("Did we reach here?\n");
                memcpy(temp_file+4, temp_buffer, frame_size);
                if(sendto(sockfd, temp_file, 32 ,0, (struct sockaddr*)&serv_addr, slen)==-1){
                    err("sendto()");
                }
                printf("Did we make progress from here?\n");
                recv_result = recvfrom(sockfd, ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr*)&serv_addr, &slen);
                printf("About acknowledgement %d\n\n", recv_result);
                if(recv_result > 0){
                    printf("Ack received\n");
                    break;
                }    
        }

    }
    return frame_size;
}

