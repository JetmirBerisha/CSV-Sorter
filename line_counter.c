#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include "line_counter.h"

int line_counter(FILE * fp)
{
   int line_num = 0;
   char ch;
   ch = fgetc(fp);
   while(ch != EOF)
   {
	if(ch == '\n')
		line_num += 1;
	ch = fgetc(fp);
   }
   return line_num;
}   

unsigned int csv_count(char dir[])
{
    DIR *dp;
    struct dirent *entry;
    int i, j;
    unsigned int count = 0;
    struct stat statbuf;
    char extension[5];
    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"Cannot open directory: |%s|\n", dir);
        return -1;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            if(strcmp(".",entry->d_name) == 0
                    || strcmp("..",entry->d_name) == 0)
                continue;
            count += csv_count(entry->d_name);
        }
        else{
			i = 0;
			j = strlen(entry->d_name) - 4;
			if (j < 0)
				continue;
			for(i = 0; i < 4; i++){
				extension[i] = entry->d_name[j++];
			}
			extension[4] = 0;

			if (strcmp(extension, ".csv") == 0)
				count++;
		}
    }
    chdir("..");
    closedir(dp);

    return count;
}
