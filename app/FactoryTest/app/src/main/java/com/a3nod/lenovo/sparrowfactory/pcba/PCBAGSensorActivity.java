package com.a3nod.lenovo.sparrowfactory.pcba;

import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.google.android.things.pio.I2cDevice;
import com.google.android.things.pio.PeripheralManager;

import java.io.IOException;

public class PCBAGSensorActivity extends PCBABaseActivity
{
    public static final int GI2C_ADDRESS = 0x18;
    public static final byte REG_BGW_CHIPID = 0x00;
    public static final byte GCHIPID = (byte) 0xFA;

    TextView tv_sensor_name, tv_sensor_status;

    private final static String sensor_text = "G-sensor In detection....";
    public static final String TAG = "pcba_gsensor_test";


    private PCBAHandler handler;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_PCBA);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pcba_sensor);
        tv_sensor_name = findViewById(R.id.tv_pcba_sensor);
        tv_sensor_status = findViewById(R.id.tv_sensor_status);
        tv_sensor_name.setText(sensor_text);
        handler=new PCBAHandler(this);

        initConfirmButton();
        getSensorStatus();
    }

    public void getSensorStatus()
    {
        new GSensorVerifyTask().execute();
    }

    class GSensorVerifyTask extends AsyncTask<Object, Object, Boolean>
    {
        @Override
        protected Boolean doInBackground(Object... objects)
        {
            PeripheralManager pioService = PeripheralManager.getInstance();
            try
            {
                I2cDevice device = pioService.openI2cDevice("I2C2", GI2C_ADDRESS);
                byte chipid = device.readRegByte(REG_BGW_CHIPID);
                device.close();
                return (chipid == GCHIPID);

            } catch (IOException e)
            {
                e.printStackTrace();
            }
            return false;
        }

        protected void onPostExecute(Boolean isPass)
        {
            super.onPostExecute(isPass);
            if (isPass)
            {
                tv_sensor_status.setText("pass");
                handler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_SUCCESS, PCBAHandler.DELAY);
            } else
            {
                tv_sensor_status.setText("fail");
                handler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_FAIL, PCBAHandler.DELAY);
            }
        }
    }
    @Override
    protected void onPause()
    {
        super.onPause();
        handler.removeCallbacksAndMessages(null);
    }

    public void initConfirmButton()
    {
        Button tv_pass = findViewById(R.id.btn_result_pass);
        tv_pass.setEnabled(false);
        tv_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_GSENSOR, true, "007");
                finish();
            }
        });
        Button tv_reset = findViewById(R.id.btn_result_reset);
        tv_reset.setVisibility(View.GONE);
        Button tv_fail = findViewById(R.id.btn_result_fail);
        tv_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_GSENSOR, false, "007");
                finish();
            }
        });

    }
}
