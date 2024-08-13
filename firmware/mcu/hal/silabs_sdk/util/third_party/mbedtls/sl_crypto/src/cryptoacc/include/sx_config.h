#ifndef SX_CONFIG_H
#define SX_CONFIG_H

// Total size of the eSecure system RAM (in bytes).
#define ESEC_RAM_SIZE 0

// Size (in bytes) of the eSecure system ROM
#define ESEC_ROM_SIZE 0

// Width of ARADDR/AWADDR bus on external DMA
#define EXTDMA_AXI_ADDR_WIDTH 32

// Include BA411E AES engine
#define AES_ENABLED 1

// Support for ECB mode with AES cipher
#define AES_ECB_ENABLED 1

// Support for CBC mode with AES cipher
#define AES_CBC_ENABLED 1

// Support for CTR mode with AES cipher
#define AES_CTR_ENABLED 1

// Support for CFB mode with AES cipher
#define AES_CFB_ENABLED 1

// Support for OFB mode with AES cipher
#define AES_OFB_ENABLED 1

// Support for CCM and CCM* modes with AES cipher
#define AES_CCM_ENABLED 1

// Support for GCM and GMAC modes with AES cipher
#define AES_GCM_ENABLED 1

// Support for XTS mode with AES cipher
#define AES_XTS_ENABLED 0

// Support for CMAC mode with AES cipher
#define AES_CMAC_ENABLED 1

// Support counter-measures in AES engine
#define AES_CM_ENABLED 0

// Support for 128-bit key with AES cipher
#define AES_128_ENABLED 1

// Support for 192-bit key with AES cipher
#define AES_192_ENABLED 1

// Support for 256-bit key with AES cipher
#define AES_256_ENABLED 1

// Include BA412 DES engine
#define DES_ENABLED 0

// Include BA413 hash engine
#define HASH_ENABLED 1

// Include MD5 support in hash engine
#define MD5_ENABLED 1

// Include SHA1 support in hash engine
#define SHA1_ENABLED 1

// Include SHA224 support in hash engine
#define SHA224_ENABLED 1

// Include SHA256 support in hash engine
#define SHA256_ENABLED 1

// Include SHA384 support in hash engine
#define SHA384_ENABLED 1

// Include SHA512 support in hash engine
#define SHA512_ENABLED 1

// Include SM3 support in hash engine
#define SM3_ENABLED 0

// Include padding support in hash engine
#define HASH_PADDING_ENABLED 1

// Include HMAC support in hash engine
#define HMAC_ENABLED 1

// Number of multipliers in PK engine
#define PK_MULTIPLIERS 4

// Maximum operand size (bits) supported by PK engine
#define PK_MAX_OP_SIZE 521

// Include BA418 SHA3 engine
#define SHA3_ENABLED 0

// Include BA419 SM4 engine
#define SM4_ENABLED 0

// Support for ECB mode with SM4 cipher
#define SM4_ECB_ENABLED 0

// Support for CBC mode with SM4 cipher
#define SM4_CBC_ENABLED 0

// Support for CTR mode with SM4 cipher
#define SM4_CTR_ENABLED 0

// Support for CFB mode with SM4 cipher
#define SM4_CFB_ENABLED 0

// Support for OFB mode with SM4 cipher
#define SM4_OFB_ENABLED 0

// Support for GCM mode with SM4 cipher
#define SM4_GCM_ENABLED 0

// Support for AIS31 in random generator
#define AIS31_ENABLED 1

// Include micro-code for counter-measures
#define PK_CM_ENABLED 0

// Support Diffie-Hellman on finite field (with modular exponentiation)
#define DH_MODP_ENABLED 0

// Support Secure Remote Password protocol
#define SRP_ENABLED 0

// Support ECC with binary curves
#define ECC_BINARY_ENABLED 0

// Support ECC with Edwards curves
#define ECC_EDWARDS_ENABLED 0

// Support ECC with Montgomery curves
#define ECC_MONTGOMERY_ENABLED 0

// Support prime number generation
#define PRIME_GEN_ENABLED 0

// Support RSA
#define RSA_ENABLED 0

// Always-on battery power is available
#define ALWAYSON_ENABLED 1

// Support DSA
#define DSA_ENABLED 0

// Integrate PUF to generate SRK (PUF_TYPE=light)
#define PUF_LIGHT_ENABLED 0

// Integrate PUF to generate SRK (PUF_TYPE=light)
#define PUF_FLEX_ENABLED 0

// Maximum size (bytes) of symmetric keys (AES, ChaChaPoly...) and
// secrets stored by the host. Note that AES-XTS requires twice the
// maximum key size as SYM_MAX_KEY_SIZE.
#define SYM_MAX_KEY_SIZE 512

// Maximum size (bytes) of Diffie-Hellman keys
#define DH_MAX_KEY_SIZE 512

