#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "htable.h"

/*htable for executable*/
int main(int argc, char *argv[])
{
   int file;
   int freqArr[257] = {0};
   char **codeArr;
   char *code = (char *)calloc(10, sizeof(char*));
   int length = 0;
   int i;
   int old, new;
 
   Node *head, *ptr;
   HuffNode *Hhead;

   codeArr = (char**)calloc(256, sizeof(char *));

   if (argc != 2)
   {
      fprintf(stderr, "Usage: htable <filename>\n");
      return 1;
   }

   file = open(argv[1], O_RDONLY);
   old = lseek(file, 0, SEEK_CUR);
   new = lseek(file, 0, SEEK_END);

   if (new == old)
   {
      fprintf(stderr, "Usage: htable <filename>\n");
      return 1;
   }

   if (file < 0)
   {
      fprintf(stderr, "Usage: htable <filename>\n");
      return 1;
   }

   lseek(file, 0, SEEK_SET);
   read_file(file, freqArr);

   head = into_link(freqArr);
   ptr = head;
   
   while(ptr != NULL)
   {
      length += 1;
      ptr = ptr -> next;
   } 

   sort(head, 0);
   ptr = head;

   Hhead = buildTree(head);

   makeCode(Hhead, code, 0, codeArr);

   for (i = 0; i < 256; i++)
   {
      if (codeArr[i] != NULL && length != 0)
      {
         printf("0x%02x: %s\n", i, codeArr[i]);
         length -= 1;
      }
      free(codeArr[i]);
   }

   close(file);
   freeTree(Hhead);
   free(codeArr);
   free(code);
   
   return 0;
}

void freeTree(HuffNode *head)
{
   if (head == NULL)
      return;

   freeTree(head->left);
   freeTree(head->right);
   
   free(head);
}

void read_file(int file, int *freqArr)
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

Node *into_link(int *freqArr)
{
   unsigned int i;
   Node *new, *ptr, *head;
   HuffNode *newH;

   head = NULL;

   for (i = 0; i < 256; i++)
   {
      if (freqArr[i] > 0)
      {
         new = (Node *)calloc(1, sizeof(Node));
         newH = (HuffNode *)calloc(1, sizeof(HuffNode));

         newH -> letter = i;
         newH -> freq = freqArr[i];
         newH -> left = NULL;
         newH -> right = NULL;

         new -> huff = newH;

         if (head == NULL)
         {
            new -> next = NULL;
            ptr = new;
            head = new;
         
         }
         else
         {
            ptr -> next = new;
            new -> next = NULL;
            ptr = new;
         }
      }
   }
   return head;
}

void sort(Node *head, int new)
{
   Node *ptr1;
   int swapped;
   int freq1, freq2;
   int let1, let2;

   if (head == NULL)
      return;
   
   swapped = 1;
   while(swapped)
   {
      swapped = 0;
      ptr1 = head;
      
      while (ptr1->next != NULL)
      {
         freq1 = ptr1->huff->freq;
         freq2 = ptr1->next->huff->freq;

         let1 = ptr1->huff->letter;
         let2 = ptr1->next->huff->letter;

         if (freq1 > freq2 || (freq1 == freq2 && !new && let1 > let2))
         {
            swap(ptr1, ptr1->next);
            swapped = 1;
         }
         ptr1 = ptr1 -> next;
      }
   }
}
 
void swap(Node *ptr1, Node *ptr2)
{
   HuffNode *temp = ptr1->huff;
   ptr1->huff = ptr2->huff;
   ptr2->huff = temp;
}


Node *combine(Node * a, Node * b)
{
   HuffNode *newH;
   Node *new;

   new = (Node *)calloc(1, sizeof(Node));
   newH = (HuffNode *)calloc(1, sizeof(HuffNode));

   newH -> letter = (a->huff->letter < b->huff->letter)? a->huff->letter : b->huff->letter;
   newH -> freq = (a->huff->freq) + (b->huff->freq);
   newH -> left = a->huff;
   newH -> right = b->huff;

   new -> huff = newH;
   new -> next = b->next;

   a -> next = NULL;
   b -> next = NULL;

   return new;
}

HuffNode *buildTree(Node *head)
{
   Node *ptr, *sm1, *sm2;
   HuffNode *out;

   ptr = head;

   while (ptr -> next != NULL)
   {
      sm1 = ptr;
      sm2 = ptr -> next;

      ptr = combine(sm1, sm2);

      if (ptr -> next != NULL)
         sort(ptr, 1); 

      free(sm1);
      free(sm2);   

   } 
   out = ptr->huff;
   free(ptr);
   return out;
} 
      
void makeCode(HuffNode *root, char *code, int curr, char *codeArr[])
{

   if (root -> right != NULL)
   {
      code[curr] = '1';
      makeCode(root -> right, code, curr + 1, codeArr);
   }

   if (root -> left != NULL)
   {
      code[curr] = '0';
      makeCode(root -> left, code, curr + 1, codeArr);
   }
   
   if (root -> left == NULL && root -> right == NULL)
   {
      code[curr] = '\0';
      codeArr[(int)root->letter] = (char *)malloc(sizeof(char) * 100);

      strcpy(codeArr[(int)root->letter], code);

   }
}

