package com.example.pathfinder.Model;

import java.util.ArrayList;
import java.util.Stack;

public class BusLine {

    // Attributes
    private String name;
    private Stack <String> comingStops;
    private Stack <String> pastStops;
    private ArrayList <String> stopList;

    // Constructor
    public BusLine(String name) {
        this.name = name;
        this.comingStops = new Stack<>();
        this.pastStops = new Stack<>();
        this.stopList = new ArrayList<>();
    }

    // Getters and setters
    public Stack<String> getComingStops() {
        return comingStops;
    }

    public void setComingStops(Stack<String> comingStops) {
        this.comingStops = comingStops;
    }

    public Stack<String> getPastStops() {
        return pastStops;
    }

    public void setPastStops(Stack<String> pastStops) {
        this.pastStops = pastStops;
    }

    public ArrayList<String> getStopList() {
        return stopList;
    }

    public void setStopList(ArrayList<String> stopList) {
        this.stopList = stopList;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Stack<String> getStops() {
        return comingStops;
    }

    public void setStops(Stack<String> stops) {
        this.comingStops = stops;
    }

    // Methods
    /**
     * Pop the next stop and saves it into the pastStops stack to later reverse the line.
     * @return a string with the next stop's name.
     */
    public String nextStop(){
        String nextStop = comingStops.pop();
        pastStops.push(nextStop);
        return nextStop;
    }

    /**
     * Reverse the line at the end of the trip. The destination becomes the starting point and the starting point the destination.
     */
    public void reverseLine(){
        Stack reversedComingStops = this.pastStops;
        Stack reversedPastStops = this.comingStops;
        this.comingStops = reversedComingStops;
        this.pastStops = reversedPastStops;
    }

    /**
     * Receives a string stop name and add it to the different list and stacks.
     * Needs to add the destination first, then the closest stop from the destination to the starting point.
     * @param stop
     */
    public void addStop(String stop){
        stopList.add(stop);
        comingStops.push(stop);
    }

}