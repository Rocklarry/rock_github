package com.a3nod.lenovo.sparrowfactory.runin.camera;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.ImageReader;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.util.Size;
import android.util.SparseIntArray;
import android.view.Surface;
import android.view.TextureView;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;
import com.a3nod.lenovo.sparrowfactory.runin.RuninConfig;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;
import com.a3nod.lenovo.sparrowfactory.tool.ThreadPoolUtils;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;


public class CameraTestActivity extends RuninBaseActivity
{

    public static final String TAG = "CameraTestActivity";

    TextView tv_status;
    CameraHandler handler;
    private static final int MSG_OPEN_CAMERA = 13;
    private static final int MSG_TAKE_PICTURE = 14;
    private static final int MSG_RELEASE_CAMERA = 15;
    private static final int MSG_RECORD_VIDEO = 16;

    private final int MAX_RECORDE = 60 * 1000;
    private final int MIN_RECORDE = 2 * 1000;
    private final int CHANGE_TIME = 1 * 1000;
    private int recordTime = 10 * 1000;


    private TextureView mTextureView;
    private ImageReader mImageReader;
    private CaptureRequest.Builder mPreViewBuidler;
    private CameraCaptureSession mPreviewSession;
    private CameraDevice mCameraDevice;
    private Handler threadHandler;
    private Size mPreViewSize;

    private String mNextVideoAbsolutePath = Constant.RUNIN_DIR + "/RuninCameraTest.mp4";
    private MediaRecorder mMediaRecorder;
    boolean isRecording = false;
    boolean isPreviewing= false;

    private long endTime;

    private int mSensorOrientation;
    private static final int SENSOR_ORIENTATION_DEFAULT_DEGREES = 90;
    private static final int SENSOR_ORIENTATION_INVERSE_DEGREES = 270;

    private static final SparseIntArray DEFAULT_ORIENTATIONS = new SparseIntArray();
    private static final SparseIntArray INVERSE_ORIENTATIONS = new SparseIntArray();

    static
    {
        DEFAULT_ORIENTATIONS.append(Surface.ROTATION_0, 90);
        DEFAULT_ORIENTATIONS.append(Surface.ROTATION_90, 0);
        DEFAULT_ORIENTATIONS.append(Surface.ROTATION_180, 270);
        DEFAULT_ORIENTATIONS.append(Surface.ROTATION_270, 180);
    }

    static
    {
        INVERSE_ORIENTATIONS.append(Surface.ROTATION_0, 270);
        INVERSE_ORIENTATIONS.append(Surface.ROTATION_90, 180);
        INVERSE_ORIENTATIONS.append(Surface.ROTATION_180, 90);
        INVERSE_ORIENTATIONS.append(Surface.ROTATION_270, 0);
    }

