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
 *  DATE : 7/30/2004  11:20:28 AM
 *
 *  FILENAME: ccs.h
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
#ifndef __CCS_H_
#define __CCS_H_


#pragma USE FAST_IO(A)
#pragma USE FAST_IO(B)
#pragma USE FAST_IO(C)
#pragma USE FAST_IO(D)
#pragma USE FAST_IO(E)
#pragma USE FAST_IO(F)
#pragma USE FAST_IO(G)
#pragma USE FAST_IO(H)
#pragma USE FAST_IO(J)

//Lets define the registers for the PIC18F8680... 
#pragma BYTE TRISA = 0xF92
#pragma BYTE TRISB = 0xF93
#pragma BYTE TRISC = 0xF94
#pragma BYTE TRISD = 0xF95
#pragma BYTE TRISE = 0xF96
#pragma BYTE TRISF = 0xF97
#pragma BYTE TRISG = 0xF98
#pragma BYTE TRISH = 0xF99
#pragma BYTE TRISJ = 0xF9A

#pragma BYTE PORTA = 0xF80
#pragma BYTE PORTB = 0xF81
#pragma BYTE PORTC = 0xF82
#pragma BYTE PORTD = 0xF83
#pragma BYTE PORTE = 0xF84
#pragma BYTE PORTF = 0xF85
#pragma BYTE PORTG = 0xF86
#pragma BYTE PORTH = 0xF87
#pragma BYTE PORTJ = 0xF88

#pragma BYTE MEMCON = 0xF9C

#pragma BYTE ADCON0 = 0xFC2
#pragma BYTE ADCON1 = 0xFC1
#pragma BYTE ADCON2 = 0xFC0
#pragma BYTE ADRESL = 0xFC3
#pragma BYTE ADRESH = 0xFC4

#pragma BYTE TMR1L  = 0xFCE
#pragma BYTE TMR1H  = 0xFCF
#pragma BYTE T1CON  = 0xFCD

#pragma BYTE TMR0=0xFD6
#pragma BYTE TMR0L  = 0xFD6
#pragma BYTE TMR0H  = 0xFD7
#pragma BYTE T0CON  = 0xFD5

#pragma BYTE TMR3=0xFB2
#pragma BYTE TMR3L=0xFB2
#pragma BYTE TMR3H=0xFB3
#pragma BYTE T3CON=0xFB1

#pragma BYTE INTCON = 0xFF2
#pragma BYTE INTCON2 = 0xFF1
#pragma BYTE INTCON3 = 0xFF0

#pragma BYTE PIE1=0xF9D
#pragma BYTE PIR1=0xF9E
#pragma BYTE PIE2=0xFA0
#pragma BYTE PIR2=0xFA1
#pragma BYTE IPR2=0xFA2
#pragma BYTE PIE3=0xFA3
#pragma BYTE PIR3=0xFA4
#pragma BYTE IPR3=0xFA5

#pragma BYTE SSPCON1=0xFC6
#pragma BYTE SSPSTAT=0xFC7

#pragma BYTE CVRCON=0xFB5
#pragma BYTE CMCON=0xFB4
#endif //__CCS_H_
