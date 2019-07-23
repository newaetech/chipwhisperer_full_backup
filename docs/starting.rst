.. _starting:

########
Starting
########

After finishing the installation process, there are two different ways of
using the **chipwhisperer**. You will most likely be using a Jupyter
server to run a Jupyter notebook inside of your browser. This will make
following the :ref:`tutorials <tutorials>` easier.

*****************
Jupyter Notebooks
*****************

There are a lot of online resources for learning how to use Jupyter
Notebooks, a search will come up with too many. We will just provide a
quick start guide based on you having finished the installation of
**chipwhisperer**.

 #. Make sure you are using a bash like terminal. Many of the jupyter
    notebooks use bash to run **make** or remove files for building
    firmware. This may be changed in the future but for now it is
    required. If you installed using the Virtual Machine (VM) the
    terminal is a bash like terminal and you should have no problem.
    If you installed on linux, same deal. However, if you installed
    on Windows/Mac you may have to install a bash like terminal. If
    you installed Git to install **chipwhisperer** you already have
    **git-bash** available to you. Here are a few bash-like terminals
    available on other windows:

      * (Recommended) `Git-Bash <https://git-scm.com/>`_ (make sure you select to
        install git bash during the installation of Git.)

      * `MinGW <http://mingw.org/>`_

      * `Cygwin <https://www.cygwin.com/>`_

 #. Start the bash terminal. Make sure you have access to
    **chipwhisperer** in the terminal using::

        python -c "import chipwhisperer as cw"

    This should exit without printing anything. If you get something
    like "module not found" then you should read up on PYTHONPATH and
    system PATH.

 #. Navigate to the **chipwhisperer** directory. If you installed
    **chipwhisperer** to /home/user/chipwhisperer then use:

    .. code:: bash

        cd /home/user/chipwhisperer

 #. Start the Jupyter Server in that directory:

    .. code:: bash

        jupyter notebook

    The Jupyter Notebook Server interface should be automatically opened in
    your browser. If not see next step.

 #. Navigate to the Jupyter Notebook Server interface at **localhost:8888**
    using a browser and typing the address in the address bar.


 #. You should see the **chipwhisperer** folder in your browser, the tutorials
    can be found in the **jupyter** folder.


**************
Python Package
**************

The **chipwhisperer** software can also be used without Jupyter Notebooks if
you prefer. After you install **chipwhisperer** to the correct Python
interpreter, use **chipwhisperer** as you normally would any python package::

    import chipwhisperer as cw
    help(cw)

The :ref:`Python API <api>` has all the information about what functions,
and classes are at your disposal. You can also you the
:ref:`tutorials <tutorials>` as a reference as well.