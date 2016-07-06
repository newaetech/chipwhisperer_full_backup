from PySide.QtGui import *
import logging

""" Copied from: http://stackoverflow.com/questions/28655198/best-way-to-display-logs-in-pyqt """

class QPlainTextEditLogger(logging.Handler):
    def __init__(self, parent):
        super(QPlainTextEditLogger, self).__init__()
        self.widget = QPlainTextEdit(parent)
        self.widget.setReadOnly(True)

    def emit(self, record):
        msg = self.format(record)
        self.append(msg)

    def write(self, text):
        self.append(text)

    def append(self, text):
        self.widget.moveCursor(QTextCursor.End)
        self.widget.insertPlainText(text)


class LoggingWidget(QWidget, QPlainTextEditLogger):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)

        self.editor = QPlainTextEditLogger(self)
        logging.getLogger().addHandler(self.editor)

        self._level = QComboBox()
        self._levels = ['CRITICAL', 'ERROR', 'WARNING', 'INFO', 'DEBUG']
        [self._level.addItem(item) for item in self._levels]

        self._format = QComboBox()
        self._formats = {'DATA-TIME-MESSAGE': '%(asctime)s - %(levelname)s - %(message)s\n',
                         'MESSAGE': '%(levelname)s - %(message)s\n'}
        [self._format.addItem(item) for item in self._formats]

        self._clearBtn = QPushButton("Clear")

        # Add the new logging box widget to the layout
        layout = QVBoxLayout()
        layoutH = QHBoxLayout()

        layout.addWidget(self.editor.widget)
        layout.addLayout(layoutH)
        layoutH.addWidget(QLabel('Level: '))
        layoutH.addWidget(self._level)
        layoutH.addWidget(QLabel('Format: '))
        layoutH.addWidget(self._format)
        layoutH.addWidget(self._clearBtn)
        layoutH.addStretch()
        self.setLayout(layout)

        # Connect signal to slot
        self._level.activated[str].connect(self.setLevel)
        self._format.activated[str].connect(self.setFormat)
        self._clearBtn.clicked.connect(self.editor.widget.clear)

        # You can control the logging level
        self._level.setCurrentIndex(self._level.findText('INFO'))
        self.setLevel('INFO')
        self._format.setCurrentIndex(self._format.findText('MESSAGE'))
        self.setFormat('MESSAGE')

    def setFormat(self, format):
        self.editor.setFormatter(logging.Formatter(self._formats[format]))

    def setLevel(self, levelName):
        logging.getLogger().setLevel(logging._levelNames[levelName])

    def __del__(self):
        logging.getLogger().removeHandler(self.editor)