#ifndef  _DATATYPE_H_
#define _DATATYPE_H_
#include <stdlib.h>
#include <stdio.h>


typedef unsigned long       DWORD;
#ifndef ST_VOID
#define ST_VOID					void
#endif


#ifndef ST_CHAR
#define ST_CHAR					char
#endif

#ifndef ST_UCHAR
#define ST_UCHAR				unsigned char
#endif



#ifndef ST_INT16
#define ST_INT16				signed short
#endif

#ifndef ST_UINT16
#define ST_UINT16				unsigned short
#endif

#ifndef ST_INT32
#define ST_INT32				int
#endif

#ifndef ST_UINT32
#define ST_UINT32				unsigned int
#endif	

#ifndef ST_LONG
#define ST_LONG					long
#endif

#ifndef ST_ULONG
#define ST_ULONG				unsigned long
#endif

#ifndef ST_RET
#define ST_RET					int
#endif

#ifndef ST_FLOAT
#define ST_FLOAT				float
#endif

#ifndef SD_SUCCESS
#define SD_SUCCESS				0
#endif

#ifndef SD_FAILURE		
#define SD_FAILURE				1
#endif

#ifndef ST_SOCKET				
#define ST_SOCKET				int
#endif

#ifndef ST_BOOL					
#define ST_BOOL					int
#endif

#ifndef false            
#define false                                    0
#endif

#ifndef true
#define true                                     1
#endif

#ifndef BYTE
#define BYTE                                    unsigned char
#endif


#endif
