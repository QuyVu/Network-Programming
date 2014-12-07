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
#include "server.h"

/*
 * send contents with n fields to client
 */
#define SEND(fields, contents) \
    bzero(buffer, SERVER_BUFFER_LENGTH);\
    strcpy(buffer, message_serializing(fields, contents));\
    send(sock, buffer, strlen(buffer) + 1, 0);\

int number_of_clients = 0; // number of connected users 
ClientNode clients[SERVER_MAX_CONNECTION]; //an array store name & socket of connected users

static void* thread_handler(void *);
static BOOLEAN on_signup(int, const char*, const char*);
static BOOLEAN on_login(int, const char*, const char*);
static BOOLEAN on_logout(int sock, const char*);
static BOOLEAN on_send(const char*, const char*, const char*);
static BOOLEAN on_quit(int);
static void notify_listuser();
static char** create_contents(int, ...);

static void insert_client(const char*, int);
static void remove_client(const char*);
static int find_client(const char*);

/*
 * Initialize handler for thread procedure 
 */
static void* thread_handler(void *arg) {
    int listensock, sock;
    int nread;

    listensock = (intptr_t) arg;

    while (1) {
        sock = accept(listensock, NULL, NULL);
        printf(TEXT_INFO "Client sock %i CONNECTED to child thread %ld with pid %i.\n" TEXT_NORMAL, sock, pthread_self(), getpid());
        while (1) {
            char buffer[SERVER_BUFFER_LENGTH];
            nread = recv(sock, buffer, SERVER_BUFFER_LENGTH, 0);
            if (nread < 0) {
                data_close(sock);
                break;
            }
            
            //analyze message from client
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
            insert_client(name, sock);
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
            if (find_client(name) < 0) {
                insert_client(name, sock);
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
    remove_client(name);
    SEND(1, create_contents(1, Message[SERVER_LOGOUT_SUCCESS]));
    notify_listuser();

    return TRUE;
}

static BOOLEAN on_send(const char *sender, const char *recipient, const char *message) {
    char buffer[SERVER_BUFFER_LENGTH];
    int sock;
    sock = clients[find_client(sender)].sock;
    SEND(4, create_contents(4, Message[SERVER_RECEIVE], sender, recipient, message));
    sock = clients[find_client(recipient)].sock;
    SEND(4, create_contents(4, Message[SERVER_RECEIVE], sender, recipient, message));

    return TRUE;
}

static BOOLEAN on_quit(int sock) {

    return !close(sock);
}

/*
 * send list users to client
 */
static void notify_listuser() {

    char* listuser[SERVER_MAX_CONNECTION + 1];
    listuser[0] = (char*) Message[SERVER_LISTING_USER];
    int i;

    for (i = 1; i <= number_of_clients; i++)
        listuser[i] = clients[i - 1].username;


    char buffer[SERVER_BUFFER_LENGTH];

    for (i = 0; i <= number_of_clients - 1; i++) {
        int sock = clients[i].sock;
        SEND(number_of_clients + 1, listuser);
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

//insert new loged in user to array
static void insert_client(const char *username, int sock) {
    ClientNode clientNode;
    clientNode.username = strdup(username);
    clientNode.sock = sock;
    clients[number_of_clients++] = clientNode;
}

//remove loged out user from array
static void remove_client(const char *username) {
    int pos = find_client(username);
    int i;
    for (i = pos; i <= number_of_clients - 2; i++) {
        clients[i] = clients[i + 1];
    }
    number_of_clients--;
}

//find specific user
static int find_client(const char *username) {
    int i;
    for (i = 0; i <= number_of_clients - 1; i++) {
        if (strcmp(username, clients[i].username) == 0)
            return i;
    }
    return -1;
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

    int i;
    pthread_t thread_id;
    for (i = 0; i < SERVER_MAX_CONNECTION; i++) {
        result = pthread_create(&thread_id, NULL, thread_handler, (void*) (intptr_t) listenfd);
        if (result != 0) {
            printf(TEXT_ERROR "Cannot create thread.\n" TEXT_NORMAL);
            exit(EXIT_FAILURE);
        }
        sched_yield();
    }
    pthread_join(thread_id, NULL);
}