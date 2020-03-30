package com.synaptics.sagitta.rest;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Arrays;

public class WifiTask extends AsyncTask {
    private String TAG = "===rock=== WIFI_TASK";
    private Globals g = Globals.getInstance();
    BaseFunction baseFunction;
    WifiManager wifi;
    private String sSID = "sagitta_hotspot";
    private String password = "Sagitta123";
    Activity activity;

    public WifiTask(Activity act) {
        activity = act;
        baseFunction = new BaseFunction(activity);
        Log.d(TAG, "WifiTask  " + activity);
    }

    @SuppressLint("ResourceAsColor")
    @Override
    // 作用：接收输入参数、执行任务中的耗时操作、返回 线程任务执行的结果
    // 注：必须复写，从而自定义线程任务

    protected String doInBackground(Object[] params) {
        wifi = (WifiManager) activity.getSystemService(Context.WIFI_SERVICE);
        TextView APIPtextView = (TextView)activity.findViewById(R.id.APIP);
        String connectAPResultMessage = null;

        int currentNetworkID = wifi.getConnectionInfo().getNetworkId();
        Log.d(TAG, "Current network ID on phone is " + currentNetworkID);
        g.setNetworkID(currentNetworkID);
        Log.d(TAG, "Global internet network ID =" + g.getNetworkID());
        Log.d(TAG, "In doInBackground get boardStatus = " + g.getBoardStatus());
        if (g.getBoardStatus() != g.BOARD_REACHABLE_BY_WIFI) {
            if (connectToNiue()) {  //；连接AP网络
                Log.d(TAG, "Connect to SYNA Board");
                long time = System.currentTimeMillis();
                long curTime = System.currentTimeMillis();
                while (curTime - time < 5000) {
                    if (wifi.getConnectionInfo().getNetworkId() == g.getNiueNetworkID()) {
                        byte[] ipAddress = convert2Bytes(wifi.getDhcpInfo().gateway);
                        if (ipAddress != null) {
                            try {
                                String apIpAddr = InetAddress.getByAddress(ipAddress).getHostAddress();
                                Log.d(TAG, "Get IP: " + apIpAddr);
                                g.setRestServer(apIpAddr);
                                g.setBoardStatus(g.BOARD_REACHABLE_BY_AP);
                                connectAPResultMessage = activity.getResources().getString(R.string.getDeviceIPMessage);
                                connectAPResultMessage += apIpAddr;
                                break;
                            } catch (UnknownHostException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                    curTime = System.currentTimeMillis();
                }
                if (connectAPResultMessage == null) {
                    connectAPResultMessage = activity.getResources().getString(R.string.getAPIPfailed);
                }
            } else {
                connectAPResultMessage = activity.getResources().getString(R.string.connectDeviceFailed);//连接失败
            }
        } else {
            connectAPResultMessage = activity.getResources().getString(R.string.deviceConnectedWifi);
        }
        activity.findViewById(R.id.status).setVisibility(View.INVISIBLE);
        baseFunction.setText(APIPtextView, connectAPResultMessage);
        Button connectDevice = activity.findViewById(R.id.connectDevice);
        baseFunction.enableButton(connectDevice);
        return null;
    }

    private static byte[] convert2Bytes(int hostAddress) {
        if (hostAddress == 0) {
            return null;
        }
        byte[] addressBytes = { (byte)(0xff & hostAddress),
                (byte)(0xff & (hostAddress >> 8)),
                (byte)(0xff & (hostAddress >> 16)),
                (byte)(0xff & (hostAddress >> 24)) };

        Log.d("===rock=== ", "addressBytes="+ Arrays.toString(addressBytes));
        return addressBytes;
    }

    private boolean connectToNiue(){
        WifiConfiguration boardAPConfig;
        boardAPConfig = new WifiConfiguration();
        boardAPConfig.SSID = "\""+ sSID +"\"";
        //Use WPA as Security Type
        boardAPConfig.preSharedKey = "\""+ password +"\"";
        boardAPConfig.status = WifiConfiguration.Status.ENABLED;

        Log.d(TAG, "SSID="+ boardAPConfig.SSID+" key="+boardAPConfig.preSharedKey+" status="+boardAPConfig.status);

        Log.d(TAG, "Try to add network");
        int netId = wifi.addNetwork(boardAPConfig);
        Log.d(TAG, "Get netId " + netId);
        g.setNiueNetworkID(netId);
        return wifi.enableNetwork(netId, true);
    }
}
