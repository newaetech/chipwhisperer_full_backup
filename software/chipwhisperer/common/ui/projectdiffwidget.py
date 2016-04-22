from pyqtgraph.parametertree import Parameter, ParameterTree
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter
from PySide.QtGui import *

class ProjectDiffWidget(QWidget):
    """Widget that displays differences between versions of the project file"""

    def __init__(self, parent=None, project=None):
        super(ProjectDiffWidget, self).__init__(parent)
        self._project = project

        hlayout = QHBoxLayout()

        self.changedTree = ParameterTree()
        self.addedTree = ParameterTree()
        self.deletedTree = ParameterTree()

        self.updateParamTree(self.changedTree, [], "Changed Sections")
        self.updateParamTree(self.addedTree, [], "Added Sections")
        self.updateParamTree(self.deletedTree, [], "Removed Sections")

        hlayout.addWidget(self.changedTree)
        hlayout.addWidget(self.addedTree)
        hlayout.addWidget(self.deletedTree)

        self.setLayout(hlayout)
        self.checkDiff()

    def updateParamTree(self, paramTree, changelist, name):
        paramlist = []
        for k in changelist:
            paramlist.append({'name':k})
        params = Parameter.create(name=name, type='group', children=paramlist)
        ExtendedParameter.reloadParams([params], paramTree)

    def checkDiff(self, ignored=None, updateGUI=False):
        """
        Check if there is a difference - returns True if so, and False
        if no changes present. Also updates widget with overview of the
        differences if requested with updateGUI
        """
        added, removed, changed = self._project.checkDiff()
        if updateGUI:
            self.updateParamTree(self.changedTree, changed, "Changed Sections")
            self.updateParamTree(self.addedTree, added, "Added Sections (not on disk)")
            self.updateParamTree(self.deletedTree, removed, "Removed Sections (on disk)")