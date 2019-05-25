#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include "mergeSort.h"

static int sortingCol;

int mergeSort(node** head, int sortBy){
 	sortingCol = sortBy;
 	// Recursively divide the array into two parts, except for the first row
 	sort(head);
 	return 0;
}

void sort(node** headRef)
{
    node* head = *headRef;
    node* a;
    node* b;
    if ((head == NULL) || (head->next == NULL))
    {
        return;
    }
    frontbacksplit(head, &a, &b);
    sort(&a);
    sort(&b);
    *headRef = sortedmerge(a, b);
}

node* sortedmerge(node* a, node* b)
{
    node* result = NULL;

    if (a == NULL)
        return(b);
    else if (b == NULL)
        return(a);

    if (compareByType(a->data[sortingCol], b->data[sortingCol]) <= 0)
    {
        result = a;
        result->next = sortedmerge(a->next, b);
    }
    else
    {
        result = b;
        result->next = sortedmerge(a, b->next);
    }
    return(result);
}


int compareByType(char* left, char* right){
   	// Ints: 2, 3, 4, 5, 7, 8, 12, 13, 15, 18, 22, 23, 24, 27
   	// Doubles: 25, 26
	// Null case
	if (left == NULL )
		return -1;
	else if (right == NULL)
		return 1;

	//Ints
   	if (sortingCol == 2 || sortingCol == 3 || sortingCol == 4 || sortingCol == 5 || sortingCol == 7 || sortingCol == 8 || sortingCol == 12 || sortingCol == 13 || sortingCol == 15 || sortingCol == 18 || sortingCol == 22 || sortingCol == 23 || sortingCol == 24 || sortingCol == 27) {
   		int leftInt = 0;
		leftInt = atoi(left);
		int rightInt = 0;
		rightInt = atoi(right);
		int returnVal = 0;

		if (leftInt < rightInt)
			returnVal = -1;
		else if (leftInt > rightInt)
			returnVal = 1;
		else if (leftInt == rightInt)
			returnVal = 0;

		return returnVal;
   	}

   	// Doubles
    else if (sortingCol == 25 || sortingCol == 26){
    	double leftDouble = 0;
    	double rightDouble = 0;

    	sscanf(left, "%lf", &leftDouble); 		//Scanf a double from a string
    	sscanf(right, "%lf", &rightDouble);

    	if (leftDouble < rightDouble)
    		return -1;
    	else if (leftDouble > rightDouble)
    		return 1;
    	else if (leftDouble == rightDouble)
    		return 0;
    }

    // Strings
    else
    	return strcmp(left, right);
}


void frontbacksplit(node* source, node** frontRef, node** backRef)
{
    node* fast;
    node* slow;
    if (source==NULL || source->next==NULL)
    {
        *frontRef = source;
        *backRef = NULL;
    }
    else
    {
        slow = source;
        fast = source->next;
        while (fast != NULL)
        {
            fast = fast->next;
            if (fast != NULL)
            {
                slow = slow->next;
                fast = fast->next;
            }
        }
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}

int getSortingColumn(char* array[], char* sortBy) {
  	int i;
   	for(i = 0; i < 28; i++ )
 		if (strcmp(array[i], sortBy) == 0){
   			sortingCol = i;
 			return i;
 		}

  	return -1;
}
