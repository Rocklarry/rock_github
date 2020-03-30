package com.a3nod.lenovo.sparrowfactory;

import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.driver.AmpSetupValues;
import com.a3nod.lenovo.sparrowfactory.driver.Bh1745SensorDriver;
import com.a3nod.lenovo.sparrowfactory.facecamera.CameraActivity;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManagerService;
import com.a3nod.lenovo.sparrowfactory.mmi.wifi.WifiManageUtils;
import com.a3nod.lenovo.sparrowfactory.runin.RuninMainActivity;
import com.a3nod.lenovo.sparrowfactory.runin.RuninService;
import com.a3nod.lenovo.sparrowfactory.tool.BrightnessTools;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SystemInfoTools;
import com.google.android.things.pio.Gpio;
import com.google.android.things.pio.I2cDevice;
import com.google.android.things.pio.PeripheralManager;

import java.io.File;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static com.a3nod.lenovo.sparrowfactory.driver.AmpSetupValues.registers_10;
import static com.a3nod.lenovo.sparrowfactory.driver.AmpSetupValues.registers_8;

/**
 * Created by Lenovo on 2017/11/13.
 */

public class NavigationActivity extends Activity
{
    private static final String TAG = NavigationActivity.class.getSimpleName();
    private ListView mlist;
    protected static final int REQUEST_CODE = 0;
    //    private static final String I2C_BUS = "I2C1";
//    private Tas5782m tas5782m;
    private TextView tv_time;
    DecimalFormat df = new DecimalFormat("#######00");
    private static final int MSG_CURRENT_TIME = 10;
    private static final int MSG_SHOW_MESSAGE = 11;
    StringBuilder sb;
    boolean is8Inch=true;

    private boolean isCompleteWrite = false;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
//        startService(new Intent(this, ConnectService.class));
        setContentView(R.layout.activity_navi);
        AudioManager mAudioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC,mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC),0);

        is8Inch=is8InchScreen();
        initTitle();
        initView();
        initDriver();
        initFolder();
