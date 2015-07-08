.. _naecw1173_cwlite:

CW1173: ChipWhisperer-Lite Board
================================

The ChipWhisperer-Lite Bare Board consists of two main parts: a multi-purpose power analysis capture instrument, and a target board. The target board
is a standard microcontroller which you can implement algorithms onto. For example if you wish to evaluate an AES library, you can program that library
into the target board and perform the power analysis.

.. image:: /images/cw1173/cwlite_basic.png

Quick-Start Guide
-----------------

You can see a `Video <http://www.youtube.com/watch?v=MJmkYqA-LeM&hd=1>`__ of the quickstart guide, which will take you through all the setup items discussed here:

|YouTubeCW1173Demo|_

.. |YouTubeCW1173Demo| image:: /images/cw1173/cwlite_demo_video.png
.. _YouTubeCW1173Demo: http://www.youtube.com/watch?v=MJmkYqA-LeM&hd=1


Connection Quick-Start
^^^^^^^^^^^^^^^^^^^^^^

1. Install Python and Dependancies
   
   Windows:
     * Install Python distribution (such as WinPython), see :ref:`installingwinpython`
     * Using the Python Command Prompt, use pip to install the following::
     
        pip install pyside
        pip install configobj
        pip install pyusb
        pip install pyqtgraph 
        
      What is included in releases varies. If you get a message saying it is already installed you can simply continue with this tutorial.
       
   Linux:
     * Python should come pre-installed. If so you'll also need to install the following from your package manager:
     
        * pyside
        * configobj
        * scipy
        * numpy
     
        For example the following shows the install command for Ubuntu::
        
         sudo apt-get install python2.7 python2.7-dev python2.7-libs python-numpy python-scipy python-pyside python-configobj python-setuptools python-pip
         
     * Some packages may require installation via 'pip', as they are not always in your package manager repo::
        
         $pip install pyusb
         $pip install pyqtgraph         
        
   Mac OS-X: TODO
      
2. Download and install ChipWhisperer

   * Download the latest release linked from ChipWhisperer.com
   * Unzip the file somewhere. Generally 'somewhere' will become your ChipWhisperer working directory. These examples assume
     you've chosen ``C:\chipwhisperer`` as your working directory.
   * Open a Python terminal (again if using WinPython be sure to run the specific Python command prompt),
     and run the following, adjusting paths as needed to refelct your working directory::

       cd c:\chipwhisperer\openadc\controlsw\python
       python setup.py develop
       cd c:\chipwhisperer\software
       python setup.py develop

    If you wish to confirm the installation worked, you can run ChipWhispererAnalyzer in the same terminal::

       cd c:\chipwhisperer\software\chipwhisperer\analyzer
       python ChipWhispererAnalyzer.py
       
4. Connect ChipWhisperer-Lite, install USB Drivers:

   Windows:
     * Plug micro-USB cable into ChipWhisperer-Lite
     * If the "New Hardware Found" wizard doesn't prompt you for drivers, go to the control panel and find the "ChipWhisperer-Lite" device, and select "Update Drivers".
     * You can find drivers in ``c:\chipwhisperer\hardware\capture\chipwhisperer-lite\``. You will likely need to extract them from the ``cwlite_usb_driver.zip`` file. If so
       simply extract them somewhere (i.e. your desktop), and then point the new hardware found wizard to that location.
       
   Linux:
     * Driver installation is not required, but if you do not update the 'udev' system, you will be unable to run ChipWhisperer-Capture as a regular user. To update
       the udev system, first make sure the ChipWhisperer-Lite is
       Make a file called ``/etc/udev/rules.d/99-cwlite.rules`` . The contents of this file should be::

        # allow users to claim the device
        SUBSYSTEM=="usb", ATTRS{idVendor}=="2b3e", ATTRS{idProduct}=="ace2", MODE="0664", GROUP="plugdev"

     * Add your username to the plugdev group::

        $ sudo usermod -a -G plugdev YOUR-USERNAME

     * And reset the udev system::

        $ sudo udevadm control --reload-rules

     * Finally log out & in again for the group change to take effect.
     
     * Connect the micro-USB cable
     
   MAC:
     * TODO

5. Run ChipWhisperer-Capture. This can be done from one of three ways:

    * Double-click on ``CWCapture.pyw`` in the ``chipwhisperer\software`` folder. You must have installed Python into your path for this to work.
    * run ``python CWCapture.pyw`` from the  ``chipwhisperer\software`` folder using a command prompt with Python in the path.
    * run ``python ChipWhispererCapture.py`` from the ``chipwhisperer\software\chipwhisperer\capture`` directory.
    
    The last option is the most reliable, in that it should always work on all platforms. If it doesn't start look for possible missing modules or
    other useful errors.
    
    NOTE: The first time you run ChipWhisperer-Capture or -Analyzer, the default setup of the screen is somewhat insane. You can drag windows around
    or close them to make it look more like the demos here. See the Video quickstart guide as well for details of that.

