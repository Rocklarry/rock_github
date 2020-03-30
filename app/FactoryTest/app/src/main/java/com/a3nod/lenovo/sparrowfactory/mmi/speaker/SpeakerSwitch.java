package com.a3nod.lenovo.sparrowfactory.mmi.speaker;

import com.google.android.things.pio.Gpio;
import com.google.android.things.pio.PeripheralManager;

import java.io.IOException;

import static com.google.android.things.pio.Gpio.DIRECTION_OUT_INITIALLY_HIGH;

/**
 * Created by Lenovo on 2017/12/8.
 */

public class SpeakerSwitch {
    private Gpio micPin,pin44,pin68,pin45;
    public SpeakerSwitch(){
        PeripheralManager service = PeripheralManager.getInstance();
        try {
            micPin=service.openGpio("GPIO_0");
            micPin.setDirection(DIRECTION_OUT_INITIALLY_HIGH);
            micPin.setActiveType(Gpio.ACTIVE_HIGH);
            pin68=service.openGpio("GPIO_68");
            pin68.setDirection(DIRECTION_OUT_INITIALLY_HIGH);
            pin68.setActiveType(Gpio.ACTIVE_HIGH);
            pin68.setValue(true);
            pin68.close();
            pin44=service.openGpio("GPIO_44");
            pin44.setDirection(DIRECTION_OUT_INITIALLY_HIGH);
            pin44.setActiveType(Gpio.ACTIVE_HIGH);
            pin44.setValue(false);
            pin44.close();
            pin45=service.openGpio("GPIO_45");
            pin45.setDirection(DIRECTION_OUT_INITIALLY_HIGH);
            pin45.setActiveType(Gpio.ACTIVE_HIGH);
            pin45.setValue(false);
            pin45.close();
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

    public void switchMute(boolean high){
        try {
                micPin.setValue(high);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
