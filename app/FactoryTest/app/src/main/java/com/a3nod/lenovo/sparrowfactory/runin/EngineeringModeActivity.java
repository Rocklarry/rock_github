package com.a3nod.lenovo.sparrowfactory.runin;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.SparseArray;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.RebootActivity;
import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

public class EngineeringModeActivity extends Activity
{
    public static final String TAG = "EngineeringModeActivity";

    ListView lv_runin;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_engineering_mode);
        initTitle();
        initView();
    }

    public void initTitle()
    {
        TextView tv_title = findViewById(R.id.tv_menu_title);
        tv_title.setText(R.string.runin_engineering_mode);
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
        lv_runin = findViewById(R.id.lv_runin);
        final SparseArray<RuninItem> lri = RuninConfig.getInstance().getRuninItem();


        RuninItemAdapter ria = new RuninItemAdapter(this, lri);
        lv_runin.setAdapter(ria);
        lv_runin.setOnItemClickListener(new AdapterView.OnItemClickListener()
        {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id)
            {
                SPUtils.put(EngineeringModeActivity.this, Constant.SP_KEY_IS_AUTORUNIN, false);
                if(position!=lri.size()-1)
                {
                    Intent intent = new Intent(EngineeringModeActivity.this, ((RuninItem) lv_runin.getAdapter().getItem(position)).cls);
                    startActivity(intent);
                }else{
                    Intent intent =new Intent(EngineeringModeActivity.this,RebootActivity.class);
                    startActivity(intent);
                }

            }
        });

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
}
