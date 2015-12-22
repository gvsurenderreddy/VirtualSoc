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
#include <errno.h>
#include <signal.h>
#include <netdb.h>
#include <assert.h>
#include <stdio_ext.h>

#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

extern int errno;
int port;

void myRead(char *text, int size) {
       char buf[BUFSIZ];
       fgets(buf, sizeof(buf), stdin);
       strncpy(text, buf, size);
       text[size] = 0;
       text[strlen(text) - 1] = 0;
}

void help(int check) {
       printf("\n Available commands :\n /login\n /logout\n /register\n "
              "/quit\n "
              "/viewProfile\n /addFriend\n /addPost\n\n");
}

void flushStdin(void) {
       char ch = 0;

       while ((ch = getchar()) != EOF && ch != '\n')
              ;
}

int login(int sC, int check, char *user) {
       char username[33], password[33];
       int resultAnswer = -1;
       memset(username, 0, 33);
       memset(password, 0, 33);
       write(sC, &check, sizeof(int));
       if (check == 1) {
              printf("Already logged in !\n");
              return 1;
       } else {
              printf("Username: ");
              fflush(stdout);
              myRead(username, 33);

              // printf("Password: ");
              // fflush(stdout);
              // myRead(password, 33);

              // test getpass
              char *testpass = getpass("Password: ");
              if (strlen(testpass) > 32) {
                     printf("Password longer than 32 ! But sent to server.\n");
              }
              // fin_test getpass
              printf("User:%s\n Pass:%s\n", username, testpass);
              write(sC, username, sizeof(username));
              write(sC, password, sizeof(password));

              read(sC, &resultAnswer, sizeof(int));
              // trateaza 101,102,11
              if (resultAnswer == 11) {
                     printf("Succesfully logged in ! \n");
                     strcpy(user, username);
                     return 1;
              } else {
                     printf("Login failed !\n");
                     return 0;
              }
       }
}

void register_now(int sC, int check) {
       int resultAnswer = -1;

       char id[33], *invPass, password[33], fullname[65], sex[5], about[513],
           type[17];

       write(sC, &check, sizeof(int));
       if (check == 1) {
              printf("You're logged in ! Please log out to register a new "
                     "account !\n");
              return;
       } else {
              memset(id, 0, 33);
              memset(password, 0, 33);
              memset(fullname, 0, 33);
              memset(sex, 0, 5);
              memset(about, 0, 33);
              memset(type, 0, 33);

              printf("ID (10-32 ch. alpha-numerical): ");
              fflush(stdout);
              myRead(id, 33);

              invPass = getpass("Password (10-32 ch.): ");
              strcpy(password, invPass);

              printf("Fullname (10-64 ch. alpha-numerical and spaces): ");
              fflush(stdout);
              myRead(fullname, 65);

              printf("Sex (F/M): ");
              fflush(stdout);
              myRead(sex, 5);

              printf("About (10-512 ch.):");
              fflush(stdout);
              myRead(about, 513);

              printf("Profile type (public/private):");
              fflush(stdout);
              myRead(type, 17);

              /*printf("Trimit: '%s'\n'%s'\n'%s'\n'%s'\n'%s'\n'%s'\n", id,
                     password, fullname, sex, about, type);*/

              write(sC, id, sizeof(id));
              write(sC, password, sizeof(password));
              write(sC, fullname, sizeof(fullname));
              write(sC, sex, sizeof(sex));
              write(sC, about, sizeof(about));
              write(sC, type, sizeof(type));

              read(sC, &resultAnswer, sizeof(int));

              switch (resultAnswer) {
              case 22: {
                     printf("Register Complete !\n");
                     break;
              }
              case 201: {
                     printf("ID not long enough or invalid ! (min. 10 ch, only "
                            "alpha-numerical)\n");
                     break;
              }
              case 202: {
                     printf("Password not long enough ! (min. 10 ch)\n");
                     break;
              }
              case 203: {
                     printf("Full name not long enough or invalid ! (min. 10 "
                            "ch, only "
                            "alphanumerical and spaces)\n");
                     break;
              }
              case 204: {
                     printf("Invalid sex ! (F/M)\n");
                     break;
              }
              case 205: {
                     printf("Say something 'about' you ! (min. 10 ch)\n");
                     break;
              }
              case 206: {
                     printf("Invalid Profile Type ! (private/public)\n");
                     break;
              }
              case 207: {
                     printf("ID already exists !\n");
                     break;
              }
              case 208: {
                     printf("Use of double quotes ( \" ) is forbidden ! Please "
                            "use single quotes ( ' ) !\n");
                     break;
              }
              }
       }
}

