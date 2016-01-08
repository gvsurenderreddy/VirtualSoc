#define CLIENT 1
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

void help(bool check)
{
	printf("\n Available commands :\n /login\n /logout\n /register\n "
		   "/quit\n "
		   "/viewProfile\n /addFriend\n /addPost\n /recvReq\n /sentReq\n /accFriend\n /friends\n /online\n /setProfile\n /createChat\n /chat\n /deleteChat\n /joinChat\n /deleteRecvReq\n /deleteSentReq\n /deleteFriend\n\n");
}

int login(int sC, bool check, char *currentUser)
{
	int resultAnswer = -1;

	char user[33], password[33];
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
		getPassV2("Password: ", password, 33);

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

	char user[33], password[33], fullname[65], sex[5], about[513],
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

		getPassV2("Password (10-32 ch.): ", password, 33);


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
		fprintf(stdout, GREEN "Profile type: " RESET "%s", info);
		break;
	case 5:
		switch (atoi(info))
		{
		case 0:
			fprintf(stdout, GREEN "	Privileges: " RESET "root\n\n");
			break;

		case 1:
			fprintf(stdout, GREEN "	Privileges: " RESET "Common\n\n");
			break;

		case 2:
			fprintf(stdout, GREEN "	Privileges: " RESET "Admin\n\n");
			break;
		}
		break;
	}
}

static void userPrints(int sC, const char *user)
{
	int i, postsCount;
	char info[520], postType[5], date[60];

	for (i = 0; i < 6; i++)
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
		memset(date, 0, 60);

		safePrefRead(sC, info);
		safePrefRead(sC, postType);
		safePrefRead(sC, date);

		switch (atoi(postType))
		{
		case 1:
			printf(GREEN "[%s][publ]:" RESET "%s\n", date, info);
			break;

		case 2:
			printf(GREEN "[%s][frnd]:" RESET "%s\n", date, info);
			break;

		case 3:
			printf(GREEN "[%s][cf/f]:" RESET "%s\n", date, info);
			break;
		}
	}
	printf("\n");
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
	int resultAnswer = -1, userPriv;


	char option[3], fullname[65], sex[5], about[513], type[17], password[33], user[33];
	memset(option, 0, 3);
	memset(fullname, 0, 65);
	memset(sex, 0, 5);
	memset(about, 0, 513);
	memset(type, 0, 17);
	memset(password, 0, 33);
	memset(user, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in ! \n");
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safeStdinRead("Set profile of user: ", user, 33);
			safePrefWrite(sC, user);

			bool validUser;
			safeRead(sC, &validUser, sizeof(bool));
			if (validUser == 0)
			{
				printf("User '%s' doesn't exist or invalid !\n", user);
				return;
			}
		}
		else
		{
			strcpy(user, "yourself");
		}



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
			getPassV2("Password (10-32 ch.): ", password, 33);
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
				printf("Fullname of '%s' changed to '%s' !\n", user, fullname);
				break;
			case 2:
				printf("Sex of '%s' changed to '%s' \n", user, sex);
				break;
			case 3:
				printf("Description of '%s' changed to : %s \n", user, about);
				break;
			case 4:
				printf("Password of '%s' changed !\n", user);
				break;
			case 5:
				printf("Profile type of '%s' changed to %s !\n", user, type);
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

		case 807:
			break;
		}
	}
}

