/*

  HELPER.C
  ========
  (c) Paul Griffiths, 1999
  Email: mail@paulgriffiths.net

  Implementation of sockets helper functions.

  Many of these functions are adapted from, inspired by, or 
  otherwise shamelessly plagiarised from "Unix Network 
  Programming", W Richard Stevens (Prentice Hall).

*/

//
//  serv_helper.c
//
//
//  Created by AakashBasnet on 3/10/18.
//
#include "stdint.h"
#include "inttypes.h"
#include "helper.h"
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




void process_file(char* buffer, int length_of_file, char* target_file, char* convert_option){
    printf("We are inside the process file\n");
    int i = 0;
    char * format_type = convert_option;
    printf("%c\n",format_type[0]);

    for(int i = 0; i< length_of_file; i++){
        printf("%d. The character member is %d\n",i+1, buffer[i]);
    }


    FILE* fileptr;
    char* filename = target_file;

    //printf("%s\n", filename);
    fileptr = fopen(target_file,"wb");
    //fseek(fileptr, 0, SEEK_SET);
    printf("File pointer is created and file is ready to be written\n");


    
    //fseek(fileptr, 0, SEEK_SET);
    
    while(i < length_of_file){
        while(i < length_of_file){
            //        printf("The value in buffer is %d\n",buffer[i]);
            //        printf("The value in buffer is %u\n",buffer[i]);
            if(buffer[i] == 0){
                //printf("Buffer i is %02x and index is %u\n",buffer[i], i);
                //printf("The index here in type 1 is %d\n", i);
                i = typezero_input(i, buffer,fileptr,format_type);
                //printf("The index returned here in type 1 is %d\n",i);
                
            }
            else if(buffer[i] == 1){
                //printf("The index here in type 2 is %d\n",i);
                i = typefirst_input(i, buffer, fileptr,format_type);
                //printf("The index returned here in type 2 is %d\n",i);
            }

            else{
                printf("Format Error\n");
                exit(0);
            }
        }
        printf("Succesfully converted and written into the file\n");
        //j = typezero_input(31,buffer, ptr1);
        
    }

    fclose(fileptr);
}
    
    
int typezero_input(int pos, char* binary_buffer, FILE* pointer, char* convert_option){
    char* format_type = convert_option;
        
        printf("The index before entering type 0 is %d\n", pos);
        int size_of_unit = 2;
        uint8_t amount = binary_buffer[pos+1];          
        uint16_t first_input[amount];
        //printf("The number of amount is %d\n",binary_buffer[pos+1]);
        int count = pos+2;
        
        int i = 0;
        
        //reversing the byte order because of endianness
        while(i < amount){
            first_input[i] = binary_buffer[count]<<8 | binary_buffer[count+1] ;
            //printf("%d   ...\n", first_input[i]);
            count = count + 2;
            i = i + 1;
            
        }
        
        //change the format based on convert options
        
        if(format_type[0] == '1' || format_type[0] == '3'){  //conversion from the command line argument
            convert_to_typeOne(pointer, amount, first_input);
        }
        else{                   //No conversion
            uint8_t type = 0;
            //printf("%d   type\n",type);
            //printf("%d    amount\n",amount);
            fwrite(&type, sizeof(uint8_t), 1,pointer);
            fwrite(&amount, sizeof(uint8_t), 1,pointer);
            for(int i =0; i<amount; i++){
                fwrite(&first_input[i], sizeof(short),1, pointer);
            }
        }
        printf("The index before returning from type 0 is %d\n", count);
        return count;
    }
    
void convert_to_typeOne(FILE* pointer,  uint8_t amount, uint16_t numbers[]){
        uint8_t type = 1;
        uint8_t comma = 44;
        //printf("I am inside the conversion function\n");
        fwrite(&type, sizeof(uint8_t), 1, pointer);
        //printf("I was just about to convert it\n");
        //amount is three bytes in type 1
        char amount_array[4];
        amount_array[0] = 48;
        amount_array[1] = 48;
        amount_array[2] = amount;
        amount_array[3] = '\0' ; //null character
        fprintf(pointer, "%s", amount_array);
        int i = 0;
        while(i<amount){
            //printf("Trying to write files\n");
            fprintf(pointer,"%d",numbers[i]);
            //printf("Not sure if it was succesful\n");
            if(i<=amount-2){   //if condition to insert comma
                fwrite(&comma, sizeof(uint8_t),1,pointer);
                
            }
            i++;
            
        }
        
        //printf("I think I converted it\n");
    }
    
    
    
