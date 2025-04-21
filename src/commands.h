#include <sqlite3.h>
#include <stdio.h>
#include <time.h>

#ifndef COMMANDS_H
#define COMMANDS_H
typedef struct {
  struct tm timestamp;
  char *task;
} Task;

extern int create_new_task(sqlite3 *db, Task task);
extern void print_time(struct tm timestamp);
extern void print_task(Task task);
extern Task add_task(sqlite3 *db, char **task_name);
extern int remove_task(sqlite3 *db, int id);
extern int list_tasks(sqlite3 *db);
#endif
