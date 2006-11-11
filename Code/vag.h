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
 *  DATE : 10/30/2004  8:48:00 AM
 *
 *  FILENAME: vag.h
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/

#ifndef __VAG_H
#define __VAG_H

#include "datatypes.h"
#include "iso.h"

#define VAG_CHAR_DELAY 5
#define VAG_CHAR_TIMEOUT 1000

#define VAG_BLOCK_DELAY 10
#define VAG_BLOCK_TIMEOUT 2000

UBYTE vagGetBlock(UBYTE *data, UBYTE len);
UBYTE vagPutBlock(UBYTE *data, UBYTE len);


#endif __VAG_H
