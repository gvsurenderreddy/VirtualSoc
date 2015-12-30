#include "vsoc.h"

int main(int argc, char *argv[])
{

	if (argc != 3)
	{
		printf("[client] Run:   %s ServerAddr Port \n", argv[0]);
		return -1;
	}
	int socketConnect;
	if ((socketConnect = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("[client]Socket creation error ! socket(). \n");
		return errno;
	}

	port = atoi(argv[2]);
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(port);

	if (connect(socketConnect, (struct sockaddr *)&server,
				sizeof(struct sockaddr)) == -1)
	{
		perror("[client]Connect error ! connect(). \n");
		return errno;
	}

	//##############################CLIENT##################################
	signal(SIGINT, (__sighandler_t)quitforce);
	int clientCommand = -1;
	char clientCommandChar[1000];
	char currentID[33];
	int logged = 0;
	printf(RED "	Welcome to VirtualSoc ~ by Cristea Alexandru\n	/help  - "
			   " syntax and available commands !\n\n\n" RESET);
	while (1)
	{

		if (logged == 0)
		{
			printf(RED "[offline] @ vSoc >>: " RESET);
			fflush(stdout);
		}
		else if (logged == 1)
		{
			printf(GREEN "%s @ vSoc >>: " RESET, currentID);
			fflush(stdout);
		}


		memset(clientCommandChar, 0, sizeof(clientCommandChar));
		read(0, clientCommandChar, sizeof(clientCommandChar));
		if (clientCommandChar[0] == '\n')
		{
			continue;
		}
		clientCommandChar[strlen(clientCommandChar) - 1] = 0;


		clientCommand = encodeCommand(clientCommandChar);
		if (clientCommand == -1)
		{
			printf("Wrong command ! Check /help ! \n Commands are "
				   "case-sensitive. \n");
			continue;
		}
		if (clientCommand == 3)
		{
			help(logged);
			continue;
		}

		if (write(socketConnect, &clientCommand, sizeof(int)) <= 0)
		{
			perror("[client]Write error to server ! \n");
			return errno;
		}

		if (clientCommand == 0)
		{
			quit(socketConnect, logged);
			break;
		}

		switch (clientCommand)
		{
		case 1:
			logged = login(socketConnect, logged, currentID);
			break;

		case 2:
			register_now(socketConnect, logged);
			break;

		case 4:
			viewProfile(socketConnect, logged);
			break;

		case 5:
			logged = logout(socketConnect, logged, currentID);
			break;

		case 6:
			addFriend(socketConnect, logged);
			break;

		case 7:
			addPost(socketConnect, logged);
			break;

		case 8:
			setProfile(socketConnect, logged);
			break;

		case 9:
			checkReq(socketConnect, logged);
			break;

		case 10:
			accFriend(socketConnect, logged);
			break;

		case 11:
			accChat(socketConnect, logged);
			break;

		case 12:
			friends(socketConnect, logged);
			break;

		case 13:
			online(socketConnect, logged);
			break;
		}
	}
	close(socketConnect);
	return 0;
}
