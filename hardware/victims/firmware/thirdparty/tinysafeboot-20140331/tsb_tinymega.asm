;***********************************************************************
;***********************************************************************
;***********************************************************************
; tinysafeboot - the universal bootloader for ATtinys and ATmegas
;***********************************************************************
;***********************************************************************
;***********************************************************************
;
;-----------------------------------------------------------------------
; Written in 2011-2014 by Julien Thomas (Germany)
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 3
; of the License, or (at your option) any later version.
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty
; of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
; See the GNU General Public License for more details.
; You should have received a copy of the GNU General Public License
; along with this program; if not, see:
; http://www.gnu.org/licenses/
;-----------------------------------------------------------------------
;
;***********************************************************************
; ADJUSTMENTS FOR INDIVIDUAL ASSEMBLY
;***********************************************************************
;
; This Sourcecode is directly compatible to: AVRASM2, GAVRASM*
;
; *) Newer devices may not be directly supported
;
.nolist
;
;-----------------------------------------------------------------------
; SPECIFY TARGET AVR
;-----------------------------------------------------------------------
;
; Comment in and provide def.inc file for target device
;
; [Examples]
;
;.include "tn2313def.inc"
;.include "tn85def.inc"
;.include "m8515def.inc"
;.include "m168def.inc"
;.include "m324Adef.inc"
;.include "tn441def.inc"
;.include "tn167def.inc"
;.include "tn861def.inc"
;.include "tn841def.inc"

;
; [...]
;

;-----------------------------------------------------------------------
; BUILD INFO
;-----------------------------------------------------------------------
; YY = Year - MM = Month - DD = Day
.set    YY      =       14
.set    MM      =        3
.set    DD      =       31

;.set    BUILDSTATE      = 0b11110000    ; version management option
.set BUILDSTATE = $F0

;-----------------------------------------------------------------------
; TSB / TSB-INSTALLER SWITCH
;-----------------------------------------------------------------------
; 0 = Regular assembly to target address
; 1 = TSB-Installer (ATtinys only)
;
.set    TSBINSTALLER = 1

;-----------------------------------------------------------------------
; PORTS
;-----------------------------------------------------------------------
;
; Important note: These are the defaults for making database template
;
.equ    RXPORT  = PORTB
.equ    RXPIN   = PINB
.equ    RXDDR   = DDRB
.equ    RXBIT   = 0
.equ    TXPORT  = PORTB
.equ    TXDDR   = DDRB
.equ    TXBIT   = 1

;-----------------------------------------------------------------------
;
;
;
;***********************************************************************
; DIRECTIVES AND DEFINITIONS FOR DIFFERENT ASSEMBLY OPTIONS
;***********************************************************************
;
; Autodetect ATtiny / ATmega and set TINYMEGA switch accordingly

.ifndef RWW_START_ADDR
        .equ TINYMEGA = 0
        .message "DETECTED ATTINY DEFINITIONS."
.else
        .if RWW_START_ADDR == RWW_STOP_ADDR
        .equ TINYMEGA = 0
        .message "DETECTED ATTINY DEFINITIONS."
        .else
        .equ TINYMEGA = 1
        .message "DETECTED ATMEGA DEFINITIONS."
        .endif
.endif

;-----------------------------------------------------------------------

.if FLASHEND > ($7fff)
        .error "SORRY! Devices over 64 KB not supported at the time."
        .exit
.endif

;-----------------------------------------------------------------------
;
; Workarounds for some newer devices with renamed or missing definitions
;
.ifndef SPMCSR                  ; SPMEN / PGERS / ...
        .equ SPMCSR = SPMCR
.endif

.ifndef MCUSR                   ; PORF / EXTRF / BORF / WDRF
        .equ MCUSR = MCUCSR
.endif

; Detect Attiny441/841 to amend missing definition of PAGESIZE
;
.set FOURPAGES = 0
.if SIGNATURE_000 == $1E
        .if SIGNATURE_002 == $15
                .if SIGNATURE_001 == $92 || SIGNATURE_001 == $93
                    .equ PAGESIZE = 32
                    .set FOURPAGES = 1
                    .message "ATTINY441/841: 4-PAGE-ERASE MODE"
                .endif
        .endif
.endif

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
;-----------------------------------------------------------------------


; Command and confirmation characters
.equ    REQUEST         = '?'           ; request / answer / go on
.equ    CONFIRM         = '!'           ; confirm / attention

; Current bootloader version
.equ    BUILDDATE   = YY * 512 + MM * 32 + DD

; Programming Presets
.equ    INFOLEN         = 8              ; exact words of device info

; Absolute Address constants (WORDS) for assembly of TSB

.if TINYMEGA == 0

; TSB at target location on ATtinys
.equ    TSBLEN          = ((280 / PAGESIZE)+1) * PAGESIZE
.equ    BOOTSTART       = (FLASHEND+1) - PAGESIZE - TSBLEN
.equ    LASTPAGE        = BOOTSTART + TSBLEN
.equ    DEVICEINFO      = BOOTSTART + TSBLEN - INFOLEN

; Start address of temporary TSB code for TSB-Installer (only ATtinys)
.equ    TBOOTSTART      = $00C0
.equ    TLASTPAGE       = TBOOTSTART + TSBLEN
.equ    TDEVICEINFO     = TLASTPAGE - INFOLEN
.endif

.if TINYMEGA == 1

; TSB at target location on ATmegas
.equ    BOOTSTART       = (FLASHEND+1)-256      ; = 512 Bytes
.equ    LASTPAGE        = BOOTSTART - PAGESIZE  ; = 1 page below TSB!
.endif

; SRAM buffer and stack
.equ    BUFFER          = SRAM_START

; Registers (in use by TSB-Firmware  AND  TSB-Installer)
.def    avecl   = r4                    ; application vector temp low
.def    avech   = r5                    ; application vector temp high
.def    bclkl   = r6                    ; baudclock low byte
.def    bclkh   = r7                    ; baudclock high byte

.def    tmp1    = r16                   ; universal
.def    tmp2    = r17                   ; temporary
.def    tmp3    = r18                   ; registers
.def    bcnt    = r19                   ; page bytecounter

;-----------------------------------------------------------------------
;
; we may assemble TSB directly for target address area (ATtinys/ATmegas)
; or make the TSB-Installer that's beginning at Flashstart (ATtinys)

.if TINYMEGA == 0
.message "START ASSEMBLY OF TSB FOR ATTINY."

.if TSBINSTALLER == 1
.message "Assembly of TSB-Installer."


        .if FOURPAGES == 0
                .set RPAGESIZE = PAGESIZE
        .else
                .set RPAGESIZE = PAGESIZE / 4
        .endif

;***********************************************************************
; START OF TSB-INSTALLER FOR ATTINYS
;***********************************************************************
; TSB-Installers may be loaded via ISP but also by any bootloader
; already residing on the target ATtiny. With first activation,
; the TSB-Installer will check its own integrity and completeness,
; then remove reset vector and erase old bootloader from upper memory
; locations. It will then transfer the brought-along new TSB-Firmware
; to its target location and finally remove itself from memory.
;
; NOTE: This update mechanism is applicable only on ATtinys.
;
; The process evolves in 5 levels:
;
; 1. check if the program was uploaded true and completely.
; 2. remove modified reset-jump at $0000 and thus reinstate itself
;    for being the one and only reset target.
; 3. erase previous bootloader code.
; 4. write and verify new code into upper flash with insistent retries
;    on errors, reset or power failure, until success.
; 5. cleanup (self-erase) and handover to new bootloader.
;
;-----------------------------------------------------------------------
; Registers (exclusively in use by TSB-Installer)
;-----------------------------------------------------------------------
;
.def    pcnt    = r20                   ; page counter
.def    checkl  = r21
.def    checkh  = r22                   ; checksum counter

.def    tslo    = r8                    ; temp current source address
.def    tshi    = r9
.def    ttlo    = r10                   ; temp current target address
.def    tthi    = r11
.def    bflo    = r12                   ; start address of buffer
.def    bfhi    = r13

;-----------------------------------------------------------------------
; 1st PAGE: RESET-JUMP / JUMP TO LEVEL 1 (INTEGRITY CHECK)
;-----------------------------------------------------------------------

.org $0000                      ; Installer starts like ordinary App

.if FLASHEND >= ($1fff)
        .message "Using JMP for Reset Vector."
        jmp  TL1CHECK
.else
        .message "Using RJMP for Reset Vector."
        rjmp TL1CHECK
.endif

.org (RPAGESIZE-2)
TL1CHECK:
rjmp L1CHECK                            ; LEVEL 1 start with selftest
;rjmp TTRESET                           ; circumvent selftest
.db $00, $00                            ; checksum to be placed here

