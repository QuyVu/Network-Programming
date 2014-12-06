#include <stdarg.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "global.h"
#include "data.h"
#include "message.h"
#include "tree.h"
#include "server.h"

#define SEND(fields, contents) \
    bzero(buffer, SERVER_BUFFER_LENGTH);\
    strcpy(buffer, message_serializing(fields, contents));\
    send(sock, buffer, strlen(buffer) + 1, 0);\

// tree structure: key - username, value - sock address
Tree clients;

static void* thread_proc(void *);
static BOOLEAN on_signup(int, const char*, const char*);
static BOOLEAN on_login(int, const char*, const char*);
static BOOLEAN on_logout(int sock, const char*);
static BOOLEAN on_send(const char*, const char*, const char*);
static BOOLEAN on_quit(int);
static void notify_listuser();
static char** create_contents(int, ...);

static void* thread_proc(void *arg) {
    int listensock, sock;
    int nread;

    listensock = (intptr_t) arg;

    while (1) {
        sock = accept(listensock, NULL, NULL);
        printf(TEXT_INFO "Client sock %i CONNECTED to child thread %ld with pid %i.\n" TEXT_NORMAL, sock, pthread_self(), getpid());
        // TODO: notify client CONNECT_SUCCESS
        while (1) {
            char buffer[SERVER_BUFFER_LENGTH];
            nread = recv(sock, buffer, SERVER_BUFFER_LENGTH, 0);
            if (nread < 0) {
                data_close(sock);
                break;
            }

            buffer[nread] = '\0';
            if (strlen(buffer) > 0) {
                printf(TEXT_SUCCESS "Sock(%i) Read(%i): '%s'\n" TEXT_NORMAL, sock, nread, buffer);
                char **contents = message_deserializing(buffer);
                if (strcmp(contents[0], Message[CLIENT_SIGNUP]) == 0) {
                    on_signup(sock, contents[1], contents[2]);
                } else if (strcmp(contents[0], Message[CLIENT_LOGIN]) == 0) {
                    on_login(sock, contents[1], contents[2]);
                } else if (strcmp(contents[0], Message[CLIENT_LOGOUT]) == 0) {
                    on_logout(sock, contents[1]);
                } else if (strcmp(contents[0], Message[CLIENT_SEND]) == 0) {
                    on_send(contents[1], contents[2], contents[3]);
                } else if (strcmp(contents[0], Message[CLIENT_QUIT]) == 0) {
                    on_quit(sock);
                    break;
                } else {
                }
            }
        }
        printf(TEXT_INFO "Client sock %i DISCONNECTED from child thread %ld with pid %i.\n" TEXT_NORMAL, sock, pthread_self(), getpid());
    }
}

/*
 * Client action handler
 */
static BOOLEAN on_signup(int sock, const char *name, const char *password) {
    char buffer[SERVER_BUFFER_LENGTH];
    if (data_get_password(name) != NULL) {
        SEND(2, create_contents(2,
                (char*) Message[SERVER_SIGNUP_FAILURE], "Username already exists"));
        return FALSE;
    } else {
        BOOLEAN res = data_add_user(name, password);
        if (res) {
            SEND(1, create_contents(1, Message[SERVER_SIGNUP_SUCCESS]));
            tree_insert_str(clients, (char*) name, new_val_i(sock));
            SEND(1, create_contents(1, Message[SERVER_LOGIN_SUCCESS]));
            notify_listuser();
        } else {
            SEND(1, create_contents(1, Message[SERVER_SIGNUP_FAILURE]));
        }
        return res;
    }
}

