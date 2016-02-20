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

#ifndef AT88SC102_HIGHLEVEL_H_
#define AT88SC102_HIGHLEVEL_H_

#include "at88sc102_lowlevel.h"
#include <stdlib.h>

#define printSmartCardInfo()

/************ PROTOTYPES ************/
RET_TYPE writeToApplicationZoneAndCheck(uint16_t addr, uint16_t nb_bits, uint8_t* buffer, uint8_t* temp_buffer);
RET_TYPE checkAuthenticatedReadWriteAccessToZone1And2(void);
RET_TYPE checkAuthenticatedReadWriteAccessToZone1(void);
RET_TYPE checkAuthenticatedReadWriteAccessToZone2(void);
RET_TYPE setAuthenticatedReadWriteAccessToZone1(void);
RET_TYPE setAuthenticatedReadWriteAccessToZone2(void);
void setAuthenticatedReadWriteAccessToZone1and2(void);
void writeSecurityCode(volatile uint16_t* code);
RET_TYPE transformBlankCardIntoMooltipass(void);
uint8_t getNumberOfSecurityCodeTriesLeft(void);
uint8_t getNumberOfAZ2WritesLeft(void);
RET_TYPE cardDetectedRoutine(void);
void printSMCDebugInfoToUSB(void);
uint16_t readSecurityCode(void);
void eraseSmartCard(void);
void resetBlankCard(void);
void readApplicationZone1(uint8_t* buffer);
void writeApplicationZone1(uint8_t* buffer);
void readApplicationZone2(uint8_t* buffer);
void writeApplicationZone2(uint8_t* buffer);
uint8_t* readFabricationZone(uint8_t* buffer);
uint8_t* readIssuerZone(uint8_t* buffer);
void writeIssuerZone(uint8_t* buffer);
uint8_t* readSecurityCodeAttemptsCounters(uint8_t* buffer);
uint8_t* readCodeProtectedZone(uint8_t* buffer);
void writeCodeProtectedZone(uint8_t* buffer);
uint8_t* readApplicationZone1EraseKey(uint8_t* buffer);
void writeApplicationZone1EraseKey(uint8_t* buffer);
uint8_t* readApplicationZone2EraseKey(uint8_t* buffer);
void writeApplicationZone2EraseKey(uint8_t* buffer);
uint8_t* readMemoryTestZone(uint8_t* buffer);
void writeMemoryTestZone(uint8_t* buffer);
uint8_t* readManufacturerZone(uint8_t* buffer);
void writeManufacturerZone(uint8_t* buffer);
void writeManufacturerFuse(void);
void write_issuers_fuse(void);
void write_ec2en_fuse(void);

/*
                SMART CARD MEMORY MAP
Bit Address                 Description                 Bits    Words
0–15            Fabrication Zone (FZ)                   16      1
16–79           Issuer Zone (IZ)                        64      4
80–95           Security Code (SC)                      16      1
96–111          Security Code Attempts counter (SCAC)   16      1
112–175         Code Protected Zone (CPZ)               64      4
176–687         Application Zone 1 (AZ1)                512     32
688–735         Application Zone 1 Erase Key (EZ1)      48      3
736–1247        Application Zone 2 (AZ2)                512     32
1248–1279       Application Zone 2 Erase Key (EZ2)      32      2
1280–1407       Application Zone 2 Erase Counter (EC2)  128     8
1408–1423       Memory Test Zone (MTZ)                  16      1
1424–1439       Manufacturer’s Zone (MFZ)               16      1
1440–1455       Block Write/Erase                       16      1
1456–1471       MANUFACTURER’S FUSE                     16      1
1529            EC2EN FUSE (Controls use of EC2)        1
1552 - 1567     ISSUER FUSE                             161
AZ1 composition (bits): 16 reserved + 256 AES key + 240 MTP password
AZ2 composition (bits): 16 reserved + 496 MTP login
*/



#endif /* AT88SC102_HIGHLEVEL_H_ */