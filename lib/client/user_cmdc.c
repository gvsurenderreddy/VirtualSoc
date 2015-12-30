#define CLIENT 1
#include "vsoc.h"

void myRead(char *text, int size)
{
	char buf[BUFSIZ];
	fgets(buf, sizeof(buf), stdin);
	strncpy(text, buf, (size_t)size);
	text[size] = 0;
	text[strlen(text) - 1] = 0;
}

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

ssize_t safePrefRead(int sock, void *buffer)
{
	int length = strlen(buffer);

	ssize_t nbytesW = read(sock, &length, sizeof(int));
	if (nbytesW == -1)
	{
		perror("read() error for length ! Exiting !\n");
		exit(EXIT_FAILURE);
	}

	nbytesW = read(sock, buffer, length);
	if (nbytesW == -1)
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

void help(int check)
{
	printf("\n Available commands :\n /login\n /logout\n /register\n "
		   "/quit\n "
		   "/viewProfile\n /addFriend\n /addPost\n /checkReq\n /accFriend\n /friends\n /online\n\n");
}

int login(int sC, int check, char *user)
{
	char id[33], password[33], *invPass;
	int resultAnswer = -1;
	memset(id, 0, 33);
	memset(password, 0, 33);

	write(sC, &check, sizeof(int));
	if (check == 1)
	{
		printf("Already logged in !\n");
		return 1;
	}
	else
	{
		safeStdinRead("ID: ", id, 33);

		invPass = getpass("Password: ");
		strcpy(password, invPass);

		write(sC, id, sizeof(id));
		write(sC, password, sizeof(password));

		read(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 11:
		{
			printf("Login Complete !\n");
			strcpy(user, id);
			return 1;
		}
		case 101:
		{
			printf("ID doesn't exist ! \n");
			return 0;
		}
		case 102:
		{
			printf("Wrong Password ! \n");
			return 0;
		}
		case 103:
		{
			printf("Invalid ID \\ Password ! \n");
			return 0;
		}
		case 104:
		{
			printf("You're already logged in ! (probably in a different terminal)\n");
			return 0;
		}
		}
	}
	return 0;
}

void register_now(int sC, int check)
{
	int resultAnswer = -1;

	char id[33], *invPass, password[33], fullname[65], sex[5], about[513],
		type[17];

	write(sC, &check, sizeof(int));
	if (check == 1)
	{
		printf("You're logged in ! Please log out to register a new "
			   "account !\n");
		return;
	}
	else
	{
		memset(id, 0, 33);
		memset(password, 0, 33);
		memset(fullname, 0, 33);
		memset(sex, 0, 5);
		memset(about, 0, 33);
		memset(type, 0, 33);

		printf("ID (10-32 ch. alpha-numerical): ");
		fflush(stdout);
		myRead(id, 33);

		invPass = getpass("Password (10-32 ch.): ");
		strcpy(password, invPass);

		printf("Fullname (10-64 ch. alpha-numerical and spaces): ");
		fflush(stdout);
		myRead(fullname, 65);

		printf("Sex (F/M): ");
		fflush(stdout);
		myRead(sex, 5);

		printf("About (10-512 ch.):");
		fflush(stdout);
		myRead(about, 513);

		printf("Profile type (public/private):");
		fflush(stdout);
		myRead(type, 17);

		/*printf("Trimit: '%s'\n'%s'\n'%s'\n'%s'\n'%s'\n'%s'\n", id,
           password, fullname, sex, about, type);*/

		write(sC, id, sizeof(id));
		write(sC, password, sizeof(password));
		write(sC, fullname, sizeof(fullname));
		write(sC, sex, sizeof(sex));
		write(sC, about, sizeof(about));
		write(sC, type, sizeof(type));

		read(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 22:
		{
			printf("Register Complete !\n");
			break;
		}
		case 201:
		{
			printf("ID not long enough or invalid ! (min. 10 ch, only "
				   "alpha-numerical and . _ )\n");
			break;
		}
		case 202:
		{
			printf("Password not long enough ! (min. 10 ch)\n");
			break;
		}
		case 203:
		{
			printf("Full name not long enough or invalid ! (min. 10 "
				   "ch, only "
				   "alphanumerical and spaces)\n");
			break;
		}
		case 204:
		{
			printf("Invalid sex ! (F/M)\n");
			break;
		}
		case 205:
		{
			printf("Say something 'about' you ! (min. 10 ch)\n");
			break;
		}
		case 206:
		{
			printf("Invalid Profile Type ! (private/public)\n");
			break;
		}
		case 207:
		{
			printf("ID already exists !\n");
			break;
		}
		case 208:
		{
			printf("Use of double quotes ( \" ) is forbidden ! Please "
				   "use single quotes ( ' ) !\n");
			break;
		}
		}
	}
}

void viewProfile(int sC, int check)
{
	int resultAnswer = -1;
	char user[32], post[64];
	memset(user, 0, 32);
	memset(post, 0, 64);
	if (check == 0)
		printf("You can only view public users ! You're not logged in ! \n");
	else
		printf("You can view public/private users ! You're logged in ! \n");

	printf("View profile of user: ");
	fflush(stdout);
	myRead(user, 32);

	write(sC, &check, sizeof(int));
	write(sC, user, sizeof(user));

	read(sC, &resultAnswer, sizeof(int));
	if (resultAnswer == 401)
		printf("Not logged\n");
	if (resultAnswer == 44)
		printf("Logged\n");
	if (resultAnswer == 401)
		printf("The user is private ! You cannot view his profile !\n");

	if (resultAnswer == 44)
	{
		printf("Succesfull View\n");
		read(sC, post, sizeof(post));
		printf("Posts: %s\n", post);
	}
}

int logout(int sC, int check, char *id)
{
	write(sC, &check, sizeof(int));

	if (check == 0)
	{
		printf("You're not logged in ! \n");
	}
	else
	{
		printf("Logged out succesfully !\n");
	}
	memset(id, 0, 33);
	return 0;
}

void addFriend(int sC, int check)
{
	int resultAnswer = -1;
	char user[33], friendType[33];
	memset(user, 0, 33);
	memset(friendType, 0, 33);

	write(sC, &check, sizeof(int));
	if (check == 0)
	{
		printf("You're not logged in !\n");
		return;
	}
	else
	{
		printf("Add Friend: ");
		fflush(stdout);
		myRead(user, 33);

		printf("Group - 1(friends) / 2(close-friends) / 3 "
			   "(family)): ");
		fflush(stdout);
		myRead(friendType, 33);

		write(sC, user, sizeof(user));
		write(sC, friendType, sizeof(friendType));

		read(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 66:
		{
			printf("Friend Request has been sent to '%s' ! \n", user);
			break;
		}
		case 601:
		{
			printf("'%s' does not exist in our "
				   "database !\n",
				   user);
			break;
		}
		case 602:
		{
			printf("'%s' is already in your friends "
				   "list !\n",
				   user);
			break;
		}
		case 603:
		{
			printf("Type of friend is not valid !\n");
			break;
		}
		case 604:
		{
			printf("You cannot add yourself to friend !\n");
			break;
		}
		case 605:
		{
			printf("Friend request is already sent ! \n");
			break;
		}
		case 606:
		{
			printf("'%s' already sent you a friend request ! Accept him ! (/accFriend) \n", user);
			break;
		}
		}
		return;
	}
}

void addPost(int sC, int check)
{
	int resultAnswer = -1;
	char post[513], posttype[5];
	memset(post, 0, 513);
	memset(posttype, 0, 5);

	safeWrite(sC, &check, sizeof(int));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		safeStdinRead("Post (10-512 ch.):", post, 513);
		safeStdinRead("Type - 1(public) / 2(registered) / 3(friends) ", posttype, 5);

		safePrefWrite(sC, post);
		safePrefWrite(sC, posttype);

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 77:
			printf("You've added the post succesfully !\n");
			break;
		case 701:
			printf("Invalid type of post !\n");
			break;
		case 702:
			printf("Post not long enough (min. 10 ch.) or invalid ! (no quotes \" allowed)\n");
			break;
		}
	}
	return;
}


