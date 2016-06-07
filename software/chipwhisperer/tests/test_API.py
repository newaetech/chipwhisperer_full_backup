import sys
from unittest import TestCase
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from PySide.QtCore import *

class TestTraceManager(TestCase):
    def setUp(self):
        self.app = QCoreApplication(sys.argv)

    def tearDown(self):
        self.app = None

    def test_connectNone(self):
        t = CWCoreAPI()
        self.assertEqual(t.connectTarget, True)
        self.assertEqual(t.getTarget(), None)
        self.assertEqual(t.getScope(), None)
        self.assertEqual(t.capture1(), True)
        self.assertEqual(t.captureM(), True)
        sys.exit(self.app.exec_())