;-----------------------------------------------------------------------
; 2nd PAGE: START OF TSB-INSTALLER CRITICAL CODE
;-----------------------------------------------------------------------

.org (RPAGESIZE)

TTRESET:
        cli
        ldi tmp1, low (RAMEND)          ; write ramend low to SPL
        out SPL, tmp1
        .ifdef SPH
        ldi tmp1, high(RAMEND)          ; write ramend high to SPH
        out SPH, tmp1                   ; when SRAM > 256 bytes
        .endif

        rjmp L2EraseFirstPage

;-----------------------------------------------------------------------
; LEVEL 1 - CHECK INTEGRITY OF TSB-INSTALLER
;-----------------------------------------------------------------------
; First of all we have to check integrity of the TSB-Installer at whole.
; Therefore a Checksum is calculated over all pages carrying relevant
; TSB-Installer and attached TSB-Firmware machine code.
; If this value (16 bit) is equal to the checksum that's been placed
; into the first page (precalculated by TSB-SW in the course of making
; customized TSB-Installer), we can be sure that the Installer has been
; uploaded completely and it is safe to start LEVEL 2.
; Yet with checksum error, the Installer would never leave this level
; and the old Bootloader will stay in force.

L1CHECK:
        cli
        ldi zh, high((RPAGESIZE-1)*2)    ; checksum stored by TSB-SW
        ldi zl, low ((RPAGESIZE-1)*2)    ; in last two bytes of 1st page
        lpm checkh, z+                  ; load checksum high byte
        lpm checkl, z+                  ; load checksum low byte
        ldi pcnt,(TLASTPAGE/RPAGESIZE)-1 ; # of pages to check
L1C0:   ldi bcnt, low(RPAGESIZE*2)       ; # of bytes to check per page
L1C1:   lpm tmp1, z+                    ; load byte of TSB-FW
        sub checkl, tmp1                ; subtract byte value
        sbci checkh, 0                  ; from checksum word
        dec bcnt                        ; until full page checked
        brne L1C1                       ; loop on
        dec pcnt
        brne L1C0                       ; until all pages checked
L1C2:
        cpi checkl, 0                   ; if checksum was correct,
        brne L1C2                       ; both bytes will be zero
        cpi checkh, 0                   ; else we canot continue
        brne L1C2                       ; and loop in here

        rjmp TTRESET

;-----------------------------------------------------------------------
; LEVEL 2 - REMOVE RESET-VECTOR TO OLD BOOTLOADER
;-----------------------------------------------------------------------
; After integrity of the TSB-Installer has been proven in Level 1,
; we can simply erase the whole 1st page, since it only contained the
; reset-jump to the old bootloader, the rjmp to the checksum routine
; and the checksum.
; Relevant machine code for the remaining tasks of TSB-Installer
; begins in 2nd page and will then be started directly with further
; resets or coldstarts.

L2EraseFirstPage:
        clr zl                          ; set Z to $0000
        clr zh                          ; start of 1st page
        ldi tmp1, 0b00000011            ; enable PGERS + SPMEN
        out SPMCSR, tmp1                ; in SPMCSR and go -
        spm                             ; erase that 1st page!

;-----------------------------------------------------------------------
; LEVEL 3 - ERASE PREVIOUS BOOTLOADER
;-----------------------------------------------------------------------
; Erase flash memory that may contain old bootloader's code
; down from LASTPAGE/FLASHEND to TLASTPAGE.

        ldi zl, low (LASTPAGE*2)        ; reset Z to LASTPAGE's start
        ldi zh, high(LASTPAGE*2)
        ldi pcnt, low ((LASTPAGE - TLASTPAGE) / RPAGESIZE) + 1
L3EPB0:
        ldi tmp1, 0b00000011            ; enable PGERS + SPMEN
        out SPMCSR, tmp1                ; in SPMCSR and erase current
        spm                             ; page by SPM (MCU halted)
        subi zl, low (RPAGESIZE*2)       ; Z = Z - PAGESIZE * 2
        sbci zh, high(RPAGESIZE*2)
        dec pcnt
        brne L3EPB0

;-----------------------------------------------------------------------
; LEVEL 4 - WRITE/VERIFY NEW BOOTLOADER
;-----------------------------------------------------------------------
; Copy new bootloader to its final location in upper flash memory.
; If verifying should fail, this will be repeated by every next reset.
; Pages copied top-to-bottom to avoid overlap in small attinys < 1k.

L4WriteVerify:

        ldi tmp1, low  (TLASTPAGE*2)    ; starting address source
        ldi tmp2, high (TLASTPAGE*2)
        movw tslo, tmp1

        ldi tmp1, low  (LASTPAGE*2)     ; starting address target
        ldi tmp2, high (LASTPAGE*2)
        movw ttlo, tmp1

        ldi pcnt, (((LASTPAGE-BOOTSTART)/RPAGESIZE)+1)   ; # of pages

; load source page contents into buffer
L4WV0:
        ldi yl, low (BUFFER)            ; set y to start address
        ldi yh, high(BUFFER)            ; of buffer (in SRAM)
        movw bflo, yl                   ; buffer start to bfhi/bflo
        ldi bcnt, low (RPAGESIZE*2)

        movw zl, tslo                   ; load Z with source address
L4WV1:
        lpm tmp1, z+                    ; load flash byte
        st y+, tmp1                     ; and store in buffer
        dec bcnt                        ; until full page buffered
        brne L4WV1                      ; loop on
        subi zl, low (RPAGESIZE*4)       ; decrease by two pages
        sbci zh, high(RPAGESIZE*4)       ; to continue one page below
        movw tslo, zl                   ; save current Z

; transfer buffer to flash write buffer and write to target page
        movw yl, bflo                   ; restore y to buffer start
        ldi bcnt, low(RPAGESIZE*2)
        movw zl, ttlo                   ; load Z with target address
L4WV2:
        ld r0, y+                       ; fill R0/R1 with word
        ld r1, y+                       ; from buffer position Y
        ldi tmp1, 0b00000001            ; set only SPMEN
        out SPMCSR, tmp1                ; to activate page buffering
        spm                             ; store word in page buffer
        adiw zl, 2                      ; Z = Z + 2
        subi bcnt, 2                    ; bcnt=bcnt-2
        brne L4WV2
        subi zl, low (RPAGESIZE*2)       ; restore Z to start of current
        sbci zh, high(RPAGESIZE*2)       ; page for write and verify

        ldi tmp1, 0b00000101            ; enable PRWRT + SPMEN
        out SPMCSR, tmp1                ; in SPMCSR and
        spm                             ; write flash page from buffer

L4WV3:  in tmp1, SPMCSR                 ; wait for flash write finished
        sbrc tmp1, 0                    ; go on if SPMEN cleared
        rjmp L4WV3                      ; otherwise wait along

; verify page just written, but stop everything in case of an error

L4WV4:
        movw yl, bflo                   ; restore y to buffer start
        ldi bcnt, low (RPAGESIZE*2)
L4WV5:
        lpm tmp1, z+                    ; load byte from flash
        ld tmp2, y+                     ; load buffer byte
        cp tmp1, tmp2                   ; compare them
        breq L4WV7                      ; and loop on if equal

L4WV6:  rjmp L4WV6                      ; else - hangup!

L4WV7:  dec bcnt                        ; count down page bytecounter
        brne L4WV5                      ; loop until all bytes checked
        subi zl, low (RPAGESIZE*4)       ; decrease by two pages
        sbci zh, high(RPAGESIZE*4)       ; to continue one page below
        movw ttlo, zl                   ; save z to target pointer
L4WV8:
        dec pcnt                        ; loop on with write and verify
        brne L4WV0                      ; until all pages transferred
L4WVx:

;-----------------------------------------------------------------------
; LEVEL 5 - CLEANUP AND HANDOVER TO NEW TSB
;-----------------------------------------------------------------------
; Jump to the absolute address of 'EraseAppFlash' in new TSB.
; This will remove all installer code from memory and safely complete
; the installation by leaving new TSB with cleaned up AppFlash.

L5EraseInstaller:
        ldi tmp1, low (EraseAppFlash+(BOOTSTART-TBOOTSTART))
        ldi tmp2, high(EraseAppFlash+(BOOTSTART-TBOOTSTART))
        push tmp1                       ; push lowbyte on stack
        push tmp2                       ; push highbyte on stack
        ret                             ; = absolute jump


;***********************************************************************
; END OF TSB-INSTALLER ROUTINES
;***********************************************************************

