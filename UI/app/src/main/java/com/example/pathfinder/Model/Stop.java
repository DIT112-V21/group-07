package com.example.pathfinder.Model;

import com.google.android.gms.maps.model.LatLng;

public class Stop {
    private String mName;
    private LatLng mLatLong;

    public Stop(String name, LatLng latLong) {
        mName = name;
        mLatLong = latLong;
    }

    public String getName() {
        return mName;
    }

    public LatLng getLatLong() {
        return mLatLong;
    }
}
