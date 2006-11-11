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

//Slave PICs

#include "datatypes.h"

#include "scanner.h"

#include "serial.h"
#include "datatypes.h"
#include "LCD.h"
#include "flash.h"
#include "pwm.h"
#include "vpw.h"
#include "command.h"
#include "iso.h"
#include "xmodem.h"
#include "codes.h"
#include "obdii.h"
#include "knob.h"
#include "firmware.h"
   

#define USE_LCD 1




#define SW0 (0x20)
#define SW1 (0x10)
#define SW2 (0x08)
#define SW3 (0x04)

BYTE knob=0;
UDWORD seconds=0;


UBYTE getHwVer()
{
	UBYTE temp;
	set_tris_h(TRIS_H | 0x04);
	temp=2; 
	if (input(PIN_H2)>0)
		temp++;
	set_tris_h(TRIS_H);
	return temp;
}

UDWORD getSeconds()
{
	UDWORD temp;
	disable_interrupts(INT_TIMER1);
	temp=seconds;
	enable_interrupts(INT_TIMER1);
	return temp;
}


UINT datalog_bin()
{
	UBYTE level;
	UBYTE timer;
	UBYTE done;

	printf("Binary logging\n\r");
	printf("press any key to begin\n\r");

	while(!kbhit());

	while(kbhit())
		getch2();

	done=0;
	while(!done)
	{
		level=VPW_IN;
		while(level==VPW_IN)
		{
			if (TMR0>=127)			
			{
				TMR0=0;
				
				if (level)
					putch(0xFF);
				else
					putch(0xEF);
				done=kbhit();
				if (done)
					break;
			}		
		}
		timer=TMR0;
		TMR0=0;
		if (level)
			putch(timer | 0x80);
		else
			putch(timer);

	}
	while(kbhit())
		getch2();

	return 0;
}

//this function is one to do data logging
#define MAX_LOG 400
UINT datalog()
{
	UDWORD data[MAX_LOG];
	UWORD count;
	UWORD i;
	UBYTE done;

	printf("Recording\n\r");
	printf("Press any key to stop\n\r");

	done=0;
	i=0;
	TMR0=0;
	while(!done)
	{
		UBYTE level;
		UDWORD timer;
		
		///ps_released=(ps_released<<1) | (POWER_SW>0);
		done=kbhit();

		//count=0;
		timer=0;
		level=VPW_IN;
		if (level)
			output_low(PIN_B6);
		else
			output_high(PIN_B6);
		while(level==VPW_IN)
		{
			if (TMR0>=255)			
			{
				timer+=255;
				TMR0=0;
				done=kbhit();
				if (done)
					break;
				//printf("%d %d\n\r",level,count);
			}		
		}
		timer+=TMR0;
	//	while(level==VPW_IN && POWER_SW==1);
		TMR0=0;
		
		
		if (level)
		{
			data[i]=timer | 0x80000000;
		}else
			data[i]=timer;
		i++;
		if (i>=MAX_LOG)
			done=1;

	}
	count=i;
	
	while (kbhit())
		getch2();
	printf("\n\r\n\r NEW data %lu\n\r",count);
	for (i=0; i<count; i++)
	{
		printf("%d %lu\n\r",(data[i] & 0x80000000)>0, (data[i] & 0x7FFFFFFF));
	}

	return 0;
}


//flash led for errors
UBYTE flash_leds()
{
	UBYTE i;

	for(i=0; i<15; i++)
	{
		LED_CAT(1);
		LED_HEATED_CAT(1);
		LED_DTC(1);
		LED_EVAP(1);
		LED_AIR(1);
		LED_O2(1);
		LED_HEATED_O2(1);
		LED_EGR(1);
		LED_MISFIRE(1);
		LED_AC(1);
		LED_FUEL(1);
		LED_COMP(1);
		delay_ms(100);
		LED_CAT(0);
		LED_HEATED_CAT(0);
		LED_DTC(0);
		LED_EVAP(0);
		LED_AIR(0);
		LED_O2(0);
		LED_HEATED_O2(0);
		LED_EGR(0);
		LED_MISFIRE(0);
		LED_AC(0);
		LED_FUEL(0);
		LED_COMP(0);
		delay_ms(100);		
	}
	return 0;
}


	

