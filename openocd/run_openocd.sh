#!/bin/bash

USAGE="run_openocd.sh [-l|--list-targets] [-h|--help] [-p|--program path/to/fw.elf] [--no-user-io] husky|pro|lite|nano|bergen jtag|swd -- OPENOCD_ARGS"
EXAMPLE="example: ./run_openocd.sh husky jtag -- -f 'target/stm32f3x.cfg'"
EXAMPLE2="./run_openocd.sh -p hardware/victims/firmware/simpleserial-aes/simpleserial-aes-CWLITEARM.elf husky jtag -- -f 'target/stm32f3x.cfg'"

if [[ -z $OPENOCD_PATH ]]; then
    OPENOCD_PATH=`which openocd`
fi

print_help () {
    echo $USAGE
    echo "Example:"
    echo '    ' $EXAMPLE2
    echo "Set OPENOCD_PATH for custom openocd binary"
    exit
}

list_targets () {
    echo "Some useful targets for ChipWhisperer:"
    echo "    " "target/stm32f3x.cfg, target/stm32f4x.cfg, etc"
    echo "    " "target/at91sam4sXX.cfg, ./fe310.cfg, target/kx.cfg (k82 target)"
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

# if [[ $# -lt 4 ]]; then
#     print_help
# fi

while [[ $1 != '--' ]]; do
    if [[ $# -lt 1 ]]; then
        print_help
    fi
    case $1 in
        -h|--help)
            print_help
            ;;
        -l|--list-targets)
            list_targets
            ;;
        -p|--program)
            progfile=$2
            shift
            shift
            ;;
        --no-user-io)
            HUSKY_UIO="NO"
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

if [[ $progfile != "" ]]; then
    OCD_ARGS+=" -c 'init; targets; halt; program $progfile verify reset exit'"
fi

MODE=${SCRIPT_ARGS[1]}

if [[ $MODE != "jtag" ]] && [[ $MODE != "swd" ]]; then
    echo "Invalid mode $MODE"
    print_help
fi

echo -n "Setting up MPSSE mode..."
case ${SCRIPT_ARGS[0]} in
    husky)
        PID=0xace5
        if [[ $HUSKY_UIO == "NO" ]]; then
            enable_MPSSE
        else
            python -c "import chipwhisperer as cw; scope = cw.scope(); scope.enable_MPSSE(husky_userio='$MODE')"
            sleep 1
        fi
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
echo " Done"


cmd="$OPENOCD_PATH -s $SCRIPT_DIR -f cw_openocd.cfg -c 'ftdi vid_pid 0x2b3e $PID' -c 'transport select $MODE' $OCD_ARGS"

echo running $cmd

bash -c "$cmd"

if [[ $progfile != "" ]]; then
    echo -n "Disabling MPSSE mode..."
    python -c "import chipwhisperer as cw; scope = cw.scope(); scope.enable_MPSSE(0)"
    echo " Done"
fi

# echo "SCRIPT_ARGS=${SCRIPT_ARGS[@]} OCD_ARGS=$OCD_ARGS PROG=$progfile"
