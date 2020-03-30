package com.synaptics.sagitta.rest;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetSocketAddress;
import java.net.MulticastSocket;
import java.net.NetworkInterface;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.Map;

public class SSDPTask extends AsyncTask {
    private String TAG = "===rock=== SSDP";
    private Globals g = Globals.getInstance();
    BaseFunction baseFunction;
    Activity activity;

    public SSDPTask(Activity act) {
        activity = act;
        baseFunction = new BaseFunction(activity);
    }

    private Map parseProps(String data) {
        String[] lines = data.split("\r\n");
        Map props = new HashMap<>();
        for (String line : lines) {
            Log.i(TAG, "Dan get : index for line " + line);
            if (line == "") {
                continue;
            }
            Integer index = line.indexOf(":");
            Log.i(TAG, "Dan get : index " + index);
            if ( index == -1) {
                continue;
            }
            String key = line.substring(0,index);
            String value = line.substring(index+1);
            props.put(key,value);
        }
        return props;
    }

    @Override
    //UPNP 协议
    protected String doInBackground(Object[] params) {
        Log.d(TAG, "In function doInBackground");
        final String query_start = "M-SEARCH * HTTP/1.1\r\n";
        final String SSDP_ADDRESS = "239.255.255.250";
        final Integer SSDP_PORT = 1900;
        final String SSDP_URN = "urn:synaptics-as-390";

        SocketAddress mSSDPMulticastGroup;
        MulticastSocket mSSDPSocket = null;
        NetworkInterface netIf = null;
        String resultMessage = null;
        TextView textView = null;
        Button button = null;

        try {
            mSSDPMulticastGroup = new InetSocketAddress(SSDP_ADDRESS, SSDP_PORT);
            netIf = NetworkInterface.getByName("wlan0");
            mSSDPSocket = new MulticastSocket(SSDP_PORT);
            if ((mSSDPSocket != null) && (netIf != null)) {
                mSSDPSocket.joinGroup(mSSDPMulticastGroup, netIf);
                long time = System.currentTimeMillis();
                long curTime = System.currentTimeMillis();

                // SSDP server broadcast every 5 seconds
                // Set 5 seconds dealine here
                while (curTime - time < 5000) {
                    Log.d(TAG, "In doInBackground while loop");
                    DatagramPacket p = new DatagramPacket(new byte[1024], 1024);
                    Log.d(TAG, "Before scan ssdp");
                    mSSDPSocket.setSoTimeout(5000);
                    mSSDPSocket.receive(p);
                    Log.d(TAG, "After scan ssdp");
                    String s = new String(p.getData(), 0, p.getLength());
                    Map format_ssdp_result = parseProps(s);
                    Log.d(TAG, "After parse ssdp result");
                    curTime = System.currentTimeMillis();
                    Log.d(TAG, "current time" + curTime + " time " + time);
                    Log.d(TAG, "Get s " + s);
                    Log.d(TAG, "Get format_ssdp_result " + format_ssdp_result);
                    if ((s != null)&&(format_ssdp_result != null)&&
                            (format_ssdp_result.get("HOST") != null)&&
                            (format_ssdp_result.get("ST") != null) &&
                            (s.startsWith(query_start))&&
                            (format_ssdp_result.get("HOST").equals(SSDP_ADDRESS + ":" + SSDP_PORT))&&
                            (format_ssdp_result.get("ST").equals(SSDP_URN))) {
                        Log.i(TAG, "Find ssdp server");
                        Globals g = Globals.getInstance();
                        String deviceIP = p.getAddress().getHostAddress();
                        g.setRestServer(deviceIP);
                        g.setBoardStatus(g.BOARD_REACHABLE_BY_WIFI);
                        resultMessage = activity.getResources().getString(R.string.getDeviceIPMessage);
                        resultMessage += deviceIP;
                        if (g.getMessageLocation() == g.ON_WIFI_PAGE) {
                            Log.d(TAG, "Set wifi status");
                            resultMessage = activity.getResources().getString(R.string.deviceInSameWifi);
                        } else if (g.getMessageLocation() == g.ON_MAIN_PAGE) {
                            activity.findViewById(R.id.APIP).setVisibility(View.INVISIBLE);
                        }
                        break;
                    }
                }
            }
        } catch (UnknownHostException e) {
            Log.e(TAG, "Get local host failed " + e.getMessage());
        } catch (SocketException e) {
            Log.e(TAG, "Get local host InetAddress failed " + e.getMessage());
        } catch (IOException e) {
            Log.e(TAG, "Operate socket failed " + e.getMessage());
        } finally {
            if (mSSDPSocket != null) {
                mSSDPSocket.close();
            }
        }
        Log.d(TAG, "Finished finding ssdp");
        if (resultMessage == null) {
            resultMessage = activity.getResources().getString(R.string.getDeviceFailed);
            g.setBoardStatus(g.BOARD_UNREACHABLE);
        }
        if (g.getMessageLocation() == g.ON_WIFI_PAGE) {
            textView = (TextView)activity.findViewById(R.id.wifiStatus);
            button = activity.findViewById(R.id.reconnectInternetAP);
        } else if (g.getMessageLocation() == g.ON_MAIN_PAGE) {
            activity.findViewById(R.id.status).setVisibility(View.INVISIBLE);
            textView = (TextView) activity.findViewById(R.id.DeviceIP);
            button = activity.findViewById(R.id.getDeviceIP);
        }
        baseFunction.setText(textView, resultMessage);
        baseFunction.enableButton(button);
        return null;
    }
}