void viewProfile(int sC, int check) {
       int resultAnswer = -1;
       char user[32], post[64];
       memset(user, 0, 32);
       memset(post, 0, 64);
       if (check == 0)
              printf(
                  "You can only view public users ! You're not logged in ! \n");
       else
              printf(
                  "You can view public/private users ! You're logged in ! \n");

       printf("View profile of user: ");
       fflush(stdout);
       myRead(user, 32);

       write(sC, &check, sizeof(int));
       write(sC, user, sizeof(user));

       read(sC, &resultAnswer, sizeof(int));
       if (resultAnswer == 401)
              printf("Not logged\n");
       if (resultAnswer == 44)
              printf("Logged\n");
       if (resultAnswer == 401)
              printf("The user is private ! You cannot view his profile !\n");

       if (resultAnswer == 44) {
              printf("Succesfull View\n");
              read(sC, post, sizeof(post));
              printf("Posts: %s\n", post);
       }
}

int logout(int sC, int check, char *user) {
       write(sC, &check, sizeof(int));
       if (check == 0) {
              printf("You're not logged in ! \n");
       } else {
              printf("Logged out succesfully !\n");
       }
       memset(user, 0, 33);
       return 0;
}

void addFriend(int sC, int check) {
       int resultAnswer = -1;
       char user[33], friendType[33];
       memset(user, 0, 33);
       memset(friendType, 0, 33);

       write(sC, &check, sizeof(int));
       if (check == 0) {
              printf("You're not logged in !\n");
              return;
       } else {
              printf("Add: ");
              fflush(stdout);
              myRead(user, 33);

              printf("Group - frn(friends) / cfr(close-friends) / fam "
                     "(family)): ");
              fflush(stdout);
              myRead(friendType, 33);

              write(sC, user, sizeof(user));
              write(sC, friendType, sizeof(friendType));

              read(sC, &resultAnswer, sizeof(int));

              switch (resultAnswer) {
              case 66: {
                     printf("'%s' added ! \n", user);
                     break;
              }
              case 601: {
                     printf("'%s' does not exist in our "
                            "database !\n",
                            user);
                     break;
              }
              case 602: {
                     printf("'%s' is already in your friends "
                            "list !\n",
                            user);
              }
              default: { return; }
              }
       }
}

void addPost(int sC, int check) {
       int resultAnswer = -1;
       char post[257], postType[33];
       memset(post, 0, 257);
       memset(postType, 0, 33);

       write(sC, &check, sizeof(int));
       if (check == 0) {
              printf("You're not logged in !\n");
              return;
       } else {

              printf("Text:");
              fflush(stdout);
              myRead(post, 257);

              printf("To - all (even not logged in) / pub (even not friend) "
                     "/ frn (friends) / "
                     "cfr (close friends) / fam (family) :");
              fflush(stdout);
              myRead(postType, 33);

              write(sC, post, sizeof(post));
              write(sC, postType, sizeof(postType));

              read(sC, &resultAnswer, sizeof(int));

              switch (resultAnswer) {
              case 77: {
                     printf("Post added succesfully !\n");
                     break;
              }
              case 701: {
                     printf("Failed to add post !\n");
                     break;
              }
              default: { return; }
              }
       }
}

void setProfile(int sC, int check) {
       int resultAnswer = -1;

       write(sC, &check, sizeof(int));
       if (check == 0) {
              printf("You're not logged in ! \n");
       } else {
       }
}

