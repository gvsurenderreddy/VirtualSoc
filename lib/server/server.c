#include "vsoc.h"

static void *treat(void *arg);

int main(int argc, char *argv[])
{
	signal(SIGINT, (__sighandler_t)forcequit);

	int i = 0;
	pthread_t th[100];
	if (argc < 2)
	{
		fprintf(stderr, "Usage: ./server 1/0 (1 for normal, 0 for debugging)\n");
		exit(EXIT_FAILURE);
	}

	int rc = sqlite3_open("vsoc.db", &db);

	if (rc)
	{
		fprintf(stderr, "Can't open Database %s !\n", sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
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
			exit(EXIT_FAILURE);
		}

		if (listen(socketConnect, 10) == -1)
		{
			perror("[server]Socket listen error ! listen(). \n");
			printf("[server]Errno: %d", errno);
			exit(EXIT_FAILURE);
		}

		printf("[server]Waiting at port %d \n", PORT);
		fflush(stdout);
		thData *td; // initial in while, ptr free
		while (1)
		{
			int socketClient;

			int length = sizeof(from);

			if ((socketClient = accept(socketConnect, (struct sockaddr *)&from, (socklen_t * restrict) & length)) < 0)
			{
				perror("[server]Connection accept error ! "
					   "accept()");
				continue;
			}
			td = (struct thData *)malloc(sizeof(struct thData));

			td->idThread = i++;
			td->client = socketClient;

			if (pthread_create(&th[i], NULL, &treat, td) < 0)
			{
				perror("[server]Could not create thread !\n");
			}
		} // while - connection
		free(td);
		sqlite3_close(db);
		return 0;
	} //end of normal mode

	if (atoi(argv[1]) == 0)
	{ // debugging mode
		printf("[server]Debug Mode\n");


		sqlite3_close(db);
		return 0;
	}
}

static void *treat(void *arg)
{
	struct thData tdL;
	tdL = *((struct thData *)arg);
	printf("[thread %d]Serving client with conn. sock %d !\n", tdL.idThread, tdL.client);
	fflush(stdout);

	pthread_detach(pthread_self());
	answer((struct thData *)arg);

	printf("[thread]Closing connection for %d !\n", tdL.client);
	close(tdL.client);
	return (NULL);
}
