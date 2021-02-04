#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the OpenADC
# project, http://www.assembla.com/spaces/openadc
#
# This project is released under the 2-Clause BSD License. See LICENSE
# file which should have came with this code.
#

import sys
import os
import ConfigParser

def printusage():
    """Print usage information & exit"""
    print "Usage: %s inputfile <outputfile>"%sys.argv[0]

class ISEModify(object):
    def __init__(self):
        self.f = None
        self.ucfSeq = 0
        self.verilogSeq = 0
        self.coregenSeq = 0

    def loadTemplate(self, tfile):
        self.f = open(tfile, 'r').read().splitlines(True)
        self.fname = tfile

    def saveProject(self, outputfile):
        of = open(outputfile, 'w')
        of.write("".join(self.f))
        of.close()

    def modifyProperty(self, propname, newvalue):
        """Modify a property in an ISE Project File"""

        linendx = None

        pstr = 'property xil_pn:name="'+propname+'"'
        for ndx, l in enumerate(self.f):
            if pstr in l:
                linendx = ndx

        if linendx is None:
            raise ValueError("Did not find property %s in %s"%(propname, self.fname))

        part = self.f[linendx].partition(pstr)

        self.f[linendx] = part[0] + '%s xil_pn:value="%s" xil_pn:valueState="non-default"/>\n'%(pstr, newvalue)

    def modifyVersion(self, ver="14.1"):
        """Set the supposed version number"""

        for ndx,l in enumerate(self.f):
            if "<version xil_pn:ise_version=" in l:
                self.f[ndx] = '  <version xil_pn:ise_version="%s" xil_pn:schema_version="2"/>\n'%ver
                break
        
    def findFilesIndex(self):
        for ndx,l in enumerate(self.f):
            if "<files>" in l:
                return ndx
        return None

    def addUCF(self, ucffile):
        loc = self.findFilesIndex()
        fstr =  '    <file xil_pn:name="%s" xil_pn:type="FILE_UCF">\n'%ucffile
        fstr += '      <association xil_pn:name="Implementation" xil_pn:seqID="%d"/>\n'%self.ucfSeq
        fstr += '    </file>\n\n'
        self.f.insert(loc+1, fstr)

        self.ucfSeq += 1

    def addVerilog(self, vfile):
        loc = self.findFilesIndex()

        fstr =  '    <file xil_pn:name="%s" xil_pn:type="FILE_VERILOG">\n'%vfile
        fstr +=  '      <association xil_pn:name="BehavioralSimulation" xil_pn:seqID="%d"/>\n'%self.verilogSeq
        fstr +=  '      <association xil_pn:name="Implementation" xil_pn:seqID="%d"/>\n'%self.verilogSeq
        fstr +=  '    </file>\n\n'
        self.f.insert(loc+1, fstr)

        self.verilogSeq += 1

    def addCoregen(self, cfile):
        loc = self.findFilesIndex()

        fstr =  '    <file xil_pn:name="%s" xil_pn:type="FILE_COREGEN">\n'%cfile
        fstr +=  '      <association xil_pn:name="BehavioralSimulation" xil_pn:seqID="%d"/>\n'%self.coregenSeq
        fstr +=  '      <association xil_pn:name="Implementation" xil_pn:seqID="%d"/>\n'%self.coregenSeq
        fstr +=  '    </file>\n\n'
        self.f.insert(loc+1, fstr)

        self.coregenSeq += 1


class coregenModify(object):
    def __init__(self):
        self.f = None
        self.fname = "NO FILE LOADED"

    def loadTemplate(self, tfile):
        self.f = open(tfile, 'r').read().splitlines(True)
        self.f.insert(0, '##############################################################\n')
        self.f.insert(1, '#### WARNING: Modified by OpenADC MakeISE System          ####\n')
        self.f.insert(2, '#### The declared ISE version, timestamp are INVALID      ####\n')       
        self.fname = tfile

    def saveProject(self, outputfile):
        of = open(outputfile, 'w')
        of.write("".join(self.f))
        of.close()

    def modifyProperty(self, propname, newvalue):
        """Modify a property in an CoreGen XCO File"""
        pstr = '%s='%propname
        linendx = None
        for ndx, l in enumerate(self.f):
            if l.startswith("CSET"):
                pt = "CSET"
            else:
                pt = "SET"

            l = l.replace(" ", "")
            l = l.replace("CSET","")
            l = l.replace("SET","")
            if pstr in l:
                linendx = ndx
                linestr = l
                prefix = pt

        if linendx is None:
            raise ValueError("Did not find property %s in %s"%(propname, self.fname))

        if prefix == "SET":
            self.f[linendx] = "SET %s = %s\n"%(propname, newvalue)
        else:
            self.f[linendx] = "%s %s=%s\n"%(prefix, propname, newvalue)

