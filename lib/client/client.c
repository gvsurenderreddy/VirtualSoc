#include "vsoc.h"

int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		printf("Usage: %s xxx.xxx.xxx.xxx  \n", argv[0]);
		return EXIT_FAILURE;
	}

	int socketConnect;
	if ((socketConnect = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("[client]socket() creation error ! \n");
		return errno;
	}


	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(PORT);

	if (connect(socketConnect, (struct sockaddr *)&server,
				sizeof(struct sockaddr)) == -1)
	{
		perror("[client]connect() error ! \n");
		return errno;
	}

	signal(SIGINT, (__sighandler_t)quitforce);
	signal(SIGWINCH, NULL);
	tcgetattr(fileno(stdin), &oflags);

	//##############################CLIENT##################################


	int clientCommand = -1;
	char clientCommandChar[65];
	char currentID[33];
	bool logged = 0;

	printf(RED "\n\n	Welcome to VirtualSoc ~ by Cristea Alexandru\n	/help  - "
			   " syntax and available commands !\n\n\n" RESET);
	while (1)
	{

		if (logged == 0)
		{
			printf(RED "[offline] @ vSoc >>: " RESET);
			fflush(stdout);
		}
		if (logged == 1)
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

		/*
        safeStdinRead("", clientCommandChar, 65);

        if (clientCommandChar[0] == 0)
        {
            continue;
        }
		*/

		clientCommand = encodeCommand(clientCommandChar);


		if (clientCommand == -1 && clientCommandChar[0] != 0)
		{
			printf("Wrong command ! Check /help ! \nCommands are "
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
			recvReq(socketConnect, logged);
			break;

		case 10:
			accFriend(socketConnect, logged);
			break;

		case 12:
			friends(socketConnect, logged);
			break;

		case 13:
			logged = online(socketConnect);
			break;

		case 14:
			createChat(socketConnect, logged);
			break;

		case 15:
			chat(socketConnect, logged);
			break;

		case 16:
			deleteChat(socketConnect, logged);
			break;

		case 17:
			joinChat(socketConnect, logged);
			break;

		case 18:
			setPriv(socketConnect, logged);
			break;

		case 19:
			deleteFriend(socketConnect, logged);
			break;

		case 20:
			deleteRecvReq(socketConnect, logged);
			break;

		case 21:
			sentReq(socketConnect, logged);
			break;

		case 22:
			deleteSentReq(socketConnect, logged);
			break;

		case 23:
			deleteUser(socketConnect, logged);
			break;
		}
	}
	close(socketConnect);
	return 0;
}
