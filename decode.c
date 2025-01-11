#include <stdio.h>
#include "types.h"
#include "decode.h"
#include <string.h>
#include <stdlib.h>
#include "common.h"

//Read and validate decode 
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
     printf("Checking arguments is started\n");
    //store name of output file if it is a .bmp file
    int len;
    len = strlen(argv[2]);
    if(strstr(argv[2],".bmp")!=NULL)
    {
        //store name of output file
        decInfo->stego_decodeimage_fname = malloc(len+1);
        strcpy(decInfo->stego_decodeimage_fname,argv[2]);
    }
    else
    {
        printf("Error : Difficulty in checking source file extension in .bmp\nDifficulty in checking file extensions\n");
        return e_failure;
    }
    //storing the name of secret file
    if(argv[3]==NULL)
    {
        strcpy(decInfo->temp,"output");
       
    }
    //if name of the secret file is given by user
    else
    {
        //if any extension is given remove the extension and store the secret file name
        if((strstr(argv[3],".txt")!=NULL) || (strstr(argv[3],".sh")!=NULL) || (strstr(argv[3],".c")!=NULL))
        {
            strcpy(decInfo->temp,strtok(argv[3],"."));
        }
        else
        {
            //store the secret file name
            strcpy(decInfo->temp,argv[3]);
        }
        printf("%s\n",decInfo->temp);
        

    }
    printf("Arguments verified successfully.\n");
    return e_success;

}

Status do_decoding(DecodeInfo *decInfo)
{
    //do decoding from the image
    //open files for decoding
    if(open_files_decode(decInfo)==e_success)
    {
        printf("Files opened successfully\n");
        //skip the header file
        if(skip_bmp_header(decInfo->fptr_decodestego_image)==e_success)
        {
            //decode magic string
            if(decode_magic_string(MAGIC_STRING, decInfo)==e_success)
            {
               printf("Magic string decoded successfully\n");
               //decoding secret file extension size
                if(decode_secret_file_extn_size(decInfo)==e_success)
                {
                    printf("Secret file extension size decoded successfully\n");
                    //decoding secret file extension
                    if(decode_secret_file_extn(decInfo)==e_success)
                    {
                        printf("Secret file extension decoded successfully\n");
                        //decode secret file size
                        if(decode_secret_file_size(decInfo)==e_success)
                        {
                            printf("Secret file size decoded successfully\n");
                            //opening secret file
                           if(decode_open_secret_file(decInfo)==e_success)
                           {
                            printf("Secret message file : %s\n",decInfo->temp);
                            printf("Secret file opened successfully\n");
                            //decode secret file data
                              if(decode_secret_file_data(decInfo)==e_success)
                              {
                                 printf("Secret file data decoded successfully\n");
                                 printf("Decoding is complete\n");
                              }
                              else
                              {
                                return e_failure;
                              }
                           }
                           else
                           {
                            return e_failure;
                           }
                        }
                        else
                        {
                            printf("ERROR : Secret file size decoding failed\n");
                            return e_failure;
                        }
                   
                    }
                    else
                    {
                        printf("ERROR : Secret file extensions decoding failed\n");
                       return e_failure;
                    }
                }
                else
                {
                    printf("ERROR : Secret file extension size decoding failed\n");
                    return e_failure;
                }
            }
        
        else
        {
            printf("ERROR : Magic string decoding failed\n");
            return e_failure;
        }
    }
    else
    {
        return e_failure;
    }
}
else
{
    return e_failure;
}
}


