#include "vsoc.h"

int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		printf("Usage: %s xxx.xxx.xxx.xxx  \n", argv[0]);
		return EXIT_FAILURE;
	}

	//pregatire client
	cliPrepare();

	//conectam clientul la server cu ip argv[1]
	struct sockaddr_in server;
	int socketConnect;
	socketConnect = connTcpSock(argv[1], PORT, server);


	printf(RED "\n\n	Welcome to VirtualSoc ~ by Cristea Alexandru\n	/help  - "
			   " syntax and available commands !\n\n\n" RESET);

	int clientCommand = -1;
	bool logged = 0;
	char clientCommandChar[LONG_LEN], currentID[SHORT_LEN];

	//introducem comenzi
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


		if (read(0, clientCommandChar, LONG_LEN) < 0)
		{
			perror("read() error ! ");
			exit(EXIT_FAILURE);
		}

		if (clientCommandChar[0] == '\n')
		{
			continue;
		}


		clientCommandChar[strlen(clientCommandChar) - 1] = 0;
		clientCommand = encodeCommand(clientCommandChar);


		if (clientCommand == -1 && clientCommandChar[0] != 0)
		{
			printf("Wrong command ! Check /help !\n");
			continue;
		}

		if (clientCommand == 3)
		{
			help(logged);
			continue;
		}


		safeWrite(socketConnect, &clientCommand, sizeof(int));

		if (clientCommand == 0)
		{
			quit();
			break;
		}

		switch (clientCommand)
		{
		case 1:
			logged = login(socketConnect, currentID);
			break;

		case 2:
			logged = register_now(socketConnect);
			break;

		case 4:
			viewProfile(socketConnect);
			break;

		case 5:
			logged = logout(socketConnect, currentID);
			break;

		case 6:
			logged = addFriend(socketConnect);
			break;

		case 7:
			logged = addPost(socketConnect);
			break;

		case 8:
			logged = setProfile(socketConnect);
			break;

		case 9:
			logged = recvReq(socketConnect);
			break;

		case 10:
			logged = accFriend(socketConnect);
			break;

		case 12:
			logged = friends(socketConnect);
			break;

		case 13:
			logged = online(socketConnect);
			break;

		case 14:
			logged = createChat(socketConnect);
			break;

		case 15:
			logged = chat(socketConnect);
			break;

		case 16:
			logged = deleteChat(socketConnect);
			break;

		case 17:
			logged = joinChat(socketConnect);
			break;

		case 18:
			logged = setPriv(socketConnect);
			break;

		case 19:
			logged = deleteFriend(socketConnect);
			break;

		case 20:
			logged = deleteRecvReq(socketConnect);
			break;

		case 21:
			logged = sentReq(socketConnect);
			break;

		case 22:
			logged = deleteSentReq(socketConnect);
			break;

		case 23:
			logged = deleteUser(socketConnect);
			break;

		case 24:
			logged = deletePost(socketConnect);
			break;

		case 25:
			logged = wall(socketConnect);
			break;
		}

		memset(clientCommandChar, 0, SHORT_LEN);
	}
	close(socketConnect);
	return EXIT_SUCCESS;
}
