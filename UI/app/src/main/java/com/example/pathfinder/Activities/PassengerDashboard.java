package com.example.pathfinder.Activities;

import androidx.appcompat.app.AppCompatActivity;

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

import com.example.pathfinder.R;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapFragment;
import com.google.android.gms.maps.OnMapReadyCallback;

public class PassengerDashboard extends AppCompatActivity {

    ImageView mBackBtn, mAccessibility;
    ToggleButton mStopBtn, mHandicapBtn;
    Button mFindRouteBtn;
    RelativeLayout mStopStatus;
    SharedPreferences sharedPreferences;

    private static final String SHARED_PREF_NAME = "myPref";
    private static final String KEY_STOP = "stop";
    private static final String KEY_HANDICAP = "handicap";

    /**
     * Method to create passenger UI
     * @param savedInstanceState
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_passenger_dashboard);

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