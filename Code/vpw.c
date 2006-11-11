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

#include "vpw.h"

UBYTE vpw_send(UBYTE *resp, UBYTE len, UBYTE *msg, UBYTE msg_len)
{
	UBYTE ret,i;
	UBYTE message[20];

	if (msg_len>6)
	{
		printf("Error: VPW message allows only 7 bytes\n\r");
		return 0;
	}
 	//OK we need to build the message
	message[0]=0x68;
	message[1]=0x6A;
	message[2]=0xF1;

	for(i=0; i<msg_len; i++)
	{
		message[3+i]=msg[i];
	}

	i=3+i;
	message[i]=crc(message,i);
	i++;
 /*
	printf("Sending:\n\r");
	for(ret=0; ret<i; ret++)
	{
		printf("%X ",message[ret]);
	}
	printf("\n\r");
 */
	//now we can send message
  	ret=vpw_put(message,i,3000);
	if(ret!=i)
	{
		printf("Error: VPW message sending problems ret=%u\n\r",ret);
		return 0;
	}
	//now lets get response
	ret=vpw_get(resp,len,3000);
/*
	printf("VPW_return %d\n\r",ret);
	for(i=0; i<ret; i++)
	{
		printf("%X ",resp[i]);
	}
	printf("\n\r");
*/
	return ret;
}


////////////////////////////////////////////////
// Simple Diagnostic function for RS232
//////////////////////////////////////////////
UINT vpw_montior()
{
	//we will recevice some data from rx channel
	UBYTE data[20];
	UBYTE len;
	UBYTE i;

	disable_interrupts(GLOBAL);
	len=vpw_get(data,20,15000);
	enable_interrupts(GLOBAL);
	if (len==0)
	{
		printf("VPW time out");
		return 1;
	}
	
	for(i=0; i<len; i++)
	{
		printf("%X ",data[i]);
	}
	return 0;
}
	


////////////////////////////////////////////////////////////
// VPW Section
///////////////////////////////////////////////////////////

UBYTE vpw_wait_idle(UWORD time_out_ms)
{
	UBYTE done;
	UWORD big_timer;
	UBYTE count;
	UWORD ms_timer;
	UBYTE level;

	done=0;
	TMR0=0;
	big_timer=0;
	while(!done)
	{
		//Find current level
		level=VPW_IN;			
		//Wait for level to change
		while(VPW_IN==level)
		{
			//keep track of big_timer and our time out
			if ((UBYTE)TMR0>=250)
			{
				count++;
				big_timer=big_timer+TMR0;
				TMR0=0;
				if (count>=5)
				{
					ms_timer++;
					count=0;
					if (ms_timer>time_out_ms)
					{
						return 0;
					}
				}
			}
		}
		big_timer=big_timer+TMR0;
		TMR0=0;
		if (big_timer>=TV4_MIN && level==0)
			return 1;
		big_timer=0;
	}	
	
}

	



