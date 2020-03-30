package com.a3nod.lenovo.sparrowfactory.pcba;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.google.android.things.pio.Gpio;
import com.google.android.things.pio.GpioCallback;
import com.google.android.things.pio.PeripheralManager;

import java.io.IOException;


/**
 * Created by jianzhou.peng on 2017/11/23.
 */

public class PcbaButtonActivity extends PCBABaseActivity
{
    private Button bt_volum_add, bt_volum_down, mic_mute, camera_mute;
    public static final String TAG = "pcba_button_test";
    private PeripheralManager service;
//    private Gpio gpio85;
    private Gpio gpio86;
    private Gpio gpio87;
//    private Gpio gpio1019;
    PCBAHandler handler;
    VolumeChangeReceiver receiver;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_PCBA);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.pcba_button_activity);
        receiver=new VolumeChangeReceiver();
        lbm= LocalBroadcastManager.getInstance(this);
        handler = new PCBAHandler(this);
        initView();
        initConfirmButton();
        registerGpioListener();
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        IntentFilter filter=new IntentFilter();
        filter.addAction(Constant.ACTION_VOLUME_UP);
        filter.addAction(Constant.ACTION_VOLUME_DOWN);
        lbm.registerReceiver(receiver,filter);
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        lbm.unregisterReceiver(receiver);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        unregisterGpioListener();
    }

    private void initView()
    {
        bt_volum_add = findViewById(R.id.volum_add);
        bt_volum_down = findViewById(R.id.volum_down);
        mic_mute = findViewById(R.id.mic_mute);
/*        mic_mute.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                keyup[3] = true;
                mic_mute.setVisibility(View.INVISIBLE);
            }
        });*/
        camera_mute = findViewById(R.id.camera_mute);
        camera_mute.setVisibility(View.GONE);
/*        camera_mute.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                keyup[2] = true;
                camera_mute.setVisibility(View.INVISIBLE);
            }
        });*/
        if (service == null)
        {
            service =PeripheralManager.getInstance();
        }
        checkPass();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {

        Log.e("TAG", "keycode : " + event.getKeyCode() + " key action : " + event.getAction());
        switch (event.getKeyCode())
        {
            case 24:
                bt_volum_add.setVisibility(View.INVISIBLE);
                checkPass();
                break;
            case 25:
                bt_volum_down.setVisibility(View.INVISIBLE);
                checkPass();
                break;
/*            case 138:
                Log.e("TAG", "触发操作");
                // camera_mute.setVisibility(View.INVISIBLE);
                Log.e("TAG","触发操作");
                camera_mute.setVisibility(View.INVISIBLE);
                break;
            case 139:
                Log.e("TAG", "触发操作");
                //mic_mute.setVisibility(View.INVISIBLE);
                Log.e("TAG","触发操作");
                mic_mute.setVisibility(View.INVISIBLE);
                break;*/
            default:
                break;
        }

        return super.onKeyDown(keyCode, event);
    }

    public void initConfirmButton()
    {
        Button btn_pass = findViewById(R.id.btn_result_pass);
        btn_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_KEY, true, "007");
                finish();
            }
        });
        Button btn_reset = findViewById(R.id.btn_result_reset);
        btn_reset.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                resetTest();
            }
        });
        Button btn_fail = findViewById(R.id.btn_result_fail);
        btn_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_KEY, false, "007");
                finish();
            }
        });
    }


    /**
     * 重新测试
     */
    private void resetTest()
    {
        bt_volum_add.setVisibility(View.VISIBLE);
        bt_volum_down.setVisibility(View.VISIBLE);
        mic_mute.setVisibility(View.VISIBLE);
       // camera_mute.setVisibility(View.VISIBLE);
    }

    private void registerGpioListener()
    {
        try
        {
            Log.d(TAG, "registerGpioListener =" + service.getGpioList());


            gpio87 = service.openGpio("GPIO_87");
            gpio87.setDirection(Gpio.DIRECTION_IN);
            gpio87.setEdgeTriggerType(Gpio.EDGE_BOTH);
            gpio87.registerGpioCallback(gpioCallback87);
            Log.d(TAG, "gpio87 value " + gpio87.getValue());

            gpio86 = service.openGpio("GPIO_86");
            gpio86.setDirection(Gpio.DIRECTION_IN);
            gpio86.setEdgeTriggerType(Gpio.EDGE_BOTH);
            gpio86.registerGpioCallback(gpioCallback86);
            Log.d(TAG, "gpio86 value " + gpio86.getValue());

/*            gpio85 = service.openGpio("GPIO_85");
            gpio85.setDirection(Gpio.DIRECTION_IN);
            gpio85.setEdgeTriggerType(Gpio.EDGE_BOTH);
            gpio85.registerGpioCallback(gpioCallback85);
            Log.d(TAG, "gpio85 value " + gpio85.getValue());

            gpio1019 = service.openGpio("GPIO_1019");
            gpio1019.setDirection(Gpio.DIRECTION_IN);
            gpio1019.setEdgeTriggerType(Gpio.EDGE_BOTH);
            gpio1019.registerGpioCallback(gpioCallback1019);
            Log.d(TAG, "gpio85 value " + gpio1019.getValue());*/
        } catch (Exception e)
        {
            e.printStackTrace();
        }
    }

    GpioCallback gpioCallback87 = new GpioCallback()
    {
        @Override
        public boolean onGpioEdge(Gpio gpio)
        {
            try
            {
                boolean camera_mute_result = gpio.getValue();
                if (camera_mute_result)
                {
                    //camera_mute.setVisibility(View.INVISIBLE);
                }
                checkPass();
                Log.d(TAG, "gpioCallback87 value =" + camera_mute_result);
            } catch (IOException e)
            {
                e.printStackTrace();
            }
            return true;
        }
    };

    GpioCallback gpioCallback86 = new GpioCallback()
    {
        @Override
        public boolean onGpioEdge(Gpio gpio)
        {
            try
            {
                boolean mic_mute_result = gpio.getValue();
                if (mic_mute_result)
                {
                    mic_mute.setVisibility(View.INVISIBLE);
                }
                checkPass();
                Log.d(TAG, "gpioCallback86 value =" + mic_mute_result);
            } catch (IOException e)
            {
                e.printStackTrace();
            }
            return true;
        }
    };

   /* GpioCallback gpioCallback85 = new GpioCallback()
    {
        @Override
        public boolean onGpioEdge(Gpio gpio)
        {
            try
            {
                boolean keyDown = gpio.getValue();
                if(keyDown){
                    Log.d(TAG, "gpioCallback85 volumeUP key down" );
                }else{
                    Log.d(TAG, "gpioCallback85 volumeUP key up" );
                }
                bt_volum_add.setVisibility(View.INVISIBLE);
                checkPass();
            } catch (IOException e)
            {
                e.printStackTrace();
            }
            return true;
        }
    };
    GpioCallback gpioCallback1019 = new GpioCallback()
    {
        @Override
        public boolean onGpioEdge(Gpio gpio)
        {
            try
            {
                boolean keyDown = gpio.getValue();
                if(keyDown){
                    Log.d(TAG, "gpioCallback1019 volumeDOWN key down" );
                }else{
                    Log.d(TAG, "gpioCallback1019 volumeDOWN key up" );
                }
                bt_volum_down.setVisibility(View.INVISIBLE);
                checkPass();
            } catch (IOException e)
            {
                e.printStackTrace();
            }
            return true;
        }
    };*/

    private void checkPass()
    {
        if (bt_volum_add.getVisibility() == View.INVISIBLE &&
                bt_volum_down.getVisibility() == View.INVISIBLE &&
                mic_mute.getVisibility() == View.INVISIBLE )
              //  && camera_mute.getVisibility() == View.INVISIBLE)
        {
            findViewById(R.id.btn_result_pass).setEnabled(true);
            handler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_SUCCESS, PCBAHandler.DELAY);
        } else
        {
            findViewById(R.id.btn_result_pass).setEnabled(false);
        }
    }

    private void unregisterGpioListener()
    {
        try
        {
            if (gpio87 != null)
            {
                gpio87.close();
                gpio87.unregisterGpioCallback(gpioCallback87);
            }
            if (gpio86 != null)
            {
                gpio86.close();
                gpio86.unregisterGpioCallback(gpioCallback86);
            }
/*            if (gpio85 != null)
            {
                gpio85.close();
                gpio85.unregisterGpioCallback(gpioCallback85);
            }
            if (gpio1019 != null)
            {
                gpio1019.close();
                gpio1019.unregisterGpioCallback(gpioCallback1019);
            }*/
        } catch (IOException e)
        {
            e.printStackTrace();
        }
    }
    class  VolumeChangeReceiver extends BroadcastReceiver
    {
        @Override
        public void onReceive(Context context, Intent intent)
        {
            String action=intent.getAction();
            int current=intent.getIntExtra(Constant.INTENT_KEY_CURRENT_VOLUME,100);
            int volume_type=intent.getIntExtra(Constant.INTENT_KEY_VOLUME_TYPE, AudioManager.STREAM_MUSIC);
            if(Constant.ACTION_VOLUME_UP.equals(action))
            {
                bt_volum_add.setVisibility(View.INVISIBLE);
                checkPass();
            }else if(Constant.ACTION_VOLUME_DOWN.equals(action))
            {
                bt_volum_down.setVisibility(View.INVISIBLE);
                checkPass();
            }
        }
    }
}
