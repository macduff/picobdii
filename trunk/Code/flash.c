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

#include "flash.h"


//#define spi_write(x) flash_send(x)
//#define spi_read(x) flash_send(x)

//////////////////////////////////////////////////////////////////
//	EXTERNAL FLASH IF
/////////////////////////////////////////////////////////////////

/////////////////////////
// Flash_Init 
// Sets up flash I/F
///////////////////////

#define FLASH_DELAY 10

#separate
void flash_wait();

UBYTE flash_send(UBYTE data)
{
	UBYTE ret;	
	UBYTE mask;
	//assume CS is driven low
	ret=0;
	mask=0x80;
	while(mask)
	{
		ret=ret>>1;
		
		output_low(FLASH_CLK);
		delay_ms(FLASH_DELAY);
		output_bit(FLASH_SDO,((data & mask)>0));
		output_high(FLASH_CLK);
		if (input(FLASH_SDI))
			ret=ret | 0x80;
		delay_ms(FLASH_DELAY);
		mask=mask>>1;
	}
	return ret;
}
		
		

void flash_init()
{
	//set_tris_c(0xD7);
	output_high(FLASH_CS);
	output_high(FLASH_CLK);
	//setup the SPI port to master
	setup_spi(SPI_MASTER | SPI_H_TO_L | SPI_CLK_DIV_4 | SPI_SS_DISABLED);
	bit_clear(SSPCON1,5); //turn SPI off
	SSPCON1=0x30;
	SSPSTAT=0x00;
	
	//setup the CS pin for slave 
	//set_tris_f(TRIS_F);	//this is already done
	
	
}

//Erases all of flash memory
UBYTE flash_erase_page(UWORD page) 
{
	UWORD temp;

	//first send the erase opcode;
	output_low(FLASH_CS);
	spi_write(0x81);
	temp=page<<1;;
	spi_write(make8(temp,1));
	spi_write(make8(temp,0));
	spi_write(0x00);
	output_high(FLASH_CS);	

	delay_us(10);
	//now lets wait for the status to say all is good.
	flash_wait();
	return 0; //no error
}	

#org LOADER_ADDR, LOADER_END
UBYTE flash_read(UDWORD addr)
{
    UBYTE buff_addr;
	UDWORD page;
	
	//make address correct
	buff_addr=make8(addr,0);
	page=addr & FLASH_MAX_PAGES; 
	page=page<<1;
	
	//printf("addr %lX page %lX %X\n\r",addr,page,buff_addr);
	//send read command
	flash_wait();
	output_low(FLASH_CS);
	spi_write(0xD2);
	spi_write(make8(page,2));
	spi_write(make8(page,1));
	spi_write(buff_addr);
	//send 4 don't care bytes
	spi_write(0);
	spi_write(0);
	spi_write(0);
	spi_write(0);
	buff_addr=spi_read(0);
	output_high(FLASH_CS);
	return buff_addr;
}

#org LOADER_ADDR
void flash_wait()
{
	UBYTE status;
	UWORD timeout;

	output_low(FLASH_CS);
	spi_write(FLASH_RD_STATUS);
	status=spi_read(0);
	timeout=0;
	while ((status & 0x80)==0)
	{
		status=spi_read(0);
		timeout++;
		delay_us(2);
		if(timeout>10000)
		{
			printf("ERROR: flash time out %X\n\r",status);
			return;
		}
		//printf("status %X\n\r",status);
	} 
	output_high(FLASH_CS);
}
#org LOADER_ADDR
UBYTE flashRead(UBYTE *data, UBYTE count, UDWORD address)
{
	UBYTE i;
	UDWORD addr;
	addr=address;
	
	for(i=0; i<count; i++)
	{
		data[i]=flash_read(addr);
		addr++;
	}
	return i;
}

#org default
UBYTE flash_buf_read(UDWORD address)
{
	UBYTE buff_addr;
	UDWORD page;
	
	//make address correct
	buff_addr=make8(address,0);
	page=address & FLASH_MAX_PAGES; 
	page=page<<1;
	//addr=addr | buff_addr;
	
	//read page into buffer
	//transfer main memory into buffer #1
	output_low(FLASH_CS);
	spi_write(0xD4);
	spi_write(0);
	spi_write(0);
	spi_write(buff_addr);
	spi_write(0);
	buff_addr=spi_read(0);
	output_high(FLASH_CS);	
	return buff_addr;
}
	

UBYTE flash_put(UDWORD address, UBYTE data)
{
	UBYTE buff_addr;
	UDWORD page;
	
	//make address correct
	buff_addr=make8(address,0);
	page=address & FLASH_MAX_PAGES; 
	page=page<<1;
	//addr=addr | buff_addr;


	//read page into buffer
	//transfer main memory into buffer #1
	output_low(FLASH_CS);
	spi_write(0x53);
	spi_write(make8(page,2));
	spi_write(make8(page,1));
	spi_write(buff_addr);
	output_high(FLASH_CS);

    flash_wait();

	//write byte to buffer
	output_low(FLASH_CS);
	spi_write(0x84);
	spi_write(0);
	spi_write(0);
	spi_write(buff_addr);
	spi_write(data);
	output_high(FLASH_CS);
	delay_ms(1);
	flash_wait();

	//we need to write buffer to page
	output_low(FLASH_CS);
	spi_write(0x83);
	spi_write(make8(page,2));
	spi_write(make8(page,1));
	spi_write(make8(page,0));
	output_high(FLASH_CS);

	delay_ms(20);

	flash_wait();
  


	return 0;
}


