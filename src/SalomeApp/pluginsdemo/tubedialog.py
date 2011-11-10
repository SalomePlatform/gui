# -*- coding: iso-8859-1 -*-
import sys
from PyQt4 import QtGui
from PyQt4 import QtCore


class TubeDialog(QtGui.QDialog):
    def __init__(self, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.setupUi()

    def setupUi(self):
        self.setObjectName("Dialog")
        self.resize(400, 300)
        self.hboxlayout = QtGui.QHBoxLayout(self)
        self.hboxlayout.setMargin(9)
        self.hboxlayout.setSpacing(6)
        self.hboxlayout.setObjectName("hboxlayout")
        self.vboxlayout = QtGui.QVBoxLayout()
        self.vboxlayout.setMargin(0)
        self.vboxlayout.setSpacing(6)
        self.vboxlayout.setObjectName("vboxlayout")
        self.hboxlayout1 = QtGui.QHBoxLayout()
        self.hboxlayout1.setMargin(0)
        self.hboxlayout1.setSpacing(6)
        self.hboxlayout1.setObjectName("hboxlayout1")
        self.vboxlayout1 = QtGui.QVBoxLayout()
        self.vboxlayout1.setMargin(0)
        self.vboxlayout1.setSpacing(6)
        self.vboxlayout1.setObjectName("vboxlayout1")
        self.lblRadius = QtGui.QLabel(self)
        self.lblRadius.setObjectName("lblRadius")
        self.vboxlayout1.addWidget(self.lblRadius)
        self.lblLength = QtGui.QLabel(self)
        self.lblLength.setObjectName("lblLength")
        self.vboxlayout1.addWidget(self.lblLength)
        self.lblWidth = QtGui.QLabel(self)
        self.lblWidth.setObjectName("lblWidth")
        self.vboxlayout1.addWidget(self.lblWidth)
        self.hboxlayout1.addLayout(self.vboxlayout1)
        self.vboxlayout2 = QtGui.QVBoxLayout()
        self.vboxlayout2.setMargin(0)
        self.vboxlayout2.setSpacing(6)
        self.vboxlayout2.setObjectName("vboxlayout2")
        self.txtRadius = QtGui.QLineEdit(self)
        self.txtRadius.setObjectName("txtRadius")
        self.vboxlayout2.addWidget(self.txtRadius)
        self.txtLength = QtGui.QLineEdit(self)
        self.txtLength.setObjectName("txtLength")
        self.vboxlayout2.addWidget(self.txtLength)
        self.txtWidth = QtGui.QLineEdit(self)
        self.txtWidth.setObjectName("txtWidth")
        self.vboxlayout2.addWidget(self.txtWidth)
        self.hboxlayout1.addLayout(self.vboxlayout2)
        self.vboxlayout.addLayout(self.hboxlayout1)
        spacerItem = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.vboxlayout.addItem(spacerItem)
        self.buttonBox = QtGui.QDialogButtonBox(self)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.NoButton|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.vboxlayout.addWidget(self.buttonBox)
        self.hboxlayout.addLayout(self.vboxlayout)

        self.setWindowTitle("Tube construction")
        self.lblRadius.setText("Rayon")
        self.lblLength.setText("Longueur")
        self.lblWidth.setText("Epaisseur")

        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), self.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), self.reject)

    def accept(self):
        '''Callback function when dialog is accepted (click Ok)'''
        self._wasOk = True
        QtGui.QDialog.accept(self)

    def reject(self):
        '''Callback function when dialog is rejected (click Cancel)'''
        self._wasOk = False
        QtGui.QDialog.reject(self)

    def wasOk(self):
        return self._wasOk

    def setData(self, radius, length, width):
        self.txtRadius.setText(str(radius))
        self.txtLength.setText(str(length))
        self.txtWidth.setText(str(width))

    def getData(self):
        try:
            radius=eval(str(self.txtRadius.text()))
            length=eval(str(self.txtLength.text()))
            width=eval(str(self.txtWidth.text()))
        except:
            print "pb a la saisie"

        return radius, length, width


def TEST_getData():
    # This use case illustrates the MVC pattern on this simple dialog example.
    a = QtGui.QApplication(sys.argv)

    tubedialog = TubeDialog()
    tubedialog.setData(10,50,3)
    tubedialog.exec_()
    if tubedialog.wasOk():
        radius, length, width = tubedialog.getData()
        print radius, length, width

    sys.exit(0)

def main( args ):
    a = QtGui.QApplication(sys.argv)

    tubedialog = TubeDialog()
    tubedialog.setData(10,50,3)
    sys.exit(tubedialog.exec_())

if __name__=="__main__":
    #main(sys.argv)
    TEST_getData()

