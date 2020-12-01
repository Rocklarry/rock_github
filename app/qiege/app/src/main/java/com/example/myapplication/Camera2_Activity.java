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
    int framerate = 15; //һ��30֡
    H264Encoder encoder; //�Զ���ı��������

    private boolean video_type=true;

    AudioManager mAudioManager;
    private static  int camera_width,camera_height;

    private DecimalFormat df;

    private Paint p;
    public String[] chars = new String[]{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "-", ":", " ","��","��","��"};
    public Map<String, byte[]> map = new HashMap<>();


    private Handler handler = new Handler();
    private Runnable task = new Runnable() {
        public void run() {
            // TODO Auto-generated method stub
            handler.postDelayed(this,500);//����ѭ��ʱ�䣬�˴���500ms��
            //��Ҫִ�еĴ���
            count++;
            cpu_info.setText("CPUʹ����: "+df.format(Device_info.getCpuUsed()*100)+"%"+"\n");
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
        handler.postDelayed(task,100);//�ӳٵ���

        //startPreview();

        set_device_info();

        if (supportH264Codec()){ //��ѯ�ֻ��Ƿ�֧��AVC����
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

                    //handler.postDelayed(task,500);//�ӳٵ���
                    //handler.post(task);//��������

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
                    video_H264.setText("��ʼ H624");
                    video_type = false;
                    video_H264.setBackgroundColor(Color.rgb(0, 255, 0));
                    Camera.Parameters parameters = mCamera.getParameters();
                    Camera.Size size = parameters.getPreviewSize();
                    encoder = new H264Encoder(size.width,size.height,framerate);
                    encoder.startEncoder(); //��ʼ����

                }else{
                    video_H264.setText("���� H624");
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
                if (check) {// ѡ��˯��
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
        parameters.setPreviewFormat(ImageFormat.NV21); //�������ݸ�ʽ
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
           // startPreview();//���Զ��ص�Ԥ��
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
        // ����֧�ֵı����ʽ��Ϣ,����ѯ��û��֧��H.264(avc)�ı���
        if (Build.VERSION.SDK_INT >= 18){
            //������õı����������
            int number = MediaCodecList.getCodecCount();
            for (int i=number-1 ; i >0 ; i--){
                //���ָ���ı��������Ϣ
                MediaCodecInfo codecInfo = MediaCodecList.getCodecInfoAt(i);
                //�õ�֧�ֵ�����
                String[] types = codecInfo.getSupportedTypes();
                //��ѯ��û��֧��H.264(avc)�ı���
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
        //ֹͣԤ�������ͷ���Դ
        if (camera != null){
            camera.setPreviewCallback(null);
            camera.stopPreview();
            camera = null;
        }
        if (encoder != null){
            //ֹͣ����
            encoder.stopEncoder();
        }*/
    }




    public void onPreviewFrame(byte[] bytes, Camera camera) {
        //�������Ԥ������Ƶ����,����H264Encoder����ѹ��ΪH.264(avc)���ļ�test.mp4
        //�������Bytes�����ݾ���NV21��ʽ������
        //Log.e("TAG" , "byte_leng="+bytes.length);

        if (encoder != null){
            encoder.putDate(bytes); //��һ֡�����ݴ���ȥ����
            //encoder.encoderYUV420(bytes,mCamera);
        }
    }




    private void set_device_info() {

        StringBuilder s1 = new StringBuilder();
        s1.append("�ֱ��ʣ�"+camera_width+" X "+camera_height+"\n");
        s1.append( "�ֻ����̣�" + Device_info.getDeviceBrand()+"\n");
        s1.append( "�ֻ��ͺţ�" + Device_info.getSystemModel()+"\n");
        s1.append( "�ֻ���ǰϵͳ���ԣ�" + Device_info.getSystemLanguage()+"\n");
        s1.append( "Androidϵͳ�汾�ţ�" + Device_info.getSystemVersion()+"\n");
        s1.append( "�ֻ�IMEI��" + Device_info.getIMEI(getApplicationContext())+"\n");

        s1.append( "���壺" + Build.BOARD+"\n"); // ����
        s1.append( "ϵͳ�����̣�" + Build.BRAND+"\n"); // ϵͳ������
        s1.append( "�豸������" + Build.DEVICE+"\n"); // �豸����
        s1.append( "��ʾ��������" + Build.DISPLAY+"\n"); // ��ʾ������
        s1.append( "Ψһ��ţ�" + Build.FINGERPRINT+"\n"); // Ψһ���
        s1.append( "Ӳ�����кţ�" + Build.SERIAL+"\n"); // Ӳ�����к�
        s1.append( "�޶��汾�б�" + Build.ID+"\n"); // �޶��汾�б�
        s1.append( "Ӳ�������̣�" + Build.MANUFACTURER+"\n"); // Ӳ��������
        s1.append( "�汾��" + Build.MODEL+"\n"); //�汾
        s1.append( "Ӳ������" + Build.HARDWARE+"\n"); //Ӳ����
        s1.append( "�ֻ���Ʒ����" + Build.PRODUCT+"\n"); //�ֻ���Ʒ��
        s1.append( "����build�ı�ǩ��" + Build.TAGS+"\n"); // ����build�ı�ǩ
        s1.append( "Builder���ͣ�" + Build.TYPE+"\n"); // Builder����
        s1.append( "��ǰ�������ţ�" + Build.VERSION.CODENAME+"\n"); //��ǰ��������
        s1.append( "Դ����ư汾�ţ�" + Build.VERSION.INCREMENTAL+"\n"); //Դ����ư汾��
        s1.append( "�汾�ַ�����" + Build.VERSION.RELEASE+"\n"); //�汾�ַ���
        s1.append( "�汾�ţ�" + Build.VERSION.SDK_INT+"\n"); //�汾��
        s1.append( "HOSTֵ��" + Build.HOST+"\n"); // HOSTֵ
        s1.append( "User����" + Build.USER+"\n"); // User��
        s1.append( "����ʱ�䣺" + Build.TIME+"\n"); // ����ʱ��
        s1.append( "ָ����ƣ�" + Build.CPU_ABI+"\n");
        s1.append( "CPUָ���" + Build.SUPPORTED_ABIS+"\n"); //CPUָ�
        s1.append( "RADIO �̼��汾�ţ�" + Build.RADIO+"\n");
        s1.append( "�豸�汾���� ��" + Build.TYPE+"\n");
        s1.append( "�豸ID ��" + Build.ID+"\n");
        s1.append( "RESOURCES_SDK_INT ��" + Build.VERSION.PREVIEW_SDK_INT+"\n");
        s1.append( "SDK ��" + Build.VERSION.SDK+"\n");
        s1.append( "SDK_INT ��" + Build.VERSION.SDK_INT +"\n");
        s1.append( "��ȫ���� ��" + Build.VERSION.SECURITY_PATCH+"\n");
        s1.append( "ƽ̨ ��" + Device_info.getHardWare()+"\n");
        s1.append( "�ں˰汾 ��" + Device_info.getKernelVersion()+"\n");
        s1.append( "�ں˼ܹ� ��" + Device_info.getKernelArchitecture()+"\n");
        s1.append( "����� ��" + Device_info.getJavaVM()+"\n");
        s1.append( "�Ƿ�root ��" + Device_info.isRooted()+"\n");
        s1.append( "���SD���ܴ�С ��" + Device_info.getSDTotalSize(Camera2_Activity.this)+"\n");
        s1.append( "sd��ʣ������ ��" + Device_info.getSDAvailableSize(Camera2_Activity.this)+"\n");
        s1.append( "ROM�ܴ�С ��" + Device_info.getRomTotalSize(Camera2_Activity.this)+"\n");
        s1.append( "����ROM ��" + Device_info.getRomAvailableSize(Camera2_Activity.this)+"\n");
        s1.append( "�ֱ��� ��" + Device_info.getWindowWidth(Camera2_Activity.this)+" X "+Device_info.getWindowHeight(Camera2_Activity.this)+"\n");






        device_info.setText(s1);
    }


}



























