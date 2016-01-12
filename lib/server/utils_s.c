#include "vsoc.h"

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

int servPrepare(int port, struct sockaddr_in server)
{
	int sock, on = 1;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("[server] Socket creation error ! socket(). \n");
		printf("[server] Errno: %d", errno);
		exit(EXIT_FAILURE);
	}

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	if (bind(sock, (struct sockaddr *)&server,
			 sizeof(struct sockaddr)) == -1)
	{
		perror("[server]Socket bind error ! bind(). \n");
		printf("[server]Errno: %d", errno);
		exit(EXIT_FAILURE);
	}

	if (listen(sock, 10) == -1)
	{
		perror("[server]Socket listen error ! listen(). \n");
		printf("[server]Errno: %d", errno);
		exit(EXIT_FAILURE);
	}

	return sock;
}

bool isValidChar(char elem)
{
	// lowercase, uppercase, underline, dot
	return ((elem >= 0x30 && elem <= 0x39) || // numbers
			(elem >= 0x61 && elem <= 0x7a) || // lower case
			(elem >= 41 && elem <= 0x5a) || // upper case
			(elem == 0x5f || elem == 0x2e)); // _ .
}

__sighandler_t forcequit(void)
{
	printf("[server]Force quit !\n");
	//dbForceQuit();
	exit(EXIT_SUCCESS);
}
