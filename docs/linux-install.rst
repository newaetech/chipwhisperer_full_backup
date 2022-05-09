.. _install-linux:

####################
Linux Installation
####################

The following instructions were designed for Ubuntu/Debian, but should
be easily adaptable to other Linux distros.

*******************
Quick Installation
*******************

If you're in a hurry, running the following commands and rebooting
should get you up and running:

.. code:: bash

    sudo apt update && sudo apt upgrade

    # python prereqs
    sudo apt-get install build-essential gdb lcov pkg-config \
        libbz2-dev libffi-dev libgdbm-dev libgdbm-compat-dev liblzma-dev \
        libncurses5-dev libreadline6-dev libsqlite3-dev libssl-dev \
        lzma lzma-dev tk-dev uuid-dev zlib1g-dev curl

    sudo apt install libusb-dev make git avr-libc gcc-avr \
        gcc-arm-none-eabi libusb-1.0-0-dev usbutils

    # install pyenv - skip if already done
	curl https://pyenv.run | bash
	echo 'export PATH="~/.pyenv/bin:$PATH"' >> ~/.bashrc
	echo 'export PATH="~/.pyenv/shims:$PATH"' >> ~/.bashrc
	echo 'eval "$(pyenv init -)"' >> ~/.bashrc 
	echo 'eval "$(pyenv virtualenv-init -)"' >> ~/.bashrc

    source ~/.bashrc

	pyenv install 3.9.5
	pyenv virtualenv 3.9.5 cw
    pyenv activate cw

    cd ~/
    git clone https://github.com/newaetech/chipwhisperer
    cd chipwhisperer
    sudo cp hardware/50-newae.rules /etc/udev/rules.d/50-newae.rules
    sudo udevadm control --reload-rules
    sudo usermod -aG dialout $USER
    sudo usermod -aG plugdev $USER
    git submodule update --init jupyter

    python setup.py develop
    python -m pip install -r jupyter/requirements.txt

Next, we'll go through those commands

*******************
Manual Installation
*******************

=================
Required Packages
=================

Begin by updating all your packages:

.. code:: bash

    sudo apt update && sudo apt upgrade

From there, we'll need prerequisites for building Python from pyenv:

.. code:: bash

    sudo apt install build-essential gdb lcov pkg-config \
        libbz2-dev libffi-dev libgdbm-dev libgdbm-compat-dev liblzma-dev \
        libncurses5-dev libreadline6-dev libsqlite3-dev libssl-dev \
        lzma lzma-dev tk-dev uuid-dev zlib1g-dev curl

Next, grab the prerequisites for building firmware for targets:

.. code:: bash

    sudo apt install libusb-dev make git avr-libc gcc-avr \
        gcc-arm-none-eabi libusb-1.0-0-dev usbutils

======
Python
======

The recommended way to grab Python is to use pyenv, as it allows
you to use multiple copies of Python without having to fiddle with
paths. If you've already got pyenv installed, you can skip this step,
though you may still want to setup your own copy of Python for ChipWhisperer.

The first step is to grab pyenv:

.. code:: bash

	curl https://pyenv.run | bash

Next, we need to add the following to our :code:`.bashrc` file,
so that pyenv starts properly with your terminal:

.. code:: bash

	echo 'export PATH="~/.pyenv/bin:$PATH"' >> ~/.bashrc
	echo 'export PATH="~/.pyenv/shims:$PATH"' >> ~/.bashrc
	echo 'eval "$(pyenv init -)"' >> ~/.bashrc 
	echo 'eval "$(pyenv virtualenv-init -)"' >> ~/.bashrc

    #apply these changes to current terminal
    source ~/.bashrc 

With that done, install a version of Python. Anything >= 3.7 should work,
but we'll grab 3.9.5 here and make an env called cw:

.. code:: bash

	pyenv install 3.9.5
	pyenv virtualenv 3.9.5 cw
    pyenv activate cw

========================
Installing ChipWhisperer
========================

Start by grabbing ChipWhisperer from Github. Install wherever you like,
but the code below will download it to your home directory:

.. code:: bash

    cd ~/
    git clone https://github.com/newaetech/chipwhisperer
    cd chipwhisperer
    git submodule update --init jupyter # grab Jupyter notebook tutorials

Next, we'll need to make some udev rules so that we have permission
to access USB and serial devices. ChipWhisperer includes a default
set of rules that give you access to NewAE USB devices:

.. code:: bash

    sudo cp hardware/50-newae.rules /etc/udev/rules.d/50-newae.rules
    sudo udevadm control --reload-rules

Those rules actually give permission to the plugdev group, so
you'll need to add your user to that group:

.. code:: bash

    sudo usermod -aG dialout $USER
    sudo usermod -aG plugdev $USER

Not all Linux distros have these groups, so the above gives you errors,
try creating the groups and rerunning that step.

With that done, all that's left is to install ChipWhisperer:

.. code:: bash

    python setup.py develop
    python -m pip install -r jupyter/requirements.txt