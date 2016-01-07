#include "vsoc.h"


/* TYPE OF REQUESTS: 1-Add Friend */


int callback(void *data, int argc, char **argv, char **azColName)
{
	//callback - default
	int i;

	for (i = 0; i < argc; i++)
	{
		printf("'%s' = '%s'\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}


int cbSingle(char *data, int argc, char **argv, char **azColName)
{
	//callback - numarator de linii, intoarce mereu o singura linie;
	// 		   - multiple utilizari
	strcpy(data, argv[0]);
	return 0;
}


int cbDSlines(int *data, int argc, char **argv, char **azColName)
{
	//callback - intoarce interogari direct la socket
	int i;

	for (i = 0; i < argc; i++)
	{
		safePrefWrite(data[0], argv[i]);
	}

	return 0;
}

int cbSendMsg(char **data, int argc, char **argv, char **azColName)
{
	//callback - trimite mesaje de la data la toti descr din room
	int i;

	for (i = 0; i < argc; i++)
	{
		printf("Scriu de la '%s' la '%d' mesajul '%s'\n", data[0], atoi(argv[i]), data[1]);
		safePrefWrite(atoi(argv[i]), data[0]);
		safePrefWrite(atoi(argv[i]), data[1]);
	}

	return 0;
}

void dbInsertUser(const char *ID, const char *PASS, const char *FULLNAME, const char *SEX, const char *ABOUT,
				  const char *TYPE)
{
	//insereaza un user cu toate detaliile in tabela USERS
	char *sql;
	sql = calloc(110 + strlen(ID) + strlen(PASS) + strlen(FULLNAME) + strlen(SEX) + strlen(ABOUT) + strlen(TYPE) + 10,
				 sizeof(char));

	sprintf(sql, "INSERT INTO USERS (ID,PASS,FULLNAME,SEX,ABOUT,TYPE,PRIV) "
				 "VALUES (\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"1 \");",
			ID, PASS, FULLNAME, SEX, ABOUT, TYPE);

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbInsertUser:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbInserUser:Succes\n");
	}

	free(sql);
}

int dbRegCheckUser(const char *ID)
{
	// verifica daca exista un user cu ID (asemanator, nu se face diferenta intre caps / noncaps) in USERS, pentru register

	char *sql;
	sql = calloc(60 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM USERS WHERE LOWER(ID)=LOWER(\"%s\");", ID);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbRegCheckUser:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbRegCheckUseR:Succes \n");
	}

	free(sql);

	return atoi((char *)&data);
}

int dbCheckUser(const char *ID)
{
	//verifica daca exista un user cu (exact!)ID in USERS, pentru logare si adaugare prieteni etc.

	char *sql;
	sql = calloc(60 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM USERS WHERE ID=\"%s\";", ID);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbLogCheckUser:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbLogCheckUser:Succes \n");
	}

	free(sql);

	return atoi((char *)&data);
}

int dbLogCheck(const char *ID, const char *PASS)
{
	//verifica (exact!) ID, PASS in users pentru logare
	char *sql;
	sql = calloc(60 + strlen(ID) + strlen(PASS), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM USERS WHERE ID=\"%s\" AND PASS=\"%s\";",
			ID, PASS);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbLogCheck:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbLogCheck:Succes \n");
	}

	free(sql);

	return atoi((char *)&data);
}

int dbSetOnline(const char *ID)
{
	// seteaza ID ca fiind ONLINE (introduce in tabela)
	char *sql;
	sql = calloc(50 + strlen(ID), sizeof(char));

	sprintf(sql, "INSERT INTO ONLINE (ID) "
				 "VALUES (\"%s\");",
			ID);
	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbSetOnline:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		free(sql);
		return 0;
	}

	fprintf(stdout, "dbSetOnline:Succes \n");
	free(sql);

	return 1;
}

void dbSetOffline(const char *ID)
{
	// seteaza ID ca fiind OFFLINE (sterge din tabela)
	char *sql;
	sql = calloc(40 + strlen(ID), sizeof(char));

	sprintf(sql, "DELETE FROM ONLINE WHERE ID=\"%s\";", ID);

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbSetOffline:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbSetOffline:Succes \n");
	}

	free(sql);
}

