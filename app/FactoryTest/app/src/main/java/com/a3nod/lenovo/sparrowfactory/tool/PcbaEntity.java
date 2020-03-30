package com.a3nod.lenovo.sparrowfactory.tool;


import android.os.Environment;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.mmi.datacheck.DataEntity;
import com.google.gson.Gson;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Created by Lenovo on 2017/11/27.
 */

public class PcbaEntity
{
    private static class Holder
    {
        private static PcbaEntity entity = new PcbaEntity();
    }

    private PcbaEntity()
    {
    }

    public static PcbaEntity getInstants()
    {
        return Holder.entity;
    }

    private String SN;
    private String mLcdReuslt;
    private String mTPReuslt;
    private String mCameraReuslt;
    private String mAudioReuslt;
    private String mGSensorReuslt;
    private String mPSensorReuslt;
    private String mLSensorReuslt;
    private String mButtonReuslt;
    private String mSpeakReuslt;
    private String mBTReuslt;
    private String mWifiReuslt;
    private DataEntity mSysInfoReuslt;

    public void cleanEntity()
    {
        SN = "";
        mLcdReuslt = "";
        mTPReuslt = "";
        mCameraReuslt = "";
        mAudioReuslt = "";
        mGSensorReuslt = "";
        mPSensorReuslt = "";
        mLSensorReuslt = "";
        mButtonReuslt = "";
        mSpeakReuslt = "";
        mBTReuslt = "";
        mWifiReuslt = "";
        mSysInfoReuslt = null;
        deleteReport();
    }
    public static void deleteReport()
    {
        File file=new File(Constant.PCBA_REPORT_PATH);
        if(file.exists()){
            file.delete();
        }
        try
        {
            file.createNewFile();
        } catch (IOException e)
        {
            e.printStackTrace();
        }
    }

    public static void saveResult()
    {
        Gson gson = new Gson();
        String mContent = gson.toJson(PcbaEntity.getInstants());
        LogUtil.i("gson : " + mContent);
        File file = new File(Constant.PCBA_REPORT_PATH);
        FileOutputStream output = null;
        try
        {
            output = new FileOutputStream(file);
            output.write(mContent.getBytes());
            output.close();
        } catch (FileNotFoundException e)
        {
            e.printStackTrace();
        } catch (IOException e)
        {
            e.printStackTrace();
        }finally
        {
            try
            {
                output.close();
            } catch (Exception e)
            {
                e.printStackTrace();
            }
        }

    }

    static String getSDPath()
    {
        File sdDir = null;
        boolean sdCardExist = Environment.getExternalStorageState()
                .equals(Environment.MEDIA_MOUNTED);//判断sd卡是否存在
        if (sdCardExist)
        {
            sdDir = Environment.getExternalStorageDirectory();//获取跟目录
        }
        return sdDir.toString();
    }

    public void setResult(int item, boolean isSuccess)
    {
        String result = isSuccess ? "pass" : "fail";
        switch (item)
        {
            case Protocol.PCBA_TEST_ITEM_LCD:
                mLcdReuslt = result;
                break;
            case Protocol.PCBA_TEST_ITEM_TP:
                mTPReuslt = result;
                break;
            case Protocol.PCBA_TEST_ITEM_CAMERA:
                mCameraReuslt = result;
                break;
            case Protocol.PCBA_TEST_ITEM_MIC:
                mAudioReuslt = result;
                break;
            case Protocol.PCBA_TEST_ITEM_SPEAKER:
                mSpeakReuslt = result;
                break;
            case Protocol.PCBA_TEST_ITEM_GSENSOR:
                mGSensorReuslt = result;
                break;
            case Protocol.PCBA_TEST_ITEM_PSENSOR:
                mPSensorReuslt = result;
                break;
            case Protocol.PCBA_TEST_ITEM_LSENSOR:
                mLSensorReuslt = result;
                break;
            case Protocol.PCBA_TEST_ITEM_KEY:
                mButtonReuslt = result;
                break;
            case Protocol.PCBA_TEST_ITEM_BLUETOOTH:
                mBTReuslt = result;
                break;
            case Protocol.PCBA_TEST_ITEM_WIFI:
                mWifiReuslt = result;
                break;
        }
        if(!isSuccess){
            saveResult();
        }
    }

    public void setDataCheckResult(DataEntity entity)
    {
        mSysInfoReuslt = entity;
    }
    public DataEntity getDataCheckResult()
    {
        return mSysInfoReuslt;
    }
}


//adb内通过adb shell am broadcast -a "xxxxxxx"给APP内发送自定义命令，xxxxx为以下定义的命令：
//1.测试LCD："lcd_test"
//2.测试TP："tp_test"
//3.测试Camera："camera_test"
//4.测试Audio："audio_test"
//5.测试GSensor："gs_test"
//6.测试PSensor："ps_test"
//7.测试LSensor："ls_test"
//8.测试Button："bn_test"
//9.测试DataCheck："data_test"

//测试完成后测试结果会以json形式保存为文件，格式如上，通过以下命令将文件导入到电脑：
//adb pull /sdcard/pcba.test "xxxxx"
//xxxx为电脑存放文件的路径，注意以上命令不要在adb shell 里面执行.
//测试结果字段只显示"pass"or "fail"，没有显示的字段则表示没有进行测试.