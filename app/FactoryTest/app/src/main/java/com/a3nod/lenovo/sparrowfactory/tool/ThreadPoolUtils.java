package com.a3nod.lenovo.sparrowfactory.tool;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

/**
 * Description
 * Created by aaa on 2018/1/23.
 */

public class ThreadPoolUtils
{
    private static int MAX_THREAD_COUNT = 6;
    private static ExecutorService multiThreadExecutor = Executors.newCachedThreadPool();
    private static ExecutorService singleThreadExecutor = Executors.newSingleThreadExecutor();
    private static ScheduledExecutorService singleScheduleThreadExecutor = Executors.newSingleThreadScheduledExecutor();

    private static ExecutorService serverSocketThreadExecutor = Executors.newSingleThreadExecutor();
    private static ExecutorService clientSocketThreadExecutor = Executors.newCachedThreadPool();


    public static void executeMultiThread(Runnable run)
    {
        multiThreadExecutor.execute(run);
    }

    public static void executeServerSocketThread(Runnable run)
    {
        serverSocketThreadExecutor.execute(run);
    }
    public static void executeClientSocketThread(Runnable run)
    {
        clientSocketThreadExecutor.execute(run);
    }

    public static void executeSingleThread(Runnable run)
    {
        singleThreadExecutor.execute(run);
    }

    public static void executeSingleScheduleThread(Runnable run, Long delay, long period)
    {
        singleScheduleThreadExecutor.scheduleAtFixedRate(run, delay, period, TimeUnit.SECONDS);
    }

}
