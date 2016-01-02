#define CLIENT 1
#include "vsoc.h"

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
		perror("write() error ! Exiting !\n");
		exit(EXIT_FAILURE);
	}

	return length;
}

ssize_t safePrefRead(int sock, void *buffer)
{
	ssize_t length = strlen(buffer);

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
	ssize_t length = strlen(buffer);

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

void help(bool check)
{
	printf("\n Available commands :\n /login\n /logout\n /register\n "
		   "/quit\n "
		   "/viewProfile\n /addFriend\n /addPost\n /checkReq\n /accFriend\n /friends\n /online\n\n");
}

int login(int sC, bool check, char *currentUser)
{
	int resultAnswer = -1;

	char user[33], password[33], *invPass;
	memset(user, 0, 33);
	memset(password, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 1)
	{
		printf("You're already logged in !\n");
		return 1;
	}
	else
	{
		safeStdinRead("User: ", user, 33);

		invPass = getpass("Password: ");
		strcpy(password, invPass);

		safePrefWrite(sC, user);
		safePrefWrite(sC, password);

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 11:
			printf("Login Complete !\n");
			strcpy(currentUser, user);
			return 1;

		case 101:
			printf("User doesn't exist ! \n");
			return 0;

		case 102:
			printf("Wrong Password ! \n");
			return 0;

		case 103:
			printf("Invalid ID \\ Password ! \n");
			return 0;

		case 104:
			printf("You're already logged in ! (probably in a different terminal)\n");
			return 0;
		}
	}
	return 0;
}

void register_now(int sC, bool check)
{
	int resultAnswer = -1;

	char user[33], *invPass, password[33], fullname[65], sex[5], about[513],
		type[17];
	memset(user, 0, 33);
	memset(password, 0, 33);
	memset(fullname, 0, 65);
	memset(sex, 0, 5);
	memset(about, 0, 513);
	memset(type, 0, 17);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 1)
	{
		printf("You're logged in ! Please log out to register a new "
			   "account !\n");
		return;
	}

	else
	{
		safeStdinRead("User (10-32 ch. alpha-numerical): ", user, 33);

		invPass = getpass("Password (10-32 ch.): ");
		strcpy(password, invPass);

		safeStdinRead("Fullname (10-64 ch. alpha-numerical and spaces): ", fullname, 65);

		safeStdinRead("Sex (F/M): ", sex, 5);

		safeStdinRead("About (10-512 ch.): ", about, 513);

		safeStdinRead("Profile type (public/private):", type, 17);

		safePrefWrite(sC, user);
		safePrefWrite(sC, password);
		safePrefWrite(sC, fullname);
		safePrefWrite(sC, sex);
		safePrefWrite(sC, about);
		safePrefWrite(sC, type);

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 22:
			printf("Register Complete !\n");
			break;

		case 201:
			printf("User not long enough or invalid ! (min. 10 ch, only "
				   "alpha-numerical and . _ )\n");
			break;

		case 202:
			printf("Password not long enough ! (min. 10 ch)\n");
			break;

		case 203:
			printf("Full name not long enough or invalid ! (min. 10 "
				   "ch, only "
				   "alphanumerical and spaces)\n");
			break;

		case 204:
			printf("Invalid sex ! (F/M)\n");
			break;

		case 205:
			printf("Say something 'about' you ! (min. 10 ch)\n");
			break;

		case 206:
			printf("Invalid Profile Type ! (private/public)\n");
			break;

		case 207:
			printf("User already exists !\n");
			break;

		case 208:
			printf("Use of double quotes ( \" ) is forbidden ! Please "
				   "use single quotes ( ' ) !\n");
			break;
		}
	}
	return;
}

static void infoUserPrints(int i, char *info)
{
	switch (i)
	{
	case 0:
		fprintf(stdout, GREEN "Username: " RESET "%s", info);
		break;
	case 1:
		fprintf(stdout, GREEN "	Name: " RESET "%s", info);
		break;
	case 2:
		fprintf(stdout, GREEN "	Sex: " RESET "%s\n\n", info);
		break;
	case 3:
		fprintf(stdout, GREEN "About: " RESET "%s\n\n", info);
		break;
	case 4:
		fprintf(stdout, GREEN "Profile type: " RESET "%s\n\n", info);
		break;
	}
}

