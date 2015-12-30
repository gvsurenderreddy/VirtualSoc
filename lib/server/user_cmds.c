#include "vsoc.h"

ssize_t safeRead(int sock, void *buffer, size_t length)
{
	ssize_t nbytesR = read(sock, buffer, length);

	if (nbytesR == -1)
	{
		perror("read() error ! Exiting !\n");
		exit(EXIT_FAILURE);
	}

	return length;
}
ssize_t safeWrite(int sock, const void *buffer, size_t length)
{
	ssize_t nbytesW = write(sock, buffer, length);

	if (nbytesW == -1)
	{
		perror("read() error ! Exiting !\n");
		exit(EXIT_FAILURE);
	}

	return length;
}

ssize_t safePrefRead(int sock, void *buffer)
{
	int length = strlen(buffer);

	ssize_t nbytesR = read(sock, &length, sizeof(int));
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

	return length;
}

ssize_t safePrefWrite(int sock, const void *buffer)
{
	int length = strlen(buffer);

	ssize_t nbytesW = write(sock, &length, sizeof(int));
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

	return length;
}

int createConnSocketR()
{
	int sd;
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("[server] Socket creation error ! socket(). \n");
		printf("[server] Errno: %d", errno);
		exit(0);
	}
	int on = 1;
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	return sd;
}

void login(int sC, char *currentUser)
{
	int resultAnswer = 11;
	int check;
	char id[33], password[33];
	memset(id, 0, 33);
	memset(password, 0, 33);

	read(sC, &check, sizeof(int));
	if (check == 1)
		return;
	else
	{
		read(sC, id, sizeof(id));
		read(sC, password, sizeof(password));

		if ((strchr(id, '\"') != NULL || strchr(password, '\"') != NULL))
		{
			resultAnswer = 103;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbLogCheckUser(id) == 0)
		{
			resultAnswer = 101;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbLogCheck(id, password) == 0)
		{
			resultAnswer = 102;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbSetOnline(id) == 0)
		{
			resultAnswer = 104;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}

		write(sC, &resultAnswer, sizeof(int));
		strcpy(currentUser, id);
		return;
	}
}

