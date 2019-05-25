#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "relativeDir.h"

/**
 * Convers relative ./ directories to absolute ones
 */
char* singleDot(char* string){
	char* str = string;
	char* storeDir;
	if (str[0] == '.' && str[1] != '.' && (str[1] == '/' || str[1] == 0)) {
		storeDir = get_current_dir_name();
		int lenDir, lenArg, i;
		lenDir = strlen(storeDir);
		lenArg = strlen(str) - 1;
		int len = lenDir + strlen(str)+1;
		char temp[len];

		for (i = 0; i < lenDir; i++){
			temp[i] = storeDir[i];
		}
		temp[i] = '/';

		for (i = i + 1; i - lenDir < lenArg; i++){
			temp[i] = str[i-lenDir + 1];
		}
		temp[i] = 0;

		free(storeDir);
		storeDir = (char*) malloc(len*sizeof(char));
		strcpy(storeDir, temp);
		return storeDir;
	}
	else {
		storeDir = (char*) malloc(sizeof(char) * (strlen(str) + 1));
		strcpy(storeDir, str);
		return storeDir;
	}
}

/**
 * Convers relative ../ directories to absolute ones
 */
char* doubleDot(char* string){
	char* str = string;
	char* storeDir;
	size_t storeDirSize = 30;
	storeDir = (char*) malloc(sizeof(char)*storeDirSize);
	getcwd(storeDir, storeDirSize);

	if (str[0] == '.' && str[1] == '.' && (str[2] == '/' || str[2] == 0)){	/*		../		*/
		while(strlen(storeDir) == 0){
			storeDirSize = storeDirSize * 2;
			storeDir = (char*) realloc(storeDir, sizeof(char)*storeDirSize);
			getcwd(storeDir, storeDirSize);
		}
		int index = strlen(storeDir)-1;
		while ((index--) > 0) {
			if (storeDir[index] == '/'){
				storeDir[index + 1] = 0;
				return storeDir;
			}
		}
		return storeDir;
	}
	else {
		free(storeDir);
		storeDir = (char*) malloc(sizeof(char) * (strlen(str) + 1));
		strcpy(storeDir, str);
		return storeDir;
	}
}
