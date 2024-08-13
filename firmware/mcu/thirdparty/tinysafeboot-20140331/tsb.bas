'-----------------------------------------------------------------------
'-----------------------------------------------------------------------
'-----------------------------------------------------------------------
' CONSOLE TOOL FOR TINYSAFEBOOT
' ... the tiny and safe bootloader for AVR-ATtinys
'-----------------------------------------------------------------------
'-----------------------------------------------------------------------
'-----------------------------------------------------------------------
' Written in 2011-2014 by Julien Thomas <joytec@gmx.de>
'
' This program is free software; you can redistribute it and/or
' modify it under the terms of the GNU General Public License
' as published by the Free Software Foundation; either version 3
' of the License, or (at your option) any later version.
' This program is distributed in the hope that it will be useful,
' but WITHOUT ANY WARRANTY; without even the implied warranty
' of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
' See the GNU General Public License for more details.
' You should have received a copy of the GNU General Public License
' along with this program; if not, see <http://www.gnu.org/licenses/>.
'
' ----------------------------------------------------------------------
'
' To be compiled with FreeBasic 0.24 up
'
'-----------------------------------------------------------------------
' VARIABLE AND CONSTANTS DEFINITIONS
'-----------------------------------------------------------------------
'
#lang "fb"


' Working variables

dim shared Filename as string

dim astr as string
dim bstr as string
dim cstr as string

dim a as ubyte
dim b as ubyte
dim i as ushort
dim j as ushort
dim k as ushort
dim z as UInteger

' COM-Port related
const Comset = ",N,8,1,CS,DS,RB0,TB0,BIN"
dim shared Comport              as string
dim shared OneWireLocalEcho     as Ubyte    : OneWireLocalEcho = 0

' Global variables
dim shared TSBIDENT     as string
dim shared TSBBUILD     as uinteger
dim shared TSBSTATUS    as ubyte
dim shared SIG000       as ubyte
dim shared SIG001       as ubyte
dim shared SIG002       as ubyte
dim shared PAGESIZE     as ubyte
dim shared FLASHSIZE    as ushort
dim shared APPFLASH     as ushort
dim shared EEPROMSIZE   as ushort
dim shared JMPMODE      as ubyte = 0
dim shared TINYMEGA     as ubyte = 0
dim shared APPJUMP      as ushort
dim shared TIMEOUT      as ubyte
dim shared PASSWORD     as string

dim shared AAAA         as UInteger = 0
dim shared ADDR         as UInteger = 0

dim RXTXPB (3,1)        as Ubyte       ' opcodes conversion array
dim shared DEVPORTS(6)  as Ubyte


const maxarray as UInteger = 65536
dim shared BINARRAY(maxarray) as ubyte  : Clear BINARRAY(0), 255, maxarray

dim shared LASTPAGE(255) as ubyte       : Clear LASTPAGE(0), 255, 255

const REQUEST as string = "?"
const CONFIRM as string = "!"

'-----------------------------------------------------------------------
' SUBS AND FUNCTIONS
'-----------------------------------------------------------------------

function CheckChecksum (ByRef ihline as string) as ubyte
dim I as ubyte
dim C as ubyte
    if ihline = "" then return (255)
        for I = 2 To (len (ihline)) Step 2
        C = C + Val ("&h"+ (mid$(ihline,i,2)))
    next I
C = 0 - C   ' equal to +1 and 2's complement (type is ubyte)
'with checksum also added, this should be 0 if record line was o.k.
return (C)
end function