.endif          ; from IF-part for TSB-INSTALLER = 1

;***********************************************************************
; START OF REGULAR TSB CODE FOR ATTINYS
;***********************************************************************
; NOTE: The programming of TSB is largely relocatable which allows
; it being assembled inline with the Installer-routines above.
; Only the absolute addresses of LASTPAGE, BOOTSTART and DEVICEINFO are
; to be hard coded with respect to the bootloader's target location.

; Make TSB piggyback Installer at TBOOTSTART
.if TSBINSTALLER == 1
.org TBOOTSTART
.endif

; Make TSB for target address at BOOTSTART
.if TSBINSTALLER == 0
.org BOOTSTART
.endif

RESET:
        cli                             ; just to be sure...
        ldi tmp1, low (RAMEND)          ; write ramend low
        out SPL, tmp1                   ; into SPL (stackpointer low)
        .ifdef SPH
        ldi tmp1, high(RAMEND)          ; write ramend high for ATtinys
        out SPH, tmp1                   ; with SRAM > 256 bytes
        .endif

        in tmp1, MCUSR                  ; check MCUSR for WDRF
        sbrc tmp1, WDRF                 ; if this is a watchdog reset
        rjmp APPJUMP                    ; immediately leave TSB

;-----------------------------------------------------------------------
; ACTIVATION CHECK
;-----------------------------------------------------------------------
; Configure RX/TX Ports
; Wait for startbit and first auto-calibration character before timeout

WaitRX:
        sbi TXDDR, TXBIT                ; if RX=TX ("One-Wire"), port is
        cbi RXDDR, RXBIT                ; driven like open collector,
        sbi RXPORT, RXBIT               ; else RX is input with pullup
        sbi TXPORT, TXBIT               ; and TX actively driven "1"

WRXto:
        adiw xl, 1                      ; allow port levels to set
        brne WRXto                      ; await some milliseconds

        sbis RXPIN, RXBIT               ; if RX is low from the start
        rjmp APPJUMP                    ; direct APPJUMP

        rcall ZtoLASTPAGE               ; set Z to start'o'LASTPAGE
        adiw zl, 2                      ; skip first 2 bytes (APPJUMP)
        lpm tmp3, z+                    ; load TIMEOUT byte, leave Z+3
        cpi tmp3, 8                     ; ensure least delay for
        brcc WRX0                       ; accessibility of bootloader
        ldi tmp3, 8
WRX0:
        dec tmp1                        ; inner counter to delay
        brne WRX0                       ; for debouncing/denoising
        sbis RXPIN, RXBIT               ; if serial startbit occurs
        rjmp Activate                   ; go Activate
        dec tmp2                        ; else
        brne WRX0                       ; count down
        dec tmp3                        ; to TIMEOUT (from LASTPAGE)
        brne WRX0                       ; if timeout elapsed
        rjmp APPJUMP                    ; goto APPJUMP in LASTPAGE

;-----------------------------------------------------------------------
; BAUDRATE CALIBRATION CYCLE
;-----------------------------------------------------------------------
; Calibrate from leading autobaud characters ('@@@')
;                databits
;    startbit > S12345678S < stopbit
;    1st byte : 0000000101   startbit + bits 1-6 only for activation
; followed by : 0000000101   measure starts with bitcell 8 in 1st byte
; followed by : 0000000101   ends with bitcell 8 in last byte
; ... summing up to 17 bitcells for good timing reference

Activate:
        clr xl                          ; clear temporary
        clr xh                          ; baudrate counters
        ldi tmp1, 5                     ; counter for bit-changes
actw0:
        sbis RXPIN, RXBIT               ; idle 0-states
        rjmp actw0                      ; which may be inaccurate
actw1:
        sbic RXPIN, RXBIT               ; idle first 1-state
        rjmp actw1
actw2:
        adiw xl, 1                      ; precision measuring loop
        sbis RXPIN, RXBIT               ; count clock cycles in 0-state
        rjmp actw2
        dec tmp1                        ; count intermediary 1-states
        brne actw1                      ; up to complete result
        movw bclkl, xl                  ; save result in bclk
;        rcall Waitbitcell               ; since we are still in stopbit

;-----------------------------------------------------------------------
; CHECK PASSWORD
;-----------------------------------------------------------------------
; Check Password, if existing. No feedback whatsoever while characters
; are being received - silent hangup with wrong characters submit.
; After full and correct password has been entered, send device info.
; (OR: detect and execute command for an Emergency Erase)
; uses: tmp1, tmp3, Z (must point to LASTPAGE+3 at the beginning)

CheckPW:
chpw1:
        lpm tmp3, z+                    ; load character from Flash
        cpi tmp3, 255                   ; byte value (255) indicates
        breq chpwx                      ; end of password -> exit
        rcall Receivebyte               ; else receive next character
chpw2:
        cp tmp3, tmp1                   ; compare with password
        breq chpw1                      ; if equal check next character
        cpi tmp1, 0                     ; or was it 0 (emergency erase)
chpwl:  brne chpwl                      ; if not, loop infinitely
        rcall RequestConfirmation       ; if yes, request confirm
        brts chpa                       ; not confirmed, leave
        rcall RequestConfirmation       ; request 2nd confirm
        brts chpa                       ; can't be mistake now
        rcall EmergencyErase            ; go, emergency erase!

chpa:
        rjmp APPJUMP                    ; start application
chpwx:
;       rjmp SendDeviceInfo             ; go on to SendDeviceInfo

;-----------------------------------------------------------------------
; SEND DEVICEINFO
;-----------------------------------------------------------------------

SendDeviceInfo:
        ldi zl, low (DEVICEINFO*2)      ; load address of deviceinfo
        ldi zh, high(DEVICEINFO*2)      ; low and highbyte
        ldi bcnt, INFOLEN*2
        rcall SendFromFlash
;       rjmp Mainloop

;-----------------------------------------------------------------------
; MAIN LOOP TO RECEIVE AND EXECUTE COMMANDS
;-----------------------------------------------------------------------

Mainloop:
        clr zl                          ; clear Z pointer
        clr zh                          ; which is frequently used
        ldi tmp1, CONFIRM               ; send CONFIRM
        rcall Transmitbyte              ; via RS232
        rcall Receivebyte               ; receive command via RS232
        rcall CheckCommands             ; check command letter
        rjmp Mainloop                   ; and loop on

;-----------------------------------------------------------------------
; CHANGE USER DATA IN LASTPAGE
;-----------------------------------------------------------------------
; Receive page data and write into LASTPAGE
; uses: Y, Z, r0, r1, tmp1, bcnt

ChangeSettings:
        rcall GetNewPage                ; get new LASTPAGE contents
        brtc ChaSets0                   ; from Host (if confirmed)
        ret
ChaSets0:
        rcall ZtoLASTPAGE               ; re-write LASTPAGE
        set                             ; T=1 to disable internal verify
        rcall WritePageAfterErase       ; erase and write LASTPAGE

;-----------------------------------------------------------------------
; SEND USER DATA FROM LASTPAGE
;-----------------------------------------------------------------------
; uses: bcnt

ControlSettings:
        rcall ZtoLASTPAGE               ; point to LASTPAGE
;       rjmp SendPageFromFlash          ; send LASTPAGE over RS232

;-----------------------------------------------------------------------
; SEND DATA FROM FLASH MEMORY
;-----------------------------------------------------------------------
; Send bcnt number of flash memory bytes, count up Z accordingly
; uses: tmp1, bcnt, Z

SendPageFromFlash:
        ldi bcnt, low (PAGESIZE*2)

SendFromFlash:
        lpm tmp1, z+                    ; read directly from flash
        rcall Transmitbyte              ; and send out to RS232
        dec bcnt                        ; bcnt is number of bytes
        brne SendFromFlash
        ret

;-----------------------------------------------------------------------
; READ APPLICATION FLASH
;-----------------------------------------------------------------------
; read and transmit application flash area (pagewise)
; uses: Z (should be $0000 at the beginning), tmp1, bcnt

ReadAppFlash:
RAF0:   rcall RwaitConfirmation
        brts RAFx
        ldi bcnt, low(PAGESIZE*2)
        rcall SendPageFromFlash
RAF1:   cpi zl, low (BOOTSTART*2)
        brne RAF0
        cpi zh, high(BOOTSTART*2)       ; count Z to end of flash
        brne RAF0
RAFx:
        ret

;-----------------------------------------------------------------------
; WRITE APPLICATION FLASH
;-----------------------------------------------------------------------
; Write Appflash pagewise, modify reset vector and RJMP APP

