#include<stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
/*
NAME : Beeman P;
DATE : 10-01-2024
DESCRIPTION : Steganography
*/


int main(int argc,char *argv[])
{ 
    EncodeInfo encInfo;  
     DecodeInfo decInfo;

       if(check_operation_type(argv) == e_encode)
       {
	       if(argc >=4)
	       { 
		       printf("1  <<<<<<<<<        YOU SELECT ENCODING      >>>>>>>>>\n");
		       if(read_and_validate_encode_args(argv,&encInfo)==e_success)
		       {
			       printf("2 -------------Read and validate is done---------------\n");
			       if(do_encoding(&encInfo)==e_success)
			       {
				       printf("   >>>>>>>>>>>     ENCODIND COMPILETED     <<<<<<<<<<<\n");
			       }
			       else
			       {
				       printf("   >>>>>>>>>>         FAIL ENCODING      <<<<<<<<<<<\n");
			       }
		       }
		       else
		       {
			       printf("2 ****************Read and validate is not done**********************\n");
		       }
	       }
	       else
	       {
		       printf("2 *************** Enter a validate files *********************\n");
	       }
       }       
       else if (check_operation_type(argv) == e_decode)
       {
	       if(argc ==4 || argc ==3)
	       {
	            printf("1  <<<<<<<<<<          YOU SELECT DECODING        >>>>>>>>>>>>>>\n");
			       if(read_and_validate_decode_args(argv,&decInfo)==d_success)
			       {
				       printf("2 ------------------Read and validate is done------------------------\n");
				       if(do_decoding(&decInfo)==d_success)
				       {
					       printf("  >>>>>>>>>>>>       DECODING COMPILETED      <<<<<<<<<<<<<<<<\n");
				       }
				       else
				       {
					       printf("   >>>>>>>>>>>        FAIL DECODING        <<<<<<<<<<<<<<<<<\n");
				       }
			       } 
			   
		     
	       }   
            else
               {
		      printf("2 ************** Enter a validate files **********************\n");
	       } 
       }  
      
       else if (check_operation_type(argv) == e_unsupported)
       {
	       printf("1 ***************pass the correct arguments******************\n");
       }
}
OperationType check_operation_type(char *argv[])
{
	if(argv[1]!=NULL)
	{
		if(strcmp(argv[1],"-e")==0)
		return e_encode;
		else if(strcmp(argv[1],"-d")==0)
		 return e_decode;
		else
		return e_unsupported;
	}
	else
		return e_unsupported;
}
