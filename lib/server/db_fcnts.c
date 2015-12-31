#include "vsoc.h"


/* TYPE OF REQUESTS: 1-Add Friend, 2-Chat Invite */


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


int cbDynamicSender(int *data, int argc, char **argv, char **azColName)
{
	//callback - intoarce interogari direct la socket > eficient,dinamic
	int i;

	char *line;
	line = (char *)calloc(data[1], sizeof(char));

	for (i = 0; i < argc; i++)
	{
		strcat(line, argv[i]);
	}

	//prefWrite(data[0], line);
	write(data[0], line, data[1]);
	//printf("'%s'\n", line);
	free(line);

	return 0;
}

int cbDSlines(int *data, int argc, char **argv, char **azColName)
{
	//callback - intoarce interogari direct la socket > eficient,dinamic
	int i;

	for (i = 0; i < argc; i++)
	{
		int len = strlen(argv[i]);
		write(data[0], &len, sizeof(int));
		write(data[0], argv[i], len);
		//safewrite
	}

	return 0;
}

void dbInsertUser(char *ID, char *PASS, char *FULLNAME, char *SEX, char *ABOUT,
				  char *TYPE)
{
	//insereaza un user in tabela USERS
	char *sql;
	sql = (char *)calloc(100 + strlen(ID) + strlen(PASS) + strlen(FULLNAME) + strlen(SEX) + strlen(ABOUT) + strlen(TYPE) + 10,
						 sizeof(char));

	sprintf(sql, "INSERT INTO USERS (ID,PASS,FULLNAME,SEX,ABOUT,TYPE) "
				 "VALUES (\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");",
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

int dbRegCheckUser(char *ID)
{
	// verifica daca exista un user cu ID (asemanator, nu se face diferenta intre caps / noncaps) in USERS, pentru register
	char *sql;
	sql = (char *)calloc(70 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM USERS WHERE LOWER(ID)=LOWER(\"%s\");", ID);

	char *data;

	/* Execute SQL statement */
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

	int count = atoi((char *)&data);
	free(sql);

	return count;
}

int dbLogCheckUser(char *ID)
{
	//verifica daca exista un user cu (exact!)ID in USERS, pentru logare si adaugare prieteni etc.
	char *sql;
	sql = (char *)calloc(70 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM USERS WHERE ID=\"%s\";", ID);

	char *data;

	/* Execute SQL statement */
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

	int count = atoi((char *)&data);
	free(sql);

	return count;
}

int dbLogCheck(char *ID, char *PASS)
{
	//verifica (exact!) ID, PASS in users pentru logare
	char *sql;
	sql = (char *)calloc(70 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM USERS WHERE ID=\"%s\" AND PASS=\"%s\";",
			ID, PASS);

	char *data;

	/* Execute SQL statement */
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

	int count = atoi((char *)&data);
	free(sql);

	return count;
}

int dbSetOnline(char *ID)
{
	// seteaza ID ca fiind ONLINE (introduce in tabela)
	char *sql;
	sql = (char *)calloc(100 + strlen(ID), sizeof(char));

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

void dbSetOffline(char *ID)
{
	// seteaza ID ca fiind OFFLINE (sterge din tabela)
	char *sql;
	sql = (char *)calloc(100 + strlen(ID), sizeof(char));

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
	sql = (char *)calloc(100, sizeof(char));

	sprintf(sql, "DELETE FROM ONLINE;");

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

int dbRequestCheckType(char *ID1, char *ID2, char *type)
{
	// verifica daca exista cereri in REQUESTS de tip TYPE , de la ID1 la ID2
	char *sql;
	sql = (char *)calloc(150 + strlen(ID1) + strlen(ID2), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM REQUESTS WHERE fromUser=\"%s\" AND toUser=\"%s\" AND TYPE=\"%s\";", ID1, ID2, type);

	char *data;

	/* Execute SQL statement */
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

	int count = atoi((char *)&data);
	free(sql);

	return count;
}


int dbFriendCheck(char *ID1, char *ID2)
{
	// verifica daca ID1 il are ca prieten deja pe ID2
	char *sql;
	sql = (char *)calloc(150 + strlen(ID1) + strlen(ID2), sizeof(char));
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

	int count = atoi((char *)&data);
	free(sql);

	return count;
}

void dbRequestSend(char *ID1, char *ID2, char *type, char *friendtype)
{
	// adauga o cerere de tip TYPE de la ID1 la ID2, daca e de tip friend, adauga tipul FRIENDTYPE de prieten
	char *sql;
	sql = (char *)calloc(100 + strlen(ID1) + strlen(ID2) + strlen(type), sizeof(char));

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

int dbRequestCheckCount(char *ID)
{
	//verifica daca ID are cereri de tip TYPE > returneaza nr lor
	char *sql;
	sql = (char *)calloc(150 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM REQUESTS WHERE toUser=\"%s\";", ID);

	char *data;

	/* Execute SQL statement */
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

	int count = atoi((char *)&data);
	free(sql);

	return count;
}


void dbRequestCheck(char *ID, int sC, int length)
{
	//intoarce requesturile primite de ID catre sC
	char *sql;
	sql = (char *)calloc(150 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT fromUser,type FROM REQUESTS WHERE toUser=\"%s\";", ID);

	int data[3];
	data[0] = sC;
	data[1] = length;

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, (void *)cbDynamicSender, data, &zErrMsg);
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

void dbInsertFriend(char *ID1, char *ID2, char *friendtype)
{
	// adauga in FRIENDS, ID2 prieten pentru ID1, tip FRIENDTYPE
	char *sql;
	sql = (char *)calloc(100 + strlen(ID1) + strlen(ID2) + strlen(friendtype), sizeof(char));

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

char *dbGetFTypeFromReq(char *ID1, char *ID2)
{
	// intoarce tipul de prieten TYPE pus de ID1 in cererea catre ID2
	char *sql;
	sql = (char *)calloc(80 + strlen(ID1) + strlen(ID2), sizeof(char));
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

void dbDeleteRequestType(char *ID1, char *ID2, char *type)
{
	// sterge cererile de tip TYPE din tabela REQUESTS de la ID1 la ID2
	char *sql;
	sql = (char *)calloc(80 + strlen(ID1) + strlen(ID2) + strlen(type), sizeof(char));

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

int dbFriendsCount(char *ID)
{
	//intoarce numarul de prieteni a lui ID
	char *sql;
	sql = (char *)calloc(50 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM FRIENDS WHERE owner=\"%s\";", ID);

	char *data;

	/* Execute SQL statement */
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

	int count = atoi((char *)&data);
	free(sql);

	return count;
}

void dbFriends(char *ID, int sC, int length)
{
	//intoarce prietenii lui ID / paseaza lui cbSmartFriends desc. sC
	char *sql;
	sql = (char *)calloc(60 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT friend,type FROM FRIENDS WHERE owner=\"%s\";", ID);

	int data[3];
	data[0] = sC;
	data[1] = length;

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, (void *)cbDynamicSender, data, &zErrMsg);
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

int dbOnlineCount(char *ID)
{
	//intoarce numarul de prieteni online a lui ID
	char *sql;
	sql = (char *)calloc(90 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM FRIENDS F, ONLINE O WHERE F.friend=O.id AND F.owner = \"%s\";", ID);

	char *data;

	/* Execute SQL statement */
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



	int count = atoi((char *)&data);
	free(sql);

	return count;
}

void dbOnline(char *ID, int sC, int length)
{
	//intoarce numarul de prieteni online a lui ID
	char *sql;
	sql = (char *)calloc(110 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT F.friend,F.type FROM FRIENDS F, ONLINE O WHERE F.friend = O.id AND F.owner = \"%s\";", ID);

	int data[3];
	data[0] = sC;
	data[1] = length;

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, (void *)cbDynamicSender, data, &zErrMsg);
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

void dbInsertPost(char *ID, char *post, char *posttype)
{
	//insereaza postarea POST in POSTS de tipul POSTTYPE, pentru userul ID
	char *sql;
	sql = (char *)calloc(70 + strlen(ID) + strlen(post) + strlen(posttype), sizeof(char));
	sprintf(sql, "INSERT INTO POSTS(user,post,type) VALUES (\"%s\",\"%s\",\"%s\");", ID, post, posttype);

	/* Execute SQL statement */
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

	return;
}

void dbSetProfile(char *ID, char *value, char *col)
{
	//schimba in atributul COL valoare in VALUE pentru userul ID
	char *sql;
	sql = (char *)calloc(70 + strlen(ID) + strlen(value) + strlen(col), sizeof(char));
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

char *dbGetUserType(char *ID)
{
	// intoarce tipul de prieten TYPE pus de ID1 in cererea catre ID2
	char *sql;
	sql = (char *)calloc(80 + strlen(ID), sizeof(char));
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

char *dbGetFType(char *ID1, char *ID2)
{
	// intoarce tipul de prieten TYPE pus de ID1 pentru ID2 in FRIENDS
	char *sql;
	sql = (char *)calloc(70 + strlen(ID1) + strlen(ID2), sizeof(char));
	sprintf(sql, "SELECT type FROM FRIENDS WHERE owner=\"%s\" AND friend=\"%s\";", ID1, ID2);

	char *data;
	data = calloc(5, sizeof(char));

	/* Execute SQL statement */
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

void dbGetInfoUser(char *ID, int sC)
{
	// trimite la sC toate informatiile despre ID din tabela USERS
	char *sql;
	sql = (char *)calloc(70 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT ID,FULLNAME,SEX,ABOUT,TYPE FROM USERS WHERE ID=\"%s\"", ID);

	int data[3];
	data[0] = sC;

	/* Execute SQL statement */
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

int dbGetPostsCount(char *ID, char *posttype, int limit)
{
	//intoarce numarul de postari ale lui ID in conformitate cu isFriend,usertype si limit

	char *sql;
	sql = (char *)calloc(80 + strlen(ID), sizeof(char));

	sprintf(sql, "SELECT COUNT(*) FROM POSTS WHERE user=\"%s\" AND type<=\"%s\" LIMIT %d;", ID, posttype, limit);

	char *data;

	/* Execute SQL statement */
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

	int count = atoi((char *)&data);
	free(sql);

	return count;
}


void dbGetPosts(char *ID, int sC, char *posttype, int limit)
{
	// trimite la sC toate informatiile despre ID din tabela USERS
	char *sql;
	sql = (char *)calloc(70 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT post FROM POSTS WHERE user=\"%s\" AND type<=\"%s\" LIMIT %d;", ID, posttype, limit);

	int data[3];
	data[0] = sC;

	/* Execute SQL statement */
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
