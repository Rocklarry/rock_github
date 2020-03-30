package com.a3nod.lenovo.sparrowfactory.facecamera;

import android.app.Activity;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.R;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.sql.Connection;
import java.util.List;

public class CameraActivity extends Activity implements SurfaceHolder.Callback, Camera.FaceDetectionListener, View.OnClickListener {
    private Camera mCamera;
    private SurfaceHolder mHolder;
    private FaceView mFaceView;
    private Button takePicture;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_facecamera);
        SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surfaceView);

        mFaceView = (FaceView) findViewById(R.id.faceView);

        takePicture = (Button) findViewById(R.id.takePicture);
        takePicture.setOnClickListener(this);
        mHolder = surfaceView.getHolder();
        mHolder.addCallback(this);
        openCamera();
    }

    public void openCamera() {
        int cametacount = Camera.getNumberOfCameras();
        mCamera=Camera.open(cametacount-1);
        mCamera.setFaceDetectionListener(this);
        Camera.Parameters  parameters = mCamera.getParameters();
        boolean isable = parameters.getMaxNumDetectedFaces() > 0;
        Log.d("CameraActivity", "mCamera:" + mCamera+" isable="+isable);
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {

        if (mCamera == null) {
            return;
        }
        try {
            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPictureFormat(PixelFormat.JPEG);

            List<Camera.Size> pictures = parameters.getSupportedPictureSizes();

            Camera.Size size = pictures.get(pictures.size() -1);
            Log.d("CameraActivity", size.width + " " + size.height);
            parameters.setPictureSize(size.width,size.height);

            List<String> focusModes = parameters.getSupportedFocusModes();
            if (focusModes.contains("continuous-video")) {
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
            }

            mCamera.setParameters(parameters);
            mCamera.setPreviewDisplay(surfaceHolder);
            mCamera.setDisplayOrientation(90);
            mCamera.startPreview();
            if (parameters.getMaxNumDetectedFaces() > 0) {
                mCamera.startFaceDetection();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        if (mCamera != null) {
            mCamera.stopFaceDetection();
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }

    }

    @Override
    public void onFaceDetection(Camera.Face[] faces, Camera camera) {
        Toast.makeText(this, "faces.length:" + faces.length, Toast.LENGTH_SHORT).show();
        mFaceView.setFaces(faces);
    }

    public static void prepareMatrix(Matrix matrix, boolean mirror, int displayOrientation,
                                     int viewWidth, int viewHeight) {
        // Need mirror for front camera.
        matrix.setScale(mirror ? -1 : 1, 1);
        // This is the value for android.hardware.Camera.setDisplayOrientation.
        matrix.postRotate(displayOrientation);
        // Camera driver coordinates range from (-1000, -1000) to (1000, 1000).
        // UI coordinates range from (0, 0) to (width, height).
        matrix.postScale(viewWidth / 2000f, viewHeight / 2000f);
        matrix.postTranslate(viewWidth / 2f, viewHeight / 2f);
    }


    @Override
    public void onClick(View view) {
        mCamera.takePicture(null, null, new Camera.PictureCallback() {
            @Override
            public void onPictureTaken(byte[] bytes, Camera camera) {
                String path = savePicture(bytes);
                Log.d("CameraActivity", path);
                mCamera.stopPreview();
                mCamera.stopFaceDetection();
                finish();
            }
        });
    }

    public String savePicture(byte[] bytes) {

        try {
            String path = getFilesDir().getAbsolutePath() + "/share.png";
            FileOutputStream fos = new FileOutputStream(path);
            fos.write(bytes);
            fos.flush();
            fos.close();
            return path;

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return null;
    }

    private long lastMillis = 0;
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == KeyEvent.ACTION_DOWN)
        {
            long currentMillis = System.currentTimeMillis();
            if (currentMillis - lastMillis > 2000)
            {
                lastMillis = currentMillis;
                Toast.makeText(this, getString(R.string.double_click_exit), Toast.LENGTH_SHORT).show();
            } else
            {
                finish();
            }
        }
        return false;
    }
}
