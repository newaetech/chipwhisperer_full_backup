.. _hwsakurag:

SAKURA-G Target
===============

Preparing the SAKURA-G Hardware
--------------------------------

The SAKURA-G must be prepared for use with the ChipWhisperer system. This requires the OpenADC device mounted using an interposer
PCB. In addition the SMA connector must route the power measurement, and the trigger signal must be connected. This assumes the
``FPGA #1`` on the SAKURA-G has been programmed with the AES example.

A Xilinx Platform USB Cable or similar is used for programming. The complete setup looks as follows:

.. image:: /images/sakurag/sakurag_hardware.jpg

You must then run Xilinx Impact program, and have it detect the Spartan 6 LX9 device. You can then add a SPI Flash:

.. image:: /images/sakurag/addspiflash.png

This will prompt you to specify the PROM file. This file is located in 
``chipwhisperer\hardware\capture\sakura-g\sakurag_ise_project\sakurag-chipwhisperer.mcs``.

.. image:: /images/sakurag/addpromfile.png

Finally select the SPI Flash Type. Confirm the part number by looking at part U13 on the SAKURA-G, just to the right of the LX9 FPGA.

.. image:: /images/sakurag/selectflash.png

You can now program the SPI Flash. This step will take some time to complete. Once this is done you have a working ChipWhisperer system.

.. image:: /images/sakurag/rightclick-program.png

Running SAKURA-G Example
--------------------------

The SAKURA-G has an example script shipped with the ChipWhisperer release, which can be run from the ChipWhisperer-Capture
environment.

You can see a `Video <http://www.youtube.com/watch?v=hykeV8DCf2M&hd=1>`__ of the system working on YouTube:

|YouTubeSakura|_

.. |YouTubeSakura| image:: /images/sakurag/sakuragyoutube.png
.. _YouTubeSakura: http://www.youtube.com/watch?v=hykeV8DCf2M&hd=1


    
