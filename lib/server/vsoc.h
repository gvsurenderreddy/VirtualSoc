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
#include <stdbool.h>
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
ssize_t safePrefRead(int, void *);
ssize_t safePrefWrite(int, const void *);
ssize_t safeRead(int, void *, size_t);
ssize_t safeWrite(int, const void *, size_t);
int createConnSocketR();
void register_now(int);
void login(int, char *);
void logout(int, char *);
void viewProfile(int, const char *);
void addFriend(int, const char *);
void addPost(int, const char *);
void setProfile(int, const char *);
void checkReq(int, const char *);
void accFriend(int, const char *);
void accChat(int, const char *);
void friends(int, const char *);
void online(int, const char *);
void quit(int, char *);
__sighandler_t forcequit(void);
void answer(void *);

//--functii server DB SQLITE3--//
int callback(void *, int, char **, char **);
int cbSingle(char *, int, char **, char **);
int cbDSlines(int *, int, char **, char **);
void dbInsertUser(const char *, const char *, const char *, const char *, const char *, const char *);
int dbRegCheckUser(const char *);
int dbCheckUser(const char *);
int dbLogCheck(const char *, const char *);
int dbSetOnline(const char *);
void dbSetOffline(const char *);
void dbForceQuit(void);
int dbRequestCheckType(const char *, const char *, const char *);
int dbRequestCheckCount(const char *);
void dbRequestCheck(const char *, int);
void dbRequestSend(const char *, const char *, const char *, const char *);
int dbFriendCheck(const char *, const char *);
void dbInsertFriend(const char *, const char *, const char *);
char *dbGetFTypeFromReq(const char *, const char *);
void dbDeleteRequestType(const char *, const char *, const char *);
int dbFriendsCount(const char *);
void dbFriends(const char *, int);
int dbOnlineCount(const char *);
void dbOnline(const char *, int);
void dbInsertPost(const char *, const char *, const char *);
void dbSetProfile(const char *, const char *, const char *);
char *dbGetUserType(const char *);
char *dbGetFType(const char *, const char *);
void dbGetInfoUser(const char *, int);
int dbGetPostsCount(const char *, const char *, int);
void dbGetPosts(const char *, int, const char *, int);
// end_functii server

#endif
