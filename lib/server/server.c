#include "vsoc.h"

static void *treat(void *arg);

int main(int argc, char *argv[])
{
	signal(SIGINT, (__sighandler_t)forcequit);
	signal(SIGPIPE, (__sighandler_t)forcequit);

	int i = 0;
	pthread_t th[100];
	thData *td;

	//pregatim serverul pentru conexiuni
	struct sockaddr_in server;
	struct sockaddr_in from;
	memset(&server, 0, sizeof(server));
	memset(&from, 0, sizeof(from));
	int socketConnect = servPrepare(PORT, server);

	fprintf(stdout, "[server]Waiting at port %d \n", PORT);

	while (true)
	{
		int socketClient = 0;
		int length = sizeof(from);

		//acceptam conexiuni una cate una
		if ((socketClient = accept(socketConnect, (struct sockaddr *)&from, (socklen_t * restrict) & length)) < 0)
		{
			perror("[server]Connection accept error ! "
				   "accept()");
			continue;
		}

		//cream un nou thread pentru fiecare conexiune
		td = (struct thData *)malloc(sizeof(struct thData));
		td->idThread = i++;
		td->client = socketClient;

		if (pthread_create(&th[i], NULL, &treat, td) < 0)
		{
			perror("[server]Could not create thread !\n");
			free(td);
		}
	}
	sqlite3_close(db);
	return EXIT_SUCCESS;
}

static void *treat(void *arg)
{
	struct thData tdL;
	tdL = *((struct thData *)arg);

	printf("[thread %d]Serving client with conn. sock %d !\n", tdL.idThread, tdL.client);
	fflush(stdout);

	pthread_detach(pthread_self());

	//deschidere baza de date

	if ((rc = sqlite3_open("vsoc.db", &db)) != 0)
	{
		fprintf(stderr, "Can't open Database %s !\n", sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}
	else
	{
		fprintf(stdout, "[thread %d]Opened database succesfully for %d !\n", tdL.idThread, tdL.client);
	}

	//raspunde clientului
	answer((struct thData *)arg);

	//inchidere conexiune
	printf("[thread]Closing connection & database for %d !\n", tdL.client);
	close(tdL.client);
	//free(arg);

	return (NULL);
}
