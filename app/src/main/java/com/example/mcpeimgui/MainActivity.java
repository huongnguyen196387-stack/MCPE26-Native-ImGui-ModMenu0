package com.example.mcpeimgui;

import android.app.Activity;
import android.os.Bundle;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends Activity {

    static {
        System.loadLibrary("modmenu");
    }

    private GLSurfaceView glView;

    private static native void nativeInit(int width, int height);
    private static native void nativeRender();
    private static native void nativeTouch(
            int action,
            float x,
            float y
    );

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        glView = new GLSurfaceView(this);
        glView.setEGLContextClientVersion(3);

        glView.setRenderer(new GLSurfaceView.Renderer() {

            @Override
            public void onSurfaceCreated(
                    GL10 gl,
                    EGLConfig config
            ) {
                nativeInit(
                        glView.getWidth(),
                        glView.getHeight()
                );
            }

            @Override
            public void onSurfaceChanged(
                    GL10 gl,
                    int width,
                    int height
            ) {
                nativeInit(width, height);
            }

            @Override
            public void onDrawFrame(GL10 gl) {
                nativeRender();
            }
        });

        glView.setRenderMode(
                GLSurfaceView.RENDERMODE_CONTINUOUSLY
        );

        glView.setOnTouchListener((view, event) -> {
            nativeTouch(
                    event.getActionMasked(),
                    event.getX(),
                    event.getY()
            );
            return true;
        });

        setContentView(glView);
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (glView != null) {
            glView.onPause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (glView != null) {
            glView.onResume();
        }
    }
}
