package com.a3nod.lenovo.sparrowfactory.mmi;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;

import java.util.List;

/**
 * Created by Lenovo on 2017/11/15.
 */

public class MainGridAdapter extends BaseAdapter
{
    private List<String> mList;
    private Context mContext;
    private String[][] mResult = {/*{ASSYEntity.getInstants().CameraResult, ASSYEntity.getInstants().CameraTestTimes + ""},*/ {ASSYEntity.getInstants().BacklightResult, ASSYEntity.getInstants().BacklightTestTimes + ""},
            {ASSYEntity.getInstants().ButtonsResult, ASSYEntity.getInstants().ButtonsTestTimes + ""}, {ASSYEntity.getInstants().SpeakerResult, ASSYEntity.getInstants().SpeakerTestTimes + ""},
            {ASSYEntity.getInstants().MicResult, ASSYEntity.getInstants().MicTestTimes + ""}, /*{ASSYEntity.getInstants().GsensorResult, ASSYEntity.getInstants().GsensorTestTimes + ""},*/
            {ASSYEntity.getInstants().PsensorResult, ASSYEntity.getInstants().PsensorTestTimes + ""}, {ASSYEntity.getInstants().LsensorResult, ASSYEntity.getInstants().LsensorTestTimes + ""}, {ASSYEntity.getInstants().RGBsensorResult, ASSYEntity.getInstants().RGBsensorTestTimes + ""},
            {ASSYEntity.getInstants().BTResult, ASSYEntity.getInstants().BTTestTimes + ""}, {ASSYEntity.getInstants().WifiResult, ASSYEntity.getInstants().WifiTestTimes + ""},
            {ASSYEntity.getInstants().LCDResult, ASSYEntity.getInstants().LCDTestTimes + ""}, {ASSYEntity.getInstants().TPResult, ASSYEntity.getInstants().TPTestTimes + ""},
            {ASSYEntity.getInstants().PointResult, ASSYEntity.getInstants().PointTestTimes + ""}};

    public MainGridAdapter(List<String> list, Context context)
    {
        this.mList = list;
        this.mContext = context;
    }
    public void updateData(){
        mResult = new String[][]{/*{ASSYEntity.getInstants().CameraResult, ASSYEntity.getInstants().CameraTestTimes + ""}, */{ASSYEntity.getInstants().BacklightResult, ASSYEntity.getInstants().BacklightTestTimes + ""},
                {ASSYEntity.getInstants().ButtonsResult, ASSYEntity.getInstants().ButtonsTestTimes + ""}, {ASSYEntity.getInstants().SpeakerResult, ASSYEntity.getInstants().SpeakerTestTimes + ""},
                {ASSYEntity.getInstants().MicResult, ASSYEntity.getInstants().MicTestTimes + ""}, /*{ASSYEntity.getInstants().GsensorResult, ASSYEntity.getInstants().GsensorTestTimes + ""},*/
                {ASSYEntity.getInstants().PsensorResult, ASSYEntity.getInstants().PsensorTestTimes + ""}, {ASSYEntity.getInstants().LsensorResult, ASSYEntity.getInstants().LsensorTestTimes + ""},{ASSYEntity.getInstants().RGBsensorResult, ASSYEntity.getInstants().RGBsensorTestTimes + ""},
                {ASSYEntity.getInstants().BTResult, ASSYEntity.getInstants().BTTestTimes + ""}, {ASSYEntity.getInstants().WifiResult, ASSYEntity.getInstants().WifiTestTimes + ""},
                {ASSYEntity.getInstants().LCDResult, ASSYEntity.getInstants().LCDTestTimes + ""}, {ASSYEntity.getInstants().TPResult, ASSYEntity.getInstants().TPTestTimes + ""},
                {ASSYEntity.getInstants().PointResult, ASSYEntity.getInstants().PointTestTimes + ""}};
        notifyDataSetChanged();
    }

    @Override
    public int getCount()
    {
        return mList.size();
    }

    @Override
    public Object getItem(int i)
    {
        return mList.get(i);
    }

    @Override
    public long getItemId(int i)
    {
        return i;
    }

    @Override
    public View getView(int i, View view, ViewGroup viewGroup)
    {
        ViewHolder viewHolder = null;
        if (view == null)
        {
            view = LayoutInflater.from(mContext).inflate(R.layout.grid_item_mmi, viewGroup,false);
            viewHolder = new ViewHolder(view);
            view.setTag(viewHolder);
        } else
        {
            viewHolder = (ViewHolder) view.getTag();
        }
        viewHolder.name.setText(mList.get(i));

        if (i <= mResult.length - 1)
        {
            if (!mResult[i][0].equals("null"))
            {
                if (mResult[i][0].equals("success"))
                {
                    viewHolder.result.setText("√");
                } else
                {
                    viewHolder.result.setText("×");
                }
            }
        }
        return view;
    }

    static class ViewHolder
    {
        TextView name, result;

        public ViewHolder(View view)
        {
            result = view.findViewById(R.id.gv_item_tv_result);
            name = view.findViewById(R.id.gv_item_tv_name);
        }
    }
}
