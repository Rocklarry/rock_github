/*************************************************************************
	> File Name: test.cpp
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2017年11月22日 星期三 15时39分53秒
 ************************************************************************/

//新数组的使用
#include<iostream>
#include<array>
#include<stdlib.h>
#include<vector>
#include<algorithm>
#include <string.h>

#include <fstream> 


#include <stdio.h>  

using namespace std;

//typedef unsigned int            char32_t;
void array_test(void);
void vector_test();

float coldata[]  = {1,2,3,4,5,6,7,8,9};

		
float b[] = {0.60816233, 
		1.14792959,
		2.54674062,
		2.44733487,
		2.54674062,
		1.14792959,
		0.60816233};
		
float a[] = { 1.,
		1.58060653,
		2.8675266, 
		2.35079111, 
		2.07251069, 
		0.81179641, 
		0.3697686 };

/*
float b[] = {0.6082,
				1.1479,
				2.5467,
				2.4473,
				2.5467,
				1.1479,
				0.6082}; 
float a[] = {
			1.,
			1.5806,
			2.8675,
			2.3502,
			2.0725,
			0.8118,
			0.3698};
			*/

float dot(float *ax, float *bx,  int len)
{
	 int i;
	 float res = 0;
	for( i = 0; i < len; i++) 
	{	
		res += ax[i]*bx[i];	
	}
	//snav_info_print("res=[%.8f] ",res);
	return res;
}
float dot_tttt(float *ax, float *bx,  int len)
{
	 int i;
	 float *var = {0};
	 float res = 0;
	 
	 var=(float*)malloc(len*sizeof(float));

	for(i = 0;i<len;i++)
	{
	/*cout <<i<<"ax[i]="<<ax[i]<<endl;
	cout <<i<<"bx[i]="<<bx[i]<<endl;*/
	var[i]= ax[i]*bx[i];
	 
	}

	for( i = 0; i < len; i++) 
        res += var[i];
  
	//cout <<"res="<<res<<endl;
	
	return res;
}

float *iir_filter_tt(float *coldata,float *a,float *b)  
{  
	float xs[] = {0,0,0,0,0,0,0};
	float c[] = {1.5806,2.8675,2.3502,2.0725,0.8118,0.3698};
	float ys[] = {0,0,0,0,0,0,0};
	float as[] = {0,0,0,0,0,0};
	float bs[] = {0,0,0,0,0,0};


 
	   
	   
	int count = 6;//sizeof(coldata);

	//cout << count<<endl;

	float *out;
	 out=(float*)malloc(count*sizeof(float));


	for(int i = 0;i<count;i++)
	{
		memset(as,0,6);

		for(int t = 0;t<6;t++)
			as[t] = xs[t];

		for(int j = 1;j<count;j++)
			xs[j] = as[j-1];

		xs[0] = coldata[i];

		/*for(int t = 0;t<count;t++)
			cout << "---xs" << t << "="  <<xs[t] <<" " ;
			cout <<endl ;*/



		//cout << "b[i]="<< b[i] <<endl;
		out[i] = dot(b,xs,7) - dot(c,ys,6);
		//cout << "out[0]="<< out[i] <<endl;
		//cout <<"dot(b,xs,7)="<<dot(b,xs,7)<<endl;
		//cout <<"dot(b,xs,6)="<<dot(c,ys,6)<<endl;



		memset(bs,0,6);

		for(int t = 0;t<count-1;t++)
			bs[t] = ys[t];

		for(int j = 1;j<6;j++)
			ys[j] = bs[j-1];

		ys[0] = out[i];

		/*for(int t = 0;t<6;t++)
			cout << "---ys" << t << "="  <<ys[t] <<" " ;
			cout <<endl ;*/
		

		cout <<"out"<<i<<"="<<out[i]<<endl;
		
	}
	return out;  
} 




