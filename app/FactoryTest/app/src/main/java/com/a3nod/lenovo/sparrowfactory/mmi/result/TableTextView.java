package com.a3nod.lenovo.sparrowfactory.mmi.result;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.widget.TextView;

/**
 * Created by Lenovo on 2017/11/14.
 */

public class TableTextView extends TextView {
    Paint paint = new Paint();
public TableTextView(Context context, AttributeSet attrs) {
        super(context, attrs);
        int color = Color.parseColor("#999999");
       // 为边框设置颜色
        paint.setColor(color);
       }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        // 画TextView的4个边
        canvas.drawLine(0, 0, this.getWidth() - 1, 0, paint);
        canvas.drawLine(0, 0, 0, this.getHeight() - 1, paint);
        canvas.drawLine(this.getWidth() - 1, 0, this.getWidth() - 1, this.getHeight() - 1, paint);
        canvas.drawLine(0, this.getHeight() - 1, this.getWidth() - 1, this.getHeight() - 1, paint);
    }
}
