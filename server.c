#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include "usr_getline.h"
#include "mergeSort.h"
#include "tokenizer.h"
#include "GlobalLL.h"
#include "tokenizer.h"

#define BACKLOG 100

struct clientArgs {
	int socket;
};

int CSVcount = -3;
pthread_mutex_t wait_Fcsv;
pthread_cond_t is_set;

pthread_cond_t dump;
pthread_mutex_t make_ready;
char* portNumber;
pthread_mutex_t lock;
int num_connections = 0;

void* accept_clients(void *args);
void* service_client(void *args);

int main(int argc, char* argv[]) {
	pthread_mutex_init(&lock, 0);
	int errno, portL = 0;
	void* retval;
	char* port;

	pthread_mutex_init(&wait_Fcsv, 0);
	pthread_mutex_init(&make_ready, 0);
	pthread_cond_init(&dump, 0);
	pthread_cond_init(&is_set, 0);

	if (argc != 3) {
		fprintf(stderr, "\n Please accompany the command \"-p\" with a  port number. \n");
		return EXIT_FAILURE;
	}
	else if (strcmp(argv[1], "-p") != 0) {
		printf("\n Please input a port number \n");
		return EXIT_FAILURE;
	}
	else
		port = argv[2];

	portNumber = port;

	errno = 0;
	pthread_t server_thread;
	if (pthread_create(&server_thread, NULL, accept_clients, NULL) != 0) {
		fprintf(stderr, " \n ERROR: At pthread_create(server_thread/accept_clients); File:%s Line:%d\n");
		perror("Perror");
		return EXIT_FAILURE;
	}

	pthread_join(server_thread, NULL);
	pthread_mutex_destroy(&lock);
	return 0;
}

