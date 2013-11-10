# -*- coding: utf -*-
# This file is part of ChipWhisperer
# Copyright (C) Colin O'Flynn 2013
# See www.newae.com for details
#
# Released under GPL License

import sys
from functools import partial
import time

from PySide.QtCore import *
from PySide.QtGui import *

import PartialReconfiguration as pr

try:
    from pyqtgraph.parametertree import Parameter
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()
    
sys.path.append('../../common')
sys.path.append('../.')
from ExtendedParameter import ExtendedParameter

glitchaddr = 51    
CODE_READ       = 0x80
CODE_WRITE      = 0xC0

# sign extend b low bits in x
# from "Bit Twiddling Hacks"
def SIGNEXT(x, b):
    m = 1 << (b - 1)
    x = x & ((1 << b) - 1)
    return (x ^ m) - m

class ChipWhispererGlitch(QObject):
    paramListUpdated = Signal(list)
             
    def __init__(self, showScriptParameter=None):
        paramSS = [
                {'name':'Clock Source', 'type':'list', 'values':{'Target IO-IN':0}, 'value':0},
                {'name':'Glitch Width (% of period)', 'type':'float', 'limits':(0, 100), 'step':0.39062, 'readonly':True, 'set':self.setGlitchWidth},
                {'name':'Glitch Width (fine adj)', 'key':'widthfine', 'type':'int', 'limits':(-255, 255), 'set':self.setGlitchWidthFine},
                {'name':'Glitch Trigger', 'type':'list', 'values':{'Capture':0}, 'value':0},                   
                {'name':'Delay (% of period)', 'type':'float', 'limits':(-100, 100), 'step':0.390625},
                {'name':'Repeat', 'type':'int'},
                {'name':'Read Status', 'type':'action', 'action':self.checkLocked},
                {'name':'Reset DCM', 'type':'action', 'action':self.resetDCMs},
                ]
    
            
        #Load FPGA partial configuration data
        self.glitchWidthPR = pr.PartialReconfigData()
        self.prCon = pr.PartialReconfigConnection()
        self.oa = None
        
        try:            
            self.glitchWidthPR.load("scopes/cw-partial-files/s6lx25-glitchwidth.p")
            self.prEnabled = True
        except IOError, e:
            print str(e)
            self.prEnabled = False
            
        if self.prEnabled:
            #Enable glitch width, check what we've got access to
            paramSS[1]['readonly'] = False
            lim = (self.glitchWidthPR.limits[0] / 2.55, self.glitchWidthPR.limits[1] / 2.55 )
            if lim[0] < 0:
                lim = (0, lim[1])
            paramSS[1]['limits'] = lim
                
        self.params = Parameter.create(name='Glitch Module', type='group', children=paramSS)
        ExtendedParameter.setupExtended(self.params, self)
        self.showScriptParameter = showScriptParameter
        
    def paramTreeChanged(self, param, changes):
        if self.showScriptParameter is not None:
            self.showScriptParameter(param, changes, self.params)

    def setOpenADC(self, oa):
        if self.prEnabled:
            self.prCon.con(oa)            
            #Reset FPGA back to defaults in case previous bitstreams loaded
            self.setGlitchWidth(0)
            
        self.oa = oa
            
        self.params.getAllParameters()
        
    def paramList(self):
        p = []
        p.append(self.params)            
        return p
    
    def setGlitchWidth(self, width):
        cnt = round((width/100) * 256)
        
        #Limit hardware
        if cnt >= 256:
            cnt = 255
            
        bs = self.glitchWidthPR.getPartialBitstream(cnt)
        
        #for p in bs:
        #    print "%4x"%p
         
        if self.prEnabled:
            self.prCon.program(bs)
            if self.oa is not None:
                self.resetDCMs()   
            
        #print "Partial: %d"%cnt
        
        
    def resetDCMs(self):
        reset = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8) 
        reset[5] |= (1<<1)
        self.oa.sendMessage(CODE_WRITE, glitchaddr, reset, Validate=False)
        reset[5] &= ~(1<<1)
        self.oa.sendMessage(CODE_WRITE, glitchaddr, reset, Validate=False)
        
        self.findParam('widthfine').setValue(0)
        
    def checkLocked(self):
        current = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)        
        #current[4] #39..32
        
        if current[4] & 0x80:
            print "Glitch-DCM1 Locked"
        else:
            print "Glitch-DCM1 Unlocked"
            
        if current[5] & 0x01:
            print "Glitch-DCM2 Locked"
        else:
            print "Glitch-DCM2 Unlocked"
        
        #assign clockglitch_settings_read[39] = dcm1_locked;
        #assign clockglitch_settings_read[40] = dcm2_locked;
        
        print self.getGlitchWidthFine()
        
    def setGlitchWidthFine(self, fine):
        '''Set the fine phase adjust, range -255 to 255'''
        
        current = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)               

        LSB = fine & 0x00FF;
        MSB = (fine & 0x0100) >> 8;
       
        current[0] = LSB #7..0
        current[1] = (current[1] & 0xFE) | MSB #15..8
        current[2] = current[2] | 0x04 #23..16    
        
        
        #31..24
        
        #assign clockglitch_settings_read[18:0] = clockglitch_settings_reg[18:0];

        #assign clockglitch_settings_read[37] = phase1_done_reg;
        #assign clockglitch_settings_read[38] = phase2_done_reg;
     
        self.oa.sendMessage(CODE_WRITE, glitchaddr, current, Validate=False)
        
    def getGlitchWidthFine(self):
        current = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)
          
          
        #assign clockglitch_settings_read[36:19] = {phase2_actual, phase1_actual};
        #current[2]#23..16
        #current[3]#31..24
        #current[4]#39..32
        
        phase1 = current[2] >> 3
        phase1 |= (current[3] & 0x0F) << 5
        
        phase1 = SIGNEXT(phase1, 9)
        
        #phase2 = (current[3] & 0xF0) >> 4
        #phase2 |= (current[4] & 0x1F) << 4    
        
        return phase1   
          

        
        