sub SendCommand (ByRef astr as string)
    dim a as ushort
    dim t as ushort
    dim bstr as string
    if len(astr) = 0 then exit sub
    print #8, astr;             ' send commando/data string

    if OneWireLocalEcho then        ' strip off local echo
        a = 0
        do until a = len(astr)
            t = Timer + 1
            do until (LOC(8)) Or (Timer > t) : loop
            bstr = input$(1, #8)
            a = a + 1
        loop
    endif
end sub

function RXBuffer () as string
    dim a as ushort
    dim t as UInteger
    dim astr as string
    dim bstr as string

    t = Timer + 3
    do until LOC(8) Or (Timer > t) : loop
    if LOC(8) = 0 then return ("")
    bstr = ""
    t = Timer + 3
    do until Eof(8) Or (Timer > t)
        a = LOC(8) : sleep 100 : a = LOC(8) - a
            if a = 0 then
            astr = input$(LOC(8), #8)
            else    astr = input$(1, #8)
            endif
        bstr = bstr + astr
    loop

    return (bstr)

end function

function GetUserData () as ubyte

'load userdata from LASTPAGE into an array named LASTPAGE()

dim i as ubyte
dim bstr as string

SendCommand ("c")
bstr = RXBuffer
if len(bstr) < PAGESIZE+1       then return (255) ' page read error
if right$(bstr,1) <> CONFIRM    then return (255) ' block not terminated

clear LASTPAGE(0), 255, 255
for i = 0 To PAGESIZE-1 : LASTPAGE (i) = asc(mid$(bstr,i+1,1)) : next i

APPJUMP = (LASTPAGE(0) + LASTPAGE(1)*256) ' low/high byte in memory

if TINYMEGA = 1 then APPJUMP = 0000       ' ATmegas APPJUMP not relevant

TIMEOUT = LASTPAGE(2)   ' load timeout byte from LASTPAGE

i = 3 : PASSWORD = ""

do until (i = PAGESIZE) or (LASTPAGE(i) = 255)
        PASSWORD = PASSWORD + chr$(LASTPAGE(i))
        i = i + 1
loop

return (0)

end function

function VerifyUserData () as ubyte

dim i as ubyte
dim bstr as string
SendCommand ("c")           ' read LASTPAGE again
bstr = RXBuffer
if len(bstr) < 16       then return (255)       ' data page not correctly read
if right$(bstr,1) <> CONFIRM then return (255)  ' data block not terminated

for i = 0 To Len(bstr)-1
    if LASTPAGE (i) <> asc(mid$(bstr, i+1, 1)) then Exit for
next i

if i < (Len(bstr)-1) then return (255)  ' differences detected

return (0)              ' all data identical to LASTPAGE

end function

function Word2Date (Byref InWord as ushort) as UInteger
Word2Date =     ( InWord and 31 ) + _
        ((InWord and 480) \ 32) * 100 + _
        ((Inword and 65024 ) \ 512) * 10000 _
        + 20000000
end function

function ActivateTSB (ByRef Comport as string) as ubyte

dim bstr as string

if open COM (Comport + Comset for binary as #8) > 0 then return (Err)
sleep 100

print #8, "@@@";        ' first try without password
                        ' in case of no reaction, password may be entered
bstr = RXBuffer

if left$(bstr,3) = "@@@" then
        OneWireLocalEcho = 1                ' one-wire echo detected
        bstr = right$(bstr, (len(bstr)-3))  ' detach echoed characters
        print
        print "One-Wire interface detected."
endif

if bstr = "" then

    line input "Password : ", PASSWORD
    SendCommand (PASSWORD)              ' submit password

    bstr = RXBuffer

    if bstr <> "" then
    print : print "Password ... OK" : print
    else return (255)
    endif

endif

' analyze tsb's reply to gain device data (permanent data string)

if right$(bstr,1)        <> CONFIRM     then return (255)      ' no good
if LCase$(left$(bstr,3)) <> "tsb"       then return (255)      ' no good

dim BUILDWORD as ushort

TSBIDENT        = left$(bstr,3)
BUILDWORD       = asc(mid$(bstr,4,1)) + asc(mid$(bstr,5,1)) * 256
TSBSTATUS       = asc(mid$(bstr,6,1))
SIG000          = asc(mid$(bstr,7,1))
SIG001          = asc(mid$(bstr,8,1))
SIG002          = asc(mid$(bstr,9,1))
PAGESIZE        = (asc(mid$(bstr,10,1))) * 2            ' WORDS * 2 = BYTES
APPFLASH        = (asc(mid$(bstr,11,1)) + asc(mid$(bstr,12,1))*256) * 2 ' WORDS * 2 = BYTES
FLASHSIZE       = ((APPFLASH \ 1024) + 1) * 1024        ' round up to next higher multiple of 1024
EEPROMSIZE      = (asc(mid$(bstr,13,1)) + asc(mid$(bstr,14,1))*256) + 1 ' always BYTES

' NOTE: While AVR/Assembler is accounting in WORDS, except from EEPROM,
'       on the PC-side we are using BYTES for memory allocation
'
'
' Check for regular PAGESIZEs
if      (PAGESIZE<>16) and _    ' smallest PAGESIZE = 8 (WORDS)
        (PAGESIZE<>32) and _
        (PAGESIZE<>64) and _
        (PAGESIZE<>128) _       ' maximum PAGESIZE = 64 (WORDS)
        then
        print "PAGESIZE NOT VALID - ABORT."
        return (255)
endif

' consider TSB firmware with new date identifier and status byte
if BUILDWORD < 32768 then
    TSBBUILD = Word2Date (BUILDWORD)
else    'old date encoding in tsb-fw (three bytes)
    TSBBUILD = BUILDWORD + 65536 + 20000000
endif

' detect whether device is ATtiny or ATmega from identifier
' in last byte of device info block
' while decision for jmp/rjmp depends on memory size
select case asc(mid$(bstr,15,1))
case &h00 : JMPMODE = 0 : TINYMEGA = 0
case &h0C : JMPMODE = 1 : TINYMEGA = 0
case &hAA : JMPMODE = 0 : TINYMEGA = 1
end select

print

return GetUserData()        ' 0 = OK / <> 0 = ERROR

end function

sub DeactivateTSB ()
print #8, "q";
close #8
end sub

function LoadBinToArray () as ubyte

clear BINARRAY(0), 255, maxarray

dim ff as ubyte = Freefile
dim ihline as string
dim LL as ubyte
dim i as ubyte
dim as uinteger Max_AAAA = 0

' Load binary file

        AAAA = 0
        if open (Filename for Binary as ff) <> 0 then
            print "File not found!" : return (Err)
        endif

        if lof(ff) = 0 then return (1)
        do while AAAA < lof (ff)
            BINARRAY(AAAA) = asc(input$(1,ff))
        AAAA = AAAA + 1
        loop

        if AAAA <> lof(ff) then return (1) ' load error

close ff

' if function leaves here, the binary data from file has been loaded
' into BINARRAY() and AAAA is the number of bytes loaded.

return (0)

end function

function LoadHexToArray () as ubyte

clear BINARRAY(0), 255, maxarray

dim ff as ubyte = Freefile
dim ihline as string
dim LL as ubyte
dim i as ubyte
dim as uinteger Max_AAAA = 0

' Try to load ihex encoded file by default.
' Return values:
' 0 = success
' 1 = file not found
' 2 = no valid ihex
' 3 = checksum error

' Load ihex file

    if open (Filename for input as #ff) <> 0 then close ff: return (1)

        While not eof(ff)

        line input #ff, ihline       ' fetch whole lines (CR/LF-terminated)

        if left$(ihline,1) <> ":" then close ff : return (2) ' no valid ihex

        ihline = UCase$(ihline)

        if CheckChecksum (ihline) then close ff : return (3) ' checksum error

            if mid$(ihline, 8,2) = "00" then
                LL =   Val("&h"+mid$(ihline,2,2))
                AAAA = Val("&h"+mid$(ihline,4,4))

                for i = 1 To LL

                    BINARRAY(AAAA) = val("&h" + mid$( ihline, 8 + i*2 , 2 ) )

                                        if AAAA > Max_AAAA then Max_AAAA=AAAA: endif

                    AAAA = AAAA+1

                next i

            endif

        Wend

        AAAA = Max_AAAA ' set AAAA to max address of any hexfile content loaded

        AAAA = AAAA+1   ' set return value of AAAA to AAAA+1 ( = number of bytes)

close ff

' if function leaves here, contents of the hex file have been successfully
' loaded into BINARRAY() with AAAA as the exact number of bytes thereof.

return (0)

end function

function LoadToArray () as ubyte

select  case LoadHexToArray()
        case 1 : print : print "File not found." : return (1)
        case 2 : if LoadBinToArray () <> 0 then return(1)
        case 3 : print : print "Checksum error in Hex record." : return (1)
end select

return (0)

end function

function SaveFromArray () as ubyte

if AAAA = 0 then return (1)

ADDR = 0

dim ff as ubyte = Freefile
dim i as ubyte
dim C as ubyte
dim astr as string
const CRLF as string = Chr$(13) + Chr$(10) ' all lines CRLF terminated


if lcase$(right$(Filename,4)) = ".hex" _
or lcase$(right$(Filename,4)) = ".eep" then

' Save iHEX encoded file

    if open (Filename for Output as #ff) <> 0 then return (Err)

    print #ff, ":020000020000FC"; CRLF;     ' same 1st line in 16-bit IHex

    do until ADDR >= AAAA
    astr = ":10" + WHex (ADDR, 4) + "00"
        for i = 1 To 16
            astr = astr + WHex(BINARRAY(ADDR),2)
            ADDR = ADDR + 1
        next i
    C = 0
        for i = 2 To (Len (astr)) Step 2
            C = C + Val ("&h"+ (mid$(astr,i,2)))
        next I

    C = 0 - C

    astr = astr + WHex(C,2)
    print #ff, astr; CRLF;
    loop
    astr = ":00000001FF" :  print #ff, astr; CRLF;   ' send last line

    else

' Save binary file

        ADDR = 0
        if open (Filename for Output as #ff) <> 0 then return (Err)
        do until (ADDR = AAAA)
        print #ff, Chr$(BINARRAY(ADDR));
        ADDR = ADDR + 1
        loop

endif

close ff

return (Err)

end function

function Check4SPM() as ubyte
dim CheckADDR as ushort = 0
do until CheckADDR >= AAAA
    if BINARRAY(CheckADDR)=&hE8 And BINARRAY(CheckADDR+1)=&h95 then Exit do : endif
    CheckADDR = CheckADDR + 2
loop
  if CheckADDR\2 < AAAA\2 then return (255) else return (0)
end function

function CurrentDateBlock () as string
' this function is currently not needed
dim bstr as string * 8
    bstr = right$(Date$, 4)
    bstr = bstr + left$(Date$,2)
    bstr = bstr + mid$(Date$,4, 2)
return bstr
end function

function FWnumber () as string

dim fstr as string * 8
dim taddr as ushort
dim BUILDWORD as ushort

taddr = AAAA - 256      ' start search well below last page
fstr = ""

do  ' find TSB's device info block
taddr = taddr + 1
fstr = chr$(BINARRAY(taddr)) + chr$(BINARRAY(taddr+1)) + chr$(BINARRAY(taddr+2))
loop until (fstr = "tsb") or (fstr = "TSB") or (taddr = AAAA)

if taddr = AAAA then return ("")
BUILDWORD = (BINARRAY(taddr+3) + BINARRAY(taddr+4)*256)
if BUILDWORD < 32768 then
    fstr = str$( Word2Date(BUILDWORD) )
else
    fstr = str$( BINARRAY(taddr+3) + BINARRAY(taddr+4) * 256 + 65536 + 20000000)
endif
return fstr

end function

function SendLASTPAGE () as ubyte

dim i as ubyte
dim astr as string
dim bstr as string

SendCommand ("C")               ' command Change for TSB

if RXBuffer <> REQUEST then return (255)    ' would be error

    for i = 0 To PAGESIZE-1
        Astr = Astr + Chr$(LASTPAGE(i))
    next i

SendCommand (CONFIRM + Astr)

if RXBuffer = CONFIRM then return (255)     ' would be error

return (0)

end function

function DatasToArray (byref devname as string) as ubyte

Clear BINARRAY(0), 255, maxarray

dim ihline as string
dim LL as ubyte
dim i as ubyte
dim max_AAAA as UInteger : max_AAAA = 0

devname = lcase$(devname)

restore
' search for beginning of tsb-templates / tt-datas
' (included and compiled into this program)
do
read ihline
loop until lcase$(ihline) = "begin tt" or ihline = ""

if ihline ="" then return (1)   ' serious problem: no tt-datas found!

' o.k. datas found, now parse for the desired attiny
do
read ihline
loop until (ihline = devname) or lcase$(ihline) = "end tt"

if ihline = "end tt" then return (1)    ' did not find that stuff

    do
        read ihline     'fetch hex record from datas
        if CheckChecksum (ihline) then return (1)
        if mid$(ihline, 8,2) = "00" then
            LL =   Val("&h"+mid$(ihline,2,2))
            AAAA = Val("&h"+mid$(ihline,4,4))
            for i = 1 to LL
                BINARRAY(AAAA) = Val("&h"+mid$(ihline, 8+i*2,2))
                if AAAA > max_AAAA then max_AAAA=AAAA
                AAAA = AAAA+1
            next i
        endif
    loop until ucase$(ihline) = ":00000001FF"   'ihex end marker
    AAAA = Max_AAAA+1  'set AAAA to max address of hexfile contents + 1

return (0)

end function

function DatasToPortMatrix (byref devname as string) as Ubyte

dim astr as string
dim a as Ubyte
dim i as Ubyte

devname = lcase$(devname)

' search datas of port definitions
restore
do
read astr
loop until lcase$(astr) = "begin devices" or astr = ""
if astr ="" then return (1)

' o.k. found them, now parse the desired ATtiny
do
read astr
astr = lcase$(astr)
loop until (astr = devname) or lcase$(astr) = "end devices"
if astr ="end devices" then return (1)     ' no match found

' device found, now skip the 3 signature bytes
read a, a, a

' load and convert port assignments
for i = 0 to 6
        read DEVPORTS(i)
        if DEVPORTS(i) < &hFF then DEVPORTS(i) = DEVPORTS(i) * 8
next i

return (0)

end function

function SignatureToDevicename (byref S0 as Ubyte, S1 as Ubyte, S2 as Ubyte) as String

dim astr        as string
dim a           as ubyte
dim devname     as string
dim i           as ubyte
dim as ubyte    Sig0, Sig1, Sig2

' find signature-to-devicename mapping for given signature

restore
do
        read astr
loop    until lcase$(astr) = "begin devices" or astr = ""

if astr ="" then return ("DATABASE ERROR")

' o.k. found them, now parse for given signature

do
    read devname                ' read Device Name
    read Sig0, Sig1, Sig2       ' read Device Signature
    read a,a,a,a,a,a,a          ' dummy read further port data
    a = 0
    if (S0 = Sig0) and (S1 = Sig1) and (S2 = Sig2) then a = 1

loop until (a=1) or lcase$(devname) = "end devices"

if devname ="end devices" then return ("UNKNOWN DEVICE")     ' no match found

if left$(devname,2) = "tn" then devname = trim (devname, "tn")   : devname = "ATtiny" + devname
if left$(devname,1) = "m"  then devname = trim (devname, "m")    : devname = "ATmega" + devname
if right$(devname,1)= "A"  then devname = left$(devname, len(devname)-1)

return (devname)

end function

sub ShowDeviceInfo ()
        print "TINY SAFE BOOTLOADER"
        print "VERSION   : "; TSBBUILD
        print "STATUS    : "; WHex(TSBStatus)
        print "SIGNATURE : "; WHex(SIG000,2);" "; WHex(SIG001,2);" "; WHex(SIG002,2)
        print "DEVICE    : "; SignatureToDevicename(SIG000, SIG001, SIG002)
        print "FLASH     : "; FLASHSIZE
        print "APPFLASH  : "; APPFLASH
        print "PAGESIZE  : "; PAGESIZE
        print "EEPROM    : "; EEPROMSIZE
        print "APPJUMP   : "; WHex(APPJUMP,4)
        print "TIMEOUT   : "; TIMEOUT
        print
end sub

sub TSBChecksum()
' Parse loaded and customized TSB-Firmware/Installer in BINARRAY():
'
' (1) Found TSB-Installer = calculate checksum beginning from 2nd page
'     and store in the last 2 bytes prior to TSB-Firmware-segment
' (2) Found TSB- or other firmware =
'     do not change anything

dim i as ushort
dim j as ushort

dim FWChecksum as ushort = &h0000

i = 3   ' skip possible RJMP/JMP in 1st page (TSB-Installer)

do : i = i + 1 : loop until (BINARRAY(i)) < 255 or (i > 256)

if i < 8 then return    ' apparently no TSB-Installer
if i > 256 then return  ' possibly TSB-Firmware, no checksum 2b calc'd

j = i + 2
i = i + 4

BINARRAY(j+0) = 0
BINARRAY(j+1) = 0

BINARRAY(AAAA) = 255

do
    FWChecksum = FWChecksum + BINARRAY(i)
    i = i + 1
loop    until i = AAAA

BINARRAY(j+0) = FWChecksum \ 256
BINARRAY(j+1) = FWChecksum - (FWChecksum \ 256) * 256

'? Whex (FWCHECKSUM)

end sub

'-----------------------------------------------------------------------
'-----------------------------------------------------------------------
'-----------------------------------------------------------------------


'-----------------------------------------------------------------------
' MAIN PROGRAM - PARSE COMMANDLINE
'-----------------------------------------------------------------------

Mainprogram:

if command$(1) = ""     then goto HelpScreen
if __FB_ARGC__ < 2      then goto HelpScreen
astr = lcase$(trim (command$(1), any "-/"))
if astr = "l"   then goto Licensenote
if astr = "h"   or astr = "?" then goto HelpScreen

' 1st argument may be either some COM-Port or AVR-Devicename

astr = lcase$(command$(1))

If      left$(astr,3) <> "com"  and _
        left$(astr,4) <> "/dev" and _
        left$(astr,4) <> "/tty" then goto MakeMode ' make bootloader

Comport = command$(1)         ' user wants to talk to the bootloader

i = 0
Comport = trim (Comport, ":")
do : i = i + 1 : loop until (i = len(Comport)) or (mid$(Comport,i,1) = ":")
if i = (len(Comport)) then Comport = Comport + ":9600"

astr = UCase$(command$(2))          ' check mode switch
astr = trim (astr, any "-/")

If astr = "" then goto ShowInfo
If astr = "?" then goto HelpScreen
If astr = "I" then goto ShowInfo
If astr = "P" then goto PasswordChange
If astr = "T" then goto TimeoutChange
If astr = "EE" then goto EEPROMerase
If astr = "FE" then goto FLASHerase
If astr = "XXX" then goto EmergencyErase

Filename = (command$(3))            ' get filename

If Filename = "" then goto GError

If astr = "FR" then goto FLASHread
If astr = "FW" then goto FLASHwrite
If astr = "FV" then goto FLASHverify

If astr = "ER" then goto EEPROMread
If astr = "EW" then goto EEPROMwrite
if astr = "EV" then goto EEPROMverify

goto GError

'-----------------------------------------------------------------------
' END COMMANDLINE PARSER
'-----------------------------------------------------------------------
'
'
'
'
'-----------------------------------------------------------------------
' MAKE CUSTOMIZED TSB FIRMWARE FLASH FILE FROM FIRMWARE TEMPLATE
'-----------------------------------------------------------------------
' Search internal database for template, make TSB with customized ports.

MakeMode:

AAAA = 0
ADDR = 0
astr = lcase$(astr)

Filename = astr

print
'
' STEP 1:
' First we search the Database for the pre-assembled code template
' of the respective device that matches the given Filename.
'
if DatasToArray (Filename) then
    print "Sorry, this Device is not in the database yet." : print
    goto GError
endif
'
' STEP 2:
' If Template has been successfully loaded, we get the port definitions
' (ports existing, ports i/o) for that device.
'
if DatasToPortMatrix (Filename) then
    print "Matrix error." : print
    goto GError
endif

if AAAA < 250             then goto GError  ' binary way too short
if Len (command$(2)) <> 4 then goto GError  ' port definitions seem ok

' Filename for saving customized tsb
if command$(3) = "" then                ' if no filename specified, save in hex anyway
        Filename = "tsb_" + astr + "_" + _
        lcase$(command$(2)) + "_" +  FWnumber$() + ".hex"
        else Filename = command$(3)     ' otherwise try using filename from commandline
endif
'
' STEP 3:
' Prepare working array of RX/TX addresses
' for the ports of chosen device.
'
astr = UCase$(command$(2))
'valid port name ranges from "A" to "G"
a = asc(mid$(astr,1,1)) : if a < 65 or a > 71 then goto GError
a = a - 65                                      ' result can be 0,1,2,3
'valid port bits range from "0" to "7"
b = val (mid$(astr,2,1))
if b > 7 then              print "Portbit must range from 0 to 7." : goto GError
if DEVPORTS(a) = &hFF then print "Invalid port assignment." : goto GError

RXTXPB(0,0) = DEVPORTS (a) + 0  + b          ' PINx
RXTXPB(1,0) = DEVPORTS (a) + 8  + b          ' DDRx
RXTXPB(2,0) = DEVPORTS (a) + 16 + b          ' PORTx

'ensure that port name ranges from "A" to "G"
a = asc(mid$(astr,3,1)) : if a < 65 or a > 71 then goto GError
a = a - 65
'ensure that port bit  ranges from "0" to "7"
b = val (mid$(astr,4,1))
if b > 7 then              print "Portbit must range from 0 to 7." : goto GError
if DEVPORTS(a) = &hFF then print "Invalid port assignment." : goto GError

RXTXPB(0,1) = DEVPORTS (a) + 0  + b          ' PINx
RXTXPB(1,1) = DEVPORTS (a) + 8  + b          ' DDRx
RXTXPB(2,1) = DEVPORTS (a) + 16 + b          ' PORTx

'for i = 0 to 2 : ? whex(RXTXPB(i,0),2), whex(RXTXPB(i,1),2): next i

print "Make TSB from code template:   "; Filename
print
print "RXD = P"; mid$(astr,1,2); " / TXD = P"; mid$(astr,3,2)
print
'
' STEP 4:
' Search pre-assembled firmware template for dummy port assignments
' (being RX = PINB0/DDRB0/PORTB0 / TX = PINB1/DDRB1/PORTB1)
' and modify these with respect to the desired ports and portbits
'
do until ADDR >= AAAA

    select case BINARRAY(ADDR+1)
    case &h98, &h99, &h9A, &h9B     ' find opcodes of cbi/sbic/sbi/sbis

        select case (BINARRAY (ADDR+0) And &b00000111)
        case 0
            select case (BINARRAY (ADDR+0) And &b11111000)
            case DEVPORTS(1)+0   : BINARRAY(ADDR+0) = RXTXPB(0,0)  ' change PINB
            case DEVPORTS(1)+8   : BINARRAY(ADDR+0) = RXTXPB(1,0)  ' change DDRB
            case DEVPORTS(1)+16  : BINARRAY(ADDR+0) = RXTXPB(2,0)  ' change PORTB
            end select
        case 1
            select case (BINARRAY (ADDR+0) And &b11111000)
            case DEVPORTS(1)+0   : BINARRAY(ADDR+0) = RXTXPB(0,1)  ' change PINB
            case DEVPORTS(1)+8   : BINARRAY(ADDR+0) = RXTXPB(1,1)  ' change DDRB
            case DEVPORTS(1)+16  : BINARRAY(ADDR+0) = RXTXPB(2,1)  ' change PORTB
            end select
        end select
    end select

    ADDR = ADDR + 2
    print Chr$(13) ; "Modifying reference code $" ; WHex(ADDR,4); " ... ";

loop

print "OK"
print
'
' STEP 5:
' Save modified machine code with filename determined above
'
TSBchecksum()   ' calculate actual checksum for TSB-Installer
SaveFromArray()
print "Saved TSB firmware file:   "; filename
Err = 0
end


'-----------------------------------------------------------------------
' EEPROM READ
'-----------------------------------------------------------------------
EEPROMread:

if ActivateTSB(Comport) <> 0 then goto GError

ADDR = 0

SendCommand ("e")

do
        SendCommand(CONFIRM)
        astr = RXBuffer ()
                if Len(astr) < PAGESIZE then goto GError
        for i = 1 To PAGESIZE
        BINARRAY (ADDR) = asc(mid$(astr,i,1))
        print Chr$(13) ; "EEPROM READ  $" ; WHex(ADDR,3); " ... ";
        ADDR = ADDR +1
        next
loop    until ADDR = EEPROMSIZE

print "OK"
AAAA = EEPROMSIZE : SaveFromArray()
print

goto Finished

'-----------------------------------------------------------------------
' EEPROM ERASE (OVERWRITE EEPROM WITH $FF)
'-----------------------------------------------------------------------
EEPROMerase:

if ActivateTSB(Comport) <> 0 then goto GError

print "EEPROM ERASE ... ";

ADDR = 0
SendCommand ("E")

do      until (RXBuffer <> REQUEST) Or ADDR = EEPROMSIZE
        SendCommand (CONFIRM)
        astr = string$(PAGESIZE, Chr$(255))
        SendCommand (astr)
        ADDR = ADDR + PAGESIZE
loop
' ensure we are back on command level
SendCommand (REQUEST)
if RXBuffer <> CONFIRM then goto GError
print "OK"

goto Finished

'-----------------------------------------------------------------------
' EEPROM WRITE
'-----------------------------------------------------------------------
EEPROMwrite:

if LoadToArray <> 0 then goto GError
if ActivateTSB(Comport) <> 0 then goto GError
if AAAA > EEPROMSIZE then print "File too long!" : goto Finished

ADDR = 0
AAAA = (((AAAA-1)\PAGESIZE)+1)*PAGESIZE     'round up to full page

SendCommand ("E")

do      until (RXBuffer <> REQUEST) Or ADDR = AAAA
        SendCommand (CONFIRM)
        astr = ""
        for i = 1 To PAGESIZE
                astr = astr + Chr$(BINARRAY(ADDR))
                ADDR = ADDR + 1
        next i
        SendCommand (astr)

'       for i = 1 To PAGESIZE
'               SendCommand (Chr$(BINARRAY(ADDR)))
'               ADDR = ADDR + 1
'       next i

        print Chr$(13) ; "EEPROM WRITE  $" ; WHex(ADDR-1,3); " ... ";
loop

' ensure we are back on command level

SendCommand (REQUEST)
if RXBuffer <> CONFIRM then goto GError

print "OK"

goto Finished

'-----------------------------------------------------------------------
' EEPROM VERIFY
'-----------------------------------------------------------------------
EEPROMverify:

if ActivateTSB(Comport) <> 0 then goto GError

if LoadToArray <> 0 then goto GError

if AAAA > EEPROMSIZE then print "File too long!" : goto Finished

ADDR = 0

SendCommand ("e")

do
        SendCommand (CONFIRM)
        astr = RXBuffer
        if Len(astr) < PAGESIZE then goto GError
        for i = 1 To PAGESIZE
                print Chr$(13) ; "EEPROM VERIFY  $" ; WHex(ADDR,3); " ... ";
                if BINARRAY (ADDR) <> asc(mid$(astr,i,1)) then Exit do
                ADDR = ADDR+1
        next i
loop    until ADDR = EEPROMSIZE

if ADDR = EEPROMSIZE or right$(astr,1) = CONFIRM then
        print "OK"
        else
        beep
        print "--- ERROR! ---"
endif

goto Finished

'-----------------------------------------------------------------------
' FLASH READ
'-----------------------------------------------------------------------

FLASHread:
if ActivateTSB(Comport) <> 0 then goto GError
ADDR = 0
AAAA = APPFLASH

SendCommand ("f")

do
        SendCommand(CONFIRM)
        astr = RXBuffer
        if len(astr) < PAGESIZE then goto GError
        for i = 1 To PAGESIZE
                BINARRAY (ADDR) = asc(mid$(astr,i,1))
                ADDR = ADDR + 1
        next i
        print chr$(13) ; "FLASH READ  $" ; WHex(ADDR-1,4); " ... ";
loop    until ADDR = APPFLASH

print "OK"
print

' remove unallocated (erased) pages from end of read-back application
do while ((BINARRAY(AAAA)) = 255) and AAAA > 0
        AAAA = AAAA - 1
loop

' recover original reset vector (from appjump in LASTPAGE)
if AAAA > 0 then
        if TINYMEGA = 0 then
                if JMPMODE = 0 then                     ' rjmp on ATtiny

                        if (APPJUMP - &hC000) > (4096 - FLASHSIZE\2) then
                        ' newer TSB with backward rjmp
                        i = APPJUMP - PAGESIZE\2 - (4096 - FLASHSIZE\2)
                        else
                        ' older TSB with forward rjmp
                        i = APPJUMP - PAGESIZE\2
                        endif
                        BINARRAY(1) = i\256
                        BINARRAY(0) = i-BINARRAY(1)*256
                else                                    ' jmp on ATtiny
                        BINARRAY(2) = LASTPAGE(0)
                        BINARRAY(3) = LASTPAGE(1)
                endif
        endif

        SaveFromArray       ' save to file if any byte was read

else

        print "Flash was empty. No file saved."

endif

goto Finished


'-----------------------------------------------------------------------
' FLASH ERASE
'-----------------------------------------------------------------------
' Making use of the Flash Write feature in TSB-FW: starting flash write
' session but without sending data to be written will only erase the
' whole Appflash.

FLASHerase:
if ActivateTSB(Comport) <> 0 then goto GError
SendCommand ("F")
print "FLASH ERASE ... ";
if RXBuffer <> REQUEST then goto GError
SendCommand (REQUEST)       ' other than CONFIRM will do as well
if RXBuffer <> CONFIRM then goto GError
print "OK"
goto Finished

'-----------------------------------------------------------------------
' FLASH WRITE
'-----------------------------------------------------------------------
FLASHwrite:

if ActivateTSB(Comport) <> 0 then goto GError
if LoadToArray <> 0 then goto GError

'round up to full pages (multiple of PAGESIZE)
AAAA = ( ( (AAAA-1) \ PAGESIZE ) +1 ) * PAGESIZE

if AAAA > APPFLASH then print "File too long / Address conflict !" : goto Finished

if TINYMEGA = 0 then
    if Check4SPM () <> 0 then
        print "WARNING: The firmware you are about to upload"
        print "contains the SPM opcode that performs direct flash writes"
        print "and thus may be dangerous for the bootloader."
        print ""
        input "Continue anyway (Y/n) "; astr
        print
    if astr <> "Y" then goto Finished
    endif
endif

ADDR = 0

SendCommand ("F")

if RXBuffer <> REQUEST then goto Gerror

do until ADDR = AAAA
    SendCommand (CONFIRM)       'CONFIRM to bootloader's REQUEST
    astr = ""
    for i = 1 To PAGESIZE
        astr = astr + Chr$(BINARRAY(ADDR))
        ADDR = ADDR + 1
    next
    SendCommand (astr)          'send page data block to bootloader
    print Chr$(13) ; "FLASH WRITE  $" ; WHex(ADDR-1,4); " ... ";
    bstr = RXBuffer

    if bstr <> REQUEST then Exit do     'Error or end of flash override

loop

if (bstr<>REQUEST) and (right$(bstr,1)=CONFIRM) then
    print
    print "INTERNAL VERIFYING ERROR - FLASH ERASE INITIATED !" : goto GError
endif

if (bstr<>REQUEST) and (right$(bstr,1)<>CONFIRM) then
    print
    print "UNDEFINED ERROR WHILST FLASH WRITE!" : goto GError
endif

if ADDR < APPFLASH then
    SendCommand (REQUEST)                       ' finished
    if RXBuffer <> CONFIRM then goto GError     ' not confirmed?
endif

print "OK"

goto Finished

'-----------------------------------------------------------------------
' FLASH VERIFY
'-----------------------------------------------------------------------
FLASHverify:

if ActivateTSB(Comport) <> 0 then goto GError

if LoadToArray <> 0 then goto GError

SendCommand ("f")               ' = read Flash
ADDR = 4

SendCommand (CONFIRM)
' Always skip first 4 bytes with RJMP/JMP
astr = RXBuffer
if Len(astr) < PAGESIZE then goto GError
    for i = 5 To PAGESIZE
        print Chr$(13) ; "FLASH VERIFY  $" ; WHex(ADDR-1,4); " ... ";
        if BINARRAY (ADDR) <> asc(mid$(astr,i,1)) then Exit for
        ADDR = ADDR+1
    next
if ADDR < PAGESIZE then

        print "First Verify Error at $"; WHex(ADDR,4); " - Abort!"
        close : beep : end
endif

if ADDR < AAAA then             ' Compare remaining flash pages
        do
        SendCommand (CONFIRM)
        astr = RXBuffer
        for i = 1 To PAGESIZE
                print chr$(13) ; "FLASH VERIFY  $" ; WHex(ADDR,4); " ... ";
                if BINARRAY (ADDR) <> asc(mid$(astr,i,1)) then Exit do
                if ADDR = AAAA then Exit do
                ADDR = ADDR+1
        next i
        loop    until ADDR = AAAA
endif

if AAAA <= ADDR then
    print "OK"
    else
    print "First Verify Error at $"; WHex(ADDR,4); " - Abort!"
    close : beep : end
endif

end


'-----------------------------------------------------------------------
' PASSWORD CHANGE DIALOG
'-----------------------------------------------------------------------

PasswordChange:

if ActivateTSB(Comport) <> 0 then goto GError
    print using "Enter new Password (max. ### chars) : "; (PAGESIZE-4);
    line input , astr
    if Len(astr) > (PAGESIZE-4) then print "Password too long!": goto GError
    print       "Please re-enter your new Password   : ";
    line input , bstr
    print
    if astr <> bstr then print "Passwords not equal - Nothing changed.": beep: close : end

if astr <> "" then
    print "WARNING: If your Password got lost, you will no longer be able"
    print "to maintain Userdata (Flash, EEPROM, Timeout and Password)."
    print "To regain full access to TSB, you still have option to perform"
    print "an Emergency Erase."
        print
    input "Change the Password (Y/n) "; cstr
    print
    if cstr <> "Y" then print "Nothing changed!": goto Finished

    astr = astr + chr$(255)  ' add $FF terminator to new password
    ' also erase whole LASTPAGE except from APPJUMP and TIMEOUT
    for i = 3 to PAGESIZE - 1 : LASTPAGE(i) = 255 : next i
    for i = 1 to len(astr) : LASTPAGE(2+i) = asc(mid$(astr,i,1)) : next i

    if SendLASTPAGE ()  <> 0 then goto GError
    if VerifyUserData() <> 0 then goto GError
    if GetUserData()    <> 0 then goto GError
    print "NEW PASSWORD: "; PASSWORD

else
    for i = 3 To PAGESIZE-1 : LASTPAGE(i) = 255 : next i    'clear password
    if SendLASTPAGE ()  <> 0 then goto GError
    if VerifyUserData() <> 0 then goto GError
    if GetUserData()    <> 0 then goto GError
    print "Okay, you have chosen to use NO PASSWORD."
endif
Showdeviceinfo()
goto Finished

'-----------------------------------------------------------------------
' TIMEOUT CHANGE (DIALOG OR INLINE)
'-----------------------------------------------------------------------

TimeoutChange:

if ActivateTSB(Comport) <> 0 then goto GError
if command$(3) = "" then
        print      "OLD TIMEOUT BYTE : "; TIMEOUT
        line input "NEW TIMEOUT BYTE : ", astr
else    astr = command$(3)
endif
a = Val(astr)
if a < 8 Or a > 255 then
    print
    print "ERROR: Valid timeouts must range from 8 to 255!"
    print
    print "Nothing changed!": goto Finished
endif
LASTPAGE(2) = a

if SendLASTPAGE()   <> 0 then goto GError
if VerifyUserData() <> 0 then goto GError
if GetUserData()    <> 0 then goto GError

Showdeviceinfo()

goto Finished

'-----------------------------------------------------------------------
' SHOW DEVICE AND BOOTLOADER INFORMATION
'-----------------------------------------------------------------------

ShowInfo:

if ActivateTSB(Comport) <> 0 then goto GError
Showdeviceinfo()
goto Finished

'-----------------------------------------------------------------------
' LAUNCH EMERGENCY ERASE
'-----------------------------------------------------------------------

EmergencyErase:

close 8
sleep 100
if Open COM (Comport + Comset for binary as #8) > 0 then goto GError
sleep 100
print

print #8, "@@@";                        ' probe for normal TSB access

bstr = RXBuffer
if left$(bstr,3) = "@@@" then           ' one-wire echo detected
    OneWireLocalEcho = 1
    bstr = right$(bstr, (len(bstr) - 3))
endif

if len (bstr) then                      ' received device data?
    print "Seems that TSB is still accessible without password!"
    goto Finished
endif
print "WARNING: Emergency Erase deletes all Application Flash"
print "and EEPROM contents, as well as Timeout and Password."
print "This provides for a clean TSB with default values."
print
input "Do you really want to continue (Y/n) "; cstr
print
if cstr <> "Y" then print "Nothing changed!": goto Finished

SendCommand (Chr$(0))
if RXBuffer <> REQUEST then goto GError
SendCommand (CONFIRM)
if RXBuffer <> REQUEST then goto GError
SendCommand (CONFIRM)
print "TSB performing Emergency Erase - Wait some seconds ... "
print

z = Timer + 10 : do until LOC(8) Or (Timer > z) : loop

if RXBuffer <> CONFIRM then goto GError

SendCommand ("c")
bstr = RXBuffer

if Len(bstr) < 32 then goto GError
if right$(bstr,1) <> CONFIRM then goto GError

i = 2
do until (asc(mid$(bstr, i+1, 1)) < 255) Or (i > 255)
    i = i + 1
loop

if i<>32 and i<>64 and i<>128 then goto GError  ' check for usual pagesizes

print "Emergency Erase successfull! TSB restored to defaults."

goto Finished

'-----------------------------------------------------------------------
' HELPSCREEN
'-----------------------------------------------------------------------

HelpScreen:
'                                                                                      < 80 characters
print "-------------------------------------------------------------------------------"
print "Console Tool for TinySafeBoot, the tiny and safe AVR bootloader    SW:";
print left$(__DATE_ISO__,4) + mid$(__DATE_ISO__,6,2) + right$(__DATE_ISO__,2)
print "-------------------------------------------------------------------------------"
print ""
print "Info:         tsb        Show this helpscreen"
print "              tsb -l     Show license note and version info"
print
print "SYNTAX 1:     CONNECT TO BOOTLOADER"
print ""
print "              tsb [Devicename[:Baudrate] {Mode} [Filename] ]"
print "Devicename"
print "COMx          Device name for genuine serial ports under Win/DOS."
print "/dev/ttySx    Original device name for genuine serial ports under Linux."
print "/dev/ttyUSBx  Device name for USB ('virtual') serial ports under Linux."
print "[:Baudrate]   Specify standard baudrate for serial communications."
print "              With no baudrate specified, default of 9600 is chosen."
print "              Try lower baudrates for devices with low clock frequency."
print ""
print "{Mode}        Switches that specify operation mode and target."
print "              Switches not case-sensitive. Prefixes '-' or '/' are optional."
print "ER EE EW EV   EEPROM Read / Erase / Write / Verify"
print "FR FE FW FV   FLASH  Read / Erase / Write / Verify"
print "I             Show bootloader and device info."
print "P             Enter Password changing dialog (TAKE CARE!)."
print "T             Change timeout factor (8...255, approx. 0.1 to some seconds)."
print "XXX           Emergency Erase - kill Userdata and restore Bootloader."
print ""
print "Filename      Filename (or full path) of source or target file."
print "              For write and verify, Intel(R) Hex is automatically recognized."
print "              With read operations, specifying extension '.hex' or '.eep' saves"
print "              the data in a Hex record, other extensions will save raw binary."
print ""
print "Examples:     tsb com1:19200 fw firmware.bin             (Win/DOS)"
print "              tsb com2:38400 ew workingdata.eep          (Win/DOS)"
print "              ./tsb /dev/ttyS0:19200 fw firmware.hex     (Linux)"
print "              ./tsb /dev/ttyUSB0:19200 ev eeprom.eep     (Linux)"
print ""
print "*** Apply RESET or POWER-UP on target device right before TSB session! ***"
print ""
print ""
print "SYNTAX 2:     MAKE CUSTOM TSB FIRMWARE"
print ""
print "              tsb tnxxxx {RxTx} [Filename]   or   tsb mxxxx {RxTx} [Filename]"
print "              Generate TSB for designated ATtiny/ATmega using specified I/O. "
print "              Firmware file will be Hex (.hex) or Binary (other extension)."
print ""
print "Examples:     tsb tn2313 d0d1 mytiny.hex"
print "              Generate TSB installer 'mytiny.hex' for the ATtiny2313"
print "              using Port D0=RXD and D1=TXD for serial communications."
print ""
print "              tsb m8515 e0e0"
print "              Generate TSB for ATmega8515 with bidirectional (One-Wire)"
print "              interface on port E0. Filename automatically chosen."
print ""
end


Licensenote:

print
print "TSB - Console Tool for TinySafeBoot, the tiny and safe AVR bootloader"
print "Copyright (C) Julien Thomas <joytec@gmx.de>"
print ""
print "This program is free software; you can redistribute it and/or modify it"
print "under the terms of the GNU General Public License as published"
print "by the Free Software Foundation; either version 3 of the License"
print "or (at your option) any later version."
print "This program is distributed in the hope that it will be useful,"
print "but WITHOUT ANY WARRANTY; without even the implied warranty"
print "of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."
print "See the GNU General Public License for more details."
print "You should have received a copy of the GNU General Public License"
print "along with this program; if not, see <http://www.gnu.org/licenses/>."
print ""
print "Compiled on " & __DATE_ISO__ " with " & __FB_SIGNATURE__
print
end


'-----------------------------------------------------------------------
' RAW ERROR HANDLING
'-----------------------------------------------------------------------

GError:
close
print : print "ERROR."
print
end

Finished:

DeactivateTSB
print
close
Err = 0
end
'
'-----------------------------------------------------------------------
' DEVICE SIGNATURES AND PORT DEFINITIONS
'-----------------------------------------------------------------------
' Listing contains ALL known ATtinys and ATmegas in question for TSB.
' Bootloader-capable devices are a subset of these.
' Fields containing $FF indicate non-existing ports.
'
data "begin devices"
'                  SIG0  SIG1  SIG2  PINA  PINB  PINC  PIND  PINE  PINF  PING
data "1200"      , &h1E, &h90, &h01, &hFF, &h16, &hFF, &h10, &hFF, &hFF, &hFF
data "2313"      , &h1E, &h91, &h01, &hFF, &h16, &hFF, &h10, &hFF, &hFF, &hFF
data "2323"      , &h1E, &h91, &h02, &hFF, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "2343"      , &h1E, &h91, &h03, &hFF, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "4414"      , &h1E, &h92, &h01, &h19, &h16, &h13, &h10, &hFF, &hFF, &hFF
data "4433"      , &h1E, &h92, &h03, &hFF, &h16, &h13, &h10, &hFF, &hFF, &hFF
data "4434"      , &h1E, &h93, &h03, &h19, &h16, &h13, &h10, &hFF, &hFF, &hFF
data "8515"      , &h1E, &h93, &h01, &h19, &h16, &h13, &h10, &hFF, &hFF, &hFF
data "8535"      , &h1E, &h93, &h03, &h19, &h16, &h13, &h10, &hFF, &hFF, &hFF
data "m1280"     , &h1E, &h97, &h03, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m1281"     , &h1E, &h97, &h04, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m1284"     , &h1E, &h97, &h06, &h00, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m1284P"    , &h1E, &h97, &h05, &h00, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m1284RFR2" , &h1E, &hA7, &h03, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m128A"     , &h1E, &h97, &h02, &h19, &h16, &h13, &h10, &h01, &h00, &hFF
data "m128"      , &h1E, &h97, &h02, &h19, &h16, &h13, &h10, &h01, &h00, &hFF
data "m128RFA1"  , &h1E, &hA7, &h01, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m128RFR2"  , &h1E, &hA7, &h02, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m162"      , &h1E, &h94, &h04, &h19, &h16, &h13, &h10, &h05, &hFF, &hFF
data "m164A"     , &h1E, &h94, &h0F, &h00, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m164PA"    , &h1E, &h94, &h0A, &h00, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m164P"     , &h1E, &h94, &h0A, &h00, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m165A"     , &h1E, &h94, &h10, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m165PA"    , &h1E, &h94, &h07, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m165P"     , &h1E, &h94, &h07, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m168A"     , &h1E, &h94, &h06, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m168"      , &h1E, &h94, &h06, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m168PA"    , &h1E, &h94, &h0B, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m168P"     , &h1E, &h94, &h0B, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m169A"     , &h1E, &h94, &h11, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m169PA"    , &h1E, &h94, &h05, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m169P"     , &h1E, &h94, &h05, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m16A"      , &h1E, &h94, &h03, &h19, &h16, &h13, &h10, &hFF, &hFF, &hFF
data "m16"       , &h1E, &h94, &h03, &h19, &h16, &h13, &h10, &hFF, &hFF, &hFF
data "m16HVA"    , &h1E, &h94, &h0C, &h00, &h03, &h06, &hFF, &hFF, &hFF, &hFF
data "m16HVB"    , &h1E, &h94, &h0D, &h00, &h03, &h06, &hFF, &hFF, &hFF, &hFF
data "m16M1"     , &h1E, &h94, &h84, &hFF, &h03, &h06, &h09, &h0C, &hFF, &hFF
data "m16U2"     , &h1E, &h94, &h89, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m16U4"     , &h1E, &h94, &h88, &hFF, &h03, &h06, &h09, &h0C, &h0F, &hFF
data "m2560"     , &h1E, &h98, &h01, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m2561"     , &h1E, &h98, &h02, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m2564RFR2" , &h1E, &hA8, &h03, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m256RFR2"  , &h1E, &hA8, &h02, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m324A"     , &h1E, &h95, &h15, &h00, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m324PA"    , &h1E, &h95, &h11, &h00, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m324P"     , &h1E, &h95, &h08, &h00, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m3250A"    , &h1E, &h95, &h0E, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m3250"     , &h1E, &h95, &h06, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m3250PA"   , &h1E, &h95, &h0E, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m3250P"    , &h1E, &h95, &h0E, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m325A"     , &h1E, &h95, &h05, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m325"      , &h1E, &h95, &h05, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m325PA"    , &h1E, &h95, &h05, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m325P"     , &h1E, &h95, &h0D, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m328"      , &h1E, &h95, &h14, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m328P"     , &h1E, &h95, &h0F, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m3290A"    , &h1E, &h95, &h0C, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m3290"     , &h1E, &h95, &h04, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m3290PA"   , &h1E, &h95, &h0C, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m3290P"    , &h1E, &h95, &h0C, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m329A"     , &h1E, &h95, &h03, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m329"      , &h1E, &h95, &h03, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m329PA"    , &h1E, &h95, &h0B, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m329P"     , &h1E, &h95, &h0B, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m32A"      , &h1E, &h95, &h02, &h19, &h16, &h13, &h10, &hFF, &hFF, &hFF
data "m32C1"     , &h1E, &h95, &h86, &hFF, &h03, &h06, &h09, &h0C, &hFF, &hFF
data "m32"       , &h1E, &h95, &h02, &h19, &h16, &h13, &h10, &hFF, &hFF, &hFF
data "m32HVB"    , &h1E, &h95, &h10, &h00, &h03, &h06, &hFF, &hFF, &hFF, &hFF
data "m32HVBrevB", &h1E, &h95, &h10, &h00, &h03, &h06, &hFF, &hFF, &hFF, &hFF
data "m32M1"     , &h1E, &h95, &h84, &hFF, &h03, &h06, &h09, &h0C, &hFF, &hFF
data "m32U2"     , &h1E, &h95, &h8A, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m32U4"     , &h1E, &h95, &h87, &hFF, &h03, &h06, &h09, &h0C, &h0F, &hFF
data "m406"      , &h1E, &h95, &h07, &h00, &h03, &hFF, &h09, &hFF, &hFF, &hFF
data "m48A"      , &h1E, &h92, &h05, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m48"       , &h1E, &h92, &h05, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m48PA"     , &h1E, &h92, &h0A, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m48P"      , &h1E, &h92, &h0A, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m640"      , &h1E, &h96, &h08, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m644A"     , &h1E, &h96, &h09, &h00, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m644"      , &h1E, &h96, &h09, &h00, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m644PA"    , &h1E, &h96, &h0A, &h00, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m644P"     , &h1E, &h96, &h0A, &h00, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m644RFR2"  , &h1E, &hA6, &h03, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m6450A"    , &h1E, &h96, &h06, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m6450"     , &h1E, &h96, &h06, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m6450P"    , &h1E, &h96, &h06, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m645A"     , &h1E, &h96, &h05, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m645"      , &h1E, &h96, &h05, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m645P"     , &h1E, &h96, &h05, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m6490A"    , &h1E, &h96, &h04, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m6490"     , &h1E, &h96, &h04, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m6490P"    , &h1E, &h96, &h04, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m649A"     , &h1E, &h96, &h03, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m649"      , &h1E, &h96, &h03, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m649P"     , &h1E, &h96, &h0B, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m64A"      , &h1E, &h96, &h02, &h19, &h16, &h13, &h10, &h01, &h00, &hFF
data "m64C1"     , &h1E, &h96, &h86, &hFF, &h03, &h06, &h09, &h0C, &hFF, &hFF
data "m64"       , &h1E, &h96, &h02, &h19, &h16, &h13, &h10, &h01, &h00, &hFF
data "m64HVE2"   , &h1E, &h96, &h10, &h00, &h03, &hFF, &hFF, &hFF, &hFF, &hFF
data "m64M1"     , &h1E, &h96, &h84, &hFF, &h03, &h06, &h09, &h0C, &hFF, &hFF
data "m64RFR2"   , &h1E, &hA6, &h02, &h00, &h03, &h06, &h09, &h0C, &h0F, &h12
data "m8515"     , &h1E, &h93, &h06, &h19, &h16, &h13, &h10, &h05, &hFF, &hFF
data "m8535"     , &h1E, &h93, &h08, &h19, &h16, &h13, &h10, &hFF, &hFF, &hFF
data "m88A"      , &h1E, &h93, &h0A, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m88"       , &h1E, &h93, &h0A, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m88PA"     , &h1E, &h93, &h0F, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m88P"      , &h1E, &h93, &h0F, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "m8A"       , &h1E, &h93, &h07, &hFF, &h16, &h13, &h10, &hFF, &hFF, &hFF
data "m8"        , &h1E, &h93, &h07, &hFF, &h16, &h13, &h10, &hFF, &hFF, &hFF
data "m8HVA"     , &h1E, &h93, &h10, &h00, &h03, &h06, &hFF, &hFF, &hFF, &hFF
data "m8U2"      , &h1E, &h93, &h89, &hFF, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "tn10"      , &h1E, &h90, &h03, &hFF, &h00, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn13A"     , &h1E, &h90, &h07, &hFF, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn13"      , &h1E, &h90, &h07, &hFF, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn1634"    , &h1E, &h94, &h12, &h0F, &h0B, &h07, &hFF, &hFF, &hFF, &hFF
data "tn167"     , &h1E, &h94, &h87, &h00, &h03, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn20"      , &h1E, &h91, &h0F, &h00, &h04, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn2313A"   , &h1E, &h91, &h0A, &h19, &h16, &hFF, &h10, &hFF, &hFF, &hFF
data "tn2313"    , &h1E, &h91, &h0A, &h19, &h16, &hFF, &h10, &hFF, &hFF, &hFF
data "tn24A"     , &h1E, &h91, &h0B, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn24"      , &h1E, &h91, &h0B, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn25"      , &h1E, &h91, &h08, &hFF, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn261A"    , &h1E, &h91, &h0C, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn261"     , &h1E, &h91, &h0C, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn26"      , &h1E, &h91, &h09, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn28"      , &h1E, &h91, &h07, &h19, &h16, &hFF, &h10, &hFF, &hFF, &hFF
data "tn40"      , &h1E, &h92, &h0E, &h00, &h04, &h1B, &hFF, &hFF, &hFF, &hFF
data "tn4313"    , &h1E, &h92, &h0D, &h19, &h16, &hFF, &h10, &hFF, &hFF, &hFF
data "tn43U"     , &h1E, &h92, &h0C, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn441"     , &h1E, &h92, &h15, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn44A"     , &h1E, &h92, &h07, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn44"      , &h1E, &h92, &h07, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn45"      , &h1E, &h92, &h06, &hFF, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn461A"    , &h1E, &h92, &h08, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn461"     , &h1E, &h92, &h08, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn48"      , &h1E, &h92, &h09, &h0C, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "tn4"       , &h1E, &h8F, &h0A, &hFF, &h00, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn5"       , &h1E, &h8F, &h09, &hFF, &h00, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn828"     , &h1E, &h93, &h14, &h00, &h04, &h08, &h0C, &hFF, &hFF, &hFF
data "tn841"     , &h1E, &h93, &h15, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn84A"     , &h1E, &h93, &h0C, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn84"      , &h1E, &h93, &h0C, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn85"      , &h1E, &h93, &h0B, &hFF, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn861A"    , &h1E, &h93, &h0D, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn861"     , &h1E, &h93, &h0D, &h19, &h16, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn87"      , &h1E, &h93, &h87, &h00, &h03, &hFF, &hFF, &hFF, &hFF, &hFF
data "tn88"      , &h1E, &h93, &h11, &h0C, &h03, &h06, &h09, &hFF, &hFF, &hFF
data "tn9"       , &h1E, &h90, &h08, &hFF, &h00, &hFF, &hFF, &hFF, &hFF, &hFF
data "end devices"
'
'-----------------------------------------------------------------------
' INCLUDE DATA-BASE OF PRE-ASSEMBLED TSB-TEMPLATES FOR SUPPORTED DEVICES
'-----------------------------------------------------------------------
'
data "begin tt"
#include "datas.bas"
data "end tt"
'
'-----------------------------------------------------------------------
'-----------------------------------------------------------------------
'-----------------------------------------------------------------------
' END BASIC CODE FOR TINYSAFEBOOT CONSOLE TOOL
'-----------------------------------------------------------------------
'-----------------------------------------------------------------------
'-----------------------------------------------------------------------
