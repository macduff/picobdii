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

#ifndef __VPW_H
#define __VPW_H

#define SCALE	1.25
#include "datatypes.h"

#define VPW_IN	(bit_test(CMCON,6))//(input(PIN_H2))
#define VPW_IN_HIGH	(VPW_IN )
#define VPW_IN_LOW	(VPW_IN )

#define VPW_OUT	PIN_H5

#define VPW_UNKNOWN 0
#define VPW_IDLE 	1
#define VPW_ACTIVE 	2

#define VPW_THRESHOLD ((UWORD)(96*SCALE))

#define VPW_DATA_TIME_OUT 

#define TV1_MIN 34	 //us
#define TV1		66 //64	 //us
#define TV1_MAX	96
#define TV2_MIN	96
#define TV2		146 //128
#define TV2_MAX	163
#define TV3_MIN	((UWORD)(163*SCALE)) //us
#define TV3		((UWORD)(192*SCALE))
#define TV3_MAX	 ((UWORD)(239*SCALE))
#define TV4_MIN	((UWORD)(239*SCALE))
#define TV4		((UWORD)(280*SCALE))
#define	TV5_MIN	((UWORD)(239*SCALE))
#define TV5		300
#define TV6_MIN	280
#define TV6		300


UBYTE vpw_get(UBYTE *data, UBYTE len, UWORD time_out_ms);
UBYTE vpw_put(UBYTE *data, UBYTE len, UWORD time_out_ms);
UBYTE vpw_send(UBYTE *resp, UBYTE len, UBYTE *msg, UBYTE msg_len);

UINT vpw_montior();  

#endif //__VPW_H
