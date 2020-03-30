package com.a3nod.lenovo.sparrowfactory;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

public class MoreActivity extends Activity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_more);
        initTitle();
    }
    public void initTitle()
    {
        TextView tv_title = findViewById(R.id.tv_menu_title);
        tv_title.setText("More");
        ImageView iv_back = findViewById(R.id.iv_menu_back);
        iv_back.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                finish();
            }
        });
    }


    public void tremoloTest(View view)
    {
        Intent intent=new Intent(this,TremoloActivity.class);
        startActivity(intent);
    }

    public void rebootTest(View view)
    {
        Intent intent=new Intent(this,RebootActivity.class);
        startActivity(intent);
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

    public void videoTest(View view)
    {
        Intent intent=new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_LAUNCHER);
        ComponentName cn = new ComponentName("com.google.assistant.videoplayer", "com.google.assistant.videoplayer.MainActivity");
        intent.setComponent(cn);
        startActivity(intent);
    }

    public void backLight(View view)
    {
        Intent intent=new Intent(this,BackLightTestActivity.class);
        startActivity(intent);
    }

    public void temperature(View view)
    {
        startActivity(new Intent(MoreActivity.this, TemperatureActivity.class));
    }
}
