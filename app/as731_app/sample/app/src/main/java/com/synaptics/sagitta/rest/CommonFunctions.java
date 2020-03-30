package com.synaptics.sagitta.rest;

import android.app.Activity;
import android.content.Context;
import android.net.wifi.WifiManager;
import android.util.Log;

public class CommonFunctions {
    private final String TAG = "===rock=== COMMON";
    WifiManager wifi;
    private Globals g = Globals.getInstance();
    Activity activity;

    public CommonFunctions(Activity act) {
        activity = act;
    }

    //设置前
    public void discoverDevices() {
        SSDPTask discover = new SSDPTask(activity);
        discover.execute();
    }
    //设置后
    public void doAfterSetupWifi() {
        wifi = (WifiManager) activity.getSystemService(Context.WIFI_SERVICE);
        Log.d(TAG, "Internet networok ID " + g.getNetworkID());
        wifi.enableNetwork(g.getNetworkID(), true);
        Log.d(TAG, "Niue network ID " + g.getNiueNetworkID());
        wifi.removeNetwork(g.getNiueNetworkID());
        g.setBoardStatus(g.BOARD_REACHABLE_BY_WIFI);
        discoverDevices();
    }
}