//This function is used to process and display data
UBYTE display(UBYTE *data, UBYTE len,UBYTE connect)
{
	
	static UBYTE errors=0;
	//first we need to check for errors in the data stream
	//we know the data stream should be 10 bytes long
	if (len==20)
	{
		//check CRC/checksum for each ECU
		if (check_errors(data,10,connect) || check_errors(&data[10],10,connect))
		{
			flash_leds();
			printf("Error: Two ECMs checksum/crc error\n\r");
			return 0;
		}
		//CRC/checksum passed
				
		//add the error codes from two ecms
		data[5]=(data[5] & 0xEF) + (data[15] & 0xEF);
		data[6]=data[6] | data[16];
		data[7]=data[7] | data[17];
		data[8]=data[8] | data[18];
	}else if (len!=10)
	{
		errors++;
		printf("Error: processing data length problems %u",len);
		if (errors>=5)
		{		
			flash_leds();
			errors=0;
		}
		return 0;
	}

	if (len==10)
	{
		if (check_errors(data,len,connect)!=0)
		{
			errors++;
			if (errors>=5)
			{
				flash_leds();
				errors=0;
			}
			return 0;
		}
	}
 
	//we have good data lets process it 
	
	//first check for DTC 
	if (data[5] & 0x7F)
	{
		LED_DTC(1);
	}else
	{
		LED_DTC(0);
	}

	//now lets check for the Catalyst
	if (data[7] & 0x01)
	{
		//monitor supported
		if (data[8] & 0x01)
		{
			LED_CAT(0); //Not complete
		}else
		{
			LED_CAT(1);
		}
	}else
	{
		LED_CAT(1); //not applicabple
	}

	if (data[7] & 0x02)
	{
		//monitor supported
		if (data[8] & 0x02)
		{
			LED_HEATED_CAT(0); //Not complete
		}else
		{
			LED_HEATED_CAT(1);
		}
	}else
	{
		LED_HEATED_CAT(1); //not applicabple
	}

	if (data[7] & 0x04)
	{
		//monitor supported
		if (data[8] & 0x04)
		{
			LED_EVAP(0); //Not complete
		}else
		{
			LED_EVAP(1);
		}
	}else
	{
		LED_EVAP(1); //not applicabple
	}

	if (data[7] & 0x08)
	{
		//monitor supported
		if (data[8] & 0x08)
		{
			LED_AIR(0); //Not complete
		}else
		{
			LED_AIR(1);
		}
	}else
	{
		LED_AIR(1); //not applicabple
	}

	if (data[7] & 0x10)
	{
		//monitor supported
		if (data[8] & 0x10)
		{
			LED_AC(0); //Not complete
		}else
		{
			LED_AC(1);
		}
	}else
	{
		LED_AC(1); //not applicabple
	}
	if (data[7] & 0x20)
	{
		//monitor supported
		if (data[8] & 0x20)
		{
			LED_O2(0); //Not complete
		}else
		{
			LED_O2(1);
		}
	}else
	{
		LED_O2(1); //not applicabple
	}

	if (data[7] & 0x40)
	{
		//monitor supported
		if (data[8] & 0x40)
		{
			LED_HEATED_O2(0); //Not complete
		}else
		{
			LED_HEATED_O2(1);
		}
	}else
	{
		LED_HEATED_O2(1); //not applicabple
	}
	if (data[7] & 0x80)
	{
		//monitor supported
		if (data[8] & 0x80)
		{
			LED_EGR(0); //Not complete
		}else
		{
			LED_EGR(1);
		}
	}else
	{
		LED_EGR(1); //not applicabple
	}

	if (data[6] & 0x01)
	{
		//monitor supported
		if (data[6] & 0x10)
		{
			LED_MISFIRE(0); //Not complete
		}else
		{
			LED_MISFIRE(1);
		}
	}else
	{
		LED_MISFIRE(1); //not applicabple
	}

	if (data[6] & 0x02)
	{
		//monitor supported
		if (data[6] & 0x20)
		{
			LED_FUEL(0); //Not complete
		}else
		{
			LED_FUEL(1);
		}
	}else
	{
		LED_FUEL(1); //not applicabple
	}

	if (data[6] & 0x04)
	{
		//monitor supported
		if (data[6] & 0x40)
		{
			LED_COMP(0); //Not complete
		}else
		{
			LED_COMP(1);
		}
	}else
	{
		LED_COMP(1); //not applicabple
	}
	errors=0;
	return 0;
}

