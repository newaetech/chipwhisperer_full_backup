.. _hwsasebow:

SASEBO-W Target
===============

Preparing the SASEBO-W Hardware
--------------------------------

The SASEBO-W must be prepared for use with the ChipWhisperer system. This requires a few steps:

1. Program the correct files into the main FPGA. The ChipWhisperer releases are built for The Spartan 6 LX150 device, if you have a different
   FPGA on the SASEBO-W you will have to rebuild the Xilinx ISE project.
   
   You can program just the bitstream located at ``hardware\capture\sasebow-integrated\hdl\sasebow-chipwhisperer.bit``. This DOES NOT modify
   anything permantly if you don't want to modify the SPI flash.
   
   You can program the MCS file at ``sasebow-chipwhisperer-unpadded.mcs`` into the FLASH chip on the SASEBO-W. This makes the system always
   power on as a ChipWhisperer-attached device.


2. Assuming you have the OpenADC set up correctly, simply mount on the header pins. Be SURE to mount at the correct location, as
   putting power on the wrong pins may DESTROY the OpenADC.

   .. image:: /images/sasebow/openadc_connection.jpg

   
3. Mount a header on CN3, which shorts two of the GND planes together. This step is **REQUIRED FOR A RELIABLE CONNECTION**, you can see
   a blog post on `Split GND Plane <http://colinoflynn.com/tiki-view_blog_post.php?postId=39>`_ for details. The connection is between
   pins 3 and 5:

   .. image:: /images/sasebow/header.jpg


4. Using FT Prog, setup the FT2232H device on both Channel A & B for 'FT 245 FIFO' mode. You MUST program the EEPROM to set both
   Channels A & Channel B for FT 245 FIFO mode. The following image shows this being done:
   
   .. image:: /images/sasebow/ft245fifo.png

Running the SASEBO-W Example
------------------------------

The SASEBO-W example assumes you are using the SASEBO-W CardOS, and attacking the AES implementation. In this case:

1. Ensure you've setup the hardware as above

2. Insert a card into the smartcard reader

3. Run the ChipWhisperer-Capture program

4. Run the `SASEBO-W` example:

   .. image:: /images/sasebow/sasebowexample.png
   

More Information
-----------------------

Additional information is currently posted on the ChipWhisperer Wiki at these pages:

* https://www.assembla.com/spaces/chipwhisperer/wiki/SASEBO-W
* https://www.assembla.com/wiki/show/chipwhisperer/SASEBO-W_Programming_Information

