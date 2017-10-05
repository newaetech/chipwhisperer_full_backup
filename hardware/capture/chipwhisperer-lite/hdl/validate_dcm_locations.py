#
# This script checks the two DCM blocks have been positioned in a useful location. Currently the system
# requires they are written to a common address, otherwise VERY BAD things happen. In the future
# the system might be built to deal with multiple blocks changing...
#


from subprocess import call
import os
import pickle
import sys


#Windows Path
#XILINX_DIR = "C:\\Xilinx\\14.4\\ISE_DS\\ISE\\bin\\nt64\\"
#DESIGN_BASE = "cwlite_ise\\cwlite_interface"

#Linux Path
XILINX_DIR = "/opt/Xilinx/14.7/ISE_DS/ISE/bin/lin64/"
DESIGN_BASE = "cwlite_ise/cwlite_interface"

def decodeFARMIN(data, indx):
    writelen = data[indx] & 0x001F
    indx += 1

    d = data[indx]

    bram = (0xC000 & d) >> 14
    minor = (0x03FF & d)

    print "%04x: FAR_MIN Data: BRAM %d, Minor %d"%(d, bram, minor)

    fpga_addr['bram'] = bram
    fpga_addr['bram_minor'] = minor

    indx += 1

    return indx

def decodeFARMAJ(data, indx):
    writelen = data[indx] & 0x001F
    indx += 1

    d = data[indx]

    block = (d & 0xf000) >> 12
    row = (d & 0x0f00) >> 8
    major = (d & 0x0ff)

    print "%04x: FAR_MAJ Data: Block %d, Row %d, Major %d"%(d, block, row, major)
    indx += 1

    fpga_addr['major'] = major
    fpga_addr['row'] = row
    fpga_addr['block'] = block

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

def cleanup():
    dellist = ["diffbits.rbt", "diffscr.scr", "diff.pcf", "diff.ncd",
               "diffbits.bgn", "diffbits.bit", "diffbits.drc", "diffbits.ll", "diffbits_bitgen.xwbt",
               "usage_statistics_webtalk.html", "webtalk.log", "xilinx_device_details.xml",
               "diffscr.scr", "reversebits.bit", "reversebits.drc", "reversebits.bgn",
               "reversebits.rbt", "reversebits_bitgen.xwbt",
               ]
    

    for f in dellist:
        try:
            os.remove(f)
        except OSError, e:
            pass

    
def makeDiffBits_phase(ATTR_PHASE, comp = "oadc/genclocks/DCM_extclock_gen", cfgString = "CLKDV_DIVIDE:2.0 CLKIN_DIVIDE_BY_2:FALSE CLKOUT_PHASE_SHIFT:VARIABLE CLK_FEEDBACK:2X DESKEW_ADJUST:5 DFS_FREQUENCY_MODE:LOW DLL_FREQUENCY_MODE:LOW DSS_MODE:NONE DUTY_CYCLE_CORRECTION:TRUE PSCLKINV:PSCLK PSENINV:PSEN PSINCDECINV:PSINCDEC RSTINV:RST STARTUP_WAIT:FALSE VERY_HIGH_FREQUENCY:FALSE", reverse=False):
    script = "post attr main\n"
    script += "setattr main edit-mode Read-Write\n"
    script += "setattr main case_sensitive off\n"
    script += "unselect -all\n"
    script += "select comp '%s'\n"%comp
    script += "post block\n"
    script += 'setattr comp %s PHASE_SHIFT "%d"\n'%(comp, ATTR_PHASE)
    script += 'setattr comp %s Config "%s"\n'%(comp, cfgString)
    script += "block apply\n"
    script += "end block\n"
    script += 'save -w design "diff.ncd" "diff.pcf"\n'
    script += "exit\n"
    script += "end\n"

    f = open("diffscr.scr", "w")
    f.write(script)
    f.close()
    
    call([XILINX_DIR + "fpga_edline", DESIGN_BASE+".ncd", DESIGN_BASE+".pcf", "-p", "diffscr"])

    if reverse:
        call([XILINX_DIR + "bitgen", "-g", "ActiveReconfig:Yes", "diff.ncd", "diffbits.bit", "-w"])
        call([XILINX_DIR + "bitgen", "-g", "ActiveReconfig:Yes", "-r", "diffbits.bit", "-w", DESIGN_BASE+".ncd", "reversebits.bit", "-b"])
    else:
        call([XILINX_DIR + "bitgen", "-g", "ActiveReconfig:Yes", "-r", DESIGN_BASE+".bit", "-w", "diff.ncd", "diffbits.bit", "-b", "-l"])

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

def get_addresses(data):
	showData = False

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
		    if showData:
		        print "%04x :CFG DATA"%data[1][i]
		    i += 1

		if showData is False:
		    print "...Data Frames Not Printed..."

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


fpga_addr = {'block':-1, 'row':-1, 'major':-1, 'bram':-1, 'bram_minor':-1}
cleanup()
makeDiffBits_phase(-20, "reg_clockglitch/gc/DCM_extclock_gen2")
p = parseRbt()
get_addresses(p)
fpga_addr1  = fpga_addr.copy()

fpga_addr = {'block':-1, 'row':-1, 'major':-1, 'bram':-1, 'bram_minor':-1}
cleanup()
makeDiffBits_phase(-20, "reg_clockglitch/gc/DCM_extclock_gen")
p = parseRbt()
get_addresses(p)
fpga_addr2  = fpga_addr.copy()

print "*****************************************************************"
print "*****************************************************************"
print "*****************************************************************"
print "*****************************************************************"
print ""
print ""

if fpga_addr1 != fpga_addr2:
    print "DCMs LOC'd to invalid locations, PR will fail."
    print fpga_addr1
    print fpga_addr2
    print ""
    print ""
    sys.exit(-1)

else:
    print "DCMs LOC'd to compatable locations, OK to continue"

print ""
print ""
    


