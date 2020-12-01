package com.example.myapplication;


import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.media.AudioManager;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.TextView;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.io.IOException;
import java.text.DecimalFormat;
import java.util.HashMap;
import java.util.Map;

public class Camera2_Activity extends Activity implements View.OnClickListener,PreviewCallback, CompoundButton.OnCheckedChangeListener {

    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Camera mCamera;
    private SurfaceTexture surfacetexture;
    private boolean mIsSufaceCreated1 = false;
    private boolean swith_camera = true;

    private Button cam_exit,cam_swith,cam_up,cam_down,video_H264,creat_num;
    private TextView cam_info,device_info,cpu_info;
    private CheckBox system_info_show;

    private static  final  String TAG = "Camera1_Activity";

    private int count = 100;
    int framerate = 15; //一秒30帧
    H264Encoder encoder; //自定义的编码操作类

    private boolean video_type=true;

    AudioManager mAudioManager;
    private static  int camera_width,camera_height;

    private DecimalFormat df;

    private Paint p;
    public String[] chars = new String[]{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "-", ":", " ","年","月","日"};
    public Map<String, byte[]> map = new HashMap<>();


    private Handler handler = new Handler();
    private Runnable task = new Runnable() {
        public void run() {
            // TODO Auto-generated method stub
            handler.postDelayed(this,500);//设置循环时间，此处是500ms秒
            //需要执行的代码
            count++;
            cpu_info.setText("CPU使用率: "+df.format(Device_info.getCpuUsed()*100)+"%"+"\n");
            PropertyUtils.set("watermark.info.dis.updata",count+"km/h#123.369542#99.987654");

            if(count>998){
                count=100;
            }
        }

    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera2);

