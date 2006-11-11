#include "binary.h"

UBYTE putByte(FILE *fp, UBYTE data)
{
	fputc(data,fp);
	return 0;
}

UBYTE putWord(FILE *fp, UWORD data)
{
	if (BYTE_ORDER==BIG_ENDIAN)
	{
		putByte(fp,data & 0x00FF);
		putByte(fp,(data & 0xFF00)>>8);
	}else
	{
		putByte(fp,(data & 0xFF00)>>8);
		putByte(fp,data & 0x00FF);
	}
	return 0;
}

UBYTE putDword(FILE *fp, UDWORD data)
{
	if (BYTE_ORDER==BIG_ENDIAN)
	{
		putByte(fp,data & 0x000000FF);
		putByte(fp,(data>>8) & 0x00FF);
		putByte(fp,(data>>16) & 0x00FF);
		putByte(fp,(data>>24) & 0x00FF);
	}else
	{
		putByte(fp,(data>>24) & 0x00FF);
		putByte(fp,(data>>16) & 0x00FF);
		putByte(fp,(data>>8) & 0x00FF);
		putByte(fp,data & 0x00FF);
	}
	return 0;
}

UBYTE getByte(FILE *fp)
{
	return fgetc(fp);
}

UWORD getWord(FILE *fp)
{
	UWORD data;
	UWORD temp;

	if (BYTE_ORDER==BIG_ENDIAN)
	{
		data=getByte(fp);
		temp=getByte(fp);
		temp=temp<<8;
		temp=temp | data;
	}else
	{
		temp=getByte(fp);
		data=getByte(fp);
		temp=temp<<8;
		temp=temp | data;
	}

	return temp;
}

UDWORD getDword(FILE *fp)
{
	UDWORD temp;
	UDWORD temp1;
	UDWORD temp2;
	UDWORD temp3;
	
	if (BYTE_ORDER==BIG_ENDIAN)
	{
		temp=getByte(fp);
		temp1=getByte(fp);
		temp2=getByte(fp);
		temp3=getByte(fp);
		temp1=temp1<<8;
		temp=temp1 | temp;
		temp2=temp2<<16;
		temp=temp2 | temp;
		temp3=temp3<<24;
		temp=temp3 | temp;
	}else
	{
		temp3=getByte(fp);
		temp2=getByte(fp);
		temp1=getByte(fp);
		temp=getByte(fp);
		temp1=temp1<<8;
		temp=temp1 | temp;
		temp2=temp2<<16;
		temp=temp2 | temp;
		temp3=temp3<<24;
		temp=temp3 | temp;
	}
	return temp;
	
}