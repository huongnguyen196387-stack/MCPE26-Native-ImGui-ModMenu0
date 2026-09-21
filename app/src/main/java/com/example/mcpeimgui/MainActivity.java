package com.example.mcpeimgui;

import android.app.Activity;
import android.os.Bundle;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;

public final class MainActivity extends Activity {
    static { System.loadLibrary("modmenu"); }

    private static native void nativeInit();
    private static native void nativeResize(int width, int height);
    private static native void nativeRender();
    private static native void nativeTouch(int action, float x, float y);

    @Override
    protected void onCreate(Bundle state) {
        super.onCreate(state);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        );

        GLSurfaceView view = new GLSurfaceView(this);
        view.setEGLContextClientVersion(3);
        view.setEGLConfigChooser(8, 8, 8, 8, 16, 0);

        view.setRenderer(new GLSurfaceView.Renderer() {
            @Override public void onSurfaceCreated(
                    javax.microedition.khronos.egl.EGLConfig config) {
                nativeInit();
            }
            @Override public void onSurfaceChanged(
                    javax.microedition.khronos.egl.EGLConfig config,
                    int width, int height) {
                nativeResize(width, height);
            }
            @Override public void onDrawFrame(
                    javax.microedition.khronos.opengles.GL10 gl) {
                nativeRender();
            }
        });

        view.setOnTouchListener((v, event) -> {
            int a = event.getActionMasked();
            if (a == MotionEvent.ACTION_DOWN ||
                a == MotionEvent.ACTION_UP ||
                a == MotionEvent.ACTION_MOVE) {
                nativeTouch(a, event.getX(), event.getY());
            }
            return true;
        });

        setContentView(view);
    }
}
