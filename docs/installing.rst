.. _install:

############
Installation
############

You get your choice of installing ChipWhisperer and its prerequisites: the easy way
or the hard way.

Basic
 * :ref:`install-virtual-machine` (Recommended)

Advanced
 * :ref:`prerequisites`
     * :ref:`prerequisites-windows`
     * :ref:`prerequisites-linux`
     * :ref:`prerequisites-mac`

 * :ref:`Installation <install>`
     * :ref:`install-repo-releases`
     * :ref:`install-repo-pypi`
     * :ref:`install-repo-git`


.. _install-virtual-machine:

***************
Virtual Machine
***************

If this is your first time using the ChipWhisperer toolchain, the easiest
way to start is to use a virtual machine with everything already set up for
you. Note that Linux users may find it easier to do a manual install (
:ref:`prerequisites-linux`).

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
in to update ChipWhisperer.

You are now ready to use ChipWhisperer. Open Chrome/Firefox and
type **localhost:8888** into the address bar. This will give you access to
the Jupyter Notebook server running in the virtual machine.

.. _VirtualBox: https://www.virtualbox.org/wiki/Downloads
.. _VirtualBox Extension Pack: https://download.virtualbox.org/virtualbox/6.0.6/Oracle_VM_VirtualBox_Extension_Pack-6.0.6.vbox-extpack


.. _install-repo:

*************
ChipWhisperer
*************

.. note:: You must have the :ref:`prerequisites` for your system installed
	before continuing with the installation of the repository.

.. note:: You may have to replace all the calls to **python** on the command line with
    whatever gives you access to the python version you installed. On GNU/Linux you will
    probably use **python3**, or you can use the full path to the python interpreter.
    It is not required but recommended to use a virtual environment.

After satisfying prerequisites for your system, install the ChipWhisperer
repository/package using one of:

:ref:`install-repo-releases` (Recommended)
	Get the latest stable release from the GitHub repository. The release includes
	repository and zipped drivers.

:ref:`install-repo-pypi`
	The classic :code:`pip install chipwhisperer`. Does not install
	the drivers, or hardware source code currently.

:ref:`install-repo-git`
	Install the latest features and bugs from our GitHub repository.


.. _install-repo-releases:

GitHub Releases
===============

Once you have a working Python 3 installation, you're ready to install and run ChipWhisperer.

First, download a ChipWhisperer release. You can get these from the `releases`_ page.
Generally, the latest release is a good choice, but you might need an older version
for various reasons. You want the source code in .zip or .tar.gz format - not a VBox
image.

Next, uncompress your downloaded source code somewhere. Generally, 'somewhere' will
become your ChipWhisperer working directory. For example, on Windows, you might
want to use *C:\\chipwhisperer\\*.

Once you've got the file, install the python dependencies and run the Python
install procedure (setup.py) using pip. Use the -e flag for develop mode to indicate
that the files will probably be changing frequently. To do this, open a terminal and run
the following, adjusting paths as needed:

.. code:: bash

    cd chipwhisperer
    python -m pip install -e . --user

    # to be able to run jupyter and the tutorials
    pip install -r jupyter/requirements.txt --user


To test, run python and try importing the **chipwhisperer** module:

.. code:: python

    >>> import chipwhisperer as cw

If you want to run the tutorials you can now start the
:ref:`Jupyter Notebook server <starting>`.


.. _releases: https://github.com/newaetech/chipwhisperer/releases

.. _install-repo-pypi:

PyPi
====

If you want to use **chipwhisperer** as a standalone python package and are not
interested in having all the tutorials and extra jupyter notebook stuff, this
installation method is for you::

    pip install chipwhisperer

Will install the *chipwhisperer/software/chipwhisperer* python package in your
site packages. Now you can go play around with the :ref:`Python API <api>`, or
take a look at some example :ref:`tutorials <tutorials>` The tutorials are all
written in jupyter notebook, which you don't have using this installation
method. However, you can still take a look at the procedure and the code, and
use it as an example of what can be accomplished using **chipwhisperer**.


.. _install-repo-git:

Git
===

If you want the cutting-edge version of ChipWhisperer, you can clone the
repository.

.. note::

   On Unix based OS (Mac, Linux, etc), python often links to python2. You
   may need to replace python and pip calls with python3 and pip3 calls,
   respectively

If you have Git already set up, this is easy to do:

.. code:: bash

    git clone https://github.com/newaetech/chipwhisperer.git
    cd chipwhisperer

    # To get the jupyter notebook tutorials
    git submodule update --init jupyter
    python -m pip install -r jupyter/requirements.txt --user

    # note the setup.py used to be in software, it was moved
    # in version 5.1.0

    # use pip to install in develop mode
    python -m pip install -e . --user

The user flag installs ChipWhisperer in the user's local python
site-packages directory.

You may also want the OpenADC software, which is necessary to build new
firmware for the ChipWhisperer FPGA. This is unnecessary for most users. If
you need it:

.. code::

    cd ..
    git submodule update --init openadc
    cd openadc/controlsw/python
    python -m pip install -e . --user

Once ChipWhisperer is installed, you can :ref:`run chipwhisperer <starting>`.
