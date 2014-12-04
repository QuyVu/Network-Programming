#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "data.h"

static void init();

BOOLEAN data_open() {
    int rc;
    rc = sqlite3_open(DATA_FILEPATH, &chat_database);

    if (rc) {
        printf(TEXT_ERROR "Can't open database: %s\n", sqlite3_errmsg(chat_database));
        return FALSE;
    } else {
        //printf(SUCCESS "Opened database successfully\n");
        init();
        return TRUE;
    }
}

BOOLEAN data_add_user(const char *name, const char *password) {
    char *message = 0;
    int rc;
    char *sql = (char*) malloc(sizeof (char) * DATA_SQL_LENGTH);
    sprintf(sql, "INSERT INTO users (name, password) VALUES ('%s', '%s');", name, password);

    rc = sqlite3_exec(chat_database, sql, NULL, NULL, &message);
    free(sql);
    if (rc != SQLITE_OK) {
        printf(TEXT_ERROR "SQL error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    } else {
        //printf(SUCCESS "Records created successfully\n");
        return TRUE;
    }
}

char* data_get_password(const char *name) {
    char *message = 0;
    int rc;
    char *sql = (char*) malloc(sizeof (char) * DATA_SQL_LENGTH);
    sprintf(sql, "SELECT password FROM users WHERE name = '%s';", name);

    char *password = NULL;

    rc = sqlite3_exec(chat_database, sql,
            ({int $(void *data, int col, char **values, char **colname) {
            char **password = (char**) data;
            *password = strdup(values[0]);
            return 0;
        }
        $;}),
    &password, &message);
    free(sql);
    if (rc != SQLITE_OK) {
        printf(TEXT_ERROR "SQL error: %s\n", message);
        sqlite3_free(message);
        return NULL;
    } else {
        return password;
    }
}

void data_close() {
    sqlite3_close(chat_database);
}

/*
 * Private functions
 */

static void init() {
    char *sql;
    char *message = 0;

    // Check table 'users' exists or not
    sql = "SELECT * FROM users";
    int rc = sqlite3_exec(chat_database, sql, NULL, NULL, &message);
    if (rc != SQLITE_OK) {
        printf(TEXT_WARNING "SQL error: %s\n", message);
        sqlite3_free(message);

        sql = "CREATE TABLE users("  \
         "name          TEXT    PRIMARY KEY," \
         "password      TEXT    NOT NULL);";
        rc = sqlite3_exec(chat_database, sql, NULL, NULL, &message);
        if (rc != SQLITE_OK) {
            printf(TEXT_ERROR "SQL error: %s\n", message);
            sqlite3_free(message);
        } else {
            printf(TEXT_SUCCESS "Table users created successfully\n");
        }
    } else {
        //printf(SUCCESS "Operation done successfully\n");
    }

}