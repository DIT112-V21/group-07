package com.example.pathfinder.Model;

import com.google.android.gms.maps.model.LatLng;

import java.util.ArrayList;
import java.util.List;

public class BusRoute {
    private String mRoute;
    private List<Stop> mStops;

    public BusRoute(String route, List<Stop> stops) {
        mRoute = route;
        mStops = stops;
    }

    public String getRoute() {
        return mRoute;
    }

    public List<LatLng> getStopLatLong() {
        List<LatLng> latLongList = new ArrayList<>();
        for (Stop stop : mStops) {
            latLongList.add(stop.getLatLong());
        }
        return latLongList;
    }

    public Stop getFirstStop() {
        return mStops.get(0);
    }
}
