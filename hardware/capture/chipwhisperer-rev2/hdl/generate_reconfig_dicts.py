from subprocess import call
import os
import pickle


XILINX_DIR = "C:\\Xilinx\\14.4\\ISE_DS\\ISE\\bin\\nt64\\"
DESIGN_BASE = "ztex_rev2_1.11c_ise\\interface"

def cleanup():
    dellist = ["diffbits.rbt", "diffscr.scr", "diff.pcf", "diff.ncd",
               "diffbits.bgn", "diffbits.bit", "diffbits.drc", "diffbits_bitgen.xwbt",
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
        call([XILINX_DIR + "bitgen", "-w", "diff.ncd", "diffbits.bit"])
        call([XILINX_DIR + "bitgen", "-g", "ActiveReconfig:Yes", "-r", "diffbits.bit", "-w", DESIGN_BASE+".ncd", "reversebits.bit", "-b"])
    else:
        call([XILINX_DIR + "bitgen", "-g", "ActiveReconfig:Yes", "-r", DESIGN_BASE+".bit", "-w", "diff.ncd", "diffbits.bit", "-b"])

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

def updateDict(d, parsed, param, descstr="", locationstr=""):
    try:
        d["part"]
    except KeyError:
        d["part"] = parsed[0]
        d["crc32"] = 0
        d["desc"] = descstr
        d["location"] = locationstr
        d["base"] = parsed[1]
        d["values"] = {}

    if d["part"] != parsed[0]:
        raise ValueError("INVALID PART!?")

    if param is not None:
        d["values"][param] = []
        for idx, val in enumerate(parsed[1]):
            try:
                if val != d["base"][idx]:
                    d["values"][param].append( (idx, val) )
            except IndexError, e:
                print "Number of changes differs from 'base'! Make sure phase range starts at non-default"
                raise IndexError(e)
    

#for ATTR_PHASE in range(-255, 256):

#saveDict = {}            
#for ATTR_PHASE in range(-255, -253):
#    cleanup()
#    makeDiffBits_phase(ATTR_PHASE)
#    p = parseRbt()
#    updateDict(saveDict, p, ATTR_PHASE, "DCM Phase Adjust", "oadc/genclocks/DCM_extclock_gen")


comp = "reg_clockglitch/gc/DCM_extclock_gen"
saveDict = {}
cleanup()

#This inits the "base" reference bitfile, which is the partial reconfig bitstream required
#to bring system BACK to normal state. This MUST be the base state, otherwise the system
#will never record the required changes to bring the FPGA back to initial state
makeDiffBits_phase(-1, comp, reverse=True)
p = parseRbt("reversebits.rbt")
updateDict(saveDict, p, None, "Glitch Width", comp)   

for i in range(-1, 5):
    cleanup()
    print "Offset = %d"%i
    makeDiffBits_phase(i, comp)
    p = parseRbt()
    updateDict(saveDict, p, i, "Glitch Width", comp)   
cleanup()
pickle.dump(saveDict, open('partialreconfig-1.p', 'wb'))


