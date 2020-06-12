
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

    for ( i = 0; i < lSize; ++i)
    {
    	//printf("0x%x    len=%d \n", text[i],i);
    }
    free(text);
    fclose(pf);

    return text;
}


int main()
{
	//ReadFile("Addon_ASRP.bin");
	ReadFile("Alexa.bin");
	
	return 0;
}