/*************************************************************************
	> File Name: test.cpp
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2017年11月22日 星期三 15时39分53秒
 ************************************************************************/

#include<iostream>
#include<array>
#include<stdlib.h>
#include<vector>
#include <cstring> //新式头文件

#include <stdio.h>  

using namespace std;

void strlen_test()
{
	const int Size = 15;
	char name1[Size];
	char name2[Size] = "C++ owboy";
	
	cout << "Howdy! I'm" << name2;
	cout <<"! what's your name?\n";
	cin >>name1;
	cout << "Well   " << name1 << "  ,\n your name has\n";
	cout << strlen(name1) << " letters and stored \n";
	cout << "in an array of " << sizeof(name1) <<"  bytes \n";
	cout << "your initial is" << name1[0]<< " \n";
	name2[3] = '\0';
	cout << "Here are the first 3 characters of my name;\n";
	cout << name2 << endl;
}

void zhizheng_test()
{
	printf("\n\n\r\r\r*****指针test*****\n");

	int higgens = 5;
	int *pt = &higgens;
	
	cout << "value of higgens = " << higgens
		<< "; Address of higgens = " << &higgens << endl;

	cout << "\nvalue of *pt=" << *pt 
		<< "; value of pt =" << pt <<endl;
}

void new_arrary_test()
{
	printf("\n\n\r\r\r*****new  test*****\n");
	double *p3 = new double [3];
	p3[0] = 0.2;
	p3[1] = 0.5;
	p3[2] = 0.8;

	cout << "p3[1] = " << p3[1] << ".\n";
	
	p3 = p3 + 1;
	cout << "Now p3[0]= " << p3[0] << " and ";
	cout << "p3[1] = " << p3[1] << ".\n";
	
	//delete [] p3;

	p3 = p3 - 1;//给delete提供正确的地址

	delete [] p3;

}

struct inflatable
{
	char name[20];
	float volume;
	double price;
};

void new_inflatable_test()
{
	printf("\n\n\r\r\r*****inflatable  test*****\n");
	
	inflatable *ps = new inflatable;
	cout << "Enter name of inflatable item:";
	cin.get(ps -> name,20);
	
	cout << "Enter volume in cubic feet:";
	cin >> (*ps).volume;
	
	cout << "Enter price: $";
	cin >> ps -> price;

	cout << "Name:" << (*ps).name <<endl;
	cout << "Volume:" << ps -> volume << " cubic feet \n";
	cout << "price : $" << ps -> price << endl;

	delete ps;


}

int main(int argv ,char *args []) {


	//strlen_test();
	zhizheng_test();
	new_arrary_test();
	new_inflatable_test();

    return 0;
}
