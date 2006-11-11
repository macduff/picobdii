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

#include "iso.h"

//we only need state machine inside this file
#define ISO_UNKNOWN	0	//we are disconnected or unknown state
#define ISO_CONNECT 1	//we are connected and baud is correct rate

UBYTE iso_getAuto(UBYTE *data, UBYTE len, UWORD time_out_ms,UBYTE autoBaud );

UBYTE baud=0;
UBYTE iso_state=0;


//keep in mind my goal is not to make a full OBDII scanner
//just a simple "can my car be inspected"
//therefore some things are not as nice as should be

//lets attach header and CRC and send a message
//we will return length of response
UBYTE iso_send(UBYTE *resp, UBYTE len, UBYTE *msg, UBYTE msg_len, UBYTE address)
{
	UBYTE keys[2];
	UBYTE ret,i;
	UBYTE message[20];

	//printf("Connect timer %lu\n\r",connect_timer(0));
	if (connect_timer(0)>300)
		iso_state=ISO_UNKNOWN;
	if (msg_len>6)
	{
		printf("Error: ISO message allows only 7 bytes\n\r");
		return 0;
	}
	//if we are not connected then lets connect to the ECM
	if (iso_state==ISO_UNKNOWN)
	{
		//first call the fast connect
		//oops not done yet
		//then call the 5baud connect
		ret=iso_5baud(keys,2,address);
/*
		printf("ISO connect ret %d\n\r",ret);
		for(i=0; i<ret; i++)
		{  
			printf("%X ",keys[i]);
		}
		printf("\n\r");
*/
		if (ret>=2)
		{
			//we are connected
			iso_state=ISO_CONNECT;
			connect_timer(1); //reset timer
		}else
		{
			printf("Error: ISO slow connect failed\n\r");
			return 0;
		}
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
	message[i]=checksum(message,i);
	i++;
/*
	printf("Sending: ");
	for(ret=0; ret<i; ret++)
	{
		printf("%X ",message[ret]);
	}
	printf("\n\r");
*/
	//now we can send message
	ret=iso_put(message,i,ISO_P3_MIN);
	if(ret!=i)
	{
		printf("Error: ISO message sending problems\n\r");
		iso_state= ISO_UNKNOWN;
		return 0;
	}
	//now lets get response
	ret=iso_get(resp,len,ISO_P3_MAX);
/*
	printf("Got: ");
	for(i=0; i<ret; i++)
	{
		printf("%X ",resp[i]);
	}
	printf("\n\r");
*/
	if (ret)
		connect_timer(1); //reset connection timer

	return ret;
}
	

UBYTE iso_get(UBYTE *data, UBYTE len, UWORD time_out_ms)
{
  	return iso_getAuto(data,len,time_out_ms,0);
}
 
//Well I need an Async get char function
UBYTE iso_getAuto(UBYTE *data, UBYTE len, UWORD time_out_ms,UBYTE autoBaud )
{
	UWORD timer_ms;
	UBYTE i;
	UBYTE temp;	
	UBYTE index;
	UBYTE newBaud=0;

	//we assume that we are at 10400 baud
	if (baud==0)
		baud=ISO_10400_BAUD;

	index=0;
	K_HIGH;	   //make sure bus is high... 
	
	disable_interrupts(GLOBAL);	
	while(index<len)
	{	
		i=0;
		TMR0=0;
		timer_ms=0;
		if (index>0)
		{
			time_out_ms=ISO_W2_MAX;
 		}
		while(K_IN==0)	//spin bus to go high		
		{
			if (TMR0>250)
			{
				TMR0=0;
				i=i+1;
				if(i>=5)
				{
					timer_ms++;
					if (timer_ms>ISO_P2_MAX)
					{
						printf("Error: ISO get bus error\n\r");
						enable_interrupts(GLOBAL);
						return index;
					}
					i=0;
				}
			}
		}
		timer_ms=0;
		while(K_IN)	//spin waiting on sync bit
		{
			if (TMR0>250)
			{
				TMR0=0;
				i=i+1;
				if(i>=5)
				{
					timer_ms++;
					if (timer_ms>time_out_ms)
					{
						if (index==0)
							printf("Error: ISO get time out\n\r");
						enable_interrupts(GLOBAL);
						return index;
					}
					i=0;
				}
			}
		}
		
		//start bit just went low
		//wait a bit and half and read data
		TMR0=0;	
		//if baud is zero assume user wants auto baud
		if (autoBaud) 
		{
			while(K_IN==0);
			baud=TMR0+1;
			newBaud=1;
			autoBaud=0;
		}else
		{
			while(TMR0<baud);
		}
		TMR0=0;
		while(TMR0<(baud/2));	
		temp=0;
		for(i=0; i<8; i++)
		{
			temp=temp>>1;
			if (K_IN)
			{
				temp=temp | 0x80;
			}
			TMR0=0;		
			while(TMR0<baud);
		}	   
		*data=temp;
		data++;
		index++;
	}
    enable_interrupts(GLOBAL);
/*
	if (newBaud)
	{
	   printf("bps=%f %d\n\r",100000.0/((float)baud*0.8), baud);
	}
*/
	return index;
}


UDWORD iso_Baud()
{
   float temp;

   temp=1000000.0/((float)baud*0.8);
   return (UDWORD)temp;
}
		

//well life is a bitch then you die... 
//I need some way of testing ISO interface
//So I need a monitor
#define ISO_MONITOR_IN K_IN
UBYTE iso_monitor()
{
	UWORD ms_timer;
	UBYTE count,i;
	UBYTE data[20];
	
	printf("Monitor started\n\r"); 
	printf("Hit any key to stop\n\r");

	//lets wait for some activity on the 
	count=0;
	ms_timer=0;
	TMR0=0;
	while (ISO_MONITOR_IN==1 && ms_timer<=3000)
	{
	   if (TMR0>=250)
		{
			TMR0=0;
			count++;
			if (count>=5)
			{
				ms_timer++;
				count=0;
			}
		}
	}
	if (ms_timer>=3000)
	{
		printf("high for over 3sec\n\r");
		return 0;
	}
	disable_interrupts(GLOBAL);
	TMR0=0;
	count=0;
	ms_timer=0;
	while (ISO_MONITOR_IN==0 && ms_timer<3000)
	{
	   if (TMR0>=250)
		{
			TMR0=0;
			count++;
			if (count>=5)
			{
				ms_timer++;
				count=0;
			}
		}
	}
	count=TMR0;
	TMR0=0;

	if (ms_timer>=3000)
	{
		printf("low for over 3sec\n\r");
		enable_interrupts(GLOBAL);
		return 0;
	}

	
	//we have just got a low pulse
	if (ms_timer>=23 && ms_timer<=28)
	{
		UBYTE i;
		//we have fast init
		//printf("Fast\n\r");

		//now lets wait process data
		//data=iso_getc();
		disable_interrupts(GLOBAL);
		count=iso_get(data,20,3000);
		enable_interrupts(GLOBAL);
	  	printf("\n\r\n\rFast Init %u ",count);
		for(i=0; i<count; i++)
		{
			fprintf(STDIN,"%X ", data[i]);
		}
	}else
	if (ms_timer>=175 && ms_timer<225)
	{
		//we have slow init
		//printf("Slow\n\r");
		delay_ms(1600);	 //wait for rest of slow bits
		disable_interrupts(GLOBAL);
		count=iso_get(data,20,3000);
		enable_interrupts(GLOBAL);
		if (count==0)
		{
			printf("No response from ECM Slow\n\r");
			//return 0;
		}
		printf("\n\r\n\rSlow Init %u ",count);
		for(i=0; i<count; i++)
		{
			fprintf(STDIN,"%X ", data[i]);
		}

	}else 
	if (ms_timer<23 || ms_timer>225)
	{
		printf("Unknown ms=%lu,TMR0=%u\n\r",ms_timer,count);
		enable_interrupts(GLOBAL);
		return 0;
	}

	//lets recieve data
	while(!kbhit())
	{
	  	disable_interrupts(GLOBAL);
	  	count=iso_get(data,20,3000);
		enable_interrupts(GLOBAL);
	  	printf("\n\rFast %u ",count);
		for(i=0; i<count; i++)
		{
			fprintf(STDIN,"%X ", data[i]);
		}
	}
	return 0;
	
}



UBYTE iso_fast_init(UWORD idle_ms)
{
	UWORD ms_timer;
	UBYTE count; 

	//first part of the initilization process is to
	//wait for bus to be idle
	TMR0=0;
	count=0;
	ms_timer=0;
	while(ms_timer<idle_ms)
	{
		if (K_IN==0)
			return 0; //bus in use

		if (TMR0>=250)
		{
			TMR0=0;
			count++;
			if (count>=5)
			{
				ms_timer++;
				count=0;
			}
		}
	}

	//ok bus is idle, so lets send a init pulse
	K_LOW;
	L_LOW;
	delay_ms(25);
	K_HIGH;
	L_HIGH;
	//delay for rest of Twup

	//now send some data

}

//sends out a char at 5-baud rate
UWORD iso_5baud_putc(UBYTE data)
{ 
	UBYTE i;
	
	//first drive low for start bit
	K_LOW;
	L_LOW;
	delay_ms(200);
	for(i=0; i<8; i++)
	{
		if (data & 0x01)
		{
			K_HIGH;
			L_HIGH;
		} else
		{
			K_LOW;
			L_LOW;
		}
		data=data>>1;
		delay_ms(200);
	}
	//output stop bit
	K_HIGH;
	L_HIGH;
	delay_ms(200);

	return 0;
}
	
		  
	
		

//preform 5 baud initlization
//returns the time for the baud rate
UWORD iso_5baud(UBYTE *keys, UBYTE len, UBYTE address)
{
	UWORD timer_ms;
	UBYTE data;
	UBYTE i,temp;

	//first make sure that the buss is high for at least W0
	timer_ms=0;
	temp=0;
	TMR0=0;
	while(timer_ms<=ISO_W0_MIN)
	{
		if (K_IN==0)
			return 0;

		if (TMR0>=250)
		{
			temp++;
			if (temp>=5)
			{
				timer_ms++;
				temp=0;
			}
		}
	}
	printf("5 baud addr=%x\n\r",address);
	iso_5baud_putc(address);

	data=0;
	temp=iso_getAuto(&data,1,ISO_W1_MAX*4,1); //auto baud
	//printf("got %d %x\n\r",temp,data);
	if (data!=0x55)
	{
		printf("Bad 5baud init %u %X\n\r",temp,data);  
		baud=0;
		return 0;
	}
	//baud=ISO_10400_BAUD;

	//TODO handle none irregular baud rates...
	data=iso_get(keys,len,ISO_W2_MAX*4);
	if (data>=2)
	{
		temp=~keys[1];
		//fputc(temp,K_FAST);
		iso_put(&temp,1,ISO_W4_MIN);  
		if (address==0x33)
		{
			temp=0;
			i=iso_get(&temp,1,ISO_W4_MAX*2);
			if (i==0 ||  temp!=0xCC)
			{
				printf("Error: ISO 5 baud failed %X %X\n\r",keys[1],temp);
				baud=0;
				return 0; 
			}
		}/*else
		{
		   printf("5bps Keys %x %x, resp %x\n\r",keys[0],keys[1],temp);
		}  */
	} else
	{
		printf("Bad keys\n\r");
	}	 
	iso_state=ISO_CONNECT;
	return data;
}

	

UBYTE iso_put(UBYTE *data, UBYTE len, UWORD idle_wait)
{
	UWORD timer_ms;
	UBYTE i;
	UBYTE bits;
	UBYTE temp;
	UBYTE index;


	//first check for valid baud rate
	if (baud==0)
		baud=ISO_10400_BAUD; 

	//make sure buss is high
	timer_ms=0;
	i=0;
	TMR0=0;
	
	disable_interrupts(GLOBAL);
	while(timer_ms<idle_wait)
	{
		if (!K_IN)
		{
			printf("Error: Put failed\n\r");
			enable_interrupts(GLOBAL);
			return 0;
		}

		if (TMR0>=250)
		{
			TMR0=0;
			i++;
			if (i>=5)
			{
				timer_ms++;
				i=0;
			}
		}
	}

	//printf("ms timer %lu\n\r",timer_ms);

	//for each bit.... 
    index=0;
	while(index<len)
	{
		UBYTE mask;

		temp=data[index];
		mask=0x01;
		//drive with start bit, then send data
		K_LOW;
   		TMR0=0;
		while(TMR0<baud);
		TMR0=0;
		for (bits=0; bits<8; bits++)
		{
			if (temp & mask)
			{
				K_HIGH;
			}else
			{
				K_LOW;
			}		
			mask=mask<<1;
			while(TMR0<baud);
			TMR0=0;
		}

		//now we need to send stop bit
		K_HIGH;
		TMR0=0;
		while(TMR0<baud);

		index++;
		if(index>=len)
		{
			enable_interrupts(GLOBAL);
			return index;
		}
		//now wait to send next byte
		timer_ms=0;
		i=0;
		//TMR0=0;
		while(timer_ms<ISO_P4_MIN)
		{
			if (!K_IN)
			{
				fprintf(STDIN,"ERROR: ISO put failed\n\r");
				enable_interrupts(GLOBAL);
				return 0;
			}

			if (TMR0>=250)
			{
				TMR0=0;
				i++;
				if (i>=5)
				{
					timer_ms++;
					i=0;
				}
			}
		}

	}
	enable_interrupts(GLOBAL);
	return index;
}
		

	
		


