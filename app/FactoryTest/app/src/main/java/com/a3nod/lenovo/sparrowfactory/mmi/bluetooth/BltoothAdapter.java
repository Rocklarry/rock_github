package com.a3nod.lenovo.sparrowfactory.mmi.bluetooth;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.mmi.bluetooth.BTDevice;

import java.util.List;

/**
 * Created by Lenovo on 2017/11/10.
 */

public class BltoothAdapter extends BaseAdapter {
    private List<BTDevice> mList;
    private Context mContext;
    public BltoothAdapter(List<BTDevice> list, Context context){
        this.mList=list;
        this.mContext=context;
    }

    @Override
    public int getCount() {
        return mList.size();
    }

    @Override
    public Object getItem(int i) {
        return mList.get(i);
    }

    @Override
    public long getItemId(int i) {
        return i;
    }

    @Override
    public View getView(int i, View view, ViewGroup viewGroup) {
        ViewHolder viewHolder=null;
        if(view==null){
            view= LayoutInflater.from(mContext).inflate(R.layout.btlist_item,viewGroup,false);
            viewHolder=new ViewHolder(view);
            view.setTag(viewHolder);
        }else {
            viewHolder=(ViewHolder) view.getTag();
        }
        BTDevice device=mList.get(i);
        viewHolder.name.setText(device.name);
        viewHolder.address.setText(device.address);
        viewHolder.rssi.setText(+device.rssi + "\t"+  (device.isBound? "paired":""));
        return view;
    }

    static class ViewHolder{
        TextView address,rssi,name;

        public ViewHolder(View view){
            name=view.findViewById(R.id.name);
            address=view.findViewById(R.id.addr);
            rssi=view.findViewById(R.id.rssi);
        }
    }
}
