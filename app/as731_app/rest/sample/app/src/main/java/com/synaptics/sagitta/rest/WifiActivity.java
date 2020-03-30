package com.synaptics.sagitta.rest;

import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.InputType;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

public class WifiActivity extends AppCompatActivity {
    private String TAG = "WIFI";
    private WifiManager wifi;
    private Spinner ssidSpinner;
    private HttpService httpService = null;
    private Globals g = Globals.getInstance();
    BaseFunction baseFunction;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.setup_wifi);
        getAPList();
        Log.i(TAG, "Board status " + g.getBoardStatus());
    }

    public void setupWifi(View v){
        ssidSpinner = (Spinner) this.findViewById(R.id.ssidList);
        String ssid = String.valueOf(ssidSpinner.getSelectedItem());
        EditText p = (EditText)findViewById(R.id.password);
        String password = String.valueOf(p.getText());
        initHttpService();
        String url = "/settings/network/current_access_point";
        String postData = "{\"ssid\" : \""  + ssid + "\"," +
                "\"password\" : \""  + password + "\"}";
        TextView wifiStatusTextView = findViewById(R.id.wifiStatus);
        wifiStatusTextView.setText(getResources().getString(R.string.sendingwifiInfo));
        wifiStatusTextView.setVisibility(View.VISIBLE);
        httpService.sendHttpRequest(url, postData);
    }

    public void reconnectInternetAP(View v){
        Log.d(TAG, "In reconnectInternetAP function");
        findViewById(R.id.wifiStatus).setVisibility(View.INVISIBLE);
        g.setMessageLocation(g.ON_WIFI_PAGE);
        initBaseFunction();
        Button reconnectInternetAP = findViewById(R.id.reconnectInternetAP);
        initBaseFunction();
        baseFunction.disableButton(reconnectInternetAP);
        CommonFunctions commonFunctions = new CommonFunctions(this);
        commonFunctions.doAfterSetupWifi();
    }

    public void reloadSSID(View v) {
        getAPList();
    }

    public void showPassword(View v) {
        EditText password = findViewById(R.id.password);
        TextView showPassword = findViewById(R.id.show);
        if (password.getInputType() == InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD) {
            password.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
            showPassword.setText(getResources().getString(R.string.show));
        } else {
            password.setInputType(InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD);
            showPassword.setText(getResources().getString(R.string.hide));
        }
    }

    private void getAPList() {
        initHttpService();
        String url = "/settings/network/wireless_access_points";
        httpService.sendHttpRequest(url, null);
    }

    private void initHttpService() {
        if (httpService == null) {
            httpService = new HttpService(this);
        }
    }

    private void initBaseFunction() {
        if (baseFunction == null) {
            baseFunction = new BaseFunction(this);
        }
    }
}