WriteAppFlash:
        rcall EraseAppFlash             ; Erase whole app flash
        rcall GetNewPage                ; get first page's data
        brtc Flash1                     ; continue on CONFIRM (T=0)
        ret                             ; abort with Flash erased (T=1)

Flash1:
        rcall YtoBUFFER

.if FLASHEND >= ($1fff)                 ; biggg ATtiny

.message "ATtiny => 16 KB uses JMP for modified reset vector."
        ldd avecl, y+2                  ; read address portion of JMP
        ldd avech, y+3                  ; directly into avecl/avech
        ldi tmp1, low (BOOTSTART)       ; generate new JMP target
        ldi tmp2, high(BOOTSTART)       ; (16 bits = byte 3+4)
        std y+2, tmp1                   ; write low/highbyte of changed
        std y+3, tmp2                   ; jump address into buffer

.else                                   ; small ATtiny

.message "ATtiny < 16 KB uses RJMP for modified reset vector."
        ldd tmp1, y+0                   ; read bytes from original rjmp
        ldd tmp2, y+1                   ; into tmp1/tmp2
        subi tmp1, low (-(4096-LASTPAGE))      ; calculate
        sbci tmp2, high(-(4096-LASTPAGE))      ; rjmp APPSTART
        movw avecl, tmp1                ; save for later reference
        ldi tmp1, low (BOOTSTART-1 + $C000)     ; generate new opcode
        ldi tmp2, high(BOOTSTART-1 + $C000)     ; for rjmp BOOTSTART
        std y+0, tmp1                   ; replace rjmp in buffer
        std y+1, tmp2                   ; with rjmp BOOTSTART

.endif

        rcall WritePage                 ; write modified first page

Flash2:
        rcall GetNewPage                ; get next page to write
        brts FlashX                     ; stop on user's behalf
Flash3:
        rcall WritePage                 ; write page data into flash
Flash4:
        cpi zh, high(BOOTSTART*2-1)     ; check for end of Appflash
        brne Flash2                     ; if Z reached last location
        cpi zl, low (BOOTSTART*2-1)     ; then we are finished
        brne Flash2                     ; else go on
FlashX:
        rcall ReadLastPage              ; finally update LASTPAGE
        std y+0, avecl                  ; change first opcode
        std y+1, avech                  ; to jump target, then go
        rjmp WritePageafterErase

;-----------------------------------------------------------------------
; CHECK COMMANDS
;-----------------------------------------------------------------------
; Placed 'in between' to reach all subs by direct branches :-)

CheckCommands:
        cpi tmp1, 'c'                   ; read LASTPAGE
        breq ControlSettings
        cpi tmp1, 'C'                   ; write LASTPAGE
        breq ChangeSettings
        cpi tmp1, 'f'                   ; read Appflash
        breq ReadAppFlash
        cpi tmp1, 'F'                   ; write Appflash
        breq WriteAppFlash
        cpi tmp1, 'e'                   ; read EEPROM
        breq EEpromRead
        cpi tmp1, 'E'                   ; write EEPROM
        breq EEpromWrite
        rjmp APPJUMP                    ; else start application

;-----------------------------------------------------------------------
; EEPROM READ/WRITE ACCESS
;-----------------------------------------------------------------------
; Read or write full EEPROM address space
; With blocksize = PAGESIZE * 2 (since PAGESIZE is defined in words)
; uses: Y, Z, tmp1, tmp3

EEpromRead:
EeRe1:
        rcall RwaitConfirmation         ; wait to confirm
        brts EERWFx                     ; else we are finished
        ldi bcnt, low(PAGESIZE*2)       ; again PAGESIZE*2 is blocksize
EERe2:
        out EEARL, zl                   ; current EEPROM address low
        .ifdef  EEARH
        out EEARH, zh                   ; current EEPROM address high
        .endif
        sbi EECR, 0                     ; set EERE - EEPROM read enable
        in tmp1, EEDR                   ; read byte from current address
        rcall Transmitbyte              ; send out to RS232
        adiw zl,1                       ; count up EEPROM address
        dec bcnt                        ; count down block byte counter
        brne EERe2                      ; loop on if block not finished
        rjmp EERe1

;-----------------------------------------------------------------------

EEpromWrite:
EEWr0:
        rcall GetNewPage                ; get EEPROM datablock
        brts EERWFx                     ; or abort on host's demand
EEWr1:
        rcall YtoBUFFER                 ; Y = Buffer and Bcnt = blocksize
EEWr2:
        ld tmp1, y+                     ; read EEPROM byte from buffer
        rcall EEWriteByte
        dec bcnt                        ; count down block byte counter
        brne EEWr2                      ; loop on if block not finished
        rjmp EeWr0

EERWFx:
        ret

;-----------------------------------------------------------------------

EEWriteByte:
        out EEDR, tmp1                  ; write to EEPROM data register
        out EEARL, zl                   ; current EEPROM address low
        .ifdef  EEARH
        out EEARH, zh                   ; high EEARH for some attinys
        .endif
        sbi EECR, 2                     ; EEPROM master prog enable
        sbi EECR, 1                     ; EEPE initiate prog cycle
EeWB:
        sbic EECR, 1                    ; wait write cycle to complete
        rjmp EeWB                       ; before we can go on
        adiw zl,1                       ; count up EEPROM address
        ret

;-----------------------------------------------------------------------
; READ LASTPAGE INTO BUFFER
;-----------------------------------------------------------------------
; Page Address Z, SRAM buffer pointer Y, repoint Z to LASTPAGE

ReadLastPage:
        rcall ZtoLASTPAGE               ; reset Z to LASTPAGE start
        rcall YtoBUFFER                 ; reset Y to BUFFER start
ReLaPa:
        lpm tmp1, z+                    ; read byte from flash memory
        st y+, tmp1                     ; store in SRAM via Y pointer
        dec bcnt
        brne ReLaPa
ReLax:
        rcall ZtoLASTPAGE               ; reset Z to LASTPAGE start
        rjmp  YtoBUFFER                 ; reset Y to BUFFER start

;-----------------------------------------------------------------------
; GET NEW PAGE
;-----------------------------------------------------------------------
; Read page data into SRAM buffer
; uses: X, tmp1, bcnt, SRAM

GetNewPage:
        rcall RequestConfirmation       ; check for confirmation
        brts GNPx                       ; abort if not confirmed
GNP0:   rcall YtoBUFFER                 ; Y = BUFFER, bcnt = PAGESIZE*2
GNP1:   rcall ReceiveByte               ; receive serial byte
        st y+, tmp1                     ; and store in buffer
        dec bcnt                        ; until full page loaded
        brne GNP1                       ; loop on
GNPx:
        ret                             ; finished

;-----------------------------------------------------------------------
; REQUEST TO CONFIRM / AWAIT CONFIRM COMMAND
;-----------------------------------------------------------------------
; Send REQUEST and wait answer from Host (received character in tmp1)
; T=0: CONFIRM / T=1: NOT CONFIRM (any other char)

RequestConfirmation:
        ldi tmp1, REQUEST               ; send request character
        rcall Transmitbyte              ; prompt to confirm (or not)

RwaitConfirmation:
        rcall ReceiveByte               ; get host's reply
        clt                             ; set T=0 for confirm
        cpi tmp1, CONFIRM               ; if host HAS sent CONFIRM
        breq RCx                        ; return with the T=0
        set                             ; else set T=1 (NOT CONFIRMED)
RCx:
        ret                             ; whether confirmed or not

;-----------------------------------------------------------------------
; WRITE FLASH PAGES FROM BUFFER, VERIFYING AND VERIFY-ERROR-HANDLING
;-----------------------------------------------------------------------
; uses: r0, r1, Y, Z, tmp1, tmp2, bcnt
; Z must point to start address of page to be written.
; Z is left at start address of next page.
; T=0 for internal verifying and EraseAppFlash on any verifying error
; T=1 to skip verifying and consequences of an error

.if FOURPAGES == 0                      ; normal (single page) erase

WritePageafterErase:
        rcall EraseFlashPage
WritePage:
        rcall YtoBUFFER                 ; Y=BUFFER, bcnt=PAGESIZE*2
WrPa1:
        ld r0, y+                       ; fill R0/R1 with word
        ld r1, y+                       ; from buffer position Y / Y+1
        ldi tmp1, 0b00000001            ; set only SPMEN in SPMCSR
        out SPMCSR, tmp1                ; to activate page buffering
        spm                             ; store word in page buffer
        adiw zl, 2                      ; and forward to next word
        subi bcnt, 2
        brne WrPa1
        ; Z = start of next page now
        subi zl, low (PAGESIZE*2)       ; point back Z to
        sbci zh, high(PAGESIZE*2)       ; start of current page
        ; Z = back on current page's start
