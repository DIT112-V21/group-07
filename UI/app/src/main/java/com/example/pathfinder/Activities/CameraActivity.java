package com.example.pathfinder.Activities;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.ImageView;

import com.example.pathfinder.Client.MqttClient;
import com.example.pathfinder.R;

public class CameraActivity extends AppCompatActivity {

    DashboardActivity dashboardActivity = new DashboardActivity();
    MqttClient mMqttClient;
    ImageView cameraView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);
        //mMqttClient = new MqttClient(getApplicationContext(), DashboardActivity.MQTT_SERVER, DashboardActivity.TAG);
        //cameraView = findViewById(R.id.cameraView);
        //dashboardActivity.connectToMqttBroker();
    }
}