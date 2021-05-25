package com.example.pathfinder.Activities;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.media.Image;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.example.pathfinder.Client.MqttClient;
import com.example.pathfinder.R;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapFragment;
import com.google.android.gms.maps.OnMapReadyCallback;

import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class PassengerDashboard extends AppCompatActivity {

    ImageView mBackBtn, mAccessibility;
    ToggleButton mStopBtn, mHandicapBtn;
    TextView mNextStop;
    Button mFindRouteBtn;
    RelativeLayout mStopStatus;
    SharedPreferences sharedPreferences;
    private MqttClient mMqttClient;
    private boolean isMqttConnected;

    private static final String SHARED_PREF_NAME = "myPref";
    private static final String KEY_STOP = "stop";
    private static final String KEY_HANDICAP = "handicap";

    private static final String EXTERNAL_MQTT_BROKER = "test.mosquitto.org";
    private static final String LOCALHOST = "10.0.2.2";
    private static final String MQTT_SERVER = "tcp://" + LOCALHOST + ":1883";
    private static final String NEXT_STOP = "/smartcar/busNextStop";
    private static final int QOS = 1;
    private static final String TAG = "PathfinderPassenger";



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_passenger_dashboard);

        isMqttConnected = false;
        mMqttClient = new MqttClient(getApplicationContext(), MQTT_SERVER, TAG);

        mNextStop = findViewById(R.id.nextStopPassenger);
        mBackBtn = findViewById(R.id.back);
        mStopBtn = (ToggleButton) findViewById(R.id.stop);
        mHandicapBtn = (ToggleButton) findViewById(R.id.handicap);
        mStopStatus = findViewById(R.id.stop_status);
        mAccessibility = (ImageView) findViewById(R.id.accessibility);

        sharedPreferences = getSharedPreferences(SHARED_PREF_NAME, MODE_PRIVATE);

        //checks if shared pref data is available
        String stopStatus = sharedPreferences.getString(KEY_STOP, null);

        if (stopStatus == mStopBtn.getTextOff().toString()) {
            Intent intent = new Intent(PassengerDashboard.this, DriverDashboard.class);
        }

        mStopBtn.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                //toggle enabled
                if (isChecked) {
                    //puts data on shared pref
                    SharedPreferences.Editor editor = sharedPreferences.edit();
                    editor.putString(KEY_HANDICAP, mStopBtn.getTextOn().toString());
                    editor.putString(KEY_HANDICAP, mHandicapBtn.getTextOn().toString());
                    editor.apply();

                    //pass above data to DriverDashboard
                    Intent intent = new Intent(PassengerDashboard.this, DriverDashboard.class);

                    //stop requested
                    mStopStatus.setBackgroundColor(Color.parseColor("#B33701"));
                } else {
                    //toggle disabled
                    mStopStatus.setBackgroundColor(Color.WHITE);
                    mAccessibility.setColorFilter(Color.WHITE);
                }

            }
        });

        mHandicapBtn.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    //puts data on shared pref
                    SharedPreferences.Editor editor = sharedPreferences.edit();
                    editor.putString(KEY_HANDICAP, mStopBtn.getTextOn().toString());
                    editor.putString(KEY_HANDICAP, mHandicapBtn.getTextOn().toString());
                    editor.apply();

                    //pass above data to DriverDashboard
                    Intent intent = new Intent(PassengerDashboard.this, DriverDashboard.class);

                    //stop requested
                    mStopStatus.setBackgroundColor(Color.parseColor("#B33701"));
                    mAccessibility.setColorFilter(Color.parseColor("#008080"));
                } else {
                    //toggle disabled
                    mStopStatus.setBackgroundColor(Color.WHITE);
                    mAccessibility.setColorFilter(Color.WHITE);
                }

            }
        });

        //redirects user back to UserSelection activity
        mBackBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(getApplicationContext(), UserSelection.class));
            }
        });

        connectToMqttBroker();

    }


    private void connectToMqttBroker() {
        if (!isMqttConnected) {
            mMqttClient.connect(TAG, "", new IMqttActionListener() {

                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    isMqttConnected = true;

                    final String successfulConnection = "Connected to MQTT broker";
                    Log.i(TAG, successfulConnection);
                    Toast.makeText(getApplicationContext(), successfulConnection, Toast.LENGTH_SHORT).show();

                    // These are to subscribe to that related specific topics mentioned as first parameter. Topics shall match the topics smart car publishes its data on.
                    mMqttClient.subscribe(NEXT_STOP, QOS, null);

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
                    isMqttConnected = false;

                    final String connectionLost = "Lost connection to MQTT broker";
                    Log.w(TAG, connectionLost);
                    Toast.makeText(getApplicationContext(), connectionLost, Toast.LENGTH_SHORT).show();
                }

                //The topics shall be catch hold of by this method and handled through the statements for the specific functions.
                // (If a message published to a specific topic, use that message to the some specific function).
                @Override
                public void messageArrived(String topic, MqttMessage message) {
                    if (topic.equals(NEXT_STOP)) {
                        updateNextStop(message);
                        //Here a comparison could be made to check is the next stop is the chosen one. If so, we an notify the user.
                    } //alternative for list o stops (could be updated as it goes maybe ?

                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                    Log.d(TAG, "Message delivered");
                }
            });
        }
    }


    /**
     * Method that checks if the mqtt broker is connected to the app
     * create a toast if not notify the problem
     * should be used before sending messages
     */
    private void notConnected() {
        if (!isMqttConnected) {
            final String notConnected = "No connection";
            Log.e(TAG, notConnected);
            Toast.makeText(getApplicationContext(), notConnected, Toast.LENGTH_SHORT).show();
            return;
        }
    }

    /**
     * Update the interface with the next stop when receiving a new next stop from the driver's side.
     * @param message -> mqtt message send on the relevant topic NEXT_STOP.
     */
    private void updateNextStop(MqttMessage message){
        String nextStop = "Next stop: " + message.toString();
        mNextStop.setText(nextStop);
    }

}