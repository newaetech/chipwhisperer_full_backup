import chipwhisperer as cw
scope = cw.scope()

LOGGING = True
SHORT_STREAM = False

#1. short stream that works:
if SHORT_STREAM:
    SAMPLES = 33000
    STREAM_SEGMENT_SIZE = 8192
    STREAM_SEGMENT_AVAILABLE_THRESHOLD = 8192 # tends to work
    #STREAM_SEGMENT_AVAILABLE_THRESHOLD = 8192 # can result in underflow error (data being read before it's ready)

#2. long stream that doesn't work:
else:
    SAMPLES = 330000
    STREAM_SEGMENT_SIZE = 32000
    STREAM_SEGMENT_AVAILABLE_THRESHOLD = 32000

def read_reg(addr, length):
    return scope.qtadc.sc.sendMessage(0x80, addr, maxResp=length)

def write_reg(addr, data):
    scope.qtadc.sc.sendMessage(0xc0, addr, data)

def get_fpga_buildtime():
    """Returns date and time when FPGA bitfile was generated.
    """
    raw = read_reg(11, 4)
    # definitions: Xilinx XAPP1232
    day = raw[3] >> 3
    month = ((raw[3] & 0x7) << 1) + (raw[2] >> 7)
    year = ((raw[2] >> 1) & 0x3f) + 2000
    hour = ((raw[2] & 0x1) << 4) + (raw[1] >> 4)
    minute = ((raw[1] & 0xf) << 2) + (raw[0] >> 6)
    return "FPGA build time: {}/{}/{}, {}:{}".format(month, day, year, hour, minute)


def reset_fpga():
    write_reg(28, [1])
    write_reg(28, [0])

# set stream segment available threshold:
write_reg(35, list(int.to_bytes(STREAM_SEGMENT_AVAILABLE_THRESHOLD, length=4, byteorder='little')))

# disable clipping errors:
write_reg(45, [1])

print(get_fpga_buildtime())

