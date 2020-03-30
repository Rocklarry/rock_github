package com.a3nod.lenovo.sparrowfactory.mmi.camera;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
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
import android.media.AudioManager;
import android.media.Image;
import android.media.ImageReader;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.media.Ringtone;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.StatFs;
import android.text.format.Formatter;
import android.util.Log;
import android.util.Size;
import android.util.SparseIntArray;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.VideoView;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.tool.BMPUtil;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.List;

/**
 * Created by Lenovo on 2017/11/3.
 */

public class TestActivity extends Activity
{
    public static final String TAG = "TestActivity";
    public static final int MSG_TAKE_PICTURE = 1;
    public static final int MSG_SET_ORIENTATION = 2;
    public static final int MSG_BUTTON_VISIBLE = 3;
    public static final int MSG_BUTTON_INVISIBLE = 4;
    public static final int MSG_BUTTON_OPENED = 5;

    AutoFitTextureView mTextureView;
    TextView mResult;
    ImageView mThumbnail;
    Button mButton, mBnRecord;
    Handler mHandler;
    Handler mUIHandler;
    ImageReader mImageReader;
    CaptureRequest.Builder mPreViewBuidler;
    CameraCaptureSession mCameraSession;
    CameraCharacteristics mCameraCharacteristics;
    private CameraDevice mCameraDevice;
    Ringtone ringtone;
    int mSensorOrientation;
    private static final int SENSOR_ORIENTATION_DEFAULT_DEGREES = 90;
    private static final int SENSOR_ORIENTATION_INVERSE_DEGREES = 270;

    private static final SparseIntArray DEFAULT_ORIENTATIONS = new SparseIntArray();
    private static final SparseIntArray INVERSE_ORIENTATIONS = new SparseIntArray();

    private boolean pictureFlag = false;
    private boolean recordFlag = false;
    private  Button tv_pass,tv_fail;

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


    /**
     * MediaRecorder
     */
    private MediaRecorder mMediaRecorder;
    /**
     * Whether the app is recording video now
     */
    private boolean mIsRecordingVideo;

//    int testModel = Constant.INTENT_VALUE_TEST_MODEL_PCBA;
    AudioManager mAudioManager;

    private String picName;//当前拍下的照片名字

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_MMI);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);
        mAudioManager = (AudioManager)getSystemService(Context.AUDIO_SERVICE);
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC,mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC), 0);
        HandlerThread thread = new HandlerThread("Ceamera3");
        thread.start();
        mHandler = new Handler(thread.getLooper());
        init();
        initConfirmButton();
    }

    public void initConfirmButton()
    {
        tv_pass = findViewById(R.id.btn_result_pass);
        tv_pass.setEnabled(false);
        tv_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Log.e("TAG", "相机测试");
                ASSYEntity.getInstants().setCameraTestResult(true);
                MMITestProcessManager.getInstance().toNextTest();
                finish();
            }
        });
        Button tv_reset = findViewById(R.id.btn_result_reset);
        tv_reset.setVisibility(View.GONE);
        tv_reset.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {

            }
        });
        tv_fail = findViewById(R.id.btn_result_fail);
//        tv_fail.setEnabled(false);
        tv_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ASSYEntity.getInstants().setCameraTestResult(false);
                MMITestProcessManager.getInstance().testFail();
                finish();
            }
        });
    }

    private void init()
    {
        mResult = findViewById(R.id.tv_1);
        mTextureView = findViewById(R.id.camera_show);
        mButton = findViewById(R.id.bn_take);
        mBnRecord = findViewById(R.id.bn_record);
        mThumbnail = findViewById(R.id.im_pic);
        mButton.setEnabled(false);
        mBnRecord.setEnabled(false);
        mThumbnail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                View popView = LayoutInflater.from(TestActivity.this).inflate(R.layout.layout_image_preview, null);

                AlertDialog.Builder builder = new AlertDialog.Builder(TestActivity.this);
                builder.setTitle(null);
                builder.setView(popView);
                final AlertDialog alertDialog = builder.create();


                final ImageView imageView = popView.findViewById(R.id.iv_shotcut_preview);
                alertDialog.setOnDismissListener(new DialogInterface.OnDismissListener()
                {
                    @Override
                    public void onDismiss(DialogInterface dialogInterface)
                    {
                        if (imageView.getDrawingCache() != null && !imageView.getDrawingCache().isRecycled())
                        {
                            imageView.getDrawingCache().recycle();
                        }
                    }
                });
                Bitmap bm = BitmapFactory.decodeFile(Constant.CAMERA_DIR + "/"+ picName);
                imageView.setImageBitmap(bm);
                imageView.setClickable(true);
                imageView.setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View v)
                    {
                        alertDialog.dismiss();
                    }
                });
                alertDialog.show();
                WindowManager m = getWindowManager();
                Display d = m.getDefaultDisplay();  //为获取屏幕宽、高
                android.view.WindowManager.LayoutParams p = alertDialog.getWindow().getAttributes();  //获取对话框当前的参数值
                p.height = (int) (d.getHeight() * 0.7);   //高度设置为屏幕的0.3
                p.width = (int) (d.getWidth() * 0.8);    //宽度设置为屏幕的0.5
                alertDialog.getWindow().setAttributes(p);     //设置生效