def parseCoregenSection(config, sectionName, outputFile, fpga, basedir, outdir):
    cg = coregenModify()
    cg.loadTemplate(os.path.join(basedir, config.get(sectionName, 'InputFile')))

    for key in config.options(sectionName):

        if key == "InputFile":
            continue

        value = config.get(sectionName, key)

        if value.startswith("CALCULATE"):
            inps = value.replace("CALCULATE","")
            
            # We now have a string like '$input_depth$ / 4'
            # We need to do the following:
            #  1) ID the variables
            #  2) Switch them into a formattable string & list of options: '%d / 4' ['input_depth',]
            #  3) Do the calculation

            outstr = ""
            outopts = []
            varact = False

            #Steps 1 & 2
            for t in inps:
                if varact:
                    if t == '$':
                        varact = False
                        outopts.append(optstr)
                    else:
                        optstr += t

                elif t == '$':
                    varact = True
                    optstr = ""
                    outstr += "%d"

                else:
                    outstr += t

            #Step 3
            varstr = ""
            for val in outopts:
                varstr += "%s,"%config.get(sectionName, val)
            varstr.rstrip(",")
            #Double-eval... first one does var-sub, second one calculates it
            value = eval( eval('"%s"%%(%s)'%(outstr, varstr)) )

        cg.modifyProperty(key, value)

    #CoreGen uses lower-case it seems?
    cg.modifyProperty('devicefamily', fpga[0].lower())
    cg.modifyProperty('device', fpga[1].lower())
    cg.modifyProperty('package', fpga[2].lower())
    cg.modifyProperty('speedgrade', fpga[3].lower())

    if not os.path.exists(os.path.join(outdir, "coregen")):
        os.makedirs(os.path.join(outdir, "coregen"))

    cg.saveProject(os.path.join(outdir, "coregen", outputFile))

def parseVerilogSection(config, sectionName, outputFile, basedir, outdir):

    outputfile = os.path.join(outdir, outputFile)
    fout = open(outputfile, "w")
    fout.write("//AUTOMATICALLY GENERATED - MAY BE OVERWRITTEN\n")

    for key in config.options(sectionName):
        value = config.get(sectionName, key)     
        if value is None:
            value = ""
        fout.write("`define %s %s\n"%(key, value))

    fout.close()

def makeRelPath(basedir, inpfile, outdir):
    bf = os.path.join(basedir, inpfile)

    if os.path.isfile(bf) is False:
        raise IOError("Unable to find file %s, check relative path to project file"%bf)

    return os.path.relpath(bf, outdir)

def main(args = None):
    if args == None:
        args = sys.argv[1:]

    if len(args) < 1:
        printusage()
        sys.exit()    

    inputfile = args[0]

    if len(args) < 2:
        outputfile = os.path.splitext(inputfile)[0] + ".xise"
    else:
        outputfile = args[1]

    
    print "Using %s input file to make %s"%(inputfile, outputfile)
    print ""

    basedir = os.path.dirname(inputfile)
    outdir = os.path.dirname(outputfile)

    config = ConfigParser.RawConfigParser(allow_no_value=True)
    #Preserve case for ISE Tools
    config.optionxform=str
    config.read(inputfile)

    fpga_device = None
    fpga_package = None
    fpga_family = None
    fpga_speed = None

    #Setup ISE Options
    isename = 'ISE Configuration'
    iseopts = config.options(isename)
    if ('Device Family' not in iseopts) or ('Package' not in iseopts) or ('Device' not in iseopts) or ('Speed Grade' not in iseopts):
        raise ValueError('Must specify "Device Family", "Device", "Package", and "Speed Grade".')

    template = None

    #Get template file
    for opt in iseopts:
        if (opt == 'InputFile'):
	        template = os.path.join(basedir, config.get(isename, opt))

    if template is None:
        raise ValueError("Must specify 'InputFile' in project for XISE Template")
    
    ise = ISEModify()
    ise.loadTemplate(template)

    for opt in iseopts:
        if (opt == 'InputFile') or (opt == 'OutputFile'):
            continue
        elif opt == 'Version':
            ise.modifyVersion(config.get(isename, 'Version'))
        else:
            val = config.get(isename, opt)
            ise.modifyProperty(opt, val)

            if opt == 'Device':
                fpga_device = val
            elif opt == 'Package':
                fpga_package = val
            elif opt == 'Speed Grade':
                fpga_speed = val
            elif opt == 'Device Family':
                fpga_family = val        
        
    fpga = (fpga_family, fpga_device, fpga_package, fpga_speed)
    print "FPGA = %s %s in %s%s"%(fpga[0], fpga[1], fpga[2], fpga[3])

    print "**Verilog Sources:"
    if config.has_section('Verilog Files'):
        for vf in config.options('Verilog Files'):
            vSetup = config.get('Verilog Files', vf)
            if vSetup is not None:
                parseVerilogSection(config, vSetup, vf, basedir, outdir)               
            else:
                vf = makeRelPath(basedir, vf, outdir)
            ise.addVerilog(vf)
            print "   %s"%vf

    print "**UCF Files:"
    if config.has_section('UCF Files'):
        for uf in config.options('UCF Files'):
            uf = makeRelPath(basedir, uf, outdir)
            ise.addUCF(uf)
            print "   %s"%uf

    print "**XCO Files:"
    if config.has_section('CoreGen Files'):
        for cf in config.options('CoreGen Files'):
            ise.addUCF(os.path.join("coregen", cf))
            cgSetup = config.get('CoreGen Files', cf)
            print "   %s"%cf
            if cgSetup is not None:
                parseCoregenSection(config, cgSetup, cf, fpga, basedir, outdir)

    print ""
    ise.saveProject(outputfile)

if __name__ == "__main__":
    main()
