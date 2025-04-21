#include "commands.h"
#include "database.h"
#include <errno.h>
#include <limits.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

char *database_path = NULL;
char *dir_path = NULL;

void cleanup() {
  free(database_path);
  free(dir_path);
}

void init_database_path(void) {
  char *home = getenv("HOME");
  if (!home)
    return;

  // Allocate space for full dir path
  dir_path = malloc(strlen(home) + strlen("/.local/state/todo-c") + 1);
  if (!dir_path)
    return;
  strcpy(dir_path, home);
  strcat(dir_path, "/.local/state/todo-c");

  // Allocate space for database path
  database_path = malloc(strlen(dir_path) + strlen(DATABASE) + 1);
  if (!database_path) {
    free(dir_path);
    return;
  }
  strcpy(database_path, dir_path);
  strcat(database_path, DATABASE);
}

int init_db(sqlite3 **db) {
  int rc = sqlite3_open(database_path, db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
    return rc;
  }

  const char *sql = "CREATE TABLE IF NOT EXISTS tasks ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "task TEXT NOT NULL,"
                    "createdAt TIMESTAMP NOT NULL);";

  char *err_msg = 0;
  rc = sqlite3_exec(*db, sql, 0, 0, &err_msg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
    return rc;
  }

  sqlite3_close(*db);
  return SQLITE_OK;
}

int setup(sqlite3 **db) {
  struct stat st = {0};
  if (stat(dir_path, &st) == -1) {
    int rc = mkdir(dir_path, 0700);
    if (rc != 0) {
      fprintf(stderr, "There was a problem creating directory.\n");
      return rc;
    }
  }

  int rc = init_db(db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Something went wrong initiating database\n");
    return rc;
  }

  return rc;
}

int parse_int_argv(char *argv) {
  char *p;
  int num;
  errno = 0;
  long conv = strtol(argv, &p, 10);
  if (errno != 0 || *p != '\0' || conv > INT_MAX || conv < INT_MIN) {
    fprintf(stderr, "Something went wrong when parsing int. Error: %d\n",
            errno);
    return errno;
  }
  num = conv;
  return num;
}

int main(int argc, char *argv[]) {
  init_database_path();

  if (!database_path) {
    fprintf(stderr, "Failed to initialize paths\n");
    return 1;
  }

  sqlite3 *db = {NULL};
  int rc = setup(&db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Something went wrong at initial setup\n");
    return rc;
  }

  char **command;
  if (argc >= 2) {
    command = &argv[1];

    if (argc >= 3 && strcmp(*command, "add") == 0) {
      char **task_name = &argv[2];
      Task task = add_task(db, task_name);
      print_task(task);
    } else if (argc >= 3 && (strcmp(*command, "remove") == 0 ||
                             strcmp(*command, "rm") == 0)) {
      int id = parse_int_argv(argv[2]);

      if (id == errno) {
        fprintf(stderr, "action parameter is invalid\n");
        return -1;
      }

      remove_task(db, id);
      list_tasks(db);
    } else if (argc < 2 ||
               (strcmp(*command, "list") == 0 || strcmp(*command, "ls") == 0)) {
      list_tasks(db);
    } else {
      // print_instructions();
    }
  }
  sqlite3_close(db);
  return 0;
}
