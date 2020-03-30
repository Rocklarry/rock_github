package com.a3nod.lenovo.sparrowfactory.mmi.tp;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.DrawFilter;
import android.graphics.Paint;
import android.graphics.PaintFlagsDrawFilter;
import android.graphics.Path;
import android.graphics.Point;
import android.graphics.RectF;
import android.graphics.Region;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;



/**
 * Description
 * Created by aaa on 2017/11/27.
 */

public class TPTestView extends View
{
    int numWidth = 9;
    int numHeight = 16;
    List<MyRect> lr;
    List<MyRect> lrs;
    List<Point> lp;
    MyPools.SimplePool<Point> pathPool;

    Paint rectPaintSolidWhite;
    Paint rectPaintSolidGreen;
    Paint rectPaintBorder;
    Paint pathPaint;

    //是否绘制左右斜线
    boolean isLeftSlash = false;
    boolean isRightSlash = false;

    //触点轨迹
    Path drawPath = new Path();

    //右斜线
    Path rightSlashPath;
    Region rightSlashRegion = new Region();
    //左斜线
    Path leftSlashPath;
    Region leftSlashRegion = new Region();

    //整个视图区域
    Region viewRegion = new Region();

    int[] vetex = new int[4];
    DrawFilter drawFilter;
    TestSuccessCallback callback;

    private TimerTask tt;
    private Timer tr;
    private long waittingTime = 2*60*1000;

    @Override
    protected void onAttachedToWindow()
    {
        super.onAttachedToWindow();
        pathPool=new MyPools.SimplePool<>(100);
    }

    @Override
    protected void onDetachedFromWindow()
    {
        super.onDetachedFromWindow();
        pathPool.clear();
    }

    public TPTestView(Context context, @Nullable AttributeSet attrs)
    {
        super(context, attrs);
        lr = new ArrayList<>();
        lp = new ArrayList<>();
        lrs = new ArrayList<>();

        int size = (numWidth + numHeight) * 2 - 4;
        for (int i = 0; i < size; i++)
        {
            lr.add(new MyRect());
            lrs.add(new MyRect());
        }
        //消除锯齿
        drawFilter = new PaintFlagsDrawFilter(0, Paint.ANTI_ALIAS_FLAG | Paint.FILTER_BITMAP_FLAG);

        clearPath();

        //触摸移动轨迹画笔
        pathPaint = new Paint();
        pathPaint.setColor(Color.BLUE);
        pathPaint.setStrokeWidth(10);
        pathPaint.setStyle(Paint.Style.STROKE);
        pathPaint.setStrokeJoin(Paint.Join.ROUND);
        pathPaint.setStrokeCap(Paint.Cap.ROUND);

        //实心矩形框画笔  白色和绿色
        rectPaintSolidWhite = new Paint();
        rectPaintSolidWhite.setColor(Color.WHITE);
        rectPaintSolidGreen = new Paint();
        rectPaintSolidGreen.setColor(Color.GREEN);


        //空心矩形框画笔 黑泽
        rectPaintBorder = new Paint();
        rectPaintBorder.setStrokeWidth(2);
        rectPaintBorder.setStyle(Paint.Style.STROKE);
        rectPaintBorder.setColor(Color.BLACK);
        rectPaintBorder.setAntiAlias(false);

        startTimerTask();
    }

    private void startTimerTask(){
        tt=new TimerTask() {
            @Override
            public void run() {
                handler.sendEmptyMessage(0);

            }
        };
       tr=new Timer();
       tr.schedule(tt,waittingTime);
    }

    public void cancelTimerTask(){ 
        tr.cancel();
        tr=null;
        tt.cancel();
        tt=null;
    }

    private Handler handler=new Handler(){
     @Override
     public void handleMessage(Message msg) {
         super.handleMessage(msg);
         if(callback!=null) {
             callback.onTestFinish(false);
         }
     }
    };

    public void setTestFinishCallback(TestSuccessCallback callback){
        this.callback=callback;
    }
    public interface  TestSuccessCallback{
        void onTestFinish(boolean isSuccess);
    }
    @Override
    protected void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);
        canvas.setDrawFilter(drawFilter);

        int width = getMeasuredWidth();
        int height = getMeasuredHeight();

        LogUtil.i(" tp test width : "+ width + "  height : "+height);

        float childWidth = width*1f / numWidth;
        float childHeight = height*1f / numHeight;
        viewRegion.set(0, 0, width, height);
