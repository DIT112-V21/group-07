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


    /**
     * initialise the activity (page) with layout and a hardcoded map of users. This should be replaced later on with an import of a DB.
     * Todo: implement a database to replace the hardcoded users.
     * @param savedInstanceState
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
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
    }

    /**
     * - Takes both inputs from the username and password input.
     * - Compares the username with the one registered in the drivers' profiles
     * - If a match, compares the password with the one registered for that specific user.
     * - Connects if it matches, notifies if not.
     * @param view -> -> R.id.connectButton for this method.
     */
    public void connect(View view) {
        String usernameInput = ((EditText) findViewById(R.id.username)).getText().toString();
        String passwordInput = ((EditText) findViewById(R.id.passwordInput)).getText().toString();
        if (driversProfiles.containsKey(usernameInput)) {
            if (passwordInput.equals(driversProfiles.get(usernameInput))) {
                goToDashboard();
                Log.d("driver", "Connection to selected user");
            }
        }else {
            connectionFailed();
            Log.d("driver", "Connection failed, user name or password are invalid");
        }
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
        Toast.makeText(this, "Invalid username or password", Toast.LENGTH_LONG).show();
    }
}

