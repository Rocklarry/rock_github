package com.synaptics.sagitta.rest;

import android.app.Activity;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class HttpService{
    private final String TAG = "HTTP";

    private Globals g = Globals.getInstance();
    BaseFunction baseFunction;
    OkHttpClient httpClient = new OkHttpClient();
    Activity activity;

    public HttpService(Activity act) {
        activity = act;
        baseFunction = new BaseFunction(activity);
    }

    public void sendHttpRequest(String url, String json) {
        final Request request;
        if (g.getRestServer() == null) {
            Log.e(TAG, "Cannot get rest server IP!!!");
            TextView statusTextView = activity.findViewById(R.id.status);
            baseFunction.setText(statusTextView, activity.getResources().getString(R.string.getIPWarning));
        } else {
            Log.i(TAG, "Rest server IP is " + g.getRestServer());
            String full_url = "http://" + g.getRestServer() + ":8000" + url;
            Log.i(TAG, "Request URL " + full_url);
            if (json != null) {
                Log.i(TAG, "Dan receive a post request. Post data is " + json);
                MediaType JSON = MediaType.get("application/json; charset=utf-8");
                RequestBody body = RequestBody.create(JSON, json);
                request = new Request.Builder()
                        .url(full_url)
                        .post(body)
                        .build();
            } else {
                request = new Request.Builder()
                        .url(full_url)
                        .build();
            }

            httpClient.newCall(request).enqueue(new Callback() {
                String aplistURL = "http://" + g.getRestServer() + ":8000/settings/network/wireless_access_points";
                String connectWifiURL = "http://" + g.getRestServer() + ":8000/settings/network/current_access_point";

                @Override
                public void onFailure(Call call, IOException e) {
                    if (request.url().toString().compareTo(aplistURL) == 0) {
                        showGetAPListFailedMessage();
                    }
                    else if (request.url().toString().compareTo(connectWifiURL) == 0)  {
                        responseWifiConnect();
                    }
                    e.printStackTrace();
                    call.cancel();
                }

                @Override
                public void onResponse(Call call, Response response) throws IOException {
                    if (response.isSuccessful()) {
                        final String responseStr = response.body().string();
                        activity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Log.d(TAG, "Get reponse string " + responseStr);

                                if (request.url().toString().compareTo(aplistURL) == 0) {
                                    Log.d(TAG, "Get APList " + responseStr);
                                    responseAPList(responseStr);
                                }
                                else {
                                    Toast.makeText(activity, responseStr, Toast.LENGTH_SHORT).show();
                                    Log.d(TAG, "wifi url " + connectWifiURL);
                                    Log.d(TAG, "request url " + request.url().toString());
                                    if (request.url().toString().compareTo(connectWifiURL) == 0) {
                                        responseWifiConnect();
                                    }
                                }
                            }
                        });
                    }
                }

                private void responseAPList(String responseStr) {
                    List <String> apList = new ArrayList<String>();
                    try {
                        JSONArray apsJson = new JSONObject(responseStr).getJSONArray("APList");
                        if (apsJson != null) {
                            int len = apsJson.length();
                            for (int i=0;i<len - 1;i++){
                                if (apsJson.get(i) != null) {
                                    apList.add(apsJson.get(i).toString());
                                }
                            }
                        }
                        if (apList.size() > 0) {
                            Spinner ssidSpinner = (Spinner) activity.findViewById(R.id.ssidList);
                            ArrayAdapter<String> adapter = new ArrayAdapter(activity, android.R.layout.simple_spinner_dropdown_item, apList);
                            if (adapter == null) {
                                Log.e(TAG, "Spinner adapter is null");
                                showGetAPListFailedMessage();
                            } else if (ssidSpinner == null) {
                                Log.e(TAG, "Spinner is null");
                                showGetAPListFailedMessage();
                            }else {
                                ssidSpinner.setAdapter(adapter);
                                activity.findViewById(R.id.wifiStatus).setVisibility(View.INVISIBLE);
                            }
                            if (apList.size() == 1) {
                                showGetAPListFailedMessage();
                            }
                        } else {
                            showGetAPListFailedMessage();
                        }
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }

                private void responseWifiConnect() {
                    Log.d(TAG, "Update global board status to " + g.BOARD_REACHABLE_BY_WIFI);
                    g.setBoardStatus(g.BOARD_REACHABLE_BY_WIFI);
                    TextView wifiStatusTextView = activity.findViewById(R.id.wifiStatus);
                    baseFunction.setText(wifiStatusTextView, activity.getResources().getString(R.string.sentWifiInfo));
                }

                private void showGetAPListFailedMessage() {
                    if (g.getBoardStatus() == Globals.BOARD_UNREACHABLE) {
                        TextView statusTextView = activity.findViewById(R.id.status);
                        baseFunction.setText(statusTextView, activity.getResources().getString(R.string.getIPWarning));
                    } else {
                        TextView wifiStatus = (TextView)activity.findViewById(R.id.wifiStatus);
                        String getAPlistFailed = activity.getResources().getString(R.string.getAPListFailed);
                        baseFunction.setText(wifiStatus, getAPlistFailed);
                    }
                }
            });
        }
    }
}