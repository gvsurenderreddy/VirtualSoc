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
#include <stdbool.h>

#define RESET "\033[0m"
#define BLACK "\033[30m" /* Black */
#define RED "\033[31m" /* Red */
#define GREEN "\033[32m" /* Green */
#define YELLOW "\033[33m" /* Yellow */
#define BLUE "\033[34m" /* Blue */
#define MAGENTA "\033[35m" /* Magenta */
#define CYAN "\033[36m" /* Cyan */
#define WHITE "\033[37m" /* White */
#define BOLDBLACK "\033[1m\033[30m" /* Bold Black */
#define BOLDRED "\033[1m\033[31m" /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m" /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m" /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m" /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m" /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m" /* Bold White */

#define PORT 6047

int port;

typedef struct thData
{
	int idThread;
	int client;
} thData; // structura pentru thread-uri

void answer(void *arg); // functie de deservire pentru fiecare client

void myRead(char *, int);
void safeStdinRead(const char *, char *, int);
ssize_t safePrefWrite(int, const void *);
ssize_t safePrefRead(int, void *);
ssize_t safeRead(int, void *, size_t);
ssize_t safeWrite(int, const void *, size_t);
void help(int);
int login(int, int, char *);
void register_now(int, int);
void viewProfile(int, bool);
int logout(int, int, char *);
void addFriend(int, int);
void addPost(int, int);
void setProfile(int, int);
void checkReq(int, int);
void accFriend(int, int);
void accChat(int, int);
void friends(int, int);
void online(int, int);
void quit(int, int);
int encodeCommand(const char *);
__sighandler_t quitforce(void);

#endif
