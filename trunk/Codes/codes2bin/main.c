#include "datatypes.h"
#include "binary.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CODES 65000
#define STR_LEN	255



//removes leading and trailing spaces
UWORD strclean(char *str)
{
	UWORD i;
	UWORD len;
	//first remove leading spaces
	len=strlen(str);
	while(str[0]==' ')
	{
		for(i=1; i<len; i++)
		{
			str[i-1]=str[i];
		}
	}
	
	//remove trailing spaces and misc chars
	len=strlen(str);
	while (str[len-1]==' ' || str[len-1]==0x0A || str[len-1]==0x0D)
	{
		str[len-1]=0;
		len=strlen(str);
	}
	return len;

}

UDWORD codes[MAX_CODES];
UBYTE codeStr[MAX_CODES][STR_LEN];

void main()
{
	UWORD numCodes;
	UWORD i;
	FILE *fp;
	

	char code[STR_LEN];
	char str[STR_LEN];

	fp=fopen("../ccodes.txt","r");
	
	numCodes=0;
	while(!feof(fp))
	{
		fgets(code,STR_LEN,fp);
		strclean(code);
		if(code[0]=='U')
		{
			char * ptr;
			ptr=strchr(code,' ');
			strcpy(str,ptr);
			*ptr=0;

		}else
		{	
			fgets(str,STR_LEN,fp);
		}
		strclean(str);
		codes[numCodes]=atol(&code[1]); //skip the 'P'
		strcpy(codeStr[numCodes],str);
		if (codes[numCodes]>0)
			numCodes++;
		fgets(code,STR_LEN,fp);
	}
	fclose(fp);


	//now we need to write the codes to a binary file
	fp=fopen("test.bin","wb+");
	for(i=0; i<numCodes; i++)
	{
		UDWORD len,j;
		putDword(fp,codes[i]);
		len=strlen(codeStr[i]);
		if (len%4)
		{
			len=len+(4-(len%4));
		}
		putDword(fp,len);
		for(j=0; j<strlen(codeStr[i]);j++)
		{
			putByte(fp,codeStr[i][j]);
		}
		for(; j<len; j++)
		{
			putByte(fp,0);
		}
	}
	fclose(fp);
	
	for(i=0; i<numCodes; i++)
	{
		printf("P%d\t%s\n",codes[i],codeStr[i]);
	}
	printf("Number of codes %u\n",numCodes);




	return;
}
