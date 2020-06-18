package com.synaptics.sagitta.rest;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private String CoarseLocation = Manifest.permission.ACCESS_COARSE_LOCATION;
    private String TAG = "Main";
    private Globals g = Globals.getInstance();
    private HttpService httpService = null;
    BaseFunction baseFunction;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        checkPermission();
        setContentView(R.layout.activity_main);
        Log.d(TAG, "Board status " + g.getBoardStatus());
        if (g.getBoardStatus() == g.BOARD_REACHABLE_BY_WIFI) {
            CommonFunctions commonFunctions = new CommonFunctions(this);
            commonFunctions.discoverDevices();
        }
    }

    @SuppressLint("ResourceAsColor")
    public void connectDevice(View v){
        Log.d(TAG, "Try to connect board by AP");
        findViewById(R.id.APIP).setVisibility(View.INVISIBLE);
        TextView status = findViewById(R.id.status);
        status.setText(getResources().getString(R.string.connectingDevice));
        status.setVisibility(View.VISIBLE);
        Button connectDevice = findViewById(R.id.connectDevice);
        initBaseFunction();
        baseFunction.disableButton(connectDevice);
        WifiTask connectNiue = new WifiTask(this);
        connectNiue.execute();
    }

    public void setupWifiShow(View v) {
        Intent intent = new Intent(this, WifiActivity.class);
        startActivity(intent);
    }

    public void getDeviceIP(View v) {
        g.setMessageLocation(g.ON_MAIN_PAGE);
        findViewById(R.id.DeviceIP).setVisibility(View.INVISIBLE);
        TextView status = findViewById(R.id.status);
        status.setText(getResources().getString(R.string.gettingDevice));
        status.setVisibility(View.VISIBLE);
        Button getDeviceIP = findViewById(R.id.getDeviceIP);
        initBaseFunction();
        baseFunction.disableButton(getDeviceIP);
        CommonFunctions commonFunctions = new CommonFunctions(this);
        commonFunctions.discoverDevices();
    }

    public void authAVS(View v) {
        //Show verification_url and user_code in this function
        g.setMessageLocation(g.ON_MAIN_PAGE);
        initHttpService();
        String url = "/settings/avs/authinfo";
        httpService.sendHttpRequest(url, null);
    }

    public void checkPermission() {
        if (checkSelfPermission(CoarseLocation) == PackageManager.PERMISSION_GRANTED) {
            requestPermissions(new String[]{CoarseLocation}, 1);
        }
        requestPermissions(new String[]{CoarseLocation}, 1);
    }

    private void initBaseFunction() {
        if (baseFunction == null) {
            baseFunction = new BaseFunction(this);
        }
    }

    private void initHttpService() {
        if (httpService == null) {
            httpService = new HttpService(this);
        }
    }

}