package com.example.pathfinder.Activities;


import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.example.pathfinder.R;

import java.util.HashMap;
import java.util.Map;

public class DriverLogin extends AppCompatActivity {



    private Map<String, String> driversProfiles;
    Toast connectionFailedNotification;


    /* Getters and setters */
    public Map<String, String> getDriversProfiles() {return driversProfiles;}
    public void setDriversProfiles(Map<String, String> driversProfiles) {this.driversProfiles = driversProfiles;}


    /**
     * initialise the activity (page) with layout and a hardcoded map of users. This could be replaced later on with an import of a DB.
     * @param savedInstanceState -> most recent initialisation of the activity. Null in this case. See Android documentation for more details.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        int view = R.layout.driver_login;
        setContentView(view);

        driversProfiles = new HashMap<>();
        driversProfiles.put("Alex", "1234");
        driversProfiles.put("Chris", "1234");
        driversProfiles.put("Gimmy", "1234");
        driversProfiles.put("Bassam", "1234");
        driversProfiles.put("Julia", "1234");
        driversProfiles.put("Ediz", "1234");
        Log.d("driver", "Drivers' profiles successfully loaded");

        connectionFailedNotification = Toast.makeText(this, R.string.connection_failed, Toast.LENGTH_LONG);
    }

    /**
     * - Takes both inputs from the username and password input.
     * - Compares the username with the one registered in the drivers' profiles
     * - If a match, compares the password with the one registered for that specific user.
     * - Connects if it matches, notifies if not.
     * @param view -> -> R.id.connectButton for this method.
     */
    public void connect(View view) {
        String usernameInput = ((EditText) findViewById(R.id.usernameInput)).getText().toString();
        String passwordInput = ((EditText) findViewById(R.id.passwordInput)).getText().toString();
        if (isRegisteredUsername(usernameInput)) {
            if (isMatchingPassword(usernameInput,passwordInput)) {
                goToDashboard();
                Log.d("driver", "Connection to selected user");
            }
        }else {
            connectionFailed();
            Log.d("driver", "Connection failed, user name or password are invalid");
        }
    }
    
    public boolean isRegisteredUsername(String username){
        return driversProfiles.containsKey(username);
    }
    
    public boolean isMatchingPassword(String username, String password){
        return password.equals(driversProfiles.get(username));
    }
    
    
    /**
     * Helper method for connect(View view) to redirect to the dashboard activity (page).
     */
    private void goToDashboard() {
        Intent intent = new Intent(this, DashboardActivity.class);
        startActivity(intent);
        finish();
    }

    /**
     * Helper method for connect(View view) to notify the connection failed.
     */
    private void connectionFailed(){
        connectionFailedNotification.show();
    }

}

