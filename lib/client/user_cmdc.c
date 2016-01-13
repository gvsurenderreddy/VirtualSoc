#define CLIENT 1
#include "vsoc.h"

void help(bool check)
{
	printf("\n Available commands :\n /login\n /logout\n /register\n "
		   "/quit\n "
		   "/viewProfile\n /addFriend\n /addPost\n /recvReq\n /sentReq\n /accFriend\n /friends\n /online\n /setProfile\n /createChat\n /chat\n /deleteChat\n /joinChat\n /deleteRecvReq\n /deleteSentReq\n /deleteFriend\n /deletePost\n /setPriv\n /wall\n\n");
}

bool login(int sC, char *currentUser)
{
	int resultAnswer = -1;

	char user[SHORT_LEN], password[SHORT_LEN];
	memset(user, 0, SHORT_LEN);
	memset(password, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 1)
	{
		printf("You're already logged in !\n");
		return true;
	}
	else
	{
		safeStdinRead("User: ", user, SHORT_LEN);
		getPassV2("Password: ", password, SHORT_LEN);

		safePrefWrite(sC, user);
		safePrefWrite(sC, password);

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 11:
			printf("Login Complete !\n");
			strcpy(currentUser, user);
			return true;

		case 101:
			printf("User doesn't exist ! \n");
			return false;

		case 102:
			printf("Wrong Password ! \n");
			return false;

		case 103:
			printf("Invalid ID \\ Password ! \n");
			return false;

		case 104:
			printf("You're already logged in ! (probably in a different terminal)\n");
			return false;
		}
	}
	return false;
}

bool register_now(int sC)
{
	int resultAnswer = -1;

	char user[SHORT_LEN], password[SHORT_LEN], fullname[LONG_LEN], sex[TYPE_LEN], about[TEXT_LEN],
		type[LTYPE_LEN];
	memset(user, 0, SHORT_LEN);
	memset(password, 0, SHORT_LEN);
	memset(fullname, 0, LONG_LEN);
	memset(sex, 0, TYPE_LEN);
	memset(about, 0, TEXT_LEN);
	memset(type, 0, LTYPE_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 1)
	{
		printf("You're logged in ! Please log out to register a new "
			   "account !\n");
		return true;
	}
	else
	{
		safeStdinRead("User (10-32 ch. alpha-numerical): ", user, SHORT_LEN);
		getPassV2("Password (10-32 ch.): ", password, SHORT_LEN);
		safeStdinRead("Fullname (10-64 ch. alpha-numerical and spaces): ", fullname, LONG_LEN);
		safeStdinRead("Sex (F/M): ", sex, TYPE_LEN);
		safeStdinRead("About (10-512 ch.): ", about, TEXT_LEN);
		safeStdinRead("Profile type (public/private):", type, LTYPE_LEN);

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
			printf("User not long enough or invalid ! (10 - 32 ch.  "
				   "alpha-numerical and '.', '_' )\n");
			break;

		case 202:
			printf("Password not long enough ! (10 - 32 ch.)\n");
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
	return false;
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
	char info[TEXT_LEN], postType[TYPE_LEN], date[SHORT_LEN], id[SHORT_LEN];

	printf("\n___________________________________________________________________\n\n");
	for (i = 0; i < 6; i++)
	{
		memset(info, 0, TEXT_LEN);
		safePrefRead(sC, info);
		infoUserPrints(i, info);
	}
	printf("___________________________________________________________________\n\n");

	safeRead(sC, &postsCount, sizeof(int));
	printf("%s has %d post(s) \n\n", user, postsCount);

	for (i = 0; i < postsCount; i++)
	{
		memset(id, 0, SHORT_LEN);
		memset(info, 0, TEXT_LEN);
		memset(postType, 0, TYPE_LEN);
		memset(date, 0, SHORT_LEN);

		safePrefRead(sC, id);
		safePrefRead(sC, info);
		safePrefRead(sC, postType);
		safePrefRead(sC, date);

		switch (atoi(postType))
		{
		case 1:
			printf(GREEN "[ID:%s]" YELLOW "[%s]" GREEN "[publ]:" RESET "%s\n", id, date, info);
			break;

		case 2:
			printf(GREEN "[ID:%s]" YELLOW "[%s]" YELLOW "[frnd]:" RESET "%s\n", id, date, info);
			break;

		case 3:
			printf(GREEN "[ID:%s]" YELLOW "[%s]" RED "[cf/f]:" RESET "%s\n", id, date, info);
			break;
		}
	}
	printf("\n");
}

void viewProfile(int sC)
{
	int resultAnswer = -1;

	char user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	safeStdinRead("View profile of user:", user, SHORT_LEN);
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
		printf(RED "\nYou're not logged in ! \n" RESET);
		userPrints(sC, user);
		break;

	case 442:
		printf(YELLOW "\nYou're not friend with '%s' !\n" RESET, user);
		userPrints(sC, user);
		break;

	case 443:
	case 444:
		userPrints(sC, user);
		break;
	case 445:
		printf("\nIt's you !\n");
		userPrints(sC, user);
		break;

	default:
		printf("ERROR !\n");
		break;
	}
	return;
}