//                LogUtil.i("dialog : width "+params.width +"  height: "+params.height);
            }
        });
        mButton.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                if(!recordFlag&&!pictureFlag){
                    tv_pass.setEnabled(false);
//                    tv_fail.setEnabled(false);
                }
                if (mCameraSession != null)
                {
                    try
                    {
//                        shootSound();
                        Log.d(TAG, "正在拍照");
                        CaptureRequest.Builder builder = mCameraSession.getDevice().createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
                        builder.addTarget(mImageReader.getSurface());
                        builder.set(CaptureRequest.CONTROL_AF_MODE,
                                CaptureRequest.CONTROL_AF_MODE_AUTO);
                        builder.set(CaptureRequest.CONTROL_AF_TRIGGER,
                                CameraMetadata.CONTROL_AF_TRIGGER_START);
                        builder.set(CaptureRequest.JPEG_ORIENTATION, 270);
                        mCameraSession.capture(builder.build(), null, mHandler);
                    } catch (CameraAccessException e)
                    {
                        e.printStackTrace();
                    }
                }
            }
        });
        mBnRecord.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                if(!recordFlag&&!pictureFlag){
                    tv_pass.setEnabled(false);
//                    tv_fail.setEnabled(false);
                }
                if (mIsRecordingVideo)
                {
                    ismIsRecordingVideo = false;
                    stopRecordingVideo();
                    playVideo();
                } else
                {
                    deleteRecordFile();
                    mUIHandler.post(runnable);
                    ismIsRecordingVideo = true;
                    Message.obtain(mUIHandler, MSG_BUTTON_INVISIBLE).sendToTarget();
                    startRecordingVideo();
                }
            }

        });
        mUIHandler = new Handler(new InnerCallBack());
        //初始化拍照的声音
