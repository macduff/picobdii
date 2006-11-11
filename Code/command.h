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

#ifndef __COMMAND_H
#define __COMMAND_H

#include "datatypes.h"

#define MAX_CMD_LENGTH 20
#define MAX_ARG_LENGTH 20
#define MAX_ARGS 10

#define CMD_PROMPT "OBDII>"
#define CMD_NO_ERROR 0

//we need to have a method of entering command state
UINT CommandBegin(); //enters the command line machine

#endif // __COMMAND_h