WrPa2:
        ldi tmp1, 0b00000101            ; enable PRWRT + SPMEN
        out SPMCSR, tmp1                ; in SPMCSR
        spm                             ; write whole page to flash
WrPa3:
        in tmp1, SPMCSR                 ; wait for flash write finished
        sbrc tmp1, 0                    ; skip if SPMEN (bit0) cleared
        rjmp WrPa3                      ; ITS BEEN WRITTEN

;-----------------------------------------------------------------------
; INTERNAL VERIFY AGAINST BUFFER CONTENTS
;-----------------------------------------------------------------------
; If verify of any page in AppFlash has FAILED, the program code and/or
; reset-vector may be corrupted - danger of bootloader lock-out!
; It is better then to IMMEDIATELY ERASE ALL APPLICATION FLASH
; to make sure the bootloader remains accessible by the next session
; (with option for a new attempt).

.message "INTERNAL VERIFYING FOR ATTINYS."

VerifyPage:
        rcall YtoBUFFER                 ; Y=BUFFER, bcnt=PAGESIZE*2
WrPV1:
        lpm tmp1, z+                    ; load flash byte
        ld tmp2,  y+                    ; load buffer byte
        cp tmp1, tmp2                   ; else compare
        breq WrPV2                      ; if EQUAL loop on
        brts WrPV2                      ; T=1 skips verify consequences
        ldi tmp1, CONFIRM
        rcall Transmitbyte
        rcall EraseAppFlash             ; otherwise erase appflash and

WrPVf:  rjmp WrPVf                      ; freeze for bootloader's sake
WrPV2:
        dec bcnt                        ; count down page bytecounter
        brne WrPV1                      ; loop until all bytes checked
WrPx:
        ret

.endif

;-----------------------------------------------------------------------

.if FOURPAGES == 1                      ; 4-Page Erase+Write (tn441/841)
                                        ; for real PAGESIZE of 8 Words
WritePageafterErase:
        rcall EraseFlashPage
WritePage:
        rcall YtoBUFFER
        ldi tmp3, 4
WrPa4P:
        ldi bcnt, 16
WrPa1:
        ld r0, y+                       ; fill R0/R1 with word
        ld r1, y+                       ; from buffer position Y / Y+1
        ldi tmp1, 0b00000001            ; set only SPMEN in SPMCSR
        out SPMCSR, tmp1                ; to activate page buffering
        spm                             ; store word in page buffer
        adiw zl, 2                      ; and forward to next word
        subi bcnt, 2
        brne WrPa1
        sbiw zl, 16                     ; Z back to start of real-page
WrPa2:
        ldi tmp1, 0b00000101            ; enable PGWRT + SPMEN
        out SPMCSR, tmp1                ; in SPMCSR
        spm                             ; write sub-page to flash
WrPa3:
        in tmp1, SPMCSR                 ; wait for flash write finished
        sbrc tmp1, 0                    ; skip if SPMEN (bit0) cleared
        rjmp WrPa3                      ; ITS BEEN WRITTEN

        ldi bcnt, 16
        sbiw yl, 16
WrPV1:
        lpm tmp1, z+                    ; load flash byte
        ld tmp2,  y+                    ; load buffer byte
        cp tmp1, tmp2                   ; else compare
        breq WrPV2                      ; if EQUAL loop on
        brts WrPV2                      ; T=1 skips verify consequences
        ldi tmp1, CONFIRM
        rcall Transmitbyte
        rcall EraseAppFlash             ; otherwise erase appflash and
WrPVf:  rjmp WrPVf                      ; freeze for bootloader's sake

WrPV2:
        dec bcnt                        ; count down page bytecounter
        brne WrPV1                      ; loop until all bytes checked
WrPx:
        dec tmp3                        ; count all 4 sub-pages
        brne WrPa4P                     ; to be written from buffer

        ret
.endif


;-----------------------------------------------------------------------
; FLASH ERASE TOP-TO-BOTTOM ( (BOOTSTART-1) ... $0000)
;-----------------------------------------------------------------------
; uses: Z, tmp1

EraseAppFlash:
        ldi zl, low (BOOTSTART*2)       ; point Z to BOOTSTART
        ldi zh, high(BOOTSTART*2)       ; 1st page's 1st address
EAF0:
        subi zl, low (PAGESIZE*2)       ; start erasing
        sbci zh, high(PAGESIZE*2)       ; one page below

        rcall EraseFlashPage            ; then erase pagewise down
        brne EAF0                       ; until first page reached
EAFx:   ret                             ; and leave with Z = $0000

;-----------------------------------------------------------------------
; ERASE FLASH / EEPROM / USERDATA
;-----------------------------------------------------------------------
; uses: Z, tmp1

EmergencyErase:
        rcall EraseAppFlash             ; first kill AppFlash contents
        ser tmp1
EEE0:
        rcall EEWriteByte               ; write EEPROM byte, Z = Z + 1
        cpi zh, high(EEPROMEND+1)+2     ; EEPROMEND
        brne EEE0                       ; and loop on until finished

        rcall ZtoLASTPAGE               ; to finally
;       rjmp EraseFlashPage             ; erase LASTPAGE

;-----------------------------------------------------------------------
; ERASE ONE FLASH PAGE
;-----------------------------------------------------------------------
; NOTE: with tn441/841 this is always 4-page erase

EraseFlashPage:
        ldi tmp1, 0b00000011            ; enable PGERS + SPMEN
        out SPMCSR, tmp1                ; in SPMCSR and erase current
        spm                             ; page by SPM (MCU halted)
        ret                             ; then return

;-----------------------------------------------------------------------
; OTHER SUBROUTINES
;-----------------------------------------------------------------------

YtoBUFFER:
        ldi yl, low (BUFFER)            ; reset pointer
        ldi yh, high(BUFFER)            ; to programming buffer
        ldi bcnt, low(PAGESIZE*2)       ; and often needed
        ret

;-----------------------------------------------------------------------

ZtoLASTPAGE:
        ldi zl, low (LASTPAGE*2)        ; reset Z to LASTPAGE start
        ldi zh, high(LASTPAGE*2)
        ret

;-----------------------------------------------------------------------
; RS232 RECEIVE BYTE
;-----------------------------------------------------------------------
; data: tmp1
; uses: tmp1 (received byte, fully overwritten), tmp2 (bitcounter)

ReceiveByte:
        sbi RXPORT, RXBIT               ; again set pullup for RX
Recb1:
        sbic RXPIN, RXBIT               ; wait for startbit (0)
        rjmp Recb1                      ; loop while stop state (1)
Recb2:
        ldi tmp2, 8                     ; bitcounter
        rcall Waithalfbitcell           ; tune to center of startbit
Recb3:
        rcall Waitbitcell               ; tune to center of bitcell
        lsr tmp1                        ; right shift 0 into bit 7
        sbic RXPIN, RXBIT               ; if RXD bit is 1
        sbr tmp1, 0b10000000            ; set bit 7
Recb4:
        dec tmp2                        ; count down bitcounter
        brne Recb3                      ; loop until 8 bits collected
        rjmp Waitbitcell                ; wait into center of stopbit

;-----------------------------------------------------------------------
; RS232 TRANSMIT BYTE
;-----------------------------------------------------------------------
; data: tmp1
; uses: tmp1 (to be consumed), tmp2 (bitcounter)
;
; with different portlines defined for RX and TX ("two-wire")
; => TX-line is configuered as an output compatible to TTL/HCMOS logic
;
; with the same portline defined for RX and TX ("one-wire")
; => TX-line controlled like an open collector/drain with weak pullup

TransmitByte:
        rcall Waitbitcell               ; ensure safe RX-TX transition
        rcall Trx0                      ; transmit 0 = startbit
        ldi tmp2, 8                     ; set bitcounter
Trxbit:                                 ; transmit byte loop
        sbrc tmp1, 0
        rcall Trx1                      ; sent logical 1 bitcell
        sbrs tmp1, 0                    ; or
        rcall Trx0                      ; sent logical 0 bitcell
        lsr tmp1                        ; shift out that bit
        dec tmp2                        ; count down
        brne Trxbit                     ; loop until all bits sent
Trx1:
        sbi TXDDR, TXBIT                ; if RX=TX (One-Wire), result is
        cbi RXDDR, RXBIT                ; pullup to Vcc for "1" (high-Z)
        sbi TXPORT, TXBIT               ; else portbit actively driven
        rjmp Waitbitcell
Trx0:
        sbi TXDDR, TXBIT                ; set TX driver for output
        cbi TXPORT, TXBIT               ; set portbit to active "0"
