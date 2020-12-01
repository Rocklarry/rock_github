package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

import java.io.IOException;

public class Camera3_Activity extends Activity implements View.OnClickListener {

    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Camera mCamera;
    private boolean mIsSufaceCreated1 = false;
    private boolean swith_camera = true;

    private Button cam_exit,cam_swith,cam_up,cam_down;

    private static  final  String TAG = "Camera1_Activity";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera3);

        goRequestPermissions();
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);

        mSurfaceView = (SurfaceView) findViewById(R.id.camera_test3);
        mSurfaceHolder = mSurfaceView.getHolder();
        mSurfaceHolder.addCallback(mSurfaceCallback);
        mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        cam_exit = (Button) findViewById(R.id.cam3_exit);
        cam_swith = (Button) findViewById(R.id.cam3_sw);
        cam_up = (Button) findViewById(R.id.cam3_up);
        cam_down = (Button) findViewById(R.id.cam3_down);

        cam_exit.setOnClickListener(this);
        cam_swith.setOnClickListener(this);
        cam_up.setOnClickListener(this);
        cam_down.setOnClickListener(this);

        startPreview();
    }


    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.cam3_sw:
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
            case R.id.cam3_exit:
                finish();
                stopPreview();
                break;

            case R.id.cam3_up:
                stopPreview();
                Intent intent1 = new Intent(Camera3_Activity.this,Camera4_Activity.class);
                startActivity(intent1);
                break;

            case R.id.cam3_down:
                stopPreview();
                Intent intent2 = new Intent(Camera3_Activity.this,Camera2_Activity.class);
                startActivity(intent2);
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
        mCamera = Camera.open(2);
        Camera.Parameters parameters = mCamera.getParameters();
        //Camera.Size size = getBestPreviewSize(1080, 1920, parameters);//modify stephen
        Camera.Size size = getBestPreviewSize(720, 1080, parameters);
        if(size != null){
            parameters.setPreviewSize(size.width,size.height);
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
            //startPreview1();
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
}
