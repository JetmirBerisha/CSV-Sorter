#ifndef TRAVERSE_H
#define TRAVERSE_H
#include "traverse.h"

typedef struct params {
	int* kidsNum;
	pthread_mutex_t* numLock;
	char* sortBy;
	char* storeDir;
	char dir[256];
} PARAMS;

/*
 * Reads in files
 */
void* parseFile(void* params);

/*
 * Traverses through directories and creates threads
 */
void* traverseDir(void* params);

void setPort(int);

void setHost(char*);

void setCsv(int a);

int getSortAt();

#endif
