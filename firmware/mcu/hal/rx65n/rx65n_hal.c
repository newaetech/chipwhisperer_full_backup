#include "platform.h"
#include "rx65n_hal.h"

void platform_init(void)
{
    //unlock all registers
    volatile uint16_t prcr = (0xA5 << 8) | (0b1011);
    SYSTEM.PRCR.WORD = prcr;
}

void init_uart(void)
{
    SYSTEM.MSTPCRB.BIT.MSTPB31 = 0; //start SCI module
    MPC.PWPR.BIT.B0WI = 0U;
    MPC.PWPR.BIT.PFSWE = 1U;
    //multiplex
    MPC.P20PFS.BIT.PSEL = 0b001010;
    MPC.P21PFS.BIT.PSEL = 0b001010;

    //enable SCI bus
    BSC.CS4CR.BIT.BSIZE = 2;
    BSC.CS4CR.BIT.EXENB = 1;


    PORT2.PMR.BIT.B0 = 1;
    PORT2.PMR.BIT.B1 = 1;

    PORT2.PDR.BIT.B0 = 1;
    // PORT2.PMR.BIT.B1 = 1;
    SCI0.SCR.BIT.CKE = 0x00;
    //enable tx and rx

#if SS_VER == SS_VER_2_0
    SCI0.BRR = 0; //divide by 1
    SCI0.SMR.BIT.CKS = 0b10; //PCLKB / 8  works out well for 38400bps I think??? Should be about 39000bps
#else
    SCI0.BRR = 3 - 1; //divide by 3
    SCI0.SMR.BIT.CKS = 0b10; //PCLKB / 8  works out well for 38400bps I think??? Should be about 39000bps
#endif
    SCI0.SCR.BIT.RE = 1;
    SCI0.SCR.BIT.TE = 1;

    //SCI should be default 8n1
    /* Disable writing to MPC pin function control registers */
    // MPC.PWPR.BIT.PFSWE = 0U;
    // MPC.PWPR.BIT.B0WI = 1U;
}

void trigger_setup(void)
{
    // maybe this works?
    PORTA.PDR.BIT.B0 = 1;
}

void trigger_high(void)
{
    PORTA.PODR.BIT.B0 = 1;

}

void trigger_low(void)
{
    PORTA.PODR.BIT.B0 = 0;
}

char getch(void)
{
    while (!SCI0.SSR.BIT.RDRF) {
        if (SCI0.SSR.BIT.ORER) 
            SCI0.SSR.BIT.ORER = 0;
    }
    return SCI0.RDR;
}

void putch(char c)
{
    SCI0.TDR = c;
    while (!SCI0.SSR.BIT.TDRE);
}

#if (CRYPTO_TARGET==HWAES)
#include "r_tsip_rx_if.h"
#ifndef KEY_DATA_H_
    #define KEY_DATA_H_

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/** Life cycle status **/
#define LIFECYCLE_STATE_BLANK             (0)
#define LIFECYCLE_STATE_ON_THE_MARKET     (1)
#define LIFECYCLE_STATE_UPDATING          (2)

/** Key block data address **/
#define KEY_BLOCK_DATA           (0x00100000)
#define KEY_BLOCK_DATA_MIRROR    (0x00104000)

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
/** Firmware update data and user key datas */
typedef struct key_block_data
{
    /** State management data for update firmware */
    struct
    {
        uint32_t                         user_program_max_cnt;
        uint32_t                         lifecycle_state;
        uint32_t                         program_mac0[R_TSIP_AES_BLOCK_BYTE_SIZE / sizeof(uint32_t)];
        uint32_t                         program_mac1[R_TSIP_AES_BLOCK_BYTE_SIZE / sizeof(uint32_t)];
    }
    firmware_update_control_data;

    /** User key datas */
    struct
    {
        uint8_t                          encrypted_session_key[R_TSIP_AES_CBC_IV_BYTE_SIZE * 2];
        uint8_t                          iv[R_TSIP_AES_CBC_IV_BYTE_SIZE];
        uint8_t                          encrypted_user_aes128_key[R_TSIP_AES128_KEY_BYTE_SIZE + 16];
        tsip_aes_key_index_t             user_aes128_key_index;
    }
    key_data;

    uint8_t                              hash_sha1[R_TSIP_SHA1_HASH_LENGTH_BYTE_SIZE];
} st_key_block_data_t;

