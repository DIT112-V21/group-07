package com.example.pathfinder.Activities;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import java.sql.DataTruncation;

//derived from https://www.instructables.com/A-Simple-Android-UI-Joystick/

public class ThumbstickView extends SurfaceView implements SurfaceHolder.Callback, View.OnTouchListener {
    private float centerX;
    private float centerY;
    private float baseRadius;
    private float hatRadius;
    private ThumbstickListener thumbstickCallback;
    private final int ratio = 5;

    private void setupDimensions() {
        centerX = getWidth() / 2;
        centerY = getHeight() / 2;
        baseRadius = Math.min(getWidth(), getHeight()) / 3;
        hatRadius = Math.min(getWidth(), getHeight()) / 5;
    }

    public ThumbstickView(Context context) {
        super (context);
        getHolder().addCallback(this);
        setOnTouchListener((OnTouchListener) this);
        if ( context instanceof ThumbstickListener ) {
            thumbstickCallback = (ThumbstickListener) context;
        }
    }

    public ThumbstickView(Context context, AttributeSet attributes, int style) {
        super (context, attributes, style);
        getHolder().addCallback(this);
        setOnTouchListener(this);
    }

    public ThumbstickView (Context context, AttributeSet attributes) {
        super (context, attributes);
        getHolder().addCallback(this);
        setOnTouchListener((OnTouchListener) this);
    }

    private void drawThumbstick (float newX, float newY) {
        if ( getHolder().getSurface().isValid() ) {
            Canvas myCanvas = this.getHolder().lockCanvas();
            Paint colors = new Paint();
            colors.setARGB(255, 255, 255, 255);
            myCanvas.drawCircle(centerX, centerY, baseRadius, colors);
            colors.setARGB(255, 179, 128, 217);
            myCanvas.drawCircle(newX, newY, hatRadius, colors);
            getHolder().unlockCanvasAndPost(myCanvas);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        setupDimensions();
        drawThumbstick(centerX, centerY);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    public boolean onTouch(View v, MotionEvent e) {
        if ( v.equals(this) ) {
            if ( e.getAction() != e.ACTION_UP) {
                float displacement= (float) Math.sqrt((Math.pow(e.getX() - centerX, 2)) + Math.pow(e.getY() - centerY, 2)) ;
                if ( displacement < baseRadius ) {
                    drawThumbstick(e.getX(), e.getY());
                    thumbstickCallback.onThumbstickMoved((e.getX() - centerX)/baseRadius, (e.getY() - centerY)/baseRadius,getId());
                } else {
                    float ratio = baseRadius / displacement;
                    float constrainedX = centerX + (e.getX() - centerX) * ratio;
                    float constrainedY = centerY + (e.getY() - centerY) * ratio;
                    drawThumbstick(constrainedX, constrainedY);
                    thumbstickCallback.onThumbstickMoved((constrainedX-centerX)/ baseRadius, (constrainedY-centerY)/baseRadius, getId());
                }
            } else {
                drawThumbstick(centerX, centerY);
                thumbstickCallback.onThumbstickMoved(0, 0, getId());
            }
        }
        return true;
    }

    public interface ThumbstickListener {
        void onThumbstickMoved(float xPercent, float yPercent, int id);
    }
}

