package com.a3nod.lenovo.sparrowfactory.mmi.result;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

/**
 * Created by Lenovo on 2017/11/14.
 */

public class ResultActivity extends Activity
{
    public static final String TAG = "MMI_RESULT_TEST";
    private LinearLayout mainLinerLayout, rowLayout;
    private String[] titles = {"test_item", "success/fail", "times"};
    String[] data = {/*"Camera",*/ "Backlight", "Buttons", "Speaker", "Mic",/* "G-sensor",*/ "P-sensor", "L-sensor","RGB-sensor", "BT", "Wifi", "LCD", "TP", "Mult-point", "Data check"};
    private String[][] mResult = {/*{ASSYEntity.getInstants().CameraResult, ASSYEntity.getInstants().CameraTestTimes + ""},*/ {ASSYEntity.getInstants().BacklightResult, ASSYEntity.getInstants().BacklightTestTimes + ""},
            {ASSYEntity.getInstants().ButtonsResult, ASSYEntity.getInstants().ButtonsTestTimes + ""}, {ASSYEntity.getInstants().SpeakerResult, ASSYEntity.getInstants().SpeakerTestTimes + ""},
            {ASSYEntity.getInstants().MicResult, ASSYEntity.getInstants().MicTestTimes + ""}, /*{ASSYEntity.getInstants().GsensorResult, ASSYEntity.getInstants().GsensorTestTimes + ""},*/
            {ASSYEntity.getInstants().PsensorResult, ASSYEntity.getInstants().PsensorTestTimes + ""}, {ASSYEntity.getInstants().LsensorResult, ASSYEntity.getInstants().LsensorTestTimes + ""},
            {ASSYEntity.getInstants().RGBsensorResult, ASSYEntity.getInstants().RGBsensorTestTimes + ""},
            {ASSYEntity.getInstants().BTResult, ASSYEntity.getInstants().BTTestTimes + ""}, {ASSYEntity.getInstants().WifiResult, ASSYEntity.getInstants().WifiTestTimes + ""},
            {ASSYEntity.getInstants().LCDResult, ASSYEntity.getInstants().LCDTestTimes + ""}, {ASSYEntity.getInstants().TPResult, ASSYEntity.getInstants().TPTestTimes + ""},
            {ASSYEntity.getInstants().PointResult, ASSYEntity.getInstants().PointTestTimes + ""}};

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG,CrashHandler.TYPE_MMI);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_logcat);
        mainLinerLayout = findViewById(R.id.MyTable);
        ASSYEntity.getResultFromFile();
        mResult = new String[][]{/*{ASSYEntity.getInstants().CameraResult, ASSYEntity.getInstants().CameraTestTimes + ""},*/ {ASSYEntity.getInstants().BacklightResult, ASSYEntity.getInstants().BacklightTestTimes + ""},
                {ASSYEntity.getInstants().ButtonsResult, ASSYEntity.getInstants().ButtonsTestTimes + ""}, {ASSYEntity.getInstants().SpeakerResult, ASSYEntity.getInstants().SpeakerTestTimes + ""},
                {ASSYEntity.getInstants().MicResult, ASSYEntity.getInstants().MicTestTimes + ""}, /*{ASSYEntity.getInstants().GsensorResult, ASSYEntity.getInstants().GsensorTestTimes + ""},*/
                {ASSYEntity.getInstants().PsensorResult, ASSYEntity.getInstants().PsensorTestTimes + ""}, {ASSYEntity.getInstants().LsensorResult, ASSYEntity.getInstants().LsensorTestTimes + ""},
                {ASSYEntity.getInstants().RGBsensorResult, ASSYEntity.getInstants().RGBsensorTestTimes + ""},
                {ASSYEntity.getInstants().BTResult, ASSYEntity.getInstants().BTTestTimes + ""}, {ASSYEntity.getInstants().WifiResult, ASSYEntity.getInstants().WifiTestTimes + ""},
                {ASSYEntity.getInstants().LCDResult, ASSYEntity.getInstants().LCDTestTimes + ""}, {ASSYEntity.getInstants().TPResult, ASSYEntity.getInstants().TPTestTimes + ""},
                {ASSYEntity.getInstants().PointResult, ASSYEntity.getInstants().PointTestTimes + ""}};
        initData();
        setData();

        ASSYEntity.getInstants().save();
    }

    private void initData()
    {
        rowLayout = (LinearLayout) LayoutInflater.from(this).inflate(R.layout.table, null);
        TableTextView title = rowLayout.findViewById(R.id.test_item);
        title.setText(titles[0]);
        title.setTextSize(20);
        title.setTextColor(Color.BLACK);
        TableTextView result = rowLayout.findViewById(R.id.tv_result);
        result.setText(titles[1]);
        result.setTextSize(20);
        result.setTextColor(Color.BLACK);
        TableTextView times = rowLayout.findViewById(R.id.tv_num);
        times.setText(titles[2]);
        times.setTextSize(20);
        times.setTextColor(Color.BLACK);
        mainLinerLayout.addView(rowLayout);
    }

    private void initData(int i)
    {
        rowLayout = (LinearLayout) LayoutInflater.from(this).inflate(R.layout.table, null);
        TableTextView title = rowLayout.findViewById(R.id.test_item);
        title.setText(data[i]);
        title.setTextSize(14);
        title.setTextColor(Color.BLUE);
        TableTextView result = rowLayout.findViewById(R.id.tv_result);
        result.setText(mResult[i][0]);
        result.setTextSize(14);
        result.setTextColor(Color.BLUE);
        TableTextView times = rowLayout.findViewById(R.id.tv_num);
        times.setText(mResult[i][1]);
        times.setTextColor(Color.BLUE);
        times.setTextSize(14);
        mainLinerLayout.addView(rowLayout);
    }

    private void setData()
    {
        for (int i = 0; i < data.length - 1; i++)
        {
            //循环写入测试结果
            initData(i);
        }
    }

    private long lastMillis = 0;

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {

        LogUtil.i("touch");
        if (event.getAction() == KeyEvent.ACTION_DOWN)
        {
            long currentMillis = System.currentTimeMillis();
            if (currentMillis - lastMillis > 2000)
            {
                lastMillis = currentMillis;
                Toast.makeText(this, getString(R.string.double_click_exit), Toast.LENGTH_SHORT).show();
            } else
            {
                finish();
            }
        }
        return false;
    }
}


