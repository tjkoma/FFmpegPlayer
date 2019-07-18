package com.example.administrator.ffmpegplayer;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import cn.jzvd.JzvdStd;

public class MainActivity extends AppCompatActivity {

    private EditText edittext1, edittext2;
    private TextView confirm_tv;
    private SurfaceView surface_view;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("multi");
        System.loadLibrary("division");
//        System.loadLibrary("avcodec57");
//        System.loadLibrary("avdevice57");
//        System.loadLibrary("avfilter6");
//        System.loadLibrary("avformat57");
//        System.loadLibrary("avutil55");
//        System.loadLibrary("swresample2");
//        System.loadLibrary("swscale4");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        checkPermission();
        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        edittext1 = findViewById(R.id.edittext1);
        edittext2 = findViewById(R.id.edittext2);
        confirm_tv = findViewById(R.id.confirm_tv);
        surface_view = findViewById(R.id.surface_view);
        tv.setText(getAvcodecInfo());
        confirm_tv.setOnClickListener(v ->
//                tv.setText(division(Integer.parseInt(edittext1.getText().toString()), Integer.parseInt(edittext2.getText().toString())) + "")
                {
                    String folderurl = Environment.getExternalStorageDirectory().getPath();
                    String urltext_input = edittext1.getText().toString();
                    String inputurl = folderurl + "/" + urltext_input;
                    String urltext_output = edittext2.getText().toString();
                    String outputurl = folderurl + "/" + urltext_output;
                    Log.e("inputurl", inputurl);
                    Log.e("outputurl", outputurl);
//                    tv.setText(decode(inputurl, outputurl) + "");
                }
        );


    }

    //android6.0之后要动态获取权限
    private void checkPermission() {
        // Storage Permissions
        final int REQUEST_EXTERNAL_STORAGE = 1;
        String[] PERMISSIONS_STORAGE = {
                Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.WRITE_EXTERNAL_STORAGE};

        try {
            //检测是否有写的权限
            int permission = ActivityCompat.checkSelfPermission(this,
                    "android.permission.WRITE_EXTERNAL_STORAGE");
            if (permission != PackageManager.PERMISSION_GRANTED) {
                // 没有写的权限，去申请写的权限，会弹出对话框
                ActivityCompat.requestPermissions(this, PERMISSIONS_STORAGE, REQUEST_EXTERNAL_STORAGE);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }


    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native int add(int a, int b);

    public native int multi(int a, int b);

    public native int division(int a, int b);

    public native String getAvcodecInfo();

    public native void play(String path, Object object);

    public native int decode(String inputurl, String outputurl);

    public native int playVideo(String path, Object obj);
}
