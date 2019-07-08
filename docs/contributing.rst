.. _sec-contributing:

************
Contributing
************

The ChipWhisperer `software`_ is documented using sphinx and lives on
ReadTheDocs. However, NewAE Technology Inc. also develops hardware for
other types of side-channel analysis and embedded hardware security fun.
The documentation for the hardware is on the `MediaWiki site`_. The
tutorials for using the ChipWhisperer software are also on the
`MediaWiki site`_ because they are generated static Jupyter notebooks
which do not need to be included in the GitHub repository.

.. _software: https://github.com/newaetech/chipwhisperer
.. _MediaWiki site: https://wiki.newae.com


.. _sec-contributing-software_docs:

Software Documentation
======================

The software documentation is written using Sphinx.
Building the documentation requires sphinx. This can be installed using::

    python -m pip install sphinx

You can then navigate to the *chipwhisperer/docs* folder and run::

    sphinx-build -b html . _build

This will build the documentation in html form. You can then navigate to
the *_build* directory and open *index.html*. This should open the website
in your browser.

Contribution to the documentation is always welcome. To see the effect of
changes to the documentation it needs to be rebuilt after each change. This
is done using the sphinx-build command. When contributing to the documentation
here are some rough guidelines to follow:

 *  All directive blocks are indented by four spaces. This makes life easier
    when the text editor you are using is already set for four spaces because
    of python. For example:

    .. code:: ReST

        |.. note::
        |    A note to the user.

 *  Indent unnumbered lists with two spaces and a space after the symbol.
    So that the text block starts at column five. Example::

    | *  list item 1
    | *  list item 2

 *  Indent numbered lists with one space, then a period and a space.
    Example:

    .. code:: ReST

        | #. numbered list item 1
        | #. numbered list item 2
        |    Continuation of code block.

 *  We use **autodocs** for our API documentation, however to get the
    documentation of the api to look clean a lot of the module and class
    paths have to be manually specified.

 *  Refrain from adding new functions or classes to the API as we would
    like to keep the API as consistent as possible.

 *  Follow the `google style guide`_ for docstrings this makes the
    documentation easier to read while looking at the source code.

 *  Keep in mind that people will be reading both the generated
    documentation and the source code. Don't make either one too painful
    to read. This happens a lot with links to other functions. Links help
    readers of the generated documentation but look quite verbose in the source
    code. Find a balance.

 *  When adding section links, try to make them intuitive with the header and
    toctree hierarchy. For example: if a file named **info.rst** has a header
    **Additional Information** a good section link would be:

    .. code:: ReST

        .. info-additional-information:

        **********************
        Additional Information
        **********************

 *  Use this header structure for documentation:
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


.. _sec-contributing-hardware_docs:

Hardware Documentation
======================

Found on the mediawiki.

.. _sec-contributing-code_base:

Code Base
=========

Found on the media wiki


.. _sec-todo:

Todos
=====

Here is a list of all the project todos (Non-exhaustive). Feel free to
choose one and contribute.

.. todolist::