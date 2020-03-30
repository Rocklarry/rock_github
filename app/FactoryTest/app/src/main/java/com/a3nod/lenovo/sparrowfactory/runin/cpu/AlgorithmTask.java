package com.a3nod.lenovo.sparrowfactory.runin.cpu;

/**
 * Description
 * Created by aaa on 2018/1/23.
 */
class AlgorithmTask implements Runnable
{

    private int taskId;
    private CPUHandler handler;

    public AlgorithmTask(int taskId, CPUHandler handler)
    {
        this.taskId = taskId;
        this.handler = handler;
    }

    @Override
    public void run()
    {
        task(taskId);
        handler.obtainMessage(CPUTestActivity.MSG_ALGORITHM_LOOG, taskId, 0).sendToTarget();
        //handler = null;
    }

    private void task(int taskId)
    {
        switch (taskId)
        {
            case CPUTestActivity.TASK_INT_TEST:
                CPUAlgorithm.int_comp();
                break;
            case CPUTestActivity.TASK_FLOAT_TEST:
                CPUAlgorithm.float_comp();
                break;
            case CPUTestActivity.TASK_PI_TEST:
                CPUAlgorithm.pi_comp();
                break;
            case CPUTestActivity.TASK_SORT_TEST:
                CPUAlgorithm.qsort();
                break;
            case CPUTestActivity.TASK_FIVE_TEST:
                CPUAlgorithm.unknow();
                break;
            case CPUTestActivity.TASK_SIX_TEST:
                CPUAlgorithm.unknow1();
                break;
        }
    }
}
