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

#include "serial.h"


UBYTE rx_buf[SERIAL_BUF];
UBYTE rx_put_index=0;
UBYTE rx_get_index=0;


UBYTE rs232_init(UDWORD baud)
{
	UDWORD brg;

	//disable interrupt
	bit_clear(PIE1,5); //set RCIE

	brg=((UDWORD)FOSC/baud)/4-1;
	TXSTA=0x26;
	BAUDCON=0x08;
	SPBRGH=(brg>>8) & 0x00FF;
	SPBRG=brg & 0x00FF;		
	RCSTA=0x90;

	//enable interupts?
	bit_set(PIE1,5); //set RCIE
	return 0;
}

#int_rda
void rx_isr()
{
	//check for error
	if (RCSTA & 0x02)
	{
		//we may get an overflow when ISR disabled
		//printf("RS232 error %X\n\r",RCSTA);
		RCSTA=0x80;
		RCSTA=0x90;
		//rx_buf[rx_put_index]=RCREG;
		bit_clear(PIR1,5);
		return;
	}
	//we have some data in a buffer
	rx_buf[rx_put_index++]=RCREG;
	if (rx_put_index>=SERIAL_BUF)
	{
		rx_put_index=0;
	}
	//clear the IF
	bit_clear(PIR1,5);
}

UBYTE kbhit2()
{
	//printf("Put %d get %d\n\r",rx_put_index, rx_get_index);
	return (rx_put_index!=rx_get_index);
}

UBYTE getch2()
{
	UBYTE temp;

	while(!kbhit2());   //wait for char available. 
   	
   	temp=rx_buf[rx_get_index++];
   	if (rx_get_index>=SERIAL_BUF)
	{
		rx_get_index=0;
	}
	return temp;
}
	
UBYTE putch2(UBYTE data)
{
	//check bit 4 of PIR1 TXIF
	while(!bit_test(PIR1,4)); //wait for buffer empty
	TXREG=data;
	return 0;
}
