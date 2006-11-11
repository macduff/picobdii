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

#include "command.h"
#include <string.h>

#include "vpw.h"
#include "codes.h"
#include "vag.h"
#include "firmware.h" //for address to flash firmware

//typedef UINT (*PtrComdFunc)(UINT , CHAR *[]);

UINT CommandParse(CHAR *str);

//DAMN CSS Compiler does not support pointer to functions
//Therefore we have to be more "creative" 


UINT CommandExit=0;

/* 
PtrComdFunc ptrCommands[]={

};  
*/


UBYTE ascii2hex(UBYTE *str)
{
 	UBYTE j;
	UBYTE temp,t2,shift;
	temp=0;
	for (j=0; j<2; j++)
	{
		t2=str[j];
		shift=0; 

		if (j==0)
			shift=4;

		if (t2>='a')
		{
			temp=temp | ((t2-'a'+10)<<shift);
		}else
		if (t2>='A')
		{
			temp=temp | ((t2-'A'+10)<<shift);
		}else
		if (t2>='0')
		{
			temp=temp | ((t2-'0')<<shift);
		}
	}
	return temp;
}	
/*******************************************************************
 *  FUNCTION: CommandBegin
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	command.c	
 *  DATE		=   1/25/2004  3:43:22 PM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: Starts up the serial command line 
 *
 *  RETURNS: 
 *
 *
 *******************************************************************/
UINT CommandBegin()
{
	UBYTE data;
	CHAR buffer[MAX_STRING];
	UBYTE buffIndex;

	//first clear the screen and push command prompt
	CLEAR_SCREEN();
	printf(CMD_PROMPT);

	//now spin reading command and waiting on CR
	CommandExit=0;
	buffIndex=0;
	buffer[0]=0; 
	while(!CommandExit)
	{
		if(kbhit())
		{
			data=getch2();
				
			//echo the data
			putch(data);
		

			//if the data is the CR we need to process buffer
			if (data==0x0D)
			{	
				putch(0x0A);
				if (strlen(buffer)>0)
					CommandParse(buffer);
				printf("\n\r");
				printf(CMD_PROMPT);
				buffIndex=0;
				buffer[buffIndex]=0;
			} 
		


			if (data != 0x0A && data !=0x0D)
			{
				buffer[buffIndex++]=data;
				buffer[buffIndex]=0;
			}
			if (buffIndex>=(MAX_STRING-1))
			{
				printf("\n\rERROR: Command buffer overflow\n\r");
				buffIndex=0;
				buffer[0]=0;
				printf(CMD_PROMPT);
			}
		}
	}
	return 0;
}

/*******************************************************************
 *  FUNCTION: CommandParse
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	command.c	
 *  DATE		=   1/25/2004  4:03:03 PM
 *
 *  PARAMETERS: Takes the command line string	
 *
 *  DESCRIPTION: parses the command line and returns Command ID
 *
 *  RETURNS: Command ID, and 
 *
 *
 *******************************************************************/
