package com.a3nod.lenovo.sparrowfactory.runin.twodimensional;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;

import com.a3nod.lenovo.sparrowfactory.tool.ThreadPoolUtils;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Description
 * Created by aaa on 2017/11/16.
 */

public class TwoDimensionalView extends GLSurfaceView
{
    final float ANGLE_SPAN = 0.25f;

    RotateThread rthread;
    SceneRenderer mRenderer;
    public TwoDimensionalView(Context context)
    {
        super(context);
        this.setEGLContextClientVersion(2);
        mRenderer=new SceneRenderer();
        this.setRenderer(mRenderer);
        this.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }
    private class SceneRenderer implements GLSurfaceView.Renderer
    {
        Triangle tle;
        public void onDrawFrame(GL10 gl)
        {
            //清除深度缓冲与颜色缓冲
            GLES20.glClear( GLES20.GL_DEPTH_BUFFER_BIT | GLES20.GL_COLOR_BUFFER_BIT);
            //绘制三角形对
            tle.drawSelf();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height)
        {
            //设置视窗大小及位置
            GLES20.glViewport(0, 0, width, height);
            //计算GLSurfaceView的宽高比
            float ratio = (float) width / height;
            //调用此方法计算产生透视投影矩阵
            Matrix.frustumM(Triangle.mProjMatrix, 0, -ratio, ratio, -1, 1, 1, 10);
            //调用此方法产生摄像机9参数位置矩阵
            Matrix.setLookAtM(Triangle.mVMatrix, 0, 0,0,3,0f,0f,0f,0f,1.0f,0.0f);
        }
        public void onSurfaceCreated(GL10 gl, EGLConfig config)
        {
            //设置屏幕背景色RGBA
            GLES20.glClearColor(0,0,0,1.0f);
            //创建三角形对对象
            tle=new Triangle(TwoDimensionalView.this);
            //打开深度检测
            GLES20.glEnable(GLES20.GL_DEPTH_TEST);
            rthread=new RotateThread();
            ThreadPoolUtils.executeSingleThread(rthread);
        }
    }
    public class RotateThread extends Thread
    {
        public volatile boolean flag=true;
        public RotateThread()
        {
            super("thread_2d_rotate");
        }
        @Override
        public void run()
        {
            while(flag)
            {
                mRenderer.tle.xAngle=mRenderer.tle.xAngle+ANGLE_SPAN;
                try
                {
                    Thread.sleep(16);
                }
                catch(Exception e)
                {
                    e.printStackTrace();
                }
            }
        }
    }

    @Override
    public void onResume()
    {
        super.onResume();
    }

    @Override
    public void onPause()
    {
        super.onPause();
        rthread.flag=false;
    }
}