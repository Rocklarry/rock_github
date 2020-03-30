package com.example.android_things;


import android.app.Activity;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import com.google.android.things.pio.PeripheralManager;

import java.util.List;

public class Io_list extends Activity {

    ListView io_list;
    private ArrayAdapter<String> all_io;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_io_list);

        io_list = findViewById(R.id.io_list);


        PeripheralManager manager = PeripheralManager.getInstance();
        List<String> deviceList = manager.getI2cBusList();
        deviceList.addAll(manager.getSpiBusList());
        deviceList.addAll(manager.getUartDeviceList());
        deviceList.addAll(manager.getPwmList());
        deviceList.addAll(manager.getGpioList());

        all_io = new ArrayAdapter<>(this,android.R.layout.simple_list_item_1, deviceList);
        io_list.setAdapter(all_io);

    }


}
