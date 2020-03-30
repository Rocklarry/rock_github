package com.a3nod.lenovo.sparrowfactory.pcba;

import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.mmi.GetWifiMacUtils;
import com.a3nod.lenovo.sparrowfactory.mmi.datacheck.DataEntity;
import com.a3nod.lenovo.sparrowfactory.mmi.wifi.WifiManageUtils;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.PcbaEntity;
import com.a3nod.lenovo.sparrowfactory.tool.SystemInfoTools;
import com.google.gson.Gson;

import net.vidageek.mirror.dsl.Mirror;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

public class PCBADataCheckTestActivity extends PCBABaseActivity
{
    ListView lv_property;
    PropertyAdapter pa;
    //    TextView textView;
    List<Property> lp;
    public static final String TAG = "pcba_datacheck_test";
    private WifiManageUtils wifiManageUtils = null;
    private Handler handler = null;
    private DataEntity dataEntity = null;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_PCBA);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pcba_data_check_test);
        View bottonBtn = findViewById(R.id.bottom_btn);
        bottonBtn.setVisibility(View.GONE);
        handler = new Handler()
        {
            @Override
            public void handleMessage(Message msg)
            {
                switch (msg.what)
                {
                    case 0:
                        addWifiMacAddress(); //收到wifi开启可用的广播，增加wifi mac 地址
                        break;
                    default:
                        break;
                }
            }
        };

        wifiManageUtils = WifiManageUtils.getInstance(this);
        wifiManageUtils.initManager();
        wifiManageUtils.openWifi();

        final IntentFilter filter = new IntentFilter();
        filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        registerReceiver(mReceiver, filter);

        lv_property = findViewById(R.id.lv_system_property);

        lp = new ArrayList<>();
        pa = new PropertyAdapter();
        lv_property.setAdapter(pa);


       // ro.build.version.3nod
        String fingerprint=SystemInfoTools.getSystemProperty("ro.build.display.id");
        if(TextUtils.isEmpty(fingerprint))
        {
            fingerprint=SystemInfoTools.getSystemProperty("ro.vendor.build.fingerprint");
        }
        lp.add(new Property("VERSION", fingerprint));



        String cpuinfo = SystemInfoTools.getCpuInfo();
        lp.add(new Property("CPU", cpuinfo));

        long ramMemory = SystemInfoTools.getTotalMemory();
        lp.add(new Property("RAM", SystemInfoTools.formatSize(ramMemory)));

        long romMemroy = SystemInfoTools.getRomMemroy();
        lp.add(new Property("ROM", SystemInfoTools.formatSize(romMemroy)));


//        String mac= SystemInfoTools.getMACByFile().toUpperCase();
//        lp.add(new Property("WIFI MAC", mac));
//        String bt_mac= SystemInfoTools.getBtAddressViaReflection().toUpperCase();
//        String bt_mac = android.provider.Settings.Secure.getString(getContentResolver(), "bluetooth_address");

   //String bt_mac = SystemInfoTools.getProperty("persist.service.bdroid.bdaddr","").toUpperCase();
        String bt_mac =  SystemInfoTools.getBtAddressViaReflection().toUpperCase();
        if (bt_mac.equals("UNKNOWN")){
            bt_mac = getBtAddressMac();
        }
        lp.add(new Property("BLUETOOTH MAC", bt_mac));

        String googleFsi=SystemInfoTools.getSystemProperty("ro.build.version.google").replaceAll("\"","");
        lp.add(new Property("GOOGLE FSI", googleFsi));

        String MANUFACTURER = Build.MANUFACTURER;
        lp.add(new Property("MANUFACTURER", "Whirlpool"));

//        lp.add(new Property("mac", MacAddressTool.getLocalMacAddressFromBusybox()));
//        lp.add(new Property("mac", MacAddressTool.getMacAAA()));
//        lp.add(new Property("mac", MacAddressTool.getMacByAPI(this)));

//        lp.add(new Property("mac", MacAddressTool.getMacByCMD()));


        String PCBA_SN = SystemInfoTools.readSNFile(Constant.PATH_PCBA_SN);
        lp.add(new Property("PCBA SERIAL NUMBER", PCBA_SN));

        String EMMC_SERIAL_NUMBER = SystemInfoTools.getDeviceSNFromFile();
        lp.add(new Property("EMMC SERIAL NUMBER", EMMC_SERIAL_NUMBER));

        String lenovoSN=Build.SERIAL;
        lp.add(new Property("WHIRLPOOL SERIAL NUMBER", lenovoSN));

        dataEntity = new DataEntity();
        dataEntity.VERSION = fingerprint;
        dataEntity.GOOGLE_FSI=googleFsi;
        dataEntity.LENOVO_SN=lenovoSN;
        dataEntity.serialNum = PCBA_SN;
        dataEntity.boardSerialNum = EMMC_SERIAL_NUMBER;
        dataEntity.CPU = cpuinfo;
        dataEntity.RAM = SystemInfoTools.formatSize(ramMemory);
        dataEntity.Rom = SystemInfoTools.formatSize(romMemroy);
        dataEntity.BTMacAddr = bt_mac;
        dataEntity.MANUFACTURER = MANUFACTURER;
        PcbaEntity.getInstants().setDataCheckResult(dataEntity);
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
                convertView = LayoutInflater.from(PCBADataCheckTestActivity.this).inflate(R.layout.list_item_pcba_system_property, parent, false);
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

    private long lastMillis = 0;

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        if (event.getAction() == KeyEvent.ACTION_DOWN)
        {
            long currentMillis = System.currentTimeMillis();
            if (currentMillis - lastMillis > 2000)
            {
                lastMillis = currentMillis;
                Toast.makeText(this, getString(R.string.double_click_exit), Toast.LENGTH_SHORT).show();
            } else
            {
                finish();
            }
        }
        return false;
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        Gson gson = new Gson();
        String mContent = gson.toJson(PcbaEntity.getInstants().getDataCheckResult());
        setDataResult(mContent);
        wifiManageUtils.closeWifi();
        if(mReceiver!=null){
            unregisterReceiver(mReceiver);
            mReceiver=null;
        }

    }

    private void setDataResult(String remark)
    {
        try
        {
            JSONObject resultJson = new JSONObject();
            resultJson.put("msg_type", Protocol.MSG_TEST_RESULT);
            resultJson.put("test_item", Protocol.PCBA_TEST_ITEM_DATACHECK);
            resultJson.put("test_result", 0);
            JSONObject remake = new JSONObject(remark);
            resultJson.put("remark", remake);
            ConnectManager.getInstance().sendData(resultJson.toString());
        } catch (JSONException e)
        {
            e.printStackTrace();
        }
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
                case WifiManager.WIFI_STATE_CHANGED_ACTION://wifi打开与否
                    int wifistate = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, WifiManager.WIFI_STATE_DISABLED);
                    Log.e("TAG", "wifi状态改变" + wifistate);
                    if (wifistate == WifiManager.WIFI_STATE_DISABLED)
                    {
                        Log.e("TAG", "系统关闭wifi");
                    } else if (wifistate == WifiManager.WIFI_STATE_ENABLED)
                    {
                        //系统wifi开启
                        handler.sendEmptyMessage(0);

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

    public void addWifiMacAddress()
    {
        String mac = GetWifiMacUtils.getMac().toUpperCase();
        lp.add(4, new Property("WIFI MAC", mac));
        pa.notifyDataSetChanged();
        dataEntity.MacAddr = mac;
    }

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
}
