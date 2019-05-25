#ifndef USR_GETLINE_H
#define URS_GETLINE_H

char* usr_getline(FILE* input_file, char* ptr, int* counter);

int get_sort_col(char* array, char* sortBy);

ssize_t readn(int fd, void *buffer, size_t count, char* callingFile, int sockID);

ssize_t writen(int fd, void *buffer, size_t count, char* callingFile, int sockID);

#endif
