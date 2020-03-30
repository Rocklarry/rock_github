#ifndef  _CONVERTION_H
#define _CONVERTION_H



/* 十六进制转BCD码 */
#define CDZ_HEXTOBCD(a)				(((a/10)*16)+(a%10))
#define CDZ_BCDTOHEX(b)				(((b&0xF0)>>4)*10 + (b&0x0F))	

void Hex4ToFloat(ST_CHAR *Hexdata,ST_FLOAT *f_data,ST_CHAR big_duan );
void FloatToHex4(ST_FLOAT f_data,ST_UCHAR *data);
ST_UCHAR CheckSum(ST_UCHAR *data,ST_INT32 dataLen);
ST_INT32 BCDToInt32(const ST_UCHAR *pData);
ST_UCHAR Int32ToBCD(ST_UCHAR Data);
ST_UCHAR  GetBCC(ST_UCHAR *byData,ST_INT32 iDataLen);
ST_INT32 BCDToInt(const ST_CHAR *pData);


#endif

