#include "vsoc.h"


/* TYPE OF REQUESTS: 1-AddFriend, 2-Chat Invite */
char results[50000];


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


int cbCheck(char *data, int argc, char **argv, char **azColName)
{
	//callback - numarator de linii
	strcpy(data, argv[0]);
	return 0;
}

int cbSelect(char *data, int argc, char **argv, char **azColName)
{
	//callback - intoarce interogari
	int i;

	printf("apelata!\n");

	for (i = 0; i < argc; i++)
	{
		printf("'%s' = '%s'	", azColName[i], argv[i] ? argv[i] : "NULL");
		strcat(results, argv[i]);
	}
	strcat(results, " ");
	printf("\n");
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
	rc = sqlite3_exec(db, sql, cbCheck, &data, &zErrMsg);
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

int dbLogCheckUser(char *ID)
{
	//verifica daca exista un user cu (exact!)ID in USERS, pentru logare
	char *sql;
	sql = (char *)calloc(70 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM USERS WHERE ID=\"%s\";", ID);

	char *data;

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, cbCheck, &data, &zErrMsg);
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
	// printf("dbLogCheckUser:%d\n", atoi((char *)&data));
	return atoi((char *)&data);
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
	rc = sqlite3_exec(db, sql, cbCheck, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbLogCheck:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbLogCheck:Succes \n");
	}
	// printf("dbLogCheck:%d\n", atoi((char *)&data));
	free(sql);
	return atoi((char *)&data);
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
	rc = sqlite3_exec(db, sql, cbCheck, &data, &zErrMsg);
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


int dbFriendCheck(char *ID1, char *ID2)
{
	// verifica daca ID1 il are ca prieten deja pe ID2
	char *sql;
	sql = (char *)calloc(150 + strlen(ID1) + strlen(ID2), sizeof(char));
	sprintf(sql, "SELECT COUNT(*) FROM FRIENDS WHERE owner=\"%s\" AND friend=\"%s\";", ID1, ID2);

	char *data;

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, cbCheck, &data, &zErrMsg);
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
		free(sql);
	}

	fprintf(stdout, "dbRequestSend:Succes \n");

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
	rc = sqlite3_exec(db, sql, cbCheck, &data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbRequestCheckCount:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbRequestCheckCount %s :Succes \n");
	}

	free(sql);
	printf("numaru de cereri a lui '%s' : %d\n", ID, atoi((char *)&data));
	fflush(stdout);
	return atoi((char *)&data);
}


char *dbRequestCheck(char *ID)
{
	//verifica daca ID are cereri de tip TYPE
	char *sql;
	sql = (char *)calloc(150 + strlen(ID), sizeof(char));
	sprintf(sql, "SELECT fromUser,type FROM REQUESTS WHERE toUser=\"%s\";", ID);

	memset(results, 0, sizeof(results));

	//TODO: returneaza cumva interogarile !
	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, cbSelect, NULL, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "dbRequestCheck:Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "dbRequestCheck :Succes \n");
	}

	free(sql);

	//printf("results:%s", results);
	fflush(stdout);
	return results;
}
