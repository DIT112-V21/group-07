package com.example.pathfinder.Activities;


import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import com.example.pathfinder.R;
import com.google.android.material.textfield.TextInputEditText;

import java.util.HashMap;
import java.util.Map;

public class DriverLogin extends AppCompatActivity {
    Button mLoginBtn, mPassengerBtn;
    ImageView mBackBtn;
    TextInputEditText mUsername, mPassword;
    private Map<String, String> driversProfiles;


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
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_driver_login);

        mLoginBtn = findViewById(R.id.sign_in);
        mPassengerBtn = findViewById(R.id.cont_passenger);
        mBackBtn = findViewById(R.id.back);
        mUsername = (TextInputEditText)findViewById(R.id.username);
        mPassword = (TextInputEditText)findViewById(R.id.password);

        driversProfiles = new HashMap<>();
        driversProfiles.put("Alex", "1234");
        driversProfiles.put("Chris", "1234");
        driversProfiles.put("Gimmy", "1234");
        driversProfiles.put("Bassam", "1234");
        driversProfiles.put("Julia", "1234");
        driversProfiles.put("Ediz", "1234");
        Log.d("driver", "Drivers' profiles successfully loaded");

        mLoginBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /**
                 * - Takes both inputs from the username and password input.
                 * - Compares the username with the one registered in the drivers' profiles
                 * - If a match, compares the password with the one registered for that specific user.
                 * - Connects if it matches, notifies if not.
                 * @param view -> -> R.id.connectButton for this method.
                 */

                String usernameInput = mUsername.getText().toString();
                String passwordInput = mPassword.getText().toString();

                if (isRegisteredUsername(usernameInput)) {
                    if (isMatchingPassword(usernameInput,passwordInput)) {
                        startActivity(new Intent(getApplicationContext(), DriverDashboard.class));
                        Toast.makeText(DriverLogin.this, "Welcome!", Toast.LENGTH_SHORT).show();
                        Log.d("driver", "Welcome!");
                    }
                } else {
                    signInFailed();
                    Log.d("driver", "Error: invalid username or password.");
                }
            }
        });

        mPassengerBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(getApplicationContext(), PassengerDashboard.class));
            }
        });

        mBackBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(getApplicationContext(), UserSelection.class));
            }
        });
    }

    public boolean isRegisteredUsername(String username){
        return driversProfiles.containsKey(username);
    }
    
    public boolean isMatchingPassword(String username, String password){
        return password.equals(driversProfiles.get(username));
    }


    /**
     * Helper method for to notify the connection failed.
     */
    private void signInFailed(){
        Toast.makeText(this, "Invalid username or password", Toast.LENGTH_LONG).show();
    }
}

