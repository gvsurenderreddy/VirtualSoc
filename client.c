#include "lib/vsoc.h"

#define CLIENT 1

int
main(int argc, char *argv[]) {

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
