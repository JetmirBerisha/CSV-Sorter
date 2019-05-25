#ifndef GLOBAlLL_H
#define GLOBAlLL_H
#include "GlobalLL.h"

typedef struct _node{
  char** data; // Will hold the row and columns.
  struct _node* next; //Will point to the next Node.
}node;

int check_head();

int insertion_sort(char* record[], int column, int dtype);

void print_list(char* sortCol, char* path, int sortBy);

void free_list();

void get_headers(char* record[]);

node* get_head();

int insert(char* record[]);

int compare_type(node* newptr, node* tempptr, int column, int dtype);

void check_strcmp(char* string1, char* string2);

#endif


