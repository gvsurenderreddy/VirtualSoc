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

#define PORT 6047

extern int errno;

typedef struct thData {
       int idThread;
       int client;
} thData;
static void *treat(void *arg);
void answer(void *arg);

sqlite3 *db;
int rc;
char *zErrMsg = 0;

static int cbRegCheckUser(int *data, int argc, char **argv, char **azColName) {
       strcpy(data, argv[0]);
       return 0;
}

int dbRegCheckUser(char *ID) {
       char *sql;
       sql = (char *)calloc(70 + strlen(ID), sizeof(char));
       sprintf(sql, "SELECT COUNT(*) FROM USERS WHERE LOWER(ID)=LOWER('%s');",
               ID);

       char *data;

       /* Execute SQL statement */
       rc = sqlite3_exec(db, sql, cbRegCheckUser, &data, &zErrMsg);
       if (rc != SQLITE_OK) {
              fprintf(stderr, "SQL error: %s\n", zErrMsg);
              sqlite3_free(zErrMsg);
       } else {
              fprintf(stdout, "Operation done successfully \n");
       }

       int check = atoi((char *)&data);
       printf("check:%d", check);
       return check;
}

static int callback(void *data, int argc, char **argv, char **azColName) {
       int i;

       fprintf(stderr, "%s: ", (const char *)data);
       for (i = 0; i < argc; i++) {
              printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
       }
       printf("\n");
       return 0;
}

int dbInsertUser(char *ID, char *PASS, char *FULLNAME, char *SEX, char *ABOUT,
                 char *TYPE) {
       char *sql;
       sql = (char *)calloc(100 + strlen(ID) + strlen(PASS) + strlen(FULLNAME) +
                                strlen(SEX) + strlen(ABOUT) + strlen(TYPE) + 10,
                            sizeof(char));

       sprintf(sql, "INSERT INTO USERS (ID,PASS,FULLNAME,SEX,ABOUT,TYPE) "
                    "VALUES (\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");",
               ID, PASS, FULLNAME, SEX, ABOUT, TYPE);
       rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
       if (rc != SQLITE_OK) {
              fprintf(stderr, "SQL error: %s\n", zErrMsg);
              sqlite3_free(zErrMsg);
       } else {
              fprintf(stdout, "Records created successfully\n");
       }
       free(sql);
}

int createConnSocketR() {
       int sd;
       if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
              perror("[server] Socket creation error ! socket(). \n");
              printf("[server] Errno: %d", errno);
              exit(0);
       }
       int on = 1;
       setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
       return sd;
}

int main(int argc, char *argv[]) {
       int nr, pid, i = 0;
       pthread_t th[100];
       if (argc < 2) {
              fprintf(stderr,
                      "Usage: ./server 1/0 (1 for normal, 0 for debugging)\n");
              exit(0);
       }
       // conectare DB

       rc = sqlite3_open("vsoc.db", &db);

       if (rc) {
              fprintf(stderr, "Can't open Database %s !\n", sqlite3_errmsg(db));
              exit(0);
       } else {
              fprintf(stdout, "Opened database succesfully !\n");
       }

       if (atoi(argv[1]) == 1) { // normal mode
              printf("Am intrat in normal !\n");
              struct sockaddr_in server;
              struct sockaddr_in from;
              memset(&server, 0, sizeof(server));
              memset(&from, 0, sizeof(from));
              server.sin_family = AF_INET;
              server.sin_addr.s_addr = htonl(INADDR_ANY);
              server.sin_port = htons(PORT);

              int socketConnect = createConnSocketR();
              if (bind(socketConnect, (struct sockaddr *)&server,
                       sizeof(struct sockaddr)) == -1) {
                     perror("[server]Socket bind error ! bind(). \n");
                     printf("[server]Errno: %d", errno);
                     exit(0);
              }

              if (listen(socketConnect, 10) == -1) {
                     perror("[server]Socket listen error ! listen(). \n");
                     printf("[server]Errno: %d", errno);
                     exit(0);
              }

              printf("[server]Waiting at port %d \n", PORT);
              fflush(stdout);
              while (1) {
                     int socketClient;
                     thData *td;
                     int length = sizeof(from);

                     if ((socketClient =
                              accept(socketConnect, (struct sockaddr *)&from,
                                     &length)) < 0) {
                            perror("[server]Connection accept error ! "
                                   "accept(). \n");
                            continue;
                     }
                     int idThread;
                     int client;
                     td = (struct thData *)malloc(sizeof(struct thData));
                     td->idThread = i++;
                     td->client = socketClient;

                     pthread_create(&th[i], NULL, &treat, td);
              } // while - connection
       }

       if (atoi(argv[1]) == 0) { // debugging mode
              printf("Am intrat in debug !\n");
              dbInsertUser("User3", "pass3", "User Three", "F",
                           "Description Three", "public");
              dbRegCheckUser("usER3");
              sqlite3_close(db);
              return 0;
       }
       sqlite3_close(db);
       return 0;
}