bool logout(int sC, char *currentUser)
{
	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
	}
	else
	{
		printf("Logged out succesfully !\n");
	}

	memset(currentUser, 0, SHORT_LEN);
	return false;
}

bool addFriend(int sC)
{
	int resultAnswer = -1;

	char user[SHORT_LEN], friendType[TYPE_LEN];
	memset(user, 0, SHORT_LEN);
	memset(friendType, 0, TYPE_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}

	else
	{
		safeStdinRead("Add Friend: ", user, SHORT_LEN);

		safeStdinRead("Group - 1(friends) / 2(close-friends) / 3 "
					  "(family): ",
					  friendType, TYPE_LEN);

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
	return true;
}

bool addPost(int sC)
{
	int resultAnswer = -1;

	char post[TEXT_LEN], postType[TYPE_LEN];
	memset(post, 0, TEXT_LEN);
	memset(postType, 0, TYPE_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeStdinRead("Post (10-512 ch.):", post, TEXT_LEN);
		safeStdinRead("Type - 1(public) / 2(friends) / 3(close-friends/family): ", postType, TYPE_LEN);

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
	return true;
}


bool setProfile(int sC)
{
	int resultAnswer = -1, userPriv;


	char option[TYPE_LEN], fullname[LONG_LEN], sex[TYPE_LEN], about[TEXT_LEN], type[LTYPE_LEN], password[SHORT_LEN], user[SHORT_LEN];
	memset(option, 0, TYPE_LEN);
	memset(fullname, 0, LONG_LEN);
	memset(sex, 0, TYPE_LEN);
	memset(about, 0, TEXT_LEN);
	memset(type, 0, LTYPE_LEN);
	memset(password, 0, SHORT_LEN);
	memset(user, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}

	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safeStdinRead("Set profile of user: ", user, SHORT_LEN);
			safePrefWrite(sC, user);

			bool validUser;
			safeRead(sC, &validUser, sizeof(bool));
			if (validUser == 0)
			{
				printf("User '%s' doesn't exist or invalid !\n", user);
				return true;
			}
		}
		else
		{
			strcpy(user, "yourself");
		}



		safeStdinRead("Choose what you'd like to modify:\n 1-Fullname\n 2-Sex\n 3-About\n 4-Password\n 5-Account Type\n\n\n Option:", option, TYPE_LEN);

		safePrefWrite(sC, option);

		if (strlen(option) > 1)
		{
			printf("Invalid option !\n");
			return true;
		}


		switch (atoi(option))
		{
		case 1:
			safeStdinRead("Fullname (10-64 ch. alpha-numerical and spaces): ", fullname, LONG_LEN);
			safePrefWrite(sC, fullname);
			break;

		case 2:
			safeStdinRead("Sex (F/M): ", sex, TYPE_LEN);
			safePrefWrite(sC, sex);
			break;

		case 3:
			safeStdinRead("About (10-512 ch.): ", about, TEXT_LEN);
			safePrefWrite(sC, about);
			break;

		case 4:
			getPassV2("Password (10-32 ch.): ", password, SHORT_LEN);
			safePrefWrite(sC, password);
			break;

		case 5:
			safeStdinRead("Profile type (public/private):", type, LTYPE_LEN);
			safePrefWrite(sC, type);
			break;

		default:
			printf("Invalid option !\n");
			return true;
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
				printf("Sex of '%s' changed to '%s' !\n", user, sex);
				break;
			case 3:
				printf("Description of '%s' changed to : %s !\n", user, about);
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
	return true;
}

bool recvReq(int sC)
{
	int resultAnswer = -1, requestsCount, userPriv;

	char request[SHORT_LEN], type[TYPE_LEN], user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safeStdinRead("Received requests of: ", user, SHORT_LEN);
			safePrefWrite(sC, user);

			bool validUser;
			safeRead(sC, &validUser, sizeof(bool));
			if (validUser == 0)
			{
				printf("User '%s' doesn't exist or invalid !\n", user);
				return true;
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
			return true;

		case 99:
			safeRead(sC, &requestsCount, sizeof(int));
			printf("'%s' received %d requests :\n", user, requestsCount);

			while (requestsCount != 0)
			{
				memset(request, 0, SHORT_LEN);
				memset(type, 0, TYPE_LEN);
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
			break;
		}
	}
	return true;
}

bool accFriend(int sC)
{
	int resultAnswer = -1;

	char user[SHORT_LEN], friendType[TYPE_LEN];
	memset(user, 0, SHORT_LEN);
	memset(friendType, 0, TYPE_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeStdinRead("Accept: ", user, SHORT_LEN);

		safeStdinRead("Group - 1(friends) / 2(close-friends) / 3 "
					  "(family): ",
					  friendType, TYPE_LEN);

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
	return true;
}


bool friends(int sC)
{
	int resultAnswer = -1, friendsCount;

	char user[SHORT_LEN], friend[SHORT_LEN], friendType[TYPE_LEN];
	memset(user, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeStdinRead("Friends of: ", user, SHORT_LEN);

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
				memset(friend, 0, SHORT_LEN);
				memset(friendType, 0, TYPE_LEN);
				safePrefRead(sC, friend);
				safePrefRead(sC, friendType);



				switch (atoi(friendType))
				{
				case 1:
					printf("%s	" GREEN "[friends] \n" RESET, friend);
					break;
				case 2:
					printf("%s	" YELLOW "[close-friends] \n" RESET, friend);
					break;
				case 3:
					printf("%s	" RED "[family] \n" RESET, friend);
					break;
				}
				friendsCount--;
			}
			printf("\n");
			break;
		}
	}
	return true;
}

bool online(int sC)
{
	int resultAnswer = -1, onlineCount, userPriv;

	char online[SHORT_LEN], friendType[TYPE_LEN], user[SHORT_LEN];
	memset(online, 0, SHORT_LEN);
	memset(friendType, 0, TYPE_LEN);
	memset(user, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safeStdinRead("Online friends of user: ", user, SHORT_LEN);
			safePrefWrite(sC, user);

			bool validUser;
			safeRead(sC, &validUser, sizeof(bool));
			if (validUser == 0)
			{
				printf("User '%s' doesn't exist or invalid !\n", user);
				return true;
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
			return true;

		case 1313:
			safeRead(sC, &onlineCount, sizeof(int));
			printf("\n%d friends of '%s' online :\n\n", onlineCount, user);

			while (onlineCount != 0)
			{
				memset(online, 0, SHORT_LEN);
				memset(friendType, 0, TYPE_LEN);
				safePrefRead(sC, online);
				safePrefRead(sC, friendType);

				switch (atoi(friendType))
				{
				case 1:
					printf("%s	" GREEN "[friends] \n" RESET, online);
					break;
				case 2:
					printf("%s	" YELLOW "[close-friends] \n" RESET, online);
					break;
				case 3:
					printf("%s	" RED "[family] \n" RESET, online);
					break;
				}
				onlineCount--;
			}
			printf("\n");
			break;
		}
	}
	return true;
}

bool createChat(int sC)
{
	int resultAnswer = -1;

	char room[SHORT_LEN], password[SHORT_LEN];
	memset(room, 0, SHORT_LEN);
	memset(password, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}

	else
	{
		safeStdinRead("Room name (5-32 ch.): ", room, SHORT_LEN);
		getPassV2("Room password (5-32 ch.): ", password, SHORT_LEN);

		safePrefWrite(sC, room);
		safePrefWrite(sC, password);

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1401:
			printf("Invalid room name !(5-32 ch. alpha-numerical, dot and underline, no quotes)\n");
			break;

		case 1402:
			printf("A room with name '%s' already exists !\n", room);
			break;

		case 1403:
			printf("Invalid room password !(5-32 ch., no quotes)\n");
			break;

		case 1414:
			printf("Room '%s' created !\n", room);
			break;
		}
	}
	return true;
}