6. From the "Scripts" directory, run the ``ChipWhisperer-Lite: AES SimpleSerial on XMEGA`` script:

   .. image:: /images/cw1173/cwlite_simpleserial.png
   
   This should connect to the ChipWhisperer-Lite, program the FPGA, and run a few captures. Your screen should look something like this now:
   
   .. image:: /images/cw1173/cwdemo_normal.png

7. Your ChipWhisperer-Lite is now connected. See the next section for details of the demo attack.

Capture and Attack Quickstart
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. See :ref:`tutorialaes` for details. Note the hardware setup is slightly different -- but you can skip to step 5.5.3, and should be able to pick up
   from there. Be sure to use the ``ChipWhisperer-Lite: AES SimpleSerial on XMEGA`` script instead of the one referenced in step 5.5.3.
   

Basic Usage Instructions
------------------------

Programming XMEGA Device
^^^^^^^^^^^^^^^^^^^^^^^^


Programming AVR Device
^^^^^^^^^^^^^^^^^^^^^^


Using Glitch Port
^^^^^^^^^^^^^^^^^


Using Measure Port
^^^^^^^^^^^^^^^^^^


20-Pin Connector
^^^^^^^^^^^^^^^^



Breaking Target Section Apart
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You may wish to break the target section apart from the main capture board. This can easily be accomplished by following these instructions:

1. Using a sharp knife (such as Xacto knife or retractable safety knife), cut the traces on the bottom side of the board along the cut line. Pass the knife
   back and forth several times. Scoring the board deeply will make the breaking process easier and less stressful on the PCB:
   
   .. image:: /images/cw1173/breakstep1.png
   
2. Score the board on the top side:

  .. image:: /images/cw1173/breakstep2.png
  
3. Select a surface to break the board over. It is suggested to have a piece of cardboard or boxboard down to protect components on the bottom side of the ChipWhisperer:

  .. image:: /images/cw1173/breakstep3.png
  
4. Hold the main board section flat, apply even pressure to the target board section. It should snap downward:

  .. image:: /images/cw1173/breakstep4.png
  
5. Separate the two sections:

  .. image:: /images/cw1173/breakstep5.png

You can see a `Video <http://www.youtube.com/watch?v=8sIrvG5jqiQ&hd=1>`__ of the process here:

|YouTubeCW1173Break|_

.. |YouTubeCW1173Break| image:: /images/cw1173/cw1173breakvideo.png
.. _YouTubeCW1173Break: http://www.youtube.com/watch?v=8sIrvG5jqiQ&hd=1

Applying even pressure will help prevent damage to the ChipWhisperer-Lite main section. Flexing the PCB too much may cause damage to solder joints, but by holding the entire
board flat against the edge this is prevented.

Advanced Usage
--------------

Mounting Jumpers
^^^^^^^^^^^^^^^^

Note the ChipWhisperer-Lite main board and target section contain a number of jumper options. By default these are not mounted, and solder jumper bridges on the PCB have been
bridged to select the appropriate options when required. Some options are only solder jumpers, which to move the jumper requires a soldering iron to bridge or clear the appropriate
connections.

The following lists jumpers on the ChipWhisperer-Lite / Target Section:

Capture Section Jumpers:
 * JP4 is the "RESET" net for the SAM3U processor.
 * JP2 causes the SAM3U processor flash memory to be erased. When the chip is erased a rom-resident bootloader takes over. See section XXXXX for bootloader details.
 * JP5 selects the IO voltage for the FPGA bank which connects to the 20-pin target. By default SJ6 selects this to be 3.3V. It is not recommended to change this, as
   it is easy to damage the FPGA by feeding an out-of-range voltage in.
 * SJ1 selects if the power supply comes from the Micro-USB connector (default) or an external 5V supply at the +5VIN pin.

Target Section Jumpers:
 * JP7 connects the "MEASURE" SMA to the XMEGA VCC Rail. Shorted by default with SJ4.
 * JP6 connects the "GLITCH" SMA to the XMEGA VCC Rail. Shorted by default with SJ5.
 * JP12 can be used to feed an external voltage into the XMEGA VCC Rail. By default SJ3 connects this to 3.3V.
 * SJ2 selects if the 3.3V rail comes from the 20-pin IDC connector (i.e. ChipWhisperer-Lite board), or via an optional LDO and USB connector. 



