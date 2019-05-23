#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include "hencode.h"
#include "htable.h"

int main(int argc, char *argv[])
{
   int fd1 = 0, fd2 = 1, infile;
   char buf;
   char *input, *b_output;
   
   int i = 0;
   int buf_len = 0;
   int total_chars = 0;

   uint32_t buff_4 = 0;
   uint8_t buff_1 = 0;
   int freqArr[256] = {0};
   
   Node *lstHead, *ptr;
   HuffNode *huffHead;
   int length = 0;

   char **codeArr = (char **)calloc(256, sizeof(char *));
   char *code = (char *)calloc(10, sizeof(char *));

   /*need at least one file, or '-'*/
   if (argc == 1)
   {
      fprintf(stderr, "Usage: hencode infile [outfile]\n");
   }

   /*reading from file or stdin, and possibly output to file*/
   else if (argc == 2 || argc == 3)
   {
      input = (char *)malloc(buf_len + 1);

      /*read stdin*/
      if (strcmp(argv[1], "-") == 0)
      {
         while ((infile = read(fd1, &buf, 1)) > 0)
         {
            buf_len += 1;
            input = realloc(input, buf_len + 1);
            input[i] = buf;
            i++;
         }
      }
      /*open file and read from file*/
      else if ((fd1 = open(argv[1], O_RDONLY | O_CREAT)) > 0)
      {
         /*check file isn't empty*/
         if (lseek(fd1, 0, SEEK_END) == 0 && argc == 3)
         {
            fd2 = open(argv[2], O_CREAT | O_WRONLY, 0666);
            write(fd2, &buff_4, sizeof(uint32_t));
            close(fd2);
            return 1;
         }
         else if (lseek(fd1, 0, SEEK_END) == 0 && argc == 2)
         {
            return 1;
         }
         else
         {
            lseek(fd1, 0, SEEK_SET); 
         }
         /*read in character by character from given file*/
         while ((infile = read(fd1, &buff_1, 1)) > 0)
         {
            buf_len += 1;
            input = realloc(input, buf_len + 1);
            input[i] = buff_1;
            i++;
         }
      }

      input[i] = '\0';

      lseek(fd1, 0, SEEK_SET);
      /*read in chars and count frequencies*/
      read_input(fd1, freqArr);

      for (i = 0; i < 256; i++)
      {
         total_chars += freqArr[i];
      }

      lstHead = into_link(freqArr);

      /*find length of linked list*/
      ptr = lstHead;
      while(ptr != NULL)
      {
         length+=1;
         ptr = ptr->next;
      }
   
      /*create the tree*/
      sort(lstHead, 0);
      /*ptr = lstHead;*/

      huffHead = buildTree(lstHead);

      /*get the codes*/
      makeCode(huffHead, code, 0, codeArr);
      buff_4 = length;

      i = 0;
      buf_len = 0;
      b_output = (char *)calloc(50000, sizeof(char *));

      /*encod the input*/
      while (total_chars > 0)
      {
         if (i > strlen(b_output))
         {
            b_output = realloc(b_output, strlen(b_output) + 10);
         }
         if (b_output == NULL)
         {
            b_output = codeArr[(int)input[i]];
         }
         else
            strcat(b_output, codeArr[(unsigned char)input[i]]);
         buf_len += strlen(codeArr[(unsigned char)input[i]]);
         i++;
         total_chars--;
      }
      
      b_output[buf_len] = '\0';

      /*don't need input or code array anymore*/
      free(input);
   
      for (i = 0; i < 256; i++)
      {
         free(codeArr[i]);
      }
   
/*deal with file output*/

      /*output file given, open file*/
      if (argc == 3)
      {
         fd2 = open(argv[2], O_WRONLY | O_CREAT, 0666);
      }
 
      /*output number of different characters*/
      write(fd2, &buff_4, sizeof(uint32_t));


      /*read each char and its freq count*/
      for (i = 0; i < 256; i++)
      {
         if (freqArr[i] > 0)
         {
            /*write the character as 1 byte*/
            buff_1 = i;
            write(fd2, &buff_1, sizeof(uint8_t));

            /*write the frequency of character as 4 bytes*/
            buff_4 = freqArr[i];
            write(fd2, &buff_4, sizeof(uint32_t));
         }  
      }

      i = 0;

      /*read in 8 bits (1 byte), place into unit8_t and output*/
      while (b_output[i] != '\0')
      {
         buff_1 = bin_to_int(i, b_output);
         write(fd2, &buff_1, sizeof(uint8_t));
         i += 8;
      }
   
      /*if file given, close it after writing*/
      if (argc == 3)
      {
         close(fd2);   
      }

      /*free everything else*/
      close(fd1);     
      free(b_output); 

      freeTree(huffHead);

      free(lstHead);
      free(codeArr);
      free(code);
    
   }

   else
   {
     fprintf(stderr, "Usage: hencode infile [outfile]\n");
   }

   return 0;
}

/*function to read input and place character frequencies into array*/
void read_input(int file, int freqArr[])
{
   char c;
   unsigned int x;
   
   x = (read(file, &c, 1) == 1) ? (unsigned char) c : EOF;

   while (x != EOF)
   {
      freqArr[(int)x] += 1;
      x = (read(file, &c, 1) == 1) ? (unsigned char) c : EOF;
   }
}

/*translate given binary number into an integer*/
int bin_to_int(int start, char *str)
{
   int i, pw;
   int result = 0;
   pw = 7;

   for (i = start; i < start + 8; i++)
   {
      if (str[i] == '1')
      {
         result += pow(2, pw);
      }
      pw -= 1;
   }
   return result;
}
