package com.a3nod.lenovo.sparrowfactory.runin;

import android.content.Context;
import android.util.SparseArray;

import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.audio.AudioTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.camera.CameraTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.cpu.CPUTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.emmc.EMMCTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.lcd.LCDTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.memory.MemoryTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.reboot.RebootTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.threedimensional.ThreeDimensionalActivity;
import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;
import com.a3nod.lenovo.sparrowfactory.runin.twodimensional.TwoDimensionalActivity;
import com.a3nod.lenovo.sparrowfactory.runin.video.VideoTestActivity;

import java.util.ArrayList;
import java.util.List;

/**
 * Description
 * Created by aaa on 2017/11/14.
 */

public class RuninConfig
{
    public static final int RUNIN_CPU_ID=0;
    public static final int RUNIN_MEMORY_ID=1;
    public static final int RUNIN_EMMC_ID=2;
    public static final int RUNIN_LCD_ID=3;
    public static final int RUNIN_2D_ID=4;
    public static final int RUNIN_3D_ID=5;
    public static final int RUNIN_AUDIO_ID=6;
    public static final int RUNIN_VIDEO_ID=7;
    public static final int RUNIN_CAMERA_ID=8;
    public static final int RUNIN_REBOOT_ID=9;

    private int runin_times=0;
    private long  runin_duration =6*60*1000;

    private static SparseArray<TimeSetting> timeSetting;
    private static SparseArray<RuninItem> runinItems;
    private static RuninConfig config;
    private static Context context;
    static{
        initTimeSet();
        initRuninItem();
    }

    public static void init(Context ctx)
    {
        context = ctx.getApplicationContext();
    }

    public static RuninConfig getInstance()
    {
        if (config == null)
        {
            synchronized (RuninConfig.class)
            {
                if (config == null)
                {
                    config = new RuninConfig();
                }
            }
        }
        return config;
    }
    public void addTestRemark(String remarkKey, String remark)
    {
        String formerRemark=(String) SPUtils.get(context,remarkKey,"");
        SPUtils.put(context,remarkKey,formerRemark+remark);
    }
    public void saveTestResult(String resultKey)
    {
        int num=(int)SPUtils.get(context,resultKey,0)+1;
        SPUtils.put(context,resultKey,num);
    }
    public void clearReport()
    {
        SPUtils.remove(context, Constant.SP_KEY_CPU_TEST_SUCCESS);
        SPUtils.remove(context,Constant.SP_KEY_MEMORY_TEST_SUCCESS);
        SPUtils.remove(context,Constant.SP_KEY_EMMC_TEST_SUCCESS);
        SPUtils.remove(context,Constant.SP_KEY_LCD_TEST_SUCCESS);
        SPUtils.remove(context,Constant.SP_KEY_2D_TEST_SUCCESS);
        SPUtils.remove(context,Constant.SP_KEY_3D_TEST_SUCCESS);
        SPUtils.remove(context,Constant.SP_KEY_AUDIO_TEST_SUCCESS);
        SPUtils.remove(context,Constant.SP_KEY_VIDEO_TEST_SUCCESS);
        SPUtils.remove(context,Constant.SP_KEY_CAMERA_TEST_SUCCESS);
        SPUtils.remove(context,Constant.SP_KEY_SLEEP_TEST_SUCCESS);
        SPUtils.remove(context,Constant.SP_KEY_REBOOT_TEST_SUCCESS);

        SPUtils.remove(context, Constant.SP_KEY_CPU_TEST_FAIL);
        SPUtils.remove(context,Constant.SP_KEY_MEMORY_TEST_FAIL);
        SPUtils.remove(context,Constant.SP_KEY_EMMC_TEST_FAIL);
        SPUtils.remove(context,Constant.SP_KEY_LCD_TEST_FAIL);
        SPUtils.remove(context,Constant.SP_KEY_2D_TEST_FAIL);
        SPUtils.remove(context,Constant.SP_KEY_3D_TEST_FAIL);
        SPUtils.remove(context,Constant.SP_KEY_AUDIO_TEST_FAIL);
        SPUtils.remove(context,Constant.SP_KEY_VIDEO_TEST_FAIL);
        SPUtils.remove(context,Constant.SP_KEY_CAMERA_TEST_FAIL);
        SPUtils.remove(context,Constant.SP_KEY_SLEEP_TEST_FAIL);
        SPUtils.remove(context,Constant.SP_KEY_REBOOT_TEST_FAIL);
    }

