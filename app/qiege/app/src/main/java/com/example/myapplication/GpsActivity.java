package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;

import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;




public class GpsActivity extends Activity {

    private static final String TAG = MainActivity.class.getSimpleName();
    private Location location;

    private TextView info;
    private int count = 100;
    LocationManager locationManager;
    LocationListener llistener;
    String provider;

//    private Handler handler = new Handler();
//    private Runnable task = new Runnable() {
//        public void run() {
//            // TODO Auto-generated method stub
//            handler.postDelayed(this,100);//设置循环时间，此处是500ms秒
//            //需要执行的代码
//            count++;
//            //PropertyUtils.set("watermark.info.dis.updata",count+"km/h#123.369542#99.987654");
//            showLocation(location);
//            if(count>998){
//                count=100;
//            }
//        }
//    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gps);

        info = (TextView) findViewById(R.id.gps_info);
//        final LocationManager locationManager = (LocationManager)getSystemService(Context.LOCATION_SERVICE);
//        location = locationManager.getLastKnownLocation(LocationManager.GPS_PROVIDER);
//
//        showLocation(location);
//
//        locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 10, new LocationListener(){
//            public void onLocationChanged(Location location) {
//                // TODO Auto-generated method stub
//                showLocation(location);
//            }
//            public void onProviderDisabled(String provider) {
//                // TODO Auto-generated method stub
//                showLocation(null);
//            }
//            public void onProviderEnabled(String provider) {
//                // TODO Auto-generated method stub
//                showLocation(locationManager.getLastKnownLocation(provider));
//            }
//            public void onStatusChanged(String provider, int status, Bundle extras) {
//                // TODO Auto-generated method stub
//            }
//        });
//
//        handler.postDelayed(task,500);//延迟调用

        Criteria criteria = new Criteria();
        criteria.setAccuracy(Criteria.ACCURACY_FINE);
        criteria.setAltitudeRequired(false);
        criteria.setBearingRequired(false);
        criteria.setCostAllowed(true);
        criteria.setPowerRequirement(Criteria.POWER_LOW);
        String serviceName = Context.LOCATION_SERVICE;
        locationManager = (LocationManager) getSystemService(serviceName);
        locationManager.setTestProviderEnabled("gps", true);
        provider = locationManager.getBestProvider(criteria, true);
        Log.d("provider", provider);
        llistener = new LocationListener() {
            @Override
            public void onLocationChanged(Location location)
            {
                // TODO Auto-generated method stub
                Log.i("onLocationChanged", "come in");
                if (location != null)
                {
                    Log.w("Location", "Current altitude = "
                            + location.getAltitude());
                    Log.w("Location", "Current latitude = "
                            + location.getLatitude());

                    String s = "";
                    s += " Current Location: (";
                    s += location.getLongitude();
                    s += ",";
                    s += location.getLatitude();
                    s += ")\n Speed: ";
                    s += location.getSpeed();
                    s += "\n Direction: ";
                    s += location.getBearing();
                    info.setText(s);
                }

                locationManager.removeUpdates(this);
                locationManager.setTestProviderEnabled(provider, false);
            }
            @Override
            public void onProviderDisabled(String provider)
            {
                // TODO Auto-generated method stub
                Log.i("onProviderDisabled", "come in");
            }
            @Override
            public void onProviderEnabled(String provider)
            {
                // TODO Auto-generated method stub
                Log.i("onProviderEnabled", "come in");
            }
            @Override
            public void onStatusChanged(String provider, int status,
                                        Bundle extras)
            {
                // TODO Auto-generated method stub
                Log.i("onStatusChanged", "come in");
            }
        };
        locationManager.requestLocationUpdates(provider, 1000, (float) 1000.0, llistener);
    }

    protected void onDestroy()
    {
        locationManager.removeUpdates(llistener);
        locationManager.setTestProviderEnabled(provider, false);
        super.onDestroy();
    }
//
//    public void showLocation(Location currentLocation){
//        if(currentLocation != null){
//            String s = "";
//            s += " Current Location: (";
//            s += currentLocation.getLongitude();
//            s += ",";
//            s += currentLocation.getLatitude();
//            s += ")\n Speed: ";
//            s += currentLocation.getSpeed();
//            s += "\n Direction: ";
//            s += currentLocation.getBearing();
//            info.setText(s);
//        }
//        else{
//            info.setText("NULL"+count);
//        }
//    }


}