					Basic Data Sorter - Server/Client

		Run “make” to use the makefile, which compiles and creates the executable sorter_server sorter_client files

This project reads in a directory of csv files (including subdirectories) and sends them over to a server where the server sorts them and returns one large sorted file as per client's demand. The template of the csv-s are as follows:
"color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes"
for a total of 28 columns. The program is able to sort on any of the columns.

Usage:	./sorter_client -p [port] -d [directory of csv-s] -o [output directory] -c [column to sort by] -h [host/server address]
		./sorter_server -p [port]
Design
	The client sends the raw lines of the CSV files to the server, trim/tokenize in the server, adding the line to a linked list consisting of all CSV lines, sorting the linked list in the server, then returning the sorted lines back to the client.
	Sorting arrangement is entirely up to strcmp(), except for fields that are integers or floating points.