static BOOLEAN on_login(int sock, const char *name, const char *password) {
    char buffer[SERVER_BUFFER_LENGTH];
    char *p = data_get_password(name);
    if (p != NULL) {
        if (strcmp(p, password) == 0) {
            if (tree_find_str(clients, (char*) name) == NULL) {
                tree_insert_str(clients, (char*) name, new_val_i(sock));
                SEND(1, create_contents(1, Message[SERVER_LOGIN_SUCCESS]));
                notify_listuser();
                return TRUE;
            } else {
                SEND(2, create_contents(2, Message[SERVER_LOGIN_FAILURE], "User has logged in"));
                return FALSE;
            }
        } else {
            SEND(2, create_contents(2, Message[SERVER_LOGIN_FAILURE], "Wrong password"));

            return FALSE;
        }
    } else {
        SEND(2, create_contents(2, Message[SERVER_LOGIN_FAILURE], "You must signup first"));

        return FALSE;

    }
}

static BOOLEAN on_logout(int sock, const char *name) {
    char buffer[SERVER_BUFFER_LENGTH];
    Tree client = tree_find_str(clients, (char*) name);
    if (client != NULL)
        tree_delete_node(client);
    SEND(1, create_contents(1, Message[SERVER_LOGOUT_SUCCESS]));
    notify_listuser();

    return TRUE;
}

static BOOLEAN on_send(const char *sender, const char *recipient, const char *message) {
    char buffer[SERVER_BUFFER_LENGTH];
    int sock;
    sock = val_i(tree_val(tree_find_str(clients, (char*) sender)));
    SEND(4, create_contents(4, Message[SERVER_RECEIVE], sender, recipient, message));
    sock = val_i(tree_val(tree_find_str(clients, (char*) recipient)));
    SEND(4, create_contents(4, Message[SERVER_RECEIVE], sender, recipient, message));

    return TRUE;
}

static BOOLEAN on_quit(int sock) {

    return !close(sock);
}

/*
 * Others
 */
static void notify_listuser() {

    Tree c;
    char* listuser[SERVER_MAX_CONNECTION];
    listuser[0] = (char*) Message[SERVER_INFO_LISTUSER];
    int i = 1;

    TREE_TRAVERSE(c, clients) {

        listuser[i++] = val_s(c->key);
    }

    char buffer[SERVER_BUFFER_LENGTH];

    TREE_TRAVERSE(c, clients) {
        int sock = val_i(tree_val(c));
        SEND(i, listuser);
    }
}

static char** create_contents(int fields, ...) {
    va_list valist;
    va_start(valist, fields);
    char** contents = (char**) malloc(MESSAGE_MAX_FIELDS * sizeof (char**));
    int i;
    for (i = 0; i <= fields - 1; i++) {
        char *content = va_arg(valist, char*);
        contents[i] = content;
    }
    va_end(valist);

    return contents;
}

int main(int argc, char *argv[]) {
    int port = 6000;
    int val = 1;
    int result;
    int listenfd;
    printf(TEXT_SUCCESS "Server Init Successful\n" TEXT_NORMAL);
    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    result = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof (val));
    if (result < 0) {
        printf(TEXT_ERROR "Cannot set socket options\n" TEXT_NORMAL);
        exit(EXIT_FAILURE);
    }
       
    struct sockaddr_in sevraddr;
    sevraddr.sin_family = AF_INET;
    sevraddr.sin_port = htons(port);
    sevraddr.sin_addr.s_addr = INADDR_ANY;

    result = bind(listenfd, (struct sockaddr *) &sevraddr, sizeof (sevraddr));
    if (result < 0) {
        printf(TEXT_ERROR "Binding fail\n" TEXT_NORMAL);
        exit(EXIT_FAILURE);
    }

    result = listen(listenfd, SERVER_MAX_REQUESTS);
    if (result < 0) {
        printf(TEXT_ERROR "Cannot accept connection\n" TEXT_NORMAL);
        exit(EXIT_FAILURE);
    }

    data_open();
    clients = tree_make();
    
    int i;
    pthread_t thread_id;
    for (i = 0; i < SERVER_MAX_CONNECTION; i++) {
        result = pthread_create(&thread_id, NULL, thread_proc, (void*) (intptr_t) listenfd);
        if (result != 0) {
            printf(TEXT_ERROR "Cannot create thread.\n" TEXT_NORMAL);
            exit(EXIT_FAILURE);
        }
        sched_yield();
    }
    pthread_join(thread_id, NULL);
}