#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2015, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
#
#    This file is part of chipwhisperer.
#
#    chipwhisperer is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    chipwhisperer is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
#=================================================
import logging
import sys
from PySide.QtCore import *
from PySide.QtGui import *

from chipwhisperer.common.ui.CWMainGUI import CWMainGUI
from chipwhisperer.common.utils.util import hexstr2list
import chipwhisperer.common.utils.qt_tweaks as QtFixes


class APDUFilter(QObject):
        
    def validateAPDU(self, widget):       
         
        headerstart = "<font color=red>"
        headerend = "</font>"
        
        lenstart = "<font color=blue>"
        lenend = "</font>"
        
        payloadstart = "<font color=green>"
        payloadend = "</font>"
        
        
        #APDU Changed, verify & colour-code text       
        data = hexstr2list(widget.toPlainText())

        htmlstr = ""

        #Validate length - basic
        if len(data) < 5:
            fail = True
        else:            
            #First four bytes are header
            htmlstr += headerstart
            header = data[0:4]
            for d in header:
                htmlstr += "%02x "%d
            htmlstr += headerend
            
            #Next is payload length or response length
            htmlstr += lenstart
            htmlstr += "%02x "%data[4]
            htmlstr += lenend
            
            plen = data[4]
            
            #Have payload
            if len(data) > 5:
                
                #Hmm... bad format!?
                if (len(data)-5) < plen:
                    logging.error('Header too short')
                    plen = len(data)-5
                
                payload = data[5:(5+plen)]
                
                htmlstr += payloadstart
                for d in payload:
                    htmlstr += "%02x " % d
                htmlstr += payloadend
                
                if (len(data)-5) > plen:
                    htmlstr += lenstart
                    htmlstr += "%02x " % data[plen+5]
                    htmlstr += lenend
                    
                    if (len(data)-5) != (plen+1):
                        logging.error('Extra data')

            widget.setHtml(htmlstr)
    
    def eventFilter(self, widget, event):
        # FocusOut event
        if event.type() == QEvent.FocusOut:

            self.validateAPDU(widget)

            # return False so that the widget will also handle the event
            # otherwise it won't focus out
            return False
        else:
            # we don't care about other events
            return False


