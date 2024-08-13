/* circularbytebuffer.h */
/*
    This file is part of the AVR-circularbytebuffer.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \file     circularbytebuffer.h
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2009-07-24
 * \license  GPLv3 or later
 * \addtogroup  circularbytebuffer
 * \brief    declaration for circular byte buffer
 */
/*@{*/
#ifndef CIRCULARBYTEBUFFER_H_
#define CIRCULARBYTEBUFFER_H_

#include <stdint.h>
#include <stdlib.h>
#include "config.h"
 /**
  * \brief type holding the managment information for the buffer
  *
  * A variable of this type may hold all the information to control the buffer
  */
 typedef struct {
	 uint8_t buffer_size; /**< holds the amount of bytes which may be stored in the buffer */
	 uint8_t fillcount; /**< holds the amount of bytes actually stored in the buffer */
	 uint8_t* buffer; /**< pointer to the actual  buffer */
	 uint8_t* head; /**< pointer to the head of the buffer */
	 uint8_t* tail; /**< pointer to the tail of the buffer */
	 uint8_t* top; /**< pointer to the last free address in the buffer */
} circularbytebuffer_t;


#if CIRCULARBYTEBUFFER_NO_MALLOC==0
/** \brief buffer initialisation with automatic allocation
 *
 * This function initializes the given buffer context and allocates memory for
 * it by calling malloc.
 * \param buffersize size of the buffer to allocate
 * \param cb buffer context to be initialized
 */
uint8_t circularbytebuffer_init(uint8_t buffersize, circularbytebuffer_t* cb);
#endif
#if CIRCULARBYTEBUFFER_NO_INIT2==0
/** \brief buffer initialisation without automatic allocation
 *
 * This function initializes the given buffer context and uses the given buffer
 * for storage, so no malloc is needed.
 * \param buffersize size of the buffer
 * \param cb buffer context to be initialized
 * \param buffer buffer for the storage of data (you are responisble for allocation and freeing)
 */
void    circularbytebuffer_init2(uint8_t buffersize, circularbytebuffer_t* cb, void* buffer);
#endif
/** \brief
 *
 *
 */
uint16_t circularbytebuffer_get_lifo(circularbytebuffer_t* cb);
uint16_t circularbytebuffer_get_fifo(circularbytebuffer_t* cb);
uint8_t circularbytebuffer_append(uint8_t, circularbytebuffer_t* cb);
uint8_t circularbytebuffer_push(uint8_t, circularbytebuffer_t* cb);
uint8_t circularbytebuffer_cnt(circularbytebuffer_t* cb);
void circularbytebuffer_free(circularbytebuffer_t* cb);

/*@}*/
#endif /* CIRCULARBYTEBUFFER_H_ */
