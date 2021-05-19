package com.example.pathfinder.Activities;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.SeekBar;
import android.widget.ToggleButton;

import com.example.pathfinder.Client.MqttClient;
import com.example.pathfinder.R;

import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class DriverDashboard extends AppCompatActivity implements ThumbstickView.ThumbstickListener {
    private static final String TAG = "PathfinderController";
    private static final String EXTERNAL_MQTT_BROKER = "test.mosquitto.org";
    private static final String LOCALHOST = "10.0.2.2";
    private static final String MQTT_SERVER = "tcp://" + LOCALHOST + ":1883";
    private static final String THROTTLE_CONTROL = "/smartcar/control/speed";
    private static final String STEERING_CONTROL = "/smartcar/control/angle";
    private static final String PARK = "/smartcar/control/park";
    private static final String ODOMETER_LOG = "/smartcar/odometer";
    private static final String SPEEDOMETER_LOG = "/smartcar/speedometer";
    private static final int IDLE_SPEED = 0;
    private static final int STRAIGHT_ANGLE = 0;
    private static final int QOS = 1;
    private static final int IMAGE_WIDTH = 320;
    private static final int IMAGE_HEIGHT = 240;

    private MqttClient mMqttClient;
    //Park/ lock
    //private boolean isParked = false;
    //Engine activity
    //private boolean isActive = false;
    private boolean isConnected = false;
    private ImageView mVideoStream, mSignOutBtn;
    private TextView mSpeedLog, mDistanceLog;
    private TextView textView;
    private SeekBar seekBar;
    private ToggleButton mCruiseControlBtn, mParkBtn;

    SharedPreferences sharedPreferences;

    /**Used as a way to compare previously published messages with GUI's current values*/
    private int lastSentSpeed = 0;
    private int lastSentAngle = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_driver_dashboard);

        mSpeedLog = findViewById(R.id.speed_log) ;
        mDistanceLog = findViewById(R.id.distance_log);
        mSignOutBtn = findViewById(R.id.sign_out);

        mMqttClient = new MqttClient(getApplicationContext(), MQTT_SERVER, TAG);

        mVideoStream = findViewById(R.id.videoStream);

        textView = (TextView) findViewById(R.id.textView);
        seekBar = (SeekBar) findViewById(R.id.seekBar);

        //TODO: check method call location for better code quality
        seekBarListener();
        connectToMqttBroker();

        //sign out button that redirects user back to DriverLogin activity
        mSignOutBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(getApplicationContext(), DriverLogin.class));
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();

        connectToMqttBroker();
    }

    @Override
    protected void onPause() {
        super.onPause();

        mMqttClient.disconnect(new IMqttActionListener() {
            @Override
            public void onSuccess(IMqttToken asyncActionToken) {
                Log.i(TAG, "Disconnected from broker");
            }

            @Override
            public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
            }
        });
    }

    @Override
    public void onThumbstickMoved(float xPercent, float yPercent, int id) {
        int angle = (int)((xPercent) * 100);
        int strength;
        //TODO: check why do we need the negative of seekBar.getProgress()
        int seekProgress = - seekBar.getProgress();
        if(mCruiseControlBtn.isEnabled()){
            //setting fixed speed for cruise control
            strength = seekProgress;
        }else{
            //range calculation (limit speed is active)
            strength = (int)(yPercent * seekProgress);
        }
        drive(strength, angle, "driving");
    }

    public void onCruiseControlBtn(View view) {
        int strength = seekBar.getProgress();
        if (strength > IDLE_SPEED && mCruiseControlBtn.isEnabled()) {
            drive(strength, STRAIGHT_ANGLE, "driving");
        } else {
            drive(IDLE_SPEED, STRAIGHT_ANGLE, "stopping");
        }
    }

    //TODO: check location of this method for better code quality
    private void seekBarListener(){
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                textView.setText("" + progress + "%");
                if (mCruiseControlBtn.isEnabled()) {
                    drive(progress, STRAIGHT_ANGLE, "driving");
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });
    }

    private void connectToMqttBroker() {
        if (!isConnected) {
            mMqttClient.connect(TAG, "", new IMqttActionListener() {

                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    isConnected = true;

                    final String successfulConnection = "Connected to MQTT broker";
                    Log.i(TAG, successfulConnection);
                    Toast.makeText(getApplicationContext(), successfulConnection, Toast.LENGTH_SHORT).show();

                    // These are to subscribe to that related specific topics mentioned as first parameter. Topics shall match the topics smart car publishes its data on.
                    mMqttClient.subscribe("/smartcar/park", QOS, null);
                    mMqttClient.subscribe("/smartcar/camera", QOS, null);
                    mMqttClient.subscribe("/smartcar/odometer", QOS, null);
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    final String failedConnection = "Failed to connect to MQTT broker";
                    Log.e(TAG, failedConnection);
                    Toast.makeText(getApplicationContext(), failedConnection, Toast.LENGTH_SHORT).show();
                }
            }, new MqttCallback() {
                @Override
                public void connectionLost(Throwable cause) {
                    isConnected = false;

                    final String connectionLost = "Lost connection to MQTT broker";
                    Log.w(TAG, connectionLost);
                    Toast.makeText(getApplicationContext(), connectionLost, Toast.LENGTH_SHORT).show();
                }

                //The topics shall be catch hold of by this method and handled through the statements for the specific functions.
                // (If a message published to a specific topic, use that message to the some specific function).
                @Override
                public void messageArrived(String topic, MqttMessage message) throws Exception {
                    if (topic.equals("/smartcar/camera")) {
                        final Bitmap bm = Bitmap.createBitmap(IMAGE_WIDTH, IMAGE_HEIGHT, Bitmap.Config.ARGB_8888);

                        final byte[] payload = message.getPayload();
                        final int[] colors = new int[IMAGE_WIDTH * IMAGE_HEIGHT];
                        for (int ci = 0; ci < colors.length; ++ci) {
                            final byte r = payload[3 * ci];
                            final byte g = payload[3 * ci + 1];
                            final byte b = payload[3 * ci + 2];
                            colors[ci] = Color.rgb(r, g, b);
                        }
                        bm.setPixels(colors, 0, IMAGE_WIDTH, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);

                        mVideoStream.setImageBitmap(bm);
                    } else if(topic.equals("/smartcar/odometer")) {
                        distanceLog(Double.parseDouble(message.toString()));
                    } else if(topic.equals("/smartcar/speedometer")) {
                        speedLog(Integer.parseInt(message.toString()));
                    }
                    else {
                        Log.i(TAG, "[MQTT] Topic: " + topic + " | Message: " + message.toString());
                    }
                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                    Log.d(TAG, "Message delivered");
                }
            });
        }
    }

    void notConnected() {
        if (!isConnected) {
            final String notConnected = "No connection";
            Log.e(TAG, notConnected);
            Toast.makeText(getApplicationContext(), notConnected, Toast.LENGTH_SHORT).show();
            return;
        }
    }

    void drive(int throttleSpeed, int steeringAngle, String actionDescription) {
        notConnected();
        Log.i(TAG, actionDescription);
        if(throttleSpeed > lastSentSpeed + 5 || throttleSpeed < lastSentSpeed - 5 || throttleSpeed == 0){
            lastSentSpeed = throttleSpeed;
            mMqttClient.publish(THROTTLE_CONTROL, Integer.toString(throttleSpeed), QOS, null);
        }
        if(steeringAngle > 10 && lastSentAngle <= 0){
            lastSentAngle = 30;
            mMqttClient.publish(STEERING_CONTROL, Integer.toString(lastSentAngle), QOS, null);
        }else if(steeringAngle < -10 && lastSentAngle >= 0){
            lastSentAngle = -30;
            mMqttClient.publish(STEERING_CONTROL, Integer.toString(lastSentAngle), QOS, null);
        }else if (steeringAngle <= 10 && steeringAngle >= -10 && lastSentAngle != 0){
            lastSentAngle = 0;
            mMqttClient.publish(STEERING_CONTROL, Integer.toString(lastSentAngle), QOS, null);
        }
        speedLog(Math.abs(throttleSpeed));
    }

    void brake() {
        drive(0,0, "Stopped");
    }

    void speedLog(int speed) {
        notConnected();
        mMqttClient.subscribe(THROTTLE_CONTROL, QOS, null);
        mSpeedLog.setText(String.valueOf(speed) + " km/h");
    }

    // A helper method takes the distance value from ODOMETER_LOG(topic="/smartcar/odometer") and set it to distance log on the related layout in the UI.
    void distanceLog(double distance) {
        distance = distance/100;
        notConnected();
        mMqttClient.subscribe(ODOMETER_LOG, QOS, null);
        mDistanceLog.setText(String.valueOf(distance) + " m");
    }

    public void brakeBtn(View view) {
        brake();
    }


    public void nextStopBtn(View view) {

    }
}