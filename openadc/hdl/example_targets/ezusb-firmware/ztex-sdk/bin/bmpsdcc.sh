#!/bin/sh

if [ "$1" = ""  ]; then
    echo "Usage: $0 <C file> <bmp flags> <sdcc flags>"
    exit 1
fi

SDCC=$(which sdcc)
if [ -z "$SDCC" ]; then
    echo "E: bmpsdcc: sdcc is not in PATH."
    echo "I: sdcc is not related with ztex and ships with many Linux distributions or "
    echo "   is available at http://sdcc.sourceforge.net/ ."
    exit 1
fi

exe=""
if [ "$WINDIR" != "" ]; then
    exe=".exe"
fi    

BINDIR=`dirname $0`
BMP=bmp
if [ -x "$BINDIR/bmp$exe" ]; then
    BMP="$BINDIR/bmp$exe"
fi    

if [ -f "$BINDIR/peeph.def" ]; then
    PEEPH="--peep-file $BINDIR/peeph.def"
elif [ -f "/usr/local/share/ztex/peeph.def" ]; then
    PEEPH="$PEEPH --peep-file /usr/local/share/ztex/peeph.def"
elif [ -f "/usr/share/ztex/peeph.def" ]; then
    PEEPH="$PEEPH --peep-file /usr/share/ztex/peeph.def"
fi    

inp="$1"
if [ ! -f "$inp" -a -f "$inp.c" ]; then 
    inp="$1.c"
fi
base=${inp%*.c}    

if [ ! -f "$inp" ]; then 
    echo "File not found: $inp"
    exit 2
fi    

bytePad=""
while true; do
    if [ "$LINEINFO" = "no" ]; then
        li='//line %2 "%1"'
    else
        li='#line %2 "%1"'
    fi
    $BMP -c -l "$li" $2 $bytePad $inp -o $base.tmp.c
    ec="$?"
    if [ "$ec" != 0 ]; then
	exit $ec
    fi

    "$SDCC" -mmcs51 --code-loc 0x0200 --code-size 0x3800 --xram-loc 0x3A00 --xram-size 0x600 --iram-size 256 $PEEPH $3 $base.tmp.c -o $base.ihx
    ec="$?"
    if [ "$ec" != 0 ]; then
	exit $ec
    fi

    addr=`grep ".*:.* _DeviceDescriptor" $base.map | (
        read a b c d
        if [ "$c" = "_DeviceDescriptor" ]; then
    	    echo 0x${b#*:}
    	else
    	    echo 0x${a#*:}
    	fi
    )`    
    echo "Addr=$addr"
    if [ $((addr & 1)) = "0" ]; then
	exit 0
    else
	if [ "$bytePad" != "" ]; then
	    echo "USB Descriptors not word aligned, -DPAD_BYTE has no effect"
	    exit 3
	else
	    bytePad="-D PAD_BYTE"
	fi
    fi
done    
