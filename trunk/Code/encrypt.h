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
 *  DATE : 9/26/2004  1:55:31 PM
 *
 *  FILENAME: encrypt.h
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/

#ifndef __ENCRYPT_H
#define __ENCRYPT_H

#include "datatypes.h"

//note
//input[2], output[2], keys[4]
// use tiny encryption algrothim http://www.simonshepherd.supanet.com/tea.htm

void encrypt(UDWORD *input,UDWORD *output, UDWORD *keys);
#separate
void decrypt(UDWORD *input,UDWORD *output, UDWORD *keys);

#endif //_ENCRYPT_H
