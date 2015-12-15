 /* CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at src/license_cddl-1.0.txt
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at src/license_cddl-1.0.txt
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * This file is based on the Mooltipass SmartCard.h file from  https://github.com/limpkin/mooltipass/blob/master/source_code/src/CARD/smartcard.h 
 *
 */

#ifndef AT88SC102_LOWLEVEL_H_
#define AT88SC102_LOWLEVEL_H_

#include <asf.h>
#include <stdint.h>

// DEFINES
#define MAN_FUSE    0x00
#define EC2EN_FUSE  0x01
#define ISSUER_FUSE 0x02

#ifndef FALSE
#define FALSE                   0
#define TRUE                    (!FALSE)
#endif

enum card_detect_return_t       {RETURN_CARD_NDET=0, RETURN_CARD_TEST_PB=1, RETURN_CARD_4_TRIES_LEFT=14,  RETURN_CARD_3_TRIES_LEFT=13,  RETURN_CARD_2_TRIES_LEFT=12,  RETURN_CARD_1_TRIES_LEFT=11, RETURN_CARD_0_TRIES_LEFT=10};
enum mooltipass_detect_return_t {RETURN_MOOLTIPASS_INVALID = 0, RETURN_MOOLTIPASS_PB = 1, RETURN_MOOLTIPASS_BLOCKED = 2, RETURN_MOOLTIPASS_BLANK = 3, RETURN_MOOLTIPASS_USER = 4, RETURN_MOOLTIPASS_0_TRIES_LEFT = 5, RETURN_MOOLTIPASS_1_TRIES_LEFT = 6, RETURN_MOOLTIPASS_2_TRIES_LEFT = 7, RETURN_MOOLTIPASS_3_TRIES_LEFT = 8, RETURN_MOOLTIPASS_4_TRIES_LEFT = 9};
enum pin_check_return_t {RETURN_PIN_OK = 10, RETURN_PIN_NOK_3=3, RETURN_PIN_NOK_2=2, RETURN_PIN_NOK_1=1, RETURN_PIN_NOK_0=0};
enum return_type_t {RETURN_NOK = -1, RETURN_OK = 0};
typedef int8_t RET_TYPE;

// Prototypes
uint8_t* readSMC(uint8_t nb_bytes_total_read, uint8_t start_record_index, uint8_t* data_to_receive);
void writeSMC(uint16_t start_index_bit, uint16_t nb_bits, uint8_t* data_to_write);
void eraseApplicationZone1NZone2SMC(uint8_t zone1_nzone2);
RET_TYPE securityValidationSMC(uint16_t code, uint8_t reset_after_send);
RET_TYPE firstDetectFunctionSMC(void);
void blowFuse(uint8_t fuse_name);
void smartcardPowerDelay(void);
void removeFunctionSMC(void);
void scanSMCDectect(void);
void setSPIModeSMC(void);
void initPortSMC(void);

// Macros

/*! \fn     isSmartCardAbsent(void)
*   \brief  Function used to check if the smartcard is absent
*   \note   This function should only be used to check if the smartcard is absent. It works because scanSMCDectect reports the
*           smartcard absent when it is not here during only one tick. It also works because the smartcard is always reported
*           released via isCardPlugged
*   \return RETURN_OK if absent
*/
static inline RET_TYPE isSmartCardAbsent(void)
{
	return RETURN_OK;
}

// Defines
#define SMARTCARD_FABRICATION_ZONE	0x0F0F
#define SMARTCARD_FACTORY_PIN		0xF0F0
#define SMARTCARD_DEFAULT_PIN		0xF0F0
#define SMARTCARD_AZ_BIT_LENGTH     512
#define SMARTCARD_AZ1_BIT_START     176
#define SMARTCARD_AZ1_BIT_RESERVED  16
#define SMARTCARD_MTP_PASS_LENGTH   (SMARTCARD_AZ_BIT_LENGTH - SMARTCARD_AZ1_BIT_RESERVED - AES_KEY_LENGTH)
#define SMARTCARD_MTP_PASS_OFFSET   (SMARTCARD_AZ1_BIT_RESERVED + AES_KEY_LENGTH)
#define SMARTCARD_AZ2_BIT_START     736
#define SMARTCARD_AZ2_BIT_RESERVED  16
#define SMARTCARD_MTP_LOGIN_LENGTH  (SMARTCARD_AZ_BIT_LENGTH - SMARTCARD_AZ2_BIT_RESERVED)
#define SMARTCARD_MTP_LOGIN_OFFSET  SMARTCARD_AZ2_BIT_RESERVED
#define SMARTCARD_CPZ_LENGTH        8

#endif /* AT88SC102_LOWLEVEL_H_ */