/***********************************************************************************************************************
 Exported global variables
 **********************************************************************************************************************/
extern const st_key_block_data_t g_key_block_data;
extern const st_key_block_data_t g_key_block_data_mirror;
extern st_key_block_data_t       g_key_block_image;

extern const uint32_t s_flash[];

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

#endif /* KEY_DATA_H_ */


tsip_aes_handle_t state;
tsip_aes_key_index_t key_idx;
const st_key_block_data_t g_key_block_data =
{
    /* struct firmware_update_control_data; */
    {
        /* uint32_t user_program_max_cnt; */
        0,
        /* uint32_t lifecycle_state; */
        LIFECYCLE_STATE_BLANK,
        /* uint32_t program_mac0[R_TSIP_AES_BLOCK_BYTE_SIZE / sizeof(uint32_t)]; */
        {
            0
        },
        /* uint32_t program_mac1[R_TSIP_AES_BLOCK_BYTE_SIZE / sizeof(uint32_t)]; */
        {
            0
        },
    },
    /* struct key_data; */
    {
        /* uint8_t encrypted_session_key[R_TSIP_AES_CBC_IV_BYTE_SIZE * 2]; */
        {
            0xDF, 0x74, 0x1B, 0x60, 0x58, 0xAF, 0xDF, 0xE2, 0xDD, 0x18, 0x6E, 0xA0, 0xAF, 0x16, 0x84, 0x53,
            0xF2, 0xB1, 0xF3, 0x11, 0xAB, 0xAF, 0x61, 0x33, 0xAC, 0x68, 0x92, 0x6B, 0x0A, 0x47, 0x51, 0x30
        },
        /* uint8_t iv[R_TSIP_AES_CBC_IV_BYTE_SIZE]; */
        {
            0x93, 0x47, 0xE6, 0x6C, 0x27, 0x09, 0x2F, 0xB0, 0xDE, 0x3C, 0x3A, 0x2B, 0xA1, 0xDB, 0xD1, 0xD5
        },
        /* uint8_t encrypted_user_aes128_key[R_TSIP_AES128_KEY_BYTE_SIZE + 16]; */
        {
            0x24, 0x7A, 0xC4, 0xB3, 0x63, 0x68, 0xB2, 0x29, 0xB7, 0x0B, 0x9D, 0x1B, 0x51, 0x85, 0xD0, 0x83,
            0xDD, 0x75, 0xEE, 0x66, 0x9B, 0xF0, 0x51, 0xD0, 0x7A, 0x9E, 0x41, 0xFC, 0xB6, 0xA4, 0x8B, 0x45
        },
        /* tsip_aes_key_index_t user_aes128_key_index; */
        {
            0
        },
    },
    /* uint8_t hash_sha1[R_TSIP_SHA1_HASH_LENGTH_BYTE_SIZE]; */
    {
        0x64, 0x3D, 0xA7, 0x61, 0x12, 0x06, 0x28, 0x06, 0x7F, 0x6D, 0x9C, 0xD5, 0x8D, 0xC4, 0x39, 0x64,
        0x2D, 0x75, 0x3F, 0x49
    },
};
const st_key_block_data_t g_key_block_data_mirror =
{
    0
};

st_key_block_data_t g_key_block_image =
{
    0
};