bool chat(int sC)
{
	int resultAnswer = -1, roomsCount, userPriv;

	char user[SHORT_LEN], room[SHORT_LEN];

	bool check, validUser;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safeStdinRead("Available rooms of user: ", user, SHORT_LEN);
			safePrefWrite(sC, user);


			safeRead(sC, &validUser, sizeof(bool));
			if (validUser == 0)
			{
				printf("User '%s' doesn't exist or invalid !\n", user);
				return true;
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
				memset(room, 0, SHORT_LEN);
				memset(user, 0, SHORT_LEN);
				safePrefRead(sC, room);
				safePrefRead(sC, user);
				printf("%s	[Owner: %s]\n", room, user);
				roomsCount--;
			}

			printf("\n");
			break;
		}
	}

	return true;
}

bool deleteChat(int sC)
{
	int resultAnswer = -1;

	char room[SHORT_LEN];
	memset(room, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeStdinRead("Delete room: ", room, SHORT_LEN);

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
	return true;
}

bool joinChat(int sC)
{
	int resultAnswer = -1;

	char room[SHORT_LEN], password[SHORT_LEN];
	memset(room, 0, SHORT_LEN);
	memset(password, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeStdinRead("Join chat: ", room, SHORT_LEN);
		getPassV2("Chat password: ", password, SHORT_LEN);

		safePrefWrite(sC, room);
		safePrefWrite(sC, password);

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 1701:
			printf("Room '%s' invalid or doesn't exist !\n", room);
			break;

		case 1702:
			printf("Room '%s' doesn't belong to you or to your friends !\n", room);
			break;

		case 1703:
			printf("Wrong password !\n");
			break;

		case 1717:
			activeChat(sC, room);
			printf("You've exited the room '%s' !\n", room);
			break;
		}
	}
	return true;
}


