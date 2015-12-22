#ifndef VSOC_H
#define VSOC_H

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


#define PORT 6047

typedef struct thData {
  int idThread;
  int client;
} thData; // structura pentru thread-uri

void answer(void *arg); // functie de deservire pentru fiecare client

sqlite3 *db;
int rc;
char *zErrMsg;
//
// functions server
int cbRegCheckUser(char *, int, char **, char **);
int callback(void *, int, char **, char **);
int dbInsertUser(char *, char *, char *, char *, char *, char *);
int dbRegCheckUser(char *);
int createConnSocketR();
void register_now(int);
void login(int);
int logout(int);
void viewProfile(int);
void addFriend(int);
void addPost(int);
void answer(void *);

#endif
