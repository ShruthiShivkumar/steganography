#ifndef DECODE_H
#define DeCODE_H

#include "types.h"
#include <stdio.h>

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{

    /* Secret File Info */
    char *secret_decode_fname;//to store the secret file name
    char temp[100];//store name of secret file temporarily
    FILE *fptr_decode_secret;//to access the secret file
    uint decode_extn_size;//to store size of file extension
    char extn_secret_file[MAX_FILE_SUFFIX+1];//to store the secret file extension name
    long decode_file_size;//to store size of file data
    

    /* Stego Image Info */
    char *stego_decodeimage_fname;//to store the output filename
    FILE *fptr_decodestego_image;//to access the file

} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* skip bmp image header */
Status skip_bmp_header(FILE *fptr_decodestego_image);

/* decode Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extenstion size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
char decode_byte_from_lsb(char *image_buffer);

/*Decode integer from LSB of image data array */
int decode_int_from_lsb(char *image_buffer);

/*Open secret file*/
Status decode_open_secret_file(DecodeInfo *decInfo);

#endif