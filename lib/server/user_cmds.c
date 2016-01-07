#include "vsoc.h"

ssize_t safeRead(int sock, void *buffer, size_t length)
{
	ssize_t nbytesR = read(sock, buffer, length);

	if (nbytesR == -1)
	{
		perror("write() error ! Exiting !\n");
		sqlite3_close(db);
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
		sqlite3_close(db);
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
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	nbytesR = read(sock, buffer, length);
	if (nbytesR == -1)
	{
		perror("read() error for data ! Exiting !\n");
		sqlite3_close(db);
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
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	nbytesW = write(sock, buffer, length);
	if (nbytesW == -1)
	{
		perror("write() error for data ! Exiting !\n");
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	return nbytesW;
}

static bool isValidChar(char elem)
{
	// lowercase, uppercase, underline, dot
	return ((elem >= 0x30 && elem <= 0x39) || // numbers
			(elem >= 0x61 && elem <= 0x7a) || // lower case
			(elem >= 41 && elem <= 0x5a) || // upper case
			(elem == 0x5f || elem == 0x2e)); // _ .
}

int createConnSocketR()
{
	int sd;

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("[server] Socket creation error ! socket(). \n");
		printf("[server] Errno: %d", errno);
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}
	int on = 1;
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	return sd;
}

void login(int sC, char *currentUser)
{
	int resultAnswer = 11;
	bool check;

	char user[33], password[33];
	memset(user, 0, 33);
	memset(password, 0, 33);

	safeRead(sC, &check, sizeof(bool));
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

void register_now(int sC)
{
	int resultAnswer = 22;
	bool check;
	size_t i;

	char user[33], password[33], fullname[65], sex[5], about[513], type[17];
	memset(user, 0, 33);
	memset(password, 0, 33);
	memset(fullname, 0, 65);
	memset(sex, 0, 5);
	memset(about, 0, 513);
	memset(type, 0, 17);

	safeRead(sC, &check, sizeof(bool));

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
	bool check, isFriend;


	char user[33], *userType, *friendsType;
	memset(user, 0, 33);


	safeRead(sC, &check, sizeof(bool));

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
	bool check;

	safeRead(sC, &check, sizeof(bool));

	if (check == 1)
	{
		dbSetOffline(currentUser);
		dbLeaveRoom(currentUser);
		memset(currentUser, 0, 33);
	}

	return;
}

void addFriend(int sC, const char *currentUser)
{
	int resultAnswer = 66;
	bool check;

	char user[33], friendType[33];
	memset(user, 0, 33);
	memset(friendType, 0, 33);

	safeRead(sC, &check, sizeof(bool));

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

		if (dbCheckUser(user) == 0 || strchr(user, '\"') != NULL)
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

	char post[513], postType[5];
	memset(post, 0, 513);
	memset(postType, 0, 5);

	safeRead(sC, &check, sizeof(bool));

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

	char option[3], fullname[65], sex[5], about[513], type[17], password[33], user[33];
	memset(option, 0, 3);
	memset(fullname, 0, 65);
	memset(sex, 0, 5);
	memset(about, 0, 513);
	memset(type, 0, 17);
	memset(password, 0, 33);
	memset(user, 0, 33);

	safeRead(sC, &check, sizeof(bool));

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



void checkReq(int sC, const char *currentUser)
{
	int resultAnswer = 99, requestsCount;
	bool check;

	safeRead(sC, &check, sizeof(bool));
	if (check == 0)
	{
		return;
	}
	else
	{
		requestsCount = dbRequestCheckCount(currentUser);

		if (requestsCount == 0)
		{
			resultAnswer = 901;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
		}

		safeWrite(sC, &resultAnswer, sizeof(int));
		safeWrite(sC, &requestsCount, sizeof(int));

		dbRequestCheck(currentUser, sC);
	}
	return;
}

void accFriend(int sC, const char *currentUser)
{
	int resultAnswer = 1010;
	bool check;

	char user[33], friendType[33];
	memset(user, 0, 33);
	memset(friendType, 0, 33);

	safeRead(sC, &check, sizeof(bool));
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

	char user[33];
	memset(user, 0, 33);

	safeRead(sC, &check, sizeof(bool));

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
	bool check;

	char user[33];
	memset(user, 0, 33);

	safeRead(sC, &check, sizeof(bool));

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

	char room[33];
	memset(room, 0, 33);

	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		safePrefRead(sC, room);

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

		dbInsertRoom(room, currentUser);
		safeWrite(sC, &resultAnswer, sizeof(int));
	}
	return;
}

void chat(int sC, const char *currentUser)
{
	int resultAnswer = 1515, roomsCount, userPriv;
	bool check;

	char user[33];
	memset(user, 0, 33);

	safeRead(sC, &check, sizeof(bool));

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

	char room[33];
	memset(room, 0, 33);

	safeRead(sC, &check, sizeof(bool));

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

		if (dbIsEmptyRoom(room) != 0)
		{
			resultAnswer = 1603;
			safeWrite(sC, &resultAnswer, sizeof(int));
			return;
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

	char room[33];
	memset(room, 0, 33);

	safeRead(sC, &check, sizeof(bool));

	if (check == 0)
	{
		return;
	}
	else
	{
		safePrefRead(sC, room);

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
		}

		safeWrite(sC, &resultAnswer, sizeof(int));
		dbJoinRoom(currentUser, room, sC);

		activeChat(sC, currentUser, room);

		dbLeaveRoom(currentUser);
	}
	return;
}

void activeChat(int sC, char *currentUser, const char *room)
{
	bool run = true;
	char mesg[513], actionUser[35];


	sprintf(actionUser, ">%s", currentUser);
	dbSendMsgToRoom(actionUser, room, "");

	while (run)
	{
		memset(mesg, 0, 513);
		safePrefRead(sC, mesg);
		if (strcmp(mesg, "/quit") == 0)
		{
			run = false;
			sprintf(actionUser, "<%s", currentUser);
			dbSendMsgToRoom(actionUser, room, "");
		}
		/*if (strcmp(mesg, "/online") == 0)
		{
			// implmenteaza functie onlineChat
		}*/
		else
		{
			safePrefWrite(sC, currentUser);
			safePrefWrite(sC, mesg);

			dbSendMsgToRoom(currentUser, room, mesg);
		}
	}

	return;
}

void quit(int sC, char *currentUser)
{
	bool check;

	safeRead(sC, &check, sizeof(bool));

	if (check == 1)
	{
		dbSetOffline(currentUser);
		dbLeaveRoom(currentUser);
		memset(currentUser, 0, 33);
	}

	return;
}

__sighandler_t forcequit(void)
{
	printf("[server]Force quit !\n");
	dbForceQuit();
	sqlite3_close(db);
	exit(EXIT_SUCCESS);
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
			register_now(tdL.client);
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
			checkReq(tdL.client, clientID);
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
		}
	}
}
