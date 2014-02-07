#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * Neither the name of the authors nor the
#      names of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL COLIN O'FLYNN BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#


import sys
from PySide.QtCore import *
from PySide.QtGui import *
import os.path
os.environ["QT_API"] = "pyside"
import matplotlib
matplotlib.use("Qt4Agg")
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg as NavigationToolbar
from matplotlib.figure import Figure


class pysideGraph():
    def __init__(self, name="", xmin=0, xmax=1, ymin=-1.0, ymax=1.0, xfigsize=600, yfigsize=600, showNav=True, showExtraSettings=False):
        self.gb = QGroupBox(name)
        self.fig = Figure(figsize=(xfigsize,yfigsize), dpi=72)
        self.fig_ax =  self.fig.add_subplot(111)
        self.fig_ax.plot([0,0])
        clocklayout = QVBoxLayout()
        canvas = FigureCanvas(self.fig)
        layout = QVBoxLayout()
        layout.addWidget(canvas)

        if showNav:
            ntbr = NavigationToolbar(canvas, self.gb)
            layout.addWidget(ntbr)

        if showExtraSettings:
            settingsLayout = QGridLayout()
            self.xmin = QSpinBox()
            self.xmin.setMinimum(xmin)
            self.xmin.setMaximum(xmax)
            self.xmax = QSpinBox()
            self.xmax.setMinimum(xmin)
            self.xmax.setMaximum(xmax)
            self.ymin = QDoubleSpinBox()
            self.ymin.setMinimum(ymin)
            self.ymin.setMaximum(ymax)
            self.ymin.setDecimals(5)
            self.ymax = QDoubleSpinBox()
            self.ymax.setMinimum(ymin)
            self.ymax.setMaximum(ymax)
            self.ymin.setValue(ymin)
            self.ymax.setValue(ymax)
            self.xmin.setValue(xmin)
            self.xmax.setValue(xmax)
            self.ymax.setDecimals(5)

            settingsLayout.addWidget(QLabel("X Limits:"), 0, 0)
            settingsLayout.addWidget(self.xmin, 0, 1)
            settingsLayout.addWidget(self.xmax, 0, 2)
            settingsLayout.addWidget(QLabel("Y Limits:"), 1, 0)
            settingsLayout.addWidget(self.ymin, 1, 1)
            settingsLayout.addWidget(self.ymax, 1, 2)
            #settingsLayout.addWidget(self.persistant, 1, 3)
            layout.addLayout(settingsLayout)
            self.xmin.valueChanged.connect(self.updateAxis)
            self.xmax.valueChanged.connect(self.updateAxis)
            self.ymin.valueChanged.connect(self.updateAxis)
            self.ymax.valueChanged.connect(self.updateAxis)
        else:
            self.xmin = None
            
        self.gb.setLayout(layout)


    def getWidget(self):
        return self.gb

    def updateAxis(self):
        if self.xmin:
            self.fig_ax.axis([self.xmin.value(), self.xmax.value(), self.ymin.value(), self.ymax.value()])
        self.redraw()

    def redraw(self):
        self.fig.canvas.draw()
            
    def updateData(self, data=None, holdOn=False, updateAxis=True, redraw=True):
        if len(data) > 0:
            if holdOn == False:
                self.fig_ax.cla()
            self.fig_ax.plot(data)

        if self.xmin:
            if self.xmin.value() == self.xmax.value():
                cursettings = self.fig_ax.axis()
                self.xmin.setValue(cursettings[0])
                self.xmax.setValue(cursettings[1])
                self.ymin.setValue(cursettings[2])
                self.ymax.setValue(cursettings[3])
            else:
                if updateAxis:
                    self.updateAxis()
        else:
            if updateAxis:
                self.updateAxis()

        if redraw:
            self.redraw()

class TestClass(QMainWindow):
    def __init__(self):
        super(TestClass, self).__init__()        
        self.preview = pysideGraph("Here is the Title")
        self.initUI()

        #Generate some data
        self.preview.updateData(range(0,2000), holdOn=True)
        self.preview.updateData(range(1000,3000), holdOn=True)
        self.preview.updateData(range(500,1000), holdOn=True)
        
    def initUI(self):
        self.setCentralWidget(self.preview.getWidget())
        self.statusBar()
        self.show()
                                       
def main():    
    app = QApplication(sys.argv)
    ex = TestClass()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