bool setPriv(int sC)
{
	int resultAnswer = -1;

	char user[SHORT_LEN], priv[TYPE_LEN];
	memset(user, 0, SHORT_LEN);
	memset(priv, 0, TYPE_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeStdinRead("Set privilege of user: ", user, SHORT_LEN);
		safeStdinRead("Privilege (1 - common /2 - admin): ", priv, TYPE_LEN);
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
	return true;
}

bool deleteFriend(int sC)
{
	int resultAnswer = -1, userPriv;

	char user[SHORT_LEN], friend[SHORT_LEN];
	memset(user, 0, SHORT_LEN);
	memset(friend, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		safeStdinRead("Delete friend: ", friend, SHORT_LEN);
		safePrefWrite(sC, friend);

		if (userPriv != 1)
		{
			safeStdinRead("From this user's list: ", user, SHORT_LEN);
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
	return true;
}

bool deleteRecvReq(int sC)
{
	int resultAnswer = -1, userPriv;

	char user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));


		if (userPriv != 1)
		{
			safeStdinRead("Delete requests received by: ", user, SHORT_LEN);
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
	return true;
}

bool sentReq(int sC)
{
	int resultAnswer = -1, requestsCount, userPriv;

	char request[SHORT_LEN], type[TYPE_LEN], user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safeStdinRead("Received requests of: ", user, SHORT_LEN);
			safePrefWrite(sC, user);

			bool validUser;
			safeRead(sC, &validUser, sizeof(bool));
			if (validUser == 0)
			{
				printf("User '%s' doesn't exist or invalid !\n", user);
				return true;
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
			return true;

		case 2121:
			safeRead(sC, &requestsCount, sizeof(int));
			printf("'%s' sent %d requests :\n", user, requestsCount);

			while (requestsCount != 0)
			{
				memset(request, 0, SHORT_LEN);
				memset(type, 0, TYPE_LEN);
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
			break;
		}
	}

	return true;
}


bool deleteSentReq(int sC)
{
	int resultAnswer = -1, userPriv;

	char user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));


		if (userPriv != 1)
		{
			safeStdinRead("Delete requests sent by: ", user, SHORT_LEN);
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
	return true;
}

bool deleteUser(int sC)
{
	int resultAnswer = -1;

	char user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeStdinRead(RED "!!!   Warning   !!!\n!!! It will delete everything about that user !!!\n" RESET "Delete user: ", user, SHORT_LEN);

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
	return true;
}

bool deletePost(int sC)
{
	int resultAnswer = -1, userPriv;

	char user[SHORT_LEN], id[SHORT_LEN];
	memset(user, 0, SHORT_LEN);
	memset(id, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{
		safeRead(sC, &userPriv, sizeof(int));

		safeStdinRead("Delete post with ID: ", id, SHORT_LEN);
		safePrefWrite(sC, id);

		if (userPriv != 1)
		{
			safeStdinRead("From this user's profile: ", user, SHORT_LEN);
			safePrefWrite(sC, user);
		}
		else
		{
			strcpy(user, "you");
		}

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 2424:
			printf("Delete post with ID '%s' from '%s' !\n", id, user);
			break;

		case 2401:
			printf("Post '%s' doesn't exist or is invalid !\n", id);
			break;

		case 2402:
			printf("User '%s' doesn't exist or is invalid !\n", user);
			break;

		case 2403:
			printf("Post with ID '%s' is not yours ! !\n", id);
			break;
		}
	}
	return true;
}

bool wall(int sC)
{
	int i, resultAnswer = -1, postsCount;

	char postsNumber[SHORT_LEN], id[SHORT_LEN], post[TEXT_LEN], user[SHORT_LEN], postType[TYPE_LEN], date[SHORT_LEN];

	memset(postsNumber, 0, SHORT_LEN);

	bool check;
	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		printf("You're not logged in !\n");
		return false;
	}
	else
	{

		safeStdinRead("View last (number) posts: ", postsNumber, SHORT_LEN);
		safePrefWrite(sC, postsNumber);

		safeRead(sC, &resultAnswer, sizeof(int));

		switch (resultAnswer)
		{
		case 2501:
			printf("Invalid number !\n");
			break;

		case 2525:
			safeRead(sC, &postsCount, sizeof(int));

			printf("Last %d posts from your friends and you : \n\n", postsCount);

			for (i = 0; i < postsCount; i++)
			{

				memset(id, 0, SHORT_LEN);
				memset(user, 0, SHORT_LEN);
				memset(date, 0, SHORT_LEN);
				memset(postType, 0, TYPE_LEN);
				memset(post, 0, TEXT_LEN);

				safePrefRead(sC, id);
				safePrefRead(sC, user);
				safePrefRead(sC, date);
				safePrefRead(sC, postType);
				safePrefRead(sC, post);

				switch (atoi(postType))
				{
				case 1:
					printf(GREEN "[ID:%s]" WHITE "[%s]" YELLOW "[%s]" GREEN "[publ]:" RESET "%s\n", id, user, date, post);
					break;

				case 2:
					printf(GREEN "[ID:%s]" WHITE "[%s]" YELLOW "[%s][frnd]:" RESET "%s\n", id, user, date, post);
					break;

				case 3:
					printf(GREEN "[ID:%s]" WHITE "[%s]" YELLOW "[%s]" RED "[cf/f]:" RESET "%s\n", id, user, date, post);
					break;
				}
			}
			printf("\n\n");
			break;

		default:
			printf("ERROR !\n");
			break;
		}
	}
	return true;
}


void quit(void)
{
	printf(RED "You've quit VirtualSoc !\n" RESET);
	return;
}
