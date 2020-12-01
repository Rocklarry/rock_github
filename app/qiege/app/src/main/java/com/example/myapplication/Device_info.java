package com.example.myapplication;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.filterfw.geometry.Point;
import android.os.Build;
import android.os.Environment;
import android.os.StatFs;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.text.format.Formatter;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;
import java.util.Locale;

class Device_info {

    static final float CM_PER_INCH = 2.54f;
    /**
     * 获得SD卡总大小
     *
     * @return
     */
    public static String getSDTotalSize(Context mContext) {
        File path = Environment.getExternalStorageDirectory();
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSize();
        long totalBlocks = stat.getBlockCount();
        return Formatter.formatFileSize(mContext, blockSize * totalBlocks);
    }
    /**
     * 获得sd卡剩余容量，即可用大小
     *
     * @return
     */
    public static String getSDAvailableSize(Context mContext) {
        File path = Environment.getExternalStorageDirectory();
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSize();
        long availableBlocks = stat.getAvailableBlocks();
        return Formatter.formatFileSize(mContext, blockSize * availableBlocks);
    }

    /**
     * 获得机身ROM总大小
     *
     * @return
     */
    public static String getRomTotalSize(Context mContext) {
        File path = Environment.getDataDirectory();
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSize();
        long totalBlocks = stat.getBlockCount();
        return Formatter.formatFileSize(mContext, blockSize * totalBlocks);
    }

    /**
     * 获得机身可用ROM
     *
     * @return
     */
    public static String getRomAvailableSize(Context mContext) {
        File path = Environment.getDataDirectory();
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSize();
        long availableBlocks = stat.getAvailableBlocks();
        return Formatter.formatFileSize(mContext, blockSize * availableBlocks);
    }




    // 屏幕宽度（像素）
    public static int getWindowWidth(Activity context) {
        DisplayMetrics metric = new DisplayMetrics();
        context.getWindowManager().getDefaultDisplay().getMetrics(metric);
        return metric.widthPixels;
    }


    // 屏幕高度（像素）
    public static int getWindowHeight(Activity context) {
        DisplayMetrics metric = new DisplayMetrics();
        context.getWindowManager().getDefaultDisplay().getMetrics(metric);
        return metric.heightPixels;
    }




    /**
     * 获取cpu使用率
     * @return
     */
    public static float getCpuUsed() {
        try {
            RandomAccessFile reader = new RandomAccessFile("/proc/stat", "r");
            String load = reader.readLine();
            String[] toks = load.split(" ");
            long idle1 = Long.parseLong(toks[5]);
            long cpu1 = Long.parseLong(toks[2]) + Long.parseLong(toks[3]) + Long.parseLong(toks[4])
                    + Long.parseLong(toks[6]) + Long.parseLong(toks[7]) + Long.parseLong(toks[8]);
            try {
                Thread.sleep(360);
            } catch (Exception e) {
                e.printStackTrace();
            }
            reader.seek(0);
            load = reader.readLine();
            reader.close();
            toks = load.split(" ");
            long idle2 = Long.parseLong(toks[5]);
            long cpu2 = Long.parseLong(toks[2]) + Long.parseLong(toks[3]) + Long.parseLong(toks[4])
                    + Long.parseLong(toks[6]) + Long.parseLong(toks[7]) + Long.parseLong(toks[8]);
            return (float) (cpu2 - cpu1) / ((cpu2 + idle2) - (cpu1 + idle1));
        } catch (IOException ex) {
            ex.printStackTrace();
        }
        return 0;
    }




    /**
            * 获取当前手机系统语言。
            *
            * @return 返回当前系统语言。例如：当前设置的是“中文-中国”，则返回“zh-CN”
            */
    public static String getSystemLanguage() {
        return Locale.getDefault().getLanguage();
    }

    /**
     * 获取当前系统上的语言列表(Locale列表)
     *
     * @return  语言列表
     */
    public static Locale[] getSystemLanguageList() {
        return Locale.getAvailableLocales();
    }

    /**
     * 获取当前手机系统版本号
     *
     * @return  系统版本号
     */
    public static String getSystemVersion() {
        return android.os.Build.VERSION.RELEASE;
    }

    /**
     * 获取手机型号
     *
     * @return  手机型号
     */
    public static String getSystemModel() {
        return android.os.Build.MODEL;
    }

    /**
     * 获取手机厂商
     *
     * @return  手机厂商
     */
    public static String getDeviceBrand() {
        return android.os.Build.BRAND;
    }

    /**
     * 获取手机IMEI(需要“android.permission.READ_PHONE_STATE”权限)
     *
     * @return  手机IMEI
     */
    public static String getIMEI(Context ctx) {
        TelephonyManager tm = (TelephonyManager) ctx.getSystemService(Activity.TELEPHONY_SERVICE);
        if (tm != null) {
            return tm.getDeviceId();
        }
        return null;
    }

    public static String getHardWare() {
        String result = Build.HARDWARE;
        if (result.matches("qcom")) {
            result = "高通平台(Qualcomm) - " + result;
        } else if (result.matches("mt[0-9]*")) {
            result = "MTK平台(MediaTek) - " + result;
        }
        return result;
    }

    /**
     * 内核版本
     *
     * @return
     */
    public static String getKernelVersion() {
        return System.getProperty("os.version") + " (" + Build.VERSION.INCREMENTAL + ")";
    }

    /**
     * 内核架构
     *
     * @return
     */
    public static String getKernelArchitecture() {
        return System.getProperty("os.arch");
    }

    /**
     * Java VM 虚拟机
     *
     * @return
     */
    public static String getJavaVM() {
        String result = null;
        result = System.getProperty("java.vm.name");
        if (result != null) {
            result = result + System.getProperty("java.vm.version");
        }

        return result;
    }

    public static boolean isRooted() {
        // nexus 5x "/su/bin/"
        String[] paths = {"/system/xbin/", "/system/bin/", "/system/sbin/", "/sbin/", "/vendor/bin/", "/su/bin/"};
        try {
            for (int i = 0; i < paths.length; i++) {
                String path = paths[i] + "su";
                if (new File(path).exists()) {
                    String execResult = exec(new String[]{"ls", "-l", path});
                    Log.d("cyb", "isRooted=" + execResult);
                    if (TextUtils.isEmpty(execResult) || execResult.indexOf("root") == execResult.lastIndexOf("root")) {
                        return false;
                    }
                    return true;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    private static String exec(String[] exec) {
        String ret = "";
        ProcessBuilder processBuilder = new ProcessBuilder(exec);
        try {
            Process process = processBuilder.start();
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            String line;
            while ((line = bufferedReader.readLine()) != null) {
                ret += line;
            }
            process.getInputStream().close();
            process.destroy();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return ret;
    }

}
