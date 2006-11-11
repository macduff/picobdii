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
 *  DATE : 9/26/2004  12:01:15 PM
 *
 *  FILENAME: firmware.c
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
#include "firmware.h"
   
/*******************************************************************
	NOTE:
The firmware update requires that the flash read routines and the 
FirmwareUpdate be stored in Flash that is not erased.. 
*******************************************************************/

/*******************************************************************
 *  FUNCTION: FirmwareCheck
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	firmware.c	
 *  DATE		=   9/26/2004  12:01:40 PM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: Checks for valid version of firmware
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
UINT FirmwareCheck(UDWORD flashAddr)
{
	//The firmware file in flash has a header 
    //read header and see if vaild. 
    // then check version, if flash has newer version
    // Check the checksum. If all good return 0
    // on error return non-zero
    
	FIRM_HEADER header; 
	UDWORD addr;
	UDWORD check;
	UDWORD i;
	UDWORD temp; 

	
	//read header
	flashRead((UBYTE *)&header, sizeof(FIRM_HEADER), flashAddr);
	
	if (header.type!=FIRM_TYPE)
		return 1;

	if (header.verMajor<VER_MAJOR)
		return 2;

	if (header.verMajor==VER_MAJOR && header.verMinor<=VER_MINOR)
		return 3;

	//now we need to check the check sum. 
    addr=flashAddr+sizeof(FIRM_HEADER);
    
	check=0;
	for(i=0; i<(header.size/sizeof(UDWORD)); i++)
	{
		flashRead((UBYTE *)&temp, sizeof(UDWORD), addr);
		addr+=sizeof(UDWORD);
		check=check+temp;
	}

	printf("check %lx %lx\n\r",check, header.checkSum);
	if(check!=header.checkSum)
		return 4;

	//OK if we got here we have vaild data
	return 0;
}

/*******************************************************************
 *  FUNCTION: FirmwareUpdate
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	firmware.c	
 *  DATE		=   9/26/2004  12:19:19 PM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: Updates Firmware
 *	 
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/

#org LOADER_ADDR
UBYTE PgmErase(UDWORD address)
{

	TBLPTRU=(UBYTE)(address>>16);
	TBLPTRH=(UBYTE)(address>>8);
	TBLPTRL=(UBYTE)(address);
#asm 
	BSF EECON1, 7 //EEPGD //point to Flash program memory
	BCF EECON1, 6//CFGS //access Flash program memory
	BSF EECON1, 2//WREN // enable write to memory
	BSF EECON1, 4//FREE // enable Row Erase operation
	MOVLW 0x55
	MOVWF EECON2 // write 55h
	MOVLW 0xAA
    MOVWF EECON2 // write 0AAh
	BSF EECON1, 1 //WR //start erase (CPU stall)
	NOP
	BCF   EECON1, 2//WREN
#endasm
	return 0;
}

#org LOADER_ADDR
UBYTE PgmFlash(UDWORD address, UBYTE *data) //8 bytes
{
	UBYTE i;

	TBLPTRU=(UBYTE)(address>>16);
	TBLPTRH=(UBYTE)(address>>8);
	TBLPTRL=(UBYTE)(address);

#asm
		TBLRD*-  //dummy read to decrement
#endasm

	for(i=0; i<8; i++)
	{
		//printf("%x ",data[i]);

		TABLAT=data[i];
#asm
		TBLWT+*
#endasm
	}
	TBLPTRU=(UBYTE)(address>>16);
	TBLPTRH=(UBYTE)(address>>8);
	TBLPTRL=(UBYTE)(address);

	disable_interrupts(GLOBAL);

#asm 
	BCF EECON1, 4//FREE // disable Row Erase operation
	BSF EECON1, 7 //EEPGD //point to Flash program memory
	BCF EECON1, 6//CFGS //access Flash program memory
	BSF EECON1, 2//WREN // enable write to memory
	MOVLW 0x55
	MOVWF EECON2 // write 55h
	MOVLW 0xAA
    MOVWF EECON2 // write 0AAh
	BSF EECON1, 1 //WR //start erase (CPU stall)
	NOP
	BCF   EECON1, 2//WREN
#endasm
	

	return 0;
}
	
#org LOADER_ADDR
UBYTE FirmwareUpdate(UDWORD flashAddr)
{
	UBYTE ret;
	UBYTE i; //number of bytes
	UDWORD address; //start where to write data
	UDWORD check;
	UDWORD in[2];
	UDWORD out[2];
	UDWORD keys[4]={0,111,11,1111};
	FIRM_HEADER header; 

	UDWORD addr; 
	
	disable_interrupts(GLOBAL);
	ret=FirmwareCheck(flashAddr);
	if (ret)
	{
		enable_interrupts(GLOBAL);
		return ret;
	}
	
	//OK we have vaild firmware... 
	//Now to keep things simple we are going to have a define 
	//form of the data. That is the firmware is stored in 
	//a structure. 
	//   UDWORD address; //start where to write data
	//   UDWORD check;  //check sum. 
	//   UBYTE data[64]; //data
	//	 
	//
	// since our encryption algorithm operates on 64 bits we will
	// make sure each structure is multiple of 8 bytes
	// We will also limit address to modulo 64 that is the last 6 bits
	// must be zero. 
	flashRead((UBYTE *)&header, sizeof(FIRM_HEADER), flashAddr);
	addr=flashAddr+sizeof(FIRM_HEADER);

	printf("Updating Firmware %d.%02d\n\r",header.verMajor,header.verMinor);
	address=0;
   //the first step is to read the data
	flashRead((UBYTE *)&in[0],sizeof(UDWORD),addr);
	addr+=sizeof(UDWORD);
	flashRead((UBYTE *)&in[1],sizeof(UDWORD),addr);
	addr+=sizeof(UDWORD); 
	//now we should decrypt. 
	decrypt(in,out,keys);
	address=out[0];
	check=out[1];

	while(address<(UDWORD)LOADER_ADDR)
	{					
		//erase that area of Flash
		//
		PgmErase(address);

		//now lets read the data
		for(i=0; i<8; i++)
		{
			//read 8 bytes at a time
			flashRead((UBYTE *)&in[0],sizeof(UDWORD),addr);
			addr+=sizeof(UDWORD);
			flashRead((UBYTE *)&in[1],sizeof(UDWORD),addr);
			addr+=sizeof(UDWORD);
			decrypt(in,out,keys);

			//next we can program the data to flash
			/*
			data=(UBYTE *)&out[0];
			printf("%lx ",address);
			for(n=0; n<8; n++)
			{
				printf("%x ",data[n]);
			}
			printf("\n\r");
			*/
			PgmFlash(address,(UBYTE *)&out[0]);
			address+=8;
			
		}
		//the first step is to read the data
		flashRead((UBYTE *)&in[0],sizeof(UDWORD),addr);
		addr+=sizeof(UDWORD);
		flashRead((UBYTE *)&in[1],sizeof(UDWORD),addr);
		addr+=sizeof(UDWORD); 
		//now we should decrypt. 
		decrypt(in,out,keys);
		address=out[0];
		check=out[1];
		//while(PIR1bits.TXIF==0); 
	    TXREG='A';	
	}

#asm
				RESET
#endasm

	enable_interrupts(GLOBAL);
	return 0;
}
#org default
