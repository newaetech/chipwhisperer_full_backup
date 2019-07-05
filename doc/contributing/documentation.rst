*********************************
Contributing to the Documentation
*********************************

The ChipWhisperer `software`_ is documented using sphinx and lives on
ReadTheDocs. However, NewAE Technology Inc. also develops hardware for
other types of side-channel analysis and embedded hardware security fun.
The documentation for the hardware is on the `MediaWiki site`_. The
tutorials for using the ChipWhisperer software are also on the
`MediaWiki site`_ because they are generated static Jupyter notebooks
which do not need to be included in the GitHub repository.

.. _software: https://github.com/newaetech/chipwhisperer
.. _MediaWiki site: https://wiki.newae.com

Building
========

Building the documentation requires sphinx. This can be installed using::

    python -m pip install sphinx

You can then navigate to the *chipwhisperer/docs* folder and run::

    sphinx-build -b html . _build

This will build the documentation in html form. You can then navigate to
the *_build* directory and open *index.html*. This should open the website
in your browser.

Contributing
============

Contribution to the documentation is always welcome. To see the effect of
changes to the documentation it needs to be rebuilt after each change. This
is done using the sphinx-build command. When contributing to the documentation
here are some rough guidelines to follow:

 * All directive blocks are indented by four space instead of three.
   This life easier when the text editor you are using is already set for
   four spaces because of python. For example:

   .. code:: ReST

       .. note::
           A note to the user.

 * We use **autodocs** for our API documentation, however to get the
   documentation of the api to look clean a lot of the module and class
   paths have to be manually specified.

 * Refrain from adding new functions or classes to the API as we would
   like to keep the API as consistent as possible.

 * Follow the `google style guide`_ for docstrings this makes the
   documentation easier to read while looking at the source code.

 * Keep in mind that people will be reading both the generated
   documentation and the source code. Don't make either one too painful
   to read. This happens a lot with links to other functions. Links help
   readers of the generated documentation but look quite verbose in the source
   code. Find a balance.

 * When adding section links, try to make them intuitive with the toctree
   structure.

.. _google style guide: https://www.sphinx-doc.org/en/1.5/ext/example_google.html