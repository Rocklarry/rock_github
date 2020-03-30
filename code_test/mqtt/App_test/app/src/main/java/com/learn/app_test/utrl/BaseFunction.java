package com.learn.app_test.utrl;

import android.app.Activity;
import android.view.View;
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
                text.setVisibility( View.VISIBLE);
            }
        });
    }
}