int typefirst_input(int pos, char* binary_buffer, FILE *pointer, char* convert_option){
        printf("The index before entering type 1 is %d\n", pos);
        char * format_type = convert_option;
        FILE* ptr = pointer;
        int i = pos;
        uint8_t amount = (binary_buffer[i+1]-48)*100 + (binary_buffer[i+2]-48)*10 + binary_buffer[i+3]-48;
        char amount_[4];                         //printing three bytes of amount

        amount_[0] = binary_buffer[pos+1];
        amount_[1] = binary_buffer[pos+2];
        amount_[2] = binary_buffer[pos+3];
        amount_[3] = '\0';
        //printf("The amount in type 1 is %d\n",amount);
        uint16_t second_input[amount];    //this is where we store asccii values so that they can be written later on on file
        
        i = i + 4;
        int count = 1;
        //for counting the numbers of unit of the type 1 and we assume that there is at least one such unit until we find either 0 or a comma.
        int j  = 0; //working around with concatenating the bytes of ascii values
        int value = 0;      //this is the variable where we temporarily store the asccii value
        int num_of_units = 0;
        int comma = 44;
        int type0 = 0;
        int type1 = 1;     //assigning value to type 1
        int ind = 0;      //this for storing values of numbers in character array
        
        while(num_of_units<amount){
            if(binary_buffer[i] == comma){
                num_of_units = num_of_units + 1;
                //printf("The ascii value is %d\n",value);
                second_input[ind] = value;
                ind = ind + 1;
                i = i+1;
                j=0;
            }
            else if(binary_buffer[i] == type0 | binary_buffer[i] == type1){
                second_input[ind] = value;
                //printf("The ascii value is %d\n",value);
                
                //printf("The index before returning to main from type 1 is %d\n", i);
                if(format_type[0] == '3' || format_type[0] == '2'){
                    //                convert_to_typeZero(ptr, amount, second_input);
                    //printf("Converting \n");
                    uint8_t type = 0;
                    fwrite(&type, sizeof(uint8_t), 1, pointer);
                    fwrite(&amount, sizeof(uint8_t),1, pointer);
                    for(int i =0; i<amount; i++){
                        printf("%d............type1\n",second_input[i]);
                        fwrite(&second_input[i], sizeof(uint16_t),1, pointer);
                    }
                }
                
                else{                                        //writing to the file in ascii form
                    uint8_t type = 1;
                    fwrite(&type, sizeof(uint8_t),1, pointer);
                    fprintf(pointer, "%s", amount_);
                    
                    for(int i = 0; i<amount; i++){
                        fprintf(pointer,"%d",second_input[i]);
                    }
                }
                
                return i;
            }
            else{
                if(j>0){
                    value = value*10 + (binary_buffer[i]-48);
                    //printf("just the value is for the so on and so forth %ld\n", value);
                    i = i + 1;
                    j = j + 1;
                }
                else{
                    value = binary_buffer[i]-48;
                    //printf("just the value is for the first time is  %ld\n", value);
                    j = j+1;
                    i = i+1;
                }
            }
        }
}
    
void convert_to_typeZero(FILE* pointer, uint8_t amount, short numbers[]){
    
            //printf("Converting \n");
    uint8_t type = 0;
    fwrite(&type, sizeof(uint8_t), 1, pointer);
    fwrite(&amount, sizeof(uint8_t),1, pointer);
    for(int i =0; i<amount; i++){
                //printf("%d............type0\n",numbers[i]);
    fwrite(&numbers[i], sizeof(short),1, pointer);
    }
    
    
        //printf("Converted\n");
        
 }








