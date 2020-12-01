package com.example.myapplication;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.media.MediaRecorder;
import android.os.Bundle;


import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.os.Environment;
import android.os.Handler;
import android.os.PowerManager;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;

//public class MainActivity extends AppCompatActivity implements View.OnClickListener{
public class CameraActivity extends Activity implements View.OnClickListener{
    private SurfaceView mCameraPreview1,mCameraPreview2,mCameraPreview3,mCameraPreview4;
    private SurfaceHolder mSurfaceHolder1,mSurfaceHolder2,mSurfaceHolder3,mSurfaceHolder4;
    private ImageButton mShutter;
    private TextView mMinutePrefix;
    private TextView mMinuteText;
    private TextView mSecondPrefix;
    private  TextView mSecondText;
    private  String lastFileName;
    private Camera mCamera1,mCamera2,mCamera3,mCamera4;
    private MediaRecorder mRecorder;
    private PowerManager.WakeLock mWakeLock;
    private  final static int CAMERA_ID = 0;
    private boolean mIsRecording = false;

    private boolean mIsSufaceCreated1 = false;
    private boolean mIsSufaceCreated2 = false;
    private boolean mIsSufaceCreated3 = false;
    private boolean mIsSufaceCreated4 = false;

    private boolean btn_camera1 = true;
    private boolean btn_camera2 = true;
    private boolean btn_camera3 = true;
    private boolean btn_camera4 = true;

