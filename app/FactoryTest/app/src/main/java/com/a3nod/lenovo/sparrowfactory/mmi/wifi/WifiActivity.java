package com.a3nod.lenovo.sparrowfactory.mmi.wifi;

import android.Manifest;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.net.NetworkInfo;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ListView;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.pcba.PCBAHandler;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;


/**
 * Created by jianzhou.peng on 2017/11/10.
 */

public class WifiActivity extends Activity
{
    public static final String TAG = "MMI_WIFI_TEST";
    public static final int TEST_SUCCESS = 1;
    public static final int SEND_FROM_TIMER = 2;
    public static final int TEST_FAIL = 3;


    private WifiManageUtils mWifiManageUtils = null;
    private ListView listView = null;
    private WifiAdapter mWifiAdapter = null;
    private List<ScanResult> list = new ArrayList<>();
    private List<ScanResult> tempList = new ArrayList<>();
    private WifiManager mWifiManager;
    private TextView tv_pass, tv_fail, tv_data;
    private Timer timer;
	private TimerTask timerTask;
    private int flag = 0;
    private int resultFlag = 0;
    private int testModel = Constant.INTENT_VALUE_TEST_MODEL_PCBA;

    public CheckBox cbx_wifi_test;
    private volatile boolean isWifiTesting = false;
    PCBAHandler pcbaHandler;


    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.wifi_activity);

        testModel = getIntent().getIntExtra(Constant.INTENT_KEY_TEST_MODEL, Constant.INTENT_VALUE_TEST_MODEL_PCBA);
        if (testModel == Constant.INTENT_VALUE_TEST_MODEL_PCBA)
        {
            CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_PCBA);
        } else
        {
            CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_MMI);
        }

        pcbaHandler = new PCBAHandler(this);

        initView();
        initConfirmButton();

        final IntentFilter filter = new IntentFilter();
        filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        //="android.net.wifi.STATE_CHANGE"  监听wifi状态的变化
        registerReceiver(mReceiver, filter);
        mWifiAdapter = new WifiAdapter(this, list);
        listView.setAdapter(mWifiAdapter);
        mWifiManageUtils = WifiManageUtils.getInstance(getApplicationContext());
        mWifiManageUtils.initManager();
        mWifiManager = mWifiManageUtils.getWifiManager();
        permissionForM();
        //startOpenWifi();

    }

    Handler handler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case TEST_SUCCESS:
                    Log.e("TAG", "扫描到数据");
                    Log.e("TAG", "没有连接上");
                    if (hasBig50(list)){
                        tv_pass.setVisibility(View.VISIBLE);
                    }
                    tv_data.setVisibility(View.INVISIBLE);
                    tv_fail.setVisibility(View.INVISIBLE);
                    if (msg.arg1 ==SEND_FROM_TIMER){
                        list.clear();
                        list.addAll(tempList);
                    }
                    mWifiAdapter.setData(list);
                    mWifiAdapter.notifyDataSetChanged();

                    if (list != null && list.size() > 0)
                    {
                        if(hasBig50(list)){
                            findViewById(R.id.btn_result_pass).setEnabled(true);
                        }
                        if (testModel == Constant.INTENT_VALUE_TEST_MODEL_MMI && !MMITestProcessManager.getInstance().isTesting())
                        {
                            return;
                        }
                        pcbaHandler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_SUCCESS, PCBAHandler.DELAY);
                    }
                    break;
                case TEST_FAIL:

                    Log.e("TAG", "扫描不到消息");
                    //扫描不到数据
                    tv_pass.setVisibility(View.INVISIBLE);
                    tv_data.setVisibility(View.VISIBLE);
                    tv_fail.setVisibility(View.VISIBLE);
                    if (testModel == Constant.INTENT_VALUE_TEST_MODEL_MMI && !MMITestProcessManager.getInstance().isTesting())
                    {
                        return;
                    }
                    pcbaHandler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_FAIL, PCBAHandler.DELAY);
                    break;
                default:
                    break;
            }
            super.handleMessage(msg);
        }
    };

    private boolean hasBig50(List<ScanResult> list){
        for (ScanResult result:list){
            if (Math.abs(result.level)<50&&!Constant.TEST_TYPE_MMI_AUDO){
                return true;
            }
        }
        return false;
    }

    private void initView()
    {
        listView = (ListView) findViewById(R.id.wifi_list);
        tv_data = (TextView) findViewById(R.id.no_data);
        tv_pass = (TextView) findViewById(R.id.tv_pass);
        tv_fail = (TextView) findViewById(R.id.tv_fail);
        cbx_wifi_test = findViewById(R.id.cb_start_network_test);
        cbx_wifi_test.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
        {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
            {
                if (isChecked)
                {
                    startWifiTest();
                } else
                {
                    isWifiTesting = false;
                }

            }
        });
    }

    public void startWifiTest()
    {
        LogUtil.i("startWifiTest");
        new Thread()
        {
            @Override
            public void run()
            {
                super.run();
                isWifiTesting = true;
                while (isWifiTesting)
                {
                    String http = HttpUtil.requestGETWebService("http://image.baidu.com/search/detail", "ct=503316480&z=0&ipn=d&word=风景图片&step_word=&hs=0&pn=12&spn=0&di=76423349110&pi=0&rn=1&tn=baiduimagedetail&is=0%2C0&istype=2&ie=utf-8&oe=utf-8&in=&cl=2&lm=-1&st=-1&cs=2779717376%2C1807907918&os=822530736%2C2132461744&simid=0%2C0&adpicid=0&lpn=0&ln=1992&fr=&fmq=1515737787943_R&fm=result&ic=0&s=undefined&se=&sme=&tab=0&width=&height=&face=undefined&ist=&jit=&cg=&bdtype=13&oriquery=&objurl=http%3A%2F%2Fimgsrc.baidu.com%2Fimgad%2Fpic%2Fitem%2Fb21bb051f8198618b4505a5040ed2e738ad4e6cb.jpg&fromurl=ippr_z2C%24qAzdH3FAzdH3Fooo_z%26e3Bp7h7vitgw_z%26e3BvgAzdH3F6AzdH3Fri5p5AzdH3FetjoAzdH3Ft1AzdH3Fdnccmnanl8l9AzdH3F&gsm=0&rpstart=0&rpnum=0");
                    LogUtil.i(http);
                    try
                    {
                        Thread.sleep(100);
                    } catch (InterruptedException e)
                    {
                        e.printStackTrace();
                    }
                }
            }
        }.start();
    }

    public void stopWifiTest()
    {
        isWifiTesting = false;
    }

    private void permissionForM()
    {
        if (checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED || checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED)
        {
            requestPermissions(new String[]{
                    Manifest.permission.ACCESS_FINE_LOCATION,
                    Manifest.permission.ACCESS_COARSE_LOCATION
            }, 1);
        } else
        {
            startOpenWifi();
            Log.e("TAG", "权限已经授予");
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults)
    {
        if (requestCode == 1)
        {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED)
            {
                startOpenWifi();
                Log.e("TAG", "授予权限");
            }
        }
        // super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    private void startOpenWifi()
    {
        mWifiManageUtils.openWifi();//开启wfi
    }

    @Override
    protected void onDestroy()
    {
    	cancelTimer();
        mWifiManageUtils.closeWifi();
        unregisterReceiver(mReceiver);
        super.onDestroy();
    }

    public void initConfirmButton()
    {
        Button btn_pass = findViewById(R.id.btn_result_pass);
        btn_pass.setEnabled(false);
        btn_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if (testModel == Constant.INTENT_VALUE_TEST_MODEL_PCBA)
                {
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_WIFI, true, "007");
                } else if (testModel == Constant.INTENT_VALUE_TEST_MODEL_MMI)
                {
                    ASSYEntity.getInstants().setWifiTestResult(true);
                    MMITestProcessManager.getInstance().toNextTest();
                }
                finish();
            }
        });
        Button btn_reset = findViewById(R.id.btn_result_reset);
        btn_reset.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                resetTest();
            }
        });
        Button btn_fail = findViewById(R.id.btn_result_fail);
        btn_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if (testModel == Constant.INTENT_VALUE_TEST_MODEL_PCBA)
                {
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_WIFI, false, "007");
                } else if (testModel == Constant.INTENT_VALUE_TEST_MODEL_MMI)
                {
                    ASSYEntity.getInstants().setWifiTestResult(false);
                    MMITestProcessManager.getInstance().testFail();
                }
                finish();
            }
        });

    }


    private void resetTest()
    {
        LogUtil.i( "resetTest " );
        flag = 0;
        tv_pass.setVisibility(View.INVISIBLE);
        tv_fail.setVisibility(View.INVISIBLE);
        tv_data.setVisibility(View.INVISIBLE);
        list.clear();
        mWifiAdapter.setData(list);
        mWifiAdapter.notifyDataSetChanged();
        resultFlag=0;
        cancelTimer();
        startScan();
    }
	private void cancelTimer(){
        if(timer!=null) {
            timer.cancel();
            timer = null;
        }
        if(timerTask!=null) {
            timerTask.cancel();
            timerTask = null;
        }
    }
    private void startScan()
    {
        LogUtil.i( "startScan " );
        if(timer == null){
            timer = new Timer();
            timerTask = new TimerTask()
            {
                @Override
                public void run()
                {
                    Log.e("TAG", "开始扫描");
                    tempList.clear();
                    mWifiManageUtils.startScan();
                    tempList = mWifiManageUtils.getScanResult();
                    if (tempList != null && tempList.size() > 0)
                    {
                        Log.e("TAG", "list.size 长度为 " + tempList.size());
                        Message msg = new Message();
                        msg.what = TEST_SUCCESS;
                        msg.arg1 = SEND_FROM_TIMER;
                        handler.sendMessage(msg);
                        timer.cancel();
                    } else
                    {
                        if (resultFlag == 4)
                        {
                            Log.e("TAG", " 扫描次数 =" + resultFlag);
                            handler.sendEmptyMessage(TEST_FAIL);
                            resultFlag = 0;
                            timer.cancel();
                        }
                        resultFlag++;
                    }
                }
            };
            timer.schedule(timerTask, 4000, 1000);
        }else{
            Log.e("TAG", "timer is not null");
        }
    }

    class ConnectRunnable implements Runnable
    {

        private String ssid;
        private String passWord;
        private int type;

        public ConnectRunnable(String ssid, String passWord, int type)
        {
            this.ssid = ssid;
            this.passWord = passWord;
            this.type = type;
        }

        @Override
        public void run()
        {
            try
            {
                Thread.sleep(500);
                if (mWifiManager != null)
                {
                    while (mWifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLING)
                    {
                        try
                        {
                            Thread.sleep(8000);
                        } catch (Exception e)
                        {
                            e.printStackTrace();
                        }
                    }
                }
                WifiConfiguration wifiConfiguration = mWifiManageUtils.CreateWifiInfo(ssid, passWord, type);
                if (wifiConfiguration == null)
                {
                    return;
                }
                int netID = mWifiManager.addNetwork(wifiConfiguration);
                boolean enable = mWifiManager.enableNetwork(netID, true);
                // boolean connected = mWifiManager.reconnect();
            } catch (InterruptedException e)
            {
                e.printStackTrace();
            }
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
                        Log.e("TAG", "系统开启wifi");
                        startScan();
                    } else if (wifistate == WifiManager.WIFI_STATE_ENABLING)
                    {
                        Log.e("TAG", "系统开启wifi wifi可用");
                    } else if (wifistate == WifiManager.WIFI_STATE_UNKNOWN)
                    {
                        handler.sendEmptyMessage(3);
                    }
                    break;
                case WifiManager.NETWORK_STATE_CHANGED_ACTION:
                    Log.e("TAG", "网络状态改变");

                    NetworkInfo info = intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);
                    if (info.getState().equals(NetworkInfo.State.DISCONNECTED))
                    {
                        Log.e("TAG", "wifi网络连接断开");
                    } else if (info.getState().equals(NetworkInfo.State.CONNECTED))
                    {
                        Log.e("TAG", "wifi已经连接");

                    }
                    break;
            }

        }
    };


}