void register_now(int sC)
{
	int resultAnswer = 22;
	int check;
	char id[33], password[33], fullname[65], sex[5], about[513], type[17];

	memset(id, 0, 33);
	memset(password, 0, 33);
	memset(fullname, 0, 33);
	memset(sex, 0, 5);
	memset(about, 0, 33);
	memset(type, 0, 33);

	read(sC, &check, sizeof(int));
	if (check == 1)
		return;
	{
		read(sC, id, sizeof(id));
		read(sC, password, sizeof(password));
		read(sC, fullname, sizeof(fullname));
		read(sC, sex, sizeof(sex));
		read(sC, about, sizeof(about));
		read(sC, type, sizeof(type));

		/*    printf("Am primit : '%s'\n'%s'\n'%s'\n'%s'\n'%s'\n'%s'\n",
       id,
               password, fullname, sex, about, type);
        fflush(stdout);*/

		int i;

		if (strlen(id) < 10)
		{
			resultAnswer = 201;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		else
			for (i = 0; i < strlen(id); i++)
			{
				if (!(id[i] >= 'a' && id[i] <= 'z'))
					if (!(id[i] >= 'A' && id[i] <= 'Z'))
						if (!(id[i] >= '0' && id[i] <= '9'))
							if (!(id[i] == '_' || id[i] == '.'))
							{
								resultAnswer = 201;
								write(sC, &resultAnswer, sizeof(int));
								return;
							}
			}

		for (i = 0; i < strlen(fullname); i++)
		{
			if (!(fullname[i] >= 'a' && fullname[i] <= 'z'))
				if (!(fullname[i] >= 'A' && fullname[i] <= 'Z'))
					if (!(fullname[i] >= '0' && fullname[i] <= '9'))
						if (!(fullname[i] == ' ' || fullname[i] == '.' || fullname[i] == '-'))
						{
							resultAnswer = 203;
							write(sC, &resultAnswer, sizeof(int));
							return;
						}
		}
		if (strlen(password) < 10)
		{
			resultAnswer = 202;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}

		for (i = 0; i < strlen(password); i++)
		{
			if (password[i] == '\"')
			{
				resultAnswer = 208;
				write(sC, &resultAnswer, sizeof(int));
				return;
			}
		}
		for (i = 0; i < strlen(about); i++)
		{
			if (about[i] == '\"')
			{
				resultAnswer = 208;
				write(sC, &resultAnswer, sizeof(int));
				return;
			}
		}

		if (strlen(fullname) < 10)
		{
			resultAnswer = 203;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		if ((strlen(sex) != 1) || (strlen(sex) == 1 && sex[0] != 'F' && sex[0] != 'M'))
		{
			resultAnswer = 204;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		if (strlen(about) < 10)
		{
			resultAnswer = 205;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		if (strcmp(type, "private") != 0 && strcmp(type, "public") != 0)
		{
			resultAnswer = 206;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		if (dbRegCheckUser(id) != 0)
		{
			resultAnswer = 207;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
	}

	dbInsertUser(id, password, fullname, sex, about, type);
	write(sC, &resultAnswer, sizeof(int));
	return;
}

void viewProfile(int sC)
{
	int resultAnswer = -1;
	int check;
	char user[32];
	memset(user, 0, 32);

	read(sC, &check, sizeof(int));
	read(sC, user, sizeof(user));
	// verifica daca user-ul ce face view este logat
	// 44 - logat si poate vedea orice
	// 401 - nelogat, poate vedea doar publicele
	if (check == 0)
		resultAnswer = 401;
	if (check == 1)
		resultAnswer = 44;
	write(sC, &resultAnswer, sizeof(int));
	if (resultAnswer == 44)
		write(sC, &user, sizeof(user));
}

void logout(int sC, char *currentUser)
{
	int check;
	read(sC, &check, sizeof(int));

	if (check == 1)
	{
		dbSetOffline(currentUser);
	}
	return;
}

void addFriend(int sC, char *currentUser)
{
	int resultAnswer = 66, check;
	char user[33], friendType[33];
	memset(user, 0, 33);
	memset(friendType, 0, 33);

	read(sC, &check, sizeof(int));
	if (check == 0)
	{
		return;
	}
	else
	{
		read(sC, user, sizeof(user));
		read(sC, friendType, sizeof(friendType));

		switch (atoi(friendType))
		{
		case 1:
		case 2:
		case 3:
		{
			resultAnswer = 66;
			break;
		}
		default:
		{
			resultAnswer = 603;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		}


		if (strcmp(user, currentUser) == 0)
		{
			resultAnswer = 604;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		if (dbLogCheckUser(user) == 0 || strchr(user, '\"') != NULL)
		{
			resultAnswer = 601;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		if (dbFriendCheck(currentUser, user) != 0)
		{
			resultAnswer = 602;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		if (dbRequestCheckType(currentUser, user, "1") != 0)
		{
			resultAnswer = 605;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		if (dbRequestCheckType(user, currentUser, "1") != 0)
		{
			resultAnswer = 606;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}

		dbRequestSend(currentUser, user, "1", friendType);
		write(sC, &resultAnswer, sizeof(int));
		return;
	}
}

void addPost(int sC, char *currentUser)
{
	int resultAnswer = 77, check;
	char post[513], posttype[5];
	memset(post, 0, 513);
	memset(posttype, 0, 5);

	safeRead(sC, &check, sizeof(int));

	if (check == 0)
	{
		return;
	}
	else
	{
		safePrefRead(sC, post);
		safePrefRead(sC, posttype);

		switch (atoi(posttype))
		{
		case 1:
		case 2:
		case 3:
			resultAnswer = 77;
			break;
		default:
			resultAnswer = 701;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (strlen(post) < 10 || strchr(post, '\"') != NULL)
		{
			resultAnswer = 702;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		safeWrite(sC, &resultAnswer, sizeof(int));
		dbInsertPost(currentUser, post, posttype);
	}

	return;
}

void setProfile(int sC, char *currentUser)
{
	//int resultAnswer = 88;
	int check;
	read(sC, &check, sizeof(int));
	if (check == 0)
	{
		return;
	}
	else
	{
	}
	return;
}

void checkReq(int sC, char *currentUser)
{
	int resultAnswer = 99, check;

	read(sC, &check, sizeof(int));
	if (check == 0)
	{
		return;
	}
	else
	{
		int requestsCount = dbRequestCheckCount(currentUser);

		if (requestsCount == 0)
		{
			resultAnswer = 901;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		write(sC, &resultAnswer, sizeof(int));
		write(sC, &requestsCount, sizeof(int));

		dbRequestCheck(currentUser, sC, 35);
	}
	return;
}

void accFriend(int sC, char *currentUser)
{
	int resultAnswer = 1010, check;
	char user[33], friendType[33];

	memset(user, 0, 33);
	memset(friendType, 0, 33);

	read(sC, &check, sizeof(int));
	if (check == 0)
	{
		return;
	}
	else
	{

		read(sC, user, sizeof(user));
		read(sC, friendType, sizeof(friendType));

		switch (atoi(friendType))
		{
		case 1:
		case 2:
		case 3:
		{
			resultAnswer = 1010;
			break;
		}
		default:
		{
			resultAnswer = 1001;
			break;
		}
		}


		if (strcmp(user, currentUser) == 0)
		{
			resultAnswer = 1002;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		if (dbLogCheckUser(user) == 0 || strchr(user, '\"') != NULL)
		{
			resultAnswer = 1003;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		if (dbFriendCheck(currentUser, user) != 0)
		{
			resultAnswer = 1004;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}
		if (dbRequestCheckType(user, currentUser, "1") == 0)
		{
			resultAnswer = 1005;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}

		dbInsertFriend(currentUser, user, friendType);
		dbInsertFriend(user, currentUser, dbGetFTypeFromReq(user, currentUser));
		dbDeleteRequestType(currentUser, user, "1");
		dbDeleteRequestType(user, currentUser, "1");
		write(sC, &resultAnswer, sizeof(int));
		return;
	}
	return;
}



void accChat(int sC, char *currentUser)
{
	//int resultAnswer = 1111;
	int check;

	read(sC, &check, sizeof(int));

	if (check == 0)
	{
		return;
	}
	else
	{
	}
	return;
}

void friends(int sC, char *currentUser)
{
	int resultAnswer = 1212, check;
	;
	char user[33];
	memset(user, 0, 33);

	read(sC, &check, sizeof(int));

	if (check == 0)
	{
		return;
	}
	else
	{
		read(sC, user, sizeof(user));


		if (strchr(user, '\"') != NULL)
		{
			resultAnswer = 1201;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbLogCheckUser(user) == 0)
		{
			resultAnswer = 1201;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbFriendCheck(currentUser, user) == 0 && strcmp(currentUser, user) != 0)
		{
			resultAnswer = 1202;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}

		int friendsCount = dbFriendsCount(user);

		if (friendsCount == 0)
		{
			resultAnswer = 1203;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}



		write(sC, &resultAnswer, sizeof(int));
		write(sC, &friendsCount, sizeof(int));

		dbFriends(user, sC, 35);
	}

	return;
}

void online(int sC, char *currentUser)
{
	int resultAnswer = 1313, check, onlineCount;

	read(sC, &check, sizeof(int));

	if (check == 0)
	{
		return;
	}
	else
	{
		onlineCount = dbOnlineCount(currentUser);

		if (onlineCount == 0)
		{
			resultAnswer = 1301;
			write(sC, &resultAnswer, sizeof(int));
			return;
		}

		write(sC, &resultAnswer, sizeof(int));
		write(sC, &onlineCount, sizeof(int));

		dbOnline(currentUser, sC, 33);
	}
	return;
}



void quit(int sC, char *currentUser)
{
	int check;
	read(sC, &check, sizeof(int));
	if (check == 1)
	{
		dbSetOffline(currentUser);
	}
	return;
}

__sighandler_t forcequit(void)
{
	printf("[server] Force quit !\n");
	dbForceQuit();
	exit(0);
}

void answer(void *arg)
{
	struct thData tdL;
	tdL = *((struct thData *)arg);

	int clientCommand = -1;
	char clientID[33];
	memset(clientID, 0, 33);

	while (clientCommand != 0)
	{
		if (read(tdL.client, &clientCommand, sizeof(int)) <= 0)
		{
			printf("[thread %d]Client with conn. sock %d - ", tdL.idThread, tdL.client);
			fflush(stdout);
			perror("read() error or forced disconnect");
			dbSetOffline(clientID);
			break;
		}

		printf("[thread %d]Received command : %d\n", tdL.idThread, clientCommand);

		switch (clientCommand)
		{
		case 0:
			quit(tdL.client, clientID);
			break;

		case 1:
			login(tdL.client, clientID);
			break;

		case 2:
			register_now(tdL.client);
			break;

		case 4:
			viewProfile(tdL.client);
			break;

		case 5:
			logout(tdL.client, clientID);
			break;

		case 6:
			addFriend(tdL.client, clientID);
			break;

		case 7:
			addPost(tdL.client, clientID);
			break;

		case 8:
			setProfile(tdL.client, clientID);
			break;

		case 9:
			checkReq(tdL.client, clientID);
			break;

		case 10:
			accFriend(tdL.client, clientID);
			break;

		case 11:
			accChat(tdL.client, clientID);
			break;

		case 12:
			friends(tdL.client, clientID);
			break;

		case 13:
			online(tdL.client, clientID);
			break;
		}
	}
}
