package com.a3nod.lenovo.sparrowfactory.runin.cpu;

import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

/**
 * Description
 * Created by aaa on 2017/11/22.
 */

public class CPUAlgorithm
{
    private static int N_qsort = 10000;//快排的数据规模
    private static int M = 20000, N = 50000;//整点、浮点运算的规模
    private static int N_pi = 100000000;//计算圆周率的运算规模
    private static double intResult, s_float, piResult, sortResult;

    public static int main()
    {
        LogUtil.i("------性能测试开始");
        int_comp();//整点运算
        float_comp();//浮点运算
        pi_comp();//泰勒级数推论式计算圆周率
        qsort();//快速排序
        LogUtil.i("------测试结束");
        LogUtil.i("整点运算得分:" + intResult);
        LogUtil.i("泰勒级数推论式计算圆周率运算得分:" + piResult);
        LogUtil.i("排序运算得分:%lf " + sortResult);
        LogUtil.i("总得分:%lf " + (intResult + s_float + piResult + sortResult));
        panduan();
        return 0;
    }

    public static void int_comp()
    {//整点加法
        LogUtil.i("整点运算测试中(运算次数为:) " + (double) M * N);
        long start, end;
        int i, j;
        start = System.currentTimeMillis();
        for (i = 0; i < M; i++)
        {
            for (j = 0; j < N; j++)
            {
            }
        }
        end = System.currentTimeMillis();
        double duration = (double) (end - start) / 1000;
        double score = (M * N) / duration;
     /*注：score本身即为运算速度，数量级一般在亿，为方便起见，本程序的分数均采用运算速度除以一万后的结果！除特殊说明，后面类同！*/
        intResult = score / 10000;
        //LogUtil.i("整点运算测试完毕!分数:%lf ",intResult);
    }

    public static void float_comp()
    {//浮点加法
        LogUtil.i("浮点运算测试中(运算次数为:) " + (double) M * N);
        long start, end;
        float i, j;
        start = System.currentTimeMillis();
        for (i = 0; i < M; i++)
        {
            for (j = 0; j < N; j++)
            {
            }
        }
        end = System.currentTimeMillis();
        double duration = (double) (end - start) / 1000;
        double score = (M * N) / duration;
        s_float = score / 10000;
        //LogUtil.i("浮点运算测试完毕!分数:%lf ",s_float);
    }

    public static void pi_comp()
    {
        LogUtil.i("泰勒级数推论式计算圆周率中 运算次数为:" + N_pi);
        int m, i = 1;
        double s = 0;
        long start, end;
        start = System.currentTimeMillis();
        for (m = 1; m < N_pi; m += 2)
        {
            s += i * (1.0 / m);
            i = -i;
        }
        end = System.currentTimeMillis();
        double duration = (double) (end - start) / 1000;
        double score = N_pi / duration;
        //下面一行可输出计算出来的圆周率
        //LogUtil.i("pi=%lf ",4*s);
        piResult = score / 10000;
        //LogUtil.i("泰勒级数推论式计算圆周率完毕!分数:%lf ",piResult);
    }

    public static void Qsort(int a[], int low, int high)
    {//快排算法
        if (low >= high) return;
        int first = low;
        int last = high;
        int key = a[first];
        while (first < last)
        {
            while (first < last && a[last] >= key) --last;
            a[first] = a[last];
            while (first < last && a[first] <= key) ++first;
            a[last] = a[first];
        }
        a[first] = key;
        Qsort(a, low, first - 1);
        Qsort(a, first + 1, high);
    }

    public static void qsort()
    {//调用快排算法的函数
        int[] a = new int[N_qsort];
        for (int i = N_qsort; i > 0; i--)
        {
            a[N_qsort - 1] = i;
        }
        LogUtil.i("排序运算中(对%d个数进行快速排序) " + N_qsort);//采用最坏时间方案
        long start, end;
        start = System.currentTimeMillis();
        Qsort(a, 0, N_qsort - 1);
        end = System.currentTimeMillis();
        double duration = (double) (end - start) / 1000;
        double score = (N_qsort * N_qsort) / duration;
        sortResult = score / 10000;
        //LogUtil.i("排序运算测试完毕!分数:%lf ",sortResult);
    }

    public static void unknow(){
        int n = 0;
        double a = 0;
        for (n = 0; n <= 400000; n++)
        {
            a = a + 4 * Math.pow(-1, n) / (2 * n + 1);
        }
        LogUtil.d("unknow finish ");
    }
    public static void unknow1(){
        long m = 1;
        int i = 1;
        do
        {
            m *= i;
            i++;
        } while (i <= 1000000000);
        LogUtil.d("unknow1 finish ");
    }

    public static void panduan()
    {
        double i = intResult + s_float + piResult + sortResult;
        LogUtil.i("根据分数，授予您的爱机<");
        if (i > 0 && i < 20000)
        {
            LogUtil.i("渣渣");
        } else if (i > 20000 && i < 30000)
        {
            LogUtil.i("低端");
        } else if (i > 30000 && i < 40000)
        {
            LogUtil.i("中端");
        } else if (i > 40000 && i < 50000)
        {
            LogUtil.i("高端");
        } else if (i > 50000 && i < 60000)
        {
            LogUtil.i("超高端");
        } else if (i > 60000)
        {
            LogUtil.i("机皇");
        }
        LogUtil.i(">称号 ");
    }

}
