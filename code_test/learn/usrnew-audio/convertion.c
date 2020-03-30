
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DataType.h"

/********************************************************************************* 
 函数名称： Hex4ToST_FLOAT
 功能描述：	4字节十六进制金额转浮点数
 输    入：	Hexdata：十六进制数据buffer，f_data：转换后的浮点数
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void Hex4ToFloat(ST_CHAR *Hexdata,ST_FLOAT *f_data,ST_CHAR big_duan )
{
	ST_UCHAR i;
	ST_INT32 temp = 0;
	for(i=0;i<4;i++)
	{
		if(big_duan==1)
		{
			temp+=Hexdata[i]<<((3-i)*8);
		}
		else
		{
			temp+=Hexdata[i]<<(i*8);
		}			
	}
	*f_data = (ST_FLOAT)(temp/100.00);
}

/********************************************************************************* 
 函数名称： FloatToHex4
 功能描述：	浮点数转化为十六进制
 输    入：	f_data：未转化的浮点数，data：转换后的十六进制
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void FloatToHex4(ST_FLOAT f_data,ST_UCHAR *data)
{
	ST_UCHAR i;
	ST_INT32 temp = 0;
	temp = (ST_INT32)(f_data*100);
	for(i=0;i<4;i++)
	{
		data[i]=(ST_UCHAR)(temp>>(i*8));	//低字节在前，高字节在后
				
	}

}


/********************************************************************************* 
 函数名称： CheckSum
 功能描述：	计算校验和
 输    入：	data：参加校验数据buffer,datalen：数据长度
 输	   出： 无
 返 回 值： 计算出的校验和
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_UCHAR CheckSum(ST_UCHAR *data,ST_INT32 dataLen)
{
	ST_UCHAR Sum = 0x00;
	ST_INT32 i;
	for (i = 0; i < dataLen; i++)
	{
		Sum +=data[i];
	}
	return Sum;
}


/********************************************************************************* 
 函数名称： BCDToInt32
 功能描述：	将BCD转换成int
 输    入：	pData:BCD字符串
 输	   出： 无
 返 回 值： 计算出的整数
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/


ST_INT32 BCDToInt(const ST_CHAR *pData)
{
	ST_INT32 ulData=0;
	ulData=(ST_INT32)(pData[0]&0x0f)+(pData[0]>>4)*10;
	ulData+=(ST_INT32)(pData[1]&0x0f)*100+(ST_INT32)(pData[1]>>4)*1000;
	ulData+=(ST_INT32)(pData[2]&0x0f)*10000+(ST_INT32)(pData[2]>>4)*100000;
	ulData+=(ST_INT32)(pData[3]&0x0f)*1000000+(ST_INT32)(pData[3]>>4)*10000000;
	return ulData;
}


/********************************************************************************* 
 函数名称： Int32ToBCD
 功能描述：	将int转换成BCD
 输    入：	Data:int数据
 输	   出： 无
 返 回 值： 计算出的BCD
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_UCHAR Int32ToBCD(ST_UCHAR Data)
{
	return (((Data / 10) << 4) + (Data % 10));
}



/********************************************************************************* 
 函数名称： GetBCC
 功能描述：	计算BCC
 输    入：	byData：待计算的数据buffer,iDataLen：数据长度
 输	   出： 无
 返 回 值： BCC
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

 ST_UCHAR  GetBCC(ST_UCHAR *byData,ST_INT32 iDataLen)
 {
         ST_UCHAR byDataBCC = *byData;
         ST_INT32 i;
         for (i = 1;i < iDataLen;i++)
         {
                 byDataBCC ^= *(byData + i);
         }

         return byDataBCC;
 }

