/* cubehash_rotates.S */
/*
    This file is part of the ARM-Crypto-Lib.
    Copyright (C) 2006-2010 Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

.global rotate7left
rotate7left:
	mov r0, r25
	mov r25, r24
	mov r24, r23
	mov r23, r22
	mov r22, r0
	ror r0
	ror r25
	ror r24
	ror r23
	ror r22
	ret

.global rotate11left
rotate11left:
	mov r0, r25
	mov r25, r24
	mov r24, r23
	mov r23, r22
	mov r22, r0
	rol r0
	rol r23
	rol r24
	rol r25
	rol r22
	rol r0
	rol r23
	rol r24
	rol r25
	rol r22
	rol r0
	rol r23
	rol r24
	rol r25
	rol r22
	ret