;       rjmp Waitbitcell                ; continue with Waitbitcell

;-----------------------------------------------------------------------
; RS232 PRECISION TIMING
;-----------------------------------------------------------------------
; input: bclkh, bclkl   (division-factor for bitcells at given baudrate)

Waitbitcell:
        movw xl, bclkl                  ; load bitcell clock timer
wbc1:
        sbiw xl, 18                     ; nearly same number of clocks
        nop                             ; as in calibration loop
        brcc wbc1
wbcx:   ret

Waithalfbitcell:
        movw xl, bclkl                  ; load bitcell clock timer
        lsr xh                          ; shiftout bit 0 of xh to carry
        ror xl                          ; carry shifted in bit 7 of xl
        rjmp wbc1                       ; run timer with 1/2 divider

;-----------------------------------------------------------------------
; DEVICE INFO BLOCK = PERMANENT DATA
;-----------------------------------------------------------------------
; 16 bytes of permanent data identify bootloader and device model

.if TSBINSTALLER == 0
        .org DEVICEINFO         ; ATtiny TSB in target address
.else
        .org TDEVICEINFO        ; ATtiny address of TSB-Installer
.endif

.message "Device info block for ATtiny"
.db "tsb", low (BUILDDATE), high (BUILDDATE), BUILDSTATE
.db SIGNATURE_000, SIGNATURE_001, SIGNATURE_002, low (PAGESIZE)
.dw BOOTSTART
.dw EEPROMEND

.if FLASHEND >= ($1fff)         ; check for device >= 16 KB
        APPJUMP:
        .db $0C, $94            ; first two bytes (opcode) for a JMP
.else
        .db $00, $00
        APPJUMP:
.endif

;-----------------------------------------------------------------------
; LASTPAGE WITH APPLICATION-JUMP, TIMEOUT, PASSWORD (USER DATA)
;-----------------------------------------------------------------------
; Leaving this page blank ($FF) will be safe defaults

RLASTPAGE:           ; reference label for end of fixed TSB code
                     ; in TSB/TSB-Installer (LASTPAGE)
; Check for correct alignment of TSB code when assembled to target

.if TSBINSTALLER == 0
        .if RLASTPAGE == LASTPAGE
        .message "TSB properly aligned with target address."
        .else
        .message "WARNING: TSB CODE NOT PROPERLY ALIGNED!"
        .if RLASTPAGE <  LASTPAGE
        .message "Code shifted below LASTPAGE."
        .endif
        .if RLASTPAGE >  LASTPAGE
        .message "Code shifted beyond LASTPAGE."
        .endif
        .endif
.endif

.if TSBINSTALLER == 1
        .if RLASTPAGE == TLASTPAGE
        .message "TSB Installer properly aligned."
        .else
        .message "WARNING: TSB CODE NOT PROPERLY ALIGNED!"
        .if RLASTPAGE <  TLASTPAGE
        .message "Code shifted below LASTPAGE."
        .endif
        .if RLASTPAGE >  TLASTPAGE
        .message "Code shifted beyond LASTPAGE."
        .endif
        .endif
.endif
.message "ASSEMBLY OF TSB FOR ATTINY SUCCESSFULLY FINISHED."
.endif

;***********************************************************************
; END OF TINY SAFE BOOTLOADER FOR ATTINYS
;***********************************************************************
;
;
;
.if TINYMEGA == 1
.message "START ASSEMBLY OF TSB FOR ATMEGA."
;
;
;
;***********************************************************************
;***********************************************************************
;***********************************************************************
; START OF TSB FOR ATMEGAS
;***********************************************************************
;***********************************************************************
;***********************************************************************
;
; TSB for ATmegas is always directly coded to target address.

.org BOOTSTART

RESET:
        cli                             ; just to be sure...
        ldi tmp1, low (RAMEND)          ; write ramend low
        out SPL, tmp1                   ; into SPL (stackpointer low)
        .ifdef SPH
        ldi tmp1, high(RAMEND)          ; write ramend high for ATtinys
        out SPH, tmp1                   ; with SRAM > 256 bytes
        .endif

        in tmp1, MCUSR                  ; check MCUCR for WDRF
        sbrc tmp1, WDRF                 ; if this is a watchdog reset
        rjmp APPJUMP                    ; immediately leave TSB

;-----------------------------------------------------------------------
; ACTIVATION CHECK
;-----------------------------------------------------------------------
; Configure RX/TX Ports
; Wait for startbit and first auto-calibration character before timeout

WaitRX:
        sbi TXDDR, TXBIT                ; if RX=TX ("one-wire"), port is
        cbi RXDDR, RXBIT                ; driven like open collector,
        sbi RXPORT, RXBIT               ; else RX is input with pullup
        sbi TXPORT, TXBIT               ; and TX is actively driven "1"

WRXto:
        adiw xl, 1                      ; allow port levels to set
        brne WRXto                      ; await some milliseconds

        sbis RXPIN, RXBIT               ; if RX is low from the start
        rjmp APPJUMP                    ; direct APPJUMP

        rcall ZtoLASTPAGE               ; set Z to start'o'LASTPAGE
        adiw zl, 2                      ; skip first 2 bytes (APPJUMP)
        lpm tmp3, z+                    ; load TIMEOUT byte, leave Z+3
        cpi tmp3, 8                     ; ensure least delay for
        brcc WRX0                       ; accessibility of bootloader
        ldi tmp3, 8
WRX0:
        dec tmp1                        ; inner counter to delay
        brne WRX0                       ; for debouncing/denoising
        sbis RXPIN, RXBIT               ; if serial startbit occurs
        rjmp Activate                   ; go Activate
        dec tmp2                        ; else
        brne WRX0                       ; count down
        dec tmp3                        ; to TIMEOUT (from LASTPAGE)
        brne WRX0                       ; if timeout elapsed
;        rjmp APPJUMP

;-----------------------------------------------------------------------
; ATMEGA APPJUMP - SIMPLY JUMP TO $0000 (ORIGINAL RESET VECTOR)
;-----------------------------------------------------------------------
; Boot Reset Vector must be activated (BOOTRST=0).
; After timeout or executing commands, TSB for ATmegas will simply
; handover to the App by a (relative or absolute) jump to $0000.

APPJUMP:
rcall SPMwait                           ; make sure everything's done
.if FLASHEND >= ($1fff)
        jmp  $0000                      ; absolute jump
.else
        rjmp $0000                      ; relative jump
.endif

;-----------------------------------------------------------------------
; BAUDRATE CALIBRATION CYCLE
;-----------------------------------------------------------------------
; Calibrate from leading autobaud characters ('@@@')
;                databits
;    startbit > S12345678S < stopbit
;    1st byte : 0000000101   startbit + bits 1-6 only for activation
; followed by : 0000000101   measure starts with bitcell 8 in 1st byte
; followed by : 0000000101   ends with bitcell 8 in last byte
; ... summing up to 17 bitcells for good timing reference

Activate:
        clr xl                          ; clear temporary
        clr xh                          ; baudrate counters
        ldi tmp1, 5                     ; counter for bit-changes
actw0:
        sbis RXPIN, RXBIT               ; idle 0-states
        rjmp actw0                      ; which may be inaccurate
actw1:
        sbic RXPIN, RXBIT               ; idle first 1-state
        rjmp actw1
actw2:
        adiw xl, 1                      ; precision measuring loop
        sbis RXPIN, RXBIT               ; count clock cycles in 0-state
        rjmp actw2
        dec tmp1                        ; count intermediary 1-states
        brne actw1                      ; up to complete result
        movw bclkl, xl                  ; save result in bclk
        rcall Waitbitcell               ; since we are still in stopbit
;       rjmp CheckPW

;-----------------------------------------------------------------------
; CHECK PASSWORD / EMERGENCY ERASE
;-----------------------------------------------------------------------
; Check Password, if existing. No feedback whatsoever while characters
; received - silent hangup with wrong characters submit.
; After full and correct password has been entered, go SendDeviceinfo.
; Emergency Erase: Detect command and engage Emergency Erase
; (two zeroes sent and double confirmed)
; uses: tmp1, tmp3, Z (must point to LASTPAGE+3 at the beginning)

CheckPW:

chpw1:
        lpm tmp3, z+                    ; load character from Flash
        cpi tmp3, 255                   ; byte value (255) indicates
        breq chpwx                      ; end of password -> okay
        rcall Receivebyte               ; else receive next character
chpw2:
        cp tmp3, tmp1                   ; compare with password
        breq chpw1                      ; if equal check next character
        cpi tmp1, 0                     ; or was it 0 (emergency erase)