//we need a function to read the VPW data
UBYTE vpw_get(UBYTE *data, UBYTE len, UWORD time_out_ms)
{
	UBYTE i;
	UBYTE level;
	UBYTE done2;
	UWORD temp;
	UBYTE state;
	UBYTE bits;
	UBYTE vpw_data;
	UWORD ms_timer;
	UBYTE count;
		
		
	i=0;
	done2=0;
  	TMR0=0;
	level=0;
	bits=0;
	state=VPW_UNKNOWN;

	//wait for bus to go inactive
	ms_timer=0;
	count=0;

vpw_wait:
	while(VPW_IN==1)
	{
		if (TMR0>250)
		{
			TMR0=0;
			count++;
			//printf("bus active\n\r");
			if (count>=5)
			{
				ms_timer++;
				count=0;
				if (ms_timer>time_out_ms)
				{
					printf("ERROR: VPW timed out waiting for idle bus\n\r");
					//enable_interrupts(GLOBAL);
					return 0;	 //we timed out
				}
			}
		}
	}

	disable_interrupts(GLOBAL);
	//first we must wait for buss to go high
	count=0;
	while(VPW_IN==0)
	{
		if (TMR0>250)
		{
			TMR0=0;
			count++;
			if (count>=5)
			{
				ms_timer++;
				count=0;
				if (ms_timer>time_out_ms)
				{
					printf("ERROR: VPW timed out waiting for SOH\n\r");
					enable_interrupts(GLOBAL);
					return 0;	 //we timed out
				}
			}
		}
	}
	count=TMR0;		
	TMR0=0;

	
	//next we should get a SOH pulse
	temp=0;
	while(VPW_IN==1)
	{
		if (TMR0>250)
		{
			temp=temp+TMR0;
			TMR0=0;
			count++;
			if (count>=5)
			{
				ms_timer++;
				count=0;
				if (ms_timer>time_out_ms)
				{
					printf("ERROR: VPW timed out waiting for SOH\n\r");
					enable_interrupts(GLOBAL);
					return 0;	 //we timed out
				}
			}

		}
	}
	temp=temp+TMR0;
	TMR0=0;
	

	if (temp<TV3_MIN || temp>TV3_MAX)
	{
		//printf("ERROR: VPW SOH ill formed %lu %u %lu\n\r",ms_timer,count,temp);
		//ms_timer++;
		goto vpw_wait;
		enable_interrupts(GLOBAL);

		return 0;
	}
	
	//now we should be clocking data
	while(i<len)
	{
		temp=0;
		count=0;
		level=VPW_IN;
		ms_timer=0;
		while(VPW_IN==level && VPW_IN==level)
		{
			if (TMR0>250)
			{
				temp=temp+TMR0;
				TMR0=0;
				count++;
				if(count>=5)
				{
					ms_timer++;
					count=0;
					if (ms_timer>55)
					{
						//printf("ERROR: VPW timed out in data %lu\n\r",temp);
						enable_interrupts(GLOBAL);
						return i;
					}
				}
			}
		}
		temp=temp+TMR0;
		TMR0=0;
		if(temp>TV3_MAX)
		{
			enable_interrupts(GLOBAL);
			return i;
		}

		if (temp>VPW_THRESHOLD)
		{
			//we have long pulse			
			vpw_data=(vpw_data<<1) | (level==0);	
			bits++;
		}else
		{
			//short pulse
			vpw_data=(vpw_data<<1) | level;	
			bits++;
		}

		if (bits==8)
		{
			data[i]=vpw_data;
			i++;
			bits=0;
			vpw_data=0;
		}
	}
	enable_interrupts(GLOBAL);
	return i;
}

UBYTE vpw_put(UBYTE *data, UBYTE len, UWORD time_out_ms)
{
	UBYTE index;
	UBYTE mask;
	UBYTE temp;
	UBYTE level;
	UBYTE tcount;
	UWORD ms_timer;
	UWORD us_timer;
	UBYTE state;
	UBYTE thisbit; 

	index=0;
	output_low(VPW_OUT);


	//first we have to wait for bus to go passive
	ms_timer=0;
	state=VPW_UNKNOWN;

	while (state==VPW_UNKNOWN)
	{
		tcount=0;
	  	while(VPW_IN==1)
		{
			if (TMR0>=250)
			{
				TMR0=0;
				tcount++;
				if (tcount>=5)
				{
					tcount=0;
					ms_timer++;
					if (ms_timer>time_out_ms)
					{	
						return 100;//index;
					}
				}
			}
		}

		//bus is in a low state, make sure it is low for idle time
		TMR0=0;
		us_timer=0;
		while(us_timer<TV6 && VPW_IN==0)
		{
			//if (VPW_IN==1)
			 //	break; //return to waiting for buss inactive	
			if (TMR0>=250)
			{
				us_timer=us_timer+TMR0;
			}
		}
		us_timer=us_timer+TMR0;
		if (us_timer>=TV6)
			state=VPW_IDLE; //we are now idle
	}
	
						
	//temp=data[index];
	mask=0x80;  
	disable_interrupts(GLOBAL); 
	thisbit=temp & mask;
	//now we need to drive the bus with an active SOF
	output_high(VPW_OUT);
	TMR0=0;
	while(TMR0<(TV3>>1));
	TMR0=0;
	while(TMR0<(TV3>>1));

	output_low(VPW_OUT);
	TMR0=0;
	level=0;
		
	//clock out each bit	
	while (index<len)
	{	 
	    temp=data[index];
		if (temp & mask)
		{
			if (level)
			{
				level=0;
				while(TMR0<(TV1));	
				output_low(VPW_OUT);
				TMR0=0;
			}else
			{
				level=1;
				while(TMR0<(TV2+22));	
				output_high(VPW_OUT);
				TMR0=0;
			}
		}else
		{
			if (level)
			{
				level=0;
				while(TMR0<TV2);	
				output_low(VPW_OUT);
				TMR0=0;
			}else
			{
				level=1;
				while(TMR0<(TV1+20));	
				output_high(VPW_OUT);
				TMR0=0;
			}
		}		
	   	mask=mask>>1;
 		if (mask==0)
		{
			mask=0x80;
			index++;
		}

	} 
	enable_interrupts(GLOBAL);
	return index;
}

