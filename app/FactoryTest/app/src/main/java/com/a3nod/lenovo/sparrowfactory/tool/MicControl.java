package com.a3nod.lenovo.sparrowfactory.tool;

import com.google.android.things.pio.Gpio;
import com.google.android.things.pio.PeripheralManager;

import java.io.IOException;

import static com.google.android.things.pio.Gpio.DIRECTION_OUT_INITIALLY_HIGH;

/**
 * Created by Lenovo on 2017/12/8.
 */

public class MicControl {
    private Gpio micPin;
    public MicControl(){
        PeripheralManager service = PeripheralManager.getInstance();
        try {
            micPin=service.openGpio("GPIO_117");
            micPin.setDirection(DIRECTION_OUT_INITIALLY_HIGH);
            micPin.setActiveType(Gpio.ACTIVE_HIGH);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void close(){
        try {
            if(micPin!=null)
                micPin.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void switchMic(boolean high){
        try {
                micPin.setValue(high);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
