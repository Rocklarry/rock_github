/*************************************************************************
	> File Name: read_bin.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年04月16日 星期四 09时21分18秒
 ************************************************************************/




#include <stdio.h>
#include <stdlib.h>

typedef signed char int8_t;

char * ReadFile(char * filename)
{
    char *text;
    int i;


    FILE *pf = fopen(filename,"r");
    fseek(pf,0,SEEK_END);
    long lSize = ftell(pf);
    // 用完后需要将内存free掉
    text=(void *)malloc(lSize+1);
    rewind(pf);
    fread(text,sizeof(char),lSize,pf);
    //text[lSize] = '\0';

    printf("%ld\n",lSize );

    //for ( i = 0; i < lSize; ++i)
	for ( i = 0; i < 16; ++i)
    {
    	printf("0x%x    len=%d \n", text[i],i);
    }
    free(text);
    fclose(pf);

    return text;
}


void  ReadFile_size(char * filename)
{
    char text[32];
    int i;
	static int N=0;

    FILE *pf = fopen(filename,"r");
    fseek(pf,0,SEEK_END);
    long lSize = ftell(pf);
	printf("%ld\n",lSize );
	rewind(pf);
	
	for(N=0; N<lSize/32; ++N)
	{
		fseek(pf,0,SEEK_CUR);
		fread(text,sizeof(char),32,pf);
		for ( i = 0; i < 32; ++i)
		{
			printf("%d = 0x%x  \n ", i,text[i]);
		}

	}
    fclose(pf);

}


int main()
{
	//ReadFile("D7_FW.bin");
	ReadFile_size("D7_FW.bin");
	
	//ReadFile("Alexa.bin");
	//ReadFile("ASRP.bin");
	//ReadFile("Pomelo.bin");
	//ReadFile("Addon_ASRP.bin");
	//ReadFile("Sensory.bin");
	
	return 0;
}