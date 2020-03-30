package com.a3nod.lenovo.sparrowfactory.runin;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.report.ReportActivity;
import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

import java.util.ArrayList;
import java.util.List;

public class RuninActivity extends Activity
{
    List<String> duration;
    GridView gv_item;
    Spinner sp_duration;
    RuninConfig config;
    List<RuninTestItem> lti;
    long[] durations = {
            3 * 60 * 1000,
            60 * 60 * 1000,
            2 * 60 * 60 * 1000,
            3 * 60 * 60 * 1000,
            4 * 60 * 60 * 1000,
            5 * 60 * 60 * 1000,
            6 * 60 * 60 * 1000,
            12 * 60 * 60 * 1000,
            24 * 60 * 60 * 1000,
            36 * 60 * 60 * 1000,
            48 * 60 * 60 * 1000,
            72 * 60 * 60 * 1000,
    };

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        LogUtil.i("RuninActivity onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_runin);

        config = RuninConfig.getInstance();

        initTitle();

        initTestTime();
        initGridView();
/*        new Thread(){
            @Override
            public void run()
            {
                super.run();

                try
                {
                    Thread.sleep(3000);
                } catch (InterruptedException e)
                {
                    e.printStackTrace();
                }
                SPUtils.put(RuninActivity.this, Constant.SP_KEY_IS_AUTORUNIN, true);
                try
                {
                    Thread.sleep(2000);
                } catch (InterruptedException e)
                {
                    e.printStackTrace();
                }
                throw new RuntimeException("测试崩溃重启");
            }
        }.start();*/
    }

    @Override
    protected void onSaveInstanceState(Bundle outState)
    {
        super.onSaveInstanceState(outState);
        LogUtil.i("RuninActivity onSaveInstanceState");
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState)
    {
        super.onRestoreInstanceState(savedInstanceState);
        LogUtil.i("RuninActivity onRestoreInstanceState");
    }

    public void initTitle()
    {
        TextView tv_title = findViewById(R.id.tv_menu_title);
        tv_title.setText(R.string.runin_factory_mode);
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

    public void initGridView()
    {
        gv_item = findViewById(R.id.gv_test_item);
        lti = new ArrayList<>();
        lti.add(new RuninTestItem(getString(R.string.runin_cpu), true));
        lti.add(new RuninTestItem(getString(R.string.runin_memory), true));
        lti.add(new RuninTestItem(getString(R.string.runin_emmc), true));
        lti.add(new RuninTestItem(getString(R.string.runin_lcd), true));
        lti.add(new RuninTestItem(getString(R.string.runin_two_dimensional), true));
        lti.add(new RuninTestItem(getString(R.string.runin_three_dimensional), true));
        lti.add(new RuninTestItem(getString(R.string.runin_audio), true));
        lti.add(new RuninTestItem(getString(R.string.runin_video), true));
        //lti.add(new RuninTestItem(getString(R.string.runin_camera), true));
        lti.add(new RuninTestItem(getString(R.string.runin_reboot), true));
        final RuninTestItemAdapter rtia = new RuninTestItemAdapter(this, lti);
        gv_item.setAdapter(rtia);
        gv_item.setOnItemClickListener(new AdapterView.OnItemClickListener()
        {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id)
            {
                lti.get(position).isChoose = !lti.get(position).isChoose;
                rtia.notifyDataSetChanged();
            }
        });
    }

    class RuninTestItem
    {
        public RuninTestItem(String name, boolean isChoose)
        {
            this.name = name;
            this.isChoose = isChoose;
        }

        String name;
        boolean isChoose;
    }

    public void initTestTime()
    {
        duration = new ArrayList<>();
        duration.add("3 mins");
        duration.add("1 hour");
        duration.add("2 hour");
        duration.add("3 hour");
        duration.add("4 hour");
        duration.add("5 hour");
        duration.add("6 hour");
        duration.add("12 hour");
        duration.add("24 hour");
        duration.add("36 hour");
        duration.add("48 hour");
        duration.add("72 hour");


        sp_duration = findViewById(R.id.sp_duration);
        ArrayAdapter aa=new ArrayAdapter<>(this, R.layout.layout_spinner, duration);
        aa.setDropDownViewResource(R.layout.layout_spinner_dropdown);
        sp_duration.setAdapter(aa);
        sp_duration.setSelection(0);
    }


    @Override
    protected void onResume()
    {
        super.onResume();
    }


    public void startRunin(View view)
    {
        LogUtil.i("runin service start manual");
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < lti.size(); i++)
        {
            if (lti.get(i).isChoose)
            {
                //change the camera item to reboot item,the cameta is cancel
                if(i==8){
                    i=9;
                }
                sb.append(i + ",");
            }
        }
        if(TextUtils.isEmpty(sb.toString())){
            return ;
        }

        Intent intent = new Intent(RuninActivity.this, RuninService.class);
        intent.putExtra(Constant.INTENT_KEY_START_AUTO_RUNIN, true);
        intent.putExtra(Constant.INTENT_KEY_RUNIN_DURATION,durations[sp_duration.getSelectedItemPosition()]);
        intent.putExtra(Constant.INTENT_KEY_RUNIN_TEST_ITEM,sb.toString());
        startService(intent);

    }





    public void viewReport(View view)
    {
        Intent intent = new Intent(this, ReportActivity.class);
        startActivity(intent);
    }


    class RuninTestItemAdapter extends BaseAdapter
    {
        List<RuninTestItem> lti;
        Context context;

        public RuninTestItemAdapter(Context context, List<RuninTestItem> lti)
        {
            this.context = context;
            this.lti = lti;
        }

        @Override
        public int getCount()
        {
            return lti.size();
        }

        @Override
        public Object getItem(int position)
        {
            return lti.get(position);
        }

        @Override
        public long getItemId(int position)
        {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent)
        {
            ViewHolder holder;
            if (convertView == null)
            {
                holder = new ViewHolder();
                convertView = LayoutInflater.from(context).inflate(R.layout.gi_runin_test_item, parent,false);
                holder.textView = convertView.findViewById(R.id.gi_tv_test_item);
                convertView.setTag(holder);
            } else
            {
                holder = (ViewHolder) convertView.getTag();
            }
            holder.textView.setText(lti.get(position).name);
            if (lti.get(position).isChoose)
            {
                holder.textView.setTextColor(Color.WHITE);
                holder.textView.setBackgroundColor(Color.GREEN);
            } else
            {
                holder.textView.setTextColor(Color.DKGRAY);
                holder.textView.setBackgroundColor(Color.LTGRAY);
            }
            return convertView;
        }

        class ViewHolder
        {
            TextView textView;
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
