package com.learn.app_test.utrl;


import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;
import com.learn.app_test.R;
import java.util.ArrayList;

public class HistoryAdapter extends RecyclerView.Adapter<HistoryAdapter.ViewHolder>{
    private ArrayList<String> history;

    public static  class  ViewHolder extends  RecyclerView.ViewHolder{
        public TextView mTextView;
        public ViewHolder(@NonNull View itemView) {
            super ( itemView );
            mTextView = itemView.findViewById( R.id.row_text);
        }
    }

    public HistoryAdapter(ArrayList<String> dataSet){
        history = dataSet;
    }

    @Override
    public HistoryAdapter.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        // Create View
        View view = LayoutInflater.from ( parent.getContext () ).inflate ( R.id.history_recycler_view,parent,false);
        return new ViewHolder ( view );
    }

    public void add(String data){
        history.add ( data );
        this.notifyDataSetChanged ();
    }
    @Override
    public void onBindViewHolder(@NonNull HistoryAdapter.ViewHolder holder, int position) {
    holder.mTextView.setText ( history.get ( position ) );
    }

    @Override
    public int getItemCount() {
        return history.size ();
    }
}
