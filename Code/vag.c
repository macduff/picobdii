/*
Copyright (C) Trampas Stern  name of author

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/*******************************************************************
 *
 *  DESCRIPTION:
 *
 *  AUTHOR : Trampas Stern 
 *
 *  DATE : 10/30/2004  8:48:27 AM
 *
 *  FILENAME: vag.c
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/

#include "vag.h"


UBYTE vagGetBlock(UBYTE *data, UBYTE len)
{
	UBYTE resp;
	UBYTE ret;
	UBYTE temp;
	UBYTE *ptr;
	UBYTE count;
	UBYTE vagCount;

	//now lets get char from serial	
	ptr=data;
	count=0;
	temp=0x00;
	ret=iso_get(&temp,1,VAG_BLOCK_TIMEOUT);
	vagCount=temp;
	while(ret!=0)
	{
		*ptr++=temp;
		count++;
		if ((count>len) || (count>vagCount))
		{
			return count;
		}
		//printf("ECU\t\t%x\n\r",temp);
			 
		//invert data and send back
		resp=~temp;
		ret=iso_put(&resp,1,VAG_CHAR_DELAY);
		//printf("PC\t%x\n\r",resp);
		temp=0x00;
		ret=iso_get(&temp,1,VAG_CHAR_TIMEOUT);
	}
	return count;
}

UBYTE vagPutBlock(UBYTE *data, UBYTE len)
{
	UBYTE resp;
	UBYTE ret;
	UBYTE temp,i;
	
	//first we need to send the block len
	temp=len+1; //add for block end
	printf("PC->%x",temp);
	ret=iso_put(&temp,1,VAG_CHAR_DELAY);
	//get response
	ret=iso_get(&resp,1,VAG_CHAR_TIMEOUT);
	//printf("ECM->%x\n\r",resp);
	for(i=0; i<len; i++)
	{
		temp=data[i];
		printf(" %x",temp);
		ret=iso_put(&temp,1,VAG_CHAR_DELAY);
		//get response
		ret=iso_get(&resp,1,VAG_CHAR_TIMEOUT);
		if (resp!=~temp)
		{
			printf("\n\rERROR PC->%X ECM->%x\n\r",temp,resp);
		}
		//printf("ECM->%x\n\r",resp);
	}
	temp=0x03;
	printf(" %x\n\r",temp);
	ret=iso_put(&temp,1,VAG_CHAR_DELAY);
	return len;
}

					
