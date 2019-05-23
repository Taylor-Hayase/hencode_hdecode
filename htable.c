#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include "htable.h"

/*function to free the tree after program finishes*/
void freeTree(HuffNode *head)
{
   if (head == NULL)
      return;

   freeTree(head->left);
   freeTree(head->right);
   
   free(head);
}

/*program to read from a file pointer*/
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

/*change frequency array into a linked list*/
Node *into_link(int *freqArr)
{
   unsigned int i;
   Node *new, *ptr, *head;

   head = NULL;

   for (i = 0; i < 256; i++)
   {
      if (freqArr[i] > 0)
      {
         new = (Node *)calloc(1, sizeof(Node));
         new->huff = (HuffNode *)calloc(1, sizeof(HuffNode));

         new->huff->letter = (unsigned int)i;
         new->huff->freq = freqArr[(unsigned int)i];
         new->huff->left = NULL;
         new->huff->right = NULL;

         if (head == NULL)
         {
            new -> next = NULL;
            ptr = new;
            head = new;
         
         }
         else
         {
            new -> next = NULL;
            ptr -> next = new;
            ptr = new;
         }
      }
   }
   return head;
}

/*do a bubble sort of linked list*/
void sort(Node *head, int new)
{
   Node *ptr1;
   int swapped;
   unsigned int freq1, freq2;
   unsigned int let1, let2;

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
 

/*swap function for bubble sort*/
void swap(Node *ptr1, Node *ptr2)
{
   HuffNode *temp;
   temp = ptr1->huff;
   ptr1 -> huff = ptr2 -> huff;
   ptr2 -> huff = temp;
}


/*combine two huffman nodes within two linked list nodes into a single huffman node*/
Node *combine(Node * a, Node * b)
{
   Node *new;

   new = (Node *)calloc(1, sizeof(Node));
   /*new->next = (Node *)calloc(1, sizeof(Node));*/
   new->huff = (HuffNode *)calloc(1, sizeof(HuffNode));

   new->huff->left = NULL;
   new->huff->right = NULL;


   new->huff->letter = (a->huff->letter < b->huff->letter)? a->huff->letter : b->huff->letter;
   new->huff->freq = (a->huff->freq) + (b->huff->freq);
   new->huff->left = a->huff;
   new->huff->right = b->huff;
   
   new->next = b->next;

   a->next = NULL;
   b->next = NULL;


   return new;
}

/*overall function for combining and sorting to construct tree*/
HuffNode *buildTree(Node *head)
{
   Node *ptr, *sm1, *sm2;

   ptr = head;

   while (ptr -> next != NULL)
   {
      sm1 = ptr;
      sm2 = ptr -> next;

      ptr = combine(sm1, sm2);

      if (ptr -> next != NULL)
         sort(ptr, 1); 
   
   } 
   return ptr->huff;
} 
     
/*read through the tree and make a table of codes*/ 
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

int decodeIn(HuffNode *ptr, int in_c, char *input, int fd)
{
   while (ptr->right != NULL && ptr->left != NULL)
   {
      if (input[in_c] == '0')
      {
         ptr = ptr->left;
         in_c+=1;
      }
      else if (input[in_c] == '1')
      {
         ptr = ptr->right;
         in_c+=1;
      }
      else
         printf("----------k-----------\n");

      if (ptr->right == NULL && ptr->left == NULL)
         break;
   }
   write(fd, &(ptr->letter), sizeof(uint8_t)); 
   return in_c;  
}