chpwl:  brne chpwl                      ; if not, loop infinitely
        rcall RequestConfirmation       ; if yes, request confirm
        brts chpa                       ; not confirmed, leave
        rcall RequestConfirmation       ; request 2nd confirm
        brts chpa                       ; can't be mistake now
        rcall EmergencyErase            ; go, emergency erase!
chpa:
        rjmp APPJUMP                    ; start application
chpwx:
;        rcall SendDeviceInfo

;-----------------------------------------------------------------------
; SEND DEVICEINFO
;-----------------------------------------------------------------------

SendDeviceInfo:
        ldi zl, low (DEVICEINFO*2)      ; load address of deviceinfo
        ldi zh, high(DEVICEINFO*2)      ; low and highbyte
        ldi bcnt, INFOLEN*2
        rcall SendFromFlash

;-----------------------------------------------------------------------
; MAIN LOOP TO RECEIVE AND EXECUTE COMMANDS
;-----------------------------------------------------------------------

Mainloop:
        clr zl                          ; clear Z pointer
        clr zh                          ; which is frequently used
        ldi tmp1, CONFIRM               ; send CONFIRM
        rcall Transmitbyte              ; via RS232
        rcall Receivebyte               ; receive command via RS232
        rcall CheckCommands             ; check command letter
        rjmp Mainloop                   ; and loop on

;-----------------------------------------------------------------------
; CHANGE USER DATA IN LASTPAGE
;-----------------------------------------------------------------------
; Receive page data and write into LASTPAGE

ChangeSettings:
        rcall GetNewPage                ; get new LASTPAGE contents
        brtc ChaSets0                   ; from Host (if confirmed)
        ret
ChaSets0:
        rcall ZtoLASTPAGE               ; re-write LASTPAGE
        rcall EraseFlashPage
        rcall WritePage                 ; erase and write LASTPAGE

;-----------------------------------------------------------------------
; SEND USER DATA FROM LASTPAGE
;-----------------------------------------------------------------------

ControlSettings:
        rcall ZtoLASTPAGE               ; point to LASTPAGE
;       rcall SendPageFromFlash

;-----------------------------------------------------------------------
; SEND DATA FROM FLASH MEMORY
;-----------------------------------------------------------------------
; Send bcnt number of flash memory bytes, count up Z accordingly

SendPageFromFlash:
        ldi bcnt, low (PAGESIZE*2)      ; whole Page to send
SendFromFlash:
        rcall SPMwait                   ; (re)enable RWW read access
        lpm tmp1, z+                    ; read directly from flash
        rcall Transmitbyte              ; and send out to RS232
        dec bcnt                        ; bcnt is number of bytes
        brne SendFromFlash
        ret

;-----------------------------------------------------------------------
; READ APPLICATION FLASH
;-----------------------------------------------------------------------
; read and transmit application flash area (pagewise)

ReadAppFlash:
RAF0:
        rcall RwaitConfirmation
        brts RAFx
        rcall SendPageFromFlash
RAF1:
        cpi zl, low (LASTPAGE*2)        ; count up to last byte
        brne RAF0                       ; below LASTPAGE
        cpi zh, high(LASTPAGE*2)
        brne RAF0
RAFx:
        ret

;-----------------------------------------------------------------------
; WRITE APPLICATION FLASH
;-----------------------------------------------------------------------
; Write Appflash pagewise, don't modify anything for ATmegas

WriteAppFlash:
        rcall EraseAppFlash             ; Erase whole app flash
Flash2:
        rcall GetNewPage                ; get next page from host
        brts FlashX                     ; stop on user's behalf
Flash3:
        rcall WritePage                 ; write page data into flash
Flash4:
        cpi zh, high(LASTPAGE*2-1)      ; end of available Appflash?
        brne Flash2                     ; if Z reached last location
        cpi zl, low (LASTPAGE*2-1)      ; then we are finished
        brne Flash2                     ; else go on
FlashX:
        ret                             ; we're already finished!

;-----------------------------------------------------------------------
; WRITE FLASH PAGE FROM BUFFER, VERIFYING AND VERIFY-ERROR-HANDLING
;-----------------------------------------------------------------------
; uses: r0, r1, Y, Z, tmp1, tmp2, bcnt
; Z must point to start address of page to be written.
; Z will always be left at start address of next page.
; T=0 for internal verifying and EraseAppFlash on any verifying error
; T=1 to skip verifying and consequences of an error

WritePage:
        rcall YtoBUFFER                 ; Y=BUFFER, bcnt=PAGESIZE*2
WrPa1:
        ld r0, y+                       ; fill R0/R1 with word
        ld r1, y+                       ; from buffer position Y / Y+1
        ldi tmp1, 0b00000001            ; set only SPMEN in SPMCSR
        out SPMCSR, tmp1                ; to activate page buffering
        spm                             ; store word in page buffer
        adiw zl, 2                      ; and forward to next word
        subi bcnt, 2
        brne WrPa1
        ; Z = start of next page now
        subi zl, low (PAGESIZE*2)       ; point back Z to
        sbci zh, high(PAGESIZE*2)       ; start of current page
        ; Z = back on current page's start
WrPa2:
        ldi tmp1, 0b00000101            ; enable PRWRT + SPMEN
        out SPMCSR, tmp1                ; in SPMCSR
        spm                             ; write whole page to flash
WrPa3:
        in tmp1, SPMCSR                 ; wait for flash write finished
        sbrc tmp1, 0                    ; skip if SPMEN (bit0) cleared
        rjmp WrPa3                      ; ITS BEEN WRITTEN
        subi zl, low (-PAGESIZE*2)      ; same effect as
        sbci zh, high(-PAGESIZE*2)      ; Z = Z + PAGESIZE*2
        ret

;-----------------------------------------------------------------------
; CHECK COMMANDS
;-----------------------------------------------------------------------
; Placed 'in between' to reach all subs by direct branches

CheckCommands:
        cpi tmp1, 'c'                   ; read LASTPAGE
        breq ControlSettings
        cpi tmp1, 'C'                   ; write LASTPAGE
        breq ChangeSettings
        cpi tmp1, 'f'                   ; read Appflash
        breq ReadAppFlash
        cpi tmp1, 'F'                   ; write Appflash
        breq WriteAppFlash
        cpi tmp1, 'e'                   ; read EEPROM
        breq EepromRead
        cpi tmp1, 'E'                   ; write EEPROM
        breq EEpromWrite
        rjmp APPJUMP                    ; else start application

;-----------------------------------------------------------------------
; EEPROM READ/WRITE ACCESS
;-----------------------------------------------------------------------
; Read or write full EEPROM address space
; With blocksize = PAGESIZE * 2 (since PAGESIZE is defined in words)
; uses: Y, Z (0 at the beginnung), tmp1, tmp3

EepromWrite:
EEWr0:
        rcall GetNewPage                ; get EEPROM datablock
        brts EERWFx                     ; or abort on host's demand
EEWr1:
        rcall YtoBUFFER                 ; Y = Buffer and Bcnt = blocksize
EEWr2:
        ld tmp1, y+                     ; read EEPROM byte from buffer
        rcall EEWriteByte
        dec bcnt                        ; count down block byte counter
        brne EEWr2                      ; loop on if block not finished
        rjmp EeWr0

;-----------------------------------------------------------------------

EEpromRead:
EeRe1:
        rcall RwaitConfirmation         ; wait to confirm
        brts EERWFx                     ; else we are finished
        ldi bcnt, low(PAGESIZE*2)       ; again PAGESIZE*2 is blocksize
EERe2:
        out EEARL, zl                   ; current EEPROM address low
        .ifdef  EEARH
        out EEARH, zh                   ; current EEPROM address high
        .endif
        sbi EECR, 0                     ; set EERE - EEPROM read enable
        in tmp1, EEDR                   ; read byte from current address
        rcall Transmitbyte              ; send out to RS232
        adiw zl,1                       ; count up EEPROM address
        dec bcnt                        ; count down block byte counter
        brne EERe2                      ; loop on if block not finished
        rjmp EERe1
EERWFx:
        ret

;-----------------------------------------------------------------------

EEWriteByte:
        out EEDR, tmp1                  ; write to EEPROM data register
        out EEARL, zl                   ; current EEPROM address low
        .ifdef  EEARH
        out EEARH, zh                   ; high EEARH for some attinys
        .endif
        sbi EECR, 2                     ; EEPROM master prog enable
        sbi EECR, 1                     ; EEPE initiate prog cycle
EeWB:
        sbic EECR, 1                    ; wait write cycle to complete
        rjmp EeWB                       ; before we can go on
        adiw zl,1                       ; count up EEPROM address
        ret

