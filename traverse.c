#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <netdb.h>
#include "tokenizer.h"
#include "GlobalLL.h"
#include "usr_getline.h"
#include "traverse.h"

int port;
char* hostName;
pthread_mutex_t lock;
char* sortBy;
char* storeDir;
pthread_mutex_t condLock;
pthread_cond_t waiton;
pthread_mutex_t insertLock = PTHREAD_MUTEX_INITIALIZER;
int* found;
int sortAt, numCsv;
char firstClient = 1;

void* traverseDir(void* params) {
	DIR* dp;
	char* str_tempdir;
	struct dirent* entry;
	char extension[5];
	int i, j, returnStatus;
	pthread_t childId;
	char* dir = (char*) params;
	j = strlen(dir);
	if (dir[j - 1] != '/') { /* Add a / to the end of the path if it doesn't have it */
		dir = (char*) realloc(dir, j + 2);
		dir[j] = '/';
		dir[j + 1] = 0;
	}

	dp = opendir(dir);
	if (dp == 0) {
		perror("\n Error");
		free(dir);
		pthread_exit(0);
	}
	chdir(dir);
	while ((entry = readdir(dp)) != 0) {
		// Skip . and ..
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		// Only consider .csv files
		j = strlen(entry->d_name) - 4;
		for (i = 0; i < 4; i++)
			extension[i] = entry->d_name[j++];
		extension[4] = 0;
		if (strcmp(extension, ".csv") == 0 && j > 3) {
			str_tempdir = (char*) malloc(sizeof(char) * (strlen(dir) + strlen(entry->d_name) + 1));
			strcpy(str_tempdir, dir);
			strcat(str_tempdir, entry->d_name);
			returnStatus = pthread_create(&childId, 0, &parseFile, str_tempdir);
			if (returnStatus) {
				fprintf(stderr, "Thread creation failed for file: %s\n", str_tempdir);
				perror("\n Error");
				continue;
			}
			pthread_detach(childId);
		}
		else {
			str_tempdir = (char*) malloc(strlen(dir) + strlen(entry->d_name) + 2);
			strcpy(str_tempdir, dir);
			strcat(str_tempdir, entry->d_name);
			returnStatus = pthread_create(&childId, 0, &traverseDir, str_tempdir);
			if (returnStatus) {
				fprintf(stderr, "Thread creation failed for folder: %s\n", str_tempdir);
				continue;
			}
			pthread_detach(childId);
		}
	}
	chdir("..");
	closedir(dp);
	free(dir);
	return 0;
}

