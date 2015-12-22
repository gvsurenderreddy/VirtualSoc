#define SERVER 1 
#include "../vsoc.h"

int cbRegCheckUser(int *data, int argc, char **argv, char **azColName) {
  strcpy(data, argv[0]);
  return 0;
}

int dbRegCheckUser(char *ID) {
  char *sql;
  sql = (char *)calloc(70 + strlen(ID), sizeof(char));
  sprintf(sql, "SELECT COUNT(*) FROM USERS WHERE LOWER(ID)=LOWER('%s');", ID);

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

int callback(void *data, int argc, char **argv, char **azColName) {
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