;-----------------------------------------------------------------------
; GET NEW PAGE
;-----------------------------------------------------------------------
; Read page data into SRAM buffer
; uses: X, tmp1, bcnt, SRAM

GetNewPage:
        rcall RequestConfirmation       ; check for confirmation
        brts GNPx                       ; abort if not confirmed
GNP0:
        rcall YtoBUFFER                 ; Y = BUFFER, bcnt = PAGESIZE*2
GNP1:
        rcall ReceiveByte               ; receive serial byte
        st y+, tmp1                     ; and store in buffer
        dec bcnt                        ; until full page loaded
        brne GNP1                       ; loop on
GNPx:
        ret                             ; finished
;-----------------------------------------------------------------------
; REQUEST TO CONFIRM / AWAIT CONFIRM COMMAND
;-----------------------------------------------------------------------
; Send REQUEST and wait answer from Host (received character in tmp1)
; T-flag cleared: CONFIRM / T-flag set: NOT CONFIRM (any other char)

RequestConfirmation:
        ldi tmp1, REQUEST               ; send request character
        rcall Transmitbyte              ; prompt to confirm (or not)

RwaitConfirmation:
        rcall ReceiveByte               ; get host's reply
        clt                             ; set T=0 for confirm
        cpi tmp1, CONFIRM               ; if host HAS sent CONFIRM
        breq RCx                        ; return with the T=0
        set                             ; else set T=1 (NOT CONFIRMED)
RCx:
        ret                             ; whether confirmed or not

;-----------------------------------------------------------------------
; FLASH ERASE TOP-TO-BOTTOM ( (BOOTSTART-1) ... $0000)
;-----------------------------------------------------------------------
; uses: Z, tmp1

EraseAppFlash:
        rcall ZtoLASTPAGE               ; point Z to LASTPAGE, directly
EAF0:
        subi zl, low (PAGESIZE*2)
        sbci zh, high(PAGESIZE*2)
        rcall EraseFlashPage
        brne EAF0                       ; until first page reached
EAFx:   ret                             ; and leave with Z = $0000

;-----------------------------------------------------------------------
; EMERGENCY ERASE OF FLASH / EEPROM / USERDATA
;-----------------------------------------------------------------------
; uses: Z, tmp1

EmergencyErase:
        rcall EraseAppFlash             ; erase Application Flash
        ser tmp1                        ; byte value for EEPROM writes
EEE0:
        rcall EEWriteByte               ; write EEPROM byte, Z = Z + 1
        cpi zh, high(EEPROMEND+1)+2     ; EEPROMEND
        brne EEE0                       ; and loop on until finished

        rcall ZtoLASTPAGE               ; LASTPAGE is to be erased
;        rcall EraseFlashPage

;-----------------------------------------------------------------------
; ERASE ONE FLASH PAGE
;-----------------------------------------------------------------------

EraseFlashPage:
        ldi tmp1, 0b00000011            ; enable PGERS + SPMEN
        out SPMCSR, tmp1                ; in SPMCSR and erase current
        spm                             ; page by SPM (MCU halted)

; Waiting for SPM to be finished is obligatory on ATmegas
SPMwait:
        in tmp1, SPMCSR
        sbrc tmp1, 0                    ; wait previous SPMEN
        rjmp SPMwait
        ldi tmp1, 0b00010001            ; set RWWSRE and SPMEN
        out SPMCSR, tmp1
        spm
        ret

;-----------------------------------------------------------------------
; OTHER SUBROUTINES
;-----------------------------------------------------------------------

YtoBUFFER:
        ldi yl, low (BUFFER)            ; reset pointer
        ldi yh, high(BUFFER)            ; to programming buffer
        ldi bcnt, low(PAGESIZE*2)       ; and often needed
        ret

;-----------------------------------------------------------------------

ZtoLASTPAGE:
        ldi zl, low (LASTPAGE*2)        ; reset Z to LASTPAGE start
        ldi zh, high(LASTPAGE*2)
        ret

;-----------------------------------------------------------------------
; RS232 RECEIVE BYTE
;-----------------------------------------------------------------------
; data: tmp1
; uses: tmp1 (received byte, fully overwritten), tmp2 (bitcounter)

ReceiveByte:
        sbi RXPORT, RXBIT               ; again set pullup for RX
Recb1:
        sbic RXPIN, RXBIT               ; wait for startbit (0)
        rjmp Recb1                      ; loop while stop state (1)
Recb2:
        ldi tmp2, 8                     ; bitcounter
        rcall Waithalfbitcell           ; tune to center of startbit
Recb3:
        rcall Waitbitcell               ; tune to center of bitcell
        lsr tmp1                        ; right shift 0 into bit 7
        sbic RXPIN, RXBIT               ; if RXD bit is 1
        sbr tmp1, 0b10000000            ; set bit 7
Recb4:
        dec tmp2                        ; count down bitcounter
        brne Recb3                      ; loop until 8 bits collected
        rjmp Waitbitcell                ; wait into center of stopbit

;-----------------------------------------------------------------------
; RS232 TRANSMIT BYTE
;-----------------------------------------------------------------------
; data: tmp1
; uses: tmp1 (to be consumed), tmp2 (bitcounter)
;
; with different portlines defined for RX and TX ("two-wire")
; => TX-line is configuered as an output compatible to TTL/HCMOS logic
;
; with the same portline defined for RX and TX ("one-wire")
; => TX-line acts like an open collector/drain with weak pullup

TransmitByte:
        rcall Waitbitcell               ; ensure safe RX-TX transition
        rcall Trx0                      ; transmit 0 = startbit
        ldi tmp2, 8                     ; set bitcounter
Trxbit:                                 ; transmit byte loop
        sbrc tmp1, 0
        rcall Trx1                      ; sent logical 1 bitcell
        sbrs tmp1, 0                    ; or
        rcall Trx0                      ; sent logical 0 bitcell
        lsr tmp1                        ; shift out that bit
        dec tmp2                        ; count down
        brne Trxbit                     ; loop until all bits sent
Trx1:
        sbi TXDDR, TXBIT                ; if RX=TX (One-Wire), result is
        cbi RXDDR, RXBIT                ; pullup to Vcc for "1" (high-Z)
        sbi TXPORT, TXBIT               ; else portbit actively driven
        rjmp Waitbitcell
Trx0:
        sbi TXDDR, TXBIT                ; set TX driver for output
        cbi TXPORT, TXBIT               ; set portbit to active "0"
;       rjmp Waitbitcell                ; continue with Waitbitcell

;-----------------------------------------------------------------------
; RS232 PRECISION TIMING
;-----------------------------------------------------------------------
; input: bclkh, bclkl   (division-factor for bitcells at given baudrate)
; uses: X
;
Waitbitcell:
        movw xl, bclkl                  ; load bitcell clock timer
wbc1:
        sbiw xl, 18                     ; nearly same number of clocks
        nop                             ; as in calibration loop
        brcc wbc1
wbcx:   ret

Waithalfbitcell:
        movw xl, bclkl                  ; load bitcell clock timer
        lsr xh                          ; shiftout bit 0 of xh to carry
        ror xl                          ; carry shifted in bit 7 of xl
        rjmp wbc1                       ; run timer with 1/2 divider

;-----------------------------------------------------------------------
; DEVICE INFO BLOCK = PERMANENT DATA
;-----------------------------------------------------------------------
; 16 bytes of permanent data to identify bootloader and device model
; may be directly attached to the active machine code on ATmegas

DEVICEINFO:

.message "Device info block for ATmega"
.db "TSB", low (BUILDDATE), high (BUILDDATE), BUILDSTATE
.db SIGNATURE_000, SIGNATURE_001, SIGNATURE_002, low (PAGESIZE)
.dw BOOTSTART-PAGESIZE
.dw EEPROMEND
.db $AA, $AA

;-----------------------------------------------------------------------
; LASTPAGE WITH APPLICATION-JUMP, TIMEOUT, PASSWORD (USER DATA)
;-----------------------------------------------------------------------
; Leaving this page blank ($FF) will be safe defaults

.message "ASSEMBLY OF TSB FOR ATMEGA SUCCESSFULLY FINISHED."
.endif               ; closing TSB for ATmega sourcecode;

;***********************************************************************
; END OF TSB FOR ATMEGAS
;***********************************************************************
;
.exit
;
;
;***********************************************************************
; END OF CONDITIONAL ASSEMBLY CODE TSB FOR ATTINYS AND TSB FOR ATMEGAS
;***********************************************************************
;
;
;
;***********************************************************************
;***********************************************************************
;***********************************************************************
;


