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
 *  DATE : 7/23/2004  10:46:47 AM
 *
 *  FILENAME: knob.h
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
#ifndef __KNOB_H
#define __KNOB_H

#include "datatypes.h"

#define KNOB_READ (PORTB>>4)	//reads knob and switches 

UINT KnobInit(void);
INT KnobRead(void);
UBYTE KnobSw1(void);
UBYTE KnobSw2(void);

#endif //__KNOB_H
