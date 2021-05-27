package com.example.pathfinder.Activities;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.example.pathfinder.R;

import java.util.ArrayList;

public class StopListAdapter extends RecyclerView.Adapter<StopListAdapter.ViewHolder> {

    private ArrayList<String> stops;
    public StopListAdapter(ArrayList<String> stops) {
        this.stops = stops;
    }

    @NonNull
    @org.jetbrains.annotations.NotNull
    @Override
    public StopListAdapter.ViewHolder onCreateViewHolder(@NonNull @org.jetbrains.annotations.NotNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.stop_row, parent, false);
        return new ViewHolder(view) ;
    }

    @Override
    public void onBindViewHolder(@NonNull @org.jetbrains.annotations.NotNull StopListAdapter.ViewHolder holder, int position) {
        holder.stopName.setText(stops.get(position));
    }

    @Override
    public int getItemCount() {
        return stops.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder {
        public TextView stopName;
        public ViewHolder(@NonNull @org.jetbrains.annotations.NotNull View itemView) {
            super(itemView);

            stopName = itemView.findViewById(R.id.stopName);
        }
    }
}
