#include "vsoc.h"

void login(int sC, char *currentUser)
{
	int resultAnswer = 11;

	char user[SHORT_LEN], password[SHORT_LEN];
	memset(user, 0, SHORT_LEN);
	memset(password, 0, SHORT_LEN);

	bool check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 1)
	{
		return;
	}
	else
	{
		safePrefRead(sC, user);
		safePrefRead(sC, password);

		if ((strchr(user, '\"') != NULL || strchr(password, '\"') != NULL))
		{
			resultAnswer = 103;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbCheckUser(user) == 0)
		{
			resultAnswer = 101;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbLogCheck(user, password) == 0)
		{
			resultAnswer = 102;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbSetOnline(user) == 0)
		{
			resultAnswer = 104;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		safeWrite(sC, &resultAnswer, sizeof(int));
		strcpy(currentUser, user);
	}
	return;
}

void register_now(int sC, const char *currentUser)
{
	int resultAnswer = 22;
	size_t i;

	char user[SHORT_LEN], password[SHORT_LEN], fullname[LONG_LEN], sex[TYPE_LEN], about[TEXT_LEN], type[LTYPE_LEN];
	memset(user, 0, SHORT_LEN);
	memset(password, 0, SHORT_LEN);
	memset(fullname, 0, LONG_LEN);
	memset(sex, 0, TYPE_LEN);
	memset(about, 0, TEXT_LEN);
	memset(type, 0, LTYPE_LEN);


	bool check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 1)
	{
		return;
	}
	{
		safePrefRead(sC, user);
		safePrefRead(sC, password);
		safePrefRead(sC, fullname);
		safePrefRead(sC, sex);
		safePrefRead(sC, about);
		safePrefRead(sC, type);

		if (strlen(user) < 10)
		{
			resultAnswer = 201;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}
		else
			for (i = 0; i < strlen(user); i++)
			{
				if (!(isValidChar(user[i])))
				{
					resultAnswer = 201;
					safeWrite(sC, &resultAnswer, sizeof(int));
					return;
				}
			}

		for (i = 0; i < strlen(fullname); i++)
		{
			if (!(isValidChar(fullname[i])) && fullname[i] != ' ')
			{
				resultAnswer = 203;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}
		if (strlen(password) < 10)
		{
			resultAnswer = 202;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		for (i = 0; i < strlen(password); i++)
		{
			if (password[i] == '\"')
			{
				resultAnswer = 208;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}

		for (i = 0; i < strlen(about); i++)
		{
			if (about[i] == '\"')
			{
				resultAnswer = 208;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}

		if (strlen(fullname) < 10)
		{
			resultAnswer = 203;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if ((strlen(sex) != 1) || (strlen(sex) == 1 && sex[0] != 'F' && sex[0] != 'M'))
		{
			resultAnswer = 204;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (strlen(about) < 10)
		{
			resultAnswer = 205;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (strcmp(type, "private") != 0 && strcmp(type, "public") != 0)
		{
			resultAnswer = 206;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbRegCheckUser(user) != 0)
		{
			resultAnswer = 207;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		dbInsertUser(user, password, fullname, sex, about, type);
		safeWrite(sC, &resultAnswer, sizeof(int));
	}
	return;
}

void viewProfile(int sC, const char *currentUser)
{
	int resultAnswer = 44, postsCount, userPriv;
	bool check = 0, isFriend;


	char user[SHORT_LEN], *userType, *friendsType;
	memset(user, 0, SHORT_LEN);


	check = dbGetStatus(currentUser);

	safePrefRead(sC, user);

	//contine " ?
	if (strchr(user, '\"') != NULL)
	{
		resultAnswer = 401;
		safeWrite(sC, &resultAnswer, sizeof(int));
		return;
	}

	//exista userul ?
	if (dbCheckUser(user) == 0)
	{
		resultAnswer = 401;
		safeWrite(sC, &resultAnswer, sizeof(int));
		return;
	}


	//propriul profil
	if (strcmp(user, currentUser) == 0)
	{
		resultAnswer = 445;
		safeWrite(sC, &resultAnswer, sizeof(int));

		dbGetInfoUser(currentUser, sC);

		postsCount = dbGetPostsCount(currentUser, "3", -1);
		safeWrite(sC, &postsCount, sizeof(int));

		dbGetPosts(user, sC, "3", -1);

		return;
	}

	//este admin sau root ? Daca da, pot vizualiza orice




	//ce tip de user e ?
	userType = dbGetUserType(user);


	if (check == 0)
	{
		//nelogat.

		//e tip private ?
		if (strcmp(userType, "private") == 0)
		{
			resultAnswer = 402;
			safeWrite(sC, &resultAnswer, sizeof(int));

			free(userType);
			////402. user nelogat incearca sa vada profil privat
			return;
		}
		//e tip public ?
		else
		{
			resultAnswer = 441;
			safeWrite(sC, &resultAnswer, sizeof(int));


			dbGetInfoUser(user, sC);

			postsCount = dbGetPostsCount(user, "1", -1);
			safeWrite(sC, &postsCount, sizeof(int));

			dbGetPosts(user, sC, "1", -1);

			free(userType);
			////441.trimite info despre user, postarile publice
			return;
		}
	}
	else
	{
		//logat.
		isFriend = dbFriendCheck(user, currentUser);
		userPriv = dbGetPrivilege(currentUser);
		if (userPriv != 1)
		{
			resultAnswer = 444;
			safeWrite(sC, &resultAnswer, sizeof(int));

			dbGetInfoUser(user, sC);

			postsCount = dbGetPostsCount(user, "3", -1);
			safeWrite(sC, &postsCount, sizeof(int));

			dbGetPosts(user, sC, "3", -1);

			return;
		}



		//sunt prieteni ?
		if (isFriend)
		{
			friendsType = dbGetFType(user, currentUser);
			//friend
			if (atoi(friendsType) == 1)
			{
				resultAnswer = 443;
				safeWrite(sC, &resultAnswer, sizeof(int));

				dbGetInfoUser(user, sC);

				postsCount = dbGetPostsCount(user, "2", -1);
				safeWrite(sC, &postsCount, sizeof(int));

				dbGetPosts(user, sC, "2", -1);

				free(userType);
				free(friendsType);
				////443.trimite info despre user.postarile publice + friend
				return;
			}
			//close-friend / family
			if (atoi(friendsType) > 1)
			{
				resultAnswer = 444;
				safeWrite(sC, &resultAnswer, sizeof(int));

				dbGetInfoUser(user, sC);

				postsCount = dbGetPostsCount(user, "3", -1);
				safeWrite(sC, &postsCount, sizeof(int));

				dbGetPosts(user, sC, "3", -1);

				free(userType);
				free(friendsType);
				////444.trimite info despre user.postarile publice + friend + close-friend + family
				return;
			}
		}
		else
		//nu sunt prieteni ?
		{
			resultAnswer = 442;
			safeWrite(sC, &resultAnswer, sizeof(int));

			dbGetInfoUser(user, sC);

			postsCount = dbGetPostsCount(user, "1", -1);
			safeWrite(sC, &postsCount, sizeof(int));

			dbGetPosts(user, sC, "1", -1);

			free(userType);
			////442.trimite info despre user, postarile publice
			return;
		}
	}
	free(userType);
	free(friendsType);

	return;
}

void logout(int sC, char *currentUser)
{
	bool check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 1)
	{
		dbSetOffline(currentUser);
		dbLeaveRoom(currentUser);
		memset(currentUser, 0, SHORT_LEN);
	}

	return;
}

void addFriend(int sC, const char *currentUser)
{
	int resultAnswer = 66;
	bool check;

	char user[SHORT_LEN], friendType[TYPE_LEN];
	memset(user, 0, SHORT_LEN);
	memset(friendType, 0, TYPE_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		safePrefRead(sC, user);
		safePrefRead(sC, friendType);

		switch (atoi(friendType))
		{
		case 1:
		case 2:
		case 3:
			resultAnswer = 66;
			break;

		default:
			resultAnswer = 603;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}


		if (strcmp(user, currentUser) == 0)
		{
			resultAnswer = 604;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (strchr(user, '\"') != NULL)
		{
			resultAnswer = 601;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbCheckUser(user) == 0)
		{
			resultAnswer = 601;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbFriendCheck(currentUser, user) != 0)
		{
			resultAnswer = 602;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbRequestCheckType(currentUser, user, "1") != 0)
		{
			resultAnswer = 605;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbRequestCheckType(user, currentUser, "1") != 0)
		{
			resultAnswer = 606;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		dbRequestSend(currentUser, user, "1", friendType);
		safeWrite(sC, &resultAnswer, sizeof(int));
	}
	return;
}

void addPost(int sC, const char *currentUser)
{
	int resultAnswer = 77;
	bool check;

	char post[TEXT_LEN], postType[TYPE_LEN];
	memset(post, 0, TEXT_LEN);
	memset(postType, 0, TYPE_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		safePrefRead(sC, post);
		safePrefRead(sC, postType);

		switch (atoi(postType))
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
		dbInsertPost(currentUser, post, postType);
	}

	return;
}

void setProfile(int sC, const char *currentUser)
{
	int resultAnswer = 88, userPriv;
	bool check;
	size_t i;

	char option[TYPE_LEN], fullname[LONG_LEN], sex[TYPE_LEN], about[TEXT_LEN], type[LTYPE_LEN], password[SHORT_LEN], user[SHORT_LEN];
	memset(option, 0, TYPE_LEN);
	memset(fullname, 0, LONG_LEN);
	memset(sex, 0, TYPE_LEN);
	memset(about, 0, TEXT_LEN);
	memset(type, 0, LTYPE_LEN);
	memset(password, 0, SHORT_LEN);
	memset(user, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		userPriv = dbGetPrivilege(currentUser);
		safeWrite(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safePrefRead(sC, user);
			bool validUser = 1;

			if (strchr(user, '\"') != NULL || strlen(user) < 5)
			{
				validUser = 0;
				safeWrite(sC, &validUser, sizeof(bool));
				return;
			}

			if (dbCheckUser(user) == 0)
			{
				validUser = 0;
				safeWrite(sC, &validUser, sizeof(bool));
				return;
			}

			safeWrite(sC, &validUser, sizeof(bool));
		}
		else
		{
			strcpy(user, currentUser);
		}

		safePrefRead(sC, option);


		if (strlen(option) > 1)
		{
			return;
		}

		switch (atoi(option))
		{
		case 1:
			safePrefRead(sC, fullname);

			if (strlen(fullname) < 10)
			{
				resultAnswer = 801;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}

			for (i = 0; i < strlen(fullname); i++)
			{
				if (!(isValidChar(fullname[i])) && fullname[i] != ' ')
				{
					resultAnswer = 801;
					safeWrite(sC, &resultAnswer, sizeof(int));
					return;
				}
			}
			safeWrite(sC, &resultAnswer, sizeof(int));
			dbSetProfile(user, fullname, "fullname");
			return;

		case 2:
			safePrefRead(sC, sex);

			if ((strlen(sex) != 1) || (strlen(sex) == 1 && sex[0] != 'F' && sex[0] != 'M'))
			{
				resultAnswer = 803;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
			safeWrite(sC, &resultAnswer, sizeof(int));
			dbSetProfile(user, sex, "sex");
			return;

		case 3:
			safePrefRead(sC, about);

			if (strlen(about) < 10 || strchr(about, '\"') != NULL)
			{
				resultAnswer = 804;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
			safeWrite(sC, &resultAnswer, sizeof(int));
			dbSetProfile(user, about, "about");
			return;

		case 4:
			safePrefRead(sC, password);

			if (strlen(password) < 10 || strchr(password, '\"') != NULL)
			{
				resultAnswer = 805;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
			safeWrite(sC, &resultAnswer, sizeof(int));
			dbSetProfile(user, password, "pass");
			return;

		case 5:
			safePrefRead(sC, type);

			if (strcmp(type, "private") != 0 && strcmp(type, "public") != 0)
			{
				resultAnswer = 806;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
			safeWrite(sC, &resultAnswer, sizeof(int));
			dbSetProfile(user, type, "type");
			return;
		}
	}
	return;
}



void recvReq(int sC, const char *currentUser)
{
	int resultAnswer = 99, requestsCount, userPriv;
	bool check;

	char user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{

		userPriv = dbGetPrivilege(currentUser);
		safeWrite(sC, &userPriv, sizeof(int));


		if (userPriv != 1)
		{
			safePrefRead(sC, user);

			bool validUser = 1;

			if (strchr(user, '\"') != NULL || strlen(user) < 5)
			{
				validUser = 0;
				safeWrite(sC, &validUser, sizeof(bool));
				return;
			}

			if (dbCheckUser(user) == 0)
			{
				validUser = 0;
				safeWrite(sC, &validUser, sizeof(bool));
				return;
			}

			safeWrite(sC, &validUser, sizeof(bool));
		}
		else
		{
			strcpy(user, currentUser);
		}

		requestsCount = dbRequestCheckCount(user, 1);

		if (requestsCount == 0)
		{
			resultAnswer = 901;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		safeWrite(sC, &resultAnswer, sizeof(int));
		safeWrite(sC, &requestsCount, sizeof(int));

		dbRequestCheck(user, sC, 1);
	}
	return;
}

void accFriend(int sC, const char *currentUser)
{
	int resultAnswer = 1010;
	bool check;

	char user[SHORT_LEN], friendType[TYPE_LEN];
	memset(user, 0, SHORT_LEN);
	memset(friendType, 0, TYPE_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{

		safePrefRead(sC, user);
		safePrefRead(sC, friendType);

		switch (atoi(friendType))
		{
		case 1:
		case 2:
		case 3:
			resultAnswer = 1010;
			break;

		default:
			resultAnswer = 1001;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}


		if (strcmp(user, currentUser) == 0)
		{
			resultAnswer = 1002;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbCheckUser(user) == 0 || strchr(user, '\"') != NULL)
		{
			resultAnswer = 1003;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbFriendCheck(currentUser, user) != 0)
		{
			resultAnswer = 1004;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbRequestCheckType(user, currentUser, "1") == 0)
		{
			resultAnswer = 1005;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		char *getFType = dbGetFTypeFromReq(user, currentUser);

		dbInsertFriend(currentUser, user, friendType);
		dbInsertFriend(user, currentUser, getFType);
		dbDeleteRequestType(currentUser, user, "1");
		dbDeleteRequestType(user, currentUser, "1");

		safeWrite(sC, &resultAnswer, sizeof(int));

		free(getFType);
	}
	return;
}


void friends(int sC, const char *currentUser)
{
	int resultAnswer = 1212, friendsCount;
	bool check;

	char user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		safePrefRead(sC, user);


		if (strchr(user, '\"') != NULL)
		{
			resultAnswer = 1201;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbCheckUser(user) == 0)
		{
			resultAnswer = 1201;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		// este admin / root ? daca nu, verifica daca sunt prieteni
		if (dbGetPrivilege(currentUser) == 1)
		{
			if (dbFriendCheck(currentUser, user) == 0 && strcmp(currentUser, user) != 0)
			{
				resultAnswer = 1202;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}


		friendsCount = dbFriendsCount(user);

		if (friendsCount == 0)
		{
			resultAnswer = 1203;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		safeWrite(sC, &resultAnswer, sizeof(int));
		safeWrite(sC, &friendsCount, sizeof(int));

		dbFriends(user, sC);
	}

	return;
}

void online(int sC, const char *currentUser)
{
	int resultAnswer = 1313, onlineCount, userPriv;

	char user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	bool check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		userPriv = dbGetPrivilege(currentUser);
		safeWrite(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safePrefRead(sC, user);

			bool validUser = 1;

			if (strchr(user, '\"') != NULL || strlen(user) < 5)
			{
				validUser = 0;
				safeWrite(sC, &validUser, sizeof(bool));
				return;
			}

			if (dbCheckUser(user) == 0)
			{
				validUser = 0;
				safeWrite(sC, &validUser, sizeof(bool));
				return;
			}

			safeWrite(sC, &validUser, sizeof(bool));
		}
		else
		{
			strcpy(user, currentUser);
		}

		onlineCount = dbOnlineCount(user);

		if (onlineCount == 0)
		{
			resultAnswer = 1301;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		safeWrite(sC, &resultAnswer, sizeof(int));
		safeWrite(sC, &onlineCount, sizeof(int));

		dbOnline(user, sC);
	}
	return;
}

void createChat(int sC, const char *currentUser)
{

	int resultAnswer = 1414;
	bool check;

	char room[SHORT_LEN], password[SHORT_LEN];
	memset(room, 0, SHORT_LEN);
	memset(password, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		safePrefRead(sC, room);
		safePrefRead(sC, password);

		size_t i;

		if ((strchr(room, '\"') != NULL) || strlen(room) < 5)
		{
			resultAnswer = 1401;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		for (i = 0; i < strlen(room); i++)
		{
			if (!(isValidChar(room[i])))
			{
				resultAnswer = 1401;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}

		if (dbCheckRoom(room) != 0)
		{
			resultAnswer = 1402;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if ((strchr(password, '\"') != NULL) || strlen(password) < 5)
		{
			resultAnswer = 1403;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		dbInsertRoom(room, password, currentUser);
		safeWrite(sC, &resultAnswer, sizeof(int));
	}
	return;
}

void chat(int sC, const char *currentUser)
{
	int resultAnswer = 1515, roomsCount, userPriv;
	bool check;

	char user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		userPriv = dbGetPrivilege(currentUser);
		safeWrite(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safePrefRead(sC, user);

			bool validUser = 1;

			if (strchr(user, '\"') != NULL || strlen(user) < 5)
			{
				validUser = 0;
				safeWrite(sC, &validUser, sizeof(bool));
				return;
			}

			if (dbCheckUser(user) == 0)
			{
				validUser = 0;
				safeWrite(sC, &validUser, sizeof(bool));
				return;
			}

			safeWrite(sC, &validUser, sizeof(bool));
		}
		else
		{
			strcpy(user, currentUser);
		}

		roomsCount = dbGetRoomsCount(user);

		if (roomsCount == 0)
		{
			resultAnswer = 1501;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		safeWrite(sC, &resultAnswer, sizeof(int));
		safeWrite(sC, &roomsCount, sizeof(int));
		dbGetRooms(user, sC);
	}
	return;
}

void deleteChat(int sC, const char *currentUser)
{
	int resultAnswer = 1616;
	bool check;

	char room[SHORT_LEN];
	memset(room, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		safePrefRead(sC, room);

		if (strchr(room, '\"') != NULL || strlen(room) < 5)
		{
			resultAnswer = 1601;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbCheckRoom(room) == 0)
		{
			resultAnswer = 1601;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbGetPrivilege(currentUser) == 1)
		{
			if (dbIsOwnerRoom(currentUser, room) == 0)
			{
				resultAnswer = 1602;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}

		safeWrite(sC, &resultAnswer, sizeof(int));
		dbDeleteRoom(room);
	}
	return;
}

void joinChat(int sC, char *currentUser)
{
	int resultAnswer = 1717;
	bool check;

	char room[SHORT_LEN], password[SHORT_LEN];
	memset(room, 0, SHORT_LEN);
	memset(password, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		safePrefRead(sC, room);
		safePrefRead(sC, password);

		if (strchr(room, '\"') != NULL || strlen(room) < 5)
		{
			resultAnswer = 1701;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbCheckRoom(room) == 0)
		{
			resultAnswer = 1701;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbGetPrivilege(currentUser) == 1)
		{
			if (dbCheckRoomFriends(currentUser, room) == 0)
			{
				resultAnswer = 1702;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}

			if (strchr(password, '\"') != NULL || strlen(password) < 5)
			{
				resultAnswer = 1703;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}

			if (dbCheckRoomPass(room, password) == 0)
			{
				resultAnswer = 1703;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}

		safeWrite(sC, &resultAnswer, sizeof(int));
		dbJoinRoom(currentUser, room, sC);

		activeChat(sC, currentUser, room);

		dbLeaveRoom(currentUser);
	}
	return;
}


void setPriv(int sC, const char *currentUser)
{
	int resultAnswer = 1818;
	bool check;

	char user[SHORT_LEN], priv[TYPE_LEN];
	memset(user, 0, SHORT_LEN);
	memset(priv, 0, TYPE_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}

	else
	{
		safePrefRead(sC, user);
		safePrefRead(sC, priv);

		if (dbGetPrivilege(currentUser) != 0)
		{
			resultAnswer = 1801;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		switch (atoi(priv))
		{
		case 1:
		case 2:
			resultAnswer = 1818;
			break;

		default:
			resultAnswer = 1805;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (strchr(user, '\"') != NULL)
		{
			resultAnswer = 1802;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbCheckUser(user) == 0)
		{
			resultAnswer = 1802;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbGetPrivilege(user) == 0)
		{
			resultAnswer = 1804;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		dbSetPrivilege(user, priv);
		safeWrite(sC, &resultAnswer, sizeof(int));
	}

	return;
}

void deleteFriend(int sC, const char *currentUser)
{
	int resultAnswer = 1919, userPriv;
	bool check;

	char friend[SHORT_LEN], user[SHORT_LEN];
	memset(friend, 0, SHORT_LEN);
	memset(user, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		userPriv = dbGetPrivilege(currentUser);
		safeWrite(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safePrefRead(sC, friend);
			safePrefRead(sC, user);
		}
		else
		{
			safePrefRead(sC, friend);
			strcpy(user, currentUser);
		}

		if (strchr(friend, '\"') != NULL || strlen(friend) < 5)
		{
			resultAnswer = 1901;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbCheckUser(friend) == 0)
		{
			resultAnswer = 1901;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (userPriv != 1)
		{
			if (strchr(user, '\"') != NULL || strlen(user) < 5)
			{
				resultAnswer = 1902;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}

			if (dbCheckUser(user) == 0)
			{
				resultAnswer = 1902;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}

		if (userPriv == 1)
		{
			if (dbFriendCheck(currentUser, friend) == 0)
			{
				resultAnswer = 1903;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}


		dbDeleteFriend(user, friend);
		dbDeleteFriend(friend, user);
		safeWrite(sC, &resultAnswer, sizeof(int));
	}

	return;
}

void deleteRecvReq(int sC, const char *currentUser)
{
	int resultAnswer = 2020, userPriv;
	bool check;

	char user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		userPriv = dbGetPrivilege(currentUser);
		safeWrite(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safePrefRead(sC, user);
		}
		else
		{
			strcpy(user, currentUser);
		}

		if (userPriv != 1)
		{
			if (strchr(user, '\"') != NULL || strlen(user) < 5)
			{
				resultAnswer = 2001;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}

			if (dbCheckUser(user) == 0)
			{
				resultAnswer = 2001;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}

		dbDeleteRequests(user, 1);
		safeWrite(sC, &resultAnswer, sizeof(int));
	}

	return;
}

void sentReq(int sC, const char *currentUser)
{
	int resultAnswer = 2121, requestsCount, userPriv;
	bool check;

	char user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{

		userPriv = dbGetPrivilege(currentUser);
		safeWrite(sC, &userPriv, sizeof(int));


		if (userPriv != 1)
		{
			safePrefRead(sC, user);

			bool validUser = 1;

			if (strchr(user, '\"') != NULL || strlen(user) < 5)
			{
				validUser = 0;
				safeWrite(sC, &validUser, sizeof(bool));
				return;
			}

			if (dbCheckUser(user) == 0)
			{
				validUser = 0;
				safeWrite(sC, &validUser, sizeof(bool));
				return;
			}

			safeWrite(sC, &validUser, sizeof(bool));
		}
		else
		{
			strcpy(user, currentUser);
		}

		requestsCount = dbRequestCheckCount(user, 2);

		if (requestsCount == 0)
		{
			resultAnswer = 2101;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		safeWrite(sC, &resultAnswer, sizeof(int));
		safeWrite(sC, &requestsCount, sizeof(int));

		dbRequestCheck(user, sC, 2);
	}
	return;
}

void deleteSentReq(int sC, const char *currentUser)
{
	int resultAnswer = 2222, userPriv;
	bool check;

	char user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		userPriv = dbGetPrivilege(currentUser);
		safeWrite(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safePrefRead(sC, user);
		}
		else
		{
			strcpy(user, currentUser);
		}

		if (userPriv != 1)
		{
			if (strchr(user, '\"') != NULL || strlen(user) < 5)
			{
				resultAnswer = 2201;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}

			if (dbCheckUser(user) == 0)
			{
				resultAnswer = 2201;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}

		dbDeleteRequests(user, 2);
		safeWrite(sC, &resultAnswer, sizeof(int));
	}

	return;
}

void deleteUser(int sC, const char *currentUser)
{
	int resultAnswer = 2323;
	bool check;

	char user[SHORT_LEN];
	memset(user, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		safePrefRead(sC, user);

		if (dbGetPrivilege(user) == 0)
		{
			resultAnswer = 2303;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbGetPrivilege(currentUser) != 0)
		{
			resultAnswer = 2301;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (strchr(user, '\"') != NULL || strlen(user) < 5)
		{
			resultAnswer = 2302;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbCheckUser(user) == 0)
		{
			resultAnswer = 2302;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		dbDeleteUser(user, 1);
		int sock = dbGetSock(user);
		if (sock != 0)
		{
			safePrefWrite(sock, "!quit");
			safePrefWrite(sock, "");
		}
		dbSetOffline(user);
		dbDeleteUser(user, 2);

		safeWrite(sC, &resultAnswer, sizeof(int));
	}
	return;
}

void deletePost(int sC, const char *currentUser)
{
	int resultAnswer = 2424, userPriv;
	bool check;
	size_t i;

	char id[SHORT_LEN], user[SHORT_LEN];
	memset(id, 0, SHORT_LEN);
	memset(user, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		userPriv = dbGetPrivilege(currentUser);
		safeWrite(sC, &userPriv, sizeof(int));

		if (userPriv != 1)
		{
			safePrefRead(sC, id);
			safePrefRead(sC, user);
		}
		else
		{
			safePrefRead(sC, id);
			strcpy(user, currentUser);
		}

		for (i = 0; i < strlen(id); i++)
			if (id[i] < '0' || id[i] > '9')
			{
				resultAnswer = 2401;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}

		if (strchr(id, '\"') != NULL)
		{
			resultAnswer = 2401;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (dbPostCheck(id) == 0)
		{
			resultAnswer = 2401;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		if (userPriv != 1)
		{
			if (strchr(user, '\"') != NULL || strlen(user) < 5)
			{
				resultAnswer = 2402;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}

			if (dbCheckUser(user) == 0)
			{
				resultAnswer = 2402;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}

		if (userPriv == 1)
		{
			if (dbPostCheckOwner(currentUser, id) == 0)
			{
				resultAnswer = 2403;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}
		}


		dbDeletePost(user, id);
		safeWrite(sC, &resultAnswer, sizeof(int));
	}

	return;
}

void wall(int sC, const char *currentUser)
{
	int resultAnswer = 2525, postsCount;
	bool check;
	size_t i;
	char postsNumber[SHORT_LEN];
	memset(postsNumber, 0, SHORT_LEN);

	check = dbGetStatus(currentUser);
	safeWrite(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		safePrefRead(sC, postsNumber);

		if (strchr(postsNumber, '\"') != NULL)
		{
			resultAnswer = 2501;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		for (i = 0; i < strlen(postsNumber); i++)
			if ((postsNumber[i] < '0' || postsNumber[i] > '9') && postsNumber[i] != '-')
			{
				resultAnswer = 2501;
				safeWrite(sC, &resultAnswer, sizeof(int));
				return;
			}


		safeWrite(sC, &resultAnswer, sizeof(int));

		postsCount = dbWallCount(currentUser, postsNumber);
		safeWrite(sC, &postsCount, sizeof(int));


		dbWall(currentUser, sC, postsNumber);
	}

	return;
}

void quit(int sC, char *currentUser)
{

	dbSetOffline(currentUser);
	dbLeaveRoom(currentUser);
	memset(currentUser, 0, SHORT_LEN);

	return;
}

void answer(void *arg)
{
	struct thData tdL;
	tdL = *((struct thData *)arg);

	int clientCommand = -1;
	char clientID[SHORT_LEN];
	memset(clientID, 0, SHORT_LEN);

	while (clientCommand != 0)
	{

		if (read(tdL.client, &clientCommand, sizeof(int)) <= 0)
		{
			fprintf(stdout, "[thread %d]Client with conn. sock %d - ", tdL.idThread, tdL.client);
			fflush(stdout);
			perror("read() error or forced disconnect");
			fflush(stderr);

			dbSetOffline(clientID);
			dbLeaveRoom(clientID);
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
			register_now(tdL.client, clientID);
			break;

		case 4:
			viewProfile(tdL.client, clientID);
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
			recvReq(tdL.client, clientID);
			break;

		case 10:
			accFriend(tdL.client, clientID);
			break;

		case 12:
			friends(tdL.client, clientID);
			break;

		case 13:
			online(tdL.client, clientID);
			break;

		case 14:
			createChat(tdL.client, clientID);
			break;

		case 15:
			chat(tdL.client, clientID);
			break;

		case 16:
			deleteChat(tdL.client, clientID);
			break;

		case 17:
			joinChat(tdL.client, clientID);
			break;

		case 18:
			setPriv(tdL.client, clientID);
			break;

		case 19:
			deleteFriend(tdL.client, clientID);
			break;

		case 20:
			deleteRecvReq(tdL.client, clientID);
			break;

		case 21:
			sentReq(tdL.client, clientID);
			break;

		case 22:
			deleteSentReq(tdL.client, clientID);
			break;

		case 23:
			deleteUser(tdL.client, clientID);
			break;

		case 24:
			deletePost(tdL.client, clientID);
			break;

		case 25:
			wall(tdL.client, clientID);
			break;
		}
	}

	return;
}
