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
#include <time.h>
#include "helper.h"

#include "sendlib.c"
//#define BUFLEN 512
#define MAX_LINE     (1000)
#define PORT 9930
#define HEADER_LENGTH 20
#define PACKET_LENGTH 32
#define FRAME_SIZE 20

char create_packet(int, int, int, FILE*, char*);

volatile int stop = 0;

void sigalrm_handler(int sig){
    stop = 1;
}

void err(char *s)
{
    perror(s);
    exit(1);
}



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
    int recv_result = 0;
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

    float loss_probability = atof(lossProbab);
    //float loss_probability = 0.1;
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

    /* Handling time */
    struct sigaction sact;
    sigemptyset (&sact.sa_mask);
    sact.sa_flags = 0;
    sact.sa_handler = sigalrm_handler;
    sigaction(SIGALRM, &sact, NULL);

    
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
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 300000;
    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv))<0){
        perror("Error\n");
    }


    
    FILE* fp = fopen(filepath,"rb");
    fseek(fp, 0, SEEK_END);

    int file_length = ftell(fp);
    rewind(fp);
    char file_buffer[file_length];
    int num_of_items = fread(file_buffer, 1, file_length, fp);
    printf("The number of read items is %d\n", num_of_items);

    printf("%d. is the file length\n", file_length);


    int filename_length = strlen(toName);

    memcpy(header_buffer, toFormat, sizeof(char));                         //copying format information
    memcpy(header_buffer+1, &filename_length, sizeof(int));               //copying file name filename_length
    memcpy(header_buffer+1+sizeof(int), toName, filename_length);           //copying toname value
    memcpy(header_buffer+1+sizeof(int)+filename_length, &file_length, sizeof(int));   //copying file length

    /*Trying to send command line arguments as header information to server*/
    while(1){
        lossy_sendto(loss_probability,random_seed,sockfd, header_buffer, 20 , (struct sockaddr*)&serv_addr, slen);
        char ack[2];
        int received_result = recvfrom(sockfd, ack, sizeof(ack_buffer), 0, (struct sockaddr*)&serv_addr, &slen);
        if(received_result>0){
            break;
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
    
    int requested_sequencenum; 
    int frame_size; 
    /*Trying to send the packet */                                   
    while(x < num_of_items){
        
        char temp_file[PACKET_LENGTH];
        int pack = create_packet(x, num_of_items, packet_num, fp, temp_file );


        while(1){
            //printf("The print statement inside while statement?\n");
           
            //memcpy(packet, create_packet(x, num_of_items, packet_num, fp),32);
            printf("The size of packet is %d\n", pack);
            //printf("Was string copy succesful?\n");

            printf("Sending files now with packet number %d\n", packet_num);
            if(lossy_sendto(loss_probability,random_seed,sockfd, temp_file, 32 ,(struct sockaddr*)&serv_addr, slen)==-1){ //sends the file with the loss probabilty and random seed to generate random number
                err("sendto()");
            }
            printf("File sent now waiting to receive acknowledgement\n");

            recv_result = recvfrom(sockfd, &requested_sequencenum, sizeof(requested_sequencenum),0, (struct sockaddr*)&serv_addr, &slen);   //recevice result from the server
            printf("File received %d bytes\n", recv_result);
            if(recv_result>0){
                packet_num++;
                break;
                    
            }
            

            
            //time_t startTime = time(NULL);
            // alarm(0.05);
            // //printf("The time now is %f\n", startTime);
            // //while(time(NULL) - startTime < 0.05){
            // while(!stop){
            //     printf("Sending files now with packet number %d\n", packet_num);
            //     if(lossy_sendto(loss_probability,random_seed,sockfd, temp_file, 32 ,(struct sockaddr*)&serv_addr, slen)==-1){ //sends the file with the loss probabilty and random seed to generate random number
            //         err("sendto()");
            //     }
            //     printf("File sent now waiting to receive acknowledgement\n");

            //     recv_result = recvfrom(sockfd, &requested_sequencenum, sizeof(requested_sequencenum),0, (struct sockaddr*)&serv_addr, &slen);   //recevice result from the server
            //     printf("File received %d bytes\n", recv_result);
            //     if(recv_result>0){
            //         packet_num++;
            //         break;
                    
            //     }
            // }
            // printf("Did we reach out of the loop?\n");
            // if(requested_sequencenum == packet_num%2){
            //     break;
            // }
        }

        x = x + pack;
        printf("The new value of x is %d\n\n\n", x);
          
        
           
    }

    //printf("May be it receives error in the if statement\n");
    
    close(sockfd);
    return 0;
}

/* Creating packets in array and returns the packet size of the packet sent*/
char create_packet(int current_index, int total_items, int packet_num, FILE* fp, char* temp_file){
    int x = current_index;
    int num_of_items = total_items;
                    //20 bytes
    int frame_size = 20;
    char temp_buffer[FRAME_SIZE];       //frame size
                     
    int sq_num = 0; 
                     
     
    /* Returns the frame size only upon the succesful receipt of acknowledgement*/
    
        if(num_of_items -x < frame_size){
                printf("%d last packet\n", num_of_items-x);
                
                //printf("Where is the error inside else statement?");
                //printf("Where is the error?\n");
                frame_size = num_of_items - x;       //last frame size
                sq_num = packet_num % 2;

                fseek(fp, x, SEEK_SET);              //seeking the pointer of the file to specific position so we can read bytes from that position onwards
                fread(temp_buffer, 1, frame_size, fp);

                memcpy(temp_file, &frame_size, 4);
                memcpy(temp_file+4, &sq_num, 4);
                
                memcpy(temp_file+8, temp_buffer, frame_size);
                printf("Succesfully returned from create packet\n");

                return frame_size;

                 
        }

        else{
                sq_num = packet_num % 2;
                //printf("The value of x is %d\n", x);
                fseek(fp, x, SEEK_SET);
                fread(temp_buffer, 1, frame_size, fp); 
                //printf("Where is the seg fault?\n");
                memcpy(temp_file, &frame_size, 4);
                // memcpy(temp_file+4, &seq_num, 4);
                //printf("Did we reach here?\n");
                memcpy(temp_file+4, &sq_num, sizeof(int));
                memcpy(temp_file+8, temp_buffer, frame_size);
                printf("Succesfully returned from create packet\n");
                return frame_size;
                 
        }

}
    


