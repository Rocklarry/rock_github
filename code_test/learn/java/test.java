/*************************************************************************
	> File Name: test.java
	> Author: Rock
	> Mail: ruidongren@163.com 
	> Created Time: 2017年05月26日 星期五 10时00分50秒
 ************************************************************************/

public class test
{	
	public static void main(String[] args)
	{
		System.out.println(" java test\n\n");

		System.out.println("GetAge"+GetAge(4)+"\n\n");
		System.out.println("递归算法GetAge_st="+GetAge_st(4)+"\n\n");
		System.out.println("GetAge_num="+GetAge_num(10,8)+"\n\n");
		

	
	}

static int GetAge(int num)
{
	int age = 10;
	while (num>1)
	{
	age += 2;

	num -= 1;
	}
	return age;
}

static int GetAge_st(int num)
	{
		if (num==1)
			return 10;
		return GetAge_st(num-1)+2;
	}

static int GetAge_num(int num,int acc)
	{
            if (num == 1)
                return acc;
            return GetAge_num(num-1,acc+2);
	}


}