static void *treat(void *arg) {
       struct thData tdL;
       tdL = *((struct thData *)arg);
       printf("[thread]- %d - Waiting for message. \n", tdL.idThread);
       fflush(stdout);
       pthread_detach(pthread_self());
       answer((struct thData *)arg);
       close((int)arg);
       return (NULL);
}

void login(int sC) {
       int resultAnswer = -1;
       int check;
       char username[33], password[33];
       memset(username, 0, 33);
       memset(password, 0, 33);

       read(sC, &check, sizeof(int));
       if (check == 1)
              return;
       else {
              read(sC, username, sizeof(username));
              read(sC, password, sizeof(password));
              // verifica user, parola.
              // 101 - user inexistent, 102 - user/parola gresite, 11 -
              // logare
              // reusita
              resultAnswer = 11;
              write(sC, &resultAnswer, sizeof(int));
       }
}

void register_now(int sC) {
       int resultAnswer = -1;
       int check;
       char id[33], password[33], fullname[65], sex[5], about[513], type[17];

       memset(id, 0, 33);
       memset(password, 0, 33);
       memset(fullname, 0, 33);
       memset(sex, 0, 5);
       memset(about, 0, 33);
       memset(type, 0, 33);

       read(sC, &check, sizeof(int));
       if (check == 1)
              return;
       {
              read(sC, id, sizeof(id));
              read(sC, password, sizeof(password));
              read(sC, fullname, sizeof(fullname));
              read(sC, sex, sizeof(sex));
              read(sC, about, sizeof(about));
              read(sC, type, sizeof(type));

              /*    printf("Am primit : '%s'\n'%s'\n'%s'\n'%s'\n'%s'\n'%s'\n",
                 id,
                         password, fullname, sex, about, type);
                  fflush(stdout);*/

              int i;
              resultAnswer = 22;
              if (strlen(id) < 10) {
                     resultAnswer = 201;
                     write(sC, &resultAnswer, sizeof(int));
                     return;
              } else
                     for (i = 0; i < strlen(id); i++) {
                            if (!(id[i] >= 'a' && id[i] <= 'z'))
                                   if (!(id[i] >= 'A' && id[i] <= 'Z'))
                                          if (!(id[i] >= '0' && id[i] <= '9')) {
                                                 resultAnswer = 201;
                                                 write(sC, &resultAnswer,
                                                       sizeof(int));
                                                 return;
                                          }
                     }

              for (i = 0; i < strlen(fullname); i++) {
                     if (!(fullname[i] >= 'a' && fullname[i] <= 'z'))
                            if (!(fullname[i] >= 'A' && fullname[i] <= 'Z'))
                                   if (!(fullname[i] >= '0' &&
                                         fullname[i] <= '9'))
                                          if (!(fullname[i] == ' ')) {
                                                 resultAnswer = 203;
                                                 write(sC, &resultAnswer,
                                                       sizeof(int));
                                                 return;
                                          }
              }
              if (strlen(password) < 10) {
                     resultAnswer = 202;
                     write(sC, &resultAnswer, sizeof(int));
                     return;
              }

              for (i = 0; i < strlen(password); i++) {
                     if (password[i] == '\"') {
                            resultAnswer = 208;
                            write(sC, &resultAnswer, sizeof(int));
                            return;
                     }
              }
              for (i = 0; i < strlen(about); i++) {
                     if (password[i] == '\"') {
                            resultAnswer = 208;
                            write(sC, &resultAnswer, sizeof(int));
                            return;
                     }
              }

              if (strlen(fullname) < 10) {
                     resultAnswer = 203;
                     write(sC, &resultAnswer, sizeof(int));
                     return;
              }
              if ((strlen(sex) != 1) ||
                  (strlen(sex) == 1 && sex[0] != 'F' && sex[0] != 'M')) {
                     resultAnswer = 204;
                     write(sC, &resultAnswer, sizeof(int));
                     return;
              }
              if (strlen(about) < 10) {
                     resultAnswer = 205;
                     write(sC, &resultAnswer, sizeof(int));
                     return;
              }
              if (strcmp(type, "private") != 0 && strcmp(type, "public") != 0) {
                     resultAnswer = 206;
                     write(sC, &resultAnswer, sizeof(int));
                     return;
              }
              if (dbRegCheckUser(id) != 0) {
                     resultAnswer = 207;
                     write(sC, &resultAnswer, sizeof(int));
                     return;
              }
       }

       dbInsertUser(id, password, fullname, sex, about, type);
       write(sC, &resultAnswer, sizeof(int));
       return;
}

