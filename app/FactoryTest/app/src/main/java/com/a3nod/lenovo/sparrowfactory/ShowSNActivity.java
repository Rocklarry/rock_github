package com.a3nod.lenovo.sparrowfactory;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.tool.SystemInfoTools;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

/**
 * Description
 * Created by aaa on 2018/1/11.
 */

public class ShowSNActivity extends Activity
{
    TextView tv_title;
    TextView tv_sn;
    String sn;
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_show_mmi_sn);
        initTitle();
        initView();

        Intent intent = getIntent();
        String action=intent.getStringExtra(Constant.INTENT_KEY_ACTION_SHOW_SN);
        if (Constant.ACTION_WRITE_MMI_SN.equals(action))
        {
            tv_title.setText(R.string.mmi_sn);
            sn= SystemInfoTools.readSNFile(Constant.PATH_MMI_SN);
            tv_sn.setText(sn);
        } else if (Constant.ACTION_WRITE_SN.equals(action))
        {
            tv_title.setText(R.string.sn);
            sn= SystemInfoTools.readSNFile(Constant.PATH_SN);
            tv_sn.setText(sn);
        }
    }

    public void initTitle()
    {
        tv_title = findViewById(R.id.tv_menu_title);
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

    public void initView()
    {
        tv_sn=findViewById(R.id.tv_show_sn);
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
                SPUtils.put(this, Constant.SP_KEY_IS_AUTORUNIN, false);
                finish();
            }
        }
        return false;
    }
}
