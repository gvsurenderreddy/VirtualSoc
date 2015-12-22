#include "vsoc.h"

int createConnSocketR() 
{
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
              write(sC, &resultAnswer, sizeof(int));
              return;
            }
      }

    for (i = 0; i < strlen(fullname); i++) {
      if (!(fullname[i] >= 'a' && fullname[i] <= 'z'))
        if (!(fullname[i] >= 'A' && fullname[i] <= 'Z'))
          if (!(fullname[i] >= '0' && fullname[i] <= '9'))
            if (!(fullname[i] == ' ')) {
              resultAnswer = 203;
              write(sC, &resultAnswer, sizeof(int));
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
      perror("Disconnect  Eroare la read() de la "
             "client.\n");
      break;
    }

    printf("[Thread %d]Received command : %d\n", tdL.idThread, clientCommand);

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
