package main;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Arrays;
import java.util.function.Consumer;
import org.json.simple.JSONArray;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

public final class Client implements Runnable {

    private Socket socket;
    private final int CLIENT_BUFFER_LENGTH = 100;

    private Consumer<String[]> signupSuccess;
    private Consumer<String[]> signupFailure;
    private Consumer<String[]> loginSuccess;
    private Consumer<String[]> loginFailure;
    private Consumer<String[]> logoutSuccess;
    private Consumer<String[]> receive;
    private Consumer<String[]> infoListuser;

    /*
     Constructor
     */
    public Client() {

    }

    public Client(String host, int port) {
        connect(host, port);
    }

    /*
     Setter for properties
     */
    public void onSignupSuccess(Consumer<String[]> signupSuccess) {
        this.signupSuccess = signupSuccess;
    }

    public void onSignupFailure(Consumer<String[]> signupFailure) {
        this.signupFailure = signupFailure;
    }

    public void onLoginSuccess(Consumer<String[]> loginSuccess) {
        this.loginSuccess = loginSuccess;
    }

    public void onLoginFailure(Consumer<String[]> loginFailure) {
        this.loginFailure = loginFailure;
    }

    public void onLogoutSuccess(Consumer<String[]> logoutSuccess) {
        this.logoutSuccess = logoutSuccess;
    }

    public void onReceive(Consumer<String[]> receive) {
        this.receive = receive;
    }

    public void onInfoListuser(Consumer<String[]> infoListuser) {
        this.infoListuser = infoListuser;
    }

    /*
     Public method
     */
    public boolean signup(String username, String password) {
        return send(new Message(Message.Client.SIGNUP, username, password));
    }

    public boolean login(String username, String password) {
        return send(new Message(Message.Client.LOGIN, username, password));
    }

    public boolean logout(String username) {
        return send(new Message(Message.Client.LOGOUT, username));
    }

    public boolean send(String sender, String recipient, String message) {
        return send(new Message(Message.Client.SEND, sender, recipient, message));
    }

    public boolean quit() {
        return send(new Message(Message.Client.QUIT));
    }

    public boolean connect(String host, int port) {
        try {
            socket = new Socket(host, port);
            System.out.println(Text.SUCCESS
                    + "Just connected to " + socket.getRemoteSocketAddress() + Text.NORMAL);
            return true;
        } catch (IOException ex) {
            System.out.println(Text.ERROR + "Cannot connect to: " + host + ":" + port + Text.NORMAL);
            return false;
        }
    }

    public boolean isConnected() {
        if (socket == null) {
            return false;
        }
        return socket.isConnected();
    }

    public boolean close() {
        try {
            socket.close();
            return true;
        } catch (IOException ex) {
            return false;
        }
    }

    /*
     Implement from Runnable interface
     */
    @Override
    public void run() {
        while (true) {
            recv();
        }
    }

    /*
     Private method
     */
    @SuppressWarnings("CallToPrintStackTrace")
    private boolean send(Message message) {
        PrintWriter out;
        try {
            out = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
            out.print(message);
            out.flush();
            return true;
        } catch (IOException ex) {
            ex.printStackTrace();
            return false;
        }
    }

    @SuppressWarnings("CallToPrintStackTrace")
    private void recv() {
        BufferedReader in;
        char buffer[] = new char[CLIENT_BUFFER_LENGTH];
        try {
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            Arrays.fill(buffer, '\0');
            in.read(buffer);
            for (String message : new String(buffer).split("\0")) {
                if (message.equals("")) {
                    return;
                }
                JSONArray array = (JSONArray) new JSONParser().parse(message);
                System.out.print(Text.INFO);
                for (int i = 0; i <= array.size() - 1; i++) {
                    System.out.print(array.get(i));
                    if (i < array.size() - 1) {
                        System.out.print(", ");
                    }
                }
                System.out.println(Text.NORMAL);

                String type = array.get(0).toString();
                String[] params = new String[Message.MAX_FIELDS - 1];
                for (int i = 1; i <= array.size() - 1; i++) {
                    params[i - 1] = array.get(i).toString();
                }
                if (type.equals(Message.Server.SIGNUP_SUCCESS.toString()) && signupSuccess != null) {
                    signupSuccess.accept(params);
                } else if (type.equals(Message.Server.SIGNUP_FAILURE.toString()) && signupFailure != null) {
                    signupFailure.accept(params);
                } else if (type.equals(Message.Server.LOGIN_SUCCESS.toString()) && loginSuccess != null) {
                    loginSuccess.accept(params);
                } else if (type.equals(Message.Server.LOGIN_FAILURE.toString()) && loginFailure != null) {
                    loginFailure.accept(params);
                } else if (type.equals(Message.Server.LOGOUT_SUCCESS.toString()) && logoutSuccess != null) {
                    logoutSuccess.accept(params);
                } else if (type.equals(Message.Server.RECEIVE.toString()) && receive != null) {
                    receive.accept(params);
                } else if (type.equals(Message.Server.INFO_LISTUSER.toString()) && infoListuser != null) {
                    infoListuser.accept(params);
                }
            }
        } catch (IOException | ParseException ex) {
            ex.printStackTrace();
        }
    }

    private enum Text {

        NORMAL("\u001B[0m"),
        ERROR("\u001B[31m"),
        SUCCESS("\u001B[32m"),
        WARNING("\u001B[33m"),
        INFO("\u001B[34m");

        private final String text;

        private Text(final String text) {
            this.text = text;
        }

        @Override
        public String toString() {
            return text;
        }
    }
}
