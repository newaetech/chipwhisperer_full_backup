#!/bin/bash

USAGE="run_openocd.sh [-h|--help] [-p|--program path/to/fw.elf] husky|pro|lite|nano|bergen jtag|swd -- OPENOCD_ARGS"
EXAMPLE="run_openocd.sh husky jtag -- -f 'target/stm32f3x.cfg'"
EXAMPLE2="run_openocd.sh -p hardware/victims/firmware/simpleserial-aes/simpleserial-aes-CWLITEARM.elf husky jtag -- -f 'target/stm32f3x.cfg'"

print_help () {
    echo $USAGE
    echo $EXAMPLE
    echo $EXAMPLE2
    exit
}

enable_MPSSE() {
    python -c "import chipwhisperer as cw; scope = cw.scope(); scope.enable_MPSSE()"
    sleep 1
}

SCRIPT_ARGS=()
OCD_ARGS=()
progfile=""
DRYRUN=""

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

echo $#

if [[ $# -lt 4 ]]; then
    print_help
fi

while [[ $1 != '--' ]]; do
    case $1 in
        -h|--help)
            print_help
            ;;
        -p|--program)
            progfile=$2
            shift
            shift
            ;;
        *)
            SCRIPT_ARGS+=("$1")
            shift
            ;;
    esac
done

shift

OCD_ARGS="$@"

if [[ progfile != "" ]]; then
    OCD_ARGS+=" -c 'init; targets; halt; program $progfile verify reset exit'"
fi

MODE=${SCRIPT_ARGS[1]}

if [[ $MODE != "jtag" ]] && [[ $MODE != "swd" ]]; then
    echo "Invalid mode $MODE"
    print_help
fi

case ${SCRIPT_ARGS[0]} in
    husky)
        PID=0xace5
        python -c "import chipwhisperer as cw; scope = cw.scope(); scope.enable_MPSSE(husky_userio='$MODE')"
        sleep 1
        ;;
    pro)
        PID=0xace3
        enable_MPSSE
        ;;
    lite)
        PID=0xace2
        enable_MPSSE
        ;;
    nano)
        PID=0xace0
        enable_MPSSE
        ;;
    bergen)
        PID=0xc310
        python -c "import chipwhisperer as cw; scope = cw.target(None, cw.targets.CW310); scope.enable_MPSSE()"
        sleep 1
        ;;
esac

cmd="openocd -s $SCRIPT_DIR -f cw_openocd.cfg -c 'ftdi vid_pid 0x2b3e $PID' -c 'transport select $MODE' $OCD_ARGS"

echo $cmd

bash -c "$cmd"

# echo "SCRIPT_ARGS=${SCRIPT_ARGS[@]} OCD_ARGS=$OCD_ARGS PROG=$progfile"