//        ringtone = RingtoneManager.getRingtone(this, Uri.parse("file:///system/media/audio/ui/camera_click.ogg"));
//        AudioAttributes.Builder attr = new AudioAttributes.Builder();
//        attr.setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION);
//        ringtone.setAudioAttributes(attr.build());
        //初始化相机布局
        mTextureView.setSurfaceTextureListener(mSurfacetextlistener);

    }

    private void playVideo()
    {
        LogUtil.i("aaaaaaaaa playvideo");
        View popView = LayoutInflater.from(this).inflate(R.layout.video_camera, null);

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(null);
        builder.setView(popView);
        final AlertDialog alertDialog = builder.create();

        WindowManager.LayoutParams params = alertDialog.getWindow().getAttributes();
        params.width = params.width;
        params.height = params.width * 3 / 4;
        alertDialog.getWindow().setAttributes(params);
        LogUtil.i("dialog : width " + params.width + "  height: " + params.height);

        final VideoView videoView = popView.findViewById(R.id.my_play);
        alertDialog.setOnDismissListener(new DialogInterface.OnDismissListener()
        {
            @Override
            public void onDismiss(DialogInterface dialogInterface)
            {
                videoView.stopPlayback();
                Log.e("TAG","点击屏幕视频播放结束");
                recordFlag = true;
                Message.obtain(mUIHandler, MSG_BUTTON_VISIBLE).sendToTarget();
                mNextVideoAbsolutePath = null;
            }
        });
        if (mNextVideoAbsolutePath!= null){
            File file = new File(mNextVideoAbsolutePath);
            if (!file.exists()){
                if (alertDialog!=null&&alertDialog.isShowing()){
                    alertDialog.dismiss();
                }
                Message.obtain(mUIHandler, MSG_BUTTON_VISIBLE).sendToTarget();
                mNextVideoAbsolutePath = null;
                Log.e(TAG,"视频视频文件不存在");
                return;
            }
        }
        videoView.setVideoPath(mNextVideoAbsolutePath);
        videoView.setOnCompletionListener(new MediaPlayer.OnCompletionListener()
        {
            @Override
            public void onCompletion(MediaPlayer mediaPlayer)
            {
                alertDialog.dismiss();
                Log.e("TAG","视频播放结束");
                recordFlag = true;
                Message.obtain(mUIHandler, MSG_BUTTON_VISIBLE).sendToTarget();
                mNextVideoAbsolutePath = null;
            }
        });
        videoView.start();
        videoView.requestFocus();

        alertDialog.show();

    }

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

        }
    };
    private CameraDevice.StateCallback cameraOpenCallBack = new CameraDevice.StateCallback()
    {
        @Override
        public void onOpened(CameraDevice cameraDevice)
        {
            mCameraDevice = cameraDevice;
            Message.obtain(mUIHandler, MSG_BUTTON_OPENED).sendToTarget();
            Log.e(TAG, "相机已经打开");
            if (ismIsRecordingVideo)
            {
                return;
            }
            try
            {

                mPreViewBuidler = cameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
                SurfaceTexture texture = mTextureView.getSurfaceTexture();
                if (texture != null)
                {
                    texture.setDefaultBufferSize(mPreViewSize.getWidth(), mPreViewSize.getHeight());
                    Surface surface = new Surface(texture);
                    mPreViewBuidler.addTarget(surface);
                    cameraDevice.createCaptureSession(Arrays.asList(surface, mImageReader.getSurface()), mSessionStateCallBack, mHandler);
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
        }

        @Override
        public void onError(CameraDevice cameraDevice, int i)
        {
            Log.e(TAG, "相机打开失败");
            mUIHandler.post(new Runnable()
            {
                @Override
                public void run()
                {
                    mResult.setText("fail");
                }
            });
            ASSYEntity.getInstants().CameraResult = "fail";
        }
    };

    private ImageReader.OnImageAvailableListener onImageAvaiableListener = new ImageReader.OnImageAvailableListener()
    {
        @Override
        public void onImageAvailable(ImageReader imageReader)
        {
            mHandler.post(new ImageSaver(imageReader.acquireNextImage()));
        }
    };

    private Size mPreViewSize;
    private TextureView.SurfaceTextureListener mSurfacetextlistener = new TextureView.SurfaceTextureListener()
    {
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int i, int i1)
        {
//            mUIHandler.sendEmptyMessage(MSG_SET_ORIENTATION);
            openCamera(i,i1);
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int i, int i1)
        {
//            mUIHandler.sendEmptyMessage(MSG_SET_ORIENTATION);
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture)
        {
            Log.d(TAG, "onSurfaceTextureDestroyed: close Camera.");
            if (mCameraDevice != null)
            {
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

    public void openCamera(int width,int height)
    {

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
            Log.d(TAG, "No cameras found");
            mResult.setText("fail");
            ASSYEntity.getInstants().CameraResult = "fail";
            ASSYEntity.getInstants().CameraTestTimes++;

            return;
        }
        String cameraid = camIds[0];
        Log.d(TAG, "Using camera id " + cameraid);

        try
        {
            mCameraCharacteristics = manager.getCameraCharacteristics(cameraid);
            mSensorOrientation = mCameraCharacteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);
            Log.d(TAG, "sensor orientation " + mSensorOrientation);
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
            mPreViewSize = getOptimalSize(map.getOutputSizes(SurfaceTexture.class),width,height);

            // We fit the aspect ratio of TextureView to the size of preview we picked.
            int orientation = getResources().getConfiguration().orientation;
            if (orientation == Configuration.ORIENTATION_LANDSCAPE)
            {
                mTextureView.setAspectRatio(mPreViewSize.getWidth(), mPreViewSize.getHeight());
            } else
            {
                mTextureView.setAspectRatio(mPreViewSize.getHeight(), mPreViewSize.getWidth());
            }
            mImageReader = ImageReader.newInstance(mPreViewSize.getWidth(), mPreViewSize.getHeight(), ImageFormat.JPEG, 5);
            mImageReader.setOnImageAvailableListener(onImageAvaiableListener, mHandler);
            Log.d(TAG, "manager.openCamera method");
            manager.openCamera(cameraid, cameraOpenCallBack, mHandler);
        } catch (CameraAccessException e)
        {
            e.printStackTrace();
        }
    }

    private Size getOptimalSize(Size[] sizeMap, int width, int height) {
        List<Size> sizeList = new ArrayList<>();
        for (Size option : sizeMap) {
            if (width > height) {
                if (option.getWidth() > width && option.getHeight() > height) {
                    sizeList.add(option);
                }
            } else {
                if (option.getWidth() > height && option.getHeight() > width) {
                    sizeList.add(option);
                }
            }
        }
        if (sizeList.size() > 0) {
            return Collections.min(sizeList, new Comparator<Size>() {
                @Override
                public int compare(Size lhs, Size rhs) {
                    return Long.signum(lhs.getWidth() * lhs.getHeight() - rhs.getWidth() * rhs.getHeight());
                }
            });
        }
        return sizeMap[0];
    }

    private void configureTransform(int viewWidth, int viewHeight)
    {
        if (null == mTextureView || null == mPreViewSize)
        {
            return;
        }
        int rotation = getWindowManager().getDefaultDisplay().getRotation();
        Log.d(TAG, "rotation : " + rotation);
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

    private class ImageSaver implements Runnable
    {
        Image reader;

        public ImageSaver(Image reader)
        {
            this.reader = reader;
        }

        @Override
        public void run()
        {
            Log.d(TAG, "正在保存图片");
            SimpleDateFormat formatter   =   new   SimpleDateFormat("yyyyMMddHHmmss");
            Date currentTime = new Date(System.currentTimeMillis());
            picName = formatter.format(currentTime)+".jpg";
            File file = new File(Constant.CAMERA_DIR + "/"+picName);
            FileOutputStream outputStream = null;
            try
            {
                outputStream = new FileOutputStream(file);
                ByteBuffer buffer = reader.getPlanes()[0].getBuffer();
                byte[] buff = new byte[buffer.remaining()];
                buffer.get(buff);
                BitmapFactory.Options ontain = new BitmapFactory.Options();
//                ontain.inSampleSize = 100;
                Bitmap bm = BitmapFactory.decodeByteArray(buff, 0, buff.length, ontain);
                Message.obtain(mUIHandler, MSG_TAKE_PICTURE, bm).sendToTarget();
                Message.obtain(mUIHandler, MSG_BUTTON_VISIBLE).sendToTarget();
                outputStream.write(buff);
                int[] pixels = new int[bm.getWidth() * bm.getHeight()];
                bm.getPixels(pixels, 0, bm.getWidth(), 0, 0, bm.getWidth(), bm.getHeight());
                BMPUtil.writeBMP(Constant.PHOTO_PATH, pixels, bm.getWidth(), bm.getHeight());
                Log.d(TAG, "保存图片完成");

            } catch (FileNotFoundException e)
            {
                e.printStackTrace();
            } catch (IOException e)
            {
                e.printStackTrace();
            } finally
            {
                if (reader != null)
                {
                    reader.close();
                }
                if (outputStream != null)
                {
                    try
                    {
                        outputStream.close();
                    } catch (IOException e)
                    {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    private class InnerCallBack implements Handler.Callback
    {
        @Override
        public boolean handleMessage(Message message)
        {
            switch (message.what)
            {
                case MSG_TAKE_PICTURE:
                    Bitmap bm = (Bitmap) message.obj;
                    mThumbnail.setImageBitmap(bm);
                    mResult.setText("pass");
                    pictureFlag = true;
                    break;
                case MSG_SET_ORIENTATION:
                    configureTransform(mTextureView.getWidth(), mTextureView.getHeight());
                    break;
                case MSG_BUTTON_INVISIBLE:
                    tv_pass.setEnabled(false);
                    mButton.setEnabled(false);
                    break;
                case MSG_BUTTON_VISIBLE:
                    if(recordFlag&&pictureFlag){
                        tv_pass.setEnabled(true);
                        recordFlag = false;
                        pictureFlag = false;
                    }
                    mButton.setEnabled(true);
                    break;
                case MSG_BUTTON_OPENED:
                    mButton.setEnabled(true);
                    mBnRecord.setEnabled(true);
                    break;
            }
            return false;
        }
    }

    /**
     * 播放系统的拍照的声音
     */
    public void shootSound()
    {
        ringtone.stop();
        ringtone.play();
    }

    private Surface mRecorderSurface;
    //    private Integer mSensorOrientation;
    private String mNextVideoAbsolutePath;

    /**
     * Update the camera preview. {@link #startPreview()} needs to be called in advance.
     */
    private void updatePreview()
    {
        if (null == mCameraDevice)
        {
            return;
        }
        try
        {
            setUpCaptureRequestBuilder(mPreViewBuidler);

            mCameraSession.setRepeatingRequest(mPreViewBuidler.build(), null, mHandler);
        } catch (CameraAccessException e)
        {
            e.printStackTrace();
        }
    }

    private void setUpCaptureRequestBuilder(CaptureRequest.Builder builder)
    {
        builder.set(CaptureRequest.CONTROL_MODE, CameraMetadata.CONTROL_MODE_AUTO);
    }

    boolean ismIsRecordingVideo = false;

    private void startRecordingVideo()
    {
        if (null == mCameraDevice || !mTextureView.isAvailable() || null == mPreViewSize)
        {
            return;
        }
        try
        {
            closePreviewSession();
            setUpMediaRecorder();
            SurfaceTexture texture = mTextureView.getSurfaceTexture();
            assert texture != null;
            texture.setDefaultBufferSize(mPreViewSize.getWidth(), mPreViewSize.getHeight());
            mPreViewBuidler = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_RECORD);
            List<Surface> surfaces = new ArrayList<>();

            // Set up Surface for the camera preview
            Surface previewSurface = new Surface(texture);
            surfaces.add(previewSurface);
            mPreViewBuidler.addTarget(previewSurface);

            // Set up Surface for the MediaRecorder
            mRecorderSurface = mMediaRecorder.getSurface();
            surfaces.add(mRecorderSurface);
            mPreViewBuidler.addTarget(mRecorderSurface);

            // Start a capture session
            // Once the session starts, we can update the UI and start recording
            mCameraDevice.createCaptureSession(surfaces, new CameraCaptureSession.StateCallback()
            {

                @Override
                public void onConfigured(CameraCaptureSession cameraCaptureSession)
                {
                    mCameraSession = cameraCaptureSession;
                    updatePreview();
                    runOnUiThread(new Runnable()
                    {
                        @Override
                        public void run()
                        {
                            // UI
                            mBnRecord.setText("stop");
                            mIsRecordingVideo = true;
                            // Start recording
                            mMediaRecorder.start();
                        }
                    });
                }

                @Override
                public void onConfigureFailed(CameraCaptureSession cameraCaptureSession)
                {
//                    Activity activity = getActivity();
//                    if (null != activity) {
//                        Toast.makeText(activity, "Failed", Toast.LENGTH_SHORT).show();
//                    }
                }
            }, mHandler);
        } catch (CameraAccessException e)
        {
            e.printStackTrace();
        } catch (IOException e)
        {
            e.printStackTrace();
        }

    }

    private void closePreviewSession()
    {
        if (mCameraSession != null)
        {
            try
            {
                mCameraSession.stopRepeating();
            } catch (CameraAccessException e)
            {
                e.printStackTrace();
            }
            mCameraSession.close();
            mCameraSession = null;
        }
//        if (mCameraDevice != null)
//        {
//            mCameraDevice.close();
//            mCameraDevice = null;
//        }
    }

    private void setUpMediaRecorder() throws IOException
    {
        mMediaRecorder = new MediaRecorder();
        mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.SURFACE);
        mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
        if (mNextVideoAbsolutePath == null || mNextVideoAbsolutePath.isEmpty())
        {
            mNextVideoAbsolutePath = getVideoFilePath(this);
        }
        Log.d(TAG, "mNextVideoAbsolutePath =" + mNextVideoAbsolutePath);
        mMediaRecorder.setOutputFile(mNextVideoAbsolutePath);
        mMediaRecorder.setVideoEncodingBitRate(10000000);
        mMediaRecorder.setVideoFrameRate(30);
        mMediaRecorder.setAudioChannels(2);
        mMediaRecorder.setVideoSize(mPreViewSize.getWidth(), mPreViewSize.getHeight());
        mMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
        mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
        int rotation = getWindowManager().getDefaultDisplay().getRotation();
        switch (mSensorOrientation)
        {
            case SENSOR_ORIENTATION_DEFAULT_DEGREES:
                mMediaRecorder.setOrientationHint(DEFAULT_ORIENTATIONS.get(rotation));
                break;
            case SENSOR_ORIENTATION_INVERSE_DEGREES:
                mMediaRecorder.setOrientationHint(INVERSE_ORIENTATIONS.get(rotation));
                break;
        }
        mMediaRecorder.prepare();
    }

    private String getVideoFilePath(Context context)
    {
        return context.getExternalFilesDir(null).getAbsolutePath() + "/3nod_camera_record_test.mp4";
    }

    private void deleteRecordFile()
    {
        File file = new File(getVideoFilePath(this));
        try
        {
            if (file.exists())
            {
                Log.d(TAG, "file is exists :");
                file.delete();
            }
        } catch (Exception e)
        {
            e.printStackTrace();
        }
    }

    private void getMemoryInfo()
    {
        StatFs statfs = new StatFs(this.getExternalFilesDir(null).getAbsolutePath());
        long count = statfs.getAvailableBlocks();
        long size = statfs.getBlockSize();
        String freeSize = Formatter.formatFileSize(this, count * size);
        Log.d(TAG, "memoryInfo = " + (count * size) + ",freeSize=" + freeSize);
        if (freeSize.contains("MB"))
        {
            int freeMemory = Integer.parseInt(freeSize.substring(0, freeSize.indexOf("MB")).trim());
            if (freeMemory <= 500 && mIsRecordingVideo)
            {
                ismIsRecordingVideo = false;
                stopRecordingVideo();
                playVideo();
            }
        }
    }

    Runnable runnable = new Runnable()
    {
        @Override
        public void run()
        {
            getMemoryInfo();
            mUIHandler.postDelayed(runnable, 5000);
        }
    };

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        Log.d(TAG, "onDestroy().");
        mUIHandler.removeCallbacks(runnable);
        mUIHandler.removeMessages(MSG_TAKE_PICTURE);
        mUIHandler.removeMessages(MSG_SET_ORIENTATION);
        mUIHandler.removeMessages(MSG_BUTTON_VISIBLE);
        mUIHandler.removeMessages(MSG_BUTTON_INVISIBLE);
        mUIHandler.removeMessages(MSG_BUTTON_OPENED);
        deleteRecordFile();
    }

    private void stopRecordingVideo()
    {
        // UI
        mIsRecordingVideo = false;
        mBnRecord.setText("record");
        Message.obtain(mUIHandler, MSG_BUTTON_VISIBLE).sendToTarget();
        mUIHandler.removeCallbacks(runnable);
        //必须将这一句放置到MediaRecorder停止释放的前面，
        //否则就会造成，接收数据方（Encoder）已经停止了，而发送数据的session还在运行。才会造成以上错误。
        startPreview();
        // Stop recording
        mMediaRecorder.stop();
        mMediaRecorder.reset();
        mMediaRecorder.release();
//        mNextVideoAbsolutePath = null;
    }

    /**
     * Start the camera preview.
     */
    private void startPreview()
    {
        if (null == mCameraDevice || !mTextureView.isAvailable() || null == mPreViewSize)
        {
            return;
        }
        try
        {
            closePreviewSession();
            SurfaceTexture texture = mTextureView.getSurfaceTexture();
            assert texture != null;
            texture.setDefaultBufferSize(mPreViewSize.getWidth(), mPreViewSize.getHeight());
            mPreViewBuidler = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);

            Surface previewSurface = new Surface(texture);
            mPreViewBuidler.addTarget(previewSurface);
            mCameraDevice.createCaptureSession(Arrays.asList(previewSurface, mImageReader.getSurface()), mSessionStateCallBack, mHandler);
        } catch (CameraAccessException e)
        {
            e.printStackTrace();
        }
    }

}