//        setScreenBrightness();

        requestPermission(new String[]{
                Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.ACCESS_FINE_LOCATION,
                Manifest.permission.RECORD_AUDIO,
                Manifest.permission.BLUETOOTH,
                "com.google.android.things.permission.MANAGE_SENSOR_DRIVERS",
                Manifest.permission.CAMERA
        });

        sb = new StringBuilder("");
        startService(new Intent(this, RuninService.class));
        startService(new Intent(this, ConnectManagerService.class));

        openWifiBluetooth();
    }

    private void  openWifiBluetooth(){
        BluetoothAdapter  bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        bluetoothAdapter.enable();

        WifiManageUtils wifiManageUtils = WifiManageUtils.getInstance(this);
        wifiManageUtils.initManager();
        wifiManageUtils.openWifi();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState)
    {
        super.onSaveInstanceState(outState);
        LogUtil.i("NavigationActivity onSaveInstanceState");
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState)
    {
        super.onRestoreInstanceState(savedInstanceState);
        LogUtil.i("NavigationActivity onRestoreInstanceState");
    }

    public boolean is8InchScreen(){
        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);//display = getWindowManager().getDefaultDisplay();display.getMetrics(dm)（把屏幕尺寸信息赋值给DisplayMetrics dm）;
        int width = dm.widthPixels;
        LogUtil.i("screen  width : " + width + " height : " + dm.heightPixels);
        if(width == 800){
            return true;
        }else{
            return false;
        }
    }


    public void initTitle()
    {
        TextView tv_title = findViewById(R.id.tv_menu_title);
        tv_title.setText(R.string.app_name);
        ImageView iv_back = findViewById(R.id.iv_menu_back);
        iv_back.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                finish();
            }
        });
        iv_back.setVisibility(View.GONE);
    }

    public void initView()
    {

        TextView tv_versionName = findViewById(R.id.tv_version_name);
        tv_versionName.setText(getVersionName());

        tv_time = findViewById(R.id.tv_system_time);

        mlist = this.findViewById(R.id.naviitem_id);
        String[] data = {"PCBAMMI", "AssyMMI", "Run-in", "More" /*,"FaceCameraTest"*/};
        ArrayAdapter<String> arrayAdapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, data);
        mlist.setAdapter(arrayAdapter);
        mlist.setOnItemClickListener(new AdapterView.OnItemClickListener()
        {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l)
            {
                if (!isCompleteWrite){
                    Toast.makeText(getApplicationContext(),getString(R.string.not_finish_write),Toast.LENGTH_LONG).show();
                    return;
                }
                switch (i)
                {
                    case 0:
                        startOtherActivity("hello.PcbaTestActivity");
                        break;
                    case 1:
                        startOtherActivity("hello.MainActivity");
                        break;
                    case 2:
                        startActivity(new Intent(NavigationActivity.this, RuninMainActivity.class));
                        break;
                    case 3:
                        startActivity(new Intent(NavigationActivity.this, MoreActivity.class));
                        break;
              /*      case 4:
                        startActivity(new Intent(NavigationActivity.this, CameraActivity.class));
                        break;*/

                }
            }
        });

    }

    MediaPlayer mMediaPlayer;

    public void initDriver()
    {
        PeripheralManager manager = PeripheralManager.getInstance();
        List<String> portList = manager.getGpioList();
        if (portList.isEmpty())
        {
            LogUtil.i("No GPIO port available on this device.");
        } else
        {
            LogUtil.i("List of available ports: " + portList);
        }

        //            SpeakerSwitch speakerSwitch = new SpeakerSwitch();
//            speakerSwitch.switchMute(false);
//            speakerSwitch.close();
        new Thread()
        {
            @Override
            public void run()
            {
                super.run();
                mMediaPlayer = MediaPlayer.create(NavigationActivity.this, R.raw.silence);
                mMediaPlayer.setLooping(true);
                mMediaPlayer.start();
                try
                {
                    sleep(4000);
                    Log.d(TAG,"-------is8Inch---"+is8Inch);
                    if (is8Inch)
                    {
                        enableAmplifier(false);
                        enableRgbSensor();
//                        try {
//                            new Bh1745SensorDriver("I2C2").registerRGBSensor();
//                        } catch(IOException e){
//                            Log.e(TAG, "Error configuring Bh1745 RGB sensor " + e.getLocalizedMessage());
//                        }
                    } else
                    {
                        enableAmplifier(true);
                        enableRgbSensor();
//                        try {
//                            new Bh1745SensorDriver("I2C2").oregisterRGBSensor();
//                        } catch(IOException e){
//                            Log.e(TAG, "Error configuring Bh1745 RGB sensor " + e.getLocalizedMessage());
//                        }
                    }
                    handler.sendEmptyMessage(MSG_SHOW_MESSAGE);
                    if (mMediaPlayer != null){
                        mMediaPlayer.stop();
                    }
                } catch (InterruptedException e)
                {
                    e.printStackTrace();
                }
            }
        }.start();

    }

    public static short registers_rgb[][] = {
            {0x41,0x00},
            {0x42,0x10}
    };
    public void enableRgbSensor(){
        String I2C_DEVICE_NAME_2 = "I2C2";
        int I2C_SLAVE_ADDRESS_2 = 0x38;

        PeripheralManager manager = PeripheralManager.getInstance();
        try (
                I2cDevice rgbControl = manager.openI2cDevice(I2C_DEVICE_NAME_2, I2C_SLAVE_ADDRESS_2);
        ){
            byte[] buf = new byte[2];
            for (int index = 0; index < registers_rgb.length; index++) {
                buf[0] = (byte)registers_rgb[index][0];
                buf[1] = (byte)registers_rgb[index][1];
                Log.d(TAG,"-------buf[0]---"+buf[0]+"---buf[1]---"+buf[1]);
                rgbControl.write(buf, 2);
            }
        }catch (IOException | IllegalStateException|NullPointerException e) {
            e.printStackTrace();
        }
    }

    public void initFolder()
    {
        File dir = new File(Constant.PROJECT_DIR);
        if (!dir.exists() || !dir.isDirectory())
        {
            dir.mkdirs();
        }
        dir = new File(Constant.RUNIN_DIR);
        if (!dir.exists() || !dir.isDirectory())
        {
            dir.mkdirs();
        }
        dir = new File(Constant.MMI_DIR);
        if (!dir.exists() || !dir.isDirectory())
        {
            dir.mkdirs();
        }
        dir = new File(Constant.PCBA_DIR);
        if (!dir.exists() || !dir.isDirectory())
        {
            dir.mkdirs();
        }
        dir = new File(Constant.CAMERA_DIR);
        if (!dir.exists() || !dir.isDirectory())
        {
            dir.mkdirs();
        }
    }

    private void setScreenBrightness()
    {
        BrightnessTools.stopAutoBrightness(this);
        BrightnessTools.saveBrightness(getContentResolver(), is8Inch?40:100);
        int brightness = BrightnessTools.getScreenBrightness(this);
        LogUtil.i(" current brightness : " + brightness);
    }


    private String getVersionName()
    {
        // 获取packagemanager的实例
        PackageManager packageManager = getPackageManager();
        // getPackageName()是你当前类的包名，0代表是获取版本信息
        PackageInfo packInfo = null;
        String version = "";
        try
        {
            packInfo = packageManager.getPackageInfo(getPackageName(), 0);
            version = packInfo.versionName;
        } catch (PackageManager.NameNotFoundException e)
        {
            e.printStackTrace();
        }
        return version;
    }

    private void getSystemTime()
    {
        long t = RuninService.time;
        tv_time.setText(format(t * 1000));
    }


    public String format(Long time)
    {
        sb.setLength(0);
        long hour = time / (60 * 60 * 1000);
        long minute = (time - hour * 60 * 60 * 1000) / (60 * 1000);
        long second = (time - hour * 60 * 60 * 1000 - minute * 60 * 1000) / 1000;
        return sb.append(df.format(hour)).append(":").append(df.format(minute)).append(":").append(df.format(second)).toString();
    }

    Handler handler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            switch (msg.what)
            {
                case MSG_CURRENT_TIME:
                    getSystemTime();
                    sendEmptyMessageDelayed(MSG_CURRENT_TIME, 1000);
                    break;
                case MSG_SHOW_MESSAGE:
                    isCompleteWrite = true;
                    Toast.makeText(getApplicationContext(),"initialization complete",Toast.LENGTH_LONG).show();
                    break;
            }
        }
    };

    @Override
    protected void onResume()
    {
        super.onResume();
        handler.sendEmptyMessage(MSG_CURRENT_TIME);
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        handler.removeCallbacksAndMessages(null);
    }

    void startOtherActivity(String action)
    {
        Intent intent = new Intent(action);
        startActivity(intent);
    }

    protected void requestPermission(String[] permissions)
    {
        List<String> lsp = new ArrayList<>();
        for (int i = 0; i < permissions.length; i++)
        {
            LogUtil.d("request : " + permissions[i]);
            if (ContextCompat.checkSelfPermission(this, permissions[i]) != PackageManager.PERMISSION_GRANTED)
            {
                lsp.add(permissions[i]);
                LogUtil.d(permissions[i] + "  permissions  leak ");
            }
        }

        if (lsp.size() > 0)
        {
            String[] sa = new String[lsp.size()];
            lsp.toArray(sa);
            LogUtil.d("request : " + sa[0]);
            ActivityCompat.requestPermissions(this, sa, REQUEST_CODE);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults)
    {
        switch (requestCode)
        {
            case REQUEST_CODE:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED)
                {
                    //用户同意授权
                    initFolder();
                } else
                {
                    //用户拒绝授权
                    Toast.makeText(this, "permission denied , exit in 3 seconds", Toast.LENGTH_SHORT).show();
                    try
                    {
                        Thread.sleep(3000);
                    } catch (InterruptedException e)
                    {
                        e.printStackTrace();
                    }

                    finish();
                }
                break;
        }
    }


    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        stopService(new Intent(this, RuninService.class));
