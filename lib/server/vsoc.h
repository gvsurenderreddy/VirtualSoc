#ifndef VSOC_H
#define VSOC_H

// librarii
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <netdb.h>
#include <pthread.h>
#include <sqlite3.h>
#include <assert.h>
#include <stdio_ext.h>
// end_librarii

#define PORT 6047

// globale server
// structura pentru thread-uri
typedef struct thData
{
	int idThread;
	int client;
} thData;

sqlite3 *db;
int rc;
char *zErrMsg;

// end_globare server

// functii server
int createConnSocketR();
void register_now(int);
void login(int, char *);
void logout(int, char *);
void viewProfile(int);
void addFriend(int, char *);
void addPost(int);
void setProfile(int, char *);
void checkReq(int, char *);
void accFriend(int, char *);
void accChat(int, char *);
void friends(int, char *);
void quit(int, char *);
void forcequit(void);
void answer(void *);

//--functii server DB SQLITE3--//
int callback(void *, int, char **, char **);
int cbSingle(char *, int, char **, char **);
int cbDynamicSender(int *, int, char **, char **);

void dbInsertUser(char *, char *, char *, char *, char *, char *);
int dbRegCheckUser(char *);
int dbLogCheckUser(char *);
int dbLogCheck(char *, char *);
int dbLog(char *);
int dbSetOnline(char *);
void dbSetOffline(char *);
void dbForceQuit(void);
int dbRequestCheckType(char *, char *, char *);
int dbRequestCheckCount(char *);
void dbRequestCheck(char *, int, int);
void dbRequestSend(char *, char *, char *, char *);
int dbFriendCheck(char *, char *);
void dbFriendInsert(char *, char *, char *);
char *dbGetFTypeFromReq(char *, char *);
void dbDeleteRequestType(char *, char *, char *);
int dbFriendsCount(char *);
void dbFriends(char *, int, int);
int dbOnlineCount(char *);
// end_functii server

#endif
