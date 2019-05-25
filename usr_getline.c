#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "usr_getline.h"
#include "usr_realloc.h"

/* This function take a File* and a char* and  copies a line from the FILE*
 *  into the char* which has memory automatically allocated for it inside
 *  this function. Similar to getline() this function returns the number of characters
 *  that were read if there is no error. In the case of an error a descriptive message
 *  is printed and -1 is returned.
 */
char* usr_getline(FILE* input_file, char* ptr, int* counter) {
	int length = 0;
	int q = 0;
	int intCh;
	char ch;
	int init_size = 256;
	ptr = (char*) malloc(init_size * sizeof(char));
	if (ptr == NULL) {
		printf("Allocation of memory failed. File:%s Line:%s", __FILE__,
				__LINE__);
		return ptr;
	}
	intCh = fgetc(input_file);
	while ((intCh != EOF) && (intCh != '\n')) {
		ch = (char) intCh;
		ptr[q++] = ch;
		if (q == (init_size - 1)) {
			ptr = usr_realloc(ptr);
			if (ptr == NULL) {
				printf("Reallocation of memory failed. File:%s Line:%s",
						__FILE__, __LINE__);
				return ptr;
			}
			init_size = init_size * 2;
		}
		intCh = fgetc(input_file);
	}

	//WHILE loop has stopped, either intCh == EOF or intCh == '\n'
	if (intCh == '\n') {
		ptr[++q] = '\0';
		*ptr = ptr[0];
	}
	if (intCh == EOF) {
		ptr[0] = (char) -1;
		ptr[1] = 0;
		*counter = q;
		return ptr;
	}
	length = 0;
	while (length++ < 4)
		if (ptr[q - length] == '\r'){
			ptr[q - length] = 0;
			q -= length;
			break;
		}
	*counter = q;
	return ptr;
}

int get_sort_col(char* array, char* sortBy) {
	char ch = array[0];
	int i = 0;
	int j = strlen(array);
	int m = strlen(sortBy);
	int counter = 0;
	int k = 0;
	while (ch != '\n' && ch != 0 && i <= j) {
		if (ch == sortBy[0]) {
			counter = i;
			while (ch == sortBy[i - counter] && ch != ',' && ch != 0
					&& ch != '\n' && i - counter <= m) {
				ch = array[++i];
			}
			/*at the end and the next one is comma or null or newline*/
			if (i - counter == m && (ch == '\n' || ch == 0 || ch == ',')) {
				return k;
			}
		}
		else if (ch == ',')
			k++;
		ch = array[++i];
	}
	return -1;
}

ssize_t readn(int fd, void *buffer, size_t n, char *callingFile, int sockID) {
	ssize_t numRead; /* # of bytes fetched by last read() */
	size_t totRead; /* Total # of bytes read so far */
	char *buf;
	buf = buffer; /* # no pointer arithmetic on void* */
	for (totRead = 0; totRead < n;) {
		numRead = read(fd, buf, n - totRead);

		if (numRead == -1) {
			perror("\nreadN");
			fprintf(stderr, "\n File: %s, sockID: %d, \n Buff: %s\n", callingFile, sockID, buf);
			fflush(stderr);
			if (errno == EINTR)
				continue; /*  Interrupted --> restart read()  */
			else
				return -1; /* Some other error */
		}
		totRead += numRead;
		buf += numRead;
	}
	return totRead; /* Must be n bytes if here */
}

ssize_t writen(int fd, void *buffer, size_t n, char* callingFile, int sockID) {
	ssize_t numWritten; /* # of bytes written by last write() */
	size_t totWritten; /* Total # of bytes written so far */
	const char *buf;
	buf = buffer; /* no pointer arithmetic on void */
	for (totWritten = 0; totWritten < n; ) {
		numWritten = write(fd, buf, n - totWritten);

		if (numWritten <= 0) {
			if (numWritten == -1){
				perror("\nWriteN");
				fprintf(stderr, "\n File: %s, sockID: %d, \n Buff: %s\n", callingFile, sockID, buf);
				fflush(stderr);
			}
			if (numWritten == -1 && errno == EINTR)
				continue;/* Interrupted --> restart write() */
			else
				return -1; /* some other error */
		}
		totWritten += numWritten;
		buf += numWritten;
	}
	return totWritten; /* Must be n bytes if here */
}
