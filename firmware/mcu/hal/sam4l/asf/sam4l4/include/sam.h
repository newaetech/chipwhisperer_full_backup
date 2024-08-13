/**
 * \file
 *
 * \brief Top header file for SAM4L
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAM4L_
#define _SAM4L_

/**
 * \defgroup SAM4L_definitions SAM4L Device Definitions
 * \brief SAM4L CMSIS Definitions.
 */

#if   defined(__SAM4LC2A__) || defined(__ATSAM4LC2A__)
  #include "sam4lc2a.h"
#elif defined(__SAM4LC2B__) || defined(__ATSAM4LC2B__)
  #include "sam4lc2b.h"
#elif defined(__SAM4LC2C__) || defined(__ATSAM4LC2C__)
  #include "sam4lc2c.h"
#elif defined(__SAM4LC4A__) || defined(__ATSAM4LC4A__)
  #include "sam4lc4a.h"
#elif defined(__SAM4LC4B__) || defined(__ATSAM4LC4B__)
  #include "sam4lc4b.h"
#elif defined(__SAM4LC4C__) || defined(__ATSAM4LC4C__)
  #include "sam4lc4c.h"
#elif defined(__SAM4LS2A__) || defined(__ATSAM4LS2A__)
  #include "sam4ls2a.h"
#elif defined(__SAM4LS2B__) || defined(__ATSAM4LS2B__)
  #include "sam4ls2b.h"
#elif defined(__SAM4LS2C__) || defined(__ATSAM4LS2C__)
  #include "sam4ls2c.h"
#elif defined(__SAM4LS4A__) || defined(__ATSAM4LS4A__)
  #include "sam4ls4a.h"
#elif defined(__SAM4LS4B__) || defined(__ATSAM4LS4B__)
  #include "sam4ls4b.h"
#elif defined(__SAM4LS4C__) || defined(__ATSAM4LS4C__)
  #include "sam4ls4c.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM4L_ */