//erase all of flash
UBYTE flash_erase()
{
	UWORD i;
//	UBYTE temp[20];
//	sprintf(temp,"Eraseing");
//	LCD_print1(temp,0);
	for(i=0; i<(FLASH_MAX_PAGES>>8); i++)
	{
		//sprintf(temp,"%lu",i);
		//LCD_print2(temp,2);
		printf("Erasing Page %lu\n\r",i);
		if (flash_erase_page(i)!=0)
			return 1;
	}
//	sprintf(temp,"DONE!");
//	LCD_print2(temp,0);
	return 0;
}

/*******************************************************************
 *  FUNCTION: Flash_serial_program
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	flash.c
 *  DATE		=   1/26/2003  5:17:39 PM
 *
 *  PARAMETERS: Destination address
 *
 *  DESCRIPTION: gets xmodem packet and programs to flash
 *
 *  RETURNS: 	0 - pass,
 *				1 - Flash write failed
 *				
 *
 *
 *******************************************************************/
UBYTE Flash_serial_program(UDWORD address)
{
    UWORD i;
    UBYTE data[XMODEM_PACKET_SIZE];
	UBYTE ret;
	UBYTE done;
	UBYTE packet;
	UBYTE buff_addr;
	UDWORD page;
	UDWORD bytes;

	packet=1;		//first packet is 1
    bytes=0;	 

	//make address correct
	buff_addr=make8(address,0);
	page=address & FLASH_MAX_PAGES; 
	page=page<<1;
	//addr=addr | buff_addr;


	//read page into buffer
	//transfer main memory into buffer #1
	output_low(FLASH_CS);
	spi_write(0x53);
	spi_write(make8(page,2));
	spi_write(make8(page,1));
	spi_write(buff_addr);
	output_high(FLASH_CS);
	flash_wait();

	//send request to start transimission
	// we will retry 40 times for slow users
	i=0;
	done=0;
	do {
		Xmodem_start();
		ret=Xmodem_get_packet(data,packet);	  //Get a packet of information	
	}while(ret==XERROR_SOH && i++<300);
	   		
	while(!done)
	{				
		UBYTE i;
		//then we need to program the data into flash
		for(i=0; i<XMODEM_PACKET_SIZE; i++)
		{
			//write each byte to buffer
			output_low(FLASH_CS);
			spi_write(0x84);
			spi_write(0);
			spi_write(0);
			spi_write(buff_addr);
			spi_write(data[i]);
			output_high(FLASH_CS);
			flash_wait();

 
			buff_addr++;
			//if buffer full write buffer to page 
			// and read next buffer
			if (buff_addr==0x00)
			{
				//we need to write buffer to page
				output_low(FLASH_CS);
				spi_write(0x83);
				spi_write(make8(page,2));
				spi_write(make8(page,1));
				spi_write(buff_addr);
				output_high(FLASH_CS);
				delay_ms(20);
				flash_wait();


				//read next page into buffer
				page=page+0x00200;
				output_low(FLASH_CS);
				spi_write(0x53);
				spi_write(make8(page,2));
				spi_write(make8(page,1));
				spi_write(buff_addr);
				output_high(FLASH_CS);
				flash_wait();

			}
			bytes++;
		}	
		packet++;
		//send request for next packet 
		if (ret==NO_ERRORS)
		{
			Xmodem_send_next();
			ret=Xmodem_get_packet(data,packet);	  //Get a packet of information
		}else 
		{
		  Xmodem_goodbye();	
		  done=1;
		}
			

	};

	//we need to write buffer to page
	output_low(FLASH_CS);
	spi_write(0x83);
	spi_write(make8(page,2));
	spi_write(make8(page,1));
	spi_write(buff_addr);
	output_high(FLASH_CS);
	delay_ms(20);
	flash_wait();

	if (ret==NO_ERRORS || ret==RECV_EOT)
	{
		Xmodem_goodbye();	
		Delay_ms(500);
		printf("\n\rFlashed %lu Bytes\n\r",bytes);
		printf("Flash Program completed\n\r");
		return NO_ERRORS;
	}
   	
	Xmodem_send_cancel();	//tell host to cancel
	Delay_ms(1000); //wait for host to give up
	while(kbhit())
		getch();
	printf("Flash Program ERROR %X packet=%d\n\r",ret,packet);
	//printf("Packet expect %d, got %d comp %d\n\r",expected,Xpacket,Xpacket_comp);
	printf("Press x to exit\n\r");
	while( getch()!='x'); 
   	return ret;				//else lets return the xmodem error
}
