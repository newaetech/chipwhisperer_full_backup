# Copyright 2015 Colin O'Flynn. NewAE Technology Inc.
#
# This file generates the partial reconfiguration data required for
# the LX9 platform. This brute-forces a generation of every possible
# width/offset combination. The previous system was no longer working
# for some reason with the LX9.

import os
import subprocess
import shutil

#To generate partial reconfiguration data, the steps are:
#
# 1. Run parallel_generate.py 4 times. Each time set the blockrng to
#    a new option. This will take 3-4 days in total. You can use 'top'
#    to monitor completion of each block, then start the next block.
#
# 2. Run combine_dicts.py to generate a final output file.
#
# 3. Delete the directory autogen_dir, it's not needed now.
#

#Run each in a block of 32 items. Each block takes about 12 hours.

blockrng = (0, 32)
#blockrng = (32, 64)
#blockrng = (64, 96)
#blockrng = (96, 128)

for width in range(blockrng[0], blockrng[1]):
    print "Starting %d"%width
    curdir = r'autogen_dir/width%d'%width
    os.makedirs(curdir)
    shutil.copyfile("cwlite_ise/cwlite_interface.ncd", "%s/cwlite_interface.ncd"%curdir)
    shutil.copyfile("cwlite_ise/cwlite_interface.bit", "%s/cwlite_interface.bit"%curdir)
    shutil.copyfile("cwlite_ise/cwlite_interface.pcf", "%s/cwlite_interface.pcf"%curdir)    
    proc = subprocess.Popen(['python','../../long_reconfig_dicts.py','cwlite_interface','%d'%width]
                   , cwd=curdir)
    
