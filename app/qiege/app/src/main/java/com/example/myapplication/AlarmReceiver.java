package com.example.myapplication;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;


public class AlarmReceiver  extends BroadcastReceiver {
    @Override
        public void onReceive(Context context, Intent intent) {
            Intent i = new Intent(context, MyService.class);
            context.startService(i);
        }


    private void showToast(Context context, String msg) {
        Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
    }


    public void onEnabled(Context context, Intent intent) {
        showToast(context,
                "�豸������ʹ��");
    }


    public void onDisabled(Context context, Intent intent) {
        showToast(context,
                "�豸������û��ʹ��");
    }

    }