void dbForceQuit(void)
{
	// delogheaza toti ID din tabela
	char *sql;
	sql = calloc(50, sizeof(char));

	sprintf(sql, "DELETE FROM ONLINE;DELETE FROM CHATUSERS;");

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbForceQuit:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbForceQuit:Succes \n");
	}

	free(sql);
}

int dbRequestCheckType(const char *ID1, const char *ID2, const char *type)
{
	// verifica daca exista cereri in REQUESTS de tip TYPE , de la ID1 la ID2
	char *sql;
	sql = calloc(90 + strlen(ID1) + strlen(ID2) + strlen(type), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM REQUESTS WHERE fromUser=\"%s\" AND toUser=\"%s\" AND TYPE=\"%s\";", ID1, ID2, type);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbRequestCheckType:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbRequestCheckType %s :Succes \n", type);
	}

	free(sql);

	return atoi((char *)&data);
}


int dbFriendCheck(const char *ID1, const char *ID2)
{
	// verifica daca ID1 il are ca prieten deja pe ID2
	char *sql;
	sql = calloc(70 + strlen(ID1) + strlen(ID2), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM FRIENDS WHERE owner=\"%s\" AND friend=\"%s\";", ID1, ID2);

	char *data;

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbFriendCheck:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbFriendCheck:Succes \n");
	}

	free(sql);

	return atoi((char *)&data);
}

void dbRequestSend(const char *ID1, const char *ID2, const char *type, const char *friendtype)
{
	// adauga o cerere de tip TYPE de la ID1 la ID2, daca e de tip friend, adauga tipul FRIENDTYPE de prieten
	char *sql;
	sql = calloc(100 + strlen(ID1) + strlen(ID2) + strlen(type) + strlen(friendtype), sizeof(char));

	sprintf(sql, "INSERT INTO REQUESTS (fromUser,toUser,type,friendtype) "
				 "VALUES (\"%s\",\"%s\",\"%s\",\"%s\");",
			ID1, ID2, type, friendtype);

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbRequestSend:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbRequestSend:Succes \n");
	}

	free(sql);
}

int dbRequestCheckCount(const char *ID)
{
	//verifica daca ID are cereri de tip TYPE > returneaza nr lor
	char *sql;
	sql = calloc(50 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM REQUESTS WHERE toUser=\"%s\";", ID);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbRequestCheckCount:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbRequestCheckCount:Succes \n");
	}

	free(sql);

	return atoi((char *)&data);
}


void dbRequestCheck(const char *ID, int sC)
{
	//intoarce requesturile primite de ID catre sC
	char *sql;
	sql = calloc(60 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT fromUser,type FROM REQUESTS WHERE toUser=\"%s\";", ID);

	int data[3];
	data[0] = sC;

	rc = sqlite3_exec(db, sql, (void *)cbDSlines, data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbRequestCheck:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbRequestCheck:Succes \n");
	}

	free(sql);

	return;
}

void dbInsertFriend(const char *ID1, const char *ID2, const char *friendtype)
{
	// adauga in FRIENDS, ID2 prieten pentru ID1, tip FRIENDTYPE
	char *sql;
	sql = calloc(80 + strlen(ID1) + strlen(ID2) + strlen(friendtype), sizeof(char));

	sprintf(sql, "INSERT INTO FRIENDS (owner,friend,type) "
				 "VALUES (\"%s\",\"%s\",\"%s\");",
			ID1, ID2, friendtype);
	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbFriendInsert:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbFriendInsert:Succes \n");
	}

	free(sql);
}

char *dbGetFTypeFromReq(const char *ID1, const char *ID2)
{
	// intoarce tipul de prieten TYPE pus de ID1 in cererea catre ID2
	char *sql;
	sql = calloc(90 + strlen(ID1) + strlen(ID2), sizeof(char));
	sprintf(sql, "SELECT friendtype FROM REQUESTS WHERE fromUser=\"%s\" AND toUser=\"%s\" AND type=\"1\";", ID1, ID2);

	char *data;
	data = calloc(5, sizeof(char));

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, (void *)cbSingle, data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbGetFTypeFromReq:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbGetFTypeFromReq:Succes \n");
	}

	free(sql);

	return data;
}

