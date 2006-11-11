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

#include "pwm.h"

UBYTE pwm_rx(UBYTE *ptr, UBYTE count, UWORD time_out_ms);
	
UBYTE pwm_monitor()
{
	UBYTE data[60];
	UBYTE ret;
	UBYTE i;

	printf("Pwm monitor\n\r");
	printf("Hit any key to stop\n\r");

	while(!kbhit())
	{
		disable_interrupts(GLOBAL);
		ret=pwm_get(data,60,4000);
		enable_interrupts(GLOBAL);
		printf("Return %u\n\r",ret);
		for(i=0; i<ret; i++)
		{
			printf("%X ",data[i]);
		}
		printf("\n\r\n\r");
	}
	while(kbhit())
	{
		getch2();
	}
	return 0;
}

UBYTE pwm_send(UBYTE *resp, UBYTE len, UBYTE *msg, UBYTE msg_len)
{
	UBYTE ret,i,j;
	UBYTE message[20];
	UBYTE done;

	if (msg_len>6)
	{
		printf("Error: PWM message allows only 7 bytes\n\r");
		return 0;
	}
 	//OK we need to build the message
	message[0]=0x61;
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
	printf("Sending: ");
	for(ret=0; ret<i; ret++)
	{
		printf("%X ",message[ret]);
	}
*/
	//now we can send message
	for(j=0; j<3; j++)
	{
		ret=pwm_put(message,i,2000);
		if (ret==i)
			break;
	}
	if(ret!=i)
	{
		printf("Error: PWM message sending problems %d\n\r",ret);
		return 0;
	}
	//now lets get response
	//delay_us(500);
	//printf("calling PWM get\n\r");
	//ret=pwm_get(resp,len,2000);

	done=0;
	i=0;
	while(!done)
	{
		ret=pwm_rx(resp,len,2000);
		if (ret>5)
		{
			if (resp[0]==0x41)
				done=1;
		}
		i++;
		if (i>20)
			done=1;
	}

			
				//ret=pwm_rx(resp,len,2000);
	//ret=pwm_rx(resp,len,2000);
/*
	printf("\tRx: ");
	for(i=0; i<ret; i++)
	{
		printf("%X ",resp[i]);
	}
	printf("\n\r");
*/

	return ret;
}


//waits and gets count bytes from PWM circuit
// returns the number of bytes actually read
UBYTE pwm_get(UBYTE *ptr, UBYTE count, UWORD time_out_ms)
{
	UBYTE cycle;
	UBYTE bits=0;
	UBYTE pwm_low; 
	UBYTE pwm_data;
	UBYTE pwm_index;
	UWORD ms_timer;
	UBYTE tcount;
	UBYTE pwm_state=0;

	//TMR0=0;
	pwm_index=0;
	tcount=0;
	ms_timer=0;


big_loop:
   	//wait for pin to go high (IDLE BUS)
	//printf("PWM is %u\n\r",PWM_IN);
   
	while(PWM_IN)
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
					printf("PWM timeout 1\n\r");
					return pwm_index;
				}
			}
		}
	}	
	while(PWM_IN_LOW ==0)
	{
		if (TMR0>=250)
		{
			pwm_state=PWM_IDLE;
			if (pwm_index)
				return pwm_index;
			TMR0=0;
			tcount++;
			if (tcount>=5)
			{
				tcount=0;
				ms_timer++;
				if (ms_timer>time_out_ms)
				{	
					printf("PWM timeout 2\n\r");
					return pwm_index;
				}
			}
		}
	}
	//first get the cycle time
	cycle=TMR0;		
	TMR0=0;
	disable_interrupts(GLOBAL);
	bits=0;
	pwm_data=0;


	//wait for SOH
	while(PWM_IN==1);
	{
		if (TMR0>250)
		{
			enable_interrupts(GLOBAL);
			return pwm_index;
		}
	}
	cycle=TMR0;
	TMR0=0;
	if (cycle>PWM_TP7_MIN && cycle<PWM_TP7_MAX)
	{
		//we have SOH
		goto pwm_loop;
	} else
	{
		goto big_loop;
	}		
	//printf("cycle %u\n\r",cycle);
