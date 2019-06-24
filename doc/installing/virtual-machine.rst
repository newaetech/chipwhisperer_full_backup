.. _sec-virtual-machine:

***************
Virtual Machine
***************

If this is your first time using the ChipWhisperer toolchain, the easiest 
way to start is to use a virtual machine with everything already set up for 
you. Note that Linux users may find it easier to do a manual install (
:ref:`sec-install-linux`).

These instructions are for the newest version of CW5 (v5.0.1+) and will not 
work with CW5's initial alpha release (v5.0.0). Virtualbox 6 is assumed 
here, but these instructions should work for Virtualbox 5 as well, provided 
the correct extension pack is installed.

 * Install `VirtualBox`_. This program is freely available on Windows, Mac, 
   and Linux.
   
 * Install the `VirtualBox Extension Pack`_. This is necessary for the VM to 
   interact with the ChipWhisperer hardware.
   
 * Download a ChipWhisperer virtual machine image release or build it 
   yourself using Vagrant.
   
 * Unzip the VirtualBox image, go to Machine>Add in VirtualBox and select 
   the VM that was unzipped.
    
 * Verify that the VM boots.

Next, we'll need to update some passwords for the VM. Boot the virtual 
machine then:

 * Log in (user: vagrant pass: vagrant). You will be asked to update your 
   password. Do so (make sure to record this password).
   
 * Setup a new password for Jupyter. This can be done by typing jupyter 
   notebook password, then entering and verifying your new password. Note 
   that Jupyter will not start until this is done. This password will be 
   needed to log into Jupyter, so make sure you record it as well.
   
 * Reboot the VM.
 
 * Once the VM is booted, you can connect to Jupyter via localhost:8888 (
   Firefox/Chrome ONLY). You will be asked for the password you set via 
   jupyter notebook password

You shouldn't need to log in to the VM again to run Jupyter (which provides 
the interface) as it should start automatically, but make sure you still 
record the password you set for the vagrant account, as you will need to log 
in to update ChipWhisperer

.. _VirtualBox: https://www.virtualbox.org/wiki/Downloads
.. _VirtualBox Extension Pack: https://download.virtualbox.org/virtualbox/6.0.6/Oracle_VM_VirtualBox_Extension_Pack-6.0.6.vbox-extpack