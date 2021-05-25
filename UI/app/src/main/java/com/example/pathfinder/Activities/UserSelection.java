package com.example.pathfinder.Activities;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import androidx.appcompat.app.AppCompatActivity;
import com.example.pathfinder.R;

public class UserSelection extends AppCompatActivity {

    RelativeLayout mDriverBtn, mPassengerBtn;

    /**
     * initialise the activity (page) based on the given layout xml file saved into the int view variable.
     * @param savedInstanceState -> most recent initialisation of the activity. Null in this case. See Android documentation for more details.
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_user_selection);

        mDriverBtn = findViewById(R.id.user_driver);
        mPassengerBtn = findViewById(R.id.user_passenger);

        /**
         * Redirect to the driver login page when the button R.id.driverButton is clicked on.
         * @param view -> R.id.driverButton for this method.
         */
        mDriverBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(getApplicationContext(), DriverLogin.class));
            }
        });

        /**
         * Redirect to the passenger dedicated page when the button R.id.passengerButton is clicked on.
         * @param view -> R.id.passengerButton for this method.
         */
        mPassengerBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(getApplicationContext(), PassengerDashboard.class));
            }
        });
    }

}
