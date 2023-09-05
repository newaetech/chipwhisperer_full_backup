.. _install-virtual-machine:

########################
Virtual Machine Installs
########################

Instead of installing natively, you also have the option of using a virtual machine
image that includes all the required software and will launch Jupyter on boot.
This image also has all of the OS configuration ready as well.

****************
VirtualBox Image
****************

 * Install `VirtualBox`_. This program is freely available on Windows, Mac,
   and Linux.

 * Install the VirtualBox Extension pack, which can be found on the VirtualBox 
   downloads page linked above. This is necessary for the VM to interact with 
   the ChipWhisperer hardware. Note that the license for the Extension pack differs
   from the base VirtualBox license: https://www.virtualbox.org/wiki/VirtualBox_PUEL

 * Download a ChipWhisperer virtual machine image release or build it
   yourself using Vagrant. VM images come as .7z files and can be found on our 
   `GitHub releases page`_, typically being called :code:`ChipWhisperer.Jupyter.7z`
   or similar.

 * Unzip the VirtualBox image, go to *Machine* > *Add* in VirtualBox and select
   the VM that was unzipped.

 * Verify that the VM boots.

.. note:: If you are on linux you need to add yourself to the *vboxusers*
    permission group using, so Virtual Box is given permission to access
    usb devices::

        sudo usermod -aG vboxusers <your username>

    Then refresh the groups by restarting your computer or logging out and in
    again.

Next, we'll need to update some passwords for the VM. Boot the virtual
machine then:

 * Log in (user: vagrant pass: vagrant).

 * Setup a new password for Jupyter. As of release 5.2.0, you will be prompted
   to set a password on startup if one doesn't exist for Jupyter. For older
   releases, simply type :code:`jupyter notebook password` in the command prompt,
   then set a password. Note
   that Jupyter will not start until this is done. This password will be
   needed to log into Jupyter, so make sure you record it as well.

 * Reboot the VM.

 * Once the VM is booted, you can connect to Jupyter via localhost:8888 (
   Firefox/Chrome ONLY). You will be asked for the password you set via
   jupyter notebook password

You shouldn't need to log in to the VM again to run Jupyter (which provides
the interface) as it should start automatically, but make sure you still
record the password you set for the vagrant account, as you will need to log
in to update ChipWhisperer.

You are now ready to use ChipWhisperer. Open Chrome/Firefox and
type **localhost:8888** into the address bar. This will give you access to
the Jupyter Notebook server running in the virtual machine.

===================================
Troubleshooting Hardware Connection
===================================

If you're not able to connect to your ChipWhisperer in a VM, but are able to outside of the VM,
you may need to disable the USB device filters and manually connect. An example of this is
being able to see the ChipWhisperer in Windows Device Manager when the VM is off, but not being
able to connect in the VM.

To disable the device filters, open VirtualBox, right click :code:`ChipWhisperer Jupyter`,
go to :code:`Settings>USB` and uncheck the boxes for your devices. 

To manually connect, boot the VM. Then, in the top tool bar, go to :code:`Devices>USB` and
click on the entry for your ChipWhisperer. You will need to do this each time you boot the VM.

If this doesn't work and you're on Linux, make sure you've added your user to :code:`vboxusers`
as indicated above.

.. _GitHub releases page: https://github.com/newaetech/chipwhisperer/releases

.. _VirtualBox: https://www.virtualbox.org/wiki/Downloads

******
VMWare
******

For various reasons, such as licensing and USB support, users may prefer to run 
ChipWhisperer through VMWare instead of VirtualBox. A VMWare compatable image is not
provided with ChipWhisperer releases, but such an image can be easily converted
from the provided image using VirtualBox

 * Install `VirtualBox`_

 * Download a ChipWhisperer virtual machine image release or build it
   yourself using Vagrant. Virtual machine images can be found on our `GitHub releases page`_.

 * Add the VM image to VirtualBox

 * Right click on the image in VirtualBox and select :code:`Export to OCI` 

 * Select :code:`OVF Format 1.0` and export using the default settings.

 * The resulting :code:`.ovf` file can be opened in VMWare. VMWare may complain
   about the file not following OVF specifications. If this happens, hit 
   :code:`retry`.

You should now have a working VMWare image. Boot the VM and add passwords as described in :ref:`install-virtual-machine`
Before logging out, run the following command and record the :code:`eth0` IP Address:

.. code:: bash

    ip addr

The final step is to setup VMWare port forwarding:

 * If you have VMWare Player, you'll need to install VMWare Workstation Pro.
   The required utility tool does not require a license to run, so Workstation
   Pro can be installed without purchasing the software. If you're already
   running Workstation Pro, you can skip this step.

 * Navigate to the folder where VMWare Workstation Pro is installed and run 
   :code:`vmnetcfg.exe`

 * Click the :code:`Change Settings` button.

 * Click on the :code:`NAT` table entry (typically VMnet8) and click on :code:`NAT Settings...` 
   Take note of the Subnet Address of this entry

 * Under the Port Fowarding table, click :code:`Add` and fill in the following settings:
     * :code:`Host port:                  8888`
     * :code:`Type:                       TCP`
     * :code:`Virtual machine IP address: <subnet address>`
     * :code:`Virtual machine port:       8888`
     * :code:`Description:                Jupyter` (optional)

 * Hit :code:`OK` until :code:`vmnetcfg.exe` is closed

You should now be able to open the VM and connect to :code:`<eth0 IP>:8888`, replacing
:code:`<eth0 IP>` with the IP address you recorded after running :code:`ip addr`. 