void dbDeleteRequestType(const char *ID1, const char *ID2, const char *type)
{
	// sterge cererile de tip TYPE din tabela REQUESTS de la ID1 la ID2
	char *sql;
	sql = calloc(80 + strlen(ID1) + strlen(ID2) + strlen(type), sizeof(char));

	sprintf(sql, "DELETE FROM REQUESTS WHERE fromUser=\"%s\" AND toUser=\"%s\" AND type=\"%s\";", ID1, ID2, type);

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbDeleteRequestType:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbDeleteRequestType:Succes \n");
	}
	free(sql);

	return;
}

int dbFriendsCount(const char *ID)
{
	//intoarce numarul de prieteni a lui ID
	char *sql;
	sql = calloc(50 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM FRIENDS WHERE owner=\"%s\";", ID);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbFriendsCount:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbFriendsCount:Succes \n");
	}

	free(sql);

	return atoi((char *)&data);
}

void dbFriends(const char *ID, int sC)
{
	//intoarce prietenii lui ID
	char *sql;
	sql = calloc(60 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT friend,type FROM FRIENDS WHERE owner=\"%s\";", ID);

	int data[2];
	data[0] = sC;

	rc = sqlite3_exec(db, sql, (void *)cbDSlines, data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbFriends:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbFriends:Succes \n");
	}

	free(sql);

	return;
}

int dbOnlineCount(const char *ID)
{
	//intoarce numarul de prieteni online a lui ID
	char *sql;
	sql = calloc(90 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM FRIENDS F, ONLINE O WHERE F.friend=O.id AND F.owner = \"%s\";", ID);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbOnlineCount:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbOnlineCount:Succes \n");
	}

	free(sql);

	return atoi((char *)&data);
}

void dbOnline(const char *ID, int sC)
{
	//intoarce prietenii online ai lui ID
	char *sql;
	sql = calloc(100 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT F.friend,F.type FROM FRIENDS F, ONLINE O WHERE F.friend = O.id AND F.owner = \"%s\";", ID);

	int data[2];
	data[0] = sC;

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, (void *)cbDSlines, data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbOnline:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbOnline:Succes \n");
	}

	free(sql);

	return;
}

void dbInsertPost(const char *ID, const char *post, const char *posttype)
{
	//insereaza postarea POST in POSTS de tipul POSTTYPE, pentru userul ID
	char *sql;
	char *currTime;

	currTime = calloc(60, sizeof(char));
	sprintf(currTime, "strftime('%%H:%%M:%%S - %%d.%%m.%%Y','now','+2 hours')");

	sql = calloc(120 + strlen(ID) + strlen(post) + strlen(posttype), sizeof(char));
	sprintf(sql, "INSERT INTO POSTS(user,post,type,date) VALUES (\"%s\",\"%s\",\"%s\",%s);", ID, post, posttype, currTime);

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbInsertPost:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbInsertPost:Succes \n");
	}

	free(sql);
	free(currTime);

	return;
}

void dbSetProfile(const char *ID, const char *value, const char *col)
{
	//schimba in atributul COL valoare in VALUE pentru userul ID
	char *sql;
	sql = calloc(50 + strlen(ID) + strlen(value) + strlen(col), sizeof(char));
	sprintf(sql, "UPDATE users SET %s=\"%s\" WHERE ID=\"%s\";", col, value, ID);

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbSetProfile:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbSetProfile:Succes \n");
	}

	free(sql);

	return;
}

char *dbGetUserType(const char *ID)
{
	// intoarce tipul de prieten TYPE pus de ID1 in cererea catre ID2
	char *sql;
	sql = calloc(40 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT type FROM USERS WHERE id=\"%s\";", ID);

	char *data;
	data = calloc(17, sizeof(char));

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, (void *)cbSingle, data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbGetUserType:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbGetUserType:Succes \n");
	}

	free(sql);

	return data;
}

char *dbGetFType(const char *ID1, const char *ID2)
{
	// intoarce tipul de prieten TYPE pus de ID1 pentru ID2 in FRIENDS
	char *sql;
	sql = calloc(70 + strlen(ID1) + strlen(ID2), sizeof(char));
	sprintf(sql, "SELECT type FROM FRIENDS WHERE owner=\"%s\" AND friend=\"%s\";", ID1, ID2);

	char *data;
	data = calloc(5, sizeof(char));

	rc = sqlite3_exec(db, sql, (void *)cbSingle, data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbGetFType:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbGetFType:Succes \n");
	}

	free(sql);

	return data;
}