void recvReq(int sC, bool check)
{
	int resultAnswer = -1, requestsCount, userPriv;

	char request[40], type[5], user[33];
	memset(user, 0, 33);

	safeWrite(sC, &check, sizeof(bool));
	if (check == 0)
	{
		printf("You're not logged in ! \n");
		return;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safeStdinRead("Received requests of: ", user, 33);
			safePrefWrite(sC, user);

			bool validUser;
			safeRead(sC, &validUser, sizeof(bool));
			if (validUser == 0)
			{
				printf("User '%s' doesn't exist or invalid !\n", user);
				return;
			}
		}
		else
		{
			strcpy(user, "you");
		}

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 901:
			printf("'%s' received no requests !\n", user);
			return;

		case 99:
			safeRead(sC, &requestsCount, sizeof(int));
			printf("'%s' received %d requests :\n", user, requestsCount);

			while (requestsCount != 0)
			{
				memset(request, 0, sizeof(request));
				memset(type, 0, 5);
				safePrefRead(sC, request);
				safePrefRead(sC, type);

				switch (atoi(type))
				{
				case 1:
					printf("'%s' wants to be a friend of '%s'! \n", request, user);
					break;

				case 2:
					printf("'%s' wants to chat with '%s' !\n", request, user);
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
	int resultAnswer = -1, onlineCount, userPriv;

	char online[35], friendType[5], user[33];
	memset(online, 0, 35);
	memset(friendType, 0, 5);
	memset(user, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safeStdinRead("Online friends of user: ", user, 33);
			safePrefWrite(sC, user);

			bool validUser;
			safeRead(sC, &validUser, sizeof(bool));
			if (validUser == 0)
			{
				printf("User '%s' doesn't exist or invalid !\n", user);
				return;
			}
		}
		else
		{
			strcpy(user, "you");
		}

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1301:
			printf("There are NO friends online at the moment for '%s' !\n", user);
			return;

		case 1313:
			safeRead(sC, &onlineCount, sizeof(int));
			printf("\n%d friends of '%s' online :\n\n", onlineCount, user);

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

void createChat(int sC, bool check)
{
	int resultAnswer = -1;

	char room[33];
	memset(room, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		safeStdinRead("Room name (5-32 ch.): ", room, 33);

		safePrefWrite(sC, room);

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1401:
			printf("Invalid room name !(5-32 ch. alpha-numerical, dot and underline, no quotes)\n");
			break;

		case 1402:
			printf("A room with name '%s' already exists !\n", room);
			break;

		case 1414:
			printf("Room '%s' created !\n", room);
			break;
		}
	}
	return;
}

void chat(int sC, bool check)
{
	int resultAnswer = -1, roomsCount, userPriv;

	char user[33], room[33];

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safeStdinRead("Available rooms of user: ", user, 33);
			safePrefWrite(sC, user);

			bool validUser;
			safeRead(sC, &validUser, sizeof(bool));
			if (validUser == 0)
			{
				printf("User '%s' doesn't exist or invalid !\n", user);
				return;
			}
		}
		else
		{
			strcpy(user, "you");
		}

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1501:
			printf("No rooms available for '%s' !\n", user);
			break;

		case 1515:
			safeRead(sC, &roomsCount, sizeof(int));

			printf("There are %d rooms available for '%s' : \n\n", roomsCount, user);
			while (roomsCount != 0)
			{
				memset(room, 0, 33);
				memset(user, 0, 33);
				safePrefRead(sC, room);
				safePrefRead(sC, user);
				printf("%s	[Owner: %s]\n", room, user);
				roomsCount--;
			}

			printf("\n");
			break;
		}
	}

	return;
}

void deleteChat(int sC, bool check)
{
	int resultAnswer = -1;

	char room[33];
	memset(room, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		safeStdinRead("Delete room: ", room, 33);

		safePrefWrite(sC, room);

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1601:
			printf("Room '%s' doesn't exist !\n", room);
			break;

		case 1602:
			printf("You're not the owner of room '%s' !\n", room);
			break;

		case 1616:
			printf("Room '%s' has been deleted !\n", room);
			break;
		}
	}
	return;
}

void joinChat(int sC, bool check)
{
	int resultAnswer = -1;

	char room[33];
	memset(room, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		safeStdinRead("Join chat: ", room, 33);

		safePrefWrite(sC, room);

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1701:
			printf("Room '%s' invalid or doesn't exist !\n", room);
			break;

		case 1702:
			printf("Room '%s' doesn't belong to you or to your friends !\n", room);
			break;

		case 1717:
			activeChat(sC, room);
			printf("You've exited the room '%s' !\n", room);
			break;
		}
	}
	return;
}

// chat part -----------------------------------------------------