/* 
  	//check for last symbol for EOD 
	if (cycle>=PWM_TP4_MIN )
	{
		bits=0;
		pwm_data=0;
		pwm_state=PWM_MARK;
		//goto pwm_loop1;	//in frame response..			

	}
	//check last symbol for bus idle
	if (cycle>=PWM_TP6_MIN)
	{
		pwm_state=PWM_IDLE;
	}
	//if state was idle then this should be a mark bit
  	if (pwm_state==PWM_IDLE)
*/
	{
		bits=0;
		pwm_data=0;
		pwm_state=PWM_MARK;		//set the state
  		goto pwm_wait_low;			//loop waiting for SOH
	}

	goto pwm_loop1;	//in frame response...
pwm_loop:
		
   	while(PWM_IN_LOW==0)
	{
	   //	if (TMR0>250)
	    if (TMR0>=PWM_TP3_MAX)//PWM_TP4_MIN)
		{
			//printf("cycle was %u %d\n\r",cycle,pwm_index);
			enable_interrupts(GLOBAL);
			return pwm_index;
		}
	}

	//first get the cycle time
	cycle=TMR0;		
	TMR0=0;
	//printf("low time is %u\n\r",cycle);
/*
	if (cycle>=PWM_TP4_MIN)
	{
		//printf("time out in data\n\r");
		enable_interrupts(GLOBAL);
		return pwm_index;
	}
*/
pwm_loop1:		
   
	pwm_low=pwm_data<<1;			//make low
  
	pwm_data=pwm_low | 0x01;	  //assume high bit
	bits++;

	while (TMR0<=PWM_TP1_MAX);
	if (PWM_IN_HIGH==1)
	{
		pwm_data=pwm_low;
	}

			
pwm_wait_low:
	if (bits==8)
	{
		//printf("%X ",pwm_data);
		
		ptr[pwm_index]=pwm_data;
		pwm_index++;
		if (pwm_index>count)
		{
			enable_interrupts(GLOBAL);
			return pwm_index--;
		}
		//putc(pwm_data);
		pwm_data=0;
		bits=0;
	}

	while (PWM_IN_HIGH==1);		
	cycle=TMR0;
	TMR0=0;
	//printf("high time %u\n\r",cycle);
	//TMR0=0;
	//cycle=TMR0;
	//printf("Low in %U\n\r",cycle);
	goto pwm_loop;
}



//Now we need a way to send data to the PWM bus
UBYTE pwm_put(UBYTE *data, UBYTE count, UWORD time_out_ms)
{
	UBYTE tcount;
	UBYTE temp;
	UWORD timer;
	UBYTE index;
	UWORD ms_timer;
	UBYTE bits;
	UBYTE thisbit;
	
	//first wait for idle bus
	TMR0=0;
	temp=0;
	tcount=0;
	index=0;
	bits=7;
	timer=0;
	//printf("got here\n\r");
	while(TMR0<PWM_TP6)
	{
		//if we get a high on bus reset idle counter	
		if (PWM_IN_HIGH==1)
		{
			timer=timer+(UWORD)TMR0;
			TMR0=0;
		}
	   
		if ((timer+TMR0)>=250)
		{
			tcount++;
			//printf("got here %lu\n\r",ms_timer);
			timer=0;
			TMR0=0;
			if (tcount>=5)
			{
				ms_timer++;
				tcount=0;
				if (ms_timer>time_out_ms)
				{
					printf("PWM put time out\n\r");
					return 0;
				}
			}
		}
	}

	
	disable_interrupts(GLOBAL);
	//bus is now idle, so we need to send SOF marker
	TMR0=0;
	output_high(PWM_OUT);
	while (TMR0<=PWM_TP7);
	output_low(PWM_OUT);	
	//check to make sure that we still have bus
	temp=TMR0+13;
	while(TMR0<=temp);
	if (PWM_IN)
   	{
   		while(PWM_IN);
		enable_interrupts(GLOBAL);
   		return 0; 
   	}
	//wait for low period
	while(TMR0<=PWM_TP4);


	temp=data[index];
	//now we need to clock bits
	bits=0x80;
	thisbit=  temp & bits;
	TMR0=0;
	while (1)
	{		
		TMR0=0;
		if (thisbit)
		{
			output_high(PWM_OUT);
			delay_us(2); //3 2
			output_low(PWM_OUT); //drive low	
		}else
		{
		 	output_high(PWM_OUT);
			delay_us(8); //8 9
			output_low(PWM_OUT); //drive low
		}
			
		bits=bits>>1;	
		if (bits==0)
		{
			bits=0x80;
			index++;
			if (index>=count)
			{
				enable_interrupts(GLOBAL);
				return index;
			}
		}
		temp=data[index];
		//wait for low period
		//check to make sure that we still have bus
		thisbit=  (temp & bits);
		delay_us(6);
		if (PWM_IN_HIGH==1)
		{
			enable_interrupts(GLOBAL);
			return index; 
		}
		while(TMR0<PWM_TP3);

			
	}
	//while(TMR0<=PWM_TP5);
	enable_interrupts(GLOBAL);
	return index;
}