    private static  final  String TAG = "stephen";
    private Handler mHandler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);
        goRequestPermissions();
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        mMinutePrefix = (TextView) findViewById(R.id.timestamp_minute_prefix);
        mMinuteText = (TextView) findViewById(R.id.timestamp_minute_text);
        mSecondPrefix = (TextView) findViewById(R.id.timestamp_second_prefix);
        mSecondText = (TextView) findViewById(R.id.timestamp_second_text);

        mCameraPreview1 = (SurfaceView) findViewById(R.id.camera_preview1);
        mCameraPreview2 = (SurfaceView) findViewById(R.id.camera_preview2);
        mCameraPreview3 = (SurfaceView) findViewById(R.id.camera_preview3);
        mCameraPreview4 = (SurfaceView) findViewById(R.id.camera_preview4);

        mSurfaceHolder1 = mCameraPreview1.getHolder();
        mSurfaceHolder1.addCallback(mSurfaceCallback1);
        mSurfaceHolder1.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        mSurfaceHolder2 = mCameraPreview2.getHolder();
        mSurfaceHolder2.addCallback(mSurfaceCallback2);
        mSurfaceHolder2.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        mSurfaceHolder3 = mCameraPreview3.getHolder();
        mSurfaceHolder3.addCallback(mSurfaceCallback3);
        mSurfaceHolder3.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        mSurfaceHolder4 = mCameraPreview4.getHolder();
        mSurfaceHolder4.addCallback(mSurfaceCallback4);
        mSurfaceHolder4.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);


        mShutter = (ImageButton) findViewById(R.id.record_shutter);
        mShutter.setOnClickListener(this);



        Button btn1 = (Button) findViewById(R.id.button1);
        Button btn2 = (Button) findViewById(R.id.button2);
        Button btn3 = (Button) findViewById(R.id.button3);
        Button btn4 = (Button) findViewById(R.id.button4);

        btn1.setText("CLOSE");
        btn2.setText("CLOSE");
        btn3.setText("CLOSE");
        btn4.setText("CLOSE");


        btn1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(btn_camera1) {
                    Toast.makeText(CameraActivity.this, "camera 1 open", Toast.LENGTH_SHORT).show();
                    btn_camera1= false;
                    btn1.setText("camera 1 open");
                    startPreview1();
                }else{
                    Toast.makeText(CameraActivity.this, "camera 1 close", Toast.LENGTH_SHORT).show();
                    btn_camera1=true;
                    btn1.setText("camera 1 close");
                    stopPreview1();
                }
            }
        });

        btn2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(btn_camera2) {
                    Toast.makeText(CameraActivity.this, "camera 2 open", Toast.LENGTH_SHORT).show();
                    btn_camera2= false;
                    btn2.setText("camera 2 open");
                    startPreview2();
                }else{
                    Toast.makeText(CameraActivity.this, "camera 2 close", Toast.LENGTH_SHORT).show();
                    btn_camera2=true;
                    btn2.setText("camera 2 close");
                    stopPreview2();
                }
            }
        });

        btn3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(btn_camera3) {
                    Toast.makeText(CameraActivity.this, "camera 3 open", Toast.LENGTH_SHORT).show();
                    btn_camera3= false;
                    btn3.setText("camera 3 open");
                    startPreview3();
                }else{
                    Toast.makeText(CameraActivity.this, "camera 3 close", Toast.LENGTH_SHORT).show();
                    btn_camera3=true;
                    btn3.setText("camera 3 close");
                    stopPreview3();
                }
            }
        });

        btn4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(btn_camera4) {
                    Toast.makeText(CameraActivity.this, "camera 4 open", Toast.LENGTH_SHORT).show();
                    btn_camera4= false;
                    btn4.setText("camera 4 open");
                    startPreview4();
                }else{
                    Toast.makeText(CameraActivity.this, "camera 4 close", Toast.LENGTH_SHORT).show();
                    btn_camera4=true;
                    btn4.setText("camera 4 close");
                    stopPreview4();
                }
            }
        });

        mHandler.postDelayed(mTimestampRunnable, 1000);
    }


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

    @Override
    protected void onPause() {
        super.onPause();
        if(mIsRecording){
            stopRecording();
        }
        stopPreview1();
        stopPreview2();
        stopPreview3();
        stopPreview4();
    }

    @Override
    protected void onResume() {
        super.onResume();
        //startPreview1();
        //startPreview2();
        //startPreview3();
        //startPreview4();
    }

    private  void  stopRecording(){
        if(mCamera1 != null){
            mCamera1.lock();
        }
        if(mRecorder != null){
            mRecorder.stop();
            mRecorder.release();
            mRecorder = null;
        }
        mShutter.setImageDrawable(getResources().getDrawable(R.mipmap.recording_shutter));
        mIsRecording = false;
        mHandler.removeCallbacks(mTimestampRunnable);
        if(null != lastFileName && !"".equals(lastFileName)){
            //modify stephen
            /*
            File f = new File(lastFileName);
            String name = f.getName().substring(0,f.getName().lastIndexOf(".mp4"));
            name += "_"  + getRecordTime() + ".mp4";
            String newPath = f.getParentFile().getAbsolutePath() + "/"
                    + name;
            Log.d(TAG,"stopRecording lastFileName:" + lastFileName + "newPath" + newPath);
            if(f.renameTo(new File(newPath))){
                int i = 0;
                i++;
            }

             */
        }
       // Log.d(TAG,"stopRecording lastFileName:" + lastFileName + "newPath" + newPath);
        mMinutePrefix.setVisibility(View.VISIBLE);
        mMinuteText.setText("0");
        mSecondPrefix.setVisibility(View.VISIBLE);
        mSecondText.setText("0");

        startPreview1();
        mWakeLock.release();

    }

    private Runnable mTimestampRunnable = new Runnable() {
        @Override
        public void run() {
            updateTimestamp();
            mHandler.postDelayed(this,1000);
        }
    };



    private String getRecordTime(){
        int second = Integer.parseInt(mSecondText.getText().toString());
        int mimute = Integer.parseInt(mMinuteText.getText().toString());
        if(mimute > 0){
            return mimute + "m" + second + "s";

        }
        return  second + "s";
    }

    private  void updateTimestamp(){
        int second = Integer.parseInt(mSecondText.getText().toString());
        int minute = Integer.parseInt(mMinuteText.getText().toString());
        second++;
        Log.d(TAG,"second" + second);
        if(second < 10){
            mSecondText.setText(String.valueOf(second));
        }else if(second >= 10 && second < 60){
            mSecondText.setVisibility(View.GONE);
            mSecondText.setText(String.valueOf(second));
        }else if(second >= 60){
            mSecondPrefix.setVisibility(View.VISIBLE);
            mSecondText.setText("0");
            minute++;
            mMinuteText.setText(String.valueOf(minute));
        }else if(minute >= 10){
            mMinutePrefix.setVisibility(View.GONE);
        }
    }




    private SurfaceHolder.Callback mSurfaceCallback1 = new SurfaceHolder.Callback() {
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

    private SurfaceHolder.Callback mSurfaceCallback2 = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(SurfaceHolder surfaceHolder) {
            mIsSufaceCreated2 = true;
        }

        @Override
        public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
            //startPreview2();
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
            mIsSufaceCreated2 = false;
        }
    };

    private SurfaceHolder.Callback mSurfaceCallback3 = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(SurfaceHolder surfaceHolder) {
            mIsSufaceCreated3 = true;
        }

        @Override
        public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
            //startPreview3();
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
            mIsSufaceCreated3 = false;
        }
    };


    private SurfaceHolder.Callback mSurfaceCallback4 = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(SurfaceHolder surfaceHolder) {
            mIsSufaceCreated4 = true;
        }

        @Override
        public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
            //startPreview4();
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
            mIsSufaceCreated4 = false;
        }
    };

    private  void startPreview1(){
        if(mCamera1 != null || !mIsSufaceCreated1){
            Log.d(TAG,"startPreview1 will return");
            return;
        }
        mCamera1 = Camera.open(0);
        Camera.Parameters parameters = mCamera1.getParameters();
        //Camera.Size size = getBestPreviewSize(1080, 1920, parameters);//modify stephen
        Camera.Size size = getBestPreviewSize(720, 1080, parameters);
        if(size != null){
            parameters.setPreviewSize(size.width,size.height);
        }
        parameters.setPreviewSize(768, 432);
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        parameters.setPreviewFrameRate(25);
        parameters.setRecordingHint(true);
        mCamera1.setDisplayOrientation(0);
        mCamera1.setParameters(parameters);
        try {
            mCamera1.setPreviewDisplay(mSurfaceHolder1);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mCamera1.startPreview();
    }


    private  void startPreview2(){
        if(mCamera2 != null || !mIsSufaceCreated2){
            Log.d(TAG,"startPreview2 will return");
            return;
        }
        mCamera2 = Camera.open(1);
        Camera.Parameters parameters = mCamera2.getParameters();
        //Camera.Size size = getBestPreviewSize(1080, 1920, parameters);//modify stephen
        Camera.Size size = getBestPreviewSize(720, 1080, parameters);
        if(size != null){
            parameters.setPreviewSize(size.width,size.height);
        }
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        parameters.setPreviewFrameRate(25);
        parameters.setRecordingHint(true);
        mCamera2.setDisplayOrientation(0);
        mCamera2.setParameters(parameters);
        try {
            mCamera2.setPreviewDisplay(mSurfaceHolder2);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mCamera2.startPreview();
    }


    private  void startPreview3(){
        if(mCamera3 != null || !mIsSufaceCreated3){
            Log.d(TAG,"startPreview3 will return");
            return;
        }
        mCamera3 = Camera.open(2);
        Camera.Parameters parameters = mCamera3.getParameters();
        //Camera.Size size = getBestPreviewSize(1080, 1920, parameters);//modify stephen
        Camera.Size size = getBestPreviewSize(768, 432, parameters);
        if(size != null){
            parameters.setPreviewSize(size.width,size.height);
        }
        parameters.setPreviewSize(768, 432);
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        parameters.setPreviewFrameRate(15);
        parameters.setRecordingHint(true);
        mCamera3.setDisplayOrientation(0);
        mCamera3.setParameters(parameters);
        try {
            mCamera3.setPreviewDisplay(mSurfaceHolder3);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mCamera3.startPreview();
    }


    private  void startPreview4(){
        if(mCamera4 != null || !mIsSufaceCreated4){
            Log.d(TAG,"startPreview4 will return");
            return;
        }
        mCamera4 = Camera.open(3);
        Camera.Parameters parameters = mCamera4.getParameters();
        //Camera.Size size = getBestPreviewSize(1080, 1920, parameters);//modify stephen
        Camera.Size size = getBestPreviewSize(1080, 720, parameters);
        if(size != null){
            parameters.setPreviewSize(size.width,size.height);
        }
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        parameters.setPreviewFrameRate(25);
        parameters.setRecordingHint(true);
        mCamera4.setDisplayOrientation(0);
        mCamera4.setParameters(parameters);
        try {
            mCamera4.setPreviewDisplay(mSurfaceHolder4);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mCamera4.startPreview();
    }

    private  void stopPreview1(){
        if(mCamera1 != null){
            try {
                mCamera1.setPreviewDisplay(null);
            } catch (IOException e) {
                e.printStackTrace();
            }
            mCamera1.stopPreview();
            mCamera1.release();
            mCamera1 = null;
        }

    }

    private  void stopPreview2(){
        if(mCamera2 != null){
            try {
                mCamera2.setPreviewDisplay(null);
            } catch (IOException e) {
                e.printStackTrace();
            }
            mCamera2.stopPreview();
            mCamera2.release();
            mCamera2 = null;
        }

    }
    private  void stopPreview3(){
        if(mCamera3 != null){
            try {
                mCamera3.setPreviewDisplay(null);
            } catch (IOException e) {
                e.printStackTrace();
            }
            mCamera3.stopPreview();
            mCamera3.release();
            mCamera3 = null;
        }

    }
    private  void stopPreview4(){
        if(mCamera4 != null){
            try {
                mCamera4.setPreviewDisplay(null);
            } catch (IOException e) {
                e.printStackTrace();
            }
            mCamera4.stopPreview();
            mCamera4.release();
            mCamera4 = null;
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

    @Override
    public void onClick(View v) {
        if (mIsRecording) {
            stopRecording();
        } else {
            initMediaRecorder();
            startRecording();
            //add stephen ++
            mRecorder.setOnInfoListener(
                    new MediaRecorder.OnInfoListener() {
                        @Override
                        public void onInfo(MediaRecorder mediaRecorder, int what, int extra) {
                            switch (what){
                                case MediaRecorder.MEDIA_RECORDER_INFO_MAX_DURATION_APPROACHING:
                                    Log.d(TAG,"MEDIA_RECORDER_INFO_MAX_DURATION_APPROACHING");

                                    File file = new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MOVIES)
                                            + File.separator + "VideoRecorder");
                                    lastFileName = file.getPath() + File.separator + "VID_" + System.currentTimeMillis() + ".mp4";
                                    Log.d(TAG,"setOnInfoListener lastFileName:" + lastFileName );


                                   // mRecorder.setNextOutputFile();

                                    try {
                                       RandomAccessFile f = new RandomAccessFile(lastFileName,"rws");

                                        FileDescriptor fd = f.getFD();
                                        mRecorder.setNextOutputFile(fd);
                                    } catch (FileNotFoundException e) {
                                        e.printStackTrace();
                                    } catch (IOException e) {
                                        e.printStackTrace();
                                    }


                                    break;
                                case MediaRecorder.MEDIA_RECORDER_INFO_MAX_DURATION_REACHED:
                                    Log.d(TAG,"MEDIA_RECORDER_INFO_MAX_DURATION_REACHED");
                                    break;
                                case MediaRecorder.MEDIA_RECORDER_INFO_MAX_FILESIZE_APPROACHING:
                                    Log.d(TAG,"MEDIA_RECORDER_INFO_MAX_FILESIZE_APPROACHING");
                                    break;
                                case MediaRecorder.MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED:
                                    Log.d(TAG,"MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED");
                                    break;
                                case MediaRecorder.MEDIA_RECORDER_INFO_NEXT_OUTPUT_FILE_STARTED:
                                    Log.d(TAG,"MEDIA_RECORDER_INFO_NEXT_OUTPUT_FILE_STARTED");
                                    break;
                                 default:

                            }
                        }
                    }
            );

            //add stephen ---

            //开始录像后，每隔1s去更新录像的时间戳
            mHandler.postDelayed(mTimestampRunnable, 1000);
        }
    }

    private  void  initMediaRecorder(){
        mRecorder = new MediaRecorder();
        mCamera1.unlock();
        mRecorder.setCamera(mCamera1);
        mRecorder.setOrientationHint(90);
        mRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        mRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
        mRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
        mRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.DEFAULT);
        mRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
        mRecorder.setVideoEncodingBitRate(3 * 1024 * 1024);
        mRecorder.setVideoSize(1280,720);
        mRecorder.setVideoFrameRate(25);
        mRecorder.setMaxDuration(15000);
        mRecorder.setPreviewDisplay(mSurfaceHolder1.getSurface());
        File file = new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MOVIES)
                    + File.separator + "VideoRecorder");
        lastFileName = file.getPath() + File.separator + "VID_" + System.currentTimeMillis() + ".mp4";
        Log.d(TAG,"initMediaRecorder lastFileName:" + lastFileName );

        mRecorder.setOutputFile(lastFileName);
    }

    private  void  startRecording(){
        if(mRecorder != null){
            try {
                mRecorder.prepare();
                //add stephen
                //add --
                mRecorder.start();
            } catch (IOException e) {
                mIsRecording = false;
                e.printStackTrace();
            }
        }
        mShutter.setImageDrawable(getResources().getDrawable(R.mipmap.recording_shutter_hl));
        mIsRecording = true;
        mWakeLock.acquire();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }




    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
