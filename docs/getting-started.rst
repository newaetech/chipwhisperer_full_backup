.. _getting_started:

########
Overview
########

New to ChipWhisperer? No problem!

This page will give a brief overview of what ChipWhisperer is, where you can
find documentation about different parts of the ChipWhisperer toolchain,
as well as some of the major changes that have recently happened to ChipWhisperer.

Consider this page the answer to "Help, I'm confused... there are too many websites
that have resources related to ChipWhisperer".

.. _getting_started_what-is-chipwhisperer:

**********************
What is ChipWhisperer?
**********************

ChipWhisperer is a complete open source toolchain for learning about
side channel attacks on embedded devices and validating
the side channel resistance of these devices. In particular,
ChipWhisperer focuses on power analysis, which uses information
leaked by a device's power consumption to mount an attack, as well
as voltage and clock glitching attacks, which briefly disrupt
a device's power or clock to cause unintended behaviour (such 
as skipping a password check).

.. _getting_started-links:

**********
Components
**********

In total, ChipWhisperer has four layers of open source components:

.. _getting_started-hardware:

=========
Hardware
=========

ChipWhisperer has scope boards, which are used to mount side channel attacks,
as well as target boards, which function as a device under test (DuT).
Hardware documentation can be found at https://rtfm.newae.com

Hardware is for the most part open source and design files/schematics
can be found either on the `ChipWhisperer Github repository`_,
or on a `UFO target board repository`_.

.. _getting_started-firmware:

=========
Firmware
=========

ChipWhisperer also includes open source firmware for both scopes
and targets. 

Scope firmware is written in Verilog (for
the FPGA) and C (USB microcontroller) and can be found in the
:code:`hardware/capture` sections of the `ChipWhisperer
Github repository`_.

Target firmware is mostly written in C (though we do have
a few FPGA targets with code in Verilog) and can be found 
in the :code:`hardware/victims/firmware` directory of
the `ChipWhisperer Github repository`_.

.. _getting_started-software:

=========
Software
=========

ChipWhisperer has an open source Python library for controlling the
capture hardware and communicating with the target. `ChipWhisperer ReadTheDocs`_
(this website) has the :ref:`API documentation <api>` and :ref:`installation instructions <install>` 
for this part of the ChipWhisperer toolchain.

The source code for the API is located on the `ChipWhisperer Github repository`_.

.. _getting_started-tutorials:

==========
Tutorials
==========

Finally, ChipWhisperer also includes Jupyter Notebook tutorials and labs that both
teach about side channel attacks that can be performed with ChipWhisperer,
as well as showcase how to use the Python API. These tutorials/labs 
are available in the `ChipWhisperer Jupyter Github repository`_. Some
tutorials are also available as a completed static webpage over on the
:ref:`tutorials <tutorials>` page.

.. _getting_started-important-links:

***************
Important Links
***************

`Issue Tracker`_:

    The issue tracker for the GitHub repository. If you have any issues with
    the ChipWhisperer software that you suspect may be bugs, let us know
    there. Also, if you have any suggestions for future improvements this
    is also place to suggest them. All ideas/suggestions, and issues are
    welcome. Enhancements will be considered, and discussed there before
    the decision is made.

`NewAE Forum`_:

    The forum for all things related to side-channel analysis. If you are
    stuck and need help, you may find some much needed help here. Whether
    about ChipWhisperer hardware or with other projects related to
    side-channel analysis, all discussion is welcome. One of us at NewAE
    Technology usually monitors the forum and there is a good chance we
    can help you. There are also other active members that are also on the
    forum which may have run into the same problems you have. Before posting
    please take a look at other forum posts. You may find your answer
    quicker than you think.

`Online Courses`_:

    NewAE offers paid online courses that go in depth into
    things not covered by the free tutorials, such as 
    different power measurement techniques, as well as how
    to setup non ChipWhisperer hardware for side channel
    attacks.

`NewAE Website`_:

    The is the website for NewAE Technolgy Inc. the maintainers of the
    ChipWhisperer software and the creators the of the ChipWhisperer
    platform. Here you can find out about the company, and find links
    to things like the store, and upcoming training sessions.
    Here you can you also find some other the other products NewAE
    Technology Inc. sells like the electromagnetic fault injection platform,
    the ChipSHOUTER.

`NewAE Store`_:

    Here you can get yours hands on one of those hardware boards used by all
    of the tutorials. NewAE Technology sells hardware to take away the
    frustration of setting up the hardware for side-channel attacks. These
    boards are available at the `NewAE Store`_. Boards are also available
    on `Mouser`_.

.. _getting_started-major_changes:

*************
Major Changes
*************

There were some major changes related to where resources are located
and in what format, after ChipWhisperer 5.1.0 was released. A more
:ref:`detailed version <change_log>` of the version changes also exists.
Here are some of the major changes we are most excited about:

  * We changed our interface from the QT Graphical User Interface (GUI)
    we had before to using `Jupyter Notebook`_. Keeping the GUI usable
    required a lot of effort, and since we are a small team it often
    meant bugs stayed around much too long. Also the change gave us a
    chance to make our :ref:`Python API <api>` much more usable. Our
    project is open-source, and you are free to modify the software
    to meet your needs, however before the change this was sometimes
    quite difficult. We have not completely fixed the internals of
    the software but the new, documented API should make customization
    to your needs a whole lot easier.

  * We now have a :ref:`documented Python API <api>`. You can actually
    read what functions and classes do, and this API should stay reliable
    in between minor version changes. Anything not documented in the API
    is not guaranteed to stay backwards compatible. This allows us the
    freedom to make improvements to the internals while you have a usable
    API.

  * We switched to Python 3, finally. This completely breaks all backwards
    compatibility with previous versions (less than 5.0.0), if we had
    not already done that will all of our changes.

  * We have this site now. The installation procedure, the software
    documentation, and the tutorials are on this site. We've now
    got a `separate site for hardware docs <https://rtfm.newae.com>`_ as well. This site will 
    be version based on releases of the software. Thank you `readthedocs`_!

  * :ref:`Tutorials <tutorials>` are now auto-generated with our tutorial
    build system. The jupyter notebooks that are included in the
    `ChipWhisperer GitHub repository`_  in the *jupyter* submodule
    are built with multiple different types of hardware. The output is on
    the :ref:`tutorials <tutorials>` page of this site. Just like when you
    complete the tutorials some of the tutorials will contain output with
    warnings during the capture of power traces. This will make it easier
    to know what to expect when running the tutorials yourself.

.. _Mouser: https://www.mouser.com/Search/Refine?Keyword=newae
.. _UFO target board repository: https://github.com/newaetech/chipwhisperer-target-cw308t
.. _NewAE Store: https://store.newae.com/
.. _Jupyter Notebook: https://jupyter.org/
.. _NewAE Hardware Documentation: https://rtfm.newae.com
.. _ChipWhisperer ReadTheDocs: https://chipwhisperer.readthedocs.io
.. _Issue Tracker: https://github.com/newaetech/chipwhisperer/issues
.. _ChipWhisperer GitHub repository: https://github.com/newaetech/chipwhisperer
.. _NewAE Forum: https://forum.newae.com/
.. _NewAE Website: https://newae.com/
.. _ChipWhisperer Jupyter Github repository: https://github.com/newaetech/chipwhisperer-jupyter
.. _readthedocs: https://readthedocs.org
.. _Online Courses: https://learn.chipwhisperer.io