UWORD cnt_timer;
UWORD connect_timer(UBYTE reset)
{
	if (reset)
	{
		disable_interrupts(GLOBAL);
		cnt_timer=0;
		enable_interrupts(GLOBAL);
	}
	return cnt_timer;
}

UWORD lcd_timer;
#int_timer1
void isr_timer1()
{
	
	static UBYTE count=0;
	lcd_timer++;
	if(cnt_timer<0xFFF0)
		cnt_timer++;
	
	count++;
	if(count>10)
	{
		seconds++;
		count=0; 
	}
	//reset timer1 to a 10ms timer
	TMR1H=0x3C;
	TMR1L=0xB0;
}

UBYTE high_time;
UBYTE pwm_data;
UBYTE n_pwm_data;
#int_ext2 fast
void isr_int2()
{
	UBYTE temp;
	temp=TMR0;
	TMR0=0;
	if (bit_test(INTCON2,4))
	{
		bit_clear(INTCON2,4);
		if (temp>	35)
		{
			n_pwm_data=0;
		}
	}else
	{
		high_time=temp;
		n_pwm_data++;
		n_pwm_data=n_pwm_data & 0x07;
		pwm_data=pwm_data<<1;
		if (temp<=15)
		{
			pwm_data=pwm_data | 0x01;
		}
				
		bit_set(INTCON2,4);
	}
	bit_clear(INTCON3,1);
}

#define PWM_HIGH 	1
#define PWM_LOW 	2
#define PWM_CHECK 	3
#define PWM_SOH 	4
#define PWM_SOH_LOW 5
#define PWM_DONE	0
UBYTE pwm_state=PWM_SOH;


#define PWM_OUTPUT(x)	output_bit(PIN_H6,x)	//ouput bit
#define PWM_GET	PWM_IN						//gets bit
#define PWM_CHECK_DELAY 10
UBYTE pwm_out_data[10];
UBYTE pwm_out_cnt;
#int_timer3 
void isr_timer3()
{
	static UBYTE bits=7;
	static UBYTE index=0;
	static UBYTE temp;
	static UBYTE last=0;

	TMR3H=0xFF;
	bit_clear(PIR2,1);	//clear the TMR3IF
	switch (pwm_state)
	{
		case PWM_HIGH:
			TMR3=255-(PWM_TP3-8);
			temp=pwm_out_data[index];			
			PWM_OUTPUT(1); 		//ouput high			
			if(!bit_test(temp,bits))
			{
				delay_us(5);
			}
			PWM_OUTPUT(0); 		//ouput high	
			if (bits==0)
			{
				index++;			
			}
			bits--;
			bits=bits&0x07;
				
			//pwm_state=PWM_HIGH;	//set next state
			if (index>=pwm_out_cnt)
			{
				pwm_state=PWM_DONE;
			}
			break;
		case PWM_SOH:
			index=0;
			bits=7;
			PWM_OUTPUT(1); 		//output a high
			TMR3=255-(PWM_TP7-8);	//set timer for next transistion
			pwm_state=PWM_SOH_LOW;	//set next state
			break;
		case PWM_SOH_LOW:
			PWM_OUTPUT(0); 		//ouput low
			TMR3=255-(PWM_TP4-PWM_TP7-11);	//set timer for next transistion
			pwm_state=PWM_HIGH;	//set next state
			break;
		case PWM_DONE:
			index=0;
			bits=7;
			disable_interrupts(INT_TIMER3);
			pwm_state=PWM_SOH; //set up state for next round
			break;
	}
	
}	

