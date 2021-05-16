package com.example.pathfinder.Activities;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;

import androidx.appcompat.app.AppCompatActivity;
import com.example.pathfinder.R;

public class UserSelection extends AppCompatActivity {

    /**
     * initialise the activity (page) based on the given layout xml file saved into the int view variable.
     * @param savedInstanceState -> most recent initialisation of the activity. Null in this case. See Android documentation for more details.
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        super.onCreate(savedInstanceState);
        int view = R.layout.activity_user_selection;
        setContentView(view);
    }

    /**
     * Redirect to the driver login page when the button R.id.driverButton is clicked on.
     * @param view -> R.id.driverButton for this method.
     */
    public void selectTypeDriver(View view) {
        Log.d("driver","driver user type selected");
        Intent intent = new Intent(this, DriverLogin.class);
        startActivity(intent);
    }

    /**
     * Redirect to the passenger dedicated page when the button R.id.passengerButton is clicked on.
     * @param view -> R.id.passengerButton for this method.
     */
    public void selectTypePassenger(View view){
        Log.d("passenger","passenger user type selected");
        Intent intent = new Intent(this, PassengerDashboard.class);
        startActivity(intent);
    }
}