int file_copy()
{
	ifstream in("test");  
    string filename;  
    string line;  
  
    if(in) // 有该文件  
    {  
        while (getline (in, line)) // line中不包括每行的换行符  
        {   
            cout << line << endl;  
        }  
    }  
    else // 没有该文件  
    {  
        cout <<"no such file" << endl;  
    }  
  
    return 0;  

}

void test_file()
{
	FILE * pFile;
    char mystring [100];
    pFile = fopen ("test.dat" , "r");
    if (pFile == NULL)
        perror ("Error opening file");
    else {
		for(int i=0;i<10;i++){
        if ( fgets (mystring , 100 , pFile) != NULL )
            puts (mystring);
		printf("%s",mystring);
		}
        fclose (pFile);
    }
}

float fxs[] = {0,0,0,0,0,0,0};
//float c[] = {1.5806,2.8675,2.3502,2.0725,0.8118,0.3698};
float fys[] = {0,0,0,0,0,0,0};
float as[] = {0,0,0,0,0,0};
float bs[] = {0,0,0,0,0,0};


float iir_filter(float *newdata, float *xs, float *ys, float *a, float *b, int nn) 
{ 
	
	int i; 
	float *result;
	result=(float*)malloc(6);
	//move xs 
	
	float c[6]={0};
	
	for(int j=1;j<6;j++)
	{
	for(i=1;i<nn;i++)
	{
		c[i-1] = a[i];
		//printf("c[%d]=[%.8f] \n",i,c[i-1]);
	}
	
	for(i=0;i<nn-1;i++) 
		xs[nn-i-1] = xs[nn-i-2]; 
	xs[0] = newdata[j]; 

	result[j] = dot(b,xs,nn) - dot(c,ys,nn - 1); 
	
	//printf("newdata=[%.8f] result1=[%.8f] result2=[%.8f]\n",newdata,dot(b,xs,nn), dot(c,ys,nn - 1));
	
	//move ys and save result. 
	for(i=0;i<nn-2;i++){
		ys[nn-2-i] = ys[nn-3-i];
		//printf("ys[%d]=[%.8f]\n",nn-2-i,ys[nn-2-i]);
		
	}
	ys[0] = result[j];
	printf("\n result=[%.8f]\n",result[j]);
	}
	return *result; 
}

int main(int argv ,char *args []) {

int len =7;
float data []= {1,0.8,0.5,0.1,0.05,0.012,0.01254};
float data_tt[] = {1,0.8,0.5,0.1,0.05,0.012,0.01254};
	printf("\n\n***************************iiiiiirrrrr***********************************\n\n");
	//iir_filter(coldata,a,b);
	
	/*for(int i=0;i<200;i++)
	{
	iir_filter(data,fxs,fys,a,b,len);
	iir_filter_tt(data_tt,a,b);
	}*/
	file_copy();
	

	printf("\n\n***************************iiiiiirrrrr***********************************\n\n");
	test_file();
	printf("\n\n*********test***********\n\n");

	array_test();

	char32_t  a = 10;

	printf("\n\n  a=%d\n\n",a);

    return 0;
}

void array_test(){
   //array数据类型
    printf("\n\n*********array数据类型***********\n\n");
    std::array<double ,5>data={2,3,4,5,6};
    for (auto var : data) {
        std::cout << "var =" << var << std::endl;
    }
    getchar();
	vector_test();
}

void vector_test(){

	printf("*********vector***********\n\n");

	std::vector<int>  myvec;
     myvec.push_back(1);
     myvec.push_back(2);
     myvec.push_back(3);
     myvec.push_back(4);
     myvec.push_back(5);
     int res = 0;
     //lambda 表达式  求和
     //&res直接操作一个变量,res等价于返回值，x代表参数
     //每次充当迭代器指向的元素，大括号就是代码
     std::for_each(myvec.begin(), myvec.end(), [&res](int x) {res += x; });
     std::cout << res << std::endl;
	 printf("求和：%d",res);
     std::cin.get();
}
