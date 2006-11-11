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
 *  DATE : 11/5/2004  8:35:43 AM
 *
 *  FILENAME: ui.c
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/

#include "ui.h"

/*******************************************************************
 *  FUNCTION: EditNum
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	ui.c	
 *  DATE		=   11/5/2004  8:39:02 AM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: Edits a number on the LCD using knob
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
UBYTE EditNum(FLOAT *num, UBYTE mant, UBYTE dec)
{
	char str[16];
	
	sprintf(str,"%3.2f",*num);
	LCD_print2(str,1);
}
 