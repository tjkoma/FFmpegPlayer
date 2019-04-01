package com.example.administrator.ffmpegplayer;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private EditText edittext1, edittext2;
    private TextView confirm_tv;

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

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        edittext1 = findViewById(R.id.edittext1);
        edittext2 = findViewById(R.id.edittext2);
        confirm_tv = findViewById(R.id.confirm_tv);
        tv.setText(getAvcodecInfo());
        confirm_tv.setOnClickListener(v ->
                tv.setText(division(Integer.parseInt(edittext1.getText().toString()), Integer.parseInt(edittext2.getText().toString())) + "")
        );

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
}
