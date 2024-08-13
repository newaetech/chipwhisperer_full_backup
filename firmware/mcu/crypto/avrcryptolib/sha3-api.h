/* sha3-api.h */

/**
 * \file    sha3-api.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-01-06
 * \license GPLv3 or later
 * 
 */
/*
 This file implements algortihm independent data types of the NIST API for
 SHA3 candidates as described in:
   http://csrc.nist.gov/groups/ST/hash/documents/SHA3-C-API.pdf
 .
*/

#ifndef SHA3_API_H_
#define SHA3_API_H_
typedef unsigned char BitSequence;
typedef unsigned long long DataLength;
typedef enum { SUCCESS = 0, FAIL = 1, BAD_HASHBITLEN = 2 } HashReturn;

#endif /* SHA3_API_H_ */
