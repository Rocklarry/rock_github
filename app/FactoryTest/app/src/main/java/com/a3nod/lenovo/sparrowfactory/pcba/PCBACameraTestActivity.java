package com.a3nod.lenovo.sparrowfactory.pcba;

import android.content.Context;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.util.Size;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;


/**
 * Created by jianzhou.peng on 2018/1/10.
 */

public class PCBACameraTestActivity extends PCBABaseActivity
{
    public static final int MSG_TEST_SUCCESS = 0;
    public static final int MSG_TEST_FAIL = 1;
    private TextureView mTextureView;
    private TextView tv_test_show;
    public static final String TAG = "TestActivity";
    Handler mHandler;
    CaptureRequest.Builder mPreViewBuidler;
    CameraCaptureSession mCameraSession;
    CameraCharacteristics mCameraCharacteristics;
    private CameraDevice mCameraDevice;
    int mSensorOrientation;
    private Handler myHandler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case 0://打开相机回调 success
                    setResultText(true);
                    findViewById(R.id.btn_result_pass).setEnabled(true);
                    pcbaHandler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_SUCCESS, PCBAHandler.DELAY);
                    break;
                case 1:
                    setResultText(false);
                    findViewById(R.id.btn_result_pass).setEnabled(false);
                    pcbaHandler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_FAIL, PCBAHandler.DELAY);
                    break;
                default:
                    break;
            }
        }
    };
    private PCBAHandler pcbaHandler;
    HandlerThread thread;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_PCBA);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pcba_camera);

        pcbaHandler = new PCBAHandler(this);
        thread = new HandlerThread("Ceamera3");
        thread.start();
        mHandler = new Handler(thread.getLooper());

        initView();
        init();
        initConfirmButton();
    }

    private void initView()
    {
        mTextureView = findViewById(R.id.camera_show);
        tv_test_show = findViewById(R.id.tv_1);
    }

    public void initConfirmButton()
    {
        Button tv_pass = findViewById(R.id.btn_result_pass);
        tv_pass.setEnabled(false);
        tv_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_CAMERA, true, "007");
                finish();
            }
        });
        Button tv_reset = findViewById(R.id.btn_result_reset);
        tv_reset.setVisibility(View.GONE);
        Button tv_fail = findViewById(R.id.btn_result_fail);
        tv_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_CAMERA, false, "007");
                finish();
            }
        });
    }

    private void init()
    {
        //初始化相机布局
        mTextureView.setSurfaceTextureListener(mSurfacetextlistener);
    }

    public void setResultText(boolean isSuccess)
    {
        if (isSuccess)
        {
            tv_test_show.setVisibility(View.VISIBLE);
            tv_test_show.setTextSize(40);
            tv_test_show.setTextColor(Color.GREEN);
            tv_test_show.setText("PASS");
        } else
        {
            tv_test_show.setVisibility(View.VISIBLE);
            tv_test_show.setTextSize(40);
            tv_test_show.setTextColor(Color.RED);
            tv_test_show.setText("Fail");
        }
    }

    private Size mPreViewSize;
    private TextureView.SurfaceTextureListener mSurfacetextlistener = new TextureView.SurfaceTextureListener()
    {
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int i, int i1)
        {
//            mUIHandler.sendEmptyMessage(MSG_SET_ORIENTATION);
            openCamera();
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int i, int i1)
        {
//            mUIHandler.sendEmptyMessage(MSG_SET_ORIENTATION);
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture)
        {
            if(mCameraDevice != null){
                closePreviewSession();
                mCameraDevice.close();
            }
            return false;
        }

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture)
        {

        }
    };

    public void openCamera()
    {
        new Thread()
        {
            @Override
            public void run()
            {
                super.run();
                CameraManager manager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
                //String cameraid = CameraCharacteristics.LENS_FACING_FRONT + "";
                String[] camIds = {};
                try
                {
                    camIds = manager.getCameraIdList();
                } catch (CameraAccessException e)
                {
                    Log.d(TAG, "Cam access exception getting IDs", e);
                }
                if (camIds.length < 1)
                {
                    myHandler.sendEmptyMessage(MSG_TEST_FAIL); //打开相机失败
                    return;
                }
                String cameraid = camIds[0];
                Log.d(TAG, "Using camera id " + cameraid);

                try
                {
                    mCameraCharacteristics = manager.getCameraCharacteristics(cameraid);
                    mSensorOrientation = mCameraCharacteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);
                    LogUtil.i("sensor orientation");
                    StreamConfigurationMap map = mCameraCharacteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                    Size[] size = map.getOutputSizes(ImageFormat.JPEG);
                    for (Size s : size)
                    {
                        LogUtil.i("Size : width :" + s.getWidth() + " height : " + s.getHeight());
                    }

                    LogUtil.i("");
                    Size largest = Collections.max(Arrays.asList(map.getOutputSizes(ImageFormat.JPEG)), new Comparator<Size>()
                    {
                        @Override
                        public int compare(Size size, Size t1)
                        {
                            return Long.signum((long) size.getWidth() * size.getHeight() -
                                    (long) t1.getWidth() * t1.getHeight());
                        }
                    });
                    mPreViewSize = largest;
                    manager.openCamera(cameraid, cameraOpenCallBack, mHandler);
                } catch (CameraAccessException e)
                {
                    e.printStackTrace();
                }
            }
        }.start();
    }

    private CameraDevice.StateCallback cameraOpenCallBack = new CameraDevice.StateCallback()
    {
        @Override
        public void onOpened(CameraDevice cameraDevice)
        {
            //相机已经打开
            mCameraDevice = cameraDevice;
            myHandler.sendEmptyMessage(MSG_TEST_SUCCESS); //相机已经打开
            Log.e("TAG", "相机已经打开");
            try
            {

                mPreViewBuidler = cameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
                SurfaceTexture texture = mTextureView.getSurfaceTexture();
                if (texture != null)
                {
                    texture.setDefaultBufferSize(mPreViewSize.getWidth(), mPreViewSize.getHeight());
                    Surface surface = new Surface(texture);
                    mPreViewBuidler.addTarget(surface);
                    cameraDevice.createCaptureSession(Arrays.asList(surface), mSessionStateCallBack, mHandler);
                } else
                {
                    cameraDevice.close();
                }

            } catch (CameraAccessException e)
            {
                e.printStackTrace();
            }
        }

        @Override
        public void onDisconnected(CameraDevice cameraDevice)
        {
            Log.d(TAG, "相机连接断开");
            //myHandler.sendEmptyMessage(MSG_TEST_FAIL);
            // 相机断开连接
        }

        @Override
        public void onError(CameraDevice cameraDevice, int i)
        {
            myHandler.sendEmptyMessage(MSG_TEST_FAIL);
            //相机打开失败
        }
    };
    //相机会话的监听器，通过他得到mCameraSession对象，这个对象可以用来发送预览和拍照请求
    private CameraCaptureSession.StateCallback mSessionStateCallBack = new CameraCaptureSession.StateCallback()
    {
        @Override
        public void onConfigured(CameraCaptureSession cameraCaptureSession)
        {
            try
            {
                mCameraSession = cameraCaptureSession;
                cameraCaptureSession.setRepeatingRequest(mPreViewBuidler.build(), null, mHandler);
            } catch (CameraAccessException e)
            {
                e.printStackTrace();
            }
        }

        @Override
        public void onConfigureFailed(CameraCaptureSession cameraCaptureSession)
        {
            myHandler.sendEmptyMessage(MSG_TEST_FAIL);
        }
    };
    private void closePreviewSession()
    {
        if (mCameraSession != null)
        {
            try
            {
                mCameraSession.stopRepeating();
            } catch (CameraAccessException|IllegalStateException e)
            {
                e.printStackTrace();
            }
            mCameraSession.close();
            mCameraSession = null;
        }
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        pcbaHandler.removeCallbacksAndMessages(null);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        thread.quitSafely();
        myHandler.removeCallbacksAndMessages(null);
    }
}