const uint32_t s_flash[] =
{
    0xa6f0651c, 0xa17c5a15, 0xa23e1bfb, 0xc04e1bb9,
    0x5594fc81, 0x67fad158, 0xfda808ae, 0x7e01cb88,
    0xbf12d558, 0x9ec08adc, 0x21c25af2, 0x60d43062,
    0x82e6b470, 0x178879a6, 0x9ddb263f, 0xddb57b53,
    0x220ea793, 0x24de7b88, 0x9fa846e4, 0xdf9059f9,
    0x7ce55a19, 0x686689e5, 0x9aaef400, 0x88fd178a,
    0xf4e33b46, 0xc9394a88, 0x712823b9, 0xca75513f,
    0x63859e61, 0x45477873, 0x357b5776, 0x83cc1def,
    0x74cb65ab, 0xa919863c, 0x9f75e62e, 0x5fd62143,
    0xdbae440a, 0x34053525, 0x56e221e1, 0x8ffbaeb5,
    0xa75c55f0, 0x34727e44, 0x2c791463, 0x7670923f,
    0xc0287d97, 0x0a09b5c9, 0xfaecf18e, 0x09ceab85,
    0x687ad46f, 0x7e4d8adb, 0x6def5893, 0x6f236da3,
    0xab6e15e1, 0x653f41d0, 0x05652571, 0x9ec8ec15,
    0x2d4acb06, 0x7d5c2c26, 0xf49455cb, 0x9872dc50,
    0xb9fe50a2, 0x34bf45ae, 0x4cf2b6bf, 0xe1c75c7b,
    0x6e23718f, 0x227b0a55, 0x3a5e8b00, 0x83222dba,
    0x4041008f, 0x40fc8d01, 0xcd6c5c64, 0x0b8183b5,
    0x678bf9e7, 0x57844b52, 0xb4c81735, 0x559e77f3,
    0xb2b6800e, 0x715de4e2, 0x7a2720bb, 0x7b434710,
    0xee264103, 0x9db8c751, 0x78291c62, 0x77b883f4,
    0xa27d1216, 0x4e733ba6, 0x8a5f40dc, 0x32d2dd82,
    0x8a5fdc67, 0xd8fb0926, 0x9d5aec51, 0x08bfce4d,
    0x2a54839b, 0xe6601069, 0x564fbdbf, 0x9bb43dc7,
    0x9bb59d3a, 0xc3aaa60c, 0x2f2e75d7, 0x6a953972,
    0x6de4fd23, 0x546c212b, 0xe8aad33f, 0xca416c37,
    0xa74b36f9, 0x520330f0, 0x96145828, 0x09c21110,
    0x0b29365a, 0xfe9a9e60, 0x82b3a215, 0x752daa46,
    0x45bd59d6, 0x145ba47f, 0x75e40f92, 0x2f904860,
    0x609e3b0a, 0x8e6e6aa8, 0xe88ea1c3, 0x22a1db60,
    0x9947e0c7, 0x28416ca7, 0x3cb6abe6, 0x0e367da7,
    0x17b16976, 0x5323ccde, 0xc7337459, 0xf07293ad,
};


void HW_AES128_Init(void)
{
    //R_TSIP_Aes128EcbEncryptInit(&state, &key_idx);
    volatile e_tsip_err_t x;
    x = R_TSIP_Open(NULL, NULL);
}

void HW_AES128_LoadKey(uint8_t* key)
{
    // key_idx.type = TSIP_KEY_INDEX_TYPE_AES128;
    // uint8_t *keymem = (void *)&key_idx.value;
    // for (int i = 0; i < 16; i++){
    //     keymem[i] = key[i];
    //     keymem[i+16] = key[i];
    //     keymem[i+32] = key[i];
    // }
    volatile e_tsip_err_t x;
    x = R_TSIP_GenerateAes128KeyIndex(g_key_block_data.key_data.encrypted_session_key, g_key_block_data.key_data.iv,
    g_key_block_data.key_data.encrypted_user_aes128_key, &key_idx);

}

void HW_AES128_Enc_pretrigger(uint8_t* pt)
{
    volatile e_tsip_err_t x;
    x = R_TSIP_Aes128EcbEncryptInit(&state, &key_idx);
}

void HW_AES128_Enc(uint8_t* pt)
{
    uint8_t ct[16];
    uint32_t dummy;
    R_TSIP_Aes128EcbEncryptUpdate(&state, pt, ct, 16);
    for (int i = 0; i < 16; i++) {
        pt[i] = ct[i];
    }
}

void HW_AES128_Enc_posttrigger(uint8_t* pt)
{
    uint8_t ct[16];
    uint32_t dummy;
    R_TSIP_Aes128EcbEncryptFinal(&state, ct, &dummy);
}

void HW_AES128_Dec(uint8_t *pt)
{
}
#endif