#include "vsoc.h"

int getPassV2(const char *prompt, char *pass, int length)
{

	// disable echo
	nflags = oflags;
	nflags.c_lflag &= ~ECHO;
	nflags.c_lflag |= ECHONL;

	if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
	{
		perror("tcsetattr() error ! Exiting !\n");
		return EXIT_FAILURE;
	}

	//read password
	safeStdinRead(prompt, pass, length);

	//restore echo
	if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
	{
		perror("tcsetattr() error ! Exiting!\n");
		return EXIT_FAILURE;
	}
	return 0;
}

void safeStdinRead(const char *print, char *text, int size)
{
	fprintf(stdout, "%s", print);
	fflush(stdout);

	char buf[BUFSIZ];
	fgets(buf, sizeof(buf), stdin);

	strncpy(text, buf, (size_t)size);
	text[size] = 0;
	text[strlen(text) - 1] = 0;
	return;
}

ssize_t safeRead(int sock, void *buffer, size_t length)
{
	ssize_t nbytesR = read(sock, buffer, length);

	if (nbytesR == -1)
	{
		perror("write() error ! Exiting !\n");
		exit(EXIT_FAILURE);
	}

	return nbytesR;
}
ssize_t safeWrite(int sock, const void *buffer, size_t length)
{
	ssize_t nbytesW = write(sock, buffer, length);

	if (nbytesW == -1)
	{
		perror("write() error ! Exiting !\n");
		exit(EXIT_FAILURE);
	}

	return nbytesW;
}

ssize_t safePrefRead(int sock, void *buffer)
{
	size_t length = strlen(buffer);

	ssize_t nbytesR = read(sock, &length, sizeof(size_t));
	if (nbytesR == -1)
	{
		perror("read() error for length ! Exiting !\n");
		exit(EXIT_FAILURE);
	}

	nbytesR = read(sock, buffer, length);
	if (nbytesR == -1)
	{
		perror("read() error for data ! Exiting !\n");
		exit(EXIT_FAILURE);
	}

	return nbytesR;
}

ssize_t safePrefWrite(int sock, const void *buffer)
{
	size_t length = strlen(buffer);

	ssize_t nbytesW = write(sock, &length, sizeof(size_t));
	if (nbytesW == -1)
	{
		perror("write() error for length ! Exiting !\n");
		exit(EXIT_FAILURE);
	}

	nbytesW = write(sock, buffer, length);
	if (nbytesW == -1)
	{
		perror("write() error for data ! Exiting !\n");
		exit(EXIT_FAILURE);
	}

	return nbytesW;
}

int connTcpSock(const char *ip, int port, struct sockaddr_in server)
{
	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
	{
		perror("[client]socket() creation error ! \n");
		exit(errno);
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_port = htons(port);

	if (connect(sock, (struct sockaddr *)&server,
				sizeof(struct sockaddr)) == -1)
	{
		perror("[client]connect() error ! \n");
		exit(errno);
	}


	return sock;
}

void cliPrepare()
{
	signal(SIGINT, (__sighandler_t)quitforce);
	signal(SIGWINCH, NULL);
	tcgetattr(fileno(stdin), &oflags);

	return;
}

char *strlwr(char *command)
{
	size_t i;

	for (i = 0; i < strlen(command); i++)
		if (command[i] >= 'A' && command[i] <= 'Z')
			command[i] = command[i] + 32;

	return command;
}

int encodeCommand(char *clientCommandChar)
{
	if (strcmp(strlwr(clientCommandChar), "/login") == 0)
		return 1;
	if (strcmp(strlwr(clientCommandChar), "/register") == 0)
		return 2;
	if (strcmp(strlwr(clientCommandChar), "/quit") == 0)
		return 0;
	if (strcmp(strlwr(clientCommandChar), "/help") == 0)
		return 3;
	if (strcmp(strlwr(clientCommandChar), "/viewprofile") == 0)
		return 4;
	if (strcmp(strlwr(clientCommandChar), "/logout") == 0)
		return 5;
	if (strcmp(strlwr(clientCommandChar), "/addfriend") == 0)
		return 6;
	if (strcmp(strlwr(clientCommandChar), "/addfost") == 0)
		return 7;
	if (strcmp(strlwr(clientCommandChar), "/setprofile") == 0)
		return 8;
	if (strcmp(strlwr(clientCommandChar), "/recvreq") == 0)
		return 9;
	if (strcmp(strlwr(clientCommandChar), "/accfriend") == 0)
		return 10;
	if (strcmp(strlwr(clientCommandChar), "/friends") == 0)
		return 12;
	if (strcmp(strlwr(clientCommandChar), "/online") == 0)
		return 13;
	if (strcmp(strlwr(clientCommandChar), "/createchat") == 0)
		return 14;
	if (strcmp(strlwr(clientCommandChar), "/chat") == 0)
		return 15;
	if (strcmp(strlwr(clientCommandChar), "/deletechat") == 0)
		return 16;
	if (strcmp(strlwr(clientCommandChar), "/joinchat") == 0)
		return 17;
	if (strcmp(strlwr(clientCommandChar), "/setpriv") == 0)
		return 18;
	if (strcmp(strlwr(clientCommandChar), "/deletefriend") == 0)
		return 19;
	if (strcmp(strlwr(clientCommandChar), "/deleterecvreq") == 0)
		return 20;
	if (strcmp(strlwr(clientCommandChar), "/sentreq") == 0)
		return 21;
	if (strcmp(strlwr(clientCommandChar), "/deletesentreq") == 0)
		return 22;
	if (strcmp(strlwr(clientCommandChar), "/deleteuser") == 0)
		return 23;
	if (strcmp(strlwr(clientCommandChar), "/deletepost") == 0)
		return 24;
	if (strcmp(strlwr(clientCommandChar), "/wall") == 0)
		return 25;


	return -1;
}

__sighandler_t quitforce(void)
{
	tcsetattr(fileno(stdin), TCSANOW, &oflags);
	printf("\nForced Quit!\n");
	exit(EXIT_FAILURE);
}
