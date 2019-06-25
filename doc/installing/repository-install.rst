.. _sec-install-repo:

************************
Installing ChipWhisperer
************************

.. note:: You must have the pre-requisites for your system installed
	before continuing with the installation of the repository.

After satisfying prerequisites for your system, install the ChipWhisperer 
repository/package using one of:

:ref:`sec-install-repo-releases` (Recommended)
	Get the latest stable release from the GitHub repository. The release includes
	repository and zipped drivers.

:ref:`sec-install-repo-pypi`
	The classic :code:`pip install chipwhisperer`. Does not install
	the drivers, or hardware source code currently.

:ref:`sec-install-repo-git`
	Install the latest features and bugs from our GitHub repository.


.. _sec-install-repo-releases:

GitHub Releases
===============

Once you have a working Python 3 installation, you're ready to install and run ChipWhisperer.

First, download a ChipWhisperer release. You can get these from the `releases`_ page. Generally, the latest release is a good choice, but you might need an older version for various reasons. You want the source code in .zip or .tar.gz format - not a VBox image.

Next, uncompress your downloaded source code somewhere. Generally, 'somewhere' will become your ChipWhisperer working directory. For example, on Windows, you might want to use *C:\\chipwhisperer\\*.

Once you've got the file, install the python dependencies and run the Python install procedure (setup.py). Use the develop command to indicate that the files will probably be changing frequently. to do this, open a terminal and run the following, adjusting paths as needed:

.. code:: bash
	
	cd chipwhisperer/software
	pip install -r requirements.txt
	python setup.py develop 


To test, run python and try importing the chipwhisperer module:

.. code:: python

	>>> import chipwhisperer as cw

If you want to run the tutorial notebooks, run:

.. code::

	jupyter notebook

A new window should be opened in your browser. 

.. _releases: https://github.com/newaetech/chipwhisperer/releases


.. _sec-install-repo-pypi:

PyPi
====


.. _sec-install-repo-git:

Git
===

If you want the cutting-edge version of ChipWhisperer, you can clone the 
repository.

.. note:: 

   On Unix based OS (Mac, Linux, etc), python often links to python2. You 
   may need to replace python and pip calls with python3 and pip3 calls, 
   respectively

If you have Git already set up, this is easy to do:

.. code::

	git clone https://github.com/newaetech/chipwhisperer.git
	cd chipwhisperer
	git checkout cw5dev
	cd software
	pip install -r requirements.txt
	python setup.py develop --user

The user flag installs ChipWhisperer in the user's local python 
site-packages directory.

You may also want the OpenADC software, which is necessary to build new 
firmware for the ChipWhisperer FPGA. This is unnecessary for most users. If 
you need it:

.. code::

	cd ..
	git submodule init
	git submodule update
	cd openadc/controlsw/python
	python setup.py develop --user

Once ChipWhisperer is installed, you can run it through Jupyter by typing

.. code::
	
	jupyter notebook

into the command prompt, which should open a new tab in your browser. 
Navigate to *jupyter/* to get started with the new Jupyter tutorials. 
