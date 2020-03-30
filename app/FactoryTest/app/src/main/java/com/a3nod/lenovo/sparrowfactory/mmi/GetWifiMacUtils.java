package com.a3nod.lenovo.sparrowfactory.mmi;

import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;

import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.io.FileReader;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.io.Reader;
import java.net.NetworkInterface;
import java.net.SocketException;

/**
 * Created by jianzhou.peng on 2018/1/22.
 */

public class GetWifiMacUtils
{
    /**
     * 获取手机的MAC地址
     *
     * @return
     */

    public static String getMac()
    {
        String str = "";
        String macSerial = "";
        try
        {
            Process pp = Runtime.getRuntime().exec("cat /sys/class/net/wlan0/address");
            InputStreamReader ir = new InputStreamReader(pp.getInputStream());
            LineNumberReader input = new LineNumberReader(ir);
            for (; null != str; )
            {
                str = input.readLine();
                if (str != null)
                {
                    macSerial = str.trim();// 去空格
                    LogUtil.e("TAG", "macSerial" + macSerial);
                    break;
                }
            }
        } catch (Exception ex)
        {
            ex.printStackTrace();
        }
        if ("".equals(macSerial))
        {
            try
            {
                return loadFileAsString("/sys/class/net/eth0/address")
                        .toUpperCase().substring(0, 17);
            } catch (Exception e)
            {
                e.printStackTrace();
            }
        }
        if ("".equals(macSerial)){
            macSerial = getMacByApi();
        }
        return macSerial;
    }

    private static String loadFileAsString(String fileName) throws Exception
    {
        FileReader reader = new FileReader(fileName);
        String text = loadReaderAsString(reader);
        reader.close();
        return text;
    }

    private static String loadReaderAsString(Reader reader) throws Exception
    {
        StringBuilder builder = new StringBuilder();
        char[] buffer = new char[4096];
        int readLength = reader.read(buffer);
        while (readLength >= 0)
        {
            builder.append(buffer, 0, readLength);
            readLength = reader.read(buffer);
        }
        return builder.toString();
    }

    private static String getMacByApi(){
        String mac = "";
        try {
            NetworkInterface networkInterface = NetworkInterface.getByName("wlan0");
            byte[] tempmac = networkInterface.getHardwareAddress();
            for (int i = 0;i<tempmac.length;i++){
                String hexStr = Integer.toHexString(tempmac[i]);
                mac += hexStr.substring(hexStr.length()-2) +":" ;
            }
        }catch (SocketException e){
            LogUtil.e("TAG", "networkInterface get fail");
        }
        return mac;
    }
}

