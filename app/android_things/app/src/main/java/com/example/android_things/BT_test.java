package com.example.android_things;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.view.MotionEvent;
import android.widget.TextView;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.List;
import java.util.Set;

public class BT_test extends Activity {

    private TextView m_tvLocalBluetoothMAC;
    private TextView m_tvLocalBluetoothName;
    private TextView m_tvRemoteBluetoothMAC;
    private TextView m_tvRemoteBluetoothName;


    public  static  String BT_unconnect = "BT unconnect";
    public static String BT_unconnect_name = "BT unconnect name";
    public  static  String BT_address_null = "BT   address is empty";

    private int count = 0;
    private long firClick = 0;
    private long secClick = 0;
    private final int interval = 1500;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bt_test);

        TextView tvAppVer =  findViewById(R.id.tvAppVer);
        TextView tvDevMode =  findViewById(R.id.tvDevMode);
        TextView tvAndroidVer = findViewById(R.id.tvAndroidVer);
        TextView tvVer =  findViewById(R.id.tvAPIVer)
                ;
        tvAppVer.setText(BuildConfig.VERSION_NAME);
        tvDevMode.setText(android.os.Build.MODEL);
        tvAndroidVer.setText(android.os.Build.VERSION.RELEASE);
        tvVer.setText(""+ Build.VERSION.SDK_INT);


        m_tvLocalBluetoothName =  findViewById(R.id.tvBluetoothName);
        m_tvLocalBluetoothMAC =  findViewById(R.id.tvBluetoothMac);
        m_tvRemoteBluetoothName =  findViewById(R.id.tvRemoteBluetoothName);
        m_tvRemoteBluetoothMAC =  findViewById(R.id.tvRemoteBluetoothMac);
    }

    public String getLocalBluetoothMAC() {
        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        if (btAdapter == null) {
            return "";
        }
        m_tvLocalBluetoothName.setText(btAdapter.getName());

        String bluetoothMAC = "";
        if (Build.VERSION.SDK_INT < 23) {
            bluetoothMAC = btAdapter.getAddress();
        } else {
            Class<? extends BluetoothAdapter> btAdapterClass = btAdapter.getClass();
            try {
                Class<?> btClass = Class.forName("android.bluetooth.IBluetooth");
                Field bluetooth = btAdapterClass.getDeclaredField("mService");
                bluetooth.setAccessible(true);
                Method btAddress = btClass.getMethod("getAddress");
                btAddress.setAccessible(true);
                bluetoothMAC = (String) btAddress.invoke(bluetooth.get(btAdapter));
            } catch (Exception e) {
                bluetoothMAC = btAdapter.getAddress();
            }
        }
        if (bluetoothMAC.equals("02:00:00:00:00:00")) {
            return BT_address_null;
        } else {
            return bluetoothMAC;
        }
    }

    private void getConnectedBluetoothMAC(final Context context) {
        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        if (btAdapter == null) {
            return;
        }
        int a2dp = btAdapter.getProfileConnectionState(BluetoothProfile.A2DP);
        int headset = btAdapter.getProfileConnectionState(BluetoothProfile.HEADSET);
        int health = btAdapter.getProfileConnectionState(BluetoothProfile.HEALTH);
        int flag = -1;
        if (a2dp == BluetoothProfile.STATE_CONNECTED) {
            flag = a2dp;
        } else if (headset == BluetoothProfile.STATE_CONNECTED) {
            flag = headset;
        } else if (health == BluetoothProfile.STATE_CONNECTED) {
            flag = health;
        }
        if (flag != -1) {
            btAdapter.getProfileProxy(context, new BluetoothProfile.ServiceListener() {
                @Override
                public void onServiceDisconnected(int profile) {

                }
                @Override
                public void onServiceConnected(int profile, BluetoothProfile proxy) {
                    List<BluetoothDevice> mDevices = proxy.getConnectedDevices();
                    if (mDevices != null && mDevices.size() > 0) {
                        m_tvRemoteBluetoothMAC.setText( mDevices.get(0).getAddress() );
                        m_tvRemoteBluetoothName.setText( mDevices.get(0).getName() );
                    }
                }
            }, flag);
        } else {
            m_tvRemoteBluetoothMAC.setText( BT_unconnect );
            m_tvRemoteBluetoothName.setText( BT_unconnect_name);
        }
    }

    protected void onResume() {
        super.onResume();

        //String macAddress = android.provider.Settings.Secure.getString(getContentResolver(),"bluetooth_address");
        String localBluetoothMAC = getLocalBluetoothMAC();
        if (localBluetoothMAC.equals("")) {
            m_tvLocalBluetoothMAC.setText(BT_unconnect);
        } else {
            m_tvLocalBluetoothMAC.setText(localBluetoothMAC);
        }

        String remoteBluetoothMAC = getRemoteBluetoothMAC();
        if (!remoteBluetoothMAC.equals("")) {
            m_tvRemoteBluetoothMAC.setText(remoteBluetoothMAC);
        } else {
            getConnectedBluetoothMAC(this);
        }
    }

    private String getRemoteBluetoothMAC() {
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        Class<BluetoothAdapter> bluetoothAdapterClass = BluetoothAdapter.class;
        try {
            Method method = bluetoothAdapterClass.getDeclaredMethod("getConnectionState", (Class[]) null);
            method.setAccessible(true);
            int state = (int) method.invoke(adapter, (Object[]) null);
            if(state == BluetoothAdapter.STATE_CONNECTED){
                Set<BluetoothDevice> devices = adapter.getBondedDevices();
                for(BluetoothDevice device : devices){
                    Method isConnectedMethod = BluetoothDevice.class.getDeclaredMethod("isConnected", (Class[]) null);
                    method.setAccessible(true);
                    boolean isConnected = (boolean) isConnectedMethod.invoke(device, (Object[]) null);
                    if(isConnected){
                        m_tvRemoteBluetoothName.setText(device.getName());
                        return device.getAddress();
                    }
                }
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }



    public boolean dispatchTouchEvent(MotionEvent ev) {
        if (MotionEvent.ACTION_DOWN == ev.getAction()) {
            count++;
            if (1 == count) {
                firClick = System.currentTimeMillis();
            } else if (2 == count) {
                secClick = System.currentTimeMillis();
                if (secClick - firClick < interval) {
                    count = 0;
                    firClick = 0;
                    System.exit(0);
                } else {
                    firClick = secClick;
                    count = 1;
                }
                secClick = 0;
            }
        }
        return super.dispatchTouchEvent(ev);
    }
}
