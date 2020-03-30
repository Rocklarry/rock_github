package com.a3nod.lenovo.sparrowfactory.tool;

import android.bluetooth.BluetoothAdapter;
import android.os.Build;
import android.os.Environment;
import android.os.StatFs;
import android.text.TextUtils;

import net.vidageek.mirror.dsl.Mirror;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.io.Reader;
import java.lang.reflect.Method;
import java.net.NetworkInterface;
import java.util.Collections;
import java.util.List;

/**
 * Description
 * Created by aaa on 2017/11/23.
 */

public class SystemInfoTools
{
    public static String SN_PATH = "";

    public static long getTotalMemory()
    {
        String str1 = "/proc/meminfo";
        String str2 = "";
        try
        {
            FileReader fr = new FileReader(str1);
            BufferedReader localBufferedReader = new BufferedReader(fr, 8192);
            long memory = 0;
            while ((str2 = localBufferedReader.readLine()) != null)
            {
                LogUtil.i("---" + str2);
                if (str2.contains("MemTotal"))
                {
                    memory = Long.parseLong(str2.split("\\s+")[1]);
                }
            }
            localBufferedReader.close();
            fr.close();
            return memory * 1024;
        } catch (IOException e)
        {
            e.printStackTrace();
        }
        return 0;
    }


    public static void writeSNFile(String serial, String filePath)
    {
        if (TextUtils.isEmpty(serial))
        {
            return;
        }
        File file = new File(filePath);
        if (file.exists())
        {
            file.delete();
        }
        try
        {
            FileWriter fw = new FileWriter(file);
            BufferedWriter bw = new BufferedWriter(fw);
            bw.write(serial);
            bw.close();
            fw.close();
        } catch (IOException e)
        {
            e.printStackTrace();
        }

    }

    public static String readSNFile()
    {
        String serial = null;
        try
        {
            FileReader fr = new FileReader(Environment.getExternalStorageDirectory().getAbsolutePath() + "/sn");
            BufferedReader br = new BufferedReader(fr);
            serial = br.readLine();
            br.close();
            fr.close();
        } catch (FileNotFoundException e)
        {
            e.printStackTrace();
        } catch (IOException e)
        {
            e.printStackTrace();
        }
        return serial;
    }

    public static String readSNFile(String path)
    {
        String serial = "";
        try
        {
            FileReader fr = new FileReader(path);
            BufferedReader br = new BufferedReader(fr);
            serial = br.readLine();
            br.close();
            fr.close();
        } catch (FileNotFoundException e)
        {
            e.printStackTrace();
        } catch (IOException e)
        {
            e.printStackTrace();
        }
        return serial;
    }

    public static String readSNFile1()
    {
        String serial = null;
        try
        {
            FileReader fr = new FileReader("/persist/aaa.txt");
            BufferedReader br = new BufferedReader(fr);
            serial = br.readLine();
            br.close();
            fr.close();
        } catch (FileNotFoundException e)
        {
            e.printStackTrace();
        } catch (IOException e)
        {
            e.printStackTrace();
        }
        return serial;
    }