    public List<TestItem> getReport()
    {
        List<TestItem> lti = new ArrayList<>();

        lti.add(getTestItem(R.string.runin_cpu, Constant.SP_KEY_CPU_TEST_SUCCESS,Constant.SP_KEY_CPU_TEST_FAIL));
        lti.add(getTestItem(R.string.runin_memory, Constant.SP_KEY_MEMORY_TEST_SUCCESS,Constant.SP_KEY_MEMORY_TEST_FAIL));
        lti.add(getTestItem(R.string.runin_emmc, Constant.SP_KEY_EMMC_TEST_SUCCESS,Constant.SP_KEY_EMMC_TEST_FAIL));
        lti.add(getTestItem(R.string.runin_lcd, Constant.SP_KEY_LCD_TEST_SUCCESS,Constant.SP_KEY_LCD_TEST_FAIL));
        lti.add(getTestItem(R.string.runin_two_dimensional, Constant.SP_KEY_2D_TEST_SUCCESS,Constant.SP_KEY_2D_TEST_FAIL));
        lti.add(getTestItem(R.string.runin_three_dimensional, Constant.SP_KEY_3D_TEST_SUCCESS,Constant.SP_KEY_3D_TEST_FAIL));
        lti.add(getTestItem(R.string.runin_audio, Constant.SP_KEY_AUDIO_TEST_SUCCESS,Constant.SP_KEY_AUDIO_TEST_FAIL));
        lti.add(getTestItem(R.string.runin_video, Constant.SP_KEY_VIDEO_TEST_SUCCESS,Constant.SP_KEY_VIDEO_TEST_FAIL));
      //  lti.add(getTestItem(R.string.runin_camera, Constant.SP_KEY_CAMERA_TEST_SUCCESS,Constant.SP_KEY_CAMERA_TEST_FAIL));
        lti.add(getTestItem(R.string.runin_reboot, Constant.SP_KEY_REBOOT_TEST_SUCCESS,Constant.SP_KEY_REBOOT_TEST_FAIL));
        return lti;
    }


    public SparseArray<RuninItem> getRuninItem(){
        return runinItems;
    }

    public TestItem getTestItem(int testID, String success,String fail)
    {
        TestItem ti = new TestItem();
        ti.setName(context.getString(testID));
        ti.setSuccess((int) SPUtils.get(context, success, 0));
        ti.setFail((int) SPUtils.get(context, fail, 0));
//        ti.setRemark((String) SPUtils.get(context, remark, ""));

        return ti;
    }