void activeChat(int sC, const char *room)
{
	WINDOW *winput, *woutput;
	int winrows, wincols, i = 0, inChar;
	char inMesg[513], outMesg[513], user[33];
	memset(inMesg, 0, 513);
	memset(outMesg, 0, 513);
	memset(user, 0, 33);

	initscr();
	raw();
	getmaxyx(stdscr, winrows, wincols);
	winput = newwin(1, wincols, winrows - 1, 0);
	woutput = newwin(winrows - 1, wincols, 0, 0);
	keypad(winput, true);
	scrollok(woutput, true);
	//scrollok(winput, true);
	wrefresh(woutput);
	wrefresh(winput);

	fd_set all;
	fd_set read_fds;
	FD_ZERO(&all);
	FD_ZERO(&read_fds);
	FD_SET(0, &all);
	FD_SET(sC, &all);

	wprintw(woutput, "	Welcome to room '%s' \n	Use /quit to exit or ESC !\n\n", room);
	wrefresh(woutput);
	wrefresh(winput);

	while (true)
	{
		read_fds = all;

		if (select(sC + 1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("select() error or forced exit !\n");
			endwin();
			break;
		}

		if (FD_ISSET(sC, &read_fds))
		{
			//interpreteaza date(mesaje / comenzi)
			memset(user, 0, 33);
			memset(inMesg, 0, 513);
			safePrefRead(sC, user);
			safePrefRead(sC, inMesg);

			switch (user[0])
			{
			case '>':
				strcpy(user, user + 1);
				wprintw(woutput, "%s entered the room '%s'!\n", user, room);
				wrefresh(woutput);
				wrefresh(winput);
				break;

			case '!':
				safePrefWrite(sC, "/quit");
				delwin(winput);
				delwin(woutput);
				endwin();
				endwin();
				printf("You've been disconnected !\n");
				quitforce();
				return;

			case '<':
				strcpy(user, user + 1);
				wprintw(woutput, "%s exited the room '%s' !\n", user, room);
				wrefresh(woutput);
				wrefresh(winput);
				break;

			default:
				wprintw(woutput, "%s : %s\n", user, inMesg);
				wrefresh(woutput);
				wrefresh(winput);
				break;
			}
		}



		if (FD_ISSET(0, &read_fds))
		{


			inChar = wgetch(winput);

			if (inChar == 27)
			{
				safePrefWrite(sC, "/quit");
				break;
			}

			if (inChar == KEY_UP || inChar == KEY_DOWN || inChar == KEY_LEFT || inChar == KEY_RIGHT)
				continue;

			if (inChar == KEY_BACKSPACE || inChar == KEY_DC || inChar == 127)
			{
				wdelch(winput);
				wrefresh(winput);
				if (i != 0)
				{
					outMesg[i - 1] = 0;
					i--;
				}
			}
			else
			{
				outMesg[i] = (char)inChar;
				i++;
			}


			if (outMesg[i - 1] == '\n')
			{
				outMesg[i - 1] = 0;
				i = 0;

				if (outMesg[0] == 0)
					continue;

				if (strcmp(outMesg, "/quit") == 0)
				{
					safePrefWrite(sC, outMesg);
					break;
				}

				safePrefWrite(sC, outMesg);
				delwin(winput);
				winput = newwin(1, wincols, winrows - 1, 0);
				keypad(winput, true);
				wrefresh(winput);
				memset(outMesg, 0, 513);
			}
		}
	}

	delwin(winput);
	delwin(woutput);
	endwin();
	endwin();
}

//chat end ---------------------------------------------------------

void setPriv(int sC, bool check)
{
	int resultAnswer = -1;

	char user[33], priv[5];
	memset(user, 0, 33);
	memset(priv, 0, 5);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		safeStdinRead("Set privilege of user: ", user, 33);
		safeStdinRead("Privilege (1 - common /2 - admin): ", priv, 5);
		safePrefWrite(sC, user);
		safePrefWrite(sC, priv);


		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1818:
			switch (atoi(priv))
			{
			case 1:
				printf("Privilege for user '%s' has been set to Common !\n", user);
				break;

			case 2:
				printf("Privilege for user '%s' has been set to Admin !\n", user);
				break;
			}
			break;

		case 1801:
			printf("You must be root to set privileges !\n");
			break;

		case 1802:
			printf("User '%s' doesn't exist or is invalid ! \n", user);
			break;

		case 1804:
			printf("User '%s' is root ! You can't set his privileges ! \n", user);
			break;

		case 1805:
			printf("Privilege invalid !\n");
			break;
		}
	}
}

void deleteFriend(int sC, bool check)
{
	int resultAnswer = -1, userPriv;

	char user[33], friend[33];
	memset(user, 0, 33);
	memset(friend, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		safeStdinRead("Delete friend: ", friend, 33);
		safePrefWrite(sC, friend);

		if (userPriv != 1)
		{
			safeStdinRead("From this user's list: ", user, 33);
			safePrefWrite(sC, user);
		}
		else
		{
			strcpy(user, "you");
		}

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1919:
			printf("'%s' is no longer friend with '%s' !\n", friend, user);
			break;

		case 1901:
			printf("Friend '%s' doesn't exist or is invalid !\n", friend);
			break;

		case 1902:
			printf("User '%s' doesn't exist or is invalid !\n", user);
			break;

		case 1903:
			printf("'%s' is not your friend !\n", friend);
			break;
		}
	}
	return;
}

