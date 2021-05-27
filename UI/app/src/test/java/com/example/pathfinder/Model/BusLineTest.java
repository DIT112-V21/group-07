package com.example.pathfinder.Model;

import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class BusLineTest {

    BusLine busLine;


    @Before
    public void setUp(){
        busLine = new BusLine("9");
        busLine.addStop("Sandarna");
        busLine.addStop("Sannaplan");
    }


    @Test
    public void popsAllStopsUntilEmpty_returnsStopsAndEmptyMessage() {
        String nextStop = busLine.nextStop();
        assertEquals("Sannaplan", nextStop);
        String nextStop2 = busLine.nextStop();
        assertEquals(BusLine.TERMINUS + "Sandarna", nextStop2);
        String nextStop3 = busLine.nextStop();
        assertEquals(BusLine.TERMINUS, nextStop3);
    }

    @Test
    public void popsAllStopsUntilEmptyAndReverseLine_returnsLastPoppedStops(){
        busLine.nextStop();
        busLine.nextStop();
        busLine.nextStop();
        assertEquals("Sandarna", busLine.nextStop());
    }
}