//        ConnectManager.getInstance().stopServer();
        stopService(new Intent(this, ConnectManagerService.class));
//        if (tas5782m != null) try {
//            tas5782m.close();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
        if (mMediaPlayer != null)
        {
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
    }

    public static void enableAmplifier(boolean is_10inch)
    {
        Log.i(TAG, "333  Enable Lenovo Amplifier");
        Log.i(TAG, "AmpSetupValues version ="+ AmpSetupValues.TAG);
        PeripheralManager manager = PeripheralManager.getInstance();
        String AMP_GPIO_EN = "GPIO_44";
        Gpio ampEnableGpio = null;
        I2cDevice ampControl = null;
        String I2C_DEVICE_NAME = "I2C1";
        int I2C_SLAVE_ADDRESS = 0x2c;
        try {
            Log.i(TAG, "Available GPIOs: " + Arrays.toString(manager.getGpioList().toArray()));
            Log.i(TAG, "Available I2Cs: " + Arrays.toString(manager.getI2cBusList().toArray()));
            // Turn on the amplifier - the amplifier is enabled via GPIO pin, so we select the
            // appropriate one from the list, let's suppose GPIO68, and open the GPIO. Note that
            // the name must match the name in the list EXACTLY.
            ampEnableGpio = manager.openGpio(AMP_GPIO_EN);
            // Set this GPIO's direction to 'out' and 'high'.
            ampEnableGpio.setDirection(Gpio.DIRECTION_OUT_INITIALLY_HIGH);
            // To turn off the amplifier, we could set it low.
            // ampEnableGpio.setValue(false);

            // Open a connection on the I2C bus that the amplifier is on, and specify the controller
            // slave address. From there, we'll issue the commands to configure the amplifier.
            // We'll assume the amplifier is on I2C1, and has slave address 0x49.
            ampControl = manager.openI2cDevice(I2C_DEVICE_NAME, I2C_SLAVE_ADDRESS);
            Log.i(TAG, "Initializing the amplifier with writeRegByte address = "+I2C_SLAVE_ADDRESS );
            byte[] buf = new byte[2];

            if (is_10inch)
            {
                for (int index = 0; index < registers_10.length; index++)
                {
                    buf[0] = (byte) registers_10[index][0];
                    buf[1] = (byte) registers_10[index][1];

                    // TODO(kpt): Which type of I2C write should we do here?
                    Log.d(TAG,"-------buf[0]---"+buf[0]+"---buf[1]---"+buf[1]);
                    ampControl.write(buf, 2);
                    //ampControl.writeRegByte(buf[0],(byte)buf[1]);
                    Log.d(TAG,"========amp===address="+buf[0]+"=====value="+ampControl.readRegByte(buf[0]));
                }
            } else
            {
                for (int index = 0; index < registers_8.length; index++)
                {
                    buf[0] = (byte) registers_8[index][0];
                    buf[1] = (byte) registers_8[index][1];
                    // TODO(kpt): Which type of I2C write should we do here?
                    Log.d(TAG,"-------buf[0]---"+buf[0]+"---buf[1]---"+buf[1]);
                    ampControl.write(buf, 2);
                    //ampControl.writeRegByte(buf[0],(byte)buf[1]);
                    Log.d(TAG,"========amp===address="+buf[0]+"=====value="+ampControl.readRegByte(buf[0]));
                }
            }
            // Looks like we're done with the configuration, let's clean up.
            ampControl.close();
            ampEnableGpio.close();
        } catch (IOException | IllegalStateException|NullPointerException e) {
            e.printStackTrace();
        }
    }
}
