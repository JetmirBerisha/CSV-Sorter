#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <netdb.h>
#include <errno.h>
#include "traverse.h"
#include "relativeDir.h"
#include "line_counter.h"
#include "GlobalLL.h"

static node* head = NULL;
extern char* sortBy;
extern char* storeDir;
extern pthread_mutex_t lock;
extern pthread_mutex_t condLock;
extern pthread_cond_t waiton;
extern int* found;

int main(int argc, char* argv[]) {
	int i = 1; //must start off at 1

	int b = 1; //must start off at 1
	int count_argc, sockID;
	char* flag;
	char* field;
	char buffer[1024] = { 0 };
	char* sortCol = NULL;
	char* outputDir = NULL;
	char* startDir = NULL;
	int port = 0;
	char* hostName = 0;

	//for traverse & threads
	int fag, done, csvCount, numer = 0;
	csvCount = 0;
	done = 0;
	void* crap;
	char* dir;
	found = 0;
	pthread_t tid;

	count_argc = argc;
	if (argc < 3) {
		fprintf(stderr,
				"\n !!! 2 Command line arguments are the mandatory minimum.\n !!! You must include at least -c sort_column.\n\n");
		return 1;
	}
	else if (argc > 11) {
		fprintf(stderr,
				" \n ERROR: \n !!! There should be a max of 11 command line arguments.\n");
		fprintf(stderr,
				" !!! How many you put -------------------------------------> %d\n\n",
				count_argc);
		return 1;
	}
	else if (((argc - 1) % 2) != 0) {
		fprintf(stderr,
				"\n ERROR: \n !!! Incorrect combination of flags and args");
		int op = 1;
		char* argument;
		fprintf(stderr, " !!! You entered: ");
		while (op < argc) {
			argument = argv[op++];
			fprintf(stderr, "%s ", argument);
		}
		fprintf(stderr, "\n\n");
		return 1;
	}
	else {
		while (b < argc) {
			flag = argv[b++];
			field = argv[b++];
			if (strcmp(flag, "-c") == 0) {
				if (field[0] == '-') {
					fprintf(stderr,
							"\n ERROR \n !!! You cannot pass a flag as an argument to another flag. \n");
					fprintf(stderr, " !!! You entered: ");
					int op = 1;
					char* argument;
					while (op < argc) {
						argument = argv[op++];
						fprintf(stderr, "%s ", argument);
					}
					fprintf(stderr, "\n\n");
					return 1;
				}
				sortCol = field;
				if (sortCol == NULL) {
					fprintf(stderr,
							"\n ERROR: \n !!! How did you manage to get pas the error checking?\n !!! sortCol is NULL.\n\n");
					return 1;
				}
			}
			else if (strcmp(flag, "-o") == 0) {
				if (field[0] == '-') {
					fprintf(stderr,
							"\n ERROR \n !!! You cannot pass a flag as an argument to another flag. \n");
					fprintf(stderr, " !!! You entered: ");
					int op = 1;
					char* argument;
					while (op < argc) {
						argument = argv[op++];
						fprintf(stderr, "%s ", argument);
					}
					fprintf(stderr, "\n\n");
					return 1;
				}
				outputDir = field;
				if (outputDir == NULL) {
					fprintf(stderr,
							"\n ERROR \n !!! Uneven amount of commands passed to SorterEX\n !!! You typed \"-o\" without an output_dir\n\n");
					return 1;
				}
			}
			else if (strcmp(flag, "-d") == 0) {
				if (field[0] == '-') {
					fprintf(stderr,
							"\n ERROR \n !!! You cannot pass a flag as an argument to another flag. \n");
					fprintf(stderr, " !!! You entered: ");
					int op = 1;
					char* argument;
					while (op < argc) {
						argument = argv[op++];
						fprintf(stderr, "%s ", argument);
					}
					fprintf(stderr, "\n\n");
					return 1;
				}
				startDir = field;
				if (startDir == NULL) {
					fprintf(stderr,
							"\n ERROR \n !!! Uneven amount of commands passed to SorterEX\n !!! You typed  \"-d\" without a start_dir\n\n");
					return 1;
				}
			}
			else if (strcmp(flag, "-h") == 0) {
				if (field[0] == '-') {
					fprintf(stderr,
							"\n ERROR \n !!! You cannot pass a flag as an argument to another flag. \n");
					fprintf(stderr, " !!! You entered: ");
					int op = 1;
					char* argument;
					while (op < argc) {
						argument = argv[op++];
						fprintf(stderr, "%s ", argument);
					}
					fprintf(stderr, "\n\n");
					return 1;
				}
				if (field == 0) {
					fprintf(stderr, "Host name missing\n");
					return 1;
				}
				hostName = field;
			}
			else if (strcmp(flag, "-p") == 0) {
				if (field[0] == '-') {
					fprintf(stderr,
							"\n ERROR \n !!! You cannot pass a flag as an argument to another flag. \n");
					fprintf(stderr, " !!! You entered: ");
					int op = 1;
					char* argument;
					while (op < argc) {
						argument = argv[op++];
						fprintf(stderr, "%s ", argument);
					}
					fprintf(stderr, "\n\n");
					return 1;
				}
				if (field == 0) {
					fprintf(stderr, "Port number missing\n");
					return 1;
				}
				port = atoi(field);
				if (port == 0) {
					fprintf(stderr, "Wrong port format\n");
					return 1;
				}

			}
			else {
				fprintf(stderr,
						"\n ERROR \n !!! Error: \"%s\" is not a valid flag.\n");
				fprintf(stderr,
						" !!! See the example below and note that flags and args in [flags args] are optional. \n");
				fprintf(stderr,
						" !!! sh-4.2$ ./SortEX -c sort_column [-d start_dir] [-o output_dir]\n\n",
						flag);
				return 1;
			}
		}
	}
	if (sortCol == NULL) {
		fprintf(stderr,
				"\n ERROR \n !!! You must include -c sort_column for the program to run.\n");
		int op = 1;
		char* argument;
		fprintf(stderr, " !!! You entered: ");
		while (op < argc) {
			argument = argv[op++];
			fprintf(stderr, "%s ", argument);
		}
		fprintf(stderr, "\n\n");
		return 1;
	}
	if (outputDir == NULL) {
		outputDir = "./";
	}
	if (startDir == NULL) {
		startDir = "./";
	}
	if (port == 0) {
		fprintf(stderr, "Port number is required\n");
		return 1;
	}
	if (hostName == 0) {
		fprintf(stderr,
				"The host name is required: cannot remove ‘server.o’: No such file or directory.\n");
		return 1;
	}
	fflush(stdout);
	char* headers[] = { "color", "director_name", "num_critic_for_reviews",
			"duration", "director_facebook_likes", "actor_3_facebook_likes",
			"actor_2_name", "actor_1_facebook_likes", "gross", "genres",
			"actor_1_name", "movie_title", "num_voted_users",
			"cast_total_facebook_likes", "actor_3_name", "facenumber_in_poster",
			"plot_keywords", "movie_imdb_link", "num_user_for_reviews",
			"language", "country", "content_rating", "budget", "title_year",
			"actor_2_facebook_likes", "imdb_score", "aspect_ratio",
			"movie_facebook_likes" };
	int kl = 0;
	int bool = 0;
	for (kl = 0; kl < 28; kl++) {
		if (strcmp(headers[kl], sortCol) == 0) {
			bool = 1;
			break;
		}
	}
	if (bool == 0) {
		fprintf(stderr, "\n\n ERROR: \n Invalid column: You said %s\n\n",
				sortCol);
		return 0;
	}
	bool = kl;
	kl = 1;

	pthread_mutex_init(&lock, 0);
	pthread_mutex_init(&condLock, 0);
	pthread_cond_init(&waiton, 0);
	found = &done;
	sortBy = sortCol;
	setPort(port);
	setHost(hostName);
	storeDir = singleDot(outputDir);
	dir = singleDot(startDir);

	csvCount = csv_count(dir);
	if (csvCount == -1) {
		fprintf(stderr, " ERROR  \n");
		fprintf(stderr, " !!! ...\n");
		return 0;
	}
	setCsv(csvCount);

	pthread_mutex_lock(&condLock);
	while (done < csvCount) {
		if (fag == 0) {
			pthread_create(&tid, 0, (void*) (*traverseDir), dir);
			fag = 1;
		}
		pthread_cond_wait(&waiton, &condLock);
	}
	pthread_mutex_unlock(&condLock);
	pthread_join(tid, &crap);

	pthread_mutex_destroy(&lock);
	pthread_mutex_destroy(&condLock);
	pthread_cond_destroy(&waiton);

	b = 0;
	b = strlen(storeDir);
	if (storeDir[b - 1] != '/') { /* Add a / to the end of the path if it doesn't have it */
		storeDir = (char*) realloc(storeDir, b + 2);
		storeDir[b] = '/';
		storeDir[b + 1] = 0;
	}
	// Dump FILES
	FILE* fp;
	struct addrinfo hints;
	struct addrinfo* serverInfo;
	struct addrinfo *p;
	char lePort[6] = { 0 };
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	sprintf(lePort, "%d", port);

	if (getaddrinfo(hostName, lePort, &hints, &serverInfo)) {
		gai_strerror(errno);
		fprintf(stderr, "Getaddrinfo failed at %s:%d\n", __FILE__, __LINE__);
		//free(dir);
		return 0;
	}
	int sock;

	for (p = serverInfo; p != NULL; p = p->ai_next) {
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			fprintf(stderr, "Failed to create a socket at %s:%d\n", __FILE__,
			__LINE__);
			perror("Could not open socket");
			continue;
		}

		if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
			perror("Error ");
			close(sock);
			fprintf(stderr, "Failed to connect at %s:%d\n", __FILE__, __LINE__);
			continue;
		}

		break;
	}
	sockID = sock;

	memset(buffer, 0, 1024);
	freeaddrinfo(serverInfo);
	memset(buffer, 0, strlen(buffer));
	buffer[0] = '^';
	buffer[1] = '0';
	buffer[2] = '2';
	buffer[3] = '0';
	buffer[4] = '^';
	fflush(stdout);
	writen(sock, buffer, 5, __FILE__, sockID);

	memset(buffer, 0, strlen(buffer));
	buffer[0] = '^';
	buffer[1] = '0';
	b = bool / 10;
	buffer[2] = b + '0';
	b = bool % 10;
	buffer[3] = b + '0';
	buffer[4] = '^';
	fflush(stdout);
	writen(sock, buffer, 5, __FILE__, sockID);

	/*
	 * The printing of the file
	 */
	storeDir = (char*) realloc(storeDir,
			sizeof(char)
					* (strlen("AllFiles-sorted-.csv") + strlen(sortBy)
							+ strlen(storeDir) + 1));
	strcat(storeDir, "AllFiles-sorted-");
	strcat(storeDir, sortBy);
	strcat(storeDir, ".csv");

	fp = fopen(storeDir, "w+");
	if (fp == 0) {
		perror("Error");
		fprintf(stderr, "Failed at %s:%d\n", __FILE__, __LINE__ - 4);
		free(storeDir);
		freeaddrinfo(serverInfo);
		return 1;
	}
	int nbytes, endF, endL, numChars;
	endF = endL = 0;
	while (!endF) {
		if (kl) {
			fprintf(fp, "%s%s%s%s%s",
					"color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,",
					"actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,",
					"cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,",
					"num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,",
					"imdb_score,aspect_ratio,movie_facebook_likes");
			fprintf(fp, "\n");
			kl = 0;
		}
		fflush(stdout);
		if (readn(sock, buffer, 5, __FILE__, sockID) == -1) {
			fprintf(stderr, "An error occurred while reading.\n");
			fclose(fp);
			close(sock);
			freeaddrinfo(serverInfo);
			free(storeDir);
			return 0;
		}
		if (buffer[0] == '^') {
			numChars = atoi(&buffer[1]);
			memset(buffer, 0, strlen(buffer));
		}
		else {
			fprintf(stderr, "File: %s Line: %d\n", __FILE__, __LINE__);
			fclose(fp);
			close(sock);
			freeaddrinfo(serverInfo);
			free(storeDir);
			return 0;
		}
		while (!endF) {
			fflush(stdout);
			nbytes = readn(sock, buffer, numChars, __FILE__, sockID);
			if (nbytes == numChars) /* The correct number of bytes were read for the line. */
			{
				fprintf(fp, "%s", buffer);
				memset(buffer, 0, strlen(buffer));
				numChars = 5;
				fflush(stdout);
				nbytes = readn(sock, buffer, numChars, __FILE__, sockID); /* Read for ^000^ */
				if (numChars == nbytes && strcmp(buffer, "^000^") == 0) {
					fprintf(fp, "\n");
					endL = 1;
				}
				else if (numChars == nbytes && strcmp(buffer, "^0X0^") == 0) {
					endF = 1;
					endL = 1;
				}
				else {
					fprintf(stderr,
							"Error incorrect number of bytes returned. Something went wrong. \n");
					fclose(fp);
					close(sock);
					freeaddrinfo(serverInfo);
					free(storeDir);
					return 0;
				}
				fflush(stdout);
				if (!endF && readn(sock, buffer, 5, __FILE__, sockID) == -1) {
					fprintf(stderr, "An error occurred while reading.\n");
					fclose(fp);
					close(sock);
					freeaddrinfo(serverInfo);
					free(storeDir);
					return 0;
				}
				else if (!endF && buffer[0] == '^') {
					numChars = atoi(&buffer[1]);
					memset(buffer, 0, strlen(buffer));
				}
				else if (!endF) {
					fprintf(stderr,
							"Error incorrect number of bytes returned. Something went wrong. \n");
					fclose(fp);
					close(sock);
					freeaddrinfo(serverInfo);
					free(storeDir);
					return 0;
				}
			}
			else {
				fprintf(stderr,
						"Error receiving EOL command. Something went wrong. \n");
				fclose(fp);
				close(sock);
				freeaddrinfo(serverInfo);
				free(storeDir);
				return 0;
			}
			fflush(stdout);
		}
		endL = 0;
	}

	fclose(fp);
	close(sock);
	free(storeDir);
	return 0;
}
