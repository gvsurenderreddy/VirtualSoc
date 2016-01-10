#include "vsoc.h"

int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		printf("Usage: %s xxx.xxx.xxx.xxx  \n", argv[0]);
		return EXIT_FAILURE;
	}

	cliPrepare();

	struct sockaddr_in server;
	int socketConnect;
	socketConnect = connTcpSock(argv[1], PORT, server);

	int clientCommand = -1;
	bool logged = 0;
	char clientCommandChar[65], currentID[33];

	printf(RED "\n\n	Welcome to VirtualSoc ~ by Cristea Alexandru\n	/help  - "
			   " syntax and available commands !\n\n\n" RESET);


	while (true)
	{

		if (logged == false)
		{
			printf(RED "[offline] @ vSoc >>: " RESET);
			fflush(stdout);
		}
		if (logged == true)
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

		case 24:
			deletePost(socketConnect, logged);
			break;

		case 25:
			wall(socketConnect, logged);
			break;
		}
	}
	close(socketConnect);
	return 0;
}
