package com.synaptics.sagitta.rest;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class BaseFunction {
    Activity activity;

    public BaseFunction(Activity act) {
        activity = act;
    }

    public void setText(final TextView text, final String value) {
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                text.setText(value);
                text.setVisibility(View.VISIBLE);
            }
        });
    }

    @SuppressLint("ResourceAsColor")
    public void enableButton(Button button) {
        button.getBackground().setColorFilter(null);
    }

    @SuppressLint("ResourceAsColor")
    public void disableButton(Button button) {
        button.getBackground().setColorFilter(Color.GRAY, PorterDuff.Mode.MULTIPLY);
    }
}
