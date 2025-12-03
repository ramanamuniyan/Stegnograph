#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

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
   // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
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
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling 
      
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    else
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
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


Status check_capacity(EncodeInfo *encInfo)
{

    fseek(encInfo->fptr_secret,0,SEEK_END);
    encInfo->size_secret_file=ftell(encInfo->fptr_secret);
    rewind(encInfo->fptr_secret);


    if( (encInfo->image_capacity-54) < ((encInfo->size_secret_file+ 4 + encInfo->magic_string_size+4+ encInfo->ext_size+4)*8))
    return e_failure;

    return e_success;

}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{

   unsigned  char buf[54];
    fseek(fptr_src_image,0,SEEK_SET);
    fseek(fptr_dest_image,0,SEEK_SET);
    fread(buf,54,1,fptr_src_image);
    fwrite(buf,54,1,fptr_dest_image);
    //rewind(fptr_src_image);
    if(ftell(fptr_dest_image)!=54)
    return e_failure;
    //rewind(fptr_dest_image);

    return e_success;
}


OperationType check_operation_type(char* argv[])
{
    if(strcmp(argv[1],"-e")==0)
    
        return e_encode;

    else if(strcmp(argv[1],"-d")==0)

        return e_decode;

    else
    
        return e_unsupported;
}


Status read_and_validate_encode_args(EncodeInfo *encInfo)
{
    if(encInfo->src_image_fname == NULL || strstr(encInfo->src_image_fname,".bmp")==NULL)
        return e_failure;

    if(encInfo->secret_fname == NULL || strstr(encInfo->secret_fname,".")==NULL)
        return e_failure;
    else
    {
        strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname,"."));
        encInfo->ext_size = strlen(encInfo->extn_secret_file);
    }

    return e_success;
}


 Status encode_string_len(const  int size, EncodeInfo *encInfo)
 {

   
   // for(int i=0;i< encInfo->magic_string_size;i++)
   // {

       unsigned char buff[32];
        //printf("-%lu-",ftell(encInfo->fptr_src_image));
        fread(buff,32,1,encInfo->fptr_src_image);
       // printf("%d",magic_string_size);
        encode_size_to_lsb(size,buff);
        fwrite(buff,32,1,encInfo->fptr_stego_image);


   // }

    return e_success;
 }


Status encode_magic_string(const  char *magic_string, EncodeInfo *encInfo)
{

     unsigned char buf1[8];
     for(int i=0;i< encInfo->magic_string_size;i++)
    {

        
       // printf("-%lu-",ftell(encInfo->fptr_src_image));
        fread(buf1,8,1,encInfo->fptr_src_image);
       // printf("%s",magic_string);
        encode_byte_to_lsb(magic_string[i],buf1);
        fwrite(buf1,8,1,encInfo->fptr_stego_image);



    }

    return e_success;
 }



Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{

   unsigned char buf1[8];
     for(int i=0;i< encInfo->ext_size;i++)
    {

        
       // printf("-%lu-",ftell(encInfo->fptr_src_image));
        fread(buf1,8,1,encInfo->fptr_src_image);
       // printf("%s",magic_string);
        encode_byte_to_lsb(file_extn[i],buf1);
        fwrite(buf1,8,1,encInfo->fptr_stego_image);



    }

    return e_success;

}


Status encode_secret_file_data(EncodeInfo *encInfo)
{
  unsigned char buff[8];
 unsigned char ch;

  rewind(encInfo->fptr_secret);
    for(int i = 0 ; i < encInfo->size_secret_file ; i++)
    {
        fread(&ch,1,1,encInfo->fptr_secret);
        fread(buff,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(ch,buff);
        fwrite(buff,8,1,encInfo->fptr_stego_image);
    }

return e_success;
}




Status encode_byte_to_lsb(unsigned char data, unsigned char *image_buffer)
{

  //  printf(" (%c) ",data);
   unsigned int mask;
   unsigned int get;
    for(int i=0,n=7;i<8;i++)
    {
        mask=1<<n;
        get=(data & mask)>>n;

        image_buffer[i]= image_buffer[i]  &  0xFE;

        image_buffer[i]=image_buffer[i] | get;

        n--;

    }

    return e_success;
}

Status encode_size_to_lsb(unsigned int data, unsigned char *image_buffer)
{
   unsigned int mask;
   unsigned int get;
    for(int i=0,n=31;i<32;i++)
    {
        mask=1<<n;

        get=(data & mask)>>n;

        image_buffer[i]= image_buffer[i]  &  0xFE;

        image_buffer[i]=image_buffer[i] | get;

        n--;
    }
    
    // for(int i = 0 ; i < 32 ; i++)
    // {
    //     image_buffer[i] &= 0xFE;
    //     image_buffer[i] |= ((data >> (31 - i)) & 1);
    // }
     return e_success;
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{

   unsigned char ch;

    while(fread(&ch,1,1,fptr_src)>0)
    {
        fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{



//step 1

     if(open_files(encInfo)) //file open image ,secrect ,new bmp
     { 
   // puts("Error in file open ");
    perror("");
    return e_failure;
    }
   
   else

    puts("1.File successfully found");    

//step 2 
    if( check_capacity(encInfo))  //cheching secret file size enough to code
   
   {
     puts("2.Image size is not enough to encode");
        return e_failure;
   }

   else
   puts("3.Image size is enough to encode");


//step 4
    if( copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image))  //copy header
    { 
    puts("Header is not copied");
    return e_failure;
    }
    else
    puts("4.Header is copied");
//step 5


    if(encode_string_len(encInfo->magic_string_size,encInfo))
    puts("Magic size not done");
    else
    puts("5.Magic size done");

 
    if(encode_magic_string(encInfo->magic_string,encInfo))
    puts("Magic string not done");
    else 
    puts("6.Magic string encode done");


    
    if(encode_string_len(encInfo->ext_size,encInfo))//extern size
    puts("Secrect ext size not done");
    else
    puts("7.Secrect ext size done");

    if(encode_secret_file_extn(encInfo-> extn_secret_file,encInfo))
    puts("Secrect ext not done");
    else
    puts("8.Secrect ext done");

    if(encode_string_len(encInfo->size_secret_file,encInfo))//extern size
    puts("Secrect  size not done");
    else
    puts("9.Secrect  size done");

    if(encode_secret_file_data(encInfo))
    puts("Secrect  file not done");
    else
    puts("10.Secrect  file done");

    //*/

    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image))
    puts("All data not copied");

    else
    puts("11.Remaining img data copied");


    






    


    return e_success;

}