/*
        canvas.skew(0.5f * childWidth / childHeight, 0); // 画布倾斜
        rightSlash.left=0;
        rightSlash.top=0;
        rightSlash.right=childWidth;
        rightSlash.bottom=numHeight * childHeight;
        canvas.drawRect(rightSlash, isRightSlash ? rectPaintSolidGreen : rectPaintBorder);

        canvas.skew(-1.0f * childWidth / childHeight, 0); // 倾斜
        leftSlash.left=(numWidth - 1) * childWidth;
        leftSlash.top=0;
        leftSlash.right=numWidth * childWidth;
        leftSlash.bottom=numHeight * childHeight;
        canvas.drawRect(leftSlash, isLeftSlash ? rectPaintSolidGreen : rectPaintBorder);

        canvas.skew(0.5f * childWidth / childHeight, 0); // 倾斜
*/

        //画右斜线 如果不做这个判断  会很卡很卡 截图太耗时间
        if (rightSlashPath == null)
        {
            rightSlashPath = new Path();
            rightSlashPath.moveTo(0, 0);
            rightSlashPath.lineTo(childWidth, 0);
            rightSlashPath.lineTo(width, height);
            rightSlashPath.lineTo(width - childWidth, height);
            rightSlashPath.close();
            rightSlashRegion.setPath(rightSlashPath, viewRegion);
        }
/*
        canvas.save();
        canvas.clipPath(rightSlashPath);
        canvas.drawPath(rightSlashPath, isRightSlash ? rectPaintSolidGreen : rectPaintBorder);
        canvas.restore();
*/

        canvas.drawPath(rightSlashPath, isRightSlash ? rectPaintSolidGreen : rectPaintBorder);

        //画左斜线
        if (leftSlashPath == null)
        {
            leftSlashPath = new Path();
            leftSlashPath.moveTo(width, 0);
            leftSlashPath.lineTo(width - childWidth, 0);
            leftSlashPath.lineTo(0, height);
            leftSlashPath.lineTo(childWidth, height);
            leftSlashPath.close();

            leftSlashRegion.setPath(leftSlashPath, viewRegion);
        }
        canvas.drawPath(leftSlashPath, isLeftSlash ? rectPaintSolidGreen : rectPaintBorder);
/*        canvas.save();
        canvas.clipPath(leftSlashPath);
        canvas.drawPath(leftSlashPath, isLeftSlash ? rectPaintSolidGreen : rectPaintBorder);
        canvas.restore();*/

        int k = 0;
        for (int j = 0; j < numHeight; j++)
        {
            for (int i = 0; i < numWidth; i++)
            {
                if (j == 0 || i == 0 || (j + 1) == numHeight || (i + 1) == numWidth)
                {
                    MyRect rect = lr.get(k);
                    rect.left = i * childWidth;
                    rect.right = (i + 1) * childWidth;
                    rect.top = j * childHeight;
                    rect.bottom = (j + 1) * childHeight;
                    canvas.drawRect(rect, rectPaintBorder);
                    LogUtil.i("left : "+ rect.left+ "   right : "+rect.right + "   top  :  "+rect.top + "  bottom : "+ rect.bottom);

                    MyRect rect1 = lrs.get(k);
                    rect1.left = i * childWidth + 1;
                    rect1.right = (i + 1) * childWidth - 1;
                    rect1.top = j * childHeight + 1;
                    rect1.bottom = (j + 1) * childHeight - 1;
                    canvas.drawRect(rect1, rect1.isDraw ? rectPaintSolidGreen : rectPaintSolidWhite);

                    k++;
                }
            }
        }