static void userPrints(int sC, const char *user)
{
	int i, postsCount;
	char info[520], postType[5], date[33];

	for (i = 0; i < 5; i++)
	{
		memset(info, 0, 520);
		safePrefRead(sC, info);
		infoUserPrints(i, info);
	}

	safeRead(sC, &postsCount, sizeof(int));
	printf(GREEN "%s has %d posts \n" RESET, user, postsCount);

	for (i = 0; i < postsCount; i++)
	{
		memset(info, 0, 520);
		memset(postType, 0, 5);
		memset(date, 0, 33);

		safePrefRead(sC, info);
		safePrefRead(sC, postType);
		safePrefRead(sC, date);

		switch (atoi(postType))
		{
		case 1:
			printf(GREEN "[post %d][%s][publ]:" RESET "%s\n", i + 1, date, info);
			break;

		case 2:
			printf(GREEN "[post %d][%s][frnd]:" RESET "%s\n", i + 1, date, info);
			break;

		case 3:
			printf(GREEN "[post %d][%s][cf/f]:" RESET "%s\n", i + 1, date, info);
			break;
		}
	}
}

void viewProfile(int sC, bool check)
{
	int resultAnswer = -1;

	char user[33];
	memset(user, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	safeStdinRead("View profile of user:", user, 33);
	safePrefWrite(sC, user);

	safeRead(sC, &resultAnswer, sizeof(int));

	switch (resultAnswer)
	{
	case 401:
		printf("User doesn't exist in our database or is invalid !\n");
		break;

	case 402:
		printf("This user is private ! Please login or register !\n");
		break;

	case 441:
		printf("You're not logged in ! \n\n");
		userPrints(sC, user);
		break;

	case 442:
		printf("You're not friend with '%s' !\n\n", user);
		userPrints(sC, user);
		break;

	case 443:
	case 444:
		userPrints(sC, user);
		break;
	case 445:
		printf("It's you !\n\n");
		userPrints(sC, user);
		break;

	default:
		printf("ERROR !\n");
		break;
	}
	return;
}

int logout(int sC, bool check, char *currentUser)
{
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in ! \n");
	}
	else
	{
		printf("Logged out succesfully !\n");
	}
	memset(currentUser, 0, 33);
	return 0;
}

void addFriend(int sC, bool check)
{
	int resultAnswer = -1;

	char user[33], friendType[33];
	memset(user, 0, 33);
	memset(friendType, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return;
	}
	else
	{
		safeStdinRead("Add Friend: ", user, 33);

		safeStdinRead("Group - 1(friends) / 2(close-friends) / 3 "
					  "(family)): ",
					  friendType, 33);

		safePrefWrite(sC, user);
		safePrefWrite(sC, friendType);

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 66:
			printf("Friend request has been sent to '%s' ! \n", user);
			break;

		case 601:
			printf("'%s' does not exist in our "
				   "database !\n",
				   user);
			break;

		case 602:
			printf("'%s' is already in your friends "
				   "list !\n",
				   user);
			break;

		case 603:
			printf("Type of friend is not valid !\n");
			break;

		case 604:
			printf("You cannot add yourself to friend !\n");
			break;

		case 605:
			printf("Friend request is already sent to '%s' ! \n", user);
			break;

		case 606:
			printf("'%s' already sent you a friend request ! Accept him ! (/accFriend) \n", user);
			break;
		}
	}
	return;
}

void addPost(int sC, bool check)
{
	int resultAnswer = -1;

	char post[513], postType[5];
	memset(post, 0, 513);
	memset(postType, 0, 5);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		safeStdinRead("Post (10-512 ch.):", post, 513);
		safeStdinRead("Type - 1(public) / 2(friends) / 3(close-friends/family) ", postType, 5);

		safePrefWrite(sC, post);
		safePrefWrite(sC, postType);

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


void setProfile(int sC, bool check)
{
	int resultAnswer = -1;

	char option[3], fullname[65], sex[5], about[513], type[17], password[33], *invPass;
	memset(option, 0, 3);
	memset(fullname, 0, 65);
	memset(sex, 0, 5);
	memset(about, 0, 513);
	memset(type, 0, 17);
	memset(password, 0, 33);

	safeWrite(sC, &check, sizeof(int));

	if (check == 0)
	{
		printf("You're not logged in ! \n");
	}
	else
	{

		safeStdinRead("Choose what you'd like to modify:\n 1-Fullname\n 2-Sex\n 3-About\n 4-Password\n 5-Account Type\n\n", option, 3);

		safePrefWrite(sC, option);

		if (strlen(option) > 1)
		{
			printf("Invalid option !\n");
			return;
		}

		switch (atoi(option))
		{
		case 1:
			safeStdinRead("Fullname (10-64 ch. alpha-numerical and spaces): ", fullname, 65);
			safePrefWrite(sC, fullname);
			break;

		case 2:
			safeStdinRead("Sex (F/M): ", sex, 5);
			safePrefWrite(sC, sex);
			break;

		case 3:
			safeStdinRead("About (10-512 ch.): ", about, 513);
			safePrefWrite(sC, about);
			break;

		case 4:
			invPass = getpass("Password (10-32 ch.): ");
			strcpy(password, invPass);
			safePrefWrite(sC, password);
			break;

		case 5:
			safeStdinRead("Profile type (public/private):", type, 17);
			safePrefWrite(sC, type);
			break;

		default:
			printf("Invalid option !\n");
			return;
		}

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 88:
			switch (atoi(option))
			{
			case 1:
				printf("Fullname changed to '%s' !\n", fullname);
				break;
			case 2:
				printf("Sex changed to '%s' \n", sex);
				break;
			case 3:
				printf("Description changed to : %s \n", about);
				break;
			case 4:
				printf("Password changed !\n");
				break;
			case 5:
				printf("Profile type changed to %s !\n", type);
				break;
			}
			break;

		case 801:
			printf("Full name not long enough or invalid ! (min. 10 "
				   "ch, only "
				   "alphanumerical and spaces)\n");
			break;

		case 803:
			printf("Invalid sex ! (F/M)\n");
			break;

		case 804:
			printf("Description not long enough ! (min. 10 ch)\n");
			break;

		case 805:
			printf("Password not long enough or invalid ! (min. 10 ch)\n");
			break;

		case 806:
			printf("Invalid Profile Type ! (private/public)\n");
			break;
		}
	}
}

