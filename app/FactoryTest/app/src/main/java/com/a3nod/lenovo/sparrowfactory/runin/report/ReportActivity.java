package com.a3nod.lenovo.sparrowfactory.runin.report;

import android.app.Activity;
import android.os.Build;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.RuninConfig;
import com.a3nod.lenovo.sparrowfactory.runin.TestItem;
import com.a3nod.lenovo.sparrowfactory.tool.FileUtils;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;

public class ReportActivity extends Activity
{

    ReportAdapter reportAdapter;
    ListView lv_report;
    List<TestItem> lti;
    TextView tv_actual_duration;
    TextView tv_setting_duration;
    TextView tv_runin_sn;
    long startTime;
    long endTime;
    long runintime;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_runin_report);
        startTime = (Long) SPUtils.get(this, Constant.SP_KEY_RUNIN_TEST_START_TIME, 0L);
        endTime = (Long) SPUtils.get(this, Constant.SP_KEY_RUNIN_TEST_END_TIME, 0L);
        runintime = (Long) SPUtils.get(this, Constant.SP_KEY_RUNIN_DURATION, 0L);
        initView();
    }

    private void initView()
    {
        tv_runin_sn=findViewById(R.id.tv_runin_sn);
        tv_actual_duration = findViewById(R.id.tv_runin_actual_duration);
        tv_setting_duration = findViewById(R.id.tv_runin_setting_duration);

        tv_runin_sn.setText("serial number: "+Build.SERIAL);
        tv_actual_duration.setText("runin time : " + format(endTime - startTime));
        tv_setting_duration.setText("setting time : " + format(runintime));

        RuninConfig config = RuninConfig.getInstance();
        lti = config.getReport();
        lv_report = findViewById(R.id.lv_report);
        reportAdapter = new ReportAdapter(this, lti);
        lv_report.setAdapter(reportAdapter);
    }

    public static String format(long time)
    {
        long hour = time / (60 * 60 * 1000);
        long minute = (time - hour * 60 * 60 * 1000) / (60 * 1000);
        long second = (time - hour * 60 * 60 * 1000 - minute * 60 * 1000) / 1000;
        return (hour == 0 ? "00" : (hour >= 10 ? hour : ("0" + hour))) + ":" + (minute == 0 ? "00" : (minute >= 10 ? minute : ("0" + minute))) + ":" + (second == 0 ? "00" : (second >= 10 ? second : ("0" + second)));

    }



    private long lastMillis = 0;

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
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
