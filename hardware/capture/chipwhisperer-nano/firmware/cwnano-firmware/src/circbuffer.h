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
#ifndef CIRCBUFFER_H
#define CIRCBUFFER_H


#include <stdbool.h>

#ifndef CIRCBUFSIZE
#define CIRCBUFSIZE 200
#endif

#define SERIAL_ERR 0xFF

#include <stdint.h>

typedef struct {
    volatile unsigned int head;
    volatile unsigned int tail;
    volatile unsigned int dropped;
    uint8_t buf[CIRCBUFSIZE];
} tcirc_buf;

void init_circ_buf(tcirc_buf * cbuf);
void add_to_circ_buf(tcirc_buf *cbuf, uint8_t ch, bool block);
bool circ_buf_has_char(tcirc_buf *cbuf);
uint8_t get_from_circ_buf(tcirc_buf *cbuf);
unsigned int circ_buf_count(tcirc_buf *cbuf);

#endif //CIRCBUFFER_H
