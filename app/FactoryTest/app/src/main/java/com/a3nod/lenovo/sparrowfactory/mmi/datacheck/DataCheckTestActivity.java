package com.a3nod.lenovo.sparrowfactory.mmi.datacheck;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiManager;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pManager;
import android.os.Build;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.mmi.GetWifiMacUtils;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ResultActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.wifi.WifiManageUtils;
import com.a3nod.lenovo.sparrowfactory.tool.SystemInfoTools;

import net.vidageek.mirror.dsl.Mirror;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

public class DataCheckTestActivity extends Activity
{
    public static final String TAG = "MMI_DATA_CHECK_TEST";
    ListView lv_property;
    PropertyAdapter pa;
    //    TextView textView;
    List<Property> lp;
    private WifiManageUtils wifiManageUtils = null;
    private Button btnpass,btnfail,btnreset;
    BluetoothAdapter bluetoothAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_MMI);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pcba_data_check_test);
        initConfirmButton();
        wifiManageUtils = WifiManageUtils.getInstance(this);
        wifiManageUtils.initManager();
        wifiManageUtils.openWifi();
        final IntentFilter filter = new IntentFilter();
        filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        filter.addAction(WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION);
        registerReceiver(mReceiver, filter);

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if(!bluetoothAdapter.isEnabled()){
            bluetoothAdapter.enable();
        }

        lv_property = findViewById(R.id.lv_system_property);
        lp = new ArrayList<>();
        pa = new PropertyAdapter();
        lv_property.setAdapter(pa);


        String fingerprint = SystemInfoTools.getSystemProperty("ro.build.display.id");
        if (TextUtils.isEmpty(fingerprint))
        {
            fingerprint = SystemInfoTools.getSystemProperty("ro.vendor.build.fingerprint");
        }
        lp.add(new Property("VERSION", fingerprint));

        String cpuinfo = SystemInfoTools.getCpuInfo();
        lp.add(new Property("CPU", cpuinfo));

        long ramMemory = SystemInfoTools.getTotalMemory();
        lp.add(new Property("RAM", SystemInfoTools.formatSize(ramMemory)));

        long romMemroy = SystemInfoTools.getRomMemroy();
        lp.add(new Property("ROM", SystemInfoTools.formatSize(romMemroy)));

        //lp.add(new Property("WIFI MAC", SystemInfoTools.getMACByFile().toUpperCase()));
        //lp.add(new Property("WIFI MAC", GetWifiMacUtils.getMac()));

        //        String bt_mac= android.provider.Settings.Secure.getString(getContentResolver(), "bluetooth_address").toUpperCase();
                lp.add(new Property("BLUETOOTH MAC", SystemInfoTools.getBtAddressViaReflection().toUpperCase()));
       // String bt_mac= SystemInfoTools.getBluetoothMac();