void dbGetInfoUser(const char *ID, int sC)
{
	// trimite la sC toate informatiile despre ID din tabela USERS
	char *sql;
	sql = calloc(70 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT ID,FULLNAME,SEX,ABOUT,TYPE,PRIV FROM USERS WHERE ID=\"%s\"", ID);

	int data[3];
	data[0] = sC;

	rc = sqlite3_exec(db, sql, (void *)cbDSlines, data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbGetInfoUser:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbGetInfoUser:Succes \n");
	}

	free(sql);
}

int dbGetPostsCount(const char *ID, const char *posttype, int limit)
{
	//intoarce numarul de postari ale lui ID in conformitate cu isFriend,usertype si limit

	char *sql;
	sql = calloc(80 + strlen(ID) + strlen(posttype) + 1, sizeof(char));

	sprintf(sql, "SELECT COUNT(*) FROM POSTS WHERE user=\"%s\" AND type<=\"%s\" LIMIT %d;", ID, posttype, limit);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbGetPostsCount:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbGetPostsCount:Succes \n");
	}


	free(sql);
	return atoi((char *)&data);
}


void dbGetPosts(const char *ID, int sC, const char *posttype, int limit)
{
	// trimite la sC toate postarile de tip posttype, limitate de limit ale lui ID
	char *sql;
	sql = calloc(75 + strlen(ID) + strlen(posttype) + 1, sizeof(char));
	sprintf(sql, "SELECT post,type,date FROM POSTS WHERE user=\"%s\" AND type<=\"%s\" LIMIT %d;", ID, posttype, limit);

	int data[3];
	data[0] = sC;

	rc = sqlite3_exec(db, sql, (void *)cbDSlines, data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbGetPosts:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbGetPosts:Succes \n");
	}

	free(sql);
}

int dbCheckRoom(const char *ROOM)
{
	//verifica daca exista o camera de chat ROOM

	char *sql;
	sql = calloc(50 + strlen(ROOM), sizeof(char));

	sprintf(sql, "SELECT COUNT(*) FROM chats WHERE room=\"%s\";", ROOM);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbCheckRoom:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbCheckRoom:Succes \n");
	}


	free(sql);
	return atoi((char *)&data);
}

void dbInsertRoom(const char *ROOM, const char *ID)
{
	// adauga in chats ROOM cu owneru ID
	char *sql;
	sql = calloc(60 + strlen(ID) + strlen(ROOM), sizeof(char));

	sprintf(sql, "INSERT INTO chats (room,owner) VALUES (\"%s\",\"%s\");",
			ROOM, ID);

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbInsertRoom:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbInsertRoom:Succes \n");
	}

	free(sql);
}

int dbGetRoomsCount(const char *ID)
{
	//returneaza numarul de camere create de prietenii lui ID sau de el

	char *sql;
	sql = calloc(170 + 2 * strlen(ID), sizeof(char));

	sprintf(sql, "SELECT COUNT(*) FROM (SELECT c.room,c.owner FROM friends f,chats c WHERE (f.owner=\"%s\" AND c.owner=f.friend) UNION SELECT room,owner FROM chats WHERE owner=\"%s\");", ID, ID);

	char *data;



	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbGetRoomsCount:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbGetRoomsCount:Succes \n");
	}

	free(sql);
	return atoi((char *)&data);
}

void dbGetRooms(const char *ID, int sC)
{
	// intoarce numele camerelor lui ID si a prietenilor sai
	char *sql;
	sql = calloc(150 + 2 * strlen(ID), sizeof(char));
	sprintf(sql, "SELECT c.room,c.owner FROM friends f,chats c WHERE (f.owner=\"%s\" AND c.owner=f.friend) UNION SELECT room,owner FROM chats WHERE owner=\"%s\";", ID, ID);

	int data[2];
	data[0] = sC;

	rc = sqlite3_exec(db, sql, (void *)cbDSlines, data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbGetRooms:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbGetRooms:Succes \n");
	}

	free(sql);
}

