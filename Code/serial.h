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

#ifndef __SERIAL_H
#define __SERIAL_H

#include "datatypes.h"

#define SERIAL_BUF	20 //size of serial buffer
#BYTE TXSTA=0xFAC
#BYTE RCSTA=0xFAB
#BYTE TXREG=0xFAD
#BYTE RCREG=0xFAE
#BYTE SPBRG=0xFAF
#BYTE SPBRGH=0xF7F
#BYTE BAUDCON=0xF7E


UBYTE rs232_init(UDWORD baud);
UBYTE getch2();
UBYTE kbhit2();
UBYTE putch2(UBYTE data);




#endif //__SERIAL_H