void checkReq(int sC, bool check)
{
	int resultAnswer = -1, requestsCount;

	char request[40], type[5];

	safeWrite(sC, &check, sizeof(bool));
	if (check == 0)
	{
		printf("You're not logged in ! \n");
		return;
	}
	else
	{
		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 901:
			printf("You have no requests !\n");
			return;

		case 99:
			safeRead(sC, &requestsCount, sizeof(int));
			printf("You have %d requests :\n", requestsCount);

			while (requestsCount != 0)
			{
				memset(request, 0, sizeof(request));
				memset(type, 0, 5);
				safePrefRead(sC, request);
				safePrefRead(sC, type);

				switch (atoi(type))
				{
				case 1:
					printf("'%s' wants to be your friend ! (/accFriend to accept him)\n", request);
					break;

				case 2:
					printf("'%s' wants to chat with you ! (/accChat to start chatting )\n", request);
					break;
				}
				requestsCount--;
			}
			return;
		}
	}

	return;
}

void accFriend(int sC, bool check)
{
	int resultAnswer = -1;

	char user[33], friendType[33];
	memset(user, 0, 33);
	memset(friendType, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You are not logged in !\n");
	}
	else
	{
		safeStdinRead("Accept: ", user, 33);

		safeStdinRead("Group - 1(friends) / 2(close-friends) / 3 "
					  "(family): ",
					  friendType, 33);

		safePrefWrite(sC, user);
		safePrefWrite(sC, friendType);

		safeRead(sC, &resultAnswer, sizeof(int));

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
			printf("You don't have a friend request from '%s' !\n", user);
			break;
		}
	}
	return;
}

void accChat(int sC, bool check)
{
	//int resultAnswer = -1;

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
	}

	return;
}

void friends(int sC, bool check)
{
	int resultAnswer = -1, friendsCount;

	char user[33], friend[35], friendType[5];
	memset(user, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		safeStdinRead("Friends of: ", user, 33);

		safePrefWrite(sC, user);

		safeRead(sC, &resultAnswer, sizeof(int));

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
			safeRead(sC, &friendsCount, sizeof(int));
			printf("\n\n'%s' has %d friends:\n\n", user, friendsCount);

			while (friendsCount != 0)
			{
				memset(friend, 0, 35);
				memset(friendType, 0, 5);
				safePrefRead(sC, friend);
				safePrefRead(sC, friendType);



				switch (atoi(friendType))
				{
				case 1:
					printf("%s	[friends] \n", friend);
					break;
				case 2:
					printf("%s	[close-friends] \n", friend);
					break;
				case 3:
					printf("%s	[family] \n", friend);
					break;
				}
				friendsCount--;
			}
			printf("\n");
			break;
		}
	}
	return;
}

void online(int sC, bool check)
{
	int resultAnswer = -1, onlineCount;

	char online[35], friendType[5];

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1301:
			printf("There are NO friends online at the moment !\n");
			return;

		case 1313:
			safeRead(sC, &onlineCount, sizeof(int));
			printf("\n%d friends online :\n\n", onlineCount);

			while (onlineCount != 0)
			{
				memset(online, 0, 35);
				memset(friendType, 0, 5);
				safePrefRead(sC, online);
				safePrefRead(sC, friendType);

				switch (atoi(friendType))
				{
				case 1:
					printf("%s	[friends] \n", online);
					break;
				case 2:
					printf("%s	[close-friends] \n", online);
					break;
				case 3:
					printf("%s	[family] \n", online);
					break;
				}
				onlineCount--;
			}
			printf("\n");
			break;
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

void quit(int sC, bool check)
{
	safeWrite(sC, &check, sizeof(bool));
	return;
}

__sighandler_t quitforce(void)
{
	printf("\nForced Quit!\n");
	exit(EXIT_FAILURE);
}
