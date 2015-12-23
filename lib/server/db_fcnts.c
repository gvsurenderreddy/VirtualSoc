#include "vsoc.h"

int callback(void *data, int argc, char **argv, char **azColName) {
  int i;

  fprintf(stderr, "%s: ", (const char *)data);
  for (i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

int cbCheck(char *data, int argc, char **argv, char **azColName) {
  strcpy(data, argv[0]);
  return 0;
}

void dbInsertUser(char *ID, char *PASS, char *FULLNAME, char *SEX, char *ABOUT,
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
    fprintf(stderr, "dbInsertUser:Error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "dbInserUser:Succes\n");
  }

  free(sql);
}

int dbRegCheckUser(char *ID) {
  char *sql;
  sql = (char *)calloc(70 + strlen(ID), sizeof(char));
  sprintf(sql, "SELECT COUNT(*) FROM USERS WHERE LOWER(ID)=LOWER(\"%s\");", ID);

  char *data;

  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql, cbCheck, &data, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "dbRegCheckUser:Error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "dbRegCheckUseR:Succes \n");
  }

  free(sql);
  return atoi((char *)&data);
}

int dbLogCheckUser(char *ID) {
  char *sql;
  sql = (char *)calloc(70 + strlen(ID), sizeof(char));
  sprintf(sql, "SELECT COUNT(*) FROM USERS WHERE ID=\"%s\";", ID);

  char *data;

  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql, cbCheck, &data, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "dbLogCheckUser:Error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "dbLogCheckUser:Succes \n");
  }

  free(sql);
  // printf("dbLogCheckUser:%d\n", atoi((char *)&data));
  return atoi((char *)&data);
}

int dbLogCheck(char *ID, char *PASS) {
  char *sql;
  sql = (char *)calloc(70 + strlen(ID), sizeof(char));
  sprintf(sql, "SELECT COUNT(*) FROM USERS WHERE ID=\"%s\" AND PASS=\"%s\";",
          ID, PASS);

  char *data;

  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql, cbCheck, &data, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "dbLogCheck:Error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "dbLogCheck:Succes \n");
  }
  // printf("dbLogCheck:%d\n", atoi((char *)&data));
  free(sql);
  return atoi((char *)&data);
}

int dbSetOnline(char *ID) {
  char *sql;
  sql = (char *)calloc(100 + strlen(ID), sizeof(char));

  sprintf(sql, "INSERT INTO ONLINE (ID) "
               "VALUES (\"%s\");",
          ID);
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "dbSetOnline:Error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    free(sql);
    return 0;
  }

  fprintf(stdout, "dbSetOnline:Succes \n");

  free(sql);
  return 1;
}

void dbSetOffline(char *ID) {
  char *sql;
  sql = (char *)calloc(100 + strlen(ID), sizeof(char));

  sprintf(sql, "DELETE FROM ONLINE WHERE ID=\"%s\";", ID);

  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "dbSetOffline:Error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "dbSetOffline:Succes \n");
  }
  free(sql);
}
