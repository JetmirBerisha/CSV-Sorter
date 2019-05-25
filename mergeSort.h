/**
	@author
	Jetmir Berisha
	
	This program uses a set of functions to apply merge sort to a 
	two dimensional array. It doesn't return anything since it works
	based on pointers and changes the array that is passed in.
*/


#ifndef MERGESORT_H
#define MERGESORT_H

#include "GlobalLL.h"

/**
   This function acts like strcmp. It takes two parameters and returns an integer 
   value based on those parameters. It can compare integer or string values.
   @params
   char* left is the string of the left array that is to be compared.
   char* right is the string of the right array that is to be comapred.
   @returns
   -1 if left < right
   0  if left = right
   1  if left > right
*/
int compareByType(char* left, char* right);

/**
   The main sorting function. In this function each left segment is compared with its
   respectful right segment and is sorted on an ascending lexographical order.
   @params
   headRef: is thre pointer that points to head
*/
void sort(node** headRef);

/**
	Mergers the left (a) and the right(b) nodes
 */
node* sortedmerge(node* a, node* b);

/**
   Recursively splits the array into two roughly equal parts, left and right.
   @params
   char** array is the pointer to the top left of the 2D array.
   int left is the offset of the left half.
   int right is the offset of the right half.
*/
void frontbacksplit(node* source, node** frontRef, node** backRef);

/**
   The acess point for initializing the sorting algorythm. Then this function controls the
   sort by calling all of the backbone functions in order.
   @params
   char** array is the pointer to the first element of the 2D array.
   int rows is the number of rows that this array has, since size information is lost 
   when an array is passed to a function .
   int columns is the number of columns array has.
   char* sortBy is the string which denotes the column to sort based on.
*/
int mergeSort(node** head, int sortBy);

/**
	This function takes the pointer to [0][0] of two arrays and their
	respective sizes. It then performs mergesort on them in the same fashion
	as sort(...) and return the first element of a new array that contains
	the sorted records.
 */
//char* bigMerge(char* array1, char* array2, int rows1, int rows2);

int getSortingColumn(char* array[], char* sortBy);

#endif
