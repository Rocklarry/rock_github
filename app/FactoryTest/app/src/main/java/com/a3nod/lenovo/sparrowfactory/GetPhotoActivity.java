package com.a3nod.lenovo.sparrowfactory;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
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
import android.hardware.camera2.DngCreator;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Size;
import android.util.SparseIntArray;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.tool.BMPUtil;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;

public class GetPhotoActivity extends Activity
{
    private TextureView tv_preview;
    private ImageView iv_shotcut;
    private ImageView iv_takePictrue;
    private CameraDevice device;
    private Handler mHandler;
    private CameraCaptureSession mCameraSession;
    private CameraManager manager;
    private String cameraID;
    private ImageReader mImageReader;
    private TakePictureReceiver takePictureReceiver;
    private  Size largestSize;
    public static final int MSG_SET_ORIENTATION = 1;
    public static final int MSG_OPEN_CAMERA = 2;
    public static final int MSG_SAVE_PICTURE = 3;
    private static final SparseIntArray ORIENTATIONS = new SparseIntArray();

    private Size mPreViewSize;
    TotalCaptureResult captureResult;
    CameraCharacteristics cameraCharacteristics;
    static
    {
        ORIENTATIONS.append(Surface.ROTATION_0, 90);
        ORIENTATIONS.append(Surface.ROTATION_90, 0);
        ORIENTATIONS.append(Surface.ROTATION_180, 270);
        ORIENTATIONS.append(Surface.ROTATION_270, 180);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_get_photo);
        manager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
        tv_preview = findViewById(R.id.tv_preview);
        tv_preview.setSurfaceTextureListener(mSurfacetextlistener);

        iv_shotcut = findViewById(R.id.iv_shotcut);
        iv_takePictrue=findViewById(R.id.iv_take_picture);