        goRequestPermissions();
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);

        mSurfaceView = (SurfaceView) findViewById(R.id.camera_test2);
        mSurfaceHolder = mSurfaceView.getHolder();
        mSurfaceHolder.addCallback(mSurfaceCallback);
        mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        surfacetexture=new SurfaceTexture(10);

        cam_exit = (Button) findViewById(R.id.cam2_exit);
        cam_swith = (Button) findViewById(R.id.cam2_sw);
        cam_up = (Button) findViewById(R.id.cam2_up);
        cam_down = (Button) findViewById(R.id.cam2_down);
        cam_info = (TextView)findViewById(R.id.cam2_text);
        video_H264 = (Button)findViewById(R.id.cam2_video);
        creat_num = (Button)findViewById(R.id.act_num);

        device_info = (TextView)findViewById(R.id.device_info);
        device_info.setMovementMethod(ScrollingMovementMethod.getInstance());

        cpu_info = (TextView)findViewById(R.id.cpu_info);
        system_info_show = (CheckBox)findViewById(R.id.system_info);

        cam_exit.setOnClickListener(this);
        cam_swith.setOnClickListener(this);
        cam_up.setOnClickListener(this);
        cam_down.setOnClickListener(this);
        video_H264.setOnClickListener(this);
        creat_num.setOnClickListener(this);
        system_info_show.setOnCheckedChangeListener((CompoundButton.OnCheckedChangeListener) this);



        df = new DecimalFormat("0.00");
        handler.postDelayed(task,100);//延迟调用

        //startPreview();

        set_device_info();

        if (supportH264Codec()){ //查询手机是否支持AVC编码
            cam_info.setText("cam2 support H264 hard codec");
        }else {
            Log.e("TAG" , "not support H264 hard codec");
            cam_info.setText("cam2 not support H264 hard codec");
        }
        device_info.setVisibility(View.INVISIBLE);
    }


    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.cam2_sw:
                if(swith_camera) {
                    swith_camera= false;
                    cam_swith.setText("camera 1 open");
                    startPreview();
                    PropertyUtils.set("watermark.time.dis","0");

                    //handler.postDelayed(task,500);//延迟调用
                    //handler.post(task);//立即调用

                }else{
                    swith_camera=true;
                    cam_swith.setText("camera 1 close");
                    stopPreview();
                }
                break;
            case R.id.cam2_exit:
                finish();
                stopPreview();
                break;

            case R.id.cam2_up:
                stopPreview();
                Intent intent1 = new Intent(Camera2_Activity.this,Camera3_Activity.class);
                startActivity(intent1);
                break;

            case R.id.cam2_down:
                stopPreview();
                Intent intent2 = new Intent(Camera2_Activity.this,Camera1_Activity.class);
                startActivity(intent2);
                break;
            case R.id.cam2_video:
                if(video_type){
                    video_H264.setText("开始 H624");
                    video_type = false;
                    video_H264.setBackgroundColor(Color.rgb(0, 255, 0));
                    Camera.Parameters parameters = mCamera.getParameters();
                    Camera.Size size = parameters.getPreviewSize();
                    encoder = new H264Encoder(size.width,size.height,framerate);
                    encoder.startEncoder(); //开始编码

                }else{
                    video_H264.setText("结束 H624");
                    encoder.stopEncoder();
                    video_H264.setBackgroundColor(Color.rgb(255, 0, 0));
                    video_type = true;
                }
                break;
            case R.id.act_num:
                //create_num();
                break;

            default:
                break;
        }
    }


    @Override
    public void onCheckedChanged(CompoundButton checkBox, boolean check ) {

        switch (checkBox.getId()) {
            case R.id.system_info:
                if (check) {// 选中睡觉
                    device_info.setVisibility(View.VISIBLE);
                } else {
                    device_info.setVisibility(View.INVISIBLE);

                }
                break;
            default:
                break;
        }

    }

    private  void startPreview(){
        if(mCamera != null || !mIsSufaceCreated1){
            Log.d(TAG,"startPreview1 will return");
            return;
        }
        mCamera = Camera.open(1);
        Camera.Parameters parameters = mCamera.getParameters();
        //Camera.Size size = getBestPreviewSize(1080, 1920, parameters);//modify stephen
        Camera.Size size = getBestPreviewSize(720, 1080, parameters);
        if(size != null){
            parameters.setPreviewSize(size.width,size.height);
        }
        camera_width = size.width;
        camera_height = size.height;
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        parameters.setPreviewFrameRate(25);
        parameters.setPreviewFormat(ImageFormat.NV21); //设置数据格式
        parameters.setRecordingHint(true);
        mCamera.setDisplayOrientation(0);
        mCamera.setPreviewCallback(this);
        mCamera.setParameters(parameters);

        try {
            mCamera.setPreviewDisplay(mSurfaceHolder);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mCamera.startPreview();

    }

    private  void stopPreview(){
        if(mCamera != null){
            try {
                mCamera.setPreviewDisplay(null);
            } catch (IOException e) {
                e.printStackTrace();
            }
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }

    }


    private  Camera.Size getBestPreviewSize(int width, int height, Camera.Parameters parameters){
        Camera.Size result = null;

        for(Camera.Size size : parameters.getSupportedPreviewSizes()){
            if(size.width <= width && size.height <= height){
                if(result == null){
                    result = size;
                }else {
                    int resultArea = result.width * result.height;
                    int newArea = size.width * size.height;

                    if(newArea > resultArea){
                        result = size;
                    }
                }
            }
        }

        return result;
    }

    private SurfaceHolder.Callback mSurfaceCallback = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(SurfaceHolder surfaceHolder) {
            mIsSufaceCreated1 = true;
        }

        @Override
        public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
           // startPreview();//不自动回调预览
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
            mIsSufaceCreated1 = false;
        }
    };

    void goRequestPermissions(){
        if(ContextCompat.checkSelfPermission(this,Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED){
            ActivityCompat.requestPermissions(this,new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},1);

        }

        if(ContextCompat.checkSelfPermission(this,Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED){
            ActivityCompat.requestPermissions(this,new String[]{Manifest.permission.RECORD_AUDIO},1);
        }

        if(ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED){
            ActivityCompat.requestPermissions(this,new String[]{Manifest.permission.CAMERA},1);
        }
    }

    private boolean supportH264Codec() {
        // 遍历支持的编码格式信息,并查询有没有支持H.264(avc)的编码
        if (Build.VERSION.SDK_INT >= 18){
            //计算可用的编解码器数量
            int number = MediaCodecList.getCodecCount();
            for (int i=number-1 ; i >0 ; i--){
                //获得指定的编解码器信息
                MediaCodecInfo codecInfo = MediaCodecList.getCodecInfoAt(i);
                //得到支持的类型
                String[] types = codecInfo.getSupportedTypes();
                //查询有没有支持H.264(avc)的编码
                for (int j = 0 ; j < types.length ; j++){
                    if (types[j].equalsIgnoreCase("video/avc")){
                        return true;
                    }
                }
            }
        }
        return false;
    }



    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        /*Log.i("TAG", "surfaceDestroyed Medthod");
        //停止预览，并释放资源
        if (camera != null){
            camera.setPreviewCallback(null);
            camera.stopPreview();
            camera = null;
        }
        if (encoder != null){
            //停止编码
            encoder.stopEncoder();
        }*/
    }




    public void onPreviewFrame(byte[] bytes, Camera camera) {
        //返回相机预览的视频数据,并给H264Encoder编码压缩为H.264(avc)的文件test.mp4
        //这里面的Bytes的数据就是NV21格式的数据
        //Log.e("TAG" , "byte_leng="+bytes.length);

        if (encoder != null){
            encoder.putDate(bytes); //将一帧的数据传过去处理
            //encoder.encoderYUV420(bytes,mCamera);
        }
    }




    private void set_device_info() {

        StringBuilder s1 = new StringBuilder();
        s1.append("分辨率："+camera_width+" X "+camera_height+"\n");
        s1.append( "手机厂商：" + Device_info.getDeviceBrand()+"\n");
        s1.append( "手机型号：" + Device_info.getSystemModel()+"\n");
        s1.append( "手机当前系统语言：" + Device_info.getSystemLanguage()+"\n");
        s1.append( "Android系统版本号：" + Device_info.getSystemVersion()+"\n");
        s1.append( "手机IMEI：" + Device_info.getIMEI(getApplicationContext())+"\n");

        s1.append( "主板：" + Build.BOARD+"\n"); // 主板
        s1.append( "系统定制商：" + Build.BRAND+"\n"); // 系统定制商
        s1.append( "设备参数：" + Build.DEVICE+"\n"); // 设备参数
        s1.append( "显示屏参数：" + Build.DISPLAY+"\n"); // 显示屏参数
        s1.append( "唯一编号：" + Build.FINGERPRINT+"\n"); // 唯一编号
        s1.append( "硬件序列号：" + Build.SERIAL+"\n"); // 硬件序列号
        s1.append( "修订版本列表：" + Build.ID+"\n"); // 修订版本列表
        s1.append( "硬件制造商：" + Build.MANUFACTURER+"\n"); // 硬件制造商
        s1.append( "版本：" + Build.MODEL+"\n"); //版本
        s1.append( "硬件名：" + Build.HARDWARE+"\n"); //硬件名
        s1.append( "手机产品名：" + Build.PRODUCT+"\n"); //手机产品名
        s1.append( "描述build的标签：" + Build.TAGS+"\n"); // 描述build的标签
        s1.append( "Builder类型：" + Build.TYPE+"\n"); // Builder类型
        s1.append( "当前开发代号：" + Build.VERSION.CODENAME+"\n"); //当前开发代号
        s1.append( "源码控制版本号：" + Build.VERSION.INCREMENTAL+"\n"); //源码控制版本号
        s1.append( "版本字符串：" + Build.VERSION.RELEASE+"\n"); //版本字符串
        s1.append( "版本号：" + Build.VERSION.SDK_INT+"\n"); //版本号
        s1.append( "HOST值：" + Build.HOST+"\n"); // HOST值
        s1.append( "User名：" + Build.USER+"\n"); // User名
        s1.append( "编译时间：" + Build.TIME+"\n"); // 编译时间
        s1.append( "指令集名称：" + Build.CPU_ABI+"\n");
        s1.append( "CPU指令集：" + Build.SUPPORTED_ABIS+"\n"); //CPU指令集
        s1.append( "RADIO 固件版本号：" + Build.RADIO+"\n");
        s1.append( "设备版本类型 ：" + Build.TYPE+"\n");
        s1.append( "设备ID ：" + Build.ID+"\n");
        s1.append( "RESOURCES_SDK_INT ：" + Build.VERSION.PREVIEW_SDK_INT+"\n");
        s1.append( "SDK ：" + Build.VERSION.SDK+"\n");
        s1.append( "SDK_INT ：" + Build.VERSION.SDK_INT +"\n");
        s1.append( "安全补丁 ：" + Build.VERSION.SECURITY_PATCH+"\n");
        s1.append( "平台 ：" + Device_info.getHardWare()+"\n");
        s1.append( "内核版本 ：" + Device_info.getKernelVersion()+"\n");
        s1.append( "内核架构 ：" + Device_info.getKernelArchitecture()+"\n");
        s1.append( "虚拟机 ：" + Device_info.getJavaVM()+"\n");
        s1.append( "是否root ：" + Device_info.isRooted()+"\n");
        s1.append( "获得SD卡总大小 ：" + Device_info.getSDTotalSize(Camera2_Activity.this)+"\n");
        s1.append( "sd卡剩余容量 ：" + Device_info.getSDAvailableSize(Camera2_Activity.this)+"\n");
        s1.append( "ROM总大小 ：" + Device_info.getRomTotalSize(Camera2_Activity.this)+"\n");
        s1.append( "可用ROM ：" + Device_info.getRomAvailableSize(Camera2_Activity.this)+"\n");
        s1.append( "分辨率 ：" + Device_info.getWindowWidth(Camera2_Activity.this)+" X "+Device_info.getWindowHeight(Camera2_Activity.this)+"\n");






        device_info.setText(s1);
    }


}



























