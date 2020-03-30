package com.a3nod.lenovo.sparrowfactory.runin.report;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.TestItem;

import java.util.ArrayList;
import java.util.List;

/**
 * Description
 * Created by aaa on 2017/11/17.
 */

public class ReportAdapter extends BaseAdapter
{
    Context context;
    List<TestItem> lti;
    ReportAdapter(Context context){
        this(context,new ArrayList<TestItem>());
    }
    ReportAdapter(Context context, List<TestItem> lti){
        this.context=context;
        this.lti=lti;
    }
    public void updateReport(List<TestItem> lti){
        this.lti=lti;
        notifyDataSetChanged();
    }
    @Override
    public int getCount()
    {
        return lti.size();
    }

    @Override
    public Object getItem(int position)
    {
        return lti.get(position);
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
        if(convertView==null){
            holder=new ViewHolder();
            convertView=LayoutInflater.from(context).inflate(R.layout.list_item_report,parent,false);
            holder.tv_name=convertView.findViewById(R.id.list_item_report_name);
            holder.tv_result_success=convertView.findViewById(R.id.list_item_success);
            holder.tv_result_fail=convertView.findViewById(R.id.list_item_fail);
            holder.tv_remark=convertView.findViewById(R.id.list_item_report_remark);
            convertView.setTag(holder);
        }else{
            holder=(ViewHolder)convertView.getTag();
        }

        TestItem ti=lti.get(position);
        holder.tv_name.setText(ti.getName());
        holder.tv_result_success.setText(ti.getSuccess()+"");
        holder.tv_result_fail.setText(ti.getFail()+"");
        holder.tv_remark.setText(ti.getRemark());

        return convertView;
    }

    class ViewHolder{
        TextView tv_name;
        TextView tv_result_success;
        TextView tv_result_fail;
        TextView tv_remark;
    }
}
