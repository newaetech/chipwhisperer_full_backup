/*
 * Copyright (c) 2014 NewAE Technology Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or other
 *   materials provided with the distribution.
 *
 * * Neither the name of the author nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
#include "circbuffer.h"

/*
    @brief Initializes the circular buffer.

    @param cbuf Points to the structure to be initialized.
*/
void init_circ_buf(tcirc_buf *cbuf)
{
    cbuf->head = cbuf->tail = 0;
    cbuf->dropped = 0;
}

/*
    @brief This will add a character to the circular buffer.

    @param cbuf Pointer to structure that contains data.
    @param ch Character to add to buffer.
*/
void add_to_circ_buf(tcirc_buf *cbuf, uint8_t ch, bool block)
{
    // Add char to buffer
    unsigned int newhead = cbuf->head;
    newhead++;
    if (newhead >= CIRCBUFSIZE)
        newhead = 0;
    while (newhead == cbuf->tail)
    {
        if (!block)
        {
            cbuf->dropped++;
            return;
        }
        
        //Add processing here?
        
    }

    cbuf->buf[cbuf->head] = ch;
    cbuf->head = newhead;
}

/*
    @brief This will get a character out of the circular buffer.

    @param cbuf Pointer to structure that contains data.

    @return retval Character to be returned from buffer.  If no char
    is available, returns SERIAL_ERR.
*/
uint8_t get_from_circ_buf(tcirc_buf *cbuf)
{
    // Get char from buffer
    // Be sure to check first that there is a char in buffer
    unsigned int newtail = cbuf->tail;
    uint8_t retval = cbuf->buf[newtail];

    if (newtail == cbuf->head)
        return SERIAL_ERR;

    newtail++;
    if (newtail >= CIRCBUFSIZE)
        // Rollover
        newtail = 0;
    cbuf->tail = newtail;

    return retval;
}

/*
    @brief This will determine if there is a character in the circular buffer.

    @param cbuf Pointer to structure that contains data.

    @retval true if buffer contains data.
    @retval false if buffer is empty.
*/
bool circ_buf_has_char(tcirc_buf *cbuf)
{
    // Return true if buffer empty
    unsigned int head = cbuf->head;
    return (head != cbuf->tail);
}

/*
   @brief Returns the number of stored chars in the circular buffer.

   @param cbuf is the circular buffer to query

   @return count Number of chars in buffer.
 */
unsigned int circ_buf_count(tcirc_buf *cbuf)
{
    int count;

    count = cbuf->head;
    count -= cbuf->tail;
    if (count < 0)
        count += CIRCBUFSIZE;
    return (unsigned int)count;
}