UINT CommandParse(CHAR *str)
{
	CHAR *ptr;
	CHAR *ptr2;
	UINT i;
	CHAR cmd[MAX_STRING];
	CHAR buff[MAX_CMD_LENGTH];
	CHAR argv[MAX_ARGS][MAX_ARG_LENGTH];
	//CHAR *ptrArgv[MAX_ARGS];
	UINT numArgs;

	//first we need find command and arguments	
	ptr=strchr(str,' '); //find first char

	if (ptr==0)
	{
		//we have two options, frist whole thing is command
		//second bad command
		if(strlen(str)>0)
			ptr=str+strlen(str); 
		else
			return 0; //bad command
	}

	//copy string to command buffer. 
	i=0;
	ptr2=str; 
	while(ptr!=0 && ptr!=ptr2 && i<(MAX_CMD_LENGTH-1))
	{
		buff[i++]=*ptr2++; 
	}
	buff[i]=0;

	//now buff contains the command let's get the args
	numArgs=0;
	while(*ptr!=0 && *ptr==' ')
		ptr++; //increment pointer past ' '
	if (*ptr!=0)
	{ 	
		ptr2=strchr(ptr,' ');
		if (ptr2==0)
		{
			//we have two options, frist whole thing is command
			//second bad command
			if(strlen(ptr)>0)
				ptr2=ptr+strlen(ptr); 
		}
		while(ptr2!=0 && numArgs<MAX_ARGS)
		{
			int j;
			j=0;
			while (ptr2!=ptr && j<(MAX_ARG_LENGTH-1))
			{
				argv[numArgs][j++]=*ptr++;
			}
			argv[numArgs][j++]=0;
			numArgs++;
			ptr2=0;
			if (*ptr!=0)
			{
				while(*ptr!=0 && *ptr==' ')
					ptr++; //increment pointer past ' '
				ptr2=strchr(ptr,' ');
				if (ptr2==0)
				{
					//we have two options, frist whole thing is command
					//second bad command
					if(strlen(ptr)>0)
						ptr2=ptr+strlen(ptr); 
				}
			}
		}
	}
/*
	for(i=0; i<MAX_ARGS; i++)
	{
		ptrArgv[i]=argv[i];
	}
	
	//now let's parse the command
	for(i=1; i<NUM_COMMANDS; i++)
	{
		if (strcmp((const char *)buff,(const char *)strCommands[i])==0)
			return (*ptrCommands[i])(numArgs,ptrArgv);
	}
*/
	sprintf(cmd,"exit");
	if (strcmp(buff,cmd)==0)
	{
		CommandExit=1;
	}

	sprintf(cmd,"ver");
	if (strcmp(buff,cmd)==0)
	{
	   printf("software v%d.%02d",VER_MAJOR,VER_MINOR); 
	}	
	sprintf(cmd,"hwver");
	if (strcmp(buff,cmd)==0)
	{
	   printf("hardware v%d",HW_VER); 
	}	
	sprintf(cmd,"logb");
	if (strcmp(buff,cmd)==0)
	{
		datalog_bin();

	}	
	sprintf(cmd,"vpwmon");
	if (strcmp(buff,cmd)==0)
	{
	   vpw_montior(); 
	}	
	sprintf(cmd,"log");
	if (strcmp(buff,cmd)==0)
	{
		datalog();
	}
	sprintf(cmd,"pwmhigh");
	if (strcmp(buff,cmd)==0)
	{
		output_high(PIN_F5);
	} 

	sprintf(cmd,"pwmlow");
	if (strcmp(buff,cmd)==0)
	{
		output_low(PIN_F5);
	}	

	sprintf(cmd,"pwmmon");
	if (strcmp(buff,cmd)==0)
	{
		pwm_monitor();
	}
	sprintf(cmd,"pwmread");
	if (strcmp(buff,cmd)==0)
	{
		while(!kbhit())
		{
			printf("PWM bus=%d\n\r",PWM_IN);
		}
		getch2();
	}	
		 
	sprintf(cmd,"5baud");
	if (strcmp(buff,cmd)==0)
	{
	    UBYTE keys[2];
		printf("%lu ",iso_5baud(keys,2,0x33));
		printf("%X %X\n\r",keys[0],keys[1]);
	}
   		sprintf(cmd,"crc");
	if (strcmp(buff,cmd)==0)
	{
		UBYTE data[20];
		UBYTE temp;
		for (i=0; i<numArgs; i++)
		{
			//we need to parse ASCII
			temp=ascii2hex(argv[i]);
			data[i]=temp;
		}
		for (temp=0; temp<numArgs; temp++)
		{
			printf("%X ",data[temp]);
		}
		printf("\n\r");
		fprintf(STDIN,"CRC %X\n\r",crc(data,numArgs));
	}

	sprintf(cmd,"csum");
	if (strcmp(buff,cmd)==0)
	{
		UBYTE data[20];
		UBYTE temp;
		for (i=0; i<numArgs; i++)
		{
			//we need to parse ASCII
			temp=ascii2hex(argv[i]);
			data[i]=temp;
		}
		for (temp=0; temp<numArgs; temp++)
		{
			printf("%X ",data[temp]);
		}
		printf("\n\r");
		fprintf(STDIN,"CheckSum %X\n\r",checksum(data,numArgs));
	}
	
	sprintf(cmd, "vagcom");
	if (strcmp(buff,cmd)==0)
	{
		//we need to monitor the serial port
		// when port goes high drive k-line accordingly
	}

	sprintf(cmd, "vag");
	if (strcmp(buff,cmd)==0)
	{
		UBYTE data[60];
		UBYTE resp[20];
		UBYTE ret;
		UBYTE i;
		UBYTE block;

		//first lets init communications
		ret=iso_5baud(data,2,0x01);
  
		data[2]=0;

		while(data[2]!=0x09)
		{
		    //get block from ECM
		    ret=vagGetBlock(data,60);
			for(i=0; i<ret; i++)
			{
				printf("%X ",data[i]);
			}
			printf("\n\r");
		    //now build ack
			block=data[1]+1;
		    resp[0]=data[1]+1;
		    resp[1]=0x09;
		    vagPutBlock(resp,2);
			block++;
		}
		ret=vagGetBlock(data,60);
		for(i=0; i<ret; i++)
		{
			printf("%X ",data[i]);
		}
		printf("\n\r");
		block=data[1]+1;

		printf("Test block ");	
		for(i=0; i<numArgs; i++)
		{
			resp[i+1]=ascii2hex(argv[i]);
			printf("%X ",resp[i+1]);
		}	
		printf("\n\r");

		//now lets see what happens if we send a block request for block 02
		resp[0]=block;
		//resp[1]=code;
		vagPutBlock(resp,numArgs+1);
		data[2]=0;

		while(data[2]!=0x09)
		{
		    //get block from ECM
		    ret=vagGetBlock(data,60);
			for(i=0; i<ret; i++)
			{
				printf("%X ",data[i]);
			}
			printf("\n\r");
		    //now build ack
			block=data[1]+1;
		    resp[0]=data[1]+1;
		    resp[1]=0x09;
		    vagPutBlock(resp,2);
			block++;
		}



	 }

	sprintf(cmd,"isosend");
	if (strcmp(buff,cmd)==0)
	{
	    UBYTE data[60];
		UBYTE resp[20];
		UBYTE i;
		UBYTE temp;
		
		printf("hit key to stop\n\r");
		//we need to copy args to data
		for (i=0; i<numArgs; i++)
		{
			//we need to parse ASCII
			temp=ascii2hex(argv[i]);
			data[i]=temp;
		}
		//printf("\n\r");
		while(!kbhit())
		{
			temp=iso_send(resp, 60, data, numArgs,0x33);	
			printf("response %u\n\r",temp); 
			for (i=0; i<temp; i++)
			{
				printf("%X ",resp[i]);
			}
			printf("\n\r");	
		}
		while(kbhit())
			getch2();		
	}
	

	sprintf(cmd,"pwmsend");
	if (strcmp(buff,cmd)==0)
	{
	    UBYTE data[60];
		UBYTE resp[20];
		UBYTE i;
		UBYTE temp;
		
		printf("hit key to stop\n\r");
		//we need to copy args to data
		for (i=0; i<numArgs; i++)
		{
			//we need to parse ASCII
			temp=ascii2hex(argv[i]);
			data[i]=temp;
			//printf("args %x %s\n\r",temp,argv[i]);
		}
		//printf("\n\r");
		while(!kbhit())
		{
			temp=pwm_send(resp, 60, data, numArgs);	
			printf("response %u\n\r",temp); 
			for (i=0; i<temp; i++)
			{
				printf("%X ",resp[i]);
			}
			printf("\n\r");	
		}
		while(kbhit())
			getch2();		
	}
/*
	sprintf(cmd,"flashread");
	if (strcmp(buff,cmd)==0)
	{
		UDWORD addr;
		
		addr=atoi32(argv[0]);
		printf("*%lu=%X\n\r",addr,flash_read(addr));	    		
	}

	sprintf(cmd,"flashbufread");
	if (strcmp(buff,cmd)==0)
	{
		UDWORD addr;
		
		addr=atoi32(argv[0]);
		printf("*%lu=%X\n\r",addr,flash_buf_read(addr));	    		
	}
*/
	sprintf(cmd,"flasherase");
	if (strcmp(buff,cmd)==0)
	{
		flash_erase();
		printf("Done\n\r");    		
	}

/*
	sprintf(cmd,"flashput");
	if (strcmp(buff,cmd)==0)
	{
		UDWORD addr;
		UBYTE data;
		
		addr=atoi32(argv[0]);
		data=atoi(argv[1]);
		printf("Putting *%lu=%X\n\r",addr,data);
		data=flash_put(addr,data);
		printf("returned %x\n\r",data);	    		
	}
*/

	sprintf(cmd,"vpwsend");
	if (strcmp(buff,cmd)==0)
	{
	    UBYTE data[60];
		UBYTE resp[20];
		UBYTE i;
		UBYTE temp;
		
		printf("hit key to stop\n\r");
		//we need to copy args to data
		for (i=0; i<numArgs; i++)
		{
			//we need to parse ASCII
			temp=ascii2hex(argv[i]);
			data[i]=temp;
		}
		//printf("\n\r");
		while(!kbhit())
		{
			temp=vpw_send(resp, 60, data, numArgs);	
			printf("response %u\n\r",temp); 
			for (i=0; i<temp; i++)
			{
				printf("%X ",resp[i]);
			}
			printf("\n\r");	
		}
		while(kbhit())
			getch2();		
	}

	sprintf(cmd,"isomon");
	if (strcmp(buff,cmd)==0)
	{
		while (!kbhit())
		   iso_monitor(); 
			
		while(kbhit())
			getch2();

	}

	sprintf(cmd,"isoput");
	if (strcmp(buff,cmd)==0)
	{
		UBYTE temp;
		printf("ISO Put test\n\r");
		printf("press any key to stop\n\r");
		temp=0x03;
		while (!kbhit())
			iso_put(&temp,1,20);  
			
		while(kbhit())
			getch2();
	}

	sprintf(cmd,"vpwhigh");
	if (strcmp(buff,cmd)==0)
	{
		UBYTE temp;
		printf("VPW high test\n\r");
		printf("press any key to stop\n\r");
		temp=0x03;
		output_high(VPW_OUT);
		while (!kbhit()){
			printf("VPW_IN=%u\n\r",VPW_IN);
		}	  
			
		while(kbhit())
			getch2();
		output_low(VPW_OUT);

	}

	sprintf(cmd,"isohigh");
	if (strcmp(buff,cmd)==0)
	{
		UBYTE temp;
		printf("ISO high test\n\r");
		printf("press any key to stop\n\r");
		temp=0x03;
		K_HIGH;
		while (!kbhit()){
			printf("ISO_IN=%u\n\r",K_IN);
		}	  
			
		while(kbhit())
			getch2();
		K_LOW;

	}

	sprintf(cmd,"isolow");
	if (strcmp(buff,cmd)==0)
	{
		UBYTE temp;
		printf("ISO low test\n\r");
		printf("press any key to stop\n\r");
		temp=0x03;
		K_LOW;
		while (!kbhit()){
			printf("ISO_IN=%u\n\r",K_IN);
		}	  
			
		while(kbhit())
			getch2();
		K_HIGH;

	}



	sprintf(cmd,"vpwlow");
	if (strcmp(buff,cmd)==0)
	{
		UBYTE temp;
		printf("VPW low test\n\r");
		printf("press any key to stop\n\r");
		temp=0x03;
		output_low(VPW_OUT);
		while (!kbhit()){
			printf("VPW_IN=%u\n\r",VPW_IN);
		}	  
			
		while(kbhit())
			getch2();
		output_low(VPW_OUT);

	}

	sprintf(cmd,"flashpgm");
	if (strcmp(buff,cmd)==0)
	{
		//UBYTE ret;
		printf("Start Xmodem download to unit of flash");
		delay_ms(250);
		Flash_serial_program(0);
	}
	 
	sprintf(cmd,"flashpcodes");
	if (strcmp(buff,cmd)==0)
	{
		//UBYTE ret;
		printf("Start Xmodem download to unit of P codes");
		delay_ms(250);
		Flash_serial_program(PCODE_FLASH_START);
	}

	sprintf(cmd,"flashccodes");
	if (strcmp(buff,cmd)==0)
	{
		//UBYTE ret;
		printf("Start Xmodem download to unit of C codes");
		delay_ms(250);
		Flash_serial_program(CCODE_FLASH_START);
	}
	sprintf(cmd,"flashbcodes");
	if (strcmp(buff,cmd)==0)
	{
		//UBYTE ret;
		printf("Start Xmodem download to unit of B codes");
		delay_ms(250);
		Flash_serial_program(BCODE_FLASH_START);
	}
	sprintf(cmd,"flashucodes");
	if (strcmp(buff,cmd)==0)
	{
		//UBYTE ret;
		printf("Start Xmodem download to unit of U codes");
		delay_ms(250);
		Flash_serial_program(UCODE_FLASH_START);
	}

	sprintf(cmd,"flashfirm");
	if (strcmp(buff,cmd)==0)
	{
		UWORD i;
		printf("Erasing Firmware\n\r");
		for(i=(FIRM_FLASH_START>>8); i<(FLASH_MAX_PAGES>>8); i++)
		{
			//sprintf(temp,"%lu",i);
			//LCD_print2(temp,2);
			//printf("Erasing Page %lu\n\r",i);
			if (flash_erase_page(i)!=0)
				return 1;
		}

		printf("Start Xmodem download to unit of Firmware");
		delay_ms(250);
		Flash_serial_program(FIRM_FLASH_START);
		delay_ms(250);
		printf("Checking Firmware\n\r");

		if (FirmwareCheck(FIRM_FLASH_START)==0)
		{
			printf("Updating Firmware\n\r");
			FirmwareUpdate(FIRM_FLASH_START);
		}else
		{
			printf("Firmware not correct, try reloading\n\r");
		}

	}
	sprintf(cmd,"isobaud");
	if (strcmp(buff,cmd)==0)
	{
		printf("bps=%ld\n\r",iso_Baud());
	}


	sprintf(cmd,"firmcheck");
	if (strcmp(buff,cmd)==0)
	{
		printf("Checking Firmware\n\r");

		if (FirmwareCheck(FIRM_FLASH_START)==0)
		{
			printf("Updating Firmware\n\r");
			FirmwareUpdate(FIRM_FLASH_START);
		}else
		{
			printf("Firmware not correct, try reloading\n\r");
		}

	}


/*
	sprintf(cmd,"printcode");
	if (strcmp(buff,cmd)==0)
	{
		UDWORD code;
		
		code=atoi32(argv[0]);
		PcodePrint(code);
	}
*/
	sprintf(cmd,"exit");
	if (strcmp(buff,cmd)==0)
	{
		return 0;
	}
/*
	sprintf(cmd,"codes");
	if (strcmp(buff,cmd)==0)
	{
		obdII_codes();
	}
	sprintf(cmd,"clearcodes");
	if (strcmp(buff,cmd)==0)
	{
		obdII_clear_codes();
	}
	
	sprintf(cmd,"knob");
	if (strcmp(buff,cmd)==0)
	{
		printf("Knob is %d\n\r",knob);
	}
*/   
   

	return 0; //unknown command
}	

