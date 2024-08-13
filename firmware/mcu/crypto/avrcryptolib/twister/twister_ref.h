/******************************************************************
 ** Header File for the Reference Implementation of Twister
 **
 ** Author: Ewan Fleischmann <ewan.fleischmann@uni-weimar.de> 2008
 **
 ** This algorithm and source code is released into the public domain.
 **
 *******************************************************************/

#ifndef TWISTER_REF_H_
#define TWISTER_REF_H_

#define DEBUG

#define MIN(a,b) ((a)>(b)?(b):(a))
/* state matrix has size of 8x8 BYTES i.e. 8 rows and 8 columns*/
#define NUMROWSCOLUMNS 8 
#define STATESIZE NUMROWSCOLUMNS * NUMROWSCOLUMNS

/* blocksize in BITS, i.e. size of an input message block for a compression function call*/
#define BLOCKSIZE 512

/* unit64 def for ANSIC/GNUC <-> MSVC */
#ifdef __GNUC__
#include <stdint.h>
#else
typedef unsigned __int64 uint64_t;
#endif

/* multiplication in F_256 */
#define MULT(a,b) (multab[a-1][b])

/* NIST requirements definitions */
typedef unsigned char BitSequence;
typedef uint64_t DataLength;
typedef enum {SUCCESS=0, FAIL=1, BAD_HASHBITLEN=2 } HashReturn;
typedef enum {FULL = 0, NOT_FULL = 1 } INTERMEDIATE_RESULT;

typedef struct {
  /* statematrix[Y][X] */ 
  unsigned char hs_State[NUMROWSCOLUMNS][NUMROWSCOLUMNS];

  /* matrix for checksum */ 
  unsigned char hs_Checksum[NUMROWSCOLUMNS][NUMROWSCOLUMNS];

  /* Processed message bits */
  DataLength hs_ProcessedMsgLen;

  /* Unprocessed message block */
  BitSequence hs_Data[STATESIZE];    

  /* size of data hs_Data, in bits*/
  uint64_t hs_DataBitLen;

  /* Twist counter, prevents slide attacks, counts the number of twist operations */
  uint64_t hs_Counter;

  /* output hash bit length, in bits */
  uint64_t hs_HashBitLen;

} hashState;


/******************************************************************/
/************************* N I S T    A P I ***********************/
/******************************************************************/

/* Initiate Twister */
HashReturn Init(hashState *state, int hashbitlen);

/* Process the supplied data */
HashReturn Update(hashState *state, const BitSequence *data, 
		  DataLength databitlen);

/* Perform any post processing and output filtering */
HashReturn Final(hashState *state, BitSequence *hashval);

/* Hash the supplied data and provie the resulting hash value */
HashReturn Hash(int hashbitlen, const BitSequence *data,
		DataLength databitlen, BitSequence *hashval); 


/* only for testing */
INTERMEDIATE_RESULT fill_intermediate_state( hashState *state, const BitSequence *data, DataLength *databitlen, DataLength *processed );
void twist( hashState *state, uint64_t *data );
void twist_mini_round( hashState *state );
void checksum( hashState *state, int col );


#endif /* TWISTER_REF_H_ */
