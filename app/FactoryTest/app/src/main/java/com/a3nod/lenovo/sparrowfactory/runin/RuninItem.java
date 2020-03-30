package com.a3nod.lenovo.sparrowfactory.runin;

/**
 * Description
 * Created by aaa on 2017/12/27.
 */
public class RuninItem
{
    public RuninItem(int id,int stringId,int itemDuration,Class cls){
        this.itemDuration=itemDuration;
        this.itemNameId=id;
        this.stringId=stringId;
        this.cls=cls;
    }
    int itemNameId;
    int stringId;
    int itemDuration;
    Class cls;
}
