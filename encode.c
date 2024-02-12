#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)                                                     //check the given files 
{
	if(strstr(argv[2],".bmp")!=NULL)
	{
		encInfo->src_image_fname = argv[2];                                                                        //copy 2 nd position file to struct pointer
	}
	else
	{
		printf("2.1 -Please pass .bmp file \n");
		return e_failure;
	}

	if(strstr(argv[3],".txt")!=NULL)
	{
		encInfo->secret_fname = argv[3];         
              strcpy(encInfo->extn_secret_file,strstr(argv[3],"."));                                                      
        }
	else
	{
		printf("2.2 -Please pass .txt file \n");
		return e_failure;
	}
	if(argv[4]!=NULL)                                                                                                  //output file is optional 
	{
		if(strstr(argv[4],".bmp")!=NULL)                                                                        
		encInfo->stego_image_fname=argv[4];
	}
        else                                                                                                               //if user not give output file let create 
	{ 
		encInfo->stego_image_fname = "output.bmp";
	}
   return e_success;
}
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{ 
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);                                                                         

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("=> width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("=> height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
   fseek(fptr,0,SEEK_END);
    return  ftell(fptr); 

}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
//3
Status open_files(EncodeInfo *encInfo)                                                                                 //check the files are non empty
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");                                                   //open the file to store startind add pointer 
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
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");                                              //open file and write a data to struct pointer
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
//5
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
	
	fseek(fptr_src_image,0,SEEK_SET);                                                                            //to set a cursor in first position
	if(fptr_src_image!=NULL)                                                                                   
	{
	int arr[54];                                                                                                 
	//read a 54bytes from source image
	fread(arr,1,54,fptr_src_image);                                                                              //store first 54 bytes in arr its a heaer file
	//copy a 54bytes from new file
	fwrite(arr,1,54,fptr_dest_image);                                                                            //store in new file
	return e_success;
	}
	else
	{
		return e_failure;
	}
}

//4
Status check_capacity(EncodeInfo *encInfo)
{
	//store image size to struct image_capacity
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);                                       //find the length data from image 
        //store length of text file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);                                                 //find the length of secret data
       //find the length of magic string    
      int size = strlen(MAGIC_STRING);
     if(encInfo->image_capacity >= 54+(4+4+encInfo->size_secret_file+size)*8)                                        //file is greater then data size 334
            {
          return e_success;
	    }
       else
          return e_failure;	       
}
Status encode_byte_to_lsb(char data,char *image_buffer)
{
	//unsigned int mask=1<<7;
	for(int i=0;i<8;i++)
	{
		image_buffer[i]=(image_buffer[i] & 0xfe) | data>>(7-i)&0x01;                                         //imagebuffer is a bmp file data encode lsb side
	}
	return e_success;
}


