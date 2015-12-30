#include "vsoc.h"

static void *treat(void *arg);

int main(int argc, char *argv[])
{
	signal(SIGINT, forcequit);
	int i = 0;
	pthread_t th[100];
	if (argc < 2)
	{
		fprintf(stderr, "Usage: ./server 1/0 (1 for normal, 0 for debugging)\n");
		exit(0);
	}
	// conectare DB

	rc = sqlite3_open("vsoc.db", &db);

	if (rc)
	{
		fprintf(stderr, "Can't open Database %s !\n", sqlite3_errmsg(db));
		exit(0);
	}
	else
	{
		fprintf(stdout, "[server]Opened database succesfully !\n");
	}

	if (atoi(argv[1]) == 1)
	{ // normal mode
		printf("[server]Normal Mode\n");
		struct sockaddr_in server;
		struct sockaddr_in from;
		memset(&server, 0, sizeof(server));
		memset(&from, 0, sizeof(from));
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = htonl(INADDR_ANY);
		server.sin_port = htons(PORT);

		int socketConnect = createConnSocketR();
		if (bind(socketConnect, (struct sockaddr *)&server,
				 sizeof(struct sockaddr)) == -1)
		{
			perror("[server]Socket bind error ! bind(). \n");
			printf("[server]Errno: %d", errno);
			exit(0);
		}

		if (listen(socketConnect, 10) == -1)
		{
			perror("[server]Socket listen error ! listen(). \n");
			printf("[server]Errno: %d", errno);
			exit(0);
		}

		printf("[server]Waiting at port %d \n", PORT);
		fflush(stdout);
		while (1)
		{
			int socketClient;
			thData *td;
			int length = sizeof(from);

			if ((socketClient = accept(socketConnect, (struct sockaddr *)&from, &length)) < 0)
			{
				perror("[server]Connection accept error ! "
					   "accept(). \n");
				continue;
			}
			//int idThread;
			//int client;
			td = (struct thData *)malloc(sizeof(struct thData));
			td->idThread = i++;
			td->client = socketClient;

			pthread_create(&th[i], NULL, &treat, td);
		} // while - connection
	}

	if (atoi(argv[1]) == 0)
	{ // debugging mode
		printf("[server]Debug Mode\n");

		sqlite3_close(db);
		return 0;
	}
	sqlite3_close(db);
	return 0;
}

static void *treat(void *arg)
{
	struct thData tdL;
	tdL = *((struct thData *)arg);
	printf("[thread]- Serving client %d \n", tdL.idThread);
	fflush(stdout);
	pthread_detach(pthread_self());
	answer((struct thData *)arg);
	close((int)arg);
	return (NULL);
}
