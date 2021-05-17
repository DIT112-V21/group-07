package com.example.pathfinder.Activities;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.media.Image;
import android.os.Bundle;
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


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_passenger_dashboard);

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


    }

}