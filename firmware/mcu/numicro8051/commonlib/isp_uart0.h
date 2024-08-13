#define TRUE       1
#define FALSE     0


// IAP commands: IAPCN values
// IAPB: 7:6
// FOEN: 5
// FOEN: 4
// FCTRL: 3:0
#define PAGE_ERASE_AP        0x22 // 00:1:0:0010, Page erase APROM
#define BYTE_READ_AP         0x00 // 00:0:0:0000, Byte read APROM
#define BYTE_PROGRAM_AP      0x21 // 00:1:0:0001, Program APROM
#define BYTE_READ_ID         0x0C // 00:0:0:1100, Device ID
#define PAGE_ERASE_CONFIG    0xE2 // 11:1:0:0010, Erase Config
#define BYTE_READ_CONFIG     0xC0 // 11:0:0:0000, Read Config
#define BYTE_PROGRAM_CONFIG  0xE1 // 11:1:0:0001, Program Config
#define READ_UID             0x04 // 00:0:0:0100, Unique ID
#define PAGE_ERASE_LD        0x62 // 01:1:0:0010, Page erase LDROM
#define BYTE_PROGRAM_LD      0x61 // 01:1:0:0001, Byte program LDROM
#define BYTE_READ_LD         0x40 // 01:0:0:0000, Byte read LDROM
#define READ_CID             0x0B // 00:0:0:1011, Company ID
