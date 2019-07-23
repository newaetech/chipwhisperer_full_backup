.. _getting_started:

########
Overview
########

Welcome to the home of the installation guide, software documentation,
and tutorials for the ChipWhisperer tool-chain (this site)! This page
is meant to give you an overview of the most recent
:ref:`major changes <getting_started-major_changes>`, and the provide
a overview to get you :ref:`oriented <getting_started-overview>`. Consider
this page the answer to "Help, I'm confused... there are two many websites
that have resources related to ChipWhisperer".

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
    to meat your needs, however before the change this was sometimes
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
    documentation, and the tutorials are on this site. All other resources
    stay on the `NewAE Wiki`_. This site will be version based on releases
    of the software. Thank you `readthedocs`_!

  * :ref:`Tutorials <tutorials>` are now auto-generated with our tutorial
    build system. The jupyter notebooks that are included in the
    `ChipWhisperer GitHub repository`_  in the *jupyter* submodule
    are build with multiple different types of hardware. The output is on
    the :ref:`tutorials <tutorials>` page of this site. Just like when you
    complete the tutorials some of the tutorials will contain output with
    warnings during the capture of power traces. This will make it easier
    to know what to expect when running the tutorials yourself.


.. _getting_started-overview:

********
Overview
********

You may find there are a lot of different sites with ChipWhisperer related
content. The question is how do you make sense of them all. Here is an
attempt at an overview of the resources related to the ChipWhisperer
tool-chain:

`NewAE Wiki`_:

    The home of all the documentation for ChipWhisperer hardware, and extra
    wiki stuff from NewAE, including usage of ChipWhisperer in academics
    papers, side-channel analysis theory and . If you
    need to find out how to set up a board you have this is a great place
    to start. Ever since the release of ChipWhisperer software 5.0.0 this
    site does **not** hold the most up to date tutorials for using the
    ChipWhisperer software with your board, however, it holds the tutorials
    for all previous version of the ChipWhisperer software.

    The wiki hosts all the extra theory for learning some of the
    side-channel attacks (Ex. `CPA theory`_) excluding what theory is
    included in the :ref:`tutorials` on this readthedocs site.

`ChipWhisperer ReadTheDocs`_:

    This site. It contains the software
    :ref:`installation procedure <install>`, software
    :ref:`API documentation <api>`, and the :ref:`tutorials <tutorials>` for
    the ChipWhisperer tool-chain. This is newly introduced since
    ChipWhisperer 5.1.0.

`newaetech/chipwhisperer Github Repository`_:

    The ChipWhisperer software is open-source, and can be found on our
    GitHub repository. If you are a fan of having the latest and greatest,
    with the latest and greatest bugs you can also check out the **develop**
    branch. **master** is where the stable releases go. If you want to
    contribute to the project this is the place for you. Fork the branch
    with the most up to date changes, make your changes, and then take out
    a pull request. We will review and discuss the changes then. Also,
    here is a link to our :ref:`contributing page <contributing>`.

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

`NewAE Website`_:

    The is the website for NewAE Technolgy Inc. the maintainers of the
    ChipWhisperer software and the creators the of the ChipWhisperer
    platform. Here you can find out about the company, and find links
    to things like the store. Here you can you also find some other
    the other products NewAE Technology Inc. sells like the
    electromagnetic fault injection platform, the ChipSHOUTER.

`NewAE Store`_:

    Here you can get yours hands on one of those hardware boards used by all
    of the tutorials. NewAE Technology sells hardware to take away the
    frustration of setting up the hardware for side-channel attacks. These
    boards are available at the `NewAE Store`_.


.. _NewAE Store: https://store.newae.com/
.. _Jupyter Notebook: https://jupyter.org/
.. _readthedocs: http://readthedocs.org/
.. _NewAE Wiki: https://wiki.newae.com/Main_Page
.. _ChipWhisperer ReadTheDocs: https://chipwhisperer.readthedocs.io
.. _newaetech/chipwhisperer Github Repository: https://github.com/newaetech/chipwhisperer
.. _Issue Tracker: https://github.com/newaetech/chipwhisperer/issues
.. _CPA theory: https://wiki.newae.com/Correlation_Power_Analysis
.. _ChipWhisperer GitHub repository: https://github.com/newaetech/chipwhisperer
.. _NewAE Forum: https://forum.newae.com/
.. _NewAE Website: https://newae.com/