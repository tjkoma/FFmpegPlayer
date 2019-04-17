package com.example.administrator.ffmpegplayer;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class VideoView extends SurfaceView {
    public VideoView(Context context) {
        super(context);
    }

    public VideoView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public VideoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    private void init() {
        SurfaceHolder surfaceHolder = getHolder();
        //RGBA_8888 色彩丰富，但是占用的内存大
        surfaceHolder.setFormat(PixelFormat.RGBA_8888);
    }

}
