#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_FILE_SUFFIX 4
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)

typedef struct _DecodeInfo
{

     char *magic_fname;
     char *text_file;
     char *name;
    FILE *fptr_secret;
    long size_secret_file;
      char extn_secret_file[MAX_FILE_SUFFIX];
      char image_data[MAX_IMAGE_BUF_SIZE];

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} DecodeInfo;

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *);
//check the conditions 
Status do_decoding(DecodeInfo *);
//check which type of file
Status open_file(DecodeInfo *);
//fetch the magic string
Status decode_magic_string(DecodeInfo *);
//find the magic string data
Status decode_image_to_data(char *,int,FILE *fptr_stego_image);
//fetch the lsb data
Status decode_data_lsb_side(char *);
//find the exten size
Status decode_exten_size(DecodeInfo *,int *);
//fetch the value
Status decode_size(char *,int*);
//find the extension
Status decode_exten_file(DecodeInfo *,int );
//exten output file
Status decode_secret_file_extn(DecodeInfo *);
//length of secret file size
Status decode_secret_file_size(DecodeInfo *,int );
//fetch the data in image file
Status decode_secret_file_data(DecodeInfo *);

#endif
