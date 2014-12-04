#ifndef MESSAGE_H
#define	MESSAGE_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MESSAGE_MAX_FIELDS      15

    typedef enum {
        CLIENT_SIGNUP,
        CLIENT_LOGIN,
        CLIENT_LOGOUT,
        CLIENT_SEND,
        CLIENT_QUIT,
        SERVER_SIGNUP_SUCCESS,
        SERVER_SIGNUP_FAILURE,
        SERVER_LOGIN_SUCCESS,
        SERVER_LOGIN_FAILURE,
        SERVER_LOGOUT_SUCCESS,
        SERVER_RECEIVE,
        SERVER_INFO_LISTUSER
    } MESSAGE;

    static const char* Message[] = {"SIGNUP", "LOGIN", "LOGOUT", "SEND", "QUIT",
        "SIGNUP_SUCCESS", "SIGNUP_FAILURE", "LOGIN_SUCCESS", "LOGIN_FAILURE", "LOGOUT_SUCCESS",
        "RECEIVE", "INFO_LISTUSER"};

    char** message_deserializing(char*);
    char* message_serializing(int, char*[]);

#ifdef	__cplusplus
}
#endif

#endif	/* MESSAGE_H */