void setProfile(int sC, int check)
{
	//int resultAnswer = -1;

	write(sC, &check, sizeof(int));
	if (check == 0)
	{
		printf("You're not logged in ! \n");
	}
	else
	{
	}
}

void checkReq(int sC, int check)
{
	int resultAnswer = -1, requestsCount;
	char request[40], type;
	write(sC, &check, sizeof(int));
	if (check == 0)
	{
		printf("You're not logged int ! \n");
		return;
	}
	else
	{
		read(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 901:
		{
			printf("You have no requests !\n");
			return;
		}
		case 99:
		{
			read(sC, &requestsCount, sizeof(int));
			printf("You have %d requests :\n", requestsCount);

			while (requestsCount != 0)
			{
				memset(request, 0, sizeof(request));
				read(sC, request, sizeof(request));

				type = request[strlen(request) - 1];
				request[strlen(request) - 1] = 0;

				switch (type)
				{
				case '1':
					printf("'%s' wants to be your friend ! (/accFriend to accept him)\n", request);
					break;

				case '2':

					printf("'%s' wants to chat with you ! (/accChat to start chatting )\n", request);
					break;
				}
				requestsCount--;
			}

			return;
		}
		}
	}
}

void accFriend(int sC, int check)
{
	int resultAnswer = -1;
	char user[33], friendType[33];
	memset(user, 0, 33);
	memset(friendType, 0, 33);

	write(sC, &check, sizeof(int));

	if (check == 0)
	{
		printf("You are not logged in !\n");
	}
	else
	{
		printf("Accept:");
		fflush(stdout);
		myRead(user, 33);

		printf("Group - 1(friends) / 2(close-friends) / 3 "
			   "(family)): ");
		fflush(stdout);
		myRead(friendType, 33);

		write(sC, user, sizeof(user));
		write(sC, friendType, sizeof(friendType));

		read(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1010:
			switch (atoi(friendType))
			{
			case 1:
				printf("'%s' added to friends group ! \n", user);
				break;

			case 2:
				printf("'%s' added to close-friends group ! \n", user);
				break;

			case 3:
				printf("'%s' added to family group ! \n", user);
				break;
			}
			break;

		case 1001:
			printf("Type of friend is not valid !\n");
			break;

		case 1002:
			printf("You cannot accept yourself as a friend !\n");
			break;

		case 1003:
			printf("'%s' does not exist in our "
				   "database !\n",
				   user);
			break;

		case 1004:
			printf("'%s' is already in your friends "
				   "list !\n",
				   user);
			break;

		case 1005:
			printf("You don't have a friend request from '%s' \n", user);
			break;
		}
	}
	return;
}