        HandlerThread thread = new HandlerThread("Ceamera3");
        thread.start();
        takePictureReceiver=new TakePictureReceiver();
        mHandler = new Handler(thread.getLooper());
        File dir = new File(Constant.CAMERA_DIR);
        if (!dir.exists())
        {
            dir.mkdirs();
        }
    }

    Handler uiHandler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            switch (msg.what)
            {
                case MSG_SET_ORIENTATION:
//                    configureTransform(tv_preview.getWidth(), tv_preview.getHeight());
                    break;
                case MSG_OPEN_CAMERA:
                    openCamera();
                    break;
                case MSG_SAVE_PICTURE:
                    Toast.makeText(GetPhotoActivity.this, "Picture have been saved to Path" + Constant.PHOTO_PATH, Toast.LENGTH_SHORT).show();
                    finish();
                    break;
            }
        }
    };
    private TextureView.SurfaceTextureListener mSurfacetextlistener = new TextureView.SurfaceTextureListener()
    {
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, final int width, final int height)
        {
            uiHandler.sendEmptyMessage(MSG_SET_ORIENTATION);
            uiHandler.sendEmptyMessage(MSG_OPEN_CAMERA);
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int width, int height)
        {
            uiHandler.sendEmptyMessage(MSG_SET_ORIENTATION);
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture)
        {
            if (mCameraSession != null)
            {
                try
                {
                    mCameraSession.stopRepeating();
                    mCameraSession.close();
                    mCameraSession = null;
                } catch (CameraAccessException e)
                {
                    e.printStackTrace();
                }
            }
            if (device != null)
            {
                device.close();
                device = null;
            }

            return false;
        }

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture)
        {

        }

    };
    CaptureRequest.Builder mPreViewBuidler;
    private CameraDevice.StateCallback cameraOpenCallBack = new CameraDevice.StateCallback()
    {
        @Override
        public void onOpened(CameraDevice cameraDevice)
        {
            device = cameraDevice;
            ConnectManager.getInstance().setTakePhotoResult(1, "007");
            try
            {
                SurfaceTexture texture = tv_preview.getSurfaceTexture();

                LogUtil.d("相机已连接 texture != null?" + (texture != null));
                if (texture != null)
                {
                    mPreViewBuidler = device.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
                    mPreViewBuidler.set(CaptureRequest.JPEG_ORIENTATION, 270);

                    Surface surface = new Surface(texture);
                    mPreViewBuidler.addTarget(surface);
                    texture.setDefaultBufferSize(mPreViewSize.getWidth(), mPreViewSize.getHeight());
                    LogUtil.i("texture width : " + mPreViewSize.getWidth() + " height: " + mPreViewSize.getHeight());
                    cameraDevice.createCaptureSession(Arrays.asList(surface, mImageReader.getSurface()), new CameraCaptureSession.StateCallback()
                    {
                        @Override
                        public void onConfigured(CameraCaptureSession cameraCaptureSession)
                        {
                            LogUtil.i("session create");
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

                    }, mHandler);
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
            LogUtil.d("相机连接断开");
        }

        @Override
        public void onError(CameraDevice cameraDevice, int i)
        {
            LogUtil.d("相机打开失败");
            ConnectManager.getInstance().setTakePhotoResult(2, "007");
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
                String[] camIds = {};
                try
                {
                    camIds = manager.getCameraIdList();
                    if (camIds.length >= 1)
                    {
                        cameraID = camIds[0];
                        setPreviewParam();
                        LogUtil.d("Using camera id " + cameraID);
                        manager.openCamera(cameraID, cameraOpenCallBack, mHandler);
                    } else
                    {
                        LogUtil.d("No cameras found");
                    }
                } catch (CameraAccessException e)
                {
                    LogUtil.d("Cam access exception getting IDs" + e.getMessage());
                    e.printStackTrace();
                }
            }
        }.start();
    }

    public void setPreviewParam()
    {
        CameraCharacteristics mCameraCharacteristics = null;
        try
        {
            mCameraCharacteristics = manager.getCameraCharacteristics(cameraID);
            int mSensorOrientation = mCameraCharacteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);
            StreamConfigurationMap map = mCameraCharacteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
            cameraCharacteristics=mCameraCharacteristics;

            largestSize = Collections.max(Arrays.asList(map.getOutputSizes(ImageFormat.JPEG)), new Comparator<Size>()
            {
                @Override
                public int compare(Size size, Size t1)
                {
                    return Long.signum((long) size.getWidth() * size.getHeight() -
                            (long) t1.getWidth() * t1.getHeight());
                }
            });

            mPreViewSize = map.getOutputSizes(SurfaceTexture.class)[0];
            LogUtil.i("====================================");
            LogUtil.i("mPreViewSize resolution : " + mPreViewSize.getWidth() + " - " + mPreViewSize.getHeight());
            for (Size size : map.getOutputSizes(SurfaceTexture.class))
            {
                LogUtil.i("suport resolution : " + size.getWidth() + " - " + size.getHeight());
            }
            mImageReader = ImageReader.newInstance(largestSize.getWidth(), largestSize.getHeight(), ImageFormat.JPEG, 1);
            mImageReader.setOnImageAvailableListener(onImageAvaiableListener, mHandler);
            LogUtil.i("camera mSensorOrientation " + mSensorOrientation);
        } catch (CameraAccessException e)
        {
            e.printStackTrace();
        }
    }

    private void configureTransform(int viewWidth, int viewHeight)
    {
        if (null == tv_preview || null == largestSize)
        {
            return;
        }
        int rotation = getWindowManager().getDefaultDisplay().getRotation();
        LogUtil.i("rotation : " + rotation);
        rotation = 1;
        Matrix matrix = new Matrix();
        RectF viewRect = new RectF(0, 0, viewWidth, viewHeight);
        RectF bufferRect = new RectF(0, 0, largestSize.getHeight(), largestSize.getWidth());
        float centerX = viewRect.centerX();
        float centerY = viewRect.centerY();
        if (Surface.ROTATION_90 == rotation || Surface.ROTATION_270 == rotation)
        {
            bufferRect.offset(centerX - bufferRect.centerX(), centerY - bufferRect.centerY());
            matrix.setRectToRect(viewRect, bufferRect, Matrix.ScaleToFit.FILL);
            float scale = Math.max(
                    (float) viewHeight / largestSize.getHeight(),
                    (float) viewWidth / largestSize.getWidth());
            matrix.postScale(scale, scale, centerX, centerY);
            matrix.postRotate(90 * (rotation - 2), centerX, centerY);
        } else if (Surface.ROTATION_180 == rotation)
        {
            matrix.postRotate(180, centerX, centerY);
        }
        tv_preview.setTransform(matrix);
    }
    //相机会话的监听器，通过他得到mCameraSession对象，这个对象可以用来发送预览和拍照请求

    private ImageReader.OnImageAvailableListener onImageAvaiableListener = new ImageReader.OnImageAvailableListener()
    {
        @Override
        public void onImageAvailable(ImageReader imageReader)
        {
            LogUtil.i("aaaaaaaaaaa onImageAvaiableListener : onImageAvailable");
            mHandler.post(new ImageSaver(imageReader.acquireNextImage()));
        }
    };


    public void exit(View view)
    {
        finish();
    }


    class ImageSaver implements Runnable
    {
        Image reader;

        public ImageSaver(Image reader)
        {
            this.reader = reader;
        }

        @Override
        public void run()
        {
            LogUtil.d("正在保存图片");
            if(reader.getFormat()==ImageFormat.JPEG){
                LogUtil.i("aaaaaaaaaaaaa ImageFormat.JPEG");
                try
                {
                    ByteBuffer buffer = reader.getPlanes()[0].getBuffer();
                    byte[] buff = new byte[buffer.remaining()];
                    LogUtil.i(" image  buffer size " + buff.length);
                    buffer.get(buff);
                    BitmapFactory.Options ontain = new BitmapFactory.Options();
                    Bitmap bm = BitmapFactory.decodeByteArray(buff, 0, buff.length, ontain);

                    File file = new File("/sdcard/sparrow/camera/aaa.jpg");
                    if (file.exists())
                    {
                        file.delete();
                    }
                    FileOutputStream fos = new FileOutputStream(file);
//                bm.compress(Bitmap.CompressFormat.JPEG,50,fos);
                    fos.close();
                    int[] pixels = new int[bm.getWidth() * bm.getHeight()];
                    bm.getPixels(pixels, 0, bm.getWidth(), 0, 0, bm.getWidth(), bm.getHeight());
                    BMPUtil.writeBMP(Constant.PHOTO_PATH, pixels, bm.getWidth(), bm.getHeight());
                    uiHandler.sendEmptyMessage(MSG_SAVE_PICTURE);
                    LogUtil.d("保存图片完成");
                    ConnectManager.getInstance().setTakePhotoResult(4, "007");
                } catch (FileNotFoundException e)
                {
                    ConnectManager.getInstance().setTakePhotoResult(5, "007");
                    e.printStackTrace();
                } catch (IOException e)
                {
                    ConnectManager.getInstance().setTakePhotoResult(5, "007");
                    e.printStackTrace();
                } finally
                {
                    if (reader != null)
                    {
                        reader.close();
                    }
                }
            }else if(reader.getFormat()==ImageFormat.RAW_SENSOR)
            {
                LogUtil.i("aaaaaaaaaaaaa ImageFormat.RAW_SENSOR");
                DngCreator dngCreator = new DngCreator(cameraCharacteristics, captureResult);
                FileOutputStream output = null;
                try {
                    output = new FileOutputStream(Constant.PHOTO_PATH);
                    dngCreator.writeImage(output, reader);
                    ConnectManager.getInstance().setTakePhotoResult(4, "007");
                } catch (IOException e) {
                    ConnectManager.getInstance().setTakePhotoResult(5, "007");
                    e.printStackTrace();
                } finally {
                    reader.close();
                    try
                    {
                        output.close();
                    } catch (IOException e)
                    {
                        e.printStackTrace();
                    }
                }
            }

        }
    }

    public void capture(View view)
    {
        if (device != null)
        {
            CaptureRequest.Builder builder = null;
            try
            {
                mCameraSession.stopRepeating();
                builder = device.createCaptureRequest(CameraDevice.TEMPLATE_STILL_CAPTURE);

                builder.addTarget(mImageReader.getSurface());
                builder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_AUTO);
                builder.set(CaptureRequest.CONTROL_AF_TRIGGER, CameraMetadata.CONTROL_AF_TRIGGER_START);
                builder.set(CaptureRequest.JPEG_ORIENTATION, 270);
                mCameraSession.capture(builder.build(), CaptureCallback, mHandler);
            } catch (CameraAccessException e)
            {
                e.printStackTrace();
            }
        }
    }

    CameraCaptureSession.CaptureCallback CaptureCallback = new CameraCaptureSession.CaptureCallback()
    {

        @Override
        public void onCaptureCompleted(CameraCaptureSession session,
                                       CaptureRequest request,
                                       TotalCaptureResult result)
        {
            LogUtil.i("aaaaaaaaaaa CaptureCallback : onCaptureCompleted");
            captureResult=result;
            LogUtil.d(Constant.PHOTO_PATH);
        }
    };

    @Override
    protected void onResume()
    {
        super.onResume();
        registerReceiver(takePictureReceiver,new IntentFilter(Constant.ACTION_TAKE_PICTURE));
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        unregisterReceiver(takePictureReceiver);
    }

    class TakePictureReceiver extends BroadcastReceiver
    {

        @Override
        public void onReceive(Context context, Intent intent)
        {
            iv_takePictrue.performClick();
        }
    }
}
