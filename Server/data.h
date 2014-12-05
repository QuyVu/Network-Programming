#ifndef DATA_H
#define	DATA_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <sqlite3.h>
    
#define DATA_PATH       "database/chat.db"
#define DATA_SQL_LENGTH     100

sqlite3 *chat_database;

BOOLEAN data_open();
BOOLEAN data_add_user(const char*, const char*);
char* data_get_password(const char*);
void data_close();

#ifdef	__cplusplus
}
#endif

#endif	/* DATA_H */

