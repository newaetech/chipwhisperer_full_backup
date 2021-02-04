# This file is part of the OpenADC Project. See www.newae.com for more details,
# or the codebase at http://www.assembla.com/spaces/openadc .
#
# Copyright (c) 2015, Technology Inc. All rights reserved.
# Written by Colin O'Flynn <coflynn@newae.com>.
# This project is released under the Modified FreeBSD License. See LICENSE
# file which should have came with this code.

from PySide.QtCore import *
from PySide.QtGui import *
from PySide import QtWebKit
from docutils import core, io
import sys

def html_parts(input_string, source_path=None, destination_path=None,
               input_encoding='unicode', doctitle=True,
               initial_header_level=1):
    """
    Given an input string, returns a dictionary of HTML document parts.

    Dictionary keys are the names of parts, and values are Unicode strings;
    encoding is up to the client.

    Parameters:

    - `input_string`: A multi-line text string; required.
    - `source_path`: Path to the source file or object.  Optional, but useful
      for diagnostic output (system messages).
    - `destination_path`: Path to the file or object which will receive the
      output; optional.  Used for determining relative paths (stylesheets,
      source links, etc.).
    - `input_encoding`: The encoding of `input_string`.  If it is an encoded
      8-bit string, provide the correct encoding.  If it is a Unicode string,
      use "unicode", the default.
    - `doctitle`: Disable the promotion of a lone top-level section title to
      document title (and subsequent section title to document subtitle
      promotion); enabled by default.
    - `initial_header_level`: The initial level for header elements (e.g. 1
      for "<h1>").
    """
    overrides = {'input_encoding': input_encoding,
                 'doctitle_xform': doctitle,
                 'initial_header_level': initial_header_level}
    parts = core.publish_parts(
        source=input_string, source_path=source_path,
        destination_path=destination_path,
        writer_name='html', settings_overrides=overrides)
    return parts

def html_body(input_string, source_path=None, destination_path=None,
              input_encoding='unicode', output_encoding='unicode',
              doctitle=True, initial_header_level=1):
    """
    Given an input string, returns an HTML fragment as a string.

    The return value is the contents of the <body> element.

    Parameters (see `html_parts()` for the remainder):

    - `output_encoding`: The desired encoding of the output.  If a Unicode
      string is desired, use the default value of "unicode" .
    """
    parts = html_parts(
        input_string=input_string, source_path=source_path,
        destination_path=destination_path,
        input_encoding=input_encoding, doctitle=doctitle,
        initial_header_level=initial_header_level)
    fragment = parts['html_body']
    if output_encoding != 'unicode':
        fragment = fragment.encode(output_encoding)
    return fragment

class HelpBrowser(QDialog):

    def __init__(self, parent=None):
        super(HelpBrowser, self).__init__(parent)

        self.layout = QVBoxLayout()
        self.setWindowTitle('Help Browser')
        self.webkit = QtWebKit.QWebView(None)
        self.layout.addWidget(self.webkit)
        self.setLayout(self.layout)

    def showHelp(self, rstinput):
        data = html_body(unicode(rstinput))
        self.webkit.setHtml(data)
        self.show()

    def helpwnd(self, text, param=None):
        self.showHelp(text)


if __name__ == '__main__':

    app = QApplication(sys.argv)

    wid = QWidget()
    wid.resize(250, 150)
    wid.setWindowTitle('ChipWhisperer Help Browser')
    wid.show()

    test = html_body(unicode("test\n=====\n\nHello There."))

    view = QtWebKit.QWebView(None)
    # view.load(QUrl("http://www.google.com/"))
    view.setHtml(test)

    layoutmain = QHBoxLayout()
    layoutmain.addWidget(view)

    wid.setLayout(layoutmain)

    sys.exit(app.exec_())