//this function tests the PWM get system
UBYTE pwm_rx(UBYTE *ptr, UBYTE count, UWORD time_out_ms)
{
#define PWM_RX_MAX_BUF 250
	UBYTE cycle;
	UBYTE bits=0;
	UBYTE pwm_index;
	UWORD ms_timer;
	UBYTE tcount;
	UBYTE pwm_state=0;
	UBYTE data[PWM_RX_MAX_BUF];
	UBYTE *ptrData;
	UBYTE i;
	UBYTE k;

	//TMR0=0;
	pwm_index=0;


big_loop:
	tcount=0;
	ms_timer=0;
	//wait for pin to go high (IDLE BUS)
	//printf("PWM is %u\n\r",PWM_IN);
   	i=0;
	ptrData=data;
	disable_interrupts(GLOBAL);
/*
	while(PWM_IN)
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
					printf("PWM timeout 1\n\r");
					return pwm_index;
				}
			}
		}
	}	
	while(PWM_IN_LOW ==0)
	{
		if (TMR0>=250)
		{
			pwm_state=PWM_IDLE;
			if (pwm_index)
				return pwm_index;
			TMR0=0;
			tcount++;
			if (tcount>=5)
			{
				tcount=0;
				ms_timer++;
				if (ms_timer>time_out_ms)
				{	
					printf("PWM timeout 2\n\r");
					return pwm_index;
				}
			}
		}
	}
	//first get the cycle time
	cycle=TMR0;	
*/	
	TMR0=0;
	
	pwm_state=PWM_IN;
	cycle=0;
	while (1)// && TMR0<250)
	{
		if(TMR0>250)
		{
			TMR0=50;
			cycle++;
			if(cycle>250)
				break;
		}

		if (pwm_state!=PWM_IN )
		{
			UBYTE temp;
			temp=TMR0;
			TMR0=0;
			//if (cycle)
		   //	{
				if (temp>50)
					temp=50;
				if (pwm_state)
					*ptrData++=0x80 | temp;
				else
					*ptrData++= temp;
				i++;
		   //	}else if (PWM_IN && temp>PWM_TP7_MIN && temp<PWM_TP7_MAX)
			//{
			//	cycle=1;
		   //	}

			//TMR0=0;
			pwm_state=PWM_IN;
			if(i>=PWM_RX_MAX_BUF)
				break;
		}
	}
/*
	for(pwm_index=0; pwm_index<i; pwm_index++)
	{
		printf("%d\t%d\n\r",data[pwm_index]>>7, data[pwm_index]&0x7F);
	}
*/

	k=0;

	for(pwm_index=0; pwm_index<i; pwm_index++)
	{
		UBYTE level;
		UBYTE t;
		level=data[pwm_index]>>7;
		t=data[pwm_index]&0x7F;
		if (level==1 && t>=PWM_TP7_MIN && t<=PWM_TP7_MAX)
		{
			UBYTE j;
			UBYTE result=0;
			UBYTE bits=0;
			j=pwm_index+1;
			level=data[j]>>7;
			t=data[j]&0x7F;
			while(j<i && t<PWM_TP4_MAX)
			{
				if (level==1)
				{
					result=result<<1;
					bits++;
					if(t<=PWM_TP1_MAX)
					{
						result=result | 0x01;
					}
					if (bits==8)
					{
						//printf("%X ",result);
						*ptr++=result;
						k++;
						if (k>=count)
						{
						   //	printf("end %d\n\r",k);
							enable_interrupts(GLOBAL);
							return k;
						}
						result=0;
						bits=0;

					}
				}
				j++;
				level=data[j]>>7;
				t=data[j]&0x7F;
			}
			//printf("\n\r");
		}
	}
				
	enable_interrupts(GLOBAL);

	return k;
}
	

				