    public static void setProperty(String key, String value)
    {
        try
        {
            Class<?> c = Class.forName("android.os.SystemProperties");
            Method set = c.getMethod("set", String.class, String.class);
            set.invoke(c, key, value);
        } catch (Exception e)
        {
            e.printStackTrace();
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

    public static String getSystemProperty(String propName)
    {
        String line;
        BufferedReader input = null;
        try
        {
            Process p = Runtime.getRuntime().exec("getprop " + propName);
            input = new BufferedReader(new InputStreamReader(p.getInputStream()), 1024);
            line = input.readLine();
            input.close();
        } catch (IOException ex)
        {
            LogUtil.e("Unable to read sysprop " + propName);
            return null;
        } finally
        {
            if (input != null)
            {
                try
                {
                    input.close();
                } catch (IOException e)
                {
                    LogUtil.e("Exception while closing InputStream" + e.toString());
                }
            }
        }
        return line;
    }

    public static long getRomMemroy()
    {
        long romInfo;
        //Total rom memory
        romInfo = getTotalInternalMemorySize();
        LogUtil.i("rom size : " + romInfo);

        long romAvailableInfo;
        romAvailableInfo = getAvailableInternalMemorySize();
        LogUtil.i("rom available size : " + romAvailableInfo);

        long sdcardRomInfo;
        sdcardRomInfo = getExternalMemorySize();
        LogUtil.i("flash size : " + sdcardRomInfo);


        long sdcardAvailableRomInfo;
        sdcardAvailableRomInfo = getAvailableExternalMemorySize();
        LogUtil.i("flash available size : " + sdcardAvailableRomInfo);


        return romInfo;
    }

    public static long getTotalInternalMemorySize()
    {
        File path = Environment.getDataDirectory();
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSize();
        long totalBlocks = stat.getBlockCount();
        return totalBlocks * blockSize;
    }

    /**
     * 获取手机内部可用存储空间
     *
     * @return 以M, G为单位的容量
     */
    public static long getAvailableInternalMemorySize()
    {
        File file = Environment.getDataDirectory();
        StatFs statFs = new StatFs(file.getPath());
        long availableBlocksLong = statFs.getAvailableBlocksLong();
        long blockSizeLong = statFs.getBlockSizeLong();
        return availableBlocksLong * blockSizeLong;
    }

    /**
     * 获取手机外部存储空间
     *
     * @return 以M, G为单位的容量
     */
    public static long getExternalMemorySize()
    {
        File file = Environment.getExternalStorageDirectory();
        StatFs statFs = new StatFs(file.getPath());
        long blockSizeLong = statFs.getBlockSizeLong();
        long blockCountLong = statFs.getBlockCountLong();
        return blockCountLong * blockSizeLong;
    }

    /**
     * 获取手机外部可用存储空间
     *
     * @return 以M, G为单位的容量
     */
    public static long getAvailableExternalMemorySize()
    {
        File file = Environment.getExternalStorageDirectory();
        StatFs statFs = new StatFs(file.getPath());
        long availableBlocksLong = statFs.getAvailableBlocksLong();
        long blockSizeLong = statFs.getBlockSizeLong();
        return availableBlocksLong * blockSizeLong;
    }

    public static String[] getVersion()
    {
        String[] version = {"null", "null", "null", "null"};
        String str1 = "/proc/version";
        String str2;
        String[] arrayOfString;
        try
        {
            FileReader localFileReader = new FileReader(str1);
            BufferedReader localBufferedReader = new BufferedReader(
                    localFileReader, 8192);
            str2 = localBufferedReader.readLine();
            arrayOfString = str2.split("\\s+");
            version[0] = arrayOfString[2];//KernelVersion
            localBufferedReader.close();
        } catch (IOException e)
        {
            e.printStackTrace();
        }
        version[1] = Build.VERSION.RELEASE;// firmware version
        version[2] = Build.MODEL;//model
        version[3] = Build.DISPLAY;//system version
        return version;
    }

    public static String getCpuInfo()
    {
        String str1 = "/proc/cpuinfo";
        String str2 = "";

        try
        {
            String[] arrayOfString = {};
            FileReader fr = new FileReader(str1);
            BufferedReader localBufferedReader = new BufferedReader(fr, 8192);
            while ((str2 = localBufferedReader.readLine()) != null)
            {
                if (str2.contains("Hardware"))
                {
                    LogUtil.i(str2);
                    arrayOfString = str2.split(":");
                    break;
                }
            }
            localBufferedReader.close();
            return arrayOfString[1];
        } catch (IOException e)
        {
            e.printStackTrace();
        }
        return "unknow";
    }

    public static String formatSize(long size)
    {
        String suffix = null;
        float fSize = 0;

        if (size >= 1024)
        {
            suffix = "KB";
            fSize = size / 1024;
            if (fSize >= 1024)
            {
                suffix = "MB";
                fSize /= 1024;
            }
            if (fSize >= 1024)
            {
                suffix = "GB";
                fSize /= 1024;
            }
        } else
        {
            fSize = size;
        }
        java.text.DecimalFormat df = new java.text.DecimalFormat("#0.00");
        StringBuilder resultBuffer = new StringBuilder(df.format(fSize));
        if (suffix != null)
            resultBuffer.append(suffix);
        return resultBuffer.toString();
    }

    public static String getMACByFile()
    {
        String macSerial = null;
        String str = "";

        try
        {
            FileInputStream fis = new FileInputStream("/sys/class/net/dummy0/address");
            InputStreamReader ir = new InputStreamReader(fis);
            LineNumberReader input = new LineNumberReader(ir);

            for (; null != str; )
            {
                str = input.readLine();
                if (str != null)
                {
                    macSerial = str.trim();// 去空格
                    break;
                }
            }
        } catch (IOException ex)
        {
            // 赋予默认值
            ex.printStackTrace();
        }
        if (macSerial == null)
        {
            macSerial = "";
        }
        return macSerial;

    }
    public static String getDeviceSNFromFile()
    {
        String macSerial = null;
        String str = "";

        try
        {
            FileInputStream fis = new FileInputStream("/sys/block/mmcblk0/device/serial");
            InputStreamReader ir = new InputStreamReader(fis);
            LineNumberReader input = new LineNumberReader(ir);

            for (; null != str; )
            {
                str = input.readLine();
                if (str != null)
                {
                    macSerial = str.trim();// 去空格
                    break;
                }
            }
        } catch (IOException ex)
        {
            // 赋予默认值
            ex.printStackTrace();
        }
        if (macSerial == null)
        {
            macSerial = "";
        }
        macSerial=macSerial.replaceAll("^0[x|X]", "");
        try
        {
            macSerial = Long.toHexString(Long.parseLong(macSerial, 16));
        }catch (NumberFormatException e){
            e.printStackTrace();
        }
        if ("".equals(macSerial)){
            macSerial = getSerialNumber();
        }
        return macSerial;

    }

    public static String getSerialNumber(){
        String serial = null;
        try {
            Class<?> c =Class.forName("android.os.SystemProperties");
            Method get =c.getMethod("get", String.class);
            serial = (String)get.invoke(c, "ro.serialno");
        } catch (Exception e) {
            e.printStackTrace();
        }
        return serial;
    }

    public static String getMacAddr()
    {
        try
        {
            List<NetworkInterface> all = Collections.list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface nif : all)
            {
                if (!nif.getName().equalsIgnoreCase("wlan0")) continue;

                byte[] macBytes = nif.getHardwareAddress();
                if (macBytes == null)
                {
                    return "";
                }

                StringBuilder res1 = new StringBuilder();
                for (byte b : macBytes)
                {
                    res1.append(String.format("%02X:", b));
                }

                if (res1.length() > 0)
                {
                    res1.deleteCharAt(res1.length() - 1);
                }
                return res1.toString();
            }
        } catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return "02:00:00:00:00:00";
    }

    public static String getBtAddressViaReflection()
    {
        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        Object bluetoothManagerService = new Mirror().on(bluetoothAdapter).get().field("mService");
        if (bluetoothManagerService == null)
        {
            LogUtil.i("couldn't find bluetoothManagerService");
            return "NO BLUETOOTH";
        }
        Object address = new Mirror().on(bluetoothManagerService).invoke().method("getAddress").withoutArgs();
        if (address != null && address instanceof String)
        {
            LogUtil.i("using reflection to get the BT MAC address: " + address);
            return (String) address;
        } else
        {
            return "UNKNOWN";
        }
    }

    public static String getBluetoothMac()
    {
        String str = "";
        String macSerial = "";
        try
        {
            Process pp = Runtime.getRuntime().exec(
                    "cat /sys/class/net/wlan0/address ");
            InputStreamReader ir = new InputStreamReader(pp.getInputStream());
            LineNumberReader input = new LineNumberReader(ir);

            for (; null != str; )
            {
                str = input.readLine();
                if (str != null)
                {
                    macSerial = str.trim();// 去空格
                    break;
                }
            }
        } catch (Exception ex)
        {
            ex.printStackTrace();
        }
        if (macSerial == null || "".equals(macSerial))
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
        return macSerial;
    }

    public static String loadFileAsString(String fileName) throws Exception
    {
        FileReader reader = new FileReader(fileName);
        String text = loadReaderAsString(reader);
        reader.close();
        return text;
    }

    public static String loadReaderAsString(Reader reader) throws Exception
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
}
