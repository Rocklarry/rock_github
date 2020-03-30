package com.a3nod.lenovo.sparrowfactory.tool;

import android.os.Handler;

import com.google.android.things.pio.Gpio;
import com.google.android.things.pio.GpioCallback;
import com.google.android.things.pio.PeripheralManager;

import java.io.IOException;

/**
 * Description
 * Created by aaa on 2018/1/17.
 */

public class CameraMuteControl
{
    private Gpio micPin;
    GpioCallback callback;

    public CameraMuteControl(GpioCallback callback, Handler handler)
    {
        this.callback = callback;
        PeripheralManager service = PeripheralManager.getInstance();
        try
        {
            micPin = service.openGpio("GPIO_87");
            micPin.setDirection(Gpio.DIRECTION_IN);
            micPin.setActiveType(Gpio.ACTIVE_HIGH);
            micPin.setEdgeTriggerType(Gpio.EDGE_BOTH);
            micPin.registerGpioCallback(handler,callback);
        } catch (IOException e)
        {
            e.printStackTrace();
        }
    }

    public void close()
    {
        try
        {
            if (micPin != null)
            {
                micPin.unregisterGpioCallback(callback);
                micPin.close();
            }
        } catch (IOException e)
        {
            e.printStackTrace();
        }
    }

    public void switchMute(boolean high)
    {
        try
        {
            micPin.setValue(high);
        } catch (IOException e)
        {
            e.printStackTrace();
        }
    }
}