// Maximum size (bytes) of SRP keys
#define SRP_MAX_KEY_SIZE 512

// Maximum size (bytes) of RSA keys
#define RSA_MAX_SIZE 512

// Maximum size (bytes) of P parameter of DSA (up to 3072 bits according
// to FIPS 186-4)
#define DSA_MAX_SIZE_P 384

// Maximum size (bytes) of Q parameter of DSA (up to 256 bits according
// to FIPS 186-4)
#define DSA_MAX_SIZE_Q 32

// Maximum size (bytes) of generated prime
#define PRIME_MAX_SIZE 256

// Maximum size (bits) of ECC keys
#define ECC_MAX_KEY_SIZE_BITS 521

// Maximum size (bytes) of salt input used for DeriveKey command
#define DERIV_MAX_SALT_SIZE 512

// Maximum size (bytes) of info input used for DeriveKey command
#define DERIV_MAX_INFO_SIZE 512

// Size (bytes) of the authentication data for secure storage. Can be
// zero (no authorization data)
#define SEC_STOR_AUTH_SIZE 8

// Maximum number of items in the volatile secure storage
#define VOLATILE_MAX_ID 10

// Maximum number of items in the Anti-Replay table
#define ARTABLE_SIZE 30

// Control subsampling at output of ring oscillators (ring sampling
// frequency = ClkSec frequency / (RNG_CLKDIV+1))
#define RNG_CLKDIV 7

// Number of clock cycles after FIFO being full until TRNG goes to sleep.
// A negative value will keep the ring oscillators always running.
#define RNG_OFF_TIMER_VAL 0

// When the FIFO level (number of 128-bit block) becomes smaller or equal
// to RNG_FIFO_WAKEUP_LVL, the TRNG is restarted.
#define RNG_FIFO_WAKEUP_LVL 8

// Number of clock cycles to wait after startup before sampling data from
// the noise source
#define RNG_INIT_WAIT_VAL 512

// Number of 128-bit blocks of data from the noise source that are used
// as input to the conditioning function
#define RNG_NB_128BIT_BLOCKS 8

// Select DRBG bit-strength
#define ESEC_HASH_DRBG_SEC_STRENGTH 128

// size in bytes of optional OTP area reserved for the host
// (WriteOTP/ReadOTP commands)
#define OTPHOST_SIZE 8

// number of pub keys stored per manufacturer
#define OTP_NR_PUBKEY_MAN 2

// number of firmware root keys stored per manufacturer
#define OTP_NR_FRK 2

// PK Counter-measure - enable projective coordinate randomization for
// ECC
#define PK_CM_RANDPROJ_ECC 0

// PK Counter-measure - enable scalar randomization for ECC
#define PK_CM_RANDKE_ECC 0

// PK Counter-measure - enable modulus randomization for exponentiation
#define PK_CM_RANDPROJ_MOD 0

// PK Counter-measure - enable exponent randomization for exponentiation
#define PK_CM_RANDKE_MOD 0

// eSecure RAM size (in bytes) available for the bootloader (data and
// stack). The remaider of the RAM is used to store the firmware +
// authenticated part of the header. If the size of the firmware is too
// large, the firmware will be rejected.
#define ESEC_BL_RAM_SIZE 0

// eSecure RAM size (in bytes) needed for the bootloader stack.
#define ESEC_BL_STACK_SIZE 0

// eSecure RAM size (in bytes) needed for the firmware stack.
#define ESEC_FW_STACK_SIZE 0

// The first level watchdog checks periodically that the CPU responds to
// interrupt. This parameter gives the interval in clock cycles.
#define WD_LVL1_TIMEOUT 1000000

// The second level watchog checks that eSecure firmware returns to its
// main loop. This parameter gives a maximum delay for command
// processing. It is expressed as a number of first watchdog level
// periods. Longest command is RSA key generation.
#define WD_LVL2_TIMEOUT 10000

// Select authentication algorithms used for secure boot and for EK
// attestation (CFG_AUTH_ALGO=ed25519)
#define CFG_AUTH_ALGO AUTH_ALGO_ED25519

// Size (byte) of AES keys used for secure boot and secure storage
#define ESEC_ROOT_KEYS_SIZE 16

// Host RAM size available for host secure boot. Host RAM is used to
// store the firmware + authenticated part of the header. If the size of
// the firmware is too large, the firmware will be rejected.
#define HOST_FW_RAM_SIZE 0

// Enable Ed25519 curve to be used by the EdDSA algorithm.
#define ED25519_ENABLED 0

// Enable Ed448 curve to be used by the EdDSA algorithm.
#define ED448_ENABLED 0

// Enable countermeasures when signing with Ed25519.
#define ED25519_CM_ENABLED 0

// Enable SM2 support in PK.
#define SM2_ENABLED 0

// Include brainpool curves in predefined ECC curves
#define BRAINPOOL_ENABLED 0

#endif // SX_CONFIG_H