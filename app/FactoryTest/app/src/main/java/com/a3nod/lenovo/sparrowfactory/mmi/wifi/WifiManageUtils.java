package com.a3nod.lenovo.sparrowfactory.mmi.wifi;
import android.content.Context;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.util.Log;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by jianzhou.peng on 2017/11/10.
 */

public class WifiManageUtils {

    private static volatile  WifiManageUtils instance = null;
    private static Context mContext = null;
    private WifiManager mWifiManager = null;
    private List<ScanResult> mScanResults = null;

    private WifiManageUtils(){

    }

    /**
     * 单例模式获取该类实例
     * @param context 上下文
     * @return
     */
    public static WifiManageUtils getInstance(Context context){
        if(instance == null){
            synchronized (WifiManageUtils.class){
                if(instance == null){
                    instance = new WifiManageUtils();
                    mContext = context;
                }
            }
        }
        return instance;
    }

    public void initManager(){
        mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
    }

    public WifiManager getWifiManager(){
        if(mWifiManager!= null){
            return mWifiManager;
        }
        return null;
    }
    /**
     * 开启wifi
     */
    public void openWifi(){
        if(!mWifiManager.isWifiEnabled()){
            Log.e("TAG","wifi没有开启,打开wifi");
            mWifiManager.setWifiEnabled(true);

        }else if(mWifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLING ){
            Log.e("TAG","wifi已经打开");
        }else{
            Log.e("TAG","wifi已经打开");
        }
    }

    /**
     * 关闭wifi
     */
    public void closeWifi(){
        if(mWifiManager.isWifiEnabled()){
            Log.e("TAG","wifi开启,关闭wifi");
            mWifiManager.setWifiEnabled(false);
        }else if(mWifiManager.getWifiState() == WifiManager.WIFI_STATE_DISABLED){
            Log.e("TAG","wifi已经关闭");
        }else if(mWifiManager.getWifiState() == WifiManager.WIFI_STATE_DISABLING){
            Log.e("TAG","wifi已经关闭");
        }else{
            Log.e("TAG","wifi已经关闭");
        }
    }

    /**
     * 开始扫描
     */
    public void startScan(){
        mWifiManager.startScan();
        List<ScanResult> results = mWifiManager.getScanResults();
        if(results == null){
            if(mWifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLED){
                 Log.e("TAG","当前区域没有无线网络");
            }else if(mWifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLING){
                 Log.e("TAG","wifi正在开启，请稍后重新点击扫描");
            }else{
                 Log.e("TAG","wifi没有开启，无法扫描");
            }
        }else {
                 Log.e("TAG","mScanResults 不为空 即result的长度为 = "+results.size());
            if(mScanResults == null){
                mScanResults = new ArrayList<>();
            }
            synchronized (mScanResults){
                for(ScanResult result : results){
                    Log.e("TAG","获取到的ScanResult result : results"+result.level);
                    if (result.SSID == null || result.SSID.length() == 0 || result.capabilities.contains("[IBSS]")) {
                        continue;
                    }
                    boolean found = false;
                    for(ScanResult item:mScanResults){
                        if(item.SSID.equals(result.SSID)&&item.capabilities.equals(result.capabilities)){
                            found = true;
                            break;
                        }
                    }
                    if(!found){
                        mScanResults.add(result);
                    }
                }
            }
        }
    }

    public List<ScanResult> getScanResult(){
        if(mScanResults != null){
            Log.e("TAG","mScanResults!= null"+mScanResults.size());
            return mScanResults;
        }
        return null;
    }

    public WifiConfiguration CreateWifiInfo(String SSID, String Password, int Type)
    {
        WifiConfiguration config = new WifiConfiguration();
        config.allowedAuthAlgorithms.clear();
        config.allowedGroupCiphers.clear();
        config.allowedKeyManagement.clear();
        config.allowedPairwiseCiphers.clear();
        config.allowedProtocols.clear();
        config.SSID = "\"" + SSID + "\"";

        WifiConfiguration tempConfig = this.IsExsits(SSID);
        if(tempConfig != null) {
            mWifiManager.removeNetwork(tempConfig.networkId);
        }

        if(Type == 1) //WIFICIPHER_NOPASS
        {
            config.wepKeys[0] = "";
            config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
            config.wepTxKeyIndex = 0;
        }
        if(Type == 2) //WIFICIPHER_WEP
        {
            config.hiddenSSID = true;
            config.wepKeys[0]= "\""+Password+"\"";
            config.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.SHARED);
            config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
            config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
            config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.WEP40);
            config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.WEP104);
            config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
            config.wepTxKeyIndex = 0;
        }
        if(Type == 3) //WIFICIPHER_WPA
        {
            config.preSharedKey = "\""+Password+"\"";
            config.hiddenSSID = true;
            config.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.OPEN);
            config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
            config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
            config.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
            //config.allowedProtocols.set(WifiConfiguration.Protocol.WPA);
            config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
            config.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
            config.status = WifiConfiguration.Status.ENABLED;
        }
        return config;
    }

    public WifiConfiguration IsExsits(String SSID)
    {
        List<WifiConfiguration> existingConfigs = mWifiManager.getConfiguredNetworks();
        if(existingConfigs!=null&&existingConfigs.size()>0){
            for (WifiConfiguration existingConfig : existingConfigs)
            {
                if (existingConfig.SSID.equals("\""+SSID+"\""))
                {
                    return existingConfig;
                }
            }
        }
        return null;
    }
}