//Thread for listening and accepting clients
//This function passes new socket file_descriptors to new threads
//Which store/sort and dump
void *accept_clients(void *args) {

	int serversocket, clientsocket, optval;
	int errno;
	struct addrinfo hints, *res, *p;
	struct sockaddr_storage *clientAddr;
	socklen_t sin_size = sizeof(struct sockaddr_storage);
	struct sockaddr_in addrinfo;		///
	int len = sizeof(addrinfo);			///	IP of client
	char clientip[16] = { 0 };			///
	memset(&addrinfo, 0, len);
	pthread_t handler_thread;
	char buff[1024] = { 0 };

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; //INADDR_ANY for IPV4 or IPV6

	if (getaddrinfo(NULL, portNumber, &hints, &res) != 0) {
		perror("Server: \"getaddrinfo()\" failed. Perror");
		pthread_exit(NULL);
	}

	for (p = res; p != NULL; p = p->ai_next) {
		if ((serversocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("Server: Could not open socket.  Perror");
			continue;
		}
		optval = 1;
		if (setsockopt(serversocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
			perror("Server: setsockopt () failed. Perror");
			close(serversocket);
			continue;
		}
		if (bind(serversocket, p->ai_addr, p->ai_addrlen) == -1) {
			perror("Server: bind() failed. Perror");
			close(serversocket);
			continue;
		}
		if (listen(serversocket, BACKLOG) == -1) { // Need to change until sigkill
			perror("Server: listen() failed. Perror");
			close(serversocket);
			continue;
		}
		break; //if it gets here the socket is good to go stop iterating
	}
	fflush(stdout);
	freeaddrinfo(res);

	if (p == NULL) {
		fprintf(stderr, "Could not find a socket to bind to. \n");
		pthread_exit(NULL);
	}

	printf("\nReceived connections from: ");
	fflush(stdout);
	//ACCEPT CONNECTIONS
	while (1) {
		clientAddr = malloc(sin_size);
		if ((clientsocket = accept(serversocket, (struct sockaddr*) clientAddr, &sin_size)) == -1) {
			free(clientAddr);
			perror("Server: Could not accept() connection Perror");
			continue;
		}
		// Printing the IP of each connection
		getpeername(clientsocket, (struct sockaddr*) (&addrinfo), &len);
		inet_ntop(AF_INET, &(addrinfo.sin_addr), clientip, INET_ADDRSTRLEN);
		printf("%s, ", clientip);
		fflush(stdout);
		memset(clientip, 0, strlen(clientip));

		num_connections++;
		if (pthread_create(&handler_thread, NULL, service_client, clientsocket) != 0) {	//changed to client_sock instead of ca because ca was messing with the socket and the client_socket was getting changed when the sub-thread accessed it
			free(clientAddr);
			fprintf(stderr, "\n Could not create a handler_thread. File: %s Line: %d\n",
			__FILE__, __LINE__);
			perror("Server: pthread_create: Perror");
			close(clientsocket);
			close(serversocket);
			pthread_exit(0);
		}
		free(clientAddr);
	}
	pthread_exit(0);
}

void* service_client(void *args) {
	fflush(stdout);
	int client_socket, nbytes, endL, endF, line_length, sortCol, gjatsia, i, j;
	j = i = 0;
	int n = 0;
	char buf[2048], arg_buf[10];
	char* inputline;
	memset(buf, 0, sizeof(buf));
	node *other_head, *lead, *lag;
	int jfkd;
	client_socket = (int) args;
	int sockID = client_socket;
	pthread_detach(pthread_self()); /* detached makes sense but how to check if finished finish */
	//wait for pthread_exit with detached state?
	endL = 0;
	/* Read */
	fflush(stdout);
	if (readn(client_socket, buf, 5, __FILE__, sockID) == -1) { //Read from client_socket ^nnn^ numChars to read from line.
		fprintf(stderr, "An error occurred while reading.\n");
		close(client_socket);
		pthread_exit(0);
	}

	if (strcmp(buf, "^CSV^") == 0) {
		pthread_mutex_lock(&wait_Fcsv);
		memset(buf, 0, strlen(buf));
		if (readn(client_socket, buf, 5, __FILE__, sockID) == -1) { //Read from client_socket ^nnn^ numChars to read from line.
			fprintf(stderr, "An error occurred while reading.\n");
			close(client_socket);
			pthread_mutex_unlock(&wait_Fcsv);
			pthread_exit(0);
		}

		if (buf[0] == '^') { //if the first byte of the buffer is the char representing the beginning of the line_length instruction
			CSVcount = atoi(&buf[1]);
			jfkd = CSVcount + 2;
		}
		else {
			fprintf(stderr, "\n%s  FILE: %s LINE: %d \n", &buf, __FILE__, __LINE__);
			pthread_mutex_unlock(&wait_Fcsv);
			close(client_socket);
			pthread_exit(0);
		}

		while (jfkd >= 0) {
			pthread_cond_broadcast(&is_set);
			jfkd--;
		}
		pthread_mutex_unlock(&wait_Fcsv);

		memset(buf, 0, strlen(buf));
		if(readn(client_socket, buf, 5, __FILE__, sockID) == -1){
			fprintf(stderr, "An error occurred while reading.\n");
			close(client_socket);
			pthread_mutex_unlock(&wait_Fcsv);
			pthread_exit(0);
		}
	}

	if (strcmp(buf, "^010^") == 0) /* "^010^" ==  Store into global datastructure. */
	{
		pthread_mutex_lock(&wait_Fcsv);
		while (CSVcount == -3) {
			pthread_cond_wait(&is_set, &wait_Fcsv);
		}
		pthread_mutex_unlock(&wait_Fcsv);
		fflush(stdout);
		endL = 0;
		memset(buf, 0, strlen(buf));
		fflush(stdout);
		if (readn(client_socket, buf, 5, __FILE__, sockID) == -1) { //Read from client_socket ^nnn^ numChars to read from line.
			fprintf(stderr, "An error occurred while reading.\n");
			close(client_socket);
			pthread_exit(0);
		}

		endF = 0;
		while (!endF) {
			if (buf[0] == '^') { //if the first byte of the buffer is the char representing the beginning of the line_length instruction
				line_length = atoi(&buf[1]);
			}
			else {
				close(client_socket);
				pthread_exit(0);
			}
			while (!endL) {
				fflush(stdout);
				nbytes = readn(client_socket, buf, line_length, __FILE__, sockID);
				if (nbytes == line_length) /* The correct number of bytes were read for the line. */
				{
					//get the line from the buf
					inputline = malloc(line_length * sizeof(char) + 1);
					strcpy(inputline, buf);
					pthread_mutex_lock(&lock);
					tokzert(inputline);
					pthread_mutex_unlock(&lock);
					memset(buf, 0, strlen(buf)); /* Clear the buffer*/
					line_length = 5;
					fflush(stdout);
					nbytes = readn(client_socket, buf, line_length, __FILE__, sockID); /* Read for ^000^ */
					if (line_length == nbytes && strcmp(buf, "^000^") == 0) {
						endL = 1;
						memset(buf, 0, strlen(buf));
						if (readn(client_socket, buf, 5, __FILE__, sockID) == -1) { // ^000^ was read get the line length for the next loop starting at while(!endF)
							fprintf(stderr, "An error occurred while reading.\n");
							close(client_socket);
							pthread_exit(0);
						}
					}
					else if (strcmp(buf, "^0X0^") == 0 && line_length == nbytes) {
						endF = 1;
						endL = 1;
						memset(buf, 0, strlen(buf));
					}
					else {
						fprintf(stderr, "Error incorrect number of bytes returned. Something went wrong. \n");
						close(client_socket);
						pthread_exit(0);
					}
					free(inputline);
					fflush(stdout);
				}
				else {
					close(client_socket);
					pthread_exit(0);
				}
			}
			endL = 0;
		}
	}
	else if (strcmp(buf, "^020^") == 0) /*  "^020^" ==  Sort/Dump */
	{
		pthread_mutex_lock(&make_ready);
		while (CSVcount > 0) {
			pthread_cond_wait(&dump, &make_ready);
		}
		pthread_mutex_unlock(&make_ready);
		endF = 0;
		memset(buf, 0, strlen(buf));
		fflush(stdout);
		readn(client_socket, buf, 5, __FILE__, sockID); //get sortCol
		if (buf[0] == '^') {
			sortCol = atoi(&buf[1]);
			other_head = get_head();
			mergeSort(&other_head, sortCol);
			lead = other_head;
			fflush(stdout);
			while (lead != NULL && !endF) {
				lag = lead;
				lead = lead->next;
				memset(buf, 0, strlen(buf));
				strcpy(buf, lag->data[0]);
				for (i = 1; i < 28; i++) {
					strcat(buf, ",");
					if (i == 11) {
						gjatsia = strlen(lag->data[i]);
						while (j < gjatsia) {
							if (lag->data[i][j] == ',') {
								strcat(buf, "\"");
								strcat(buf, lag->data[i]);
								strcat(buf, "\"");
								break;
							}
							j++;
						}
						if (j < gjatsia)
							continue;
					}
					strcat(buf, lag->data[i]);
				}
				nbytes = 0;
				nbytes = strlen(buf); //If theres an error with not enough chars or some shit check here for null shit
				memset(arg_buf, 0, 10*sizeof(char));
				arg_buf[0] = '^';
				j = nbytes / 100;
				arg_buf[1] = j + '0';
				j = (nbytes % 100) / 10;
				arg_buf[2] = j + '0';
				j = (nbytes % 100) % 10;
				arg_buf[3] = j + '0';
				arg_buf[4] = '^';
				fflush(stdout);
				writen(client_socket, arg_buf, 5, __FILE__, sockID);
				fflush(stdout);
				writen(client_socket, buf, nbytes, __FILE__, sockID);
				// EOF control string
				if (lead == NULL)
					endF = 1;
				memset(arg_buf, 0, strlen(buf));
				if (endF) {
					arg_buf[0] = '^';
					arg_buf[1] = '0';
					arg_buf[2] = 'X';
					arg_buf[3] = '0';
					arg_buf[4] = '^';
					free_list();
				}
				else { /* \n */
					arg_buf[0] = '^';
					arg_buf[1] = '0';
					arg_buf[2] = '0';
					arg_buf[3] = '0';
					arg_buf[4] = '^';
				}
				fflush(stdout);
				writen(client_socket, arg_buf, 5, __FILE__, sockID);
			}
		}
		fflush(stdout);
	}
	close(client_socket);
	pthread_mutex_lock(&make_ready);
	CSVcount--;
	pthread_cond_signal(&dump);
	pthread_mutex_unlock(&make_ready);
	pthread_exit(0);
}

