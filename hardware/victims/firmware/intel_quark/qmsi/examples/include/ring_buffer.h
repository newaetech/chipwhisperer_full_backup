/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

/**
 * Ring Buffer type.
 */
typedef struct {
	uint16_t *buffer; /**< Buffer given by the user. */
	uint16_t size;    /**< Size of the buffer. */
	uint16_t *head;   /**< Data to be written to. */
	uint16_t *tail;   /**< Data to be read from. */
	bool overflow;    /**< Overflow flag when head erases the tail */
} ring_buffer_t;

/**
 * Setup the Ring Buffer.
 *
 * Setup the ring buffer with the user provided
 * buffer to be used as well as its size.
 *
 * @param[in] rb Pointer to a ring_buffer_t.
 * @param[in] buffer Array for the ring buffer.
 * @param[in] size Size of the rng buffer.
 */
static __inline__ void setup_rb(ring_buffer_t *rb, uint16_t *buffer,
				uint32_t size)
{
	rb->buffer = buffer;
	rb->size = size;
	rb->head = rb->buffer;
	rb->tail = rb->buffer;
	rb->overflow = false;
}

/**
 * Get the position of the tail.
 *
 * Read the position of the tail to get
 * to position we read from.
 *
 * @param[in] rb Pointer to a ring_buffer_t.
 *
 * @return The tail position.
 */
static __inline__ uint16_t rb_get_tail_pos(ring_buffer_t *rb)
{
	return (rb->tail - rb->buffer) % rb->size;
}

/**
 * Get the next address in the ring buffer.
 *
 * Read the next address from the current one.
 *
 * @param[in] rb Pointer to a ring_buffer_t.
 * @param[in] pos Current address.
 *
 * @return The next address.
 */
static __inline__ uint16_t *rb_next(ring_buffer_t *rb, uint16_t const *pos)
{
	return rb->buffer + ((pos - rb->buffer + 1) % rb->size);
}

/**
 * Add one value in the ring buffer.
 *
 * Add one value at the head of the ring buffer.
 *
 * @param[in] rb Pointer to a ring_buffer_t.
 * @param[in] val value to be added.
 */
static __inline__ void rb_add(ring_buffer_t *rb, uint16_t val)
{
	uint16_t *next = rb_next(rb, rb->head);

	*rb->head = val;

	/*
	 * Head reached tail, we discarded one saved value
	 * Increase tail to reflect that
	 */
	if (next == rb->tail) {
		rb->tail = rb_next(rb, rb->tail);
		rb->overflow = true;
	}

	rb->head = next;
}

/**
 * Check if the ring buffer is empty.
 *
 * Check if the ring buffer is empty.
 *
 * @param[in] rb Pointer to a ring_buffer_t.
 *
 * @return True is the ring buffer is empty.
 */
static __inline__ bool rb_is_empty(ring_buffer_t *rb)
{
	if (rb->tail == rb->head) {
		return true;
	}

	return false;
}

/**
 * Get one value in the ring buffer.
 *
 * Get one value at the tail of the ring buffer.
 * Tail is moved forward after the read.
 *
 * @param[in] rb Pointer to a ring_buffer_t.
 * @param[in] val Address to store the value to.
 */
static __inline__ int rb_get(ring_buffer_t *rb, uint16_t *val)
{
	if (rb_is_empty(rb)) {
		return -EINVAL;
	}

	*val = *rb->tail;
	rb->tail = rb_next(rb, rb->tail);

	return 0;
}

#endif /* __RING_BUFFER_H__ */