    private RuninConfig()
    {

        if (context != null)
        {
            runinItems.get(RUNIN_CPU_ID).itemDuration = (Integer) SPUtils.get(context, Constant.SP_KEY_CPU_TEST_DURATION, timeSetting.get(0).duration);
            runinItems.get(RUNIN_MEMORY_ID).itemDuration = (Integer) SPUtils.get(context, Constant.SP_KEY_MEMORY_TEST_DURATION, timeSetting.get(0).duration);
            runinItems.get(RUNIN_EMMC_ID).itemDuration = (Integer) SPUtils.get(context, Constant.SP_KEY_EMMC_TEST_DURATION, timeSetting.get(0).duration);

            runinItems.get(RUNIN_CAMERA_ID).itemDuration = (Integer) SPUtils.get(context, Constant.SP_KEY_CAMERA_TEST_DURATION, timeSetting.get(0).duration);
            runinItems.get(RUNIN_AUDIO_ID).itemDuration = (Integer) SPUtils.get(context, Constant.SP_KEY_AUDIO_TEST_DURATION, timeSetting.get(0).duration);
            runinItems.get(RUNIN_VIDEO_ID).itemDuration = (Integer) SPUtils.get(context, Constant.SP_KEY_VIDEO_TEST_DURATION, timeSetting.get(0).duration);

            runinItems.get(RUNIN_LCD_ID).itemDuration = (Integer) SPUtils.get(context, Constant.SP_KEY_LCD_TEST_DURATION, timeSetting.get(0).duration);
            runinItems.get(RUNIN_2D_ID).itemDuration = (Integer) SPUtils.get(context, Constant.SP_KEY_2D_TEST_DURATION, timeSetting.get(0).duration);
            runinItems.get(RUNIN_3D_ID).itemDuration = (Integer) SPUtils.get(context, Constant.SP_KEY_3D_TEST_DURATION, timeSetting.get(0).duration);

            runinItems.get(RUNIN_REBOOT_ID).itemDuration = (Integer) SPUtils.get(context, Constant.SP_KEY_REBOOT_TEST_DURATION, timeSetting.get(0).duration);

            LogUtil.i("RuinConfig before rebuild  duration: "+ runin_duration);
            runin_duration=(long) SPUtils.get(context, Constant.SP_KEY_RUNIN_DURATION, runin_duration)+(long)SPUtils.get(context,Constant.SP_KEY_RUNIN_TEST_START_TIME,0L);
            LogUtil.i("RuinConfig rebuild  duration: "+ runin_duration);
        }
    }
    public static int getTimeDuration(int position){
        return timeSetting.get(position).duration;
    }
    public static int getPositionByDuration(int duration){
        for(int i=0;i<timeSetting.size();i++){
            if(duration==timeSetting.get(i).duration){
                return i;
            }
        }
        return 0;
    }
    public static void initTimeSet(){
        timeSetting=new SparseArray<>();
        timeSetting.append(0,new TimeSetting(10*1000,"10 s"));
        timeSetting.append(1,new TimeSetting(30*1000,"30 s"));
        timeSetting.append(2,new TimeSetting(60*1000,"1 min"));
        timeSetting.append(3,new TimeSetting(3 * 60 * 1000,"3 min"));
        timeSetting.append(4,new TimeSetting(5 * 60 * 1000,"5 min"));
        timeSetting.append(5,new TimeSetting(7 * 60 * 1000,"7 mins"));
        timeSetting.append(6,new TimeSetting(10 * 60 * 1000,"10 mins"));
        timeSetting.append(7,new TimeSetting(15 * 60 * 1000,"15 mins"));
        timeSetting.append(8,new TimeSetting(20 * 60 * 1000,"20 mins"));
        timeSetting.append(9,new TimeSetting(30 * 60 * 1000,"30 mins"));
        timeSetting.append(10,new TimeSetting(60 * 60 * 1000,"1 hours"));
        timeSetting.append(11,new TimeSetting(120 * 60 * 1000,"2 hours"));
        timeSetting.append(12,new TimeSetting(180 * 60 * 1000,"3 hours"));
    }

    public static SparseArray<TimeSetting> getTimeSetting()
    {
        return timeSetting;
    }

    public static SparseArray<RuninItem> getRuninItems()
    {
        return runinItems;
    }