void accChat(int sC, int check)
{
	//int resultAnswer = -1;

	write(sC, &check, sizeof(int));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
	}

	return;
}

void friends(int sC, int check)
{
	int resultAnswer = -1, friendsCount;
	char user[33], friend[35], friendtype;
	memset(user, 0, 33);
	memset(friend, 0, 35);

	write(sC, &check, sizeof(int));
	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		printf("Friends of:");
		fflush(stdout);
		myRead(user, 33);

		write(sC, user, sizeof(user));

		read(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1201:
			printf("'%s' doesn't exist in our database / invalid user !\n", user);
			break;

		case 1202:
			printf("'%s' is not your friend ! You cannot view his friends !\n", user);
			break;

		case 1203:
			printf("'%s' has no friends !\n", user);
			break;

		case 1212:
			read(sC, &friendsCount, sizeof(int));
			printf("\n\n'%s' has %d friends:\n\n", user, friendsCount);

			while (friendsCount != 0)
			{
				memset(friend, 0, 35);
				read(sC, friend, sizeof(friend));


				friendtype = friend[strlen(friend) - 1];
				friend[strlen(friend) - 1] = 0;
				switch (friendtype)
				{
				case '1':
					printf("%s	[friends] \n", friend);
					break;
				case '2':
					printf("%s	[close-friends] \n", friend);
					break;
				case '3':
					printf("%s	[family] \n", friend);
					break;
				}
				friendsCount--;
			}
			break;
		}
	}
	return;
}

void online(int sC, int check)
{
	int resultAnswer = -1, onlineCount;
	char online[35], friendtype;

	write(sC, &check, sizeof(int));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		read(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1301:
			printf("There are NO friends online at the moment !\n");
			return;

		case 1313:
			read(sC, &onlineCount, sizeof(int));
			printf("\n%d friends online :\n\n", onlineCount);

			while (onlineCount != 0)
			{
				memset(online, 0, 35);
				read(sC, online, sizeof(online));
				//prefRead(sC, online);

				friendtype = online[strlen(online) - 1];
				online[strlen(online) - 1] = 0;
				switch (friendtype)
				{
				case '1':
					printf("%s	[friends] \n", online);
					break;
				case '2':
					printf("%s	[close-friends] \n", online);
					break;
				case '3':
					printf("%s	[family] \n", online);
					break;
				}
				onlineCount--;
			}
			printf("\n");
			return;
		}
	}
	return;
}

int encodeCommand(const char *clientCommandChar)
{
	if (strcmp(clientCommandChar, "/login") == 0)
		return 1;
	if (strcmp(clientCommandChar, "/register") == 0)
		return 2;
	if (strcmp(clientCommandChar, "/quit") == 0)
		return 0;
	if (strcmp(clientCommandChar, "/help") == 0)
		return 3;
	if (strcmp(clientCommandChar, "/viewProfile") == 0)
		return 4;
	if (strcmp(clientCommandChar, "/logout") == 0)
		return 5;
	if (strcmp(clientCommandChar, "/addFriend") == 0)
		return 6;
	if (strcmp(clientCommandChar, "/addPost") == 0)
		return 7;
	if (strcmp(clientCommandChar, "/setProfile") == 0)
		return 8;
	if (strcmp(clientCommandChar, "/checkReq") == 0)
		return 9;
	if (strcmp(clientCommandChar, "/accFriend") == 0)
		return 10;
	if (strcmp(clientCommandChar, "/accChat") == 0)
		return 11;
	if (strcmp(clientCommandChar, "/friends") == 0)
		return 12;
	if (strcmp(clientCommandChar, "/online") == 0)
		return 13;
	return -1;
}

void quit(int sC, int check)
{
	write(sC, &check, sizeof(int));
	return;
}

__sighandler_t quitforce(void)
{
	printf("\nForced Quit!\n");
	exit(0);
}
