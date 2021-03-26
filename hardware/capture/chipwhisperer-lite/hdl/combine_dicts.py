# Copyright 2015 Colin O'Flynn. NewAE Technology Inc.
#
# Combine all the individual PR files into one large file.
#

import os
import subprocess
import shutil
import pickle

dbase = None

for width in range(0, 128):   
    d = pickle.load(open('autogen_dir/width%d/cwlite-glitchoffsetwidth.p'%width))

    if dbase:
        if d['base'] != dbase['base']:
            print "Uh no"

        dbase['values'].update(d['values'])
    else:
        dbase = d.copy()


pickle.dump(dbase, open('cwlite-glitchoffsetwidth.p', 'wb'))

