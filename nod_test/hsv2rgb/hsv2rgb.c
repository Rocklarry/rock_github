/*************************************************************************
	> File Name: hsv2rgb.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年03月30日 星期一 11时23分01秒
 ************************************************************************/

#include<stdio.h>


/*
h:0~360
s:0~1
v:0~1
*/
void hsv2rgb(float h, float s, float v)
{
	float h60;
	float f;
	unsigned int h60f;
	unsigned int hi;
 
    float p, q, t;
    float r, g, b;
    unsigned int color_r, color_g, color_b;
 
    printf("h:%.2f s:%.2f v:%.2f\r\n", h, s, v);
 
	h60 = h / 60.0;
	h60f = h / 60;
 
	hi = (int)h60f % 6;
	f = h60 - h60f;
 
    p = v * (1 - s);
    q = v * (1 - f * s);
    t = v * (1 - (1 - f) * s);
 
    r = g = b = 0;
    switch(hi)
    {
        case 0:
            r = v;  g = t;  b = p;
        break;
 
        case 1:
            r = q;  g = v;  b = p;
        break;
 
        case 2:
           r = p;   g = v;  b = t;
        break;
 
        case 3:
            r = p;  g = q;  b = v;
        break;
 
        case 4:
            r = t;  g = p;  b = v;
        break;
 
        case 5:
            r = v;  g = p;  b = q;
        break;
    }
 
    r = (r * 255.0) * 100;
    g = (g * 255.0) * 100;
    b = (b * 255.0) * 100;
 
    color_r = (r + 50) / 100;
    color_g = (g + 50) / 100;
    color_b = (b + 50) / 100;
 
    printf("color_r:%u color_g:%u color_b:%u\n\n", color_r, color_g, color_b);
 }

int main()
{
	//色相H：用角度度量，取值范围为0°～360°，从红色开始按照逆时针方向计算，红色为0°，绿色为120°，蓝色为240°，黄色为60°，青色为180°，品红为300°，对应于绕圆柱的中心轴的角度。
	//饱和度S：表示色彩的纯度，对应于离圆柱的中心轴的距离。数值越高颜色则深而艳，数值越低则颜色逐渐变灰，取值范围为0.0~1.0，白色的S=0。
	//明度V：表示颜色的明亮程度。取值范围为0.0（黑色）~1.0（白色）。对应于绕圆柱的中心轴的高度，轴取值为自底部的黑色V=0到顶部的白色V=1。
	int h = 120;
	int s = 0.5;
	int v = 0.5;

	hsv2rgb(h,s,v);

}