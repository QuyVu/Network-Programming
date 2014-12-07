#ifndef SERVER_H
#define	SERVER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define SERVER_MAX_REQUESTS                5
#define SERVER_MAX_CONNECTION              10
#define SERVER_BUFFER_LENGTH               100

    typedef struct ClientNode {
        char *username;
        int sock;
    } ClientNode;

#ifdef	__cplusplus
}
#endif

#endif	/* SERVER_H */