int encodeCommand(char *clientCommandChar) {
       if (strcmp(clientCommandChar, "/login") == 0)
              return 1;
       if (strcmp(clientCommandChar, "/register") == 0)
              return 2;
       if (strcmp(clientCommandChar, "/quit") == 0)
              return 0;
       if (strcmp(clientCommandChar, "/help") == 0)
              return 3;
       if (strcmp(clientCommandChar, "/viewProfile") == 0)
              return 4;
       if (strcmp(clientCommandChar, "/logout") == 0)
              return 5;
       if (strcmp(clientCommandChar, "/addFriend") == 0)
              return 6;
       if (strcmp(clientCommandChar, "/addPost") == 0)
              return 7;
       if (strcmp(clientCommandChar, "/setProfile") == 0)
              return 8;
       if (strcmp(clientCommandChar, "/chat") == 0)
              return 9;
       return -1;
}

void quit() {

       printf("\nQuit!\n");
       exit(0);
}

int main(int argc, char *argv[]) {

       if (argc != 3) {
              printf("[client] Run:   %s ServerAddr Port \n", argv[0]);
              return -1;
       }
       int socketConnect;
       if ((socketConnect = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
              perror("[client]Socket creation error ! socket(). \n");
              return errno;
       }

       port = atoi(argv[2]);
       struct sockaddr_in server;
       server.sin_family = AF_INET;
       server.sin_addr.s_addr = inet_addr(argv[1]);
       server.sin_port = htons(port);

       if (connect(socketConnect, (struct sockaddr *)&server,
                   sizeof(struct sockaddr)) == -1) {
              perror("[client]Connect error ! connect(). \n");
              return errno;
       }

       //##############################CLIENT##################################
       signal(SIGINT, quit);
       int clientCommand = -1;
       char clientCommandChar[1000];
       char currentUser[33];
       int logged = 0;
       printf(RED "	Welcome to VirtualSoc ~ by Cristea Alexandru\n	/help  - "
                  " syntax and available commands !\n" RESET);
       while (1) {

              if (logged == 0) {
                     printf(RED "[offline] @ Command >>: " RESET);
                     fflush(stdout);
              } else if (logged == 1) {
                     printf(GREEN "%s @ Command >>: " RESET, currentUser);
                     fflush(stdout);
              }
              memset(clientCommandChar, 0, sizeof(clientCommandChar));

              read(0, clientCommandChar, sizeof(clientCommandChar));

              if (clientCommandChar[0] == '\n') {

                     continue;
              }

              clientCommandChar[strlen(clientCommandChar) - 1] = 0;
              clientCommand = encodeCommand(clientCommandChar);
              if (clientCommand == -1) {
                     printf("Wrong command ! Check /help ! \n Commands are "
                            "case-sensitive. \n");
                     continue;
              }
              if (clientCommand == 3) {
                     help(logged);
                     continue;
              }

              if (write(socketConnect, &clientCommand, sizeof(int)) <= 0) {
                     perror("[client]Write error to server ! \n");
                     return errno;
              }

              if (clientCommand == 0) {
                     printf("[client]Quit! \n");
                     break;
              }
              if (clientCommand == 1)
                     logged = login(socketConnect, logged, currentUser);
              if (clientCommand == 2)
                     register_now(socketConnect, logged);
              if (clientCommand == 4)
                     viewProfile(socketConnect, logged);
              if (clientCommand == 5)
                     logged = logout(socketConnect, logged, currentUser);
              if (clientCommand == 6)
                     addFriend(socketConnect, logged);
              if (clientCommand == 7)
                     addPost(socketConnect, logged);
              if (clientCommand == 8)
                     setProfile(socketConnect, logged);
              /*  pid_t readSlave;
  if ((readSlave = fork()) < 0) {
         perror("[client]Fork error ! fork(). \n");
         return errno;
  }
  if (readSlave == 0) {
         int serverResult;
         if (read(socketConnect, &serverResult,
  sizeof(int)) <= 0)
  {
                perror("[client]Read error from server !
  \n");
                return errno;
         }
         printf("[client]Result = %d \n", serverResult);
         exit(1);
  } // readSlave - client
  else {
         wait(NULL);

  } // main - client*/
       }
}
