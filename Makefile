all:
	gcc -Wall server.c -pthread -l sqlite3 -o server
	gcc -Wall client.c -pthread -o client

clean:
	rm server client
