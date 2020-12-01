package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.hardware.Camera;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Environment;
import android.os.PowerManager;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;

public class Camera4_Activity extends Activity implements View.OnClickListener {

    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Camera mCamera;
    private boolean mIsSufaceCreated1 = false;
    private boolean swith_camera = true;

    private Button cam_exit,cam_swith,cam_up,cam_down,btnStartStop;

    private MediaRecorder mRecorder;
    private static boolean isRecording = false;

    Camera.Size mCamera_size;


    private static  final  String TAG = "Camera4_Activity";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera4);

        goRequestPermissions();
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);

        mSurfaceView = (SurfaceView)findViewById(R.id.camera_test4);
        mSurfaceHolder = mSurfaceView.getHolder();
        mSurfaceHolder.addCallback(mSurfaceCallback);
        mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        cam_exit = (Button)findViewById(R.id.cam4_exit);
        cam_swith = (Button)findViewById(R.id.cam4_sw);
        cam_up = (Button)findViewById(R.id.cam4_up);
        cam_down = (Button)findViewById(R.id.cam4_down);
        btnStartStop = (Button)findViewById(R.id.StartStop);
        btnStartStop.setBackgroundColor(Color.rgb(213, 0, 0));


        cam_exit.setOnClickListener(this);
        cam_swith.setOnClickListener(this);
        cam_up.setOnClickListener(this);
        cam_down.setOnClickListener(this);
        btnStartStop.setOnClickListener(this);
        startPreview();
    }


    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.cam4_sw:
                if(swith_camera) {
                    swith_camera= false;
                    cam_swith.setText("camera 1 open");
                    startPreview();
                }else{
                    swith_camera=true;
                    cam_swith.setText("camera 1 close");
                    stopPreview();
                }
                break;
            case R.id.cam4_exit:
                finish();
                stopPreview();
                break;

            case R.id.cam4_up:
                //Intent intent2 = new Intent(MainActivity.this,KeyActivity.class);
                //startActivity(intent2);
                Toast.makeText(this, "#########################", Toast.LENGTH_LONG).show();
                break;

            case R.id.cam4_down:
                stopPreview();
                Intent intent = new Intent(Camera4_Activity.this,Camera3_Activity.class);
                startActivity(intent);
                break;
            case R.id.StartStop:
                isRecording=!isRecording;
                if (isRecording) {
                    startRecord();
                    btnStartStop.setBackgroundColor(Color.rgb(0, 213, 0));
                } else {
                    stopRecord();
                    btnStartStop.setBackgroundColor(Color.rgb(213, 0, 0));
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
        mCamera = Camera.open(3);
        Camera.Parameters parameters = mCamera.getParameters();
        //Camera.Size size = getBestPreviewSize(1080, 1920, parameters);//modify stephen
        mCamera_size = getBestPreviewSize(720, 1080, parameters);
        if(mCamera_size != null){
            parameters.setPreviewSize(mCamera_size.width,mCamera_size.height);
        }
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        parameters.setPreviewFrameRate(25);
        mCamera.setDisplayOrientation(0);
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
            startPreview();
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
            mIsSufaceCreated1 = false;
            stopPreview();
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










    /**
     * 开始录制
     */
    private void startRecord() {

    //创建MediaRecorder
        mRecorder = new MediaRecorder();
        mRecorder.reset();
        mCamera.unlock();
        mRecorder.setCamera(mCamera);
        Log.d(TAG,"startRecord will return");
        try {
            // 设置音频采集方式
            mRecorder.setAudioSource(MediaRecorder.AudioSource.CAMCORDER);
            //设置视频的采集方式
            mRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
            //设置文件的输出格式
            mRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);//aac_adif， aac_adts， output_format_rtp_avp， output_format_mpeg2ts ，webm
            //设置audio的编码格式
            mRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
            //设置video的编码格式
            mRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
            //设置录制的视频编码比特率
            mRecorder.setVideoEncodingBitRate(8*1024 * 1024);
            //设置录制的视频帧率,注意文档的说明:
            mRecorder.setVideoFrameRate(25);
            //设置要捕获的视频的宽度和高度
            mSurfaceHolder.setFixedSize(mCamera_size.width, mCamera_size.height);//最高只能设置640x480
            mRecorder.setVideoSize(mCamera_size.width, mCamera_size.height);//最高只能设置640x480


            //设置记录会话的最大持续时间（毫秒）
            mRecorder.setMaxDuration(60 * 1000);
            mRecorder.setPreviewDisplay(mSurfaceHolder.getSurface());
            String path = Environment.getExternalStorageDirectory().getAbsolutePath()+ File.separator+ System.currentTimeMillis() + ".mp4";
            File file = new File(path);
            if (file.exists()){
                file.delete();
            }

                //设置输出文件的路径
                mRecorder.setOutputFile(path);
                //准备录制
                mRecorder.prepare();
                //开始录制
                mRecorder.start();
                isRecording = true;
                btnStartStop.setText("停止");

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    /**
     * 停止录制
     */
    private void stopRecord() {
        try {
            //停止录制
            mRecorder.stop();
            //重置
            mRecorder.reset();
            btnStartStop.setText("开始");
        } catch (Exception e) {
            e.printStackTrace();
        }
        isRecording = false;
    }

    /**
     * 释放MediaRecorder
     */
    private void releaseMediaRecorder() {
        if (mRecorder != null) {
            mRecorder.release();
            mRecorder = null;
        }
    }

    /**
     * 释放相机资源
     */
    private void releaseCamera() {
        try {
            if (mCamera != null) {
                mCamera.stopPreview();
                mCamera.setPreviewCallback(null);
                mCamera.unlock();
                mCamera.release();
            }
        } catch (RuntimeException e) {
        } finally {
            mCamera = null;
        }
    }









}
