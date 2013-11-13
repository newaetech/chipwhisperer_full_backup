
def decodeFARMIN(data, indx):
    writelen = data[indx] & 0x001F
    indx += 1

    d = data[indx]

    bram = (0xC000 & d) >> 14
    minor = (0x03FF & d)

    print "%04x: BRAM %d, Minor %d"%(d, bram, minor)

    indx += 1

    return indx

def decodeFARMAJ(data, indx):
    writelen = data[indx] & 0x001F
    indx += 1

    d = data[indx]

    block = (d & 0xf000) >> 12
    row = (d & 0x0f00) >> 8
    major = (d & 0x0ff)

    print "%04x: Block %d, Row %d, Major %d"%(d, block, row, major)
    indx += 1

    if writelen == 2:
        fakedata = [1, data[indx]]
        decodeFARMIN(fakedata, 0)
        indx += 1
    
    return indx

regstr = [["CRC", None],
          ["FAR_MAJ", decodeFARMAJ],
          ["FAR_MIN", None],
          ["FDRI", None],
          ["FDRO", None],
          ["CMD", None],
          ["CTL", None],
          ["MASK", None],
          ["STAT", None],
          ["LOUT", None],
          ["COR1", None],
          ["COR2", None],
          ["PWRDN_REG", None],
          ["FLR", None],
          ["IDCODE", None],
          ["CWDT", None],
          ["HC_OPT_REG", None],
          ["CSBO", None],
          ["GENERAL1", None],
          ["GENERAL2", None],
          ["GENERAL3", None],
          ["GENERAL4", None],
          ["GENERAL5", None],
          ["MODE_REG", None],
          ["PU_GWE", None],
          ["PU_GTS", None],
          ["MFWR", None],
          ["CCLK_FREQ", None],
          ["SEU_OPT", None],
          ["EXP_SIGN", None],
          ["RDBK_SIGN", None],
          ["BOOTSTS", None],
          ["EYE_MASK", None],
          ["CRC_REG", None]]

def parseRbt(bgtname="diffbits.rbt"):
    f = open(bgtname, "r")

    bitsnow = False

    data = []

    for line in f:
        if bitsnow:
            data.append(int(line, 2))
        elif "Part" in line:
            part = line.split()[1]
        elif "1111111111111111" in line:
            data.append(int(line, 2))
            bitsnow = True

    return (part, data)

data = parseRbt("diffbits.rbt")
#data = parseRbt("interface.rbt")

run = True
synced = False
i = 0
while run:
    inc = True

    d = data[1][i]
    s = ""

    dmy = 0
    while(d == 0xffff):
        dmy += 1
        i += 1
        d = data[1][i]

    nop = 0
    while(d == 0x2000):
        nop += 1
        i += 1
        try:
            d = data[1][i]
        except IndexError:
            break
    
    null = 0
    while(d == 0x0000):
        null += 1
        i += 1
        d = data[1][i]

    if dmy > 0:
        print "FFFF: Dummy (x %d)"%dmy
        inc = False
    elif nop > 0:
        print "2000: Nop (x %d)"%nop
        inc = False
    elif null > 0:
        d = 0x0000
        print "0000: Null (x %d)"%null
        inc = False

    elif ((d & 0xE000) == 0x2000) & synced:
        s = "TYPE1:"
        if (d & 0x1800) == 0x1000:
            s += "WRITE to "
        elif (d & 0x1800) == 0x0800:
            s += "READ from"

        addr = (d & 0x07E0) >> 5

        try:
            s += "%s"%regstr[addr][0]
        except IndexError:
            s += "0x%02x"%addr

        num = d & 0x001F
        s += " %d words"%num
        print "%04x: %s"%(d, s)

        if regstr[addr][1] is None:
            i += 1
            while num:
                d = data[1][i]
                num -= 1
                s = "DATA"
                print "%04x: %s"%(d, s)
                i += 1
        else:
            i = regstr[addr][1](data[1], i)

        inc = False

    elif ((d & 0xE000) == 0x4000) & synced:
        s = "TYPE2:"
        if (d & 0x1800) == 0x1000:
            s += "WRITE to "
        elif (d & 0x1800) == 0x0800:
            s += "READ from "

        addr = (d & 0x07E0) >> 5
        try:
            s += "%s"%regstr[addr][0]
        except IndexError:
            s += "0x%02x"%addr
        print "%04x: %s"%(d, s)

        i += 1
        print "%04x"%data[1][i]
        
        wordcnt = (data[1][i] << 16) | data[1][i+1]
        print "%04x: %d frames"%(data[1][i+1], wordcnt)
        i += 2

        for q in range(0, wordcnt):
            #print "%04x :CFG DATA"%data[1][i]
            i += 1

        inc = False

    elif d == 0xaa99:
        s = "SYNC1"
    elif d == 0x5566:
        s = "SYNC2"
        synced = True
    elif d == 0x000d:
        s = "DESYNC"
        synced = False


    #Generic Printing of normal cases
    if inc:
        i += 1
        print "%04x: %s"%(d, s)

    if i == len(data[1]):
        run = False
