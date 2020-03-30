package com.a3nod.lenovo.sparrowfactory.tool;

import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.util.Log;

import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.io.Reader;
import java.net.NetworkInterface;
import java.util.Collections;
import java.util.List;

/**
 * Description
 * Created by aaa on 2017/11/23.
 */

public class MacAddressTool
{
    public static String getMACByFile()
    {
        String macSerial = null;
        String str = "";

        try {
            FileInputStream fis=new FileInputStream("/sys/class/net/dummy0/address");
            InputStreamReader ir = new InputStreamReader(fis);
            LineNumberReader input = new LineNumberReader(ir);

            for (; null != str; ) {
                str = input.readLine();
                if (str != null) {
                    macSerial = str.trim();// 去空格
                    break;
                }
            }
        } catch (IOException ex) {
            // 赋予默认值
            ex.printStackTrace();
        }
        return macSerial;

    }
    public static String getMacAAA(){
        try
        {
           Process pp= Runtime.getRuntime().exec("ls");
            printCMD(pp);
            pp= Runtime.getRuntime().exec("cd sys");

            pp= Runtime.getRuntime().exec("ls");
            printCMD(pp);
            pp= Runtime.getRuntime().exec("cd class");
            pp= Runtime.getRuntime().exec("ls");
            printCMD(pp);
            pp= Runtime.getRuntime().exec("cd net");
            pp= Runtime.getRuntime().exec("ls");
            printCMD(pp);
            pp= Runtime.getRuntime().exec("cd wlan0");
            pp= Runtime.getRuntime().exec("ls");
            printCMD(pp);

        } catch (IOException e)
        {
            e.printStackTrace();
        }
        return  "";
    }
    public static void printCMD(Process pp){
        InputStreamReader is = new InputStreamReader(pp.getInputStream());
        BufferedReader br = new BufferedReader (is);
        LogUtil.i("printCMD aaaaaaaaaaaaaaaaaaa ");
        String str=null;
        try
        {
            while ((str=br.readLine())!=null)
            {
                LogUtil.i(str);
            }
        } catch (IOException e)
        {
            e.printStackTrace();
        }
    }
    public static String getLocalMacAddressFromBusybox(Context context){
        String result = "";
        String Mac = "";
        result = callCmd("busybox ifconfig","HWaddr");

        //如果返回的result == null，则说明网络不可取
        if(result==null){
            return context.getResources().getString(R.string.network_error);
        }

        //对该行数据进行解析
        //例如：eth0      Link encap:Ethernet  HWaddr 00:16:E8:3E:DF:67
        if(result.length()>0 && result.contains("HWaddr")){
            Mac = result.substring(result.indexOf("HWaddr")+6, result.length()-1);
            Log.i("test","Mac:"+Mac+" Mac.length: "+Mac.length());

             /*if(Mac.length()>1){
                 Mac = Mac.replaceAll(" ", "");
                 result = "";
                 String[] tmp = Mac.split(":");
                 for(int i = 0;i<tmp.length;++i){
                     result +=tmp[i];
                 }
             }*/
            result = Mac;
            Log.i("test",result+" result.length: "+result.length());
        }
        return result;
    }

    private static String callCmd(String cmd,String filter) {
        String result = "";
        String line = "";
        try {
            Process proc = Runtime.getRuntime().exec(cmd);
            InputStreamReader is = new InputStreamReader(proc.getInputStream());
            BufferedReader br = new BufferedReader (is);

            //执行命令cmd，只取结果中含有filter的这一行
            while ((line = br.readLine ()) != null && !line.contains(filter)) {
                //result += line;
                Log.i("test","line: "+line);
            }

            result = line;
            Log.i("test","result: "+result);
        }
        catch(Exception e) {
            e.printStackTrace();
        }
        return result;
    }
    public static String getMacAddr() {
        try {
            List<NetworkInterface> all = Collections.list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface nif : all) {
                if (!nif.getName().equalsIgnoreCase("wlan0")) continue;

                byte[] macBytes = nif.getHardwareAddress();
                if (macBytes == null) {
                    return "";
                }

                StringBuilder res1 = new StringBuilder();
                for (byte b : macBytes) {
                    res1.append(String.format("%02X:",b));
                }

                if (res1.length() > 0) {
                    res1.deleteCharAt(res1.length() - 1);
                }
                return res1.toString();
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return "02:00:00:00:00:00";
    }
    public static String getMacByAPI(Context context){
        WifiManager wifi = (WifiManager) context.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        WifiInfo info = wifi.getConnectionInfo();
        String str = info.getMacAddress();
        if (str == null) str = "";
        return str;
    }

    public static String getMacByCMD(){
        String str="";
        String macSerial="";
        try {
            Runtime.getRuntime().exec("su");
            Runtime.getRuntime().exec("chmod 777 /sys/class/net/wlan0/address");
            Process pp = Runtime.getRuntime().exec(
                    "cat /sys/class/net/wlan0/address");
            InputStreamReader ir = new InputStreamReader(pp.getInputStream());
            LineNumberReader input = new LineNumberReader(ir);

            for (; null != str;) {
                str = input.readLine();
                if (str != null) {
                    macSerial = str.trim();// 去空格
                    break;
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        if (macSerial == null || "".equals(macSerial)) {
            try {
                return loadFileAsString("/sys/class/net/eth0/address")
                        .toUpperCase().substring(0, 17);
            } catch (Exception e) {
                e.printStackTrace();

            }

        }
        return macSerial;
    }
    public static String loadFileAsString(String fileName) throws Exception {
        FileReader reader = new FileReader(fileName);
        String text = loadReaderAsString(reader);
        reader.close();
        return text;
    }
    public static String loadReaderAsString(Reader reader) throws Exception {
        StringBuilder builder = new StringBuilder();
        char[] buffer = new char[4096];
        int readLength = reader.read(buffer);
        while (readLength >= 0) {
            builder.append(buffer, 0, readLength);
            readLength = reader.read(buffer);
        }
        return builder.toString();
    }



}
