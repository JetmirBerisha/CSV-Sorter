#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "GlobalLL.h"
#include "mergeSort.h"

static node* head = NULL;
static char* headers[28];
static int times_run = 0;

void get_headers(char* record[]) {
	if (times_run == 0) {
		int i = 0;
		for (i = 0; i < 28; i++) {
			headers[i] = record[i];
		}
		times_run++;
	}
}

/* check head is To make sure head isn't NULL in main before calling
 * print_list() and free_list()
 */
int check_head() {
	int honesty = 0;
	if (head != NULL) {
		honesty += 1;
	}
	return honesty;
}

node* get_head(){
	return head;
}

int insert(char* record[]) {
	node* new_node;
	new_node = (node*) malloc(sizeof(node));
	new_node->data = (char**) malloc(28 * sizeof(char*));
	int copy = 0;
	for (copy = 0; copy < 28; copy++)
		new_node->data[copy] = record[copy];
	new_node->next = head;
	head = new_node;
}


int insertion_sort(char* record[], int sortCol, int dtype) {
	node *lead = head;
	node *prev = NULL;
	node *new_node = NULL;
	int copy = 0;
	int return_value = 1;
	int placed = 0; //boolean if node is placed do nothing else
	new_node = (node*) malloc(sizeof(node));
	for (copy = 0; copy < 28; copy++)
		new_node->data[copy] = record[copy];
	new_node->next = NULL;

	if (!placed && (lead == NULL)) { //if the head is null make head the first entry
		head = new_node; //set head equal to
		placed = 1;
	}

	if (!placed && (compare_type(new_node, lead, sortCol, dtype) <= 0)) { // if new_node->data < lead->data
		new_node->next = head;
		head = new_node;
		placed = 1;
	} 
	else {
		while ((!placed) && lead != NULL) {

			if (!placed && (compare_type(new_node, lead, sortCol, dtype) > 0)) { // if new_node->data > lead->data
				prev = lead;
				lead = lead->next;
			}
			else {
				prev->next = new_node;
				new_node->next = lead;
				placed = 1;
			}
		}
		if (!placed) {
			prev->next = new_node; //Inserts at end when lead == NULL
			placed = 1;
		}
	}
	if (placed)
		return_value = 1;
	else {
		return_value = 0;
		printf("\n Error: \n !!! 'placed' is 0, new_node->data = %s \n",
				new_node->data);
		printf(" !!! Was not inserted into the linked list! \n");
	}
	return return_value;
}

/*
 * prints list to the output_file specified
 */
void print_list(char* sortCol, char* path, int sortBy) {
	int x = 1;
	while (x == 1)
		x = mergeSort(&head, sortBy);
	int sizeof_sortcol = strlen(sortCol);
	char* output_filename = malloc(
			sizeof(char)
					* ((strlen("AllFiles-sorted-.csv") + sizeof_sortcol
							+ strlen(path)) + 1));
	strcpy(output_filename, path);
	strcat(output_filename, "AllFiles-sorted-");
	strcat(output_filename, sortCol);
	strcat(output_filename, ".csv");
	FILE* output_file = fopen(output_filename, "w");
	fprintf(output_file,
			"color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes\n");
	char* pptr = NULL;
	int length_token = 0;
	int i = 0;
	int j = 0;
	int gjatsia = 0;
	node* lead;
	lead = head;
	while (lead != NULL) {
		for (i = 0; i < 28; i++) {
			length_token = strlen(lead->data[i]);
			pptr = lead->data[i];
			gjatsia = length_token;
			for (j = 0; j < gjatsia; j++) {
				if (pptr[j] == ',') {
					fprintf(output_file, "\"%s\"", pptr);
					break;
				}
			}
			if (j == gjatsia)
				fprintf(output_file, "%s", pptr);
			if (i < 27)
				fprintf(output_file, ",");
		}
		fprintf(output_file, "\n");
		lead = lead->next;
	}
	free(output_filename);
	fclose(output_file);
}

void check_strcmp(char* new_node, char* lead) { //will print to the terminal strcmps results with interpretation.
	char* str;
	int value = -999;
	value = strcmp(new_node, lead);

	if (value < 0)
		str = "less than\0";
	else if (value == 0)
		str = "equal to\0";
	else if (value > 0)
		str = "greater than\0";
	else
		str = "unexpected\0";
	printf(
			"\n\"new_node, strcmp(new_node->data[sortCol], lead->data[sortCol]), lead\"\n");
	printf("\"new_node: %s, is %s,   lead: %s\"\n", new_node, str, lead);

}



/*
 * Hypothetically speaking this function should free each node and all of its
 * char* s
 */
void free_list() {
	if ((head == NULL)) {
		printf("WARNING: free_list called with head==NULL");
		return;
	}
	node* lead = head;
	node* willy;
	int i = 0;
	while (lead != NULL) {
		willy = lead;
		lead = lead->next;
		for (i = 0; i < 28; i++)
			free(willy->data[i]);
		if (willy != 0) {
			free(willy->data);
			free(willy);
		}
	}
	head = NULL;
}

/*
 * Compares ints, doubles, or strings, at specified sortCol and returns
 * a -1 for < and otherwise returns a 1 for >=
 */
int compare_type(node* new_node, node* lead, int sortCol, int dtype) {
	//dtype ---->   1 = int || 2 = double || 3 = string
	if (dtype == 1) { // sorting by ints
		int new_nodeInt = 0;
		int leadInt = 0;

		sscanf(new_node->data[sortCol], "%d", &new_nodeInt);
		sscanf(lead->data[sortCol], "%d", &leadInt);

		if (new_nodeInt < leadInt) // if new_node->data < lead->data
			return -1;
		else if (new_nodeInt == leadInt)
			return 0;
		else
			return 1; // if new_node->data >= lead->data
	} else if (dtype == 2) { //sorting by doubles
		double new_nodeDouble = 0;
		double leadDouble = 0;

		sscanf(new_node->data[sortCol], "%lf", &new_nodeDouble);
		sscanf(lead->data[sortCol], "%lf", &leadDouble);

		if (new_nodeDouble < leadDouble)
			return -1; // if new_node->data < lead->data
		else if (new_nodeDouble == leadDouble)
			return 0;
		else
			return 1;  // if new_node->data >= lead->data
	} 
	else { //sorting by strings
		char* new_nodeString = new_node->data[sortCol];
		char* leadString = lead->data[sortCol];
		return strcmp(new_nodeString, leadString);
	}
}