Status encode_data_to_image(char *data,int size,FILE *fptr_src_image,FILE *fptr_dest_image)
{
	for(int i=0;i<size;i++)                                                                                      //size of the file to run loop
	{
	char image[8];
	fread(image,1,8,fptr_src_image);                                                                             //fetch 1,8 in image file
           encode_byte_to_lsb(data[i],image);
	   fwrite(image,1,8,fptr_dest_image);                                                                        //encode to lsb and stored
	}
	return e_success;
}
//6
Status encode_magic_string(char *image_data,EncodeInfo *encInfo)                                          //find the magic string length and encode it
{
	if(encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
	{
		return e_success;
        }
	else
	{
		return e_failure;
	}
}  
//7
Status encode_ext_size(int file,FILE *fptr_src_image,FILE *fptr_stego_image)
{
	//signed int mask =1<<7;
     char buffer[32];
     fread(buffer,1,32,fptr_src_image);                                                                              //extension size is int so stored in 32 bytes
	for(int i=0;i<32;i++)
	{
	   buffer[i] =(buffer[i] & 0xfe) | (file>>(31-i)&0x01);
	}
     fwrite(buffer,1,32,fptr_stego_image);

	return e_success;
}
	
//8
Status encode_secret_file_extn(char *file_extn,EncodeInfo *encInfo)                                               //extension file name to stored 32 bytes from image
{
	if(encode_data_to_image(file_extn,strlen(encInfo->extn_secret_file),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
	{
		return e_success;
        }
	else
	{
		return e_failure;
	}          	
}
//9
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
     char buffer[32];
     fread(buffer,1,32,encInfo->fptr_src_image);                                                                  //encode secret file size to stored int data 
	for(int i=0;i<32;i++)
	{
	   buffer[i] =(buffer[i] & 0xfe) | (file_size>>(31-i)&0x01);                                              //for every bit data have 1 byte of data in image
	}
     fwrite(buffer,1,32,encInfo->fptr_stego_image);

	return e_success;
}
//10
Status encode_secret_file_data(EncodeInfo *encInfo)
{    
	 fseek(encInfo->fptr_secret,0,SEEK_SET);
		 char buffer[encInfo->size_secret_file];
		 fread(buffer,sizeof(char),encInfo->size_secret_file,encInfo->fptr_secret);                       //encode secret file depends on size to take byte
         if(encode_data_to_image(buffer,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
	 {
		 return e_success;
	 }
	 else
	 {
		 return e_failure;
	 }

}
//11
Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
	int ch;
	while((ch=getc(fptr_src))!=EOF)                                                                             //copy to data file
	{
		putc(ch,fptr_dest);
	}
	return e_success;
}         
Status do_encoding(EncodeInfo *encInfo)
{
	if(open_files(encInfo) == e_success)
	{
		printf("3- - - - - - - File are open successfully - - - - - -\n");
	}
	else
	{
		printf("3* * * * * * * File open failed * * * * * * * *\n");
		return e_failure;
	}

	if(check_capacity(encInfo)==e_success)
	{
		printf("4- - - - - - -check capacity is done- - - - - - - -\n");
	}
	else
	{
		printf("4* * * * * * check capacity failed* * * * * * * * * \n");
		return e_failure;
	}

	if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
	{
		printf("5- - - -- - - header copied succesfully- - - - - - - -\n");
	}
        else
	{
		printf("5* * * * * * copied header is failed* * * * * * * * * \n");
		return e_failure;
	}
       	if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
        {
	        printf("6- - - - - - Magic string is encode- - - - - - - - -\n");
        }
	else
	{
	       printf("6* * * * * * Magic string not encode* * * * * * * * *\n");
		return e_failure;
	}
	if(encode_ext_size(strlen(encInfo->extn_secret_file),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
	{
               printf("7- - - - - - Extension size is Done - - - - - - - - -\n");
	}
	else
	{
	      printf("7* * * * * * Extension size is Fail * * * * * * * * * *\n");
		return e_failure;
	}	      
       if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
       { 
             printf("8- - - - - - Encode secrect file is done- - - - - - - -\n");
       }
      else
      	{
             printf("8- - - - - - Encode secrect file not done- - - - - - - -\n");
		return e_failure;
	}
      if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
      {
	      printf("9- - - - - - Secret file size encoded- - - - - - - -\n");
      }
      else
      {
	      printf("9* * * * * * Secret file size is not encoded * * * * *\n");
	      return e_failure;
      }
     if(encode_secret_file_data(encInfo)==e_success)
     {
	     printf("10- - - - - - Secret file date encoded - - - - - - - -\n");
     }
     else
     {
	     printf("10* * * * * * * Secret file data failed * * * * * * * \n");
	     return e_failure;
     }
     if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
     {
	     printf("11 / / /          Finally Copied        / / /\n");
     }
     else
     {
	     printf("11* * * * * * Remining Date are not copy * * * * * * *\n");
	     return e_failure;
     }

	/*if open_files()==e_success
	  print files are open successfully

	  else
	  print open_files failed
	  return e_failure;
	 */
	return e_success;
}

