package com.example.pathfinder.Activities;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.media.Image;
import android.os.Bundle;
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
import com.example.pathfinder.Model.BusLine;
import com.example.pathfinder.R;

import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.util.ArrayList;

public class PassengerDashboard extends AppCompatActivity {

    ImageView mBackBtn, mAccessibility;
    ToggleButton mStopBtn, mHandicapBtn;
    TextView mNextStop, mBusLineName;
    Button mFindRouteBtn;
    RelativeLayout mStopStatus;
    SharedPreferences sharedPreferences;

    private MqttClient mMqttClient;
    private boolean isMqttConnected;

    private RecyclerView mStopList;
    private RecyclerView.LayoutManager mStopListLayoutManager;
    private RecyclerView.Adapter mStopListAdapter;
    private ArrayList <String> stopList;


    private static final String SHARED_PREF_NAME = "myPref";
    private static final String KEY_STOP = "stop";
    private static final String KEY_HANDICAP = "handicap";
    private static final String END_OF_LINE = "Terminus";

    private static final String EXTERNAL_MQTT_BROKER = "test.mosquitto.org";
    private static final String LOCALHOST = "10.0.2.2";
    private static final String MQTT_SERVER = "tcp://" + LOCALHOST + ":1883";
    private static final String NEXT_STOP = "/smartcar/busNextStop";
    private static final String NEW_PASSENGER = "/smartcar/newPassengerConnected";
    private static final String BUS_STOP_LIST_TOPIC = "/smartcar/bus/StopList";
    private static final String BUS_NAME_TOPIC = "/smartcar/bus/Name";
    private static final String NEW_PASSENGER_BUS_ROUTE_TRIGGER = "1";
    private static final String NEW_PASSENGER_BUS_NAME_TRIGGER = "2";
    private static final int QOS = 1;
    private static final String TAG = "PathfinderPassenger";





    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_passenger_dashboard);

        isMqttConnected = false;
        mMqttClient = new MqttClient(getApplicationContext(), MQTT_SERVER, TAG);
        stopList = new ArrayList<>();

        mBusLineName = findViewById(R.id.stopTitle_passenger);
        mNextStop = findViewById(R.id.nextStopPassenger);
        mNextStop.setText("Loading next stop");
        mBackBtn = findViewById(R.id.back);
        mStopBtn = (ToggleButton) findViewById(R.id.stop);
        mHandicapBtn = (ToggleButton) findViewById(R.id.handicap);
        mStopStatus = findViewById(R.id.stop_status);
        mAccessibility = (ImageView) findViewById(R.id.accessibility);

        /*
         * first checks whether stop and/or accessibility request were made and stored into
         * shared preferences
         */

        mStopBtn.setChecked(update(KEY_STOP));
        mHandicapBtn.setChecked(update(KEY_HANDICAP));



        mStopBtn.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                //if the button is toggled to true
                if (isChecked) {
                    //boolean data is stored to shared preference
                    saveIntoSharedPrefs(KEY_STOP, true);
                    passengerSharedPrefs(KEY_STOP, true);
                    /*
                     * text is given a red background color to make it visible when someone
                     * toggles on the stop button
                     */
                    mStopStatus.setBackgroundColor(Color.parseColor("#B33701"));
                    Log.d(KEY_STOP, "this is on");
                } else {
                    //boolean data is stored to shared preference
                    saveIntoSharedPrefs(KEY_STOP, false);
                    passengerSharedPrefs(KEY_STOP, false);
                    /*
                     * text is given a white background color to make it visible when someone
                     * toggles on the stop button
                     */
                    mStopStatus.setBackgroundColor(Color.WHITE);
                    Log.d(KEY_STOP, "this is off");
                }
            }
        });

        mHandicapBtn.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                //if the button is toggled to true
                if (isChecked) {
                    //boolean data is stored to shared preference
                    saveIntoSharedPrefs(KEY_STOP, true);
                    saveIntoSharedPrefs(KEY_HANDICAP, true);
                    passengerSharedPrefs(KEY_HANDICAP, true);

                    /*
                     * images are given a red and teal background colors respectively to make them
                     * visible when someone toggles on the handicap accessibility button
                     */
                    mStopStatus.setBackgroundColor(Color.parseColor("#B33701"));
                    mAccessibility.setColorFilter(Color.parseColor("#008080"));
                    Log.d(KEY_HANDICAP, "this is on");
                } else {
                    //boolean data is stored to shared preference
                    saveIntoSharedPrefs(KEY_STOP, false);
                    saveIntoSharedPrefs(KEY_HANDICAP, false);
                    passengerSharedPrefs(KEY_HANDICAP, false);
                    /*
                     * images are given a white background color to make them
                     * visible when someone toggles off the handicap accessibility button
                     */
                    mStopStatus.setBackgroundColor(Color.WHITE);
                    mAccessibility.setColorFilter(Color.WHITE);
                    Log.d(KEY_HANDICAP, "this is off");
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
                    mMqttClient.subscribe(BUS_STOP_LIST_TOPIC,QOS,null);
                    mMqttClient.subscribe(BUS_NAME_TOPIC,QOS,null);
                    notifyDriverAboutNewPassenger();
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
                    } else if(topic.equals(BUS_STOP_LIST_TOPIC)){
                        stopList = parseMessageToArray(message);
                        generateBusLine();
                    }else if(topic.equals(BUS_NAME_TOPIC)){
                        generateBusName(message);
                    }

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
        if (message.toString().equals(BusLine.TERMINUS)){
            mNextStop.setText(END_OF_LINE);
        }else {
            String nextStop = "Next stop: " + message.toString();
            mNextStop.setText(nextStop);
        }
    }


    /**
     * Notify the driver that the passenger is connected to the bus line. Triggers the sending of a message containing all buss stops.
     * Message need to be parsed.
     * Please refer to parseMessageToArray() method.
     */
    private void notifyDriverAboutNewPassenger(){
        mMqttClient.publish(NEW_PASSENGER, NEW_PASSENGER_BUS_ROUTE_TRIGGER, QOS, null);
        Log.d("Passenger to Driver", "Request bus route" );
        mMqttClient.publish(NEW_PASSENGER, NEW_PASSENGER_BUS_NAME_TRIGGER, QOS, null);
        Log.d("Passenger to Driver", "Request bus name" );
    }

    /**
     * Parse the MQTT message into an array of strings. Characters need to be separated by a ";" in order to be separated during parsing.
     * @param message -> The MQTT message to be parsed
     * @return an arrayList of strings.
     */
    private ArrayList<String> parseMessageToArray(MqttMessage message) {
        ArrayList messageAsArray = new ArrayList();
        String message_ = message.toString();
        Log.d("Bus Stop", message_);
        char[] charList = message_.toCharArray();
        String stop = "";
        for (char c : charList){
            if(c != ';'){
                stop = stop + c;
            }else{
                Log.d("Bus Stop", stop);
                messageAsArray.add(stop);
                stop = "";
            }
        }
        return messageAsArray;
    }


    /**
     * generates the view with the different stops received from the driver.
     */
    private void generateBusLine(){

        mStopList = findViewById(R.id.stopList_passenger);
        mStopList.setHasFixedSize(true);
        mStopListLayoutManager = new LinearLayoutManager(this);
        mStopListAdapter = new StopListAdapter(stopList);
        mStopList.setLayoutManager(mStopListLayoutManager);
        mStopList.setAdapter(mStopListAdapter);

    }

    private void generateBusName(MqttMessage message){
        String busName = message.toString();
        mBusLineName.setText("Bus line: " + busName);
        Log.d("Bus Name", "Generated bus name: " + busName);
    }



    /*
     * helper method to check if stop request evaluates as true.
     * if stop request has is true then the stop status lights up and is made visible; otherwise
     * its color is set to white to appear invisible.
     */
    public void getStopRequest() {
        sharedPreferences = getSharedPreferences(KEY_STOP, Context.MODE_PRIVATE);

        if (sharedPreferences.getBoolean(KEY_STOP, false)) {
            mStopStatus.setBackgroundColor(Color.parseColor("#B33701"));
            Log.d(KEY_STOP, "red");
        } else {
            mStopStatus.setBackgroundColor(Color.WHITE);
            Log.d(KEY_STOP, "white");
        }
    }

    /*
    * helper method to check if accessibility request evaluates as true.
    * if accessibility request has is true then the stop status and accessibility symbols
    * light up and is made visible; otherwise their colors are set to white to appear invisible.
    */
    public void getAccessibilityRequest() {
        sharedPreferences = getSharedPreferences(KEY_HANDICAP, Context.MODE_PRIVATE);

        if (sharedPreferences.getBoolean(KEY_HANDICAP, false)) {
            mAccessibility.setColorFilter(Color.parseColor("#008080"));
            Log.d(KEY_HANDICAP, "red");
        } else {
            mAccessibility.setColorFilter(Color.WHITE);
            Log.d(KEY_HANDICAP, "white");
        }
    }

    /*
    * Helper method to save state of buttons into passenger shared preference
    *
    * */
    public void passengerSharedPrefs(String key, Boolean value) {
        sharedPreferences = getSharedPreferences(SHARED_PREF_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putBoolean(key, value);
        editor.commit();
    }

    /*
    * Helper method to save state of buttons into a shared preference
    */
    public void saveIntoSharedPrefs(String key, Boolean value) {
        sharedPreferences = getApplicationContext().getSharedPreferences(key, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putBoolean(key, value);
        editor.apply();
    }

    /*
    * Helper update to last state of buttons into a shared preference
    */
    public boolean update(String key) {
        sharedPreferences = getSharedPreferences(SHARED_PREF_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(key, false);
    }

    @Override
    protected void onResume() {
        super.onResume();

        //checks the state of stop request
        getStopRequest();
        //checks the state of accessibility request
        getAccessibilityRequest();
    }

}