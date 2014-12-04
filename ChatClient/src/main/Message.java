package main;

import java.util.Arrays;
import org.json.simple.JSONArray;

public class Message {

    public static final int MAX_FIELDS = 10;
    private final JSONArray message;

    @SuppressWarnings("unchecked")
    public Message(Client type, String... contents) {
        message = new JSONArray();
        message.add(type.toString());
        message.addAll(Arrays.asList(contents));
    }

    @Override
    public String toString() {
        return message.toJSONString();
    }

    public enum Client {

        SIGNUP,
        LOGIN,
        LOGOUT,
        SEND,
        QUIT
    }

    public enum Server {

        SIGNUP_SUCCESS,
        SIGNUP_FAILURE,
        LOGIN_SUCCESS,
        LOGIN_FAILURE,
        LOGOUT_SUCCESS,
        RECEIVE,
        INFO_LISTUSER
    }
}