//        canvas.drawRect(lrs.get(lrs.size()), rectPaintSolidGreen);
//        canvas.drawRect(lr.get(lr.size()-1), rectPaintSolidGreen);

        if (drawPath != null)
        {
            canvas.drawPath(drawPath, pathPaint);
        }

    }

    class MyRect extends RectF
    {
        boolean isDraw = false;
    }

    private long lastMillis=0;
    float lastX,lastY;
    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        LogUtil.i("  event type " + event.getAction() + ": " + event.getX() + "," + event.getY());
        float x = event.getX();
        float y = event.getY();
        Point p=pathPool.acquire();
        if(p==null)
        {
            p=new Point((int)x,(int)y);
        }else{
            p.set((int)x,(int)y);
        }
        switch (event.getAction())
        {
            case MotionEvent.ACTION_DOWN:
                drawPath = new Path();
                clearPath();
                drawPath.moveTo(x, y);
                lp.add(p);
                for (int i = 0; i < lr.size(); i++)
                {
                    if (lr.get(i).contains(x, y))
                    {
                        lrs.get(i).isDraw = true;
                        LogUtil.i("  event type  invalidate");
                        invalidate();
                        break;
                    }
                }
                lastX=x;
                lastY=y;
                break;
            case MotionEvent.ACTION_MOVE:
                //记录移动过的点 , 后期用于判断是否偏离了左右斜线范围
                lp.add(p);
                if (Math.abs(lastX - x) < 3 && Math.abs(lastY - y) < 3) {

                } else {
                    //贝塞尔曲线
                    drawPath.quadTo(lastX, lastY, (x + lastX) / 2, (y + lastY) / 2);
                    lastX=x;
                    lastY=y;
                }
//                drawPath.lineTo(x, y);
                for (int i = 0; i < lr.size(); i++)
                {
                    if (lr.get(i).contains(x, y))
                    {
                        lrs.get(i).isDraw = true;
                        LogUtil.i("  event type  invalidate");
                        break;
                    }
                }

                //判断是否是画斜线
                if (lr.get(0).contains(x, y))
                {
                    LogUtil.i("aaaaaaa 点在左上角");
                    //如果当前点在左上角  那么判断之前有没有点到过右下角
                    if (vetex[3] >= 0)
                    {
                        //如果点到过右下角  那么判断这两个点之间是否都在斜线上
                        boolean isOutOfRightSlash = false;
                        for (int i = vetex[3]; i < lp.size(); i++)
                        {
                            if (!rightSlashRegion.contains(lp.get(i).x, lp.get(i).y))
                            {
                                isOutOfRightSlash = true;
                            }
                        }
                        if (!isOutOfRightSlash)
                        {
                            isRightSlash = true;
                        } else
                        {
                            vetex[0] = lp.size();
                        }
                    } else
                    {
                        LogUtil.i("aaaaaaa 点在左上 : " + vetex[0]);
                        vetex[0] = lp.size();
                    }
                } else if (lr.get(lr.size() - 1).contains(x, y))
                {
                    LogUtil.i("aaaaaaa 点在右下角");
                    //如果当前点在右下角  那么判断之前有没有点到过左上角
                    if (vetex[0] >= 0)
                    {
                        //如果点到过左上角  那么判断这两个点之间是否都在斜线上
                        boolean isOutOfRightSlash = false;
                        for (int i = vetex[0]; i < lp.size(); i++)
                        {
                            LogUtil.e(" is out of slash : " + isOutOfRightSlash);
                            if (!rightSlashRegion.contains(lp.get(i).x, lp.get(i).y))
                            {
                                LogUtil.i("aaaaaaa ds fas awisOutOfRightSlash  : " + isOutOfRightSlash);
                                isOutOfRightSlash = true;
                                break;
                            }
                        }
                        LogUtil.i("aaaaaaa 点在左上isOutOfRightSlash : " + isOutOfRightSlash);
                        if (!isOutOfRightSlash)
                        {
                            isRightSlash = true;
                        } else
                        {
                            vetex[3] = lp.size();
                        }
                    } else
                    {
                        vetex[3] = lp.size();
                    }
                } else if (lr.get(numWidth - 1).contains(x, y))
                {
                    LogUtil.i("aaaaaaa 点在右上角");
                    //如果当前点在右上角  那么判断之前有没有点到过左下角
                    if (vetex[2] >= 0)
                    {
                        LogUtil.i("aaaaaaa vetex[2] >0 ");
                        //如果点到过右上角  那么判断这两个点之间是否都在斜线上
                        boolean isOutOfLeftSlash = false;
                        for (int i = vetex[2]; i < lp.size(); i++)
                        {
                            if (!leftSlashRegion.contains(lp.get(i).x, lp.get(i).y))
                            {
                                LogUtil.i("aaaaaaaisOutOfLeftSlash");
                                isOutOfLeftSlash = true;
                            }
                        }
                        if (!isOutOfLeftSlash)
                        {
                            isLeftSlash = true;
                        } else
                        {
                            vetex[1] = lp.size();
                        }
                    } else
                    {
                        vetex[1] = lp.size();
                    }
                } else if (lr.get(lr.size() - numWidth).contains(x, y))
                {
                    LogUtil.i("aaaaaaa 点在左下角");
                    //如果当前点在左下角  那么判断之前有没有点到过右上角
                    if (vetex[1] >= 0)
                    {
                        //如果点到过左下角  那么判断这两个点之间是否都在斜线上
                        boolean isOutOfLeftSlash = false;
                        for (int i = vetex[1]; i < lp.size(); i++)
                        {
                            if (!leftSlashRegion.contains(lp.get(i).x, lp.get(i).y))
                            {
                                isOutOfLeftSlash = true;
                            }
                        }
                        if (!isOutOfLeftSlash)
                        {
                            isLeftSlash = true;
                        } else
                        {
                            vetex[2] = lp.size();
                        }
                    } else
                    {
                        vetex[2] = lp.size();
                    }
                }

                invalidate();
                break;
            case MotionEvent.ACTION_UP:
                drawPath = null;
                //对象回收到对象池
                for(int i=0,j=lp.size();i<j;i++){
                    pathPool.release(lp.get(i));
                }

                lp.clear();
                invalidate();
                clearPath();
                if(isAllTouched())
                {
                    if(callback!=null){
                        callback.onTestFinish(true);
                    }
                }else
                {
                    //双击退出
                  // long currentMillis = System.currentTimeMillis();
                  //  if (currentMillis - lastMillis > 300)
                  //  {
                   //     lastMillis = currentMillis;
                   // } else
                   //  {
                   //     callback.onTestFinish(false);
                   // }
                }
                break;
        }

//        mis.put(id++, new SoftReference<Point>(new Point((int) event.getX(), (int) event.getY())));
        return true;
    }

    public boolean isAllTouched()
    {
        if (isLeftSlash && isRightSlash)
        {
            for (int i = 0; i < lrs.size(); i++)
            {
                if(!lrs.get(i).isDraw){
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    public void clearPath()
    {
        for (int i = 0; i < vetex.length; i++)
        {
            vetex[i] = -1;
        }
    }

}