class SmartCardGUICard(QtFixes.QDialog):
    def __init__(self, smartCardAPI):
        super(SmartCardGUICard, self).__init__(CWMainGUI.getInstance())
        self.setWindowTitle("Smartcard Explorer")
        self.smartCardAPI = smartCardAPI
        self.mainLayout = QVBoxLayout()
        self.butLayout = QHBoxLayout()
        
        pbAddRow = QPushButton("Add Row")
        pbAddRow.clicked.connect(self.addRow)
        self.butLayout.addWidget(pbAddRow)
        
        pbSendChecked = QPushButton("Send Checked")
        pbSendChecked.clicked.connect(self.executeAPDUs)
        self.butLayout.addWidget(pbSendChecked)
        
        pbClearOutput = QPushButton("Clear Output")
        pbClearOutput.clicked.connect(self.clearOutput)
        self.butLayout.addWidget(pbClearOutput)
        
        pbSaveToClipboard = QPushButton("Copy to Clipboard")
        pbSaveToClipboard.clicked.connect(self.copyToClipboard)
        self.butLayout.addWidget(pbSaveToClipboard)
        
        pbSaveFromClipboard = QPushButton("Copy from Clipboard")
        pbSaveFromClipboard.clicked.connect(self.copyFromClipboard)
        self.butLayout.addWidget(pbSaveFromClipboard)
        
        self.butLayout.addStretch()
        self.table = QTableWidget(0,3)
        
        self.table.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)
        self.table.horizontalHeader().setResizeMode(0, QHeaderView.Interactive) 
        self.table.horizontalHeader().setResizeMode(1, QHeaderView.Stretch) 
        self.table.horizontalHeader().setResizeMode(2, QHeaderView.Interactive) 
        self.table.setHorizontalHeaderLabels(["Name", "APDU Transmit", "Response"])
        
        self.mainLayout.addWidget(self.table)
        self.mainLayout.addLayout(self.butLayout)

        self.setLayout(self.mainLayout)
        
        self._updateInProgress = False
        self.filtlist = []
               
    def addRow(self, rownum=None):
        """ Append a result to the display table """
        
        if rownum is None:
            rownum = self.table.rowCount()
        self.table.insertRow(rownum)

        enButtonItem = QTableWidgetItem()
        enButtonItem.setCheckState(Qt.Checked)
        apduItem = QTextEdit("80 04 04 00 10 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F")      
        #apduItem.textChanged.connect(lambda : self.validateAPDU(rownum))
        self.filtlist.append(APDUFilter())
        apduItem.installEventFilter(self.filtlist[-1])        
          
        respItem = QTextEdit("")
        respItem.setReadOnly(True)
        
        self.table.setItem(rownum, 0, enButtonItem)
        self.table.setCellWidget(rownum, 1, apduItem)
        self.table.setCellWidget(rownum, 2, respItem)
        
        return rownum
        
    def copyToClipboard(self):
        """Copy table data to clipboard for saving/modifying as text"""
        rows = self.table.rowCount()
        textdata = ""
        
        for row in range(0,rows):
            if self.table.item(row, 0).checkState() == Qt.Checked:
                textdata += "1,"
            else:
                textdata += "0,"
            name = self.table.item(row, 0).text()
            name = name.replace(",",".")
            textdata += name + ","            
            textdata += self.table.cellWidget(row,1).toPlainText()
            textdata += ","         
            textdata += self.table.cellWidget(row,2).toPlainText()
            
            textdata += "\n"
        
        clipboard = QApplication.clipboard()
        clipboard.setText(textdata)
        
    def copyFromClipboard(self):
        """Copy table data from clipboard for loading from text"""
        clipboard = QApplication.clipboard()
        text = clipboard.text()        
        lines = text.splitlines()
        
        shouldclear = True        
        
        for line in lines:
            #Split line based on comma
            data = line.split(",")
            
            enabled = data[0].rstrip()
            name = data[1].rstrip()
            apdu = data[2].rstrip()
            
            enabled = int(enabled)
            
            # Clear table once basic processing done, don't want to clear
            # table if invalid data present in clipboard
            if shouldclear:
                self.table.setRowCount(0)
                shouldclear = False
            
            rownum = self.addRow()
            
            if enabled:
                self.table.item(rownum, 0).setCheckState(Qt.Checked)
            else:
                self.table.item(rownum, 0).setCheckState(Qt.Unchecked)
                
            self.table.item(rownum, 0).setText(name)                
            self.table.cellWidget(rownum, 1).setText(apdu)
            
    def clearOutput(self):
        rows = self.table.rowCount()
        for rownum in range(0, rows):
            self.table.cellWidget(rownum, 2).setText("")
        
    def executeAPDUs(self):
        """Execute APDUs and save results in table"""
        rows = self.table.rowCount()
        for rownum in range(0, rows):
            if self.table.item(rownum, 0).checkState() == Qt.Checked:
                apdu = self.table.cellWidget(rownum, 1).toPlainText()
                apdu = hexstr2list(apdu)
                
                
                txdatalen = apdu[4]
                
                if len(apdu) == 5:                    
                    rxdatalen = txdatalen
                    txdatalen = 0
                    txdata = None
                else:
                    txdata = apdu[5:(5+txdatalen)]
                    
                    if len(apdu) == 5+txdatalen:
                        rxdatalen = 0
                    else:
                        rxdatalen = apdu[5+txdatalen+1]
                                                                                            
                resp = self.smartCardAPI.getConnection().sendAPDU(apdu[0], apdu[1], apdu[2], apdu[3], txdata, rxdatalen)
                
                if type(resp) == int:
                    self.table.cellWidget(rownum, 2).setText("%04x"%resp)
                else:
                    payload = ""
                    for d in resp[1]: payload += "%02x "%d
                    payload = payload.rstrip()                    
                    self.table.cellWidget(rownum, 2).setText("%04x: %s"%(resp[0], payload))        


def main():
    # Create the Qt Application
    app = QApplication(sys.argv)

    std = SmartCardGUICard(None)
    std.show()

    app.exec_()

if __name__ == '__main__':
    main()
