package com.a3nod.lenovo.sparrowfactory.runin;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

public class RuninMainActivity extends Activity
{
    public static final String TAG = "RuninMainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_runin_main);

        initTitle();
    }

    public void initTitle()
    {
        TextView tv_title=findViewById(R.id.tv_menu_title);
        tv_title.setText(R.string.runin_title);
        ImageView iv_back=findViewById(R.id.iv_menu_back);
        iv_back.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                finish();
            }
        });
    }

    @Override
    protected void onSaveInstanceState(Bundle outState)
    {
        super.onSaveInstanceState(outState);
        LogUtil.i("RuninMainActivity onSaveInstanceState");
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState)
    {
        super.onRestoreInstanceState(savedInstanceState);
        LogUtil.i("RuninMainActivity onRestoreInstanceState");
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
                CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_OTHER);
                finish();
            }
        }
        return false;
    }

    public void autuRun(View view)
    {
        Intent intent=new Intent(RuninMainActivity.this,RuninActivity.class);
        startActivity(intent);
    }

    public void singleTest(View view)
    {
        Intent intent=new Intent(RuninMainActivity.this,EngineeringModeActivity.class);
        startActivity(intent);
    }
}
