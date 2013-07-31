# -*- coding: cp1252 -*-
import sys
import os
import threading
import time
import logging
import math
from PySide.QtCore import *
from PySide.QtGui import *
from smartcard.CardType import AnyCardType
from smartcard.CardRequest import CardRequest
from smartcard.util import toHexString
             
class ConsoleCardConnectionObserver( ):
    def update( self, cardconnection, ccevent ):

        if 'connect'==ccevent.type:
            print 'connecting to ' + cardconnection.getReader()

        elif 'disconnect'==ccevent.type:
            print 'disconnecting from ' + cardconnection.getReader()

        elif 'command'==ccevent.type:
            print '> ', toHexString( ccevent.args[0] )

        elif 'response'==ccevent.type:
            if []==ccevent.args[0]:
                print '< [] ', "%-2X %-2X" % tuple(ccevent.args[-2:])
            else:
                print '< ', toHexString(ccevent.args[0]), "%-2X %-2X" % tuple(ccevent.args[-2:])

class SmartCard():

    def __init__(self):
        print "init"
        
    
    def connect(self):
        try:
            self.sccard = AnyCardType()
            self.screq = CardRequest(timeout=1, cardType=self.sccard)
            self.scserv = self.screq.waitforcard()

            #observer = ConsoleCardConnectionObserver()
            #self.scserv.connection.addObserver( observer )
            self.scserv.connection.connect()

        except CardRequestTimeoutException:
            return False

        return True        
    
    def close(self):
        self.scserv.connection.disconnect()
        
    def init(self):
        return
      
    def setModeEncrypt(self):
        return

    def setModeDecrypt(self):
        print "setmode"

    def loadEncryptionKey(self, key):
        #0x2b ,0x7e ,0x15 ,0x16 ,0x28 ,0xae ,0xd2 ,0xa6 ,0xab ,0xf7 ,0x15 ,0x88 ,0x09 ,0xcf ,0x4f ,0x3c
        #The APDU for the keyschedule is in hex (80 02 00 00 10 KEY(16Byte) 10)
        apdu = [0x80, 0x02, 0x00, 0x00, 0x10] + list(key) + [0x10]   
        response, sw1, sw2 = self.scserv.connection.transmit( apdu )

        print"%x %x"%(sw1,sw2)

        apdu = [0x80, 0x03, 0x00, 0x00, 0x00, 0x00];
        response, sw1, sw2 = self.scserv.connection.transmit( apdu );
       
    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):        
        return [0,0,0,0]

    def go(self):
        # The APDU for the encryption is in hex (80 40 00 00 10 PLAINTEXT(16Byte) 10)
        apdu = [0x80, 0x40, 0x00, 0x00, 0x10]
        apdu = apdu + list(self.input)
        apdu = apdu + [0x10];
        
        response, sw1, sw2 = self.scserv.connection.transmit( apdu )

        print"%x %x"%(sw1,sw2)
   
