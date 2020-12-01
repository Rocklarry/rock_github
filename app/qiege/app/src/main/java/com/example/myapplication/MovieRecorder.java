package com.example.myapplication;

import android.media.MediaRecorder;
import android.view.SurfaceView;

import java.io.File;
import java.io.IOException;
import java.util.Timer;
import java.util.TimerTask;

public class MovieRecorder {
    private MediaRecorder mediarecorder;
    boolean isRecording;
    public void startRecording(SurfaceView surfaceView){
        mediarecorder = new MediaRecorder();
        mediarecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
        mediarecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        //mediarecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
        mediarecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
        mediarecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
        mediarecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
        mediarecorder.setVideoSize(1280,720);
        mediarecorder.setVideoFrameRate(25);
        mediarecorder.setPreviewDisplay(surfaceView.getHolder().getSurface());
        mediarecorder.setOrientationHint(90);
        lastFileName = newFileName();
        mediarecorder.setOutputFile(lastFileName);

        try {
            mediarecorder.prepare();
            mediarecorder.start();
        } catch (IOException e) {
            e.printStackTrace();
        }


        isRecording = true;
        timeSize = 0;
        timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                timeSize++;
            }
        },0,1000);


    }
    Timer timer;
    int timeSize = 0;
    private String lastFileName;
    public void stopRecording(){
        if(mediarecorder != null){
            mediarecorder.stop();
            mediarecorder.release();
            mediarecorder = null;
            timer.cancel();
            if(null != lastFileName && !"".equals(lastFileName)){
                File f = new File(lastFileName);
                String name = f.getName().substring(0,
                        f.getName().lastIndexOf(".3gp"));
                name +="_"+ timeSize + "s.3gp";
                String newPath = f.getParentFile().getAbsolutePath() + "/"
                        + name;
                if(f.renameTo(new File(newPath))){
                    int i = 0;
                    i++;
                }
            }
        }
    }

    public String newFileName(){

        try {
            return File.createTempFile("/mov_",".3gp").getAbsolutePath();
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }

    }

    public void release(){
        if(mediarecorder != null){
            mediarecorder.stop();
            mediarecorder.release();
            mediarecorder = null;
        }
    }

}