int dbIsEmptyRoom(const char *ROOM)
{
	//verifica daca ROOM are useri in ea
	char *sql;
	sql = calloc(50 + strlen(ROOM), sizeof(char));

	sprintf(sql, "SELECT COUNT(*) FROM chatusers WHERE room=\"%s\"", ROOM);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbIsEmptyRooms:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbIsEmptyRooms:Succes \n");
	}

	free(sql);
	return atoi((char *)&data);
}

int dbIsOwnerRoom(const char *ID, const char *ROOM)
{
	//verifica daca ID este ownerul camerei ROOM
	char *sql;
	sql = calloc(65 + strlen(ROOM) + strlen(ID), sizeof(char));

	sprintf(sql, "SELECT COUNT(*) FROM chats WHERE room=\"%s\" AND owner=\"%s\"", ROOM, ID);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbIsOwnerRoom:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbIsOwnerRoom:Succes \n");
	}

	free(sql);
	return atoi((char *)&data);
}

void dbDeleteRoom(const char *ROOM)
{
	// sterge roomul ROOM
	char *sql;
	sql = calloc(40 + strlen(ROOM), sizeof(char));

	sprintf(sql, "DELETE FROM chats WHERE room=\"%s\";", ROOM);

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbDeleteRoom:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbDeleteRoom:Succes \n");
	}

	free(sql);
}

void dbJoinRoom(const char *ID, const char *ROOM, int sC)
{
	// adauga in chatusers ID ce intra in ROOM
	char *sql;
	sql = calloc(75 + strlen(ID) + strlen(ROOM), sizeof(char));

	sprintf(sql, "INSERT INTO chatusers (room,user,sock) VALUES (\"%s\",\"%s\",\"%d\");",
			ROOM, ID, sC);

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbJoinRoom:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbJoinRoom:Succes \n");
	}

	free(sql);
}

void dbLeaveRoom(const char *ID)
{
	// sterge ID ce a intrat in ROOM din el
	char *sql;
	sql = calloc(60 + strlen(ID), sizeof(char));

	sprintf(sql, "DELETE FROM CHATUSERS WHERE user=\"%s\";", ID);

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbLeaveRoom:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbLeaveRoom:Succes \n");
	}

	free(sql);
}

int dbCheckRoomFriends(const char *ID, const char *ROOM)
{
	//verifica daca ROOM in care ID vrea sa intre e a lui sa a prietenilor lui
	char *sql;
	sql = calloc(210 + 2 * strlen(ROOM) + 2 * strlen(ID), sizeof(char));

	sprintf(sql, "select count(*) from (select c.room,c.owner from friends f,chats c where (f.owner=\"%s\" and c.owner=f.friend and c.room=\"%s\") union select room,owner from chats where owner=\"%s\" and room=\"%s\");", ID, ROOM, ID, ROOM);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbCheckRoomFriends:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbCheckRoomFriends:Succes \n");
	}

	free(sql);
	return atoi((char *)&data);
}

void dbSendMsgToRoom(char *ID, const char *ROOM, const char *MESG)
{
	// trimite MESG lui ID catre userii din ROOM
	char *sql, **data;
	sql = calloc(70 + strlen(ROOM) + strlen(ID), sizeof(char));
	data = calloc(2, sizeof(char *));
	data[0] = calloc(strlen(ID) + 5, sizeof(char));
	data[1] = calloc(strlen(MESG) + 5, sizeof(char));

	strcpy(data[0], ID);
	strcpy(data[1], MESG);

	if (ID[0] == '>' || ID[0] == '<')
		strcpy(ID, ID + 1);

	sprintf(sql, "SELECT SOCK FROM CHATUSERS WHERE room=\"%s\" AND user <> \"%s\";", ROOM, ID);


	rc = sqlite3_exec(db, sql, (void *)cbSendMsg, data, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbSendMsgToRoom:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbSendMsgToRoom:Succes \n");
	}

	free(data[0]);
	free(data[1]);
	free(data);
	free(sql);
}

int dbGetPrivilege(const char *ID)
{
	// intoarce tipul de privilegiu a lui ID (0 root, 1 common user, 2 admin)
	char *sql;
	sql = calloc(70 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT priv FROM users WHERE id=\"%s\";", ID);

	char *data;

	rc = sqlite3_exec(db, sql, (void *)cbSingle, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbGetPrivilege:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbGetPrivilege:Succes \n");
	}

	free(sql);

	return atoi((char *)&data);
}
