/* hexdigit_tab.S */
/*
 *   This file is part of AnonAccess, an access system which can be used
 *    to open door or doing other things with an anonymity featured
 *    account managment.
 *   Copyright (C) 2006, 2007, 2008  Daniel Otte (daniel.otte@rub.de)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
.global hexdigit_tab_P
hexdigit_tab_P:
.global hexdigit_tab_uc_P
hexdigit_tab_uc_P:
.byte '0','1','2','3'
.byte '4','5','6','7'
.byte '8','9','A','B'
.byte 'C','D','E','F'

.global hexdigit_tab_lc_P
hexdigit_tab_lc_P:
.byte '0','1','2','3'
.byte '4','5','6','7'
.byte '8','9','a','b'
.byte 'c','d','e','f'






