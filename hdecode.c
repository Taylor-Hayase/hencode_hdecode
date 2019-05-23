#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include "hdecode.h"
#include "htable.h"

int main(int argc, char *argv[])
{

   int fd1 = 0, fd2 = 1, in_c, cf;
   int out_file;
   int i = 0, total_freq = 0;
   char c;
   unsigned int *cptr = (unsigned int *)calloc(10, sizeof(char *));
   char *input = (char *)calloc(200000, sizeof(char *));
   char *bin_buf = (char *)calloc(10, sizeof(char));

   uint32_t buff_4;
   uint8_t buff_1;
   off_t end, curr;
  /* int bit;*/

   int num_chars = 0;
   int freqArr[256] = {0};

   Node *head;
   HuffNode *huffHead/*, *ptr*/;
   
   /*read stdin and write atdout*/
   if (argc == 1)
   {
      out_file = 0;
   }
   /*either given "-" or input file*/
   else if (argc == 2)
   {
      /*given "-" read stdin*/
      if (strcmp(argv[1], "-") > 0)
      {
         fd1 = 0;
         fd2 = 1;
      }
      /*check if file exists*/
      if ((fd1 = open(argv[1], O_RDONLY)) < 0)
      {
         fprintf(stderr, "Usage: hdecode [(infile|-) [outfile]]\n");
         return 1;
      }
      /*check if file is not empty*/
      else if ((end = lseek(fd1, 0, SEEK_END)) == 0)
      {
         fprintf(stderr, "Usage: hdecode [(infile|-) [outfile]]\n");
         return 1;
      }
      else
      {
         /* to `pow' = lseek(fd1, 0, SEEK_CUR);*/
         lseek(fd1, 0, SEEK_SET);
      }
      out_file = 0;
   }
   /*input and output file given*/
   else if (argc == 3)
   {
      /*check if input exists*/
      if ((fd1 = open(argv[1], O_RDONLY)) < 0)
      {
         fprintf(stderr, "Usage: hdecode [(infile|-) [outfile]]\n");
         return 1;
      }
      /*check that input is not empty*/
      else if ((end = lseek(fd1, 0, SEEK_END)) == 0)
      {
         fprintf(stderr, "Usage: hdecode [(infile|-) [outfile]]\n");
         return 1;
      }
      else
      {
         lseek(fd1, 0, SEEK_SET);
      }
      /*open or create output file*/
      if ((fd2 = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0666)) < 0)
      {
         fprintf(stderr, "Usage: hdecode [(infile|-) [outfile]]\n");
         return 1;
      }
      out_file = 1;
   }
   else
   {
      fprintf(stderr, "Usage: hdecode [(infile|-) [outfile]]\n");
      return 1;
   }

   /*read in the number of character in file*/
   read(fd1, &buff_4, sizeof(uint32_t));

   num_chars = buff_4;
   /*printf("num chars: %d\n", num_chars);*/

   if (num_chars == 0)
   {
      close(fd2);
      return 1;
   }

   /*read in charaters and their frequencies*/
   for (i = 0; i < num_chars; i++)
   {
      /*character is 1 byte*/
      read(fd1, &buff_1, sizeof(uint8_t));
      c = buff_1;

      /*frequency is 4 bytes*/
      read(fd1, &buff_4, sizeof(uint32_t));
      cf = buff_4;

      /*place into frequency array*/
      freqArr[(int)c] = cf;
      total_freq += cf;
   }

   /*make huffman tree*/
   head = into_link(freqArr);
   
   sort(head, 0);

   huffHead = buildTree(head);

   curr = lseek(fd1, 0, SEEK_CUR);

  /* output = (char *)calloc(200000, sizeof(char *));*/

   /*read in the encoded input*/   
   while (curr < end && total_freq > 0)
   {
      /*read one byte at a time*/
      read(fd1, &buff_1, sizeof(uint8_t));

     /* for (i = 0; i < 8 && total_freq > 0; i ++)
      {
         bit = 128 & (buff_1);
         buff_1 = buff_1 << 1;
         
         if (ptr->right == NULL && ptr->left == NULL)
         {
            write(fd2, &(ptr->letter), sizeof(uint8_t));
            total_freq--;
            ptr = huffHead;
         }

         if (bit == 128)
         {
            ptr = ptr->right;
         }
         else
         {
            ptr = ptr->left;
         }

      }
      curr = lseek(fd1, 0, SEEK_CUR);

   }*/



      int_to_bin(buff_1, bin_buf, 8);
      /*printf("binary: %s\n", bin_buf);*/

      strcat(bin_buf, "\0");

      strcat(input, bin_buf);

      curr = lseek(fd1, 0, SEEK_CUR);
   }

   strcat(input, "\0"); 

   i = 0;
   in_c = 0;
   
  /* ptr = huffHead;*/
   

   /*traverse tree to decode input*/
   while (total_freq > 0)
   {
      in_c = decodeIn(huffHead, in_c, input, fd2);

      /*write(fd2, cptr, sizeof(uint8_t));*/

      total_freq -= 1;
      /*i++;*/
   }

   close(fd1);
   /*if file given, close it*/
   if (out_file == 1)
   {
      close(fd2);
   }

   /*free everything when done*/
   free(bin_buf);
   free(cptr);
   if (strlen(input) > 0)
      free(input);
   freeTree(huffHead);
   free(head);

   return 0;
}

/*translate the given integer to binary*/
void int_to_bin(uint8_t a, char *buffer, int buff_size)
{
   int i;
   int count = (buff_size - 1);

   for (i = buff_size - 1; i >= 0; i--)
   {
      *(buffer + count) = (a & 1) + '0';
      a = a >> 1;
      count--;
   }
}