void deleteRecvReq(int sC, bool check)
{
	int resultAnswer = -1, userPriv;

	char user[33];
	memset(user, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));


		if (userPriv != 1)
		{
			safeStdinRead("Delete requests received by: ", user, 33);
			safePrefWrite(sC, user);
		}
		else
		{
			strcpy(user, "you");
		}

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 2020:
			printf("Received requests deleted for '%s' !\n", user);
			break;

		case 2001:
			printf("User '%s' doesn't exist or is invalid !\n", user);
			break;
		}
	}
	return;
}

void sentReq(int sC, bool check)
{
	int resultAnswer = -1, requestsCount, userPriv;

	char request[40], type[5], user[33];
	memset(user, 0, 33);

	safeWrite(sC, &check, sizeof(bool));
	if (check == 0)
	{
		printf("You're not logged in ! \n");
		return;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safeStdinRead("Received requests of: ", user, 33);
			safePrefWrite(sC, user);

			bool validUser;
			safeRead(sC, &validUser, sizeof(bool));
			if (validUser == 0)
			{
				printf("User '%s' doesn't exist or invalid !\n", user);
				return;
			}
		}
		else
		{
			strcpy(user, "you");
		}

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 2101:
			printf("'%s' sent no requests !\n", user);
			return;

		case 2121:
			safeRead(sC, &requestsCount, sizeof(int));
			printf("'%s' sent %d requests :\n", user, requestsCount);

			while (requestsCount != 0)
			{
				memset(request, 0, sizeof(request));
				memset(type, 0, 5);
				safePrefRead(sC, request);
				safePrefRead(sC, type);

				switch (atoi(type))
				{
				case 1:
					printf("'%s' sent '%s' a friend request !\n", user, request);
					break;

				case 2:
					printf("'%s' sent '%s' a chat request !\n", user, request);
					break;
				}
				requestsCount--;
			}
			return;
		}
	}

	return;
}


void deleteSentReq(int sC, bool check)
{
	int resultAnswer = -1, userPriv;

	char user[33];
	memset(user, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));


		if (userPriv != 1)
		{
			safeStdinRead("Delete requests sent by: ", user, 33);
			safePrefWrite(sC, user);
		}
		else
		{
			strcpy(user, "you");
		}

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 2222:
			printf("Sent requests deleted for '%s' !\n", user);
			break;

		case 2201:
			printf("User '%s' doesn't exist or is invalid !\n", user);
			break;
		}
	}
	return;
}

void deleteUser(int sC, bool check)
{
	int resultAnswer = -1;

	char user[33];
	memset(user, 0, 33);

	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		safeStdinRead("!!!   Warning   !!!\n!!! It will delete everything about that user !!!\nDelete user: ", user, 33);

		safePrefWrite(sC, user);

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 2301:
			printf("You must be root to delete an user !\n");
			break;

		case 2302:
			printf("User '%s' doesn't exist or invalid !\n", user);
			break;

		case 2303:
			printf("You cannot delete root '%s' !\n", user);
			break;

		case 2323:
			printf("User '%s' has been deleted !\n", user);
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
	if (strcmp(clientCommandChar, "/recvReq") == 0)
		return 9;
	if (strcmp(clientCommandChar, "/accFriend") == 0)
		return 10;
	if (strcmp(clientCommandChar, "/friends") == 0)
		return 12;
	if (strcmp(clientCommandChar, "/online") == 0)
		return 13;
	if (strcmp(clientCommandChar, "/createChat") == 0)
		return 14;
	if (strcmp(clientCommandChar, "/chat") == 0)
		return 15;
	if (strcmp(clientCommandChar, "/deleteChat") == 0)
		return 16;
	if (strcmp(clientCommandChar, "/joinChat") == 0)
		return 17;
	if (strcmp(clientCommandChar, "/setPriv") == 0)
		return 18;
	if (strcmp(clientCommandChar, "/deleteFriend") == 0)
		return 19;
	if (strcmp(clientCommandChar, "/deleteRecvReq") == 0)
		return 20;
	if (strcmp(clientCommandChar, "/sentReq") == 0)
		return 21;
	if (strcmp(clientCommandChar, "/deleteSentReq") == 0)
		return 22;
	if (strcmp(clientCommandChar, "/deleteUser") == 0)
		return 23;
	return -1;
}

void quit(int sC, bool check)
{
	safeWrite(sC, &check, sizeof(bool));
	return;
}

__sighandler_t quitforce(void)
{
	tcsetattr(fileno(stdin), TCSANOW, &oflags);
	printf("\nForced Quit!\n");
	exit(EXIT_FAILURE);
}
