# Contributing

If you're curious about contributing to ChipWhisperer, be it software or hardware, this is the
document for you!

The ChipWhisperer [software](https://github.com/newaetech/chipwhisperer) is documented using sphinx and lives on
ReadTheDocs. However, NewAE Technology Inc. also develops hardware for
side-channel analysis and embedded hardware security fun. The documentation
for the hardware is on a [separate site](https://rtfm.newae.com). The tutorials are also
located on ReadTheDocs and are auto-generated based on the jupyter notebooks.

Contribution to the project is always welcome. We are a small team and
fixing everything right away or making those enhancements you dream of
are often a game of managing priorities. If there is a feature you would
like to help with, let us know. If you would like to start out small and
get familiar with the project whilst contributing, there are always ways
the software documentation could be improved. The software docs are all on
github. You can fork the **chipwhisperer** repository make changes and then
take out a pull request. We will discuss the changes then.

## Hardware

We're not currently accepting direct modifications to our hardware docs, but may in the future.
For now, please make a thread on our [forums](https://forum.newae.com)

## Software

Our source code can be found the [ChipWhisperer GitHub Repository](https://github.com/newaetech/chipwhisperer). Issues and enhancements being worked on can be found in our [GitHub issue tracker](https://github.com/newaetech/chipwhisperer/issues). If you need ideas about what to work on, this is the place to go. Development of `ChipWhisperer` takes place on the `develop` branch. Every release, `develop` is merged into `master`.

In general, we prefer methods, functions, variables, and modules to be in `snake_case` instead of `camelCase`. You'll see a lot
of `camelCase` scattered throughout the code as this was preferred before ChipWhisperer 5, but all public facing code - what's documented
in the API - is now `snake_case`. Class names should be in `PascalCase`. Docstrings should be in the Google style. Besides that, there aren't too many requirements for small fixes and additions.

For larger additions (such as adding a whole new scope or target), we have
a few requirements. Arguments that must be present, but can be ignored,
are indicated as well. For example, `scope._con()` must have an `sn` parameter,
but the method doesn't need to do anything with the value and as such may be ignored.

### Scopes

Scopes can have a wide variety of features. As such, there are only a few additional 
requirements:

Must inherit the `ScopeTemplate` object located in `chipwhisperer.capture.scopes.base` and
must implement the following methods:

```python
def _con(self, sn):
    """Connects to the scope. May also be called con().

    Args:
        sn (str): Unique identifier for the device. May be ignored, must be optional

    """
    ...

  def _dis(self):
    """Disconnects from the scope. May also be called dis().

    """
    ...
```

The following methods are required for a scope to be used with `cw.capture_trace()` and
otherwise recommended:

```python
def arm(self):
    """Prepares the scope to capture a power trace"""
    ...

def capture(self):
    """Reads power trace data from the scope.
    
    Returns:
        False upon success and True upon failure
    """
    ...

def get_last_trace(self):
    """Returns the scope data read by capture()

    Returns:
        A numpy array containing the scope data.
    """
    ...
```

### Targets

Targets must inherit the `TargetTemplate` object located in `chipwhisperer.capture.targets._base`
and must implement the following methods:

```python
def _con(self, scope):
    """ Connects to the target. May also be called `con`. Will be passed **kwargs from `cw.target()`.

    Args:
        scope (CW scope object): May be ignored
    """
    ...

def _dis(self):
    """ Disconnects from the target. May also be called `dis`
    """
    ...

```

The following methods are optional as not all targets will be able to implement them (for example,
public crypto may not be able to transmit a message/key in a single command as required by `simpleserial_write()`). 
These methods, however, are required for a target to be used with `cw.capture_target()` and should be implemented
as specified.

```python
def set_key(self, key, ack):
    """ Sets the encryption key on the target. Should not resend the key
    to the target if key is the same as the one previously sent by this method.

    Args:
        key (bytearray): The key to be sent to the target. Must not be ignored.
        ack (bool): Whether or not to wait for an ack by the target. May be ignored. Must be optional.
    """
    ...

def simpleserial_write(self, cmd, data, end):
    """ Sends a command to the target, along with data.

    Args:
        cmd (str): The command to send to the target. Must accept
        `'k'` for sending a key and `'p'` for sending input.
        Must not be ignored.
        data (bytearray): The data to send to the target as part of cmd.
        Must not be ignored
        end (str): A terminator for the end of the command.
        May be ignored. Must be optional.
    """
    ...

def simpleserial_read(self, cmd, pay_len, end, timeout, ack):
    """ Reads a response from the target.

    Args:
        cmd (str): The command to expect from the target. Must accept
        'r' for a response after sending 'p'. May be ignored.
        pay_len (int): The length of the expected response from the target.
        May be ignored.
        end (str): The expected termination to the response. May be ignored.
        Must be optional.
        timeout (int): A timeout value for the read. May be ignored.
        Must be optional.
        ack (bool): Whether or not to expect an ack at the end of the message.
        May be ignored. Must be optional.

    Returns:
      Must return the response as a bytearray not including cmd, end, or the ack.
    """
    ...

def is_done(self):
    """Indicates whether or not the target is finished its operation.

    Returns:
        True if the target is finished, False if it isn't. May immediately return True if
        a successful scope.capture() indicates that the device is finished its operation.
    """
    ...
```

### Software Documentation

The software documentation is written using Sphinx.
Building the documentation requires sphinx, sphinx images and pypandoc. 
This can be installed using

```bash
    python -m pip install sphinx
    python -m pip install sphinxcontrib-images
    python -m pip install pypandoc
```

You can then navigate to the `chipwhisperer/docs` folder and run

```bash
    make html
```

This command does a few things:

  * Loads the sphinx configuration file.

  * Runs a build hook called `create_tutorial_files` it can be found
    in the `conf.py` file in the `docs/` folder. The function
    copies over the `img` directory from the `tutorials/` directory
    into the `docs/tutorials` folder. It then creates auto-linked
    tutorial files in the `docs/tutorials` for every tutorials in the
    `tutorials` submodule, based on the tutorial id, scope and target.

    This function also deletes the `docs/tutorials/img`, and removes
    all `.rst` files in the `docs/tutorials` folder. It is better
    not to put anything in these folders unless it is auto-generated
    each time the build is run.

    You should see the **Generating tutorial stubs with links...** message
    from the build hook as part of the build.

  * Runs a build hook called `generate_contributing`, which
    downloads and installs pandoc using pypandoc, and converts
    this document into an `.rst` file for inclusion in the
    sphinx build.

  * Carries out the rest of the normal sphinx build process.

This will build the documentation in html form. You can then navigate to
the `_build` directory and open `index.html`. This should open the website
in your browser.

Contribution to the documentation is always welcome. To see the effect of
changes to the documentation it needs to be rebuilt after each change. This
is done using the sphinx-build command. When contributing to the documentation
here are some rough guidelines to follow:

  * All directive blocks are indented by four spaces. This makes life easier
    when the text editor you are using is already set for four spaces because
    of python. For example:

    ```ReST

        |.. note::
        |    A note to the user.
    ```

  * Indent unnumbered lists with two spaces and a space after the symbol.
    So that the text block starts at column five. Example:

    ```ReST

        |  * list item 1
        |  * list item 2
    ```

  * Indent numbered lists with one space, then a period and a space.
    Example:

    ```ReST

        | #. numbered list item 1
        | #. numbered list item 2
        |    Continuation of code block.
    ```

  * We use **autodocs** for our API documentation, however to get the
    documentation of the api to look clean a lot of the module and class
    paths have to be manually specified.

  * When using **autodoc** do not use the **autoproperty** directive for
    properties. Just use **autoattribute**. **autoproperty** causes errors
    on ReadtheDocs.

  * Follow the [google style guide](https://www.sphinx-doc.org/en/1.5/ext/example_google.html) for docstrings this makes the
    documentation easier to read while looking at the source code.

  * Keep in mind that people will be reading both the generated
    documentation and the source code. Don't make either one too painful
    to read. This happens a lot with links to other functions. Links help
    readers of the generated documentation but look quite verbose in the source
    code. Find a balance.

  * When adding section links, try to make them intuitive with the header and
    toctree hierarchy. For example: if a file named **info.rst** has a header
    **Additional Information** a good section link would be:

    ```ReST

        .. info-additional_information:

        **********************
        Additional Information
        **********************
    ```

    There are lots of examples throughout the documentation.

  * Use this header structure for documentation:
      - \# with overline, for parts
      - \* with overline, for chapters
      - =, for sections
      - -, for subsections
      - ^, for subsubsections
      - “, for paragraphs

A few useful resources:

  * [Restructured Text and Sphinx CheatSheet](http://openalea.gforge.inria.fr/doc/openalea/doc/_build/html/source/sphinx/rest_syntax.html)
  * [Example Doc Strings \(Google Style Guide\)](https://www.sphinx-doc.org/en/1.5/ext/example_google.html)
  * [Napolean \(Google Style to ReST preprocessor\)](https://www.sphinx-doc.org/en/master/usage/extensions/napoleon.html)
  * [reStructuredText - Docutils ](http://docutils.sourceforge.net/rst.html)

## Testing

ChipWhisperer software is currently tested in two ways:

* Using Jupyter Notebooks
* Unit Tests

### Jupyter Notebooks

The tutorials are not only a great resource for people learning to
use the platform, they also are the functional tests for this project.
We use the autogenerated output for the tutorials
page. Each tutorial has a test section at the end of the notebook
which asserts certain test criteria. The criteria is often hard to chose
due to the nature of the attacks, especially glitching.

#### Creating / Editing

When editing or creating tutorials here are a few things to consider:

  * If the tutorial is meant to be run using some of the chipwhisperer
    hardware the first source code block must contain
    ```python

        SCOPETYPE = 'OPENADC'
        PLATFORM = 'CWLITEARM'
        CRYPTO_TARGET = 'TINYAES128C'
    ```

    or equivalent for the different types of hardware. Don't worry about
    getting it right, when you are creating a tutorial, we can help you
    is you take out a pull request. The reason this block is there is for
    use with nbparameterize for running our automated tests so we can figure
    it out for you.

  * When writing in markdown cells please use the [pandoc style markdown](https://rmarkdown.rstudio.com/authoring_pandoc_markdown.html)

  * This list contains guidelines and will update over time. We will run the
    tutorials through the tutorial build system and fix errors that come up.

#### Tutorials Test/Export System

The tutorials are run as tests on our own testing machine where we keep
the board used for each hardware type consistent. We are looking to improve
this, and have the beginning of a full test system created. It can be found
in the `jupyter/utils/tutorials.py` script. It uses a configuration file
to specify the tutorials and which type of hardware it currently should support.

The configuration file is read in and the tutorials are run for all
different configurations. The configuration file is written using YAML
and the syntax is as follows

```YAML
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
```

You can find the current version in our [GitHub Repository](https://github.com/newaetech/chipwhisperer/) as the file
**jupyter/tests/tutorials.yaml**. To run the tests use

```bash
    cd chipwhisperer/jupyter/tests/
    python tutorials.py
```

This should run the tests in all known configurations and give the output
of passed and output written to output/file/path/ or the first error that
occurred in each notebook. There is also a notebook for running tests
individually if that is needed.

#### Running Tutorial Tests/Export Individually

Inside the `chipwhisperer/jupyter` folder there is the notebook called
`Test_Notebook.ipynb`. This notebook can be used to run the tests individually.
This is great for working on getting a tutorial running or testing a single
notebook you have made changes for.

 1. Open the notebook as you would any other
    notebook and run all the blocks leading up to the blocks containing
    different blocks similar to
    ```python
        testscope = 'OPENADC'
        testplat = 'CWLITEARM'
        crypt = 'TINYAES128C'
    ```
 1. Then choose the block that is correct for your attached hardware and run
    that.
 1. Finally select the block containing the notebook you want to test from the
    code cells following.

### Unit Tests

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

Unit tests can be found in the `chipwhisperer/tests` directory. There is no
auto discover script yet so just run:

```bash

    cd chipwhisperer/tests

    python test_api.py

```