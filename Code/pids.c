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
 *  DATE : 8/6/2004  5:40:31 PM
 *
 *  FILENAME: pids.c
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
#include "pids.h"
#include "obdii.h"

//First thing is we need to have a byte array to hold supported PIDs

UBYTE pidSupported[8]={0}; //allow up to 256 PIDS


/*******************************************************************
 *  FUNCTION: 
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	pids.c	
 *  DATE		=   8/11/2004  4:30:55 PM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/

