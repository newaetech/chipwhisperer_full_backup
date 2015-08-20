/* string-extras.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

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
/**
 * \file	string-extras.h
 * \author  Daniel Otte 
 * \date    2006-05-16
 * \license	GPLv3 or later
 * 
 */

/** \fn stridentcnt_P(char* a, PGM_P b)
 * \brief counts the number of identic chars
 * 
 * This function compares the supplyed strings and returns the index of the 
 * first char where the strings differ.
 * \param a pointer to string in RAM
 * \param b pointer to string in Flash
 * \return index of the first char where \c a and \c b differ
 */
uint16_t stridentcnt_P(char* a, PGM_P b);

/** \fn firstword_length(char* s)
 * \brief compute the length of the first word in supllyed string
 * 
 * This function searches for the first whitespace in the string and returns the
 * number of chars before the first whitespace.
 * \param s string
 * \return number of chars in first word
 */
uint16_t firstword_length(char* s);

/** \fn strstrip(char* str)
 * \brief removes whitespace at the beginning and the end of a string
 * 
 * This function removes whitespaces at the end of a string.
 * \param str sting
 * \return pointer to the first non-whitespace char in string
 */
char* strstrip(char* str);
