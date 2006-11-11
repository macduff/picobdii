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
 *  DATE : 7/23/2004  10:47:33 AM
 *
 *  FILENAME: knob.c
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/

#include "knob.h"

//For the knob interface I designed the circuit board 
// such that the knob can be interrupt driven. That is
// when the user turns or presses the knob an interrupt is 
// triggered, thus allowing us not to miss a knob event. 
//
// The first thing we need to do is intilize the knob system

UBYTE KnobState=0x00;    //state of knob and switches

BYTE Knob; //counter as to number of turns on knob

UBYTE KnobSwitch=0;

/*******************************************************************
 *  FUNCTION: KnobInit
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	knob.c	
 *  DATE		=   7/23/2004  10:50:08 AM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
UINT KnobInit()
{
	//First make sure Knob and switch inputs are inputs
	//set_tris_b(0xFF); //all of port b is inputs
	
	//next lets set inilize the knob state
	KnobState=KNOB_READ;

	//next lets turn the KBIx interupt on
	bit_set(INTCON,3);

	return 0;
}

/*******************************************************************
 *  FUNCTION: KnobISR
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	knob.c	
 *  DATE		=   7/23/2004  10:54:26 AM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: Interrupt service routine for Knob and switches
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
#int_rb
void KnobISR()
{
	UBYTE state;
	UBYTE temp;

	//read current state
	state=KNOB_READ;

	//compare against previous state;
	temp=state ^ KnobState;

	temp=temp & 0x03;
	//OK lets see if we have a knob turning... 
	if (temp)
	{
		if (bit_test(KnobState,0)==bit_test(KnobState,1))
		{
			temp=temp ^ 0x03; 
		}

		if (temp==0x01)
			Knob++;
		else
			Knob--;
	}

	if (bit_test(PORTB,6)==0)
	{
		KnobSwitch=1;
	}

	KnobState=state;
	//clear ISR flag
	bit_clear(INTCON,0);
}


/*******************************************************************
 *  FUNCTION: KnobRead
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	knob.c	
 *  DATE		=   7/23/2004  11:19:39 AM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: Reads knob value and resets knob count
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
INT KnobRead()
{
	INT temp;

	temp=Knob;
	Knob=0;
	return temp;
}

/*******************************************************************
 *  FUNCTION: KnobSw1
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	knob.c	
 *  DATE		=   7/23/2004  11:20:38 AM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: read state of switch 1
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
UBYTE KnobSw1()
{
	return !bit_test(KnobState,2);
}

/*******************************************************************
 *  FUNCTION: KnobSw2
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	knob.c	
 *  DATE		=   7/23/2004  11:21:36 AM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: reads state of switch 2 
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
UBYTE KnobSw2()
{
	return !bit_test(KnobState,3);
}
		