//        String bt_mac = SystemInfoTools.getProperty("persist.service.bdroid.bdaddr", "").toUpperCase();
//        if (bt_mac.equals("UNKNOWN")){
//            bt_mac = getBtAddressMac();
//        }
     //   lp.add(new Property("BLUETOOTH MAC", bt_mac));

        String googleFsi = SystemInfoTools.getSystemProperty("ro.build.version.google").replaceAll("\"", "");
        lp.add(new Property("GOOGLE FSI", googleFsi));

        String MANUFACTURER = Build.MANUFACTURER;
        lp.add(new Property("MANUFACTURER", "Whirlpool"));

        String SN = SystemInfoTools.readSNFile(Constant.PATH_MMI_SN);
        lp.add(new Property("ASSY SERIAL NUMBER", SN));

        String EMMC_SERIAL_NUMBER = SystemInfoTools.getDeviceSNFromFile();
        lp.add(new Property("EMMC SERIAL NUMBER", EMMC_SERIAL_NUMBER));

        String lenovoSN = Build.SERIAL;
        lp.add(new Property("WHIRLPOOL SERIAL NUMBER", lenovoSN));

    }

    public void initConfirmButton()
    {
        btnpass = findViewById(R.id.btn_result_pass);
        btnpass.setEnabled(false);
        btnpass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Intent intent = new Intent(DataCheckTestActivity.this, ResultActivity.class);
                startActivity(intent);
                finish();
            }
        });
        btnreset = findViewById(R.id.btn_result_reset);
        btnreset.setVisibility(View.INVISIBLE);
        btnfail = findViewById(R.id.btn_result_fail);
        btnfail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Intent intent = new Intent(DataCheckTestActivity.this, ResultActivity.class);
                startActivity(intent);
                finish();
            }
        });
    }

    public void getSystemProperties()
    {

        Class cls = Build.class;
        Field[] fields = cls.getDeclaredFields();
        for (int i = 0; i < fields.length; i++)
        {
            try
            {
                lp.add(new Property(fields[i].getName(), fields[i].get(null).toString()));
            } catch (IllegalAccessException e)
            {
                e.printStackTrace();
            }
        }

        cls = Build.VERSION.class;
        fields = cls.getDeclaredFields();
        for (int i = 0; i < fields.length; i++)
        {
            try
            {
                lp.add(new Property(fields[i].getName(), fields[i].get(null).toString()));
            } catch (IllegalAccessException e)
            {
                e.printStackTrace();
            }
        }

    }


    public static String getProperty(String key, String defaultValue)
    {
        String value = defaultValue;
        try
        {
            Class<?> c = Class.forName("android.os.SystemProperties");
            Method get = c.getMethod("get", String.class, String.class);
            value = (String) (get.invoke(c, key, "unknown"));
        } catch (Exception e)
        {
            e.printStackTrace();
        }
        return value;
    }

    //主板SN，整机SN号，MAC address,软件版本，TP FW，camera FW， CPU，Flash Memory size,恢复出厂，测试标志位（SN、校准、终测、Wifi FT、….)
    class PropertyAdapter extends BaseAdapter
    {

        @Override
        public int getCount()
        {
            return lp.size();
        }

        @Override
        public Object getItem(int position)
        {
            return null;
        }

        @Override
        public long getItemId(int position)
        {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent)
        {
            ViewHolder holder;
            if (convertView == null)
            {
                holder = new ViewHolder();
                convertView = LayoutInflater.from(DataCheckTestActivity.this).inflate(R.layout.list_item_pcba_system_property, parent, false);
                holder.tv_name = convertView.findViewById(R.id.list_item_property_name);
                holder.tv_value = convertView.findViewById(R.id.list_item_property_value);
                convertView.setTag(holder);
            } else
            {
                holder = (ViewHolder) convertView.getTag();
            }
            Property p = lp.get(position);
            holder.tv_name.setText(p.name);
            holder.tv_value.setText(p.value);
            return convertView;
        }

        class ViewHolder
        {
            TextView tv_name;
            TextView tv_value;
        }
    }

    class Property
    {
        public Property(String name, String value)
        {
            this.name = name;
            this.value = value;
        }

        String name;
        String value;
    }

    @Override
    protected void onDestroy()
    {
        Log.e("TAG", "关闭wifi");
        wifiManageUtils.closeWifi();
        unregisterReceiver(mReceiver);
        super.onDestroy();
        MMITestProcessManager.getInstance().testFinish();
    }

    private BroadcastReceiver mReceiver = new BroadcastReceiver()
    {
        @Override
        public void onReceive(Context context, Intent intent)
        {
            String action = intent.getAction();
            if (TextUtils.isEmpty(action))
            {
                return;
            }
            switch (action)
            {
                case WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION:
                    WifiP2pDevice thisDevice =
                            intent.getParcelableExtra(WifiP2pManager.EXTRA_WIFI_P2P_DEVICE);
                    String mymactemp = thisDevice.deviceAddress;
                    lp.add(4, new Property("WIFI MAC",mymactemp ));
                    pa.notifyDataSetChanged();
                    Log.e("TAG", "get this devices mac ="+mymactemp);
                    if(!mymactemp.equals("00:00:00:00:00:00")){
                        btnpass.setEnabled(true);
                    }
                case WifiManager.WIFI_STATE_CHANGED_ACTION://wifi打开与否
                    int wifistate = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, WifiManager.WIFI_STATE_DISABLED);
                    Log.e("TAG", "wifi状态改变" + wifistate);
                    if (wifistate == WifiManager.WIFI_STATE_DISABLED)
                    {
                        Log.e("TAG", "系统关闭wifi");
                    } else if (wifistate == WifiManager.WIFI_STATE_ENABLED)
                    {
                        //系统wifi开启
                        //                        GetWifiMacUtils.getMac();
                        String mactemp = GetWifiMacUtils.getMac().toUpperCase();
                        lp.add(4, new Property("WIFI MAC",mactemp ));
                        pa.notifyDataSetChanged();
                        if(!mactemp.equals("00:00:00:00:00:00")){
                            btnpass.setEnabled(true);
                        }
                    } else if (wifistate == WifiManager.WIFI_STATE_ENABLING)
                    {
                        Log.e("TAG", "系统开启wifi wifi可用");
                    } else if (wifistate == WifiManager.WIFI_STATE_UNKNOWN)
                    {
                    }
                    break;

                default:
                    break;
            }
        }
    };

    private  String getBtAddressMac(){
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        if(adapter == null){
            return "Bluetooth Device disable";
        }else{
            if (!adapter.isEnabled()){
                adapter.enable();
            }
        }
        Object bluetoothManagerService = new Mirror().on(adapter).get().field("mService");
        if (bluetoothManagerService == null){
            return "get bluetoothservice fail";
        }
        Object address = new Mirror().on(bluetoothManagerService).invoke().method("getAddress").withArgs();
        if (address != null && address instanceof String){
            return (String)address;
        }
        return "get bluetoorh mac fail";
    }

    //通过广播获取mac地址
    private class WifiBroadcastReceiver extends BroadcastReceiver{
        @Override
        public void onReceive(Context context, Intent intent) {
            switch (intent.getAction()){
                case WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION:
                    WifiP2pDevice thisDevice =
                            intent.getParcelableExtra(WifiP2pManager.EXTRA_WIFI_P2P_DEVICE);
                    String selfMac = thisDevice.deviceAddress;
            }
        }
    }
}
