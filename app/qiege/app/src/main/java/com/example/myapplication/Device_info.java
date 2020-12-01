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
     * ���SD���ܴ�С
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
     * ���sd��ʣ�������������ô�С
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
     * ��û���ROM�ܴ�С
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
     * ��û������ROM
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




    // ��Ļ��ȣ����أ�
    public static int getWindowWidth(Activity context) {
        DisplayMetrics metric = new DisplayMetrics();
        context.getWindowManager().getDefaultDisplay().getMetrics(metric);
        return metric.widthPixels;
    }


    // ��Ļ�߶ȣ����أ�
    public static int getWindowHeight(Activity context) {
        DisplayMetrics metric = new DisplayMetrics();
        context.getWindowManager().getDefaultDisplay().getMetrics(metric);
        return metric.heightPixels;
    }




    /**
     * ��ȡcpuʹ����
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
            * ��ȡ��ǰ�ֻ�ϵͳ���ԡ�
            *
            * @return ���ص�ǰϵͳ���ԡ����磺��ǰ���õ��ǡ�����-�й������򷵻ء�zh-CN��
            */
    public static String getSystemLanguage() {
        return Locale.getDefault().getLanguage();
    }

    /**
     * ��ȡ��ǰϵͳ�ϵ������б�(Locale�б�)
     *
     * @return  �����б�
     */
    public static Locale[] getSystemLanguageList() {
        return Locale.getAvailableLocales();
    }

    /**
     * ��ȡ��ǰ�ֻ�ϵͳ�汾��
     *
     * @return  ϵͳ�汾��
     */
    public static String getSystemVersion() {
        return android.os.Build.VERSION.RELEASE;
    }

    /**
     * ��ȡ�ֻ��ͺ�
     *
     * @return  �ֻ��ͺ�
     */
    public static String getSystemModel() {
        return android.os.Build.MODEL;
    }

    /**
     * ��ȡ�ֻ�����
     *
     * @return  �ֻ�����
     */
    public static String getDeviceBrand() {
        return android.os.Build.BRAND;
    }

    /**
     * ��ȡ�ֻ�IMEI(��Ҫ��android.permission.READ_PHONE_STATE��Ȩ��)
     *
     * @return  �ֻ�IMEI
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
            result = "��ͨƽ̨(Qualcomm) - " + result;
        } else if (result.matches("mt[0-9]*")) {
            result = "MTKƽ̨(MediaTek) - " + result;
        }
        return result;
    }

    /**
     * �ں˰汾
     *
     * @return
     */
    public static String getKernelVersion() {
        return System.getProperty("os.version") + " (" + Build.VERSION.INCREMENTAL + ")";
    }

    /**
     * �ں˼ܹ�
     *
     * @return
     */
    public static String getKernelArchitecture() {
        return System.getProperty("os.arch");
    }

    /**
     * Java VM �����
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