void iso_test()
{
	UBYTE str[16];
	UBYTE rs[4]={0};
	UBYTE cmd[16];
	UBYTE ps_released,power,done;
	UBYTE ret;
	BYTE knob;			
	//lets try an ISO simulator....
	while(connect_timer(0)<300);
	done=0;

	while(1)
	{
		UBYTE data;
		UBYTE i;
		UBYTE len;
		UBYTE byte1, byte2;

		if (KnobSw1())
			return;

				//first we need to get the 5baud signal
		//printf("Connect timer %lu\n\r",connect_timer(0));
		if (connect_timer(0)>300)
		{
			printf("Waiting on 5baud\n\r");
			while(K_IN==1); //spin waiting on low signal
			delay_ms(291);
			i=0;
			data=0;
			while(i<8)
			{
				data=data>>1;
				if(K_IN)
				{
					data=data | 0x80;
				}
				i++;
				delay_ms(200);
			}
			printf("Slow init returned %x\n\r",data);

			if(data==0x33)
			{
				//printf("Vaild 5baud\n\r");
				//next send scyronization
				data=0x55;
				ret=iso_put(&data,1,100);
				cmd[0]=0x08;
				cmd[1]=0x08;
				ret=iso_put(cmd,2,10);
				ret=iso_get(cmd,1,2000);
				if (ret==1 && cmd[0]==0xf7)
				{
					data=0xCC;
					ret=iso_put(&data,1,25);
					printf("init passed\n\r");
					done=1;
					connect_timer(1); //reset timer
				} else
				{
					printf("ISO get returned %d\n\r",ret);
				}
			}
		}

		if(done)
		{
			//ok now we are set up... 
			ret=iso_get(cmd,16,2000);
			printf("Iso Get returned %d\n\r",ret);
			if (ret!=0)
			{	
				connect_timer(1); //reset timer
				printf("Recieved ");
				for(i=0; i<ret; i++)
				{
					printf("%X ",cmd[i]);
				}
				printf("\n\r");
			}
				//OK we have gotten some data from scanner
			//now we need to parse it.... 
			byte1=0x68;
			byte2=0x6A;

			if (cmd[0]==byte1 && cmd[1]==byte2 && cmd[2]==0xF1)	
			{
				//start up request
				cmd[0]=(byte1 & 0x0F) | 0x40;
				cmd[1]=0x6B;
				cmd[2]=0x10;
				//now see what the request was
				if (cmd[3]==0x01)
				{
					//MODE 1
					cmd[3]=0x41;
					if (cmd[4]==0x00)
					{
						//request for PID support
						cmd[5]=0x80;
						cmd[6]=0x00;
						cmd[7]=0x00;
						cmd[8]=0x00;
						len=9;
					 }
					if (cmd[4]==0x01)
					{
						//request for PID 01
						cmd[5]=0x01;
						cmd[6]=0x07;
						cmd[7]=0xE1;
						cmd[8]=0x01;
						len=9;
					 }
				}
				if (cmd[3]==0x03)
				{
					//MODE 3 trouble code
					cmd[3]=0x43;
					cmd[4]=0x04;
					cmd[5]=0x20;
					len=6;				
				}
			   
				//delay_ms(500);
				cmd[len]=checksum(cmd,len);	
				len++;	

				printf("Sending ");
				for(i=0; i<len; i++)
				{
					printf("%X ",cmd[i]);
				}
				printf("\n\r");
				
				ret=iso_put(cmd,len,2);
				if (ret==0 || ret>16)
				{
					printf("Error putting data %d\n\r",ret);
				}
			}
		}
	}
}

#define CON_PWM 1
#define CON_VPW	2

void j1850_test(UBYTE connect)
{
	UBYTE str[16];
	UBYTE rs[4]={0};
	UBYTE cmd[16];
	UBYTE ps_released,power,done;
	UBYTE ret;
	BYTE knob;
	while(1)
	{
		UBYTE i;
		UBYTE len;
		UBYTE byte1, byte2;

		if (KnobSw1())
			return;

		if (connect==CON_PWM)
		{
			byte1=0x61;
			byte2=0x6A;

			ret=pwm_get(cmd,16,3000);
			if (ret==0 || ret>16)
			{
				printf("Error getting pwm data %d\n\r",ret);
				ret=0;
		}
		}else if (connect==CON_VPW)
		{
			
			byte1=0x68;
			byte2=0x6A;
			ret=vpw_get(cmd,16,1000);
			if (ret==0 || ret>16)
			{
				printf("Error getting vpw data %d\n\r",ret);
				ret=0;
			} 
		}else
		{
			return;
		}
				
		if (ret!=0)
		{	
			printf("Recieved ");
			for(i=0; i<ret; i++)
			{
				printf("%X ",cmd[i]);
			}
			printf("\n\r");	
			//OK we have gotten some data from scanner
			//now we need to parse it.... 
			if (cmd[0]==byte1 && cmd[1]==byte2 && cmd[2]==0xF1)	
			{
				//start up request
				len=0;
				cmd[0]=(byte1 & 0x0F) | 0x40;
				cmd[1]=0x6B;
				cmd[2]=0x10;
				len=3;
				//now see what the request was
				if (cmd[3]==0x01)
				{
					//MODE 1
					cmd[3]=0x41;
					len=4;
					if (cmd[4]==0x00)
					{
						//request for PID support
						cmd[5]=0x98;
						cmd[6]=0x40;
						cmd[7]=0x00;
						cmd[8]=0x00;
						len=9;
					 }
					if (cmd[4]==0x01)
					{
						//request for PID 01
						cmd[5]=0x01;
						cmd[6]=0x07;
						cmd[7]=0xE1;
						cmd[8]=0x01;
						len=9;
					}
					if (cmd[4]==0x04)
					{
						//Request for PID 4
						cmd[5]=0x127;
						len=6;				
					}
					if (cmd[4]==0x05)
					{
						//Request for PID 5
						cmd[5]=0x127;
						len=6;				
					}
					if (cmd[4]==0x0A)
					{
						//Request for PID A
					   	cmd[5]=0x127;
						len=6;				
					}
				}
				if (cmd[3]==0x03)
				{
					//MODE 3 trouble code
					cmd[3]=0x43;
					cmd[4]=0x04;
					cmd[5]=0x20;
					len=6;				
				}
				
				//delay_ms(100); //500
				cmd[len]=crc(cmd,len);	
				len++;	

				printf("Sending %u ",len);
				for(i=0; i<len; i++)
				{
					printf("%X ",cmd[i]);
				}
				printf("\n\r");
					
				if (connect==CON_PWM)
					ret=pwm_put(cmd,len,1000);
				if (connect==CON_VPW)
					ret=vpw_put(cmd,len,1000);
				if (ret==0 || ret>16)
				{
					printf("Error putting data %d\n\r",ret);
				}
			}
		}
	}	
}
	
