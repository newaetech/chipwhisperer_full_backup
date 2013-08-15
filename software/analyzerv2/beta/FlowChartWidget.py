#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
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

import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()
    
try:
    import pyqtgraph as pg
    from pyqtgraph.flowchart import Flowchart
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

class FlowChartWidget(QWidget):
    """
    This FlowChartWidget holds a pyqtgraph FlowChart, and adds a toolbar for zoom, and a (togglable) toolbox of widgets you can add.
    """    
    
    def __init__(self, imagepath='images/'):
        super(FlowChartWidget, self).__init__()
        
        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')
        self.imagepath = imagepath

        layout = QVBoxLayout()
        
        ## Create flowchart, define input/output terminals
        fc = Flowchart(terminals={
            'dataIn': {'io': 'in'},
            'dataOut': {'io': 'out'}    
            })
        w = fc.widget()
        layout.addWidget(w)
        self.setLayout(layout)
        

if __name__ == '__main__':
    
    # Create the Qt Application
    app = QApplication(sys.argv)
    
    # Create and show the form
    wid = FlowChartWidget()
    wid.show()
   
    # Run the main Qt loop
    sys.exit(app.exec_())



