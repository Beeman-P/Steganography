#include <stdio.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"
#include <stdlib.h>
 
Status open_file(DecodeInfo *decInfo)
{
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");                                          //check the image file is empty or not

	if(decInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo->stego_image_fname);
			return d_failure;
	}
	return d_success;
}
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
     if(strstr(argv[2],".bmp") && argv[2]!=NULL)                                                                    //if the 2nd positon file is bmp 
     {
	     decInfo->stego_image_fname = argv[2];                                                                  //stored struct pointer
     }
     else
     {
	     printf("2.1--Please pass .bmp file \n");
	     return d_failure;
     }
     if(argv[3]!=NULL)
      {
	      decInfo->name=malloc(20);                                                                             //allocate a memory for output file
	     decInfo->name=argv[3];
      }   
     else 
     {
	     decInfo->name=malloc(20);
	  strcpy(decInfo->name,"output");
     }
     return d_success;
}
Status decode_magic_string(DecodeInfo *decInfo)
{
	fseek(decInfo->fptr_stego_image,54,SEEK_SET);                                                              //go to last byte of header file
     int size=strlen(MAGIC_STRING);
   unsigned char ch[size+1];
   if(decode_image_to_data(ch,size,decInfo->fptr_stego_image)==d_success);                                         //check the magic string is equal or not 
   {
    ch[size]='\0';
    printf("/INFO : Magic string is %s  /\n",ch);
    if((strcmp(ch,MAGIC_STRING))==0)                                                                        //if magicstring decode base on size 1 bite have 8 bits
	    return d_success;
    else
	    return d_failure;
   }
}
Status decode_image_to_data(char *ch,int size,FILE *fptr_stego_image)
{
	char arr[8];
	for(int i=0;i<size;i++)
	{
		fread(arr,1,8,fptr_stego_image);
		ch[i]=decode_data_lsb_side(arr);
	}
	return d_success;
}
Status decode_data_lsb_side(char *image)
{
	char c=0;
	for(int i=0;i<8;i++)
	{
		c=c<<1;
		c=(image[i]&0x01)|c;
	}
         return c;
}
Status decode_exten_size(DecodeInfo *decInfo,int *s)                                                   //find the exten file size stored in 32 bits
{
	char buf[32];
	fread(buf,1,32,decInfo->fptr_stego_image);
        decode_size(buf,s);
	return d_success;

}
Status decode_size(char *buf,int *n)
{
	int num=0;
	for(int i=0;i<32;i++)
	{
		num=num<<1;
		num=(buf[i] &0x01)|num;
	}
	*n=num;
}
Status decode_exten_file(DecodeInfo *decInfo,int n)
{
     unsigned char arr[n+1];
		if(decode_image_to_data(arr,n,decInfo->fptr_stego_image)==d_success)              //find the extension file using length stored in 1 bit have 8 bits
		{
			arr[n]='\0';
		}
               decInfo->text_file=malloc(20);
	       strcpy(decInfo->text_file,arr);
	return d_success;
}
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	      if(strchr(decInfo->name,'.')==NULL)                                                //exten file is user given its stored like a exten file name
	      {
         	 decInfo->name=strcat(decInfo->name,decInfo->text_file);
	      }
	      else
	      {
		      if(strstr(decInfo->name,decInfo->text_file)==NULL)
		      {
			      decInfo->name=strcat(decInfo->name,decInfo->text_file);           //its store like default value
		      }
	      }
	 decInfo->fptr_secret = fopen(decInfo->name,"w");
	 return d_success;
}
Status decode_secret_file_size(DecodeInfo *decInfo,int size)
{
        if(decode_exten_size(decInfo,&size)==d_success);                                        //finding the secret file size using 32 bits have 32 bytes
	{  
        decInfo->size_secret_file=size;
	printf("/INFO : Secret file size = %d  /\n",size);
	return d_success;
	}
}
Status decode_secret_file_data(DecodeInfo *decInfo)
{
	int size=decInfo->size_secret_file;
	char str[size];
	if(decode_image_to_data(str,size,decInfo->fptr_stego_image)==d_success);               //finding the secret data deponds on size 
        {
		str[size]='\0';
		printf("INFO : Secret data = %s",str);
		fwrite(str,1,size,decInfo->fptr_secret);
	}
	return d_success;
       
}
Status do_decoding(DecodeInfo *decInfo)
{
	int size=0;
      if(open_file(decInfo)==d_success)
      {
	      printf("3 - - - - - - - Open file is Successfully - - - - - - - -\n");
      }
      else
      {
	      printf("3 * * * * * * * * Open failed is failed * * * * * * * * * *\n");
	      return e_failure;
      }
      if(decode_magic_string(decInfo)==d_success)
      {
             printf("4 - - - - - - - - decoded magic string - - - - - - - - - - -\n");
      }
      else
      {
	      printf("4 * * * * * * * decode magic string is fail * * * * * * * * *\n");
      }
      if(decode_exten_size(decInfo,&size)==d_success)
      {
	      printf("/INFO : Extension file size = %d  /\n",size);
	      printf("5 - - - - - - - Exten size is success - - - - - - - - - - -\n");
       }
      else
      {
	      printf("5 * * * * * * * * Exten file is fail * * * * * * * * * * * * \n");
	      return d_failure;
      }
      if(decode_exten_file(decInfo,size)==d_success)
      {
	      printf("6 - - - - - - - - Exten file is decoded - - - - - - - - - - \n");;
      }
      else
      {
	      printf("6 * * * * * * * Decode Exten file is fail * * * * * * * * * *\n");
      }
      if(decode_secret_file_extn(decInfo)==d_success)
      {
	      printf("7 - - - - - - Decoded secret file extension - - - - - - - - - \n");
      }
      else
      {
	      printf("7 * * * * * * * Fail secret file extension * * * * * * * * * * *\n");
      }
      if(decode_secret_file_size(decInfo,size)==d_success)
      {
	      printf("7 - - - - - - - Decoded secret file size - - - - - - - - - - -\n");
      }
      else
      {
	      printf("7 * * * * * * Decode file size is failed * * * * * * * * * * *\n");
      }
      if(decode_secret_file_data(decInfo)==d_success)
      {
	      printf("8 - - - - - Decoded secret file successfully - - - - - - - - -\n");
      }
      else
      {
	      printf("8 * * * * * * * * Decode file is FAIL * * * * * * * * * * * * \n");
	      return d_failure;
      }
   return d_success;  
}
