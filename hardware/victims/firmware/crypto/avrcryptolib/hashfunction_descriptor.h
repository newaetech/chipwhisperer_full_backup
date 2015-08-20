/* hashfunction_descriptor.h */

#ifndef HASHFUNCTION_DESCRIPTOR_H_
#define HASHFUNCTION_DESCRIPTOR_H_
#include <stdint.h>
#include <avr/pgmspace.h>

#ifndef VOID_FPT
#define VOID_FPT
typedef void(*void_fpt)(void);
#endif

typedef void(*hf_init_fpt)(void*);
typedef void(*hf_nextBlock_fpt)(void*, const void*);
typedef void(*hf_lastBlock_fpt)(void*, const void*, uint16_t);
typedef void(*hf_ctx2hash_fpt)(void*, void*);
typedef void(*hf_free_fpt)(void*);
typedef void(*hf_mem_fpt)(void*, const void*, uint32_t);


#define HFDESC_TYPE_HASHFUNCTION 0x02

typedef struct {
	/** typefield, always 2 for hash functions */
	uint8_t  type; /* 2 == hashfunction */
	/** flags, currently unused should be set to zero */
	uint8_t  flags;
	/** name, flash pointer to the name string */
	PGM_P    name;
	/** ctxsize_B, size of the hash context in bytes */
	uint16_t ctxsize_B;
	/** blocksize_b, size of an input block in bits */
	uint16_t blocksize_b;
	/** hashsize_b, size of the output hash value in bits */
	uint16_t hashsize_b;
	/** init, function pointer to the algorithms init function */
	hf_init_fpt init;
	/** nextBlock, function pointer to the algorithms nextBlock function */
	hf_nextBlock_fpt  nextBlock;
	/** lastBlock, function pointer to the algorithms lastBlock function */
	hf_lastBlock_fpt  lastBlock;
	/** ctx2hash, function pointer to the algorithms ctx2hash function */
	hf_ctx2hash_fpt   ctx2hash;
	/** free, function pointer to the algorithms free function or NULL if 
	 *  there is no such function */
	hf_free_fpt free;
	/** mem, function pointer to a function which hashes a message in RAM
	 *  completely or NULL if there is no such function */
	hf_mem_fpt mem;
} hfdesc_t; /* hashfunction descriptor type */

typedef struct{
	hfdesc_t* desc_ptr;
	void*     ctx;
} hfgen_ctx_t;

#endif /* HASHFUNCTION_DESCRIPTOR_H_ */

