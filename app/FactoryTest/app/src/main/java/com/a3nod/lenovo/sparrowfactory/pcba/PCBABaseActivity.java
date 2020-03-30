package com.a3nod.lenovo.sparrowfactory.pcba;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.google.gson.JsonObject;

import org.json.JSONException;
import org.json.JSONObject;

public class PCBABaseActivity extends Activity
{
    int itemId;
    LocalBroadcastManager lbm ;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        lbm =LocalBroadcastManager.getInstance(this);
    }


    public void initConfirmButton()
    {
        Button btn_pass = findViewById(R.id.btn_result_pass);
        btn_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                onTestSuccess();
            }
        });
        Button btn_reset = findViewById(R.id.btn_result_reset);
        btn_reset.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                onResetTest();
            }
        });
        Button btn_fail = findViewById(R.id.btn_result_fail);
        btn_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                onTestFail();
            }
        });
    }

    public void onTestSuccess()
    {
    }

    public void onTestFail()
    {

    }

    public void onResetTest()
    {

    }

}
