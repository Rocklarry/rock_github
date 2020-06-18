package com.synaptics.sagitta.rest;

public class Globals {
    private static Globals instance;
    private static String REST_SERVER;
    private static int NETWORK_ID;
    private static int NIUE_NETWORK_ID;
    private static int BOARD_STATUS = 0;
    private static int MESSAGE_LOCATION = 0;
    public static final int BOARD_UNREACHABLE = 0;
    public static final int BOARD_REACHABLE_BY_AP = 1;
    public static final int BOARD_REACHABLE_BY_WIFI = 2;
    public static final int ON_MAIN_PAGE = 0;
    public static final int ON_WIFI_PAGE = 1;

    public void setRestServer(String restServer) {
        Globals.REST_SERVER = restServer;
    }

    public String getRestServer() {
        return Globals.REST_SERVER;
    }

    public void setNetworkID(int networkID) {
        Globals.NETWORK_ID = networkID;
    }

    public int getNetworkID() {
        return Globals.NETWORK_ID;
    }

    public void setNiueNetworkID(int networkID) {
        Globals.NIUE_NETWORK_ID = networkID;
    }

    public int getNiueNetworkID() {
        return Globals.NIUE_NETWORK_ID;
    }

    public void setBoardStatus(int boardStatus) { Globals.BOARD_STATUS = boardStatus; }

    public int getBoardStatus() { return Globals.BOARD_STATUS; }

    public void setMessageLocation(int messageLocation) { Globals.MESSAGE_LOCATION = messageLocation; }

    public int getMessageLocation() { return Globals.MESSAGE_LOCATION; }

    public static synchronized Globals getInstance() {
        if (instance == null) {
            instance = new Globals();
        }
        return instance;
    }
}