void main()
{
	UBYTE str[16];
	UBYTE rs[4]={0};
	UBYTE cmd[16];
	UBYTE ps_released,power,done;
	UBYTE ret;
	BYTE knob;
	BYTE num;
	delay_ms(200);
	//rs232_init(9600);
	printf("Power up");

	//set up the pins
	SET_TRIS();

	//now we need to set up the internal voltage comparators
	CMCON=0x03;

	LCD_init();
	LCD_blackout();

	LED_CAT(1);
	LED_HEATED_CAT(1);
	LED_DTC(1);
	LED_EVAP(1);
	LED_AIR(1);
	LED_O2(1);
	LED_HEATED_O2(1);
	LED_EGR(1);
	LED_MISFIRE(1);
	LED_AC(1);
	LED_FUEL(1);
	LED_COMP(1);
	delay_ms(700);

	LED_CAT(0);
	LED_HEATED_CAT(0);
	LED_DTC(0);
	LED_EVAP(0);
	LED_AIR(0);
	LED_O2(0);
	LED_HEATED_O2(0);
	LED_EGR(0);
	LED_MISFIRE(0);
	LED_AC(0);
	LED_FUEL(0);
	LED_COMP(0);
	LCD_Send_command(0x01); //clear display

	//turn off PWM output
	output_low(PWM_OUT);
	output_low(VPW_OUT);
	K_HIGH;
	L_HIGH;

	PWM_ENABLE(0);	

	//setuput Timer1
	T1CON=0x91;
	TMR1H=0;
	TMR1L=0;

	setup_timer_0(RTCC_INTERNAL|RTCC_DIV_8|RTCC_8_BIT);
	setup_timer_3(T3_INTERNAL|T3_DIV_BY_8);
	flash_init();
/*
	//enable_interrupts(INT_EXT2);
	enable_interrupts(GLOBAL);

//lets test the PWM write using timer3
	pwm_out_data[0]=0xAA;
	pwm_out_data[1]=0x55;
	while(1)
	{
		pwm_out_cnt=2;
		pwm_state=PWM_SOH;
		enable_interrupts(INT_TIMER3);
		while(bit_test(PIE2,1));
		printf("PWM write done\n\r");
		delay_ms(200);
	}

//lets test the PWM read function
	while(1)
	{
		UBYTE data;
		if (n_pwm_data==0x07)
		{
			data=pwm_data;
			printf("%d %X\n\r",high_time,data);
		}
	}
*/
	KnobInit(); //Set up the knob

	enable_interrupts(INT_TIMER1);
	enable_interrupts(INT_RDA); //RS232 receive data available
	enable_interrupts(INT_RB); //RB port changed, for knob...
	enable_interrupts(GLOBAL);

	done=0;
	output_low(PIN_F6);
	output_low(PIN_F5); //Turn PWM output off

	LCD_clear();

	//WE need a way to test scanner, to do this we will use a scanner... 
	//To start with lets use just PWM mode...
	
	//first we need to get a command and send response...
	//CommandBegin();
	printf("Running test app\n\r");
	num=0;

	while(1)
	{
		if (num==0)
		{
			sprintf(str,"PWM Testing");
			LCD_print1(str,0);
			j1850_test(CON_PWM);
		}
		if (num==1)
		{
			sprintf(str,"VPW Testing");
			LCD_print1(str,0);
			j1850_test(CON_VPW);
		}
		if (num==2)
		{
			sprintf(str,"ISO Testing");
			LCD_print1(str,0);
			iso_test();
		}
		num=num+KnobRead();
		if (num<0)
			num=0;
		if (num>2)
			num=2;
	}//while



/*	
   	//lets test the OBDII process function
   	str[0]=0x48;
   	str[1]=0x6B;
   	str[2]=0x17;
   	str[3]=0x41;
   	str[4]=0x04;
   	str[5]=0x0A;
   	str[6]=0x19;
	str[7]=0x48;
   	str[8]=0x6B;
   	str[9]=0x17;
   	str[10]=0x41;
   	str[11]=0x04;
   	str[12]=0x0A;
   	str[13]=0x19;

   	
	ret=process(test,2,str,14,ISO);
  		
	sprintf(str,"ret=%d",ret);
	LCD_print1(str,0);

	for(ret=0; ret<test[0].n; ret++)
	{
		printf("%X ",test[0].data[ret]);
	}


	while(!kbhit());
*/
/*
	CommandBegin();
   
	printf("OBDII running\n\r");
	LCD_clear();
	sprintf(str,"Checking ECM");	
	LCD_print1(str,0);
	sprintf(str,"Turn Key On!");
	LCD_print2(str,0);

	while(1)
	{
		BYTE temp;
		temp=KnobRead();
		obdII_run(temp,0);
	}


	while (!done)
	{
		UBYTE msg[60];
		UBYTE data[20];
		UBYTE ret;
		UBYTE i;
		UBYTE connect;

		//first lets send the command to the PWM interface
		data[0]=0x01;
		data[1]=0x01;


		while(kbhit())
		{
			UBYTE temp;
			temp=getch2();
			printf("\n\rkey%X\n\r",temp);
			if (temp!=0x0d && temp!=0x0A)
			{
				rs[0]=rs[1];
				rs[1]=rs[2];
				rs[2]=temp;
				rs[3]=0;
				printf("\n\rCommand1=%X %X %X\n\r",rs[0], rs[1], rs[2]);
			}
			
		}
		printf("\n\rCommand=%X\n\r",RCSTA);
		if (rs[0]=='c' && rs[1]=='m' && rs[2]=='d')
		{
			printf("Command received\n\r");
			CommandBegin();
			rs[2]=0;
		}	
		
		printf("Checking ISO\n\r");
		LCD_clear();
		sprintf(str, "Checking ISO");
		LCD_print1(str,0);

		ret=iso_send(msg,60,data,2,0x33); 
		connect=ISO;
		if (ret==0)
		{
			printf("Checking PWM\n\r");	
			LCD_clear();
			sprintf(str, "Checking PWM");
			LCD_print1(str,0);

			connect=PWM;
			ret=pwm_send(msg,60,data,2); 
		}
		if (ret==0)
		{
			printf("Checking VPW\n\r");
			LCD_clear();
			sprintf(str, "Checking VPW");
			LCD_print1(str,0);

			connect=VPW;
			ret=vpw_send(msg,60,data,2); 
		}
		
		//now we need to process and display data
		printf("Display %u\n\r",ret);
		for (i=0; i<ret; i++)
		{
			printf("%X ",msg[i]);
		}
		printf("\n\r");
		display(msg,ret,connect);	

	}
	 

	
	sprintf(str,"Power");
	LCD_print2(str,3);
	delay_ms(1000);

	output_low(PIN_F0);
	//clear the LCD to let person know
	LCD_blackout();
	delay_ms(1000);
*/	
}

#include "flash.c"
#include "LCD.c"
#include "command.c"
#include "vpw.c"
#include "iso.c"
#include "pwm.c"
#include "xmodem.c"
#include "serial.c"
#include "codes.c"
#include "obdii.c"
#include "knob.c"
#include "encrypt.c"
#include "firmware.c"
