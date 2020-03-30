package com.a3nod.lenovo.sparrowfactory.mmi.wifi;

import android.content.Context;
import android.net.wifi.ScanResult;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.R;

import java.util.List;


/**
 * Created by jianzhou.peng on 2017/11/10.
 */

public class WifiAdapter extends BaseAdapter
{
    private Context mContext;
    private List<ScanResult> mScanResults = null;
    private LayoutInflater mInflater = null;
    private int id = -1;
    private boolean flag = false;

    public WifiAdapter(Context context, List<ScanResult> list)
    {
        mContext = context;
        mScanResults = list;
        mInflater = LayoutInflater.from(mContext);
    }

    @Override
    public int getCount()
    {
        return mScanResults.size();
    }

    @Override
    public Object getItem(int position)
    {
        return mScanResults.get(position);
    }

    @Override
    public long getItemId(int position)
    {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent)
    {
        ViewHolder holder;
        if (convertView == null)
        {
            holder = new ViewHolder();
            convertView = mInflater.inflate(R.layout.wifi_item, parent,false);
            holder.tv_ssid = (TextView) convertView.findViewById(R.id.ssid);
            holder.tv_level = (TextView) convertView.findViewById(R.id.level);
            holder.tv_cnnt = (TextView) convertView.findViewById(R.id.connect);
            holder.tv_discnnt = (TextView) convertView.findViewById(R.id.disconnect);
            convertView.setTag(holder);
        } else
        {
            holder = (ViewHolder) convertView.getTag();
        }
        if (mScanResults.size()>position){
            ScanResult item = mScanResults.get(position);
            holder.tv_ssid.setText(item.SSID);
            holder.tv_level.setText("" + item.level + "dbm");

            if (id == position)
            {
                if (flag)
                {
                    holder.tv_discnnt.setVisibility(View.VISIBLE);
                    holder.tv_cnnt.setVisibility(View.INVISIBLE);
                } else
                {
                    holder.tv_cnnt.setVisibility(View.VISIBLE);
                    holder.tv_discnnt.setVisibility(View.INVISIBLE);
                }
//            holder.tv_discnnt.setVisibility(View.VISIBLE);
//            holder.tv_cnnt.setVisibility(View.INVISIBLE);
            } else
            {
                holder.tv_cnnt.setVisibility(View.INVISIBLE);
                holder.tv_discnnt.setVisibility(View.INVISIBLE);
            }
        }

//        if(id == position && flag){
//            holder.tv_discnnt.setVisibility(View.VISIBLE);
//            holder.tv_cnnt.setVisibility(View.INVISIBLE);
//        }else if(id == position && !flag){
//            holder.tv_cnnt.setVisibility(View.VISIBLE);
//            holder.tv_discnnt.setVisibility(View.INVISIBLE);
//        }
        return convertView;
    }

    class ViewHolder
    {
        TextView tv_ssid, tv_level, tv_cnnt, tv_discnnt;
    }

    public void setData(List<ScanResult> data)
    {
        mScanResults = data;
    }

    public void setId(int _id)
    {
        id = _id;
    }

    public void setFlag(boolean flag)
    {
        this.flag = flag;
    }
}
