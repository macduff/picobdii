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

#ifndef __ISO_H
#define __ISO_H

#include "datatypes.h"

#define L_OUT PIN_H4
#define K_OUT PIN_F0
#define K_LOW 	(output_high(K_OUT))
#define K_HIGH 	(output_low(K_OUT))
#define L_LOW	(output_high(L_OUT))
#define L_HIGH	(output_low(L_OUT))
#define K_IN  	(bit_test(CMCON,7))		//(VPW_IN)

#define ISO_W0_MIN 	2 //2ms
#define ISO_W1_MAX	((UWORD)300) //ms
#define ISO_W2_MAX	20 //ms
#define ISO_W4_MIN	25 //ms
#define ISO_W4_MAX	50 //ms

#define ISO_P3_MIN	55 //ms	 
#define ISO_P3_MAX	((UWORD)3000) //ms	
#define ISO_P4_MIN	5 //ms

#define ISO_P2_MAX  50 //ms  changed to 100 for testing
#define ISO_TINIL	25 //ms
#define ISO_TWUP	50 //ms

#define ISO_10400_BAUD 120

UBYTE iso_send(UBYTE *resp, UBYTE len, UBYTE *msg, UBYTE msg_len, UBYTE address);
UBYTE iso_get(UBYTE *data, UBYTE len, UWORD time_out_ms);
UBYTE iso_put(UBYTE *data, UBYTE len, UWORD idle_wait);

UBYTE iso_monitor();
UWORD iso_5baud(UBYTE *keys, UBYTE len, UBYTE address);

UDWORD iso_Baud(); //returns baudrate

#endif __ISO_H
