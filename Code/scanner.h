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

#ifndef __SCANNER_H
#define __SCANNER_H

//1.02 9-24-04 Changed the delay in PWM Trying to fix problem with Tarus
//1.03 9-30-04 fixed problem with Fords 
//1.04 10-11-04 added support for mulitple responses from PWM (pwwm_send)
//1.06 10-26-04 
//		Added support for firmware update
//		Added battery monitor
//		Added hardware version number automatic determination
//		Fixed ISO problem on version 3 hardware
#define VER_MAJOR 1
#define VER_MINOR 07

#define HW_VER getHwVer()	//Hardware Version
#define BAUD 9600

 //tris 
#define TRIS_F 0x78  //pin F7 is CS others are output as well
#define TRIS_J 0x00
#define TRIS_G 0x00

#define TRIS_A 0x00 
#define TRIS_A_3 0x01 

#define TRIS_D 0x00
#define TRIS_H 0x00
#define TRIS_C 0xC0


#define SET_TRIS() { \
	set_tris_f(TRIS_F); \
	set_tris_j(TRIS_J); \
	set_tris_g(TRIS_G); \
	if (HW_VER<3) set_tris_a(TRIS_A); else  set_tris_a(TRIS_A_3); \
	set_tris_d(TRIS_D); \
	set_tris_h(TRIS_H); \
	set_tris_c(TRIS_C);	\
	bit_clear(TRISC,0); \
	bit_clear(TRISC,1); \
	bit_set(TRISC,4); \
	}


#define LED_DTC(x) output_bit(PIN_J7,(x==0))
#define LED_CAT(x) output_bit(PIN_G3,(x==0))
#define LED_HEATED_CAT(x) output_bit(PIN_G4,(x==0))
#define LED_EVAP(x) output_bit(PIN_A3,(x==0))
#define LED_AIR(x) output_bit(PIN_A2,(x==0))
#define LED_O2(x) output_bit(PIN_J6,(x==0))
#define LED_HEATED_O2(x) output_bit(PIN_J5,(x==0))
#define LED_EGR(x) output_bit(PIN_J4,(x==0))
#define LED_AC(x) output_bit(PIN_A1,(x==0))
#define LED_MISFIRE(x) output_bit(PIN_C0,(x==0))


//#if HW_VER<3
#define LED_COMP(x) {if (HW_VER<3) output_bit(PIN_A0,(x==0)); else output_bit(PIN_H0,(x==0));}
#define LED_FUEL(x) {if (HW_VER<3) output_bit(PIN_C1,(x==0)); else output_bit(PIN_H1,(x==0));} 

/*
#else
#define LED_COMP(x) output_bit(PIN_H0,(x==0))
#define LED_FUEL(x) output_bit(PIN_H1,(x==0))
#endif
*/

#define PWR_PIN		PIN_H2
#define LED_PWR(x) output_bit(PIN_H2,(x==0))

#define CONTRAST(x) output_bit(PIN_C1,x)

UDWORD getSeconds();
UBYTE getHwVer();

#endif