    HandlerThread handlerThread;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_RUNIN);
        setContentView(R.layout.activity_runin_camera);

        testTime = config.getItemDuration(RuninConfig.RUNIN_CAMERA_ID);
        endTime = System.currentTimeMillis() + testTime;
        LogUtil.i("test time : " + testTime);
        if (testTime > MAX_RECORDE)
        {
            recordTime = MAX_RECORDE;
        } else
        {
            testTime = testTime < MIN_RECORDE ? MIN_RECORDE : testTime;
            recordTime = testTime;
        }



        handler = new CameraHandler(this);

        handlerThread = new HandlerThread("Camera3");
        handlerThread.start();
        threadHandler = new Handler(handlerThread.getLooper());

        tv_status = findViewById(R.id.tv_camera_status);
        tv_status.setText("opening camera...");
        mTextureView = findViewById(R.id.camera_show);
        mTextureView.setSurfaceTextureListener(mSurfacetextlistener);

        if (isErrorRestart)
        {
            String errInfo = getIntent().getStringExtra(Constant.INTENT_KEY_ERROR_MSG);
            SPUtils.put(this, Constant.SP_KEY_CAMERA_TEST_REMARK, errInfo);
            mTestSuccess = false;
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        }
    }


    private void configureTransform(int viewWidth, int viewHeight)
    {
        if (null == mTextureView || null == mPreViewSize)
        {
            return;
        }
        int rotation = getWindowManager().getDefaultDisplay().getRotation();
        LogUtil.i("rotation : " + rotation);
        rotation = 1;
        Matrix matrix = new Matrix();
        RectF viewRect = new RectF(0, 0, viewWidth, viewHeight);
        RectF bufferRect = new RectF(0, 0, mPreViewSize.getHeight(), mPreViewSize.getWidth());
        float centerX = viewRect.centerX();
        float centerY = viewRect.centerY();
        if (Surface.ROTATION_90 == rotation || Surface.ROTATION_270 == rotation)
        {
            bufferRect.offset(centerX - bufferRect.centerX(), centerY - bufferRect.centerY());
            matrix.setRectToRect(viewRect, bufferRect, Matrix.ScaleToFit.FILL);
            float scale = Math.max(
                    (float) viewHeight / mPreViewSize.getHeight(),
                    (float) viewWidth / mPreViewSize.getWidth());
            matrix.postScale(scale, scale, centerX, centerY);
            matrix.postRotate(90 * (rotation - 2), centerX, centerY);
        } else if (Surface.ROTATION_180 == rotation)
        {
            matrix.postRotate(180, centerX, centerY);
        }
        mTextureView.setTransform(matrix);
    }

    private TextureView.SurfaceTextureListener mSurfacetextlistener = new TextureView.SurfaceTextureListener()
    {
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int i, int i1)
        {
            LogUtil.i("Runin camera test onSurfaceTextureAvailable");
            if (!isErrorRestart)
            {
                handler.sendEmptyMessage(MSG_OPEN_CAMERA);
            }
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int width, int height)
        {
            LogUtil.i("Runin camera test onSurfaceTextureSizeChanged");
//            configureTransform(width, height);
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture)
        {
            LogUtil.i("Runin camera test onSurfaceTextureDestroyed");
//            handler.sendEmptyMessageDelayed(MSG_RELEASE_CAMERA,2000);
            return true;
        }

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture)
        {
//            LogUtil.i("Runin camera test onSurfaceTextureUpdated");
        }
    };

    private void stopAndCloseCamera()
    {
/*        if (isRecording && mMediaRecorder != null)
        {
            Toast.makeText(CameraTestActivity.this, "stopRecord", Toast.LENGTH_SHORT).show();
            mMediaRecorder.stop();
            mMediaRecorder.release();
            mMediaRecorder = null;
            isRecording = false;
        }*/
        LogUtil.i("stopAndCloseCamera");
        boolean isStopRepeatSuccess=true;
        if (mPreviewSession != null)
        {
            try
            {
                mPreviewSession.stopRepeating();
                mPreviewSession.close();
                mPreviewSession = null;
                isStopRepeatSuccess=true;
            } catch (CameraAccessException e)
            {
                isStopRepeatSuccess=false;
                e.printStackTrace();
            }
        }
        LogUtil.i("isStopRepeatSuccess "+ isStopRepeatSuccess);

        if (mCameraDevice != null&&isStopRepeatSuccess)
        {
            Log.d(TAG, "mCameraDevice.close()");
            mCameraDevice.close();
            mCameraDevice = null;
        }
        LogUtil.i("stopRecord");
        if (mMediaRecorder != null) {
            Toast.makeText(CameraTestActivity.this, "stopRecord", Toast.LENGTH_SHORT).show();
            LogUtil.i("mMediaRecorder!=null");
            try {
                //设置后不会崩
                mMediaRecorder.setOnErrorListener(null);
                mMediaRecorder.setPreviewDisplay(null);
                mMediaRecorder.setOnInfoListener(null);
                LogUtil.i("setOnInfoListener null");
                mMediaRecorder.stop();
                LogUtil.i("mMediaRecorder.stop");
                mMediaRecorder.release();
                LogUtil.i("mMediaRecorder.release");
            } catch (IllegalStateException e) {
                e.printStackTrace();
            } catch (RuntimeException e) {
                e.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
            }
            mMediaRecorder = null;
            isRecording = false;
        }

    }

    private CameraCaptureSession.StateCallback mSessionStateCallBack = new CameraCaptureSession.StateCallback()
    {
        @Override
        public void onConfigured(CameraCaptureSession cameraCaptureSession)
        {
            try
            {
                isPreviewing=true;
                mPreviewSession = cameraCaptureSession;
                mPreViewBuidler.set(CaptureRequest.CONTROL_MODE, CameraMetadata.CONTROL_MODE_AUTO);
                cameraCaptureSession.setRepeatingRequest(mPreViewBuidler.build(), null, threadHandler);
                isRecording = true;
                mMediaRecorder.start();
                handler.sendEmptyMessageDelayed(MSG_RELEASE_CAMERA, recordTime);
            } catch (CameraAccessException e)
            {
                e.printStackTrace();
                isPreviewing=false;
                mTestSuccess = false;
                handler.sendEmptyMessage(MSG_TEST_FINISH);
            }
        }

        @Override
        public void onConfigureFailed(CameraCaptureSession cameraCaptureSession)
        {
            Log.d(TAG, "onConfigureFailed");
            isPreviewing=false;
        }
    };
    private CameraDevice.StateCallback cameraOpenCallBack = new CameraDevice.StateCallback()
    {
        @Override
        public void onOpened(CameraDevice cameraDevice)
        {
            mCameraDevice = cameraDevice;
            Log.d(TAG, "camera is open");
            handler.sendEmptyMessage(MSG_RECORD_VIDEO);

        }

        @Override
        public void onDisconnected(CameraDevice cameraDevice)
        {
            Log.d(TAG, "camera is disconnected");
        }

        @Override
        public void onError(CameraDevice cameraDevice, int i)
        {
            Log.d(TAG, "camera open fail"+ i);
        }
    };
    private ImageReader.OnImageAvailableListener onImageAvaiableListener = new ImageReader.OnImageAvailableListener()
    {
        @Override
        public void onImageAvailable(ImageReader imageReader)
        {
        }
    };
    @Override
    public void onPause() {
        super.onPause();
        stopAndCloseCamera();
        handler.removeCallbacksAndMessages(null);
        handlerThread.quitSafely();
        threadHandler.removeCallbacksAndMessages(null);
        threadHandler = null;
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();

    }

    @SuppressLint("HandlerLeak")
    static class CameraHandler extends Handler
    {
        WeakReference<CameraTestActivity> wra;

        public CameraHandler(CameraTestActivity activity)
        {
            wra = new WeakReference<>(activity);
        }

        public void handleMessage(Message msg)
        {
            if (wra.get() == null)
            {
                return;
            }
            switch (msg.what)
            {
                case MSG_OPEN_CAMERA:
                    wra.get().openCamera();
                    break;
                case MSG_TAKE_PICTURE:
                    wra.get().takePicture();
                    break;
                case MSG_RECORD_VIDEO:
                    wra.get().startRecord();
                    break;
                case MSG_RELEASE_CAMERA:
                    wra.get().releaseCamera();
                    break;
                case MSG_TEST_FINISH:
                    wra.get().testFinish();
                    break;
            }
            super.handleMessage(msg);
        }
    }

    public void openCamera()
    {
        if (System.currentTimeMillis() < endTime)
        {
            tv_status.setText("camera test --> open");

            //初始化相机布局
            CameraManager manager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);

            String[] camIds = null;
            try
            {
                camIds = manager.getCameraIdList();
            } catch (CameraAccessException e)
            {
                e.printStackTrace();
                Log.d(TAG, "Camera access exception getting IDs", e);
            }

            //获取不到可用相机
            if (camIds == null || camIds.length < 1)
            {
                mTestSuccess = false;
                handler.sendEmptyMessage(MSG_TEST_FINISH);
                SPUtils.put(this, Constant.SP_KEY_CAMERA_TEST_REMARK, "can not find camera");
                return;
            }

            String cameraid = camIds[0];
            try
            {
                CameraCharacteristics mCameraCharacteristics = manager.getCameraCharacteristics(cameraid);
                mSensorOrientation = mCameraCharacteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);
                StreamConfigurationMap map = mCameraCharacteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                Size maxSize = Collections.max(Arrays.asList(map.getOutputSizes(ImageFormat.JPEG)), new Comparator<Size>()
                {
                    @Override
                    public int compare(Size size, Size t1)
                    {
                        return Long.signum((long) size.getWidth() * size.getHeight() -
                                (long) t1.getWidth() * t1.getHeight());
                    }
                });
                mPreViewSize = maxSize;

//            configureTransform(mTextureView.getWidth(), mTextureView.getHeight());
                mImageReader = ImageReader.newInstance(maxSize.getWidth(), maxSize.getHeight(), ImageFormat.JPEG, 5);
                mImageReader.setOnImageAvailableListener(onImageAvaiableListener, threadHandler);
                manager.openCamera(cameraid, cameraOpenCallBack, threadHandler);
            } catch (CameraAccessException e)
            {
                e.printStackTrace();
                mTestSuccess = false;
                handler.sendEmptyMessage(MSG_TEST_FINISH);
                SPUtils.put(this, Constant.SP_KEY_CAMERA_TEST_REMARK, "can not connect to camera");
            }

        } else
        {
            mTestSuccess = true;
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        }
    }

    public void takePicture()
    {
        if (System.currentTimeMillis() < endTime)
        {
            try
            {
                Log.d(TAG, "camera is taking photo");
                CaptureRequest.Builder builder = mPreviewSession.getDevice().createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
                builder.addTarget(mImageReader.getSurface());
                builder.set(CaptureRequest.CONTROL_AF_MODE,
                        CaptureRequest.CONTROL_AF_MODE_AUTO);
                builder.set(CaptureRequest.CONTROL_AF_TRIGGER,
                        CameraMetadata.CONTROL_AF_TRIGGER_START);
                builder.set(CaptureRequest.JPEG_ORIENTATION, 90);
                mPreviewSession.capture(builder.build(), null, threadHandler);
            } catch (CameraAccessException e)
            {
                e.printStackTrace();
            }

            tv_status.setText("camera test-->take picture");
            handler.sendEmptyMessageDelayed(MSG_RELEASE_CAMERA, CHANGE_TIME);
        } else
        {
            mTestSuccess = true;
            handler.sendEmptyMessage(MSG_RELEASE_CAMERA);
        }
    }

    public void startRecord()
    {
        if (System.currentTimeMillis() >= endTime || interrupt)
        {
            mTestSuccess = true;
            handler.sendEmptyMessage(MSG_RELEASE_CAMERA);
        } else
        {
            LogUtil.i("startRecord");
            Toast.makeText(CameraTestActivity.this, "startRecord", Toast.LENGTH_SHORT).show();
            tv_status.setText("camera test-->record video");
            recordVideo();
        }
    }

    public void testFinish()
    {
        if (mTestSuccess)
        {
            config.saveTestResult(Constant.SP_KEY_CAMERA_TEST_SUCCESS);
        } else
        {
            config.saveTestResult(Constant.SP_KEY_CAMERA_TEST_FAIL);
        }
        LogUtil.d(" camera 测试完成  结果 : " + (mTestSuccess ? "成功" : "失败"));
        if (isAutoRunin)
        {
            toNextText();
            finish();
            LogUtil.d(" camera 测试结束 ");
        } else
        {
            Toast.makeText(CameraTestActivity.this, "camrea test finish", Toast.LENGTH_SHORT).show();
            tv_status.setText("camera test" + (mTestSuccess ? "success" : "fail"));
            LogUtil.d(" camera 测试完成  结果 : " + (mTestSuccess ? "成功" : "失败"));
        }
    }

    public void recordVideo()
    {
        try
        {
            setUpMediaRecorder();
            mPreViewBuidler = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_RECORD);
            SurfaceTexture texture = mTextureView.getSurfaceTexture();
            texture.setDefaultBufferSize(mPreViewSize.getWidth(), mPreViewSize.getHeight());
            Surface surface = new Surface(texture);
            mPreViewBuidler.addTarget(surface);
            mPreViewBuidler.addTarget(mMediaRecorder.getSurface());
            mCameraDevice.createCaptureSession(Arrays.asList(surface, mMediaRecorder.getSurface()), mSessionStateCallBack, /*threadHandler*/ handler);
        } catch (CameraAccessException e)
        {
            e.printStackTrace();
            mTestSuccess = false;
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        } catch (IOException e)
        {
            e.printStackTrace();
        }
    }

    public void releaseCamera()
    {
        tv_status.setText("camera test-->close camera");
        stopAndCloseCamera();
        if (System.currentTimeMillis() < endTime)
        {
            handler.sendEmptyMessageDelayed(MSG_OPEN_CAMERA, CHANGE_TIME);
        } else
        {
            mTestSuccess = true;
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        }
    }

    private void setUpMediaRecorder() throws IOException
    {
        mMediaRecorder = new MediaRecorder();
        mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.SURFACE);
        mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
        mMediaRecorder.setOutputFile(mNextVideoAbsolutePath);
        mMediaRecorder.setVideoEncodingBitRate(10000000);
        mMediaRecorder.setVideoFrameRate(30);
        mMediaRecorder.setVideoSize(mPreViewSize.getWidth(), mPreViewSize.getHeight());
        mMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
        mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
        mMediaRecorder.setAudioChannels(2);
        int rotation = getWindowManager().getDefaultDisplay().getRotation();
        switch (mSensorOrientation)
        {
            case SENSOR_ORIENTATION_DEFAULT_DEGREES:
                mMediaRecorder.setOrientationHint(DEFAULT_ORIENTATIONS.get(rotation));
                LogUtil.i("DEFAULT_ORIENTATIONS" + DEFAULT_ORIENTATIONS.get(rotation));
                break;
            case SENSOR_ORIENTATION_INVERSE_DEGREES:
                mMediaRecorder.setOrientationHint(INVERSE_ORIENTATIONS.get(rotation));
                LogUtil.i("INVERSE_ORIENTATIONS" + INVERSE_ORIENTATIONS.get(rotation));
                break;
        }
        mMediaRecorder.prepare();
    }


}