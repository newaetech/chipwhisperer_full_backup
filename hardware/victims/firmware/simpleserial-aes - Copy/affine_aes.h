#include "compiler_abstraction.h"

#ifndef _AFFINE_AES_H_
#define _AFFINE_AES_H_

#define UCHARp unsigned char *
#define UCHAR  unsigned char
#define UINT  unsigned int

#ifndef ASM
/* Note: these structures are packed so that we have compatibility 
 * with the ASM layer
 */

// NOTE:
// The following structures are contextual working buffers: they contain
// temporary data that are specific to an AES encryption or decryption
// session context. They are kept in our global contexts so that our
// lower layer primitives are thread-safe and re-entrant.

typedef struct _PACKED 
{
	/* NOTE: the local counters MUST be kept as first fields of the structures: this is an
	 * assumption of the underlying ASM functions.
	 */
	UINT	key_counter1;			// Local counter 1
	UINT	key_counter2;			// Local counter 2
	UCHAR 	key_maskedState[16*11];	// Masked key
	UCHAR 	key_masksState[16*11];	// Table of masks for all the subkeys 
	UCHAR 	key_rin[4];				// Byte for boolean input masking for SubByte operation
	UCHAR 	key_rout[4];			// Byte for boolean output masking for SubByte operation
	UCHAR 	key_rmult[4];			// Multiplicative byte
	UCHAR 	key_gtab[16*16];		// Table of the Multiplicative operation by key_rmult
	UCHAR 	key_maskedSBox[16*16];	// Masked AES SBox
	UCHAR 	key_permIndicesMC[4];	// permIndices table
	UCHAR 	key_rcon_mult[40];		// Table for rcon multiplied by rmult
}  STRUCT_KEY_CONTEXT;
#endif

// Indexes of each variable in the  STRUCT_KEY_CONTEXT structure
#define key_ctx_maskedState     (8)
#define key_ctx_masksState      (key_ctx_maskedState + (16*11))
#define key_ctx_rin             (key_ctx_masksState + (16*11))
#define key_ctx_rout            (key_ctx_rin + (4))
#define key_ctx_rmult           (key_ctx_rout + (4))
#define key_ctx_gtab            (key_ctx_rmult + (4))
#define key_ctx_maskedSBox      (key_ctx_gtab + (16*16))
#define key_ctx_permIndicesMC   (key_ctx_maskedSBox + (16*16))
#define key_ctx_rcon_mult       (key_ctx_permIndicesMC + (4))

#ifndef ASM
/* This is a macro to handle a common structure declaration and ASM accessors */
typedef struct _PACKED
{
	/* NOTE: the local counters MUST be kept as first fields of the structures: this is an
	 * assumption of the underlying ASM functions.
	 */
	UINT   aes_counter1;			// Local counter 1
	UINT   aes_counter2;			// Local counter 2
	UCHAR  aes_GTab[16*16];			// Table of the multiplication by r_mult 
	UCHAR  aes_sboxMasked[16*16];	// Sbox masked 
	UCHAR  aes_masksState[16];		// Mask state
	UCHAR  aes_permIndices[16];		// Random permutation for shuffling 
	UCHAR  aes_permIndicesBis[16];	// Random permutation for shuffling 
	UCHAR  aes_permIndicesMC[4];	// Random permutation for shuffling 
	UCHAR  aes_permIndicesBisMC[4];	// Random permutation for shuffling 	
	UCHAR  aes_rin[4];				// Byte for boolean masking for SubByte operation
	UCHAR  aes_rout[4];				// Byte for boolean masking for SubByte operation
	UCHAR  aes_rmult[4];			// Multiplicative byte
	UCHAR  aes_state    [16];		// Table of the current aes state 
	UCHAR  aes_state2   [16];		// Table of the current aes state2 (masking state of dtate variable)
	UCHAR  aes_GTab_key [16*16];	// Multiplicative table by key_rmult
	UCHAR  aes_t_temp   [16];		// 
}  STRUCT_AES128_CONTEXT;
#endif

#define aes_ctx_GTab 	   (8)
#define aes_ctx_sboxMasked (aes_ctx_GTab + (16*16))
#define aes_ctx_masksState (aes_ctx_sboxMasked + (16*16))
#define aes_ctx_permIndices (aes_ctx_masksState + (16))
#define aes_ctx_permIndicesBis (aes_ctx_permIndices + (16))
#define aes_ctx_permIndicesMC (aes_ctx_permIndicesBis + (16))
#define aes_ctx_permIndicesBisMC (aes_ctx_permIndicesMC + (4))
#define aes_ctx_rin       (aes_ctx_permIndicesBisMC + (4))
#define aes_ctx_rout      (aes_ctx_rin + (4))
#define aes_ctx_rmult     (aes_ctx_rout + (4))
#define aes_ctx_state     (aes_ctx_rmult + (4))
#define aes_ctx_state2    (aes_ctx_state + (16))
#define aes_ctx_GTab_key  (aes_ctx_state2 + (16))
#define aes_ctx_t_temp    (aes_ctx_GTab_key + (16*16))

#ifndef ASM
UCHAR aes_loadKey (STRUCT_KEY_CONTEXT*, const UCHARp, UCHARp);
UCHAR aes_init_enc (STRUCT_AES128_CONTEXT*,UCHARp);
UCHAR aes_init_dec (STRUCT_AES128_CONTEXT*, UCHARp);
UCHAR aes_dec (STRUCT_AES128_CONTEXT*, STRUCT_KEY_CONTEXT*, const UCHARp, UCHARp); 
UCHAR aes_enc (STRUCT_AES128_CONTEXT*, STRUCT_KEY_CONTEXT*, const UCHARp, UCHARp); 
#endif

#endif /* _AFFINE_AES_H_ */
