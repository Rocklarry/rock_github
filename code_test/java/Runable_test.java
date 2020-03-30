/*************************************************************************
	> File Name: Runable_test.java
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: Tuesday, June 18, 2019 AM10:30:28 HKT
 ************************************************************************/

public class Runable_test implements Runnable{

	private int tickets = 10;

	public void run(){
	
		for (int i=0;i<=100;i++){
			if(tickets>0){
				System.out.println(Thread.currentThread().getName()+"--卖出票：" + tickets--);
			}
		}

	}

	public static void main(String[] args){
		Runable_test mvRunable = new Runable_test();
		Thread thread1 = new Thread(mvRunable,"窗口一");
		Thread thread2 = new Thread(mvRunable,"窗口二");
		Thread thread3 = new Thread(mvRunable,"窗口三");

		thread1.start();  
        thread2.start();  
        thread3.start(); 
	}
}
