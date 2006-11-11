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
 *  DATE : 9/26/2004  1:56:11 PM
 *
 *  FILENAME: encrypt.c
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/

#include "encrypt.h"

/*******************************************************************
 *  FUNCTION: encrypt
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	encrypt.c	
 *  DATE		=   9/26/2004  2:02:45 PM
 *
 *  PARAMETERS:  input[2]. output[2], keys[4]	
 *
 *  DESCRIPTION: Encrypts the input and returns in output
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
void encrypt(UDWORD *input,UDWORD *output, UDWORD *keys)
{
   UDWORD y,z,sum,delta;
   UDWORD a,b,c,d,n;

  y=input[0];
  z=input[1];
 sum=0;
 delta=0x9E3779B9;
a=keys[0];
b=keys[1];
c=keys[2];
d=keys[3];
n=32;
   while(n-->0)
      {
      sum += delta;
      y += (z << 4)+a ^ z+sum ^ (z >> 5)+b;
      z += (y << 4)+c ^ y+sum ^ (y >> 5)+d;
      }

   output[0]=y; output[1]=z;
}

/*******************************************************************
 *  FUNCTION: decrypt
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	encrypt.c	
 *  DATE		=   9/26/2004  2:02:45 PM
 *
 *  PARAMETERS:  input[2]. output[2], keys[4]	
 *
 *  DESCRIPTION: Decrypts the input and returns in output
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
#org LOADER_ADDR
void decrypt(UDWORD *input,UDWORD *output, UDWORD *keys)
{
   UDWORD      y,z,sum,delta,a,b,
				c,d,n;

	y=input[0];
	z=input[1];
	sum=0xC6EF3720;
	delta=0x9E3779B9;
	a=keys[0];
	b=keys[1];
	c=keys[2];
	d=keys[3];
	n=32;
   /* sum = delta<<5, in general sum = delta * n */

   while(n-->0)
      {
      z -= (y << 4)+c ^ y+sum ^ (y >> 5)+d;
      y -= (z << 4)+a ^ z+sum ^ (z >> 5)+b;
      sum -= delta;
      }
   
   output[0]=y; output[1]=z;
}
