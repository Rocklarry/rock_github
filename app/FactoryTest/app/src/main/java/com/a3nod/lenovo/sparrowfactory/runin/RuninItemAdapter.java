package com.a3nod.lenovo.sparrowfactory.runin;

import android.content.Context;
import android.util.SparseArray;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.Spinner;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

/**
 * Description
 * Created by aaa on 2017/12/27.
 */

public class RuninItemAdapter extends BaseAdapter
{
    Context context;
    SparseArray<RuninItem> lri;
    RuninConfig config=RuninConfig.getInstance();


    public RuninItemAdapter(Context context, SparseArray<RuninItem> lri)
    {
        this.lri = lri;
        this.context = context;
    }

    @Override
    public int getCount()
    {
        return lri.size();
    }

    @Override
    public Object getItem(int position)
    {
        return lri.get(position);
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
            convertView = LayoutInflater.from(context).inflate(R.layout.li_runin, parent,false);
            holder.tv_name = (TextView) convertView.findViewById(R.id.li_runin_tv_item_name);
            holder.sp_duration = (Spinner) convertView.findViewById(R.id.li_runin_sp_item_duration);
            initSpinner(holder.sp_duration);
            convertView.setTag(holder);
        } else
        {
            holder = (ViewHolder) convertView.getTag();
        }
//        config.getItemDuration();
        RuninItem ri = lri.get(position);
        LogUtil.i("position : "+position +" name : "+ri.itemNameId);
        holder.tv_name.setText(ri.stringId);
        if (ri.itemNameId == RuninConfig.RUNIN_REBOOT_ID)
        {
            holder.tv_name.setVisibility(View.VISIBLE);
            holder.sp_duration.setVisibility(View.INVISIBLE);
        }else if(ri.itemNameId ==RuninConfig.RUNIN_CAMERA_ID) {
            holder.tv_name.setVisibility(View.GONE);
            holder.sp_duration.setVisibility(View.GONE);
        }else{
            holder.sp_duration.setVisibility(View.VISIBLE);
            holder.tv_name.setVisibility(View.VISIBLE);
            holder.sp_duration.setSelection(RuninConfig.getPositionByDuration(ri.itemDuration));
            holder.sp_duration.setOnItemSelectedListener(new ItemSelectedListener(ri));
        }
        return convertView;
    }

    public void initSpinner(Spinner sp_duration)
    {
        SparseArray<RuninConfig.TimeSetting> times=RuninConfig.getTimeSetting();
        String[] arrray=new String[times.size()];
        for(int i=0;i<times.size();i++){
            arrray[i]=times.get(i).durationText;
        }
        ArrayAdapter aa=new ArrayAdapter<String>(context, R.layout.layout_spinner, arrray);
        aa.setDropDownViewResource(R.layout.layout_spinner_dropdown);
        sp_duration.setAdapter(aa);
    }

    private static class ItemSelectedListener implements AdapterView.OnItemSelectedListener
    {
        public RuninItem item;

        public ItemSelectedListener(RuninItem item)
        {
            this.item = item;
        }

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
        {
            setDuration(position);
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent)
        {

        }

        public void setDuration(int position)
        {
            RuninConfig config = RuninConfig.getInstance();
            config.setItemDuration(item.itemNameId,position);

            config.saveConfig();
        }
    }

    public int getDuration(RuninItem item)
    {
        RuninConfig config = RuninConfig.getInstance();
        return config.getItemDuration(item.itemNameId);
    }


    private static class ViewHolder
    {
        TextView tv_name;
        Spinner sp_duration;
    }

}
