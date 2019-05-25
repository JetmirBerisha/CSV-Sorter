all: sorter_client sorter_server

sorter_client : main.o traverse.o relativeDir.o GlobalLL.o mergeSort.o usr_getline.o\
	tokenizer.o usr_realloc.o line_counter.o
	gcc -g -pthread -w -o sorter_client main.o traverse.o relativeDir.o GlobalLL.o mergeSort.o usr_getline.o\
		tokenizer.o usr_realloc.o line_counter.o


sorter_server : server.o GlobalLL.o mergeSort.o usr_getline.o\
	tokenizer.o usr_realloc.o
	gcc -g -pthread -w -o sorter_server server.o GlobalLL.o\
		mergeSort.o tokenizer.o usr_getline.o usr_realloc.o

server.o : server.c usr_getline.h tokenizer.h GlobalLL.h
	gcc -g -pthread -w -c server.c
main.o : main.c traverse.h relativeDir.h line_counter.h GlobalLL.h
	gcc -g -pthread -w -c main.c
traverse.o : traverse.c traverse.h tokenizer.h GlobalLL.h
	gcc -g -pthread -w -c traverse.c
GlobalLL.o : GlobalLL.c GlobalLL.h mergeSort.h
	gcc -g -pthread -w -c GlobalLL.c
relativeDir.o : relativeDir.c relativeDir.h
	gcc -g -pthread -w -c relativeDir.c
mergeSort.o : mergeSort.c mergeSort.h
	gcc -g -pthread -w -c mergeSort.c
tokenizer.o : tokenizer.c tokenizer.h GlobalLL.h
	gcc -g -pthread -w -c tokenizer.c
usr_getline.o : usr_getline.c usr_getline.h usr_realloc.h
	gcc -g -pthread -w -c usr_getline.c
usr_realloc.o : usr_realloc.c usr_realloc.h
	gcc -g -pthread -w -c usr_realloc.c
line_counter.o : line_counter.c line_counter.h
	gcc -g -pthread -w -c line_counter.c

clean :
	rm sorter_client sorter_server main.o server.o traverse.o GlobalLL.o relativeDir.o mergeSort.o tokenizer.o\
		usr_getline.o usr_realloc.o line_counter.o






