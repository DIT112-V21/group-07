package com.example.pathfinder.Activities;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import androidx.annotation.NonNull;

import java.sql.DataTruncation;

//derived from https://www.instructables.com/A-Simple-Android-UI-Joystick/

public class ThumbstickView extends SurfaceView implements SurfaceHolder.Callback, View.OnTouchListener {
    private float centerX;
    private float centerY;
    private float baseRadius;
    private float hatRadius;
    private ThumbstickListener thumbstickCallback;
    private final int ratio = 5;

    /**
     * Dimensions of thumbstick
     */
    private void setupDimensions() {
        centerX = getWidth() / 2;
        centerY = getHeight() / 2;
        baseRadius = Math.min(getWidth(), getHeight()) / 3;
        hatRadius = Math.min(getWidth(), getHeight()) / 5;
    }

    /**
     * Constructor
     * @param context
     */
    public ThumbstickView(Context context) {
        super(context);
        getHolder().addCallback(this);
        setOnTouchListener( this);
        if ( context instanceof ThumbstickListener ) {
            thumbstickCallback = (ThumbstickListener) context;
        }
    }

    /**
     * Constructor
     * @param context
     * @param attributes
     * @param style
     */
    public ThumbstickView(Context context, AttributeSet attributes, int style) {
        super(context, attributes, style);
        getHolder().addCallback(this);
        setOnTouchListener(this);
        if ( context instanceof ThumbstickListener ) {
            thumbstickCallback = (ThumbstickListener) context;
        }
    }

    /**
     * Constructor
     * @param context
     * @param attributes
     */
    public ThumbstickView (Context context, AttributeSet attributes) {
        super(context, attributes);
        getHolder().addCallback(this);
        setOnTouchListener(this);
        if ( context instanceof ThumbstickListener ) {
            thumbstickCallback = (ThumbstickListener) context;
        }
    }

    /**
     * Method to draw the thumbstick, includes measurements and colours
     * @param newX
     * @param newY
     */
    private void drawThumbstick (float newX, float newY) {
        if ( getHolder().getSurface().isValid() ) {
            Canvas myCanvas = this.getHolder().lockCanvas();
            myCanvas.drawColor(Color.parseColor("#ffffff"));
            Paint colors = new Paint();

            //determines sin and cos of angle that touches point relative to joystick
            //SohCahToa
            float hypotenuse = (float) Math.sqrt(Math.pow(newX - centerY, 2) + Math.pow(newY - centerY, 2));
            float sin = ( newY - centerY ) / hypotenuse;
            float cos = ( newX - centerX ) / hypotenuse;


            colors.setARGB(255, 0, 0, 0); //stroke
            myCanvas.drawCircle(centerX, centerY, (baseRadius + 10), colors);
            colors.setARGB(255, 179, 128, 217); //fill

            myCanvas.drawCircle(centerX, centerY, baseRadius, colors);
            for( int i = 1; i <= (int) (baseRadius / ratio); i++ ) {
                colors.setARGB(150/i, 255, 0, 0);
                myCanvas.drawCircle(newX - cos * hypotenuse * (ratio/baseRadius) * i,
                        newY - sin * hypotenuse * (ratio/baseRadius) * i, i * (hatRadius * ratio / baseRadius), colors);
            }

            colors.setARGB(255, 0, 0, 0); //stroke
            myCanvas.drawCircle(newX, newY, (hatRadius + 10), colors);
            colors.setARGB(255, 254, 166, 0); //fill
            myCanvas.drawCircle(newX, newY, hatRadius, colors);
            getHolder().unlockCanvasAndPost(myCanvas);
        }
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        //NonNull to remove null pointer ref
        setupDimensions();
        drawThumbstick(centerX, centerY);
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        //NonNull to remove null pointer ref
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        //NonNull to remove null pointer ref
    }

    /**
     * Called when thumbstick is moved
     */
    public interface ThumbstickListener {
        void onThumbstickMoved(float xPercent, float yPercent, int id);
    }

    /**
     * Method that calculates in which position the thumbstick currently is
     * @param v
     * @param e
     * @return true when thumbstick is used
     */
    public boolean onTouch(View v, MotionEvent e) {
        if ( v.equals(this) ) {
            if ( e.getAction() != e.ACTION_UP) {
                float displacement = (float) Math.sqrt((Math.pow(e.getX() - centerX, 2)) + Math.pow(e.getY() - centerY, 2));
                if ( displacement < baseRadius ) {
                    drawThumbstick(e.getX(), e.getY());
                    float xPercent = (float) (Math.round(((e.getX() - centerX)/baseRadius) * 100.0)/100.0);
                    float yPercent = (float) (Math.round(((e.getY() - centerY)/baseRadius)* 100.0)/100.0);
                    thumbstickCallback.onThumbstickMoved(xPercent, yPercent, getId());

                } else {
                    float ratio = baseRadius / displacement;
                    float constrainedX = centerX + (e.getX() - centerX) * ratio;
                    float constrainedY = centerY + (e.getY() - centerY) * ratio;
                    drawThumbstick(constrainedX, constrainedY);
                    float xPercent = (float) (Math.round(((constrainedX-centerX)/ baseRadius) * 100.0)/100.0);
                    float yPercent = (float) (Math.round(((constrainedY-centerY)/baseRadius)* 100.0)/100.0);
                    thumbstickCallback.onThumbstickMoved(xPercent ,yPercent , getId());
                }
            } else {
                drawThumbstick(centerX, centerY);
                thumbstickCallback.onThumbstickMoved(0, 0, getId());
            }
        }
        return true;
    }
}

