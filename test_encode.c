/*Documentation
Name : Shruthi Shivkumar
Date : 21-11-2024
DES  : Steganography
*/

#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc,char *argv[])
{
    if(argc >= 3)
    {
        //check if operation to be done is encoding
        if(check_operation_type(argv) == e_encode)
        {
            if(argc < 4)
            {
                printf("Error : Unable To perform Operation.Arguments Should be >3.\n");
                printf("For Encoding Arguments should be:\na.out -e <sourcefile.bmp> <secretfile.txt> <outputfile.bmp>(outputfile is optional)\nFor decoding Arguments should be:\na.out -d <sourcefile.bmp> <outputfile.bmp>\n");
                return 0;
            }

            printf("Encoding started.\n");
            //declaring a variable in structure EncodeInfo
            EncodeInfo encInfo;
            //checking for all the validations for the files
            if(read_and_validate_encode_args(argv,&encInfo) == e_success)
            {
                do_encoding(&encInfo);
            }
        }
        //check if operation to be done is decoding
        else if(check_operation_type(argv) == e_decode)
        {
            printf("Decoding started\n");
            //declaring a variable in structure DecodeInfo
            DecodeInfo decInfo;
            //checking for all the validations for the files
            if(read_and_validate_decode_args(argv,&decInfo) == e_success)
            {
                do_decoding(&decInfo);
            }

        }
        else
        {
            printf("ERROR : Unable to proceed operation provide valid type(-e or -d)\n");
        }
    }
    else if(argc == 1)
    {
        printf("Please pass the proper arguments\n");
    }
    else if(argc < 3)
    {
        printf("Error : Unable To perform Operation.Arguments Should be >3.\n");
        printf("For Encoding Arguments should be:\na.out -e <sourcefile.bmp> <secretfile.txt> <outputfile.bmp>(outputfile is optional)\nFor decoding Arguments should be:\na.out -d <sourcefile.bmp> <outputfile.bmp>\n");
    }
    return 0;
}
