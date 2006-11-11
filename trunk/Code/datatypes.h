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

#ifndef __DATATYPES_H
#define __DATATYPES_H
#include <18F8680.h>
#case //make compiler case sensitive 
#fuses H4,NOWDT,PROTECT,NOLVP
#use delay(clock=40000000)
//#use I2C(slave,sda=PIN_C4,scl=PIN_C3,address=0x06,FORCE_HW,FAST)
#use fast_io(A)
#use fast_io(B)
#use fast_io(C)
#use fast_io(D)
#use fast_io(E)
#use fast_io(F)

#define FOSC 40000000


//typedef signed int8 BYTE;
#include <stdlib.h>
//#use rs232(baud=5, xmit=PIN_F2,rcv=PIN_H1,stream=k_slow,BRGH1OK)
//#use rs232(baud=10400, xmit=PIN_F2,stream=K_FAST,invert)
//#use rs232(baud=10400, rcv=PIN_H2,stream=K_FAST_IN)
#use rs232(baud=9600, xmit=PIN_C6,rcv=PIN_C7)
//#use rs232(baud=38400, xmit=PIN_C6,rcv=PIN_C7,stream=STDIN)

#define kbhit() kbhit2()
//#define getch() getch2()


#byte   TBLPTRU=0xFF8   
#byte   TBLPTRH=0xFF7 
#byte   TBLPTRL=0xFF6  
#byte   TABLAT=0xFF5 
#byte   TXREG= 0xFAD

#byte   EECON1=0xFA6
#byte   EECON2=0xFA7

#byte ADCON0=0xFC2
#byte ADCON1=0xFC1
#byte ADCON2=0xFC0
#byte ADCRESH=0xFC4
#byte ADCRESL=0xFC3

typedef unsigned int8 UBYTE;
typedef unsigned int8 UINT;
typedef signed int8 INT;
typedef char CHAR;
typedef unsigned int16 UWORD;

typedef signed int16 WORD;
typedef unsigned int32 UDWORD;
typedef signed int32 DWORD; 
typedef float  FLOAT; 

#include "ccs.h"

#define MAX_STRING 20
#define CLEAR_SCREEN() { \
		putch(27); \
		puts("[2J"); \
		putch(27); \
		puts("[f"); \
		}

#define Delay_us(x) delay_us(x)
#define Delay_ms(x) delay_ms(x)

#define NEW_LINE() {puts("\n\r");}
#define putch putchar
#define ClrWdt()
#define log_printf(x) printf(x)

#define MAKEWORD(x,y) ((((UWORD)x)<<8) | (UWORD)y)

#include "serial.h"

#endif //__DATATYPES_H
