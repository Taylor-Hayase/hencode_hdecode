#ifndef HTABLE_H
#define HTABLE_H

typedef struct HuffNode {
   unsigned int letter;
   unsigned int freq;
   struct HuffNode *left;
   struct HuffNode *right;
} HuffNode;


typedef struct Node {
   struct HuffNode *huff;
   struct Node *next;
} Node; 

void read_file(int, int*);
   
Node *into_link(int *);

void sort(Node *, int);

void swap(Node *, Node *);

HuffNode *buildTree(Node *);

Node *combine(Node *, Node *);

void makeCode(HuffNode *, char *, int, char*[]);

int decodeIn(HuffNode *, int, char *, int);

void freeTree(HuffNode *);

#endif
