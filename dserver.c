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
#include "helper.c"
#include <time.h>
#define BUFLEN 1000
#define PORT 9930
#define LENGTH 32
#define HEADER_LENGTH 20

void err(char *s)
{
    perror(s);
    exit(1);
}

int main(int argc, char** argv)
{   
    char *szPort;
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
    char* target_file;
    int file_Length;

    char fileLength[4]; 
    char format[2]; 
    char fileNameLength[4];

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
    

    printf("Preparing to receive the header file\n");
    while(1){
        printf("...........\n");
        recv_result = recvfrom(sockfd, header_buffer, HEADER_LENGTH, 0, (struct sockaddr*)&cli_addr, &slen);
        printf("Bytes received %d\n", recv_result);
        if(recv_result>0){
            
            format[0] = header_buffer[0];
            format[1] = '\0';
            printf("%s is the format\n", format);

            char fileNameLength[4];
            memcpy(fileNameLength, header_buffer+1, 4);
            int file_name_length = *fileNameLength; //file name length has been copied
            target_file = (char *) malloc(file_name_length+1);


            memcpy(target_file, header_buffer+5, file_name_length); //copies into the memory
            target_file[file_name_length] = '\0';    //target file will be stored as string


                                //stores the file length for now 
            printf("%s is the target file\n", target_file);
            memcpy(fileLength, header_buffer+5+file_name_length, 4);
            file_Length = *fileLength;

            printf("%d ..is the file length\n", file_Length);

            char ack[2] ;
            ack[0] = '1';
            ack[1] = '\0';


            int sent_ack = lossy_sendto(loss_probability,random_seed,sockfd, ack, sizeof(ack), (struct sockaddr*)&cli_addr, slen);



            break;
  
        }    
    }

    
    
    /*Getting the format type*/

    printf("Preparing to send the files..................................................\n");
    
    int packet_length = 0;
    char filelength[4];
    char sqnum[4];
    int bytes_sent;

    int packet_num = 0;
    int expectedsq_num = 0;
    

    //as long as we do not read all the packets
    while(packet_length<file_Length){
        printf("...........\n");
        recv_result = recvfrom(sockfd, buffer, LENGTH, 0, (struct sockaddr*)&cli_addr, &slen);
        printf("Bytes received %d\n", recv_result);
        printf("Continue\n");
        if(recv_result>0){
            
            
            memcpy(filelength, buffer, 4);
            int file_length = *filelength;
            char file_content[file_length];
            printf("%d. is the file length\n", file_length);
            
            
            memcpy(sqnum, buffer+4, 4);
            int sq_num = *sqnum;
            

            
                if(expectedsq_num == sq_num){
                    printf("Did we reach here?\n");
                    memcpy(file_content, buffer+8, file_length);
                    memcpy(buf+packet_length, file_content, file_length);
                    packet_length = packet_length + file_length;
                    printf("1. The packet length is %d\n", packet_length);
                    packet_num = packet_num + 1;
                    expectedsq_num = packet_num % 2;

                    bytes_sent = lossy_sendto(loss_probability,random_seed,sockfd, &expectedsq_num, sizeof(ack_buffer), (struct sockaddr*)&cli_addr, slen);
                    
                }
            
                //Handling duplicate packets
                else{
                    packet_length = packet_length - file_length;
                    packet_num = packet_num - 1;
                    memcpy(file_content, buffer+8, file_length);
                    memcpy(buf+packet_length, file_content, file_length);
                    packet_length = packet_length + file_length;
                    printf("2. The packet length is %d\n", packet_length);
                    packet_num = packet_num + 1;
                    expectedsq_num = packet_num % 2;
                    bytes_sent = lossy_sendto(loss_probability,random_seed,sockfd, &expectedsq_num, sizeof(ack_buffer),(struct sockaddr*)&cli_addr, slen);

                }

            
        }    
    }

    for(int i = 0; i < packet_length; i++){
           printf("%d. The character member is %d\n",i+1, buf[i]);
    }
    
    
    //call process file 
    process_file(buf, packet_length, target_file, format);
    printf("Process file called\n");
    
    close(sockfd);
    return 0;
}

