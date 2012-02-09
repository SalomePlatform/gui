import sys
from PyQt4 import QtGui
from PyQt4 import QtCore


from tubedialog import TubeDialog
class TubeDialogWithApply(TubeDialog):
    def setupUi(self):
        """
        This setupUi adds a button Apply to execute tasks as preview 
        """
        TubeDialog.setupUi(self)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|
                                          QtGui.QDialogButtonBox.Apply|
                                          QtGui.QDialogButtonBox.Ok)

        self.setWindowFlags(self.windowFlags() |
                            QtCore.Qt.WindowStaysOnTopHint)


    def handleAcceptWith(self,callbackFunction):
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), callbackFunction)

    def handleRejectWith(self,callbackFunction):
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), callbackFunction)

    def handleApplyWith(self,callbackFunction):
        button = self.buttonBox.button(QtGui.QDialogButtonBox.Apply)        
        QtCore.QObject.connect(button, QtCore.SIGNAL("clicked()"), callbackFunction);