void viewProfile(int sC) {
       int resultAnswer = -1;
       int check;
       char user[32];
       memset(user, 0, 32);

       read(sC, &check, sizeof(int));
       read(sC, user, sizeof(user));
       // verifica daca user-ul ce face view este logat
       // 44 - logat si poate vedea orice
       // 401 - nelogat, poate vedea doar publicele
       if (check == 0)
              resultAnswer = 401;
       if (check == 1)
              resultAnswer = 44;
       write(sC, &resultAnswer, sizeof(int));
       if (resultAnswer == 44)
              write(sC, &user, sizeof(user));
}

int logout(int sC) {
       int check;
       read(sC, &check, sizeof(int));

       return 0;
}

void addFriend(int sC) {
       int resultAnswer = 66, check;
       char user[33], friendType[33];
       memset(user, 0, 33);
       memset(friendType, 0, 33);

       read(sC, &check, sizeof(int));
       if (check == 0) {
              return;
       } else {
              read(sC, user, sizeof(user));
              read(sC, friendType, sizeof(friendType));
              if (strcmp(user, "vasi") == 0)
                     resultAnswer = 601;
              if (strcmp(user, "dan") == 0)
                     resultAnswer = 602;
              write(sC, &resultAnswer, sizeof(int));
       }
}

void addPost(int sC) {
       int resultAnswer = 77, check;
       char post[257], postType[33];
       memset(post, 0, 257);
       memset(postType, 0, 33);

       read(sC, &check, sizeof(int));
       if (check == 0) {
              return;
       } else {
              read(sC, post, sizeof(post));
              read(sC, postType, sizeof(postType));

              if (strstr(post, "obscen") != NULL) {
                     resultAnswer = 701;
              }
              write(sC, &resultAnswer, sizeof(int));
       }
}

void answer(void *arg) {
       int clientCommand = -1, clientAnswer = -1, i = 0;
       struct thData tdL;
       tdL = *((struct thData *)arg);
       // citire comanda
       while (clientCommand != 0) {
              clientAnswer = -1;
              if (read(tdL.client, &clientCommand, sizeof(int)) <= 0) {
                     printf("[Thread %d]\n", tdL.idThread);
                     perror("Disconnect \ Eroare la read() de la "
                            "client.\n");
                     break;
              }

              printf("[Thread %d]Received command : %d\n", tdL.idThread,
                     clientCommand);

              // operatie - comanda
              if (clientCommand == 0) {
                     break;
              }

              switch (clientCommand) {
              case 1: {
                     login(tdL.client);
                     break;
              }
              case 2: {
                     register_now(tdL.client);
                     break;
              }
              case 4: {
                     viewProfile(tdL.client);
                     break;
              }
              case 5: {
                     logout(tdL.client);
                     break;
              }
              case 6: {
                     addFriend(tdL.client);
                     break;
              }
              case 7: {
                     addPost(tdL.client);
                     break;
              }
              }
              //
              /*
                            if (write(tdL.client, &clientAnswer,
                 sizeof(int)) <=
                 0) {
                                   printf("[Thread %d] ", tdL.idThread);
                                   perror("[Thread]Write error to
                 client.\n");
                            }

                            else
                                   printf("[Thread %d]Mesajul a fost
                 trasmis cu
                 succes.\n",
                                          tdL.idThread);
                            printf("[Thread %d]Answer for command %d\n",
                 tdL.idThread,
                                   clientAnswer);*/
              // raspuns comanda
       }
}
