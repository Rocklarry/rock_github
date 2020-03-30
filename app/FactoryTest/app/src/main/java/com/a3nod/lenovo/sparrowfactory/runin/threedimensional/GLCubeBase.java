package com.a3nod.lenovo.sparrowfactory.runin.threedimensional;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLU;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGL11;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;


public abstract class GLCubeBase extends SurfaceView implements SurfaceHolder.Callback, Runnable
{
    protected EGLContext glContext;
    protected CubeAnimator animator;
    protected SurfaceHolder sHolder;
    protected Thread t;
    protected boolean running;
    int width;
    int height;
    boolean resize;
    int fps;

    protected static FloatBuffer makeFloatBuffer(float[] arr)
    {
        ByteBuffer bb = ByteBuffer.allocateDirect(arr.length * 4);
        bb.order(ByteOrder.nativeOrder());
        FloatBuffer fb = bb.asFloatBuffer();
        fb.put(arr);
        fb.position(0);
        return fb;
    }

    protected static IntBuffer makeFloatBuffer(int[] arr)
    {
        ByteBuffer bb = ByteBuffer.allocateDirect(arr.length * 4);
        bb.order(ByteOrder.nativeOrder());
        IntBuffer ib = bb.asIntBuffer();
        ib.put(arr);
        ib.position(0);
        return ib;
    }

    protected static int loadTexture(GL10 gl, Bitmap bmp)
    {
        return loadTexture(gl, bmp, false);
    }

    protected static int loadTexture(GL10 gl, Bitmap bmp, boolean reverseRGB)
    {
        ByteBuffer bb = ByteBuffer.allocateDirect(bmp.getHeight() * bmp.getWidth() * 4);
        bb.order(ByteOrder.BIG_ENDIAN);
        IntBuffer ib = bb.asIntBuffer();

        for (int y = bmp.getHeight() - 1; y > -1; y--)
            for (int x = 0; x < bmp.getWidth(); x++)
            {
                int pix = bmp.getPixel(x, bmp.getHeight() - y - 1);
                // Convert ARGB -> RGBA
                @SuppressWarnings("unused")
                byte alpha = (byte) ((pix >> 24) & 0xFF);
                byte red = (byte) ((pix >> 16) & 0xFF);
                byte green = (byte) ((pix >> 8) & 0xFF);
                byte blue = (byte) ((pix) & 0xFF);

                ib.put(red << 24 | green << 16 | blue << 8 | 0xFF);
            }
        ib.position(0);
        bb.position(0);

        int[] tmp_tex = new int[1];

        gl.glGenTextures(1, tmp_tex, 0);
        int tex = tmp_tex[0];
        gl.glBindTexture(GL10.GL_TEXTURE_2D, tex);
        gl.glTexImage2D(GL10.GL_TEXTURE_2D, 0, GL10.GL_RGBA, bmp.getWidth(), bmp.getHeight(), 0, GL10.GL_RGBA, GL10.GL_UNSIGNED_BYTE, bb);
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);

        return tex;
    }

    public GLCubeBase(Context c)
    {
        this(c, -1);
    }

    public GLCubeBase(Context c, int fps)
    {
        super(c);
        sHolder = getHolder();
        sHolder.addCallback(this);
        sHolder.setType(SurfaceHolder.SURFACE_TYPE_GPU);
        this.fps = fps;
    }

    @Override
    protected void onAttachedToWindow()
    {
        if (animator != null)
        {
            // If we're animated, start the animation
            animator.start();
        }
        super.onAttachedToWindow();
    }

    @Override
    protected void onDetachedFromWindow()
    {
        if (animator != null)
        {
            animator.stop();
        }
        super.onDetachedFromWindow();
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int width,
                               int height)
    {
        synchronized (this)
        {
            this.width = width;
            this.height = height;
            this.resize = true;
        }
    }

    public void surfaceCreated(SurfaceHolder holder)
    {
        t = new Thread(this);
        t.start();
    }

    public void surfaceDestroyed(SurfaceHolder arg0)
    {
        running = false;
        try
        {
            t.join();
        } catch (InterruptedException ex)
        {
            ex.printStackTrace();
        }
        t = null;
    }

    public void run()
    {
        EGL10 egl = (EGL10) EGLContext.getEGL();
        EGLDisplay dpy = egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);

        int[] version = new int[2];
        egl.eglInitialize(dpy, version);

        int[] configSpec = {
                EGL10.EGL_RED_SIZE, 5,
                EGL10.EGL_GREEN_SIZE, 6,
                EGL10.EGL_BLUE_SIZE, 5,
                EGL10.EGL_DEPTH_SIZE, 16,
                EGL10.EGL_NONE
        };

        EGLConfig[] configs = new EGLConfig[1];
        int[] num_config = new int[1];
        egl.eglChooseConfig(dpy, configSpec, configs, 1, num_config);
        EGLConfig config = configs[0];

        EGLContext context = egl.eglCreateContext(dpy, config,
                EGL10.EGL_NO_CONTEXT, null);

        EGLSurface surface = egl.eglCreateWindowSurface(dpy, config, sHolder, null);
        egl.eglMakeCurrent(dpy, surface, surface, context);

        GL10 gl = (GL10) context.getGL();

        init(gl);

        int delta = -1;
        if (fps > 0)
        {
            delta = 1000 / fps;
        }
        long time = System.currentTimeMillis();

        running = true;
        while (running)
        {
            int w, h;
            synchronized (this)
            {
                w = width;
                h = height;
            }
            if (System.currentTimeMillis() - time < delta)
            {
                try
                {
                    Thread.sleep(System.currentTimeMillis() - time);
                } catch (InterruptedException ex)
                {
                    ex.printStackTrace();
                }
            }
            drawFrame(gl, w, h);
            egl.eglSwapBuffers(dpy, surface);

            if (egl.eglGetError() == EGL11.EGL_CONTEXT_LOST)
            {
                Context c = getContext();
                if (c instanceof Activity)
                {
                    ((Activity) c).finish();
                }
            }
            time = System.currentTimeMillis();
        }
        egl.eglMakeCurrent(dpy, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
        egl.eglDestroySurface(dpy, surface);
        egl.eglDestroyContext(dpy, context);
        egl.eglTerminate(dpy);
    }

    private void drawFrame(GL10 gl, int w, int h)
    {
        if (resize)
        {
            resize(gl, w, h);
            resize = false;
        }
        drawFrame(gl);
    }

    protected void resize(GL10 gl, int w, int h)
    {
        gl.glMatrixMode(GL10.GL_PROJECTION);
        gl.glLoadIdentity();
        gl.glViewport(0, 0, w, h);
        GLU.gluPerspective(gl, 45.0f, ((float) w) / h, 1f, 100f);
    }

    protected void init(GL10 gl)
    {
    }

    protected abstract void drawFrame(GL10 gl);
}