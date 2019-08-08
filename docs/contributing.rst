.. _contributing:

############
Contributing
############

The ChipWhisperer `software`_ is documented using sphinx and lives on
ReadTheDocs. However, NewAE Technology Inc. also develops hardware for
side-channel analysis and embedded hardware security fun. The documentation
for the hardware is on the `MediaWiki site`_. The tutorials are also
located on this site and are auto-generated based on the jupyter notebooks.

Contribution to the project is always welcome. We are a small team and
fixing everything right away or making those enhancements you dream of
are often a game of managing priorities. If there is a feature you would
like to help with, let us know. If you would like to start out small and
get familiar with the project whilst contributing, there are always ways
the software documentation could be improved. The software docs are all on
github. You can fork the **chipwhisperer** repository make changes and then
take out a pull request. We will discuss the changes then.

Here are some areas you can contribute in:

:ref:`Software Documentation <contributing-software_docs>`:

    This is a great place to start if you are new to contributing. Docs can
    always be improved, and you can learn how to use the `Sphinx`_ build
    system for technical documentation. The software documentation (this site)
    lives in the **chipwhisperer/docs** folder of our `GitHub repository`_.

:ref:`Code Base <contributing-code_base>`:

    The code base is open-source and is located at our `GitHub repository`_.
    If you want to make functional changes to the way ChipWhisperer works
    this is the place to make them. As most of our API documentation is in
    the source code you may also find the
    :ref:`software docs <contributing-software_docs>` section helpful.
    It contains some rough guide lines on editing the doc strings.

:ref:`Hardware Docs <contributing-hardware_docs>`:

    If you find mistakes or think of corrections that would be helpful for
    the hardware documentation on the `NewAE Wiki`_, this section is for you.

:ref:`Testing <contributing-testing>`:

    Check out this section if you want to help make **chipwhisperer**
    more stable.


.. _GitHub repository: https://github.com/newaetech/chipwhisperer
.. _Sphinx: https://www.sphinx-doc.org/en/master/
.. _software: https://github.com/newaetech/chipwhisperer
.. _MediaWiki site: https://wiki.newae.com
.. _NewAE Wiki: https://wiki.newae.com


.. _contributing-software_docs:

**********************
Software Documentation
**********************

The software documentation is written using Sphinx.
Building the documentation requires sphinx. This can be installed using::

    python -m pip install sphinx

You can then navigate to the *chipwhisperer/docs* folder and run::

    sphinx-build -b html . _build

This command does a few things:

  * Loads the sphinx configuration file.

  * Runs a build hook called *create_tutorial_files* it can be found
    in the *conf.py* file in the *docs/* folder. The function
    copies over the *img* directory from the *tutorials/* directory
    into the *docs/tutorials* folder. It then creates auto-linked
    tutorial files in the *docs/tutorials* for every tutorials in the
    *tutorials* submodule, based on the tutorial id, scope and target.

    .. warning::
        This function also deletes the *docs/tutorials/img*, and removes
        all *.rst* files in the *docs/tutorials* folder. It is better
        not to put anything in these folders unless it is auto-generated
        each time the build is run.

    You should see the **Generating tutorial stubs with links...** message
    from the build hook as part of the build.

  * Carries out the rest of the normal sphinx build process.

This will build the documentation in html form. You can then navigate to
the *_build* directory and open *index.html*. This should open the website
in your browser.

Contribution to the documentation is always welcome. To see the effect of
changes to the documentation it needs to be rebuilt after each change. This
is done using the sphinx-build command. When contributing to the documentation
here are some rough guidelines to follow:

  * All directive blocks are indented by four spaces. This makes life easier
    when the text editor you are using is already set for four spaces because
    of python. For example:

    .. code:: ReST

        |.. note::
        |    A note to the user.

  * Indent unnumbered lists with two spaces and a space after the symbol.
    So that the text block starts at column five. Example:

    .. code:: ReST

        |  * list item 1
        |  * list item 2

  * Indent numbered lists with one space, then a period and a space.
    Example:

    .. code:: ReST

        | #. numbered list item 1
        | #. numbered list item 2
        |    Continuation of code block.

  * We use **autodocs** for our API documentation, however to get the
    documentation of the api to look clean a lot of the module and class
    paths have to be manually specified.

  * When using **autodoc** do not use the **autoproperty** directive for
    properties. Just use **autoattribute**. **autoproperty** causes errors
    on ReadtheDocs.

  * Follow the `google style guide`_ for docstrings this makes the
    documentation easier to read while looking at the source code.

  * Keep in mind that people will be reading both the generated
    documentation and the source code. Don't make either one too painful
    to read. This happens a lot with links to other functions. Links help
    readers of the generated documentation but look quite verbose in the source
    code. Find a balance.

  * When adding section links, try to make them intuitive with the header and
    toctree hierarchy. For example: if a file named **info.rst** has a header
    **Additional Information** a good section link would be:

    .. code:: ReST

        .. info-additional_information:

        **********************
        Additional Information
        **********************

    There are lots of examples throughout the documentation.

  * Use this header structure for documentation:
      - # with overline, for parts
      - \* with overline, for chapters
      - =, for sections
      - -, for subsections
      - ^, for subsubsections
      - “, for paragraphs

A few useful resources:
  * `Restructured Text and Sphinx CheatSheet <http://openalea.gforge.inria.fr/doc/openalea/doc/_build/html/source/sphinx/rest_syntax.html>`_
  * `Example Doc Strings (Google Style Guide) <https://www.sphinx-doc.org/en/1.5/ext/example_google.html>`_
  * `Napolean (Google Style to ReST preprocessor) <https://www.sphinx-doc.org/en/master/usage/extensions/napoleon.html>`_
  * `reStructuredText - Docutils <http://docutils.sourceforge.net/rst.html>`_

.. _google style guide: https://www.sphinx-doc.org/en/1.5/ext/example_google.html


.. _contributing-hardware_docs:

**********************
Hardware Documentation
**********************

Found on the `NewAE Wiki`_ under the Hardware Documentation section. To
contribute or make changes to the wiki contact **wiki@newae.com**.


.. _contributing-code_base:

*********
Code Base
*********

Our source code can be found on the `GitHub repository`_. The issues and
enhancements being worked on can be found in the `GitHub issue tracker`_.
If you need ideas about what to work on, this is the place to go.
Development of **chipwhisperer** takes place on the **develop** branch
once tested, it is merged into the **master** branch and tagged for release.

Recommended Reading:

  * `Semantic Versioning <https://semver.org/>`_


.. _GitHub issue tracker: https://github.com/newaetech/chipwhisperer/issues


.. _contributing-testing:

*******
Testing
*******

This section covers the current ways that the **chipwhisperer** software
is being tested.

  * :ref:`Using the jupyter notebook tutorials <contributing-testing-tutorials>`.
    Currently the main method.
  * :ref:`Unit tests without hardware <contributing-testing-unittests>`.


.. _contributing-testing-tutorials:

Tutorials
=========

The tutorials are not only a great resource for people learning to
use the platform, they also are the functional tests for this project.
We use the autogenerated output for the :ref:`tutorials <tutorials>`
page. Each tutorial has a test section at the end of the notebook
which asserts certain test criteria. The criteria is often hard to chose
due to the nature of the attacks, especially glitching.

Creating / Editing
------------------

When editing or creating tutorials here are a few things to consider:

  * If the tutorial is meant to be run using some of the chipwhisperer
    hardware the first source code block must contain::

        SCOPETYPE = 'OPENADC'
        PLATFORM = 'CWLITEARM'
        CRYPTO_TARGET = 'TINYAES128C'

    or equivalent for the different types of hardware. Don't worry about
    getting it right, when you are creating a tutorial, we can help you
    is you take out a pull request. The reason this block is there is for
    use with nbparameterize for running our automated tests so we can figure
    it out for you.

  * When writing in markdown cells please use the `pandoc style markdown`_.

  * This list contains guidelines and will update over time. We will run the
    tutorials through the tutorial build system and fix errors that come up.

.. _pandoc style markdown: https://rmarkdown.rstudio.com/authoring_pandoc_markdown.html

Tutorials Test/Export System
----------------------------

The tutorials are run as tests on our own testing machine where we keep
the board used for each hardware type consistent. We are looking to improve
this, and have the beginning of a full test system created. It can be found
in the **jupyter/utils/tutorials.py** script. It uses a configuration file
to specify the tutorials and which type of hardware it currently should support.

The configuration file is read in and the tutorials are run for all
different configurations. The configuration file is written using YAML
and the syntax is as follows::

    # Expected Syntax:
    #
    # connected:
    #   scope: <VALID SCOPE NAME (OPENADC, CWNANO)>
    #   target: <VALID PLATFORM NAME (CWLITEARM, CWLITEXMEGA, CWNANO)>
    #   serial number: <device serial number (optional)>
    #
    # tutorials:
    #   <tutorial file name including extension>:
    #     configurations:
    #       - scope: <VALID SCOPE NAME>
    #         target: <VALID PLATFORM NAME>
    #         firmware: <VALID CRYPTO_TARGET NAME (TINYAES128C, AVRCYPTOLIB, MBEDTLS)>
    #       - ... any more configurations
    #     kwargs:  # extra keyword arguments to nbparameterise (optional)
    #       num_traces: <number of traces to capture>
    #       sample_size: <sample size>

You can find the current version in our `GitHub repository`_ as the file
**jupyter/tests/tutorials.yaml**. To run the tests use::

    cd chipwhisperer/jupyter/tests/
    python tutorials.py

This should run the tests in all known configurations and give the output
of passed and output written to output/file/path/ or the first error that
occurred in each notebook. There is also a notebook for running tests
individually if that is needed.

Running Tutorial Tests/Export Individually
------------------------------------------

Inside the **chipwhisperer/jupyter** folder there is the notebook called
**Test_Notebook.ipynb**. This notebook can be used to run the tests individually.
This is great for working on getting a tutorial running or testing a single
notebook you have made changes for.

 #. Open the notebook as you would any other
    notebook and run all the blocks leading up to the blocks containing
    different blocks similar to::

        testscope = 'OPENADC'
        testplat = 'CWLITEARM'
        crypt = 'TINYAES128C'


 #. Then choose the block that is correct for your attached hardware and run
    that.

 #. Finally select the block containing the notebook you want to test from the
    code cells following.

.. _contributing-testing-unittests:

Unit Tests
==========

There are also a few unit tests that have been created to test small parts of
the API. The unit tests are mostly used in places where we can test without
using the hardware. Here are some changes that have been though of that would
make the unit tests better.

  * Collect some traces that we know are good and use those for unit testing
    the analyzer. This should not be too hard, however, it just has to be done.

  * Find a way to test the capture side of things without using hardware.
    Might require substantial decoupling. Maybe exchange the serial backend
    of ChipWhisperer with a class that acts like a serial device. Then we
    could check is the serial messages are the ones we expect when using the
    API.

  * Increase coverage. This is related to the other improvements as they have
    to be partially completed before unit test coverage can expand drastically.

Unit tests can be found in the **chipwhisperer/tests** directory. There is no
auto discover script yet so just run::

    cd chipwhisperer/tests

    python test_api.py