    public static void initRuninItem(){
        runinItems=new SparseArray<>();
        runinItems.append(RUNIN_CPU_ID,new RuninItem(RUNIN_CPU_ID,R.string.runin_cpu,timeSetting.get(0).duration, CPUTestActivity.class));
        runinItems.append(RUNIN_MEMORY_ID,new RuninItem(RUNIN_MEMORY_ID,R.string.runin_memory,timeSetting.get(0).duration, MemoryTestActivity.class));
        runinItems.append(RUNIN_EMMC_ID,new RuninItem(RUNIN_EMMC_ID,R.string.runin_emmc,timeSetting.get(0).duration, EMMCTestActivity.class));
        runinItems.append(RUNIN_LCD_ID,new RuninItem(RUNIN_LCD_ID,R.string.runin_lcd,timeSetting.get(0).duration, LCDTestActivity.class));
        runinItems.append(RUNIN_2D_ID,new RuninItem(RUNIN_2D_ID,R.string.runin_two_dimensional,timeSetting.get(0).duration, TwoDimensionalActivity.class));
        runinItems.append(RUNIN_3D_ID,new RuninItem(RUNIN_3D_ID,R.string.runin_three_dimensional,timeSetting.get(0).duration, ThreeDimensionalActivity.class));
        runinItems.append(RUNIN_AUDIO_ID,new RuninItem(RUNIN_AUDIO_ID,R.string.runin_audio,timeSetting.get(0).duration, AudioTestActivity.class));
        runinItems.append(RUNIN_VIDEO_ID,new RuninItem(RUNIN_VIDEO_ID,R.string.runin_video,timeSetting.get(0).duration, VideoTestActivity.class));
        runinItems.append(RUNIN_CAMERA_ID,new RuninItem(RUNIN_CAMERA_ID,R.string.runin_camera,timeSetting.get(0).duration, CameraTestActivity.class));
        runinItems.append(RUNIN_REBOOT_ID,new RuninItem(RUNIN_REBOOT_ID,R.string.runin_reboot,timeSetting.get(0).duration, RebootTestActivity.class));
    }

    static class TimeSetting{
        public TimeSetting(int d,String txt){
            duration=d;
            durationText=txt;
        }
        int duration;
        String durationText;
    }
    public void setItemDuration(int itemId,int position)
    {
        if(RUNIN_REBOOT_ID==itemId){
            runinItems.get(itemId).itemDuration=position;
        }else{
            runinItems.get(itemId).itemDuration=timeSetting.get(position).duration;
        }
    }
    public int getItemDuration(int itemId)
    {
        return runinItems.get(itemId).itemDuration;
    }

    public void saveConfig()
    {
        if (context != null)
        {
            SPUtils.put(context, Constant.SP_KEY_CPU_TEST_DURATION,  runinItems.get(RUNIN_CPU_ID).itemDuration);
            SPUtils.put(context, Constant.SP_KEY_MEMORY_TEST_DURATION,  runinItems.get(RUNIN_CPU_ID).itemDuration);
            SPUtils.put(context, Constant.SP_KEY_EMMC_TEST_DURATION,  runinItems.get(RUNIN_CPU_ID).itemDuration);

            SPUtils.put(context, Constant.SP_KEY_CAMERA_TEST_DURATION,  runinItems.get(RUNIN_CPU_ID).itemDuration);
            SPUtils.put(context, Constant.SP_KEY_AUDIO_TEST_DURATION,  runinItems.get(RUNIN_CPU_ID).itemDuration);
            SPUtils.put(context, Constant.SP_KEY_VIDEO_TEST_DURATION,  runinItems.get(RUNIN_CPU_ID).itemDuration);

            SPUtils.put(context, Constant.SP_KEY_LCD_TEST_DURATION,  runinItems.get(RUNIN_CPU_ID).itemDuration);
            SPUtils.put(context, Constant.SP_KEY_2D_TEST_DURATION,  runinItems.get(RUNIN_CPU_ID).itemDuration);
            SPUtils.put(context, Constant.SP_KEY_3D_TEST_DURATION,  runinItems.get(RUNIN_CPU_ID).itemDuration);

            SPUtils.put(context, Constant.SP_KEY_REBOOT_TEST_DURATION,  runinItems.get(RUNIN_CPU_ID).itemDuration);
        }
    }

    public void setTestStatus(int itemId)
    {
        SPUtils.put(context, Constant.SP_KEY_RUNIN_CURRENT_TEST_ID, itemId);
    }
    public int getTestStatus()
    {
        return (int)SPUtils.get(context, Constant.SP_KEY_RUNIN_CURRENT_TEST_ID, 0);
    }

    public int getRunin_times()
    {
        return runin_times;
    }

    public void setRunin_times(int runin_times)
    {
        this.runin_times = runin_times;
    }

    public long getRunin_duration()
    {
        return runin_duration;
    }

    public void setRunin_duration(long runin_duration)
    {
        this.runin_duration = runin_duration;
    }
}
