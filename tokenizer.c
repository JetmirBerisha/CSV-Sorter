#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "tokenizer.h"
#include "GlobalLL.h"

char* tokenizer(char* array, int start, int size, char str) {
	char ch1;
	char * tokens;
	int i = start;
	int length;
	int j = 0;
	ch1 = array[start];
	while ((ch1 != str) && (array[i] != '\0') && (i < size)) {
		i++;
		ch1 = array[i];
	}
	length = i - start + 1;
	tokens = (char*) malloc(length * sizeof(char));
	if (tokens == NULL) {
		printf("Malloc failed to allocate memory for tokens.");
		return tokens;
	}
	for (j = 0; j < length; j++) {
		tokens[j] = array[start++];
	}
	tokens[--j] = '\0';
	return &tokens[0];
}

void tokzert(char* record) {
	char* token = (char*) malloc(sizeof(char));
	int numChs = strlen(record) + 1;
	char lagCh, leadCh;
	int i, j, m, n, a = 0;
	int is_EOF = 0;
	int index = 0;
	char inputArray[numChs];
	for (index = 0; index < numChs - 1; index++) {
		inputArray[index] = record[index];
	}
	inputArray[index] = 0;
	char* tokensArr[28];
	n = 0;
	for (i = 0; i < numChs; i++) {
		if (is_EOF)
			break;
		lagCh = inputArray[i];
		leadCh = inputArray[i + 1];

		if (i == 0) {
			if (lagCh == ',') {
				tokensArr[n] = (char*) malloc(sizeof(char) * 2);
				strcpy(tokensArr[n], "!\0");
				n++;
				// For the case when the row starts with two commas
				if (leadCh == ',') {
					tokensArr[n] = (char*) malloc(sizeof(char) * 2);
					strcpy(tokensArr[n], "!\0");
					n++;
				}
			}
			else {
				token = tokenizer(inputArray, i, numChs, ',');
				i += strlen(token) - 1;
				tokensArr[n] = (char*) malloc(strlen(token) + 2);
				strcpy(tokensArr[n], token);
				free(token);
				n++;
			}
		}
		else if (i == 1 && (lagCh != ',' && lagCh != '"')) {
			/// To take care of the star wars line. Applicable when lagCh isn't a delimiter
			token = tokenizer(inputArray, i, numChs, ',');
			i += strlen(token) - 1;
			tokensArr[n] = (char*) malloc(strlen(token) + 2);
			strcpy(tokensArr[n], token);
			free(token);
			n++;
		}
		else if (lagCh == ',' && leadCh == ',') {
			tokensArr[n] = (char*) malloc(sizeof(char) * 2);
			strcpy(tokensArr[n], "!\0");
			n++;
		}
		else if (lagCh == ',' && leadCh == '"') {
			i += 2;
			token = tokenizer(inputArray, i, numChs, '"');
			i += strlen(token);	//this extra +1 is for moving past the quote we'll end up at after adding the length of the token
			tokensArr[n] = (char*) malloc(strlen(token) + 2);
			strcpy(tokensArr[n], token);
			free(token);
			n++;
		}
		else if (lagCh == ',' && (leadCh != '"' && leadCh != ',')) {
			if (leadCh == '\0') {
				tokensArr[n] = (char*) malloc(sizeof(char) * 2);
				strcpy(tokensArr[n], "!\0");
				n++;
			}
			else {
				token = tokenizer(inputArray, i + 1, numChs, ',');
				i += strlen(token);
				tokensArr[n] = (char*) malloc(strlen(token) + 2);
				strcpy(tokensArr[n], token);
				free(token);
				n++;
			}
		}
		else if (lagCh == '"' && (leadCh != ',' && leadCh != '"')) {
			if (leadCh == '\0') {
				// TheMightyDucks //  This should be an error code
				tokensArr[n] = (char*) malloc(sizeof(char) * 2);
				strcpy(tokensArr[n], "!\0");
				n++;
			}
			else {
				token = tokenizer(inputArray, i + 1, numChs, '"');
				i += strlen(token);
				tokensArr[n] = (char*) malloc(strlen(token) + 2);
				strcpy(tokensArr[n], token);
				free(token);
				n++;
			}
		}
		// So we don't try to add more tokens than there are columns
		if (n == 28)
			i = numChs;
	}
	for (i = 0; i < 28; i++){
		trim(tokensArr[i]);
	}
	insert(tokensArr);
}

char* trim(char* trimToken) {
	if (trimToken == NULL)
		return;
	else if (strlen(trimToken) < 2)
		return;
	int k, m, n;
	k = 0;
	m = 0;
	n = 0;

	//front
	while (trimToken[k++] == ' ')
		;
	k--;

	//back
	n += strlen(trimToken) - 1;
	while (trimToken[n--] == ' ')
		;
	n++;

	// Increase efficiency in case there's no leading whitespaces
	if (k != m)
		while (trimToken[m + k] != '\0') {
			trimToken[m] = trimToken[m + k];
			m++;
		}

	// End the string
	trimToken[n - k + 1] = '\0';
	return trimToken;
}