//open files for decoding
Status open_files_decode(DecodeInfo *decInfo)
{
    printf("Open files process started\n");
    // Stego Image file
    decInfo->fptr_decodestego_image = fopen(decInfo->stego_decodeimage_fname, "r");
    // Do Error handling
    if (decInfo->fptr_decodestego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_decodeimage_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

//skip bmp header
Status skip_bmp_header(FILE *fptr_decodestego_image)
{
   //skip 54 bytes of header
   fseek(fptr_decodestego_image, 54, SEEK_SET);
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    printf("Encoding magic string.\n");
    int i;
    char buffer[8];
    char *data;
    char *data_out = MAGIC_STRING;
    //reading the magic string bytes
    for(i=0;i<strlen(MAGIC_STRING);i++)
    {
        //read 8 bit from output image
        fread(buffer,8,1,decInfo->fptr_decodestego_image);
        data[i]=decode_byte_from_lsb(buffer);
        //printf("%c",data[i]);
    }
    printf("\n");
    //checking if the magic string is the same
    for(i=0;i<strlen(MAGIC_STRING);i++)
    {
        if(data[i]!=data_out[i])
        {
            printf("No Magic string");
            return e_failure;
        }
    }
    return e_success;
} 

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    printf("Decoding the File Extension Size\n");
    char buffer[32];
    //reading 32 bits from output image
    fread(buffer,32,1,decInfo->fptr_decodestego_image);
    //decoding the value of extension size
    decInfo->decode_extn_size = decode_int_from_lsb(buffer);
    //printf("%d\n",decInfo->decode_extn_size);
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    printf("Decoding the File Extension\n");
    int i;
    char buffer[decInfo->decode_extn_size];
    for(i=0;i<decInfo->decode_extn_size;i++)
    {
        //reading bits from output file
       fread(buffer,8,1,decInfo->fptr_decodestego_image);
       //decode the extension
       decInfo->extn_secret_file[i]=decode_byte_from_lsb(buffer);
       //printf("%c",decInfo->extn_secret_file[i]);
    }
    decInfo->extn_secret_file[i] = '\0';
    printf("\n");
    return e_success;
}
Status decode_open_secret_file(DecodeInfo *decInfo)
{
    //adding extension to file
    strcat(decInfo->temp,decInfo->extn_secret_file);
    //printf("%s\n",decInfo->temp);
    //memory allocation
    decInfo->secret_decode_fname= malloc((strlen(decInfo->temp))+1);
    decInfo->temp[(strlen(decInfo->temp))] = '\0';
    //storing secret file name in secret_decode_fname
    strcpy(decInfo->secret_decode_fname,decInfo->temp);
    //opening secret file using pointer
    decInfo->fptr_decode_secret = fopen(decInfo->secret_decode_fname, "w");
    // Do Error handling
    if (decInfo->secret_decode_fname == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_decode_fname);

    	return e_failure;
    }
    // No failure return e_success
    return e_success;

}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("Decoding the secret File data\n");
    int i;
    char buffer[8];
    char secret_data[decInfo -> decode_file_size];
    for(i=0;i<(decInfo->decode_file_size);i++)
    {
        //reading bits from output file
       fread(buffer,8,1,decInfo->fptr_decodestego_image);
       //decode the extension
       secret_data[i]=decode_byte_from_lsb(buffer);
    }
    secret_data[i] = '\0';
    //printf("%s",secret_data);
    //writing data into file
    fprintf(decInfo->fptr_decode_secret,"%s",secret_data);
    
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    printf("Decoding the secret File size\n");
    char buffer[32];
    //reading 32 bits from buffer
    fread(buffer,32,1,decInfo->fptr_decodestego_image);
    //file data size
    decInfo->decode_file_size = decode_int_from_lsb(buffer);
    //printf("%ld\n",decInfo->decode_file_size);
    return e_success;
}

//decode byte from lsb
char decode_byte_from_lsb(char* buffer)
{
    char ch;
    int get,i;
    for(i=0;i<=7;i++)
    {
        get = buffer[i] & (1);//get byte
        ch = (get << (7-i)) | ch; //storing value in character
    }
    return ch;
}

//decode int from lsb
int decode_int_from_lsb(char* buffer)
{
    int ch=0;
    int get,i;
    for(i=0;i<=31;i++)
    {
        get = buffer[i] & (1);//get bit
        ch = (get << (31-i)) | ch; //storing value in character
    }
    return ch;
}

