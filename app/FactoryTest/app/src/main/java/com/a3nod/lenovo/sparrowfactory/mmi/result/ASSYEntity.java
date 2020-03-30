package com.a3nod.lenovo.sparrowfactory.mmi.result;


import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.google.gson.Gson;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.Serializable;

/**
 * Created by Lenovo on 2017/11/14.
 */

public class ASSYEntity implements Serializable
{
    private static class Holder
    {
        private static ASSYEntity entity = new ASSYEntity();
    }

    private ASSYEntity()
    {
    }

    public static ASSYEntity getInstants()
    {
        return Holder.entity;
    }

    public String CameraResult = "null";
    public int CameraTestTimes;
    public String BacklightResult = "null";
    public int BacklightTestTimes;
    public String ButtonsResult = "null";
    public int ButtonsTestTimes;
    public String SpeakerResult = "null";
    public int SpeakerTestTimes;
    public String MicResult = "null";
    public int MicTestTimes;
    public String GsensorResult = "null";
    public int GsensorTestTimes;
    public String PsensorResult = "null";
    public int PsensorTestTimes;
    public String LsensorResult = "null";
    public int LsensorTestTimes;
    public String RGBsensorResult = "null";
    public int RGBsensorTestTimes;

    public String BTResult = "null";
    public int BTTestTimes;
    public String WifiResult = "null";
    public int WifiTestTimes;
    public String LCDResult = "null";
    public int LCDTestTimes;
    public String TPResult = "null";
    public int TPTestTimes;
    public String PointResult = "null";
    public int PointTestTimes;
    public float PSensorValue;
    public double LSensorValue;
    public float RGBSensorValue;

    public void setCameraTestResult(boolean isSuccess)
    {
        CameraTestTimes++;
        CameraResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setBacklightTestResult(boolean isSuccess)
    {
        BacklightTestTimes++;
        BacklightResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setButtonsTestResult(boolean isSuccess)
    {
        ButtonsTestTimes++;
        ButtonsResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setSpeakerTestResult(boolean isSuccess)
    {
        SpeakerTestTimes++;
        SpeakerResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setMicTestResult(boolean isSuccess)
    {
        MicTestTimes++;
        MicResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setGsensorTestResult(boolean isSuccess)
    {
        GsensorTestTimes++;
        GsensorResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setPsensorTestResult(boolean isSuccess)
    {
        PsensorTestTimes++;
        PsensorResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setLsensorTestResult(boolean isSuccess)
    {
        LsensorTestTimes++;
        LsensorResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setRGBsensorTestResult(boolean isSuccess)
    {
        RGBsensorTestTimes++;
        RGBsensorResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setBTTestResult(boolean isSuccess)
    {
        this.BTTestTimes++;
        BTResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setWifiTestResult(boolean isSuccess)
    {
        WifiTestTimes++;
        WifiResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setLCDTestResult(boolean isSuccess)
    {
        this.LCDTestTimes++;
        LCDResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setTPTestResult(boolean isSuccess)
    {
        this.TPTestTimes++;
        TPResult = isSuccess ? "success" : "fail";
        save();
    }

    public void setPointTestResult(boolean isSuccess)
    {
        PointTestTimes++;
        PointResult = isSuccess ? "success" : "fail";
        save();
    }

    public void save()
    {
        Gson gson = new Gson();
        String mContent = gson.toJson(ASSYEntity.getInstants());
        LogUtil.i("gson : " + mContent);
        File file = new File(Constant.MMI_REPORT_PATH);
        if (file.exists())
        {
            file.delete();
        }
        try
        {
            FileOutputStream output = new FileOutputStream(file);
            output.write(mContent.getBytes());  //将String字符串以字节流的形式写入到输出流中
            output.close();         //关闭输出流
        } catch (IOException e)
        {
            e.printStackTrace();
        }
    }

    public void reset()
    {
        CameraResult = "null";
        CameraTestTimes = 0;
        BacklightResult = "null";
        BacklightTestTimes = 0;
        ButtonsResult = "null";
        ButtonsTestTimes = 0;
        SpeakerResult = "null";
        SpeakerTestTimes = 0;
        MicResult = "null";
        MicTestTimes = 0;
        GsensorResult = "null";
        GsensorTestTimes = 0;
        PsensorResult = "null";
        PsensorTestTimes = 0;
        LsensorResult = "null";
        LsensorTestTimes = 0;
        RGBsensorResult = "null";
        RGBsensorTestTimes = 0;
        BTResult = "null";
        BTTestTimes = 0;
        WifiResult = "null";
        WifiTestTimes = 0;
        LCDResult = "null";
        LCDTestTimes = 0;
        TPResult = "null";
        TPTestTimes = 0;
        PointResult = "null";
        PointTestTimes = 0;
        PSensorValue = 0;
        LSensorValue = 0;
        RGBSensorValue = 0;
    }

    public static void getResultFromFile()
    {

        File file = new File(Constant.MMI_REPORT_PATH);
        byte[] resultBytes = new byte[1024];
        try
        {
            FileInputStream fis = new FileInputStream(file);
            int i = fis.read(resultBytes);  //将字符串以字节流的形式写入到输出流中
            Gson gson = new Gson();
            String resultStr = new String(resultBytes, 0, i);
            LogUtil.d("read mmi result" + resultStr);
            ASSYEntity tmp = gson.fromJson(resultStr, ASSYEntity.class);
            if (tmp != null)
            {
                Holder.entity = tmp;
            }
            fis.close();         //关闭输出流
        } catch (IOException e)
        {
            e.printStackTrace();
        }
    }

    //    public static String[][] mResult={{ASSYEntity.getInstants().CameraResult,ASSYEntity.getInstants().CameraTestTimes+""},{ASSYEntity.getInstants().BacklightResult,ASSYEntity.getInstants().BacklightTestTimes+""},
    //            {ASSYEntity.getInstants().ButtonsResult,ASSYEntity.getInstants().ButtonsTestTimes+""},{ASSYEntity.getInstants().SpeakerResult,ASSYEntity.getInstants().SpeakerTestTimes+""},
    //            {ASSYEntity.getInstants().MicResult,ASSYEntity.getInstants().MicTestTimes+""},{ASSYEntity.getInstants().GsensorResult,ASSYEntity.getInstants().GsensorTestTimes+""},
    //            {ASSYEntity.getInstants().PsensorResult,ASSYEntity.getInstants().PsensorTestTimes+""},{ASSYEntity.getInstants().LsensorResult,ASSYEntity.getInstants().LsensorTestTimes+""},
    //            {ASSYEntity.getInstants().BTResult,ASSYEntity.getInstants().BTTestTimes+""},{ASSYEntity.getInstants().WifiResult,ASSYEntity.getInstants().WifiTestTimes+""},
    //            {ASSYEntity.getInstants().LCDResult,ASSYEntity.getInstants().LCDTestTimes+""},{ASSYEntity.getInstants().TPResult,ASSYEntity.getInstants().TPTestTimes+""},
    //            {ASSYEntity.getInstants().PointResult,ASSYEntity.getInstants().PointTestTimes+""}};
}
