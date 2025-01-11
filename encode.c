#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
OperationType check_operation_type(char *argv[])
{
    //compare the two strings
    if(strcmp(argv[1],"-e")==0) //if "-e" return e_encode
    {
       return e_encode; 
    }
    else if(strcmp(argv[1],"-d")==0) //if "-d" return e_decode
    {
       return e_decode; 
    }
    else
    {
        return e_unsupported; //if any other value return e_unsupported
    }
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
     printf("Checking arguments is started\n");
    int len;
    //source file
    len = strlen(argv[2]);
    //checking if the source file is .bmp file
    if(strstr(argv[2],".bmp")!=NULL)
    {
        encInfo->src_image_fname = malloc(len+1);
        strcpy(encInfo->src_image_fname,argv[2]);
    }
    else
    {
        printf("Error : Difficulty in checking source file extension in .bmp\nDifficulty in checking file extensions\n");
        return e_failure;
    }
    
    //secret file
    len = strlen(argv[3]);
    //checking if the secret file is .txt,.sh or .c
    char *ptr;

    if(strstr(argv[3],".txt")!=NULL)
    {
        ptr=strstr(argv[3],".txt");
    }
    else if(strstr(argv[3],".c")!=NULL)
    {
        ptr=strstr(argv[3],".c");
    }
    else if(strstr(argv[3],".sh")!=NULL)
    {
        ptr=strstr(argv[3],".sh");
    }
    else
    {
        printf("ERROR : Difficulty in checking secret message file extension in .txt/.c/.sh.\nDifficulty in checking file extensions\n");
        return e_failure;
    }



    if(strcmp(ptr,".txt")==0)
    {
        strcpy(encInfo -> extn_secret_file,ptr);

        encInfo -> secret_fname =malloc(strlen(argv[3])+1);
        strcpy( encInfo -> secret_fname,argv[3]);

        
    }
    else if (strcmp(ptr,".c")==0)
    {
        strcpy(encInfo -> extn_secret_file,strstr(argv[3],".c"));

        encInfo -> secret_fname =malloc(strlen(argv[3])+1);
        strcpy( encInfo -> secret_fname,argv[3]);

    }
    else if(strcmp(ptr,".sh")==0)
    {
        strcpy(encInfo -> extn_secret_file,strstr(argv[3],".sh"));

        encInfo -> secret_fname =malloc(strlen(argv[3])+1);
        strcpy( encInfo -> secret_fname,argv[3]);
    } 
     
    else
    {
        printf("ERROR : Difficulty in checking secret message file extension in .txt/.c/.sh.\nDifficulty in checking file extensions\n");
        return e_failure;
    }
    
    //output file
    len = strlen("stego.bmp");
    //checking if the output file is .bmp file
    if(argv[4]==NULL)
    {
        //if name of output file is not given
        encInfo->stego_image_fname = malloc(len+1);
        strcpy(encInfo->stego_image_fname,"stego.bmp");
        
    }
    else if(strstr(argv[4],".bmp")!=NULL)
    {
        //if name of output file is given
        encInfo->stego_image_fname = malloc((strlen(argv[4]))+1);
        strcpy(encInfo->stego_image_fname,argv[4]);
    }
    else
    {
        printf("ERROR : Provide output file with .bmp extension.\nDifficulty in checking file extensions\n");
        return e_failure;
    }
    printf("Arguments verified successfully.\n");
     return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    //open files
    if(open_files(encInfo)==e_success)
    {
        printf("Files opened successfully\n");
        printf("Encoding file : %s\n",encInfo->stego_image_fname);
        //check capacity
        if(check_capacity(encInfo)==e_success)
        {
            printf("Capacity of source file is more than the secret file\n");
            //copy header file
            if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
            {
                printf("Successfully header is copied\n");
                //encode magic string
                if(encode_magic_string(MAGIC_STRING, encInfo)==e_success)
                {
                    printf("Magic String decoded Successfully.\n");
                    //encode secret file extension size
                    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo)==e_success)
                    {
                        printf("File Extension Size encoded successfully\n");
                     //encode secret file extension
                      if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
                      {
                         printf("File extension encoded successfully\n");
                          //encode secret file size
                          if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
                          {
                            printf("File size encoded successfully\n");
                            //encode secret file data
                            if(encode_secret_file_data(encInfo)==e_success)
                            {
                                //copy remaining data
                                if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
                                {
                                   printf("Encoding is completed\n");
                                   printf("Remaining data Encoded successfully\n");
                                }
                                else
                                {
                                    printf("ERROR : Failed to copy remaining data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Secret Data Encoded successfully\n");
                                return e_failure;
                            }

                          }
                          else
                          {
                            printf("ERROR : File size encoding failed\n" );
                            return e_failure;
                          }
                      }
                      else
                      {
                        printf("ERROR : Encoding file extension failed\n");
                        return e_failure;
                      }   
                    }
                    else
                    {
                        printf("ERROR : File extension size encoding failed\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("ERROR : Magic string cannot be encoded\n");
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR : Header copying failed\n");
                return e_failure;
            }
        }
        else
        {
            printf("ERROR : Capacity of source file is less than the secret file\n");
            return e_failure;
        }
    }
    return e_failure;
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

Status check_capacity(EncodeInfo *encInfo)
{
    printf("Checking capacity to encode message\n");
    //get image capacity
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    //get secret file size
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    
    
    //checking for image capacity
    if(encInfo->image_capacity > (((2 + 4 + 4 + 4 + encInfo->size_secret_file)*8)+54))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

uint get_file_size(FILE *fptr_secret)
{
    //pointing the pointer till end of file
    fseek(fptr_secret,0,SEEK_END);
    return ftell(fptr_secret);
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("Open files process started\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    // No failure return e_success
    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    printf("Header is copying\n");
    char buffer[54];
    //Go to the beginning of the file
    rewind(fptr_src_image);
    //read 54 bytes from header file of source image
    fread(buffer,54,1,fptr_src_image);
    //copy the 54 bytes header file from source file to output file
    fwrite(buffer,54,1,fptr_stego_image);
    //checking if the header file is copied or not
    if(ftell(fptr_stego_image)==54 && ftell(fptr_src_image)==54)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }

}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int i,get;
    for(i=7;i>=0;i--)
    {
         get = (data & (1 << i)) >> i; //get byte
         image_buffer[7-i] = image_buffer[7-i] & (~1); //clear lsb byte
         image_buffer[7-i] = image_buffer[7-i] | get; //set byte
    }
    return e_success;
}

Status encode_int_to_lsb(int data, char *image_buffer)
{
    int i,get=0;
    for(i=31;i>=0;i--)
    {
         get = (data & (1 << i)) >> i; //get bit
         image_buffer[31-i] = image_buffer[31-i] & (~1); //clear lsb bit
         image_buffer[31-i] = image_buffer[31-i] | get; //set bit
    }
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("Encoding magic string.\n");
    //encode magic string to image
    if(encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
//encode data to image
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    int i;
    for(i=0;i<size;i++)
    {
        char buffer[8];
        //Read 8 bytes from source image
        fread(buffer, 8, 1, fptr_src_image);
        if(encode_byte_to_lsb(data[i],buffer)==e_failure)
        {
            return e_failure;
        }
        //write 8 bytes to output image
        fwrite(buffer, 8, 1, fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_extn_size(long extn_size, EncodeInfo *encInfo)
{
    printf("Encoding the File Extension Size\n");
    char secret_file[32];
       //read 32 bytes from the source file
       fread(secret_file, 32, 1, encInfo->fptr_src_image);
       //encode the extension size to lsb bit
       if(encode_int_to_lsb(extn_size,secret_file)==e_failure)
       {
          return e_failure;
       }
       //write 32 bytes into the output image
       
       fwrite(secret_file, 32, 1, encInfo->fptr_stego_image);
    
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("Encoding the file extension\n");
    //call encode secret file extension to image 
    if(encode_data_to_image(encInfo->extn_secret_file,strlen(encInfo->extn_secret_file),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }
    else
    {
        return e_success;
    }
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("Encoding secret file size(data size)\n");
    char buffer[32];
    //read 32 bytes to source data
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    if(encode_int_to_lsb(file_size,buffer)==e_failure)
    {
        return e_failure;
    }
    //write 32 bytes to output data 
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("Encoding secret data.\n");
    rewind(encInfo->fptr_secret);
    char secret_data[encInfo->size_secret_file];
    //read 32 bytes data
    fread(secret_data,encInfo->size_secret_file,1,encInfo->fptr_secret);
    if(encode_data_to_image(secret_data,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }
    e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("Encoding the remaining Data\n");
    char ch;
    //read 1 byte from source image
    while(fread(&ch,sizeof(ch),1,fptr_src)>0)
    {
        //write 1 byte from output image
        fwrite(&ch,sizeof(ch),1,fptr_dest);
    }
    return e_success;
}