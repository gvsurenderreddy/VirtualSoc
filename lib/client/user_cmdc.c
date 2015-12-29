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

void help(int check)
{
	printf("\n Available commands :\n /login\n /logout\n /register\n "
		   "/quit\n "
		   "/viewProfile\n /addFriend\n /addPost\n /checkReq\n /accFriend\n\n");
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
		printf("ID: ");
		fflush(stdout);
		myRead(id, 33);

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
	char post[257], postType[33];
	memset(post, 0, 257);
	memset(postType, 0, 33);

	write(sC, &check, sizeof(int));
	if (check == 0)
	{
		printf("You're not logged in !\n");
		return;
	}
	else
	{

		printf("Text:");
		fflush(stdout);
		myRead(post, 257);

		printf("To - all (even not logged in) / pub (even not friend) "
			   "/ frn (friends) / "
			   "cfr (close friends) / fam (family) :");
		fflush(stdout);
		myRead(postType, 33);

		write(sC, post, sizeof(post));
		write(sC, postType, sizeof(postType));

		read(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 77:
		{
			printf("Post added succesfully !\n");
			break;
		}
		case 701:
		{
			printf("Failed to add post !\n");
			break;
		}
		default:
		{
			return;
		}
		}
	}
}

void setProfile(int sC, int check)
{
	int resultAnswer = -1;

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
	char singleRequest[40], type;
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
				memset(singleRequest, 0, sizeof(singleRequest));
				read(sC, singleRequest, sizeof(singleRequest));
				requestsCount--;

				type = singleRequest[strlen(singleRequest) - 1];
				singleRequest[strlen(singleRequest) - 1] = 0;

				if (type == '1')
				{
					printf("'%s' wants to be your friend ! (/accFriend to accept him)\n", singleRequest);
				}
				if (type == '2')
				{
					printf("'%s' wants to chat with you ! (/accChat to start chatting )\n", singleRequest);
				}
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
	return -1;
}

void quit(int sC, int check)
{
	write(sC, &check, sizeof(int));
	return;
}

void quitforce(void)
{
	printf("\nForced Quit!\n");
	exit(0);
}
