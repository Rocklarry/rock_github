package com.a3nod.lenovo.sparrowfactory.mmi.sensor;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.hardware.SensorEvent;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Created by jianzhou.peng on 2017/11/6.
 */

public class MyGVSensor extends SurfaceView implements SurfaceHolder.Callback ,Runnable
{
    private SurfaceHolder mSurfaceHolder;
    private Paint mPaint;
    private Thread th;
    private boolean flag;
    private Canvas canvas;
    private int screenW, screenH;
    //圆形的X,Y坐标
    private int arc_x, arc_y;
    //传感器的xyz值
    private int mRight = 100;
    private int mBottom = 100;

    public MyGVSensor(Context context)
    {
        super(context);
    }

    public MyGVSensor(Context context, AttributeSet attributeSet)
    {
        super(context, attributeSet);
        initView();
    }

    public MyGVSensor(Context context, AttributeSet attributeSet, int defSy)
    {
        super(context, attributeSet, defSy);
    }

    private void initView()
    {
        mSurfaceHolder = getHolder();
        mSurfaceHolder.addCallback(this);
        setFocusable(true);

        mPaint = new Paint();
        mPaint.setColor(Color.WHITE);
        mPaint.setAntiAlias(true);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        screenW = this.getWidth();
        screenH = this.getHeight();
        Log.e("TAG", "获取的屏幕的宽度 " + screenW);
        Log.e("TAG", "获取的屏幕的宽度 " + screenH);
        flag = true;
        //启动线程
        th = new Thread(this);
        //启动线程
        th.start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        flag = false;
    }

    @Override
    public void run()
    {
        while (flag)
        {
            long start = System.currentTimeMillis();
            myDraw();
            long end = System.currentTimeMillis();
            try
            {
                if (end - start < 30)
                {
                    Thread.sleep(30 - (end - start));
                }
            } catch (InterruptedException e)
            {
                e.printStackTrace();
            }
        }
    }

    private void myDraw()
    {
        try
        {
            canvas = mSurfaceHolder.lockCanvas();
            if (canvas != null)
            {
                canvas.drawColor(Color.WHITE);
                mPaint.setColor(Color.BLUE);
                if (arc_x < 0)
                {
                    arc_x = 0;
                    if (arc_y < 0)
                    {
                        arc_y = 0;
                    } else if (arc_y + mBottom > screenH)
                    {
                        arc_y = screenH - mBottom;
                    }
                } else if (arc_y < 0)
                {
                    arc_y = 0;
                    if (arc_x + mRight > screenW)
                    {
                        arc_x = screenW - mRight;
                    }
                } else if (arc_x + mRight > screenW)
                {
                    arc_x = screenW - mRight;
                    if (arc_y + mBottom > screenH)
                    {
                        arc_y = screenH - mBottom;
                    }
                } else if (arc_y + mBottom > screenH)
                {
                    arc_y = screenH - mBottom;
                }

                canvas.drawArc(new RectF(arc_x, arc_y, arc_x + mRight, arc_y + mBottom), 0, 360, true, mPaint);
                mPaint.setTextSize(10);
            }
        } catch (Exception e)
        {
            // TODO: handle exception
        } finally
        {
            if (canvas != null)
                mSurfaceHolder.unlockCanvasAndPost(canvas);
        }
    }

    /**
     * 触屏事件监听
     */
    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        return true;
    }

    /**
     * 按键事件监听
     */
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        return super.onKeyDown(keyCode, event);
    }

    public void SensorChange(SensorEvent event)
    {
        float x = event.values[0];
        //x>0 说明当前手机左翻 x<0右翻
        float y = event.values[1];
        //y>0 说明当前手机下翻 y<0上翻
        float z = event.values[2];
        //z>0 手机屏幕朝上 z<0 手机屏幕朝下
        arc_x = (int) (x / 9.8 * getWidth() / 3) + getWidth() / 2;
        arc_y = (int) -(y / 9.8 * getHeight() / 3) + getHeight() / 2;
    }
}
