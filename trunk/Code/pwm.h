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

#ifndef __PWM_H
#define __PWM_H

#include "datatypes.h"

/////////////////////////////////////////////////////////
// Constants and stuff for PWM reading
////////////////////////////////////////////////////////
#define PWM_IN		(bit_test(CMCON,6)) //(input(PIN_F2))
#define PWM_IN_HIGH (PWM_IN)// & input(PIN_B4))
#define PWM_IN_LOW  (PWM_IN)// | input(PIN_H2) | input(PIN_H2))
#define PWM_OUT		PIN_H6

#define PWM_ENABLE(x) (output_bit(PIN_H7,x))

#define PWM_IDLE 0
#define PWM_MARK 1
#define PWM_EOD  2
#define PWM_EOF  3
#define PWM_IFS  4
#define PWM_SOF  5
#define PWM_DATA 6

#define PWM_TP1  	5 //8
#define PWM_TP1_MIN	4 //7
#define PWM_TP1_MAX	13 //13 //9	  //9-26-04 TBS changed to 16 due to delays 
#define PWM_TP2	 	12 //16
#define PWM_TP2_MIN	19 //16 //15
#define PWM_TP2_MAX	22 //17
#define PWM_TP3		29  //24
#define PWM_TP3_MIN	27 //23
#define PWM_TP3_MAX	33 //26
#define PWM_TP4		56 //48
#define PWM_TP4_MIN	48 //48 //47
#define PWM_TP4_MAX	64 //51
#define PWM_TP5		90 //72
#define PWM_TP5_MIN	87 //70
#define PWM_TP5_MAX 97 //77
#define PWM_TP6		96 //96
#define PWM_TP6_MIN 117 //94
#define PWM_TP6_MAX	255
#define PWM_TP7		30 //32
#define PWM_TP7_MIN	37 //30
#define PWM_TP7_MAX	43 //33
#define PWM_TP8		50 //40
#define PWM_TP8_MIN	47 //38
#define PWM_TP8_MAX 52 //41
#define PWM_TP9		150 //120
#define PWM_TP9_MIN	147 //118
#define PWM_TP9_MAX 255
#define PWM_TP10	60 //48
#define PWM_TP10_MIN	58 //47
#define PWM_TP10_MAX	64 //51
#define PWM_TP11_MIN	7 //6

UBYTE pwm_monitor();
UBYTE pwm_get(UBYTE *ptr, UBYTE count, UWORD time_out_ms);
UBYTE pwm_put(UBYTE *data, UBYTE count, UWORD time_out_ms);
UBYTE pwm_send(UBYTE *resp, UBYTE len, UBYTE *msg, UBYTE msg_len);

#endif //__PWM_H
