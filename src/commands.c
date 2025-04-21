#include "database.h"
#include <sqlite3.h>
#include <stdio.h>
#include <time.h>

typedef struct {
  struct tm timestamp;
  char *task;
} Task;

int create_new_task(sqlite3 *db, Task task) {
  int rc = sqlite3_open(database_path, &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to open database connection: %s\n",
            sqlite3_errmsg(db));
  }
  const char *sql = "INSERT INTO tasks (task, createdAt) VALUES (?, ?);";
  sqlite3_stmt *stmt;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
  }

  rc = sqlite3_bind_text(stmt, 1, task.task, -1, SQLITE_STATIC);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to bind task: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return rc;
  }

  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &task.timestamp);

  rc = sqlite3_bind_text(stmt, 2, timestamp, -1, SQLITE_STATIC);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to bind timestamp: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return rc;
  }

  // Execute statement
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return rc;
  }

  // Clean up
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return SQLITE_OK;
}

void print_time(struct tm timestamp) {
  char buff[20];
  strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &timestamp);
  printf("%s\n", buff);
}

void print_task(Task task) {
  printf("%s\t", task.task);
  print_time(task.timestamp);
}

Task add_task(sqlite3 *db, char **task_name) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  const Task new_task = {
      .task = *task_name,
      .timestamp = tm,
  };

  if (create_new_task(db, new_task) != SQLITE_OK) {
    fprintf(stderr, "Failed creating task in database\n");
  }
  return new_task;
}

int remove_task(sqlite3 *db, int id) {
  int rc = sqlite3_open(database_path, &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to open database connection: %s\n",
            sqlite3_errmsg(db));
  }
  const char *sql = "DELETE FROM tasks WHERE id = ?;";
  sqlite3_stmt *stmt;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return rc;
  }

  rc = sqlite3_bind_int(stmt, 1, id);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to bind ID: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return rc;
  }

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return rc;
  }

  // Check if any rows were actually deleted
  int changes = sqlite3_changes(db);
  if (changes == 0) {
    fprintf(stderr, "No task found with ID %d\n", id);
  } else {
    printf("Successfully deleted task with ID %d\n", id);
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return SQLITE_OK;
}

int list_tasks(sqlite3 *db) {
  int rc = sqlite3_open(database_path, &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to open database connection: %s\n",
            sqlite3_errmsg(db));
  }

  const char *sql =
      "SELECT id, task, createdAt FROM tasks ORDER BY createdAt DESC;";
  sqlite3_stmt *stmt;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
  }

  printf("ID\tCreated At\t\tTask\n");
  printf("------------------------------------------------\n");

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const unsigned char *task = sqlite3_column_text(stmt, 1);
    const unsigned char *createdAt = sqlite3_column_text(stmt, 2);

    printf("%d\t%s\t%s\n", id, createdAt, task);
  }

  if (rc != SQLITE_DONE) {
    fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return rc;
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return SQLITE_OK;
}