void* parseFile(void* params) {
	char* token;
	char buffer[1024] = { 0 };
	char ndBuf[6] = { 0 };
	strcpy(ndBuf, "^CSV^");
	char firstIter = 1;
	int a, numChs = 0;
	int total_num_chs;
	char* inputline = (char*) malloc(sizeof(char));
	char lagCh, leadCh;
	int lines = 0;
	int rows, columns;
	FILE* fp;
	int counter = 0;
	columns = 28;
	int i, j, m, n;
	int is_EOF = 0;
	char* dir = (char*) params;

	struct addrinfo hints;
	struct addrinfo* serverInfo;
	char lePort[6] = { 0 };
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	sprintf(lePort, "%d", port);

	if (getaddrinfo(hostName, lePort, &hints, &serverInfo)) {
		gai_strerror(errno);
		fprintf(stderr, "getaddrinfo failed at %s:%d\n", __FILE__, __LINE__);
		free(inputline);
		pthread_mutex_lock(&condLock);
		(*found)++;
		pthread_cond_signal(&waiton);
		pthread_mutex_unlock(&condLock);
		return 0;
	}

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		fprintf(stderr, "Failed to create a socket at %s:%d\n", __FILE__,
		__LINE__);
		free(inputline);
		freeaddrinfo(serverInfo);
		pthread_mutex_lock(&condLock);
		(*found)++;
		pthread_cond_signal(&waiton);
		pthread_mutex_unlock(&condLock);
		return 0;
	}

	if (connect(sock, serverInfo->ai_addr, serverInfo->ai_addrlen) == -1) {
		perror("Error: ");
		fprintf(stderr, "Failed to connect at %s:%d\n", __FILE__, __LINE__);
		free(inputline);
		freeaddrinfo(serverInfo);
		pthread_mutex_lock(&condLock);
		(*found)++;
		pthread_cond_signal(&waiton);
		pthread_mutex_unlock(&condLock);
		return 0;
	}

	int sockID;
	sockID = sock;

	fp = fopen(dir, "r");
	if (fp == NULL) {
		perror("ERROR");
		fprintf(stderr, "Error number = %d\n", errno);
		fprintf(stderr, "Error location: File:%s Line:%d\n", __FILE__,
		__LINE__);
		free(inputline);
		pthread_mutex_lock(&condLock);
		(*found)++;
		pthread_cond_signal(&waiton);
		pthread_mutex_unlock(&condLock);
		return 0;
	}
	//checks how many lines there are before array size is declared
	lines += line_counter(fp);
	if (lines == 0) {
		fprintf(stderr, "The file %s is empty\n", dir);
		fprintf(stderr, "Error location: File:%s Line:%d\n", __FILE__,
		__LINE__);
		free(inputline);
		fclose(fp);
		pthread_mutex_lock(&condLock);
		(*found)++;
		pthread_cond_signal(&waiton);
		pthread_mutex_unlock(&condLock);
		return 0;
	}

	if (lines == -1) {
		perror("ERROR");
		fprintf(stderr, "Error number = %d\n", errno);
		fprintf(stderr, "Error location: File:%s Line:%d\n", __FILE__,
		__LINE__);
		free(inputline);
		fclose(fp);
		pthread_mutex_lock(&condLock);
		(*found)++;
		pthread_cond_signal(&waiton);
		pthread_mutex_unlock(&condLock);
		return 0;
	}
	fseek(fp, 0L, SEEK_SET);

	buffer[0] = '^';
	buffer[4] = '^';
	buffer[1] = numCsv / 100 + '0';
	buffer[2] = (numCsv % 100) / 10 + '0';
	buffer[3] = (numCsv % 100) % 10 + '0';

	rows = lines;
	// Loop rows
	int prevChs = 0;
	char* prevline = usr_getline(fp, prevline, &prevChs);
	for (m = 0; m < rows && !is_EOF; m++) {
		numChs = 0;
		free(inputline);
		inputline = prevline;
		numChs = prevChs;
		prevline = usr_getline(fp, prevline, &prevChs);
		if (inputline == NULL) {
			fprintf(stderr, "Reallocation of memory failed. File:%s Line:%d\n",
			__FILE__, __LINE__);
			fclose(fp);
			pthread_mutex_lock(&condLock);
			(*found)++;
			pthread_cond_signal(&waiton);
			pthread_mutex_unlock(&condLock);
			return 0;
		}
		else if (numChs == 0) {
			fprintf(stderr, "NumChs are 0. They shouldn't be 0.\n");
			fclose(fp);
			pthread_mutex_lock(&condLock);
			(*found)++;
			pthread_cond_signal(&waiton);
			pthread_mutex_unlock(&condLock);
			return 0;
		}
		if (*prevline == (char) -1 || prevChs < 2)
			is_EOF = 1;

		if (m == 0) {
			int num_commas = 0;
			int never = 0;
			char commas;
			if ((never = get_sort_col(inputline, sortBy)) == -1) {
				free(inputline);
				fclose(fp);
				pthread_mutex_lock(&condLock);
				(*found)++;
				pthread_cond_signal(&waiton);
				pthread_mutex_unlock(&condLock);
				free(prevline);
				return 0;
			}
			sortAt = never;
			for (never = 0; never < numChs; never++) {
				commas = inputline[never];
				if (commas == ',')
					num_commas++;
			}
			if (num_commas != 27) {
				free(inputline);
				free(prevline);
				fclose(fp);
				pthread_mutex_lock(&condLock);
				(*found)++;
				pthread_cond_signal(&waiton);
				pthread_mutex_unlock(&condLock);
				return 0;
			}
		}
		if (firstIter) {
			firstIter = 0;
			//CSV-count request
			pthread_mutex_lock(&lock);
			if (firstClient) {
				firstClient = 0;
				writen(sock, ndBuf, 5, __FILE__, sockID);
				writen(sock, buffer, 5, __FILE__, sockID);
			}
			pthread_mutex_unlock(&lock);
			memset(buffer, 0, strlen(buffer));

			// Store request
			buffer[0] = '^';
			buffer[1] = '0';
			buffer[2] = '1';
			buffer[3] = '0';
			buffer[4] = '^';
			writen(sock, buffer, 5, __FILE__, sockID);

		}
		else {
			memset(buffer, 0, strlen(buffer));
			buffer[0] = '^';
			a = numChs / 100;
			buffer[1] = a + '0';
			a = (numChs % 100) / 10;
			buffer[2] = a + '0';
			a = (numChs % 100) % 10;
			buffer[3] = a + '0';
			buffer[4] = '^';
			writen(sock, buffer, 5, __FILE__, sockID);

			memset(buffer, 0, strlen(buffer));
			strcpy(buffer, inputline);

			writen(sock, buffer, numChs, __FILE__, sockID);
			// EOF control string
			memset(buffer, 0, strlen(buffer));
			if (is_EOF) {
				buffer[0] = '^';
				buffer[1] = '0';
				buffer[2] = 'X';
				buffer[3] = '0';
				buffer[4] = '^';
			}
			else { /* \n */
				buffer[0] = '^';
				buffer[1] = '0';
				buffer[2] = '0';
				buffer[3] = '0';
				buffer[4] = '^';
			}
			writen(sock, buffer, 5, __FILE__, sockID);
		}
	}

	free(inputline);
	fclose(fp);
	close(sock);
	free(prevline);
	pthread_mutex_lock(&condLock);
	(*found)++;
	pthread_cond_signal(&waiton); /* Let main know that the found variable has changed */
	pthread_mutex_unlock(&condLock);
	freeaddrinfo(serverInfo);
	return 0;
}

void setPort(int c) {
	port = c;
}

void setHost(char* c) {
	hostName = c;
}

int getSortAt() {
	return sortAt;
}

void setCsv(int a) {
	numCsv = a;
}
