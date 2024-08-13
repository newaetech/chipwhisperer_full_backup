/* dbz_strings.h */
/*
 *   This file is part of AnonAccess, an access system which can be used
 *    to open door or doing other things with an anonymity featured
 *    account managment.
 *   Copyright (C) 2006, 2007, 2008, 2009  Daniel Otte (daniel.otte@rub.de)
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


#ifndef DBZ_STRINGS_H_
#define DBZ_STRINGS_H_

/** \file dbz_strings.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \license GPLv3 or later
 * \brief functions for handling of double-zero-terminated strings
 * 
 */

#include <stdint.h>
#include <avr/pgmspace.h>

/** \fn uint8_t dbz_strcount(const char* str)
 * \brief count stings in dbz-terminated string
 * 
 * Count the single zero terminated strings in a 
 * double terminated string
 * \param str pointer to the double-zero-terminated string
 */
uint8_t dbz_strcount(const char* str);

/** \fn void dbz_splitup(char* dbzstr, char** strings)
 * \brief split up a dbz-terminated string
 * 
 * Fills an array with pointers to the single terminated string
 * in a double-zero-terminated string
 * \param dbzstr pointer to the double-zero-terminated string
 * \param strings pointer to the array of strings (char pointers)
 */
void dbz_splitup(char* dbzstr, char** strings);

/** \fn uint8_t dbz_strcount_P(PGM_P str)
 * \brief count stings in dbz-terminated string in flash
 * 
 * Count the single zero terminated string in a 
 * double terminated string which is placed in flash (progmem)
 * \param str pointer to the double-zero-terminated string in flash
 */
uint8_t dbz_strcount_P(PGM_P str);

/** \fn void dbz_splitup_P(PGM_P dbzstr, PGM_P* strings)
 * \brief split up a dbz-terminated string in flash
 * 
 * Fills an array with pointers (PGM_P) to the single terminated string
 * in a double-zero-terminated string which is placed in flash
 * \param dbzstr  pointer to the double-zero-terminated string in flash
 * \param strings pointer to the array of strings (PGM_P)
 */
void dbz_splitup_P(PGM_P dbzstr, PGM_P* strings);

#endif /*DBZ_STRINGS_H_*/
