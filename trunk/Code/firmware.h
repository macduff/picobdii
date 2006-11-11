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
 *  DATE : 9/26/2004  12:00:44 PM
 *
 *  FILENAME: firmware.h
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/

#ifndef __FIRMWARE_H
#define __FIRMWARE_H

#include "scanner.h"
#include "flash.h"

#define FIRM_TYPE 0x03
#define FIRM_FLASH_START ((UDWORD)(0x60000))
 
#define LOADER_ADDR 0xF500
#define LOADER_END 0xFFFF
 
typedef struct {
	UBYTE type; 
	UBYTE verMajor;
	UBYTE verMinor; 
	UBYTE filler; 	   //space filler
	UDWORD checkSum;
	UDWORD size;
} FIRM_HEADER; 

UINT FirmwareCheck(UDWORD flashAddr);
#separate
UBYTE FirmwareUpdate(UDWORD flashAddr);


#endif //__FIRMWARE_H

