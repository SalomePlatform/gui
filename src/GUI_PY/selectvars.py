#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import Qt

import salome
from salome.kernel.studyedit import getStudyEditor
from salome.kernel import varlist

# ---------------------------------- #
# Dialog box for variables selection #
# ---------------------------------- #

from SelectVarsDialog import Ui_SelectVarsDialog

class MySelectVarsDialog(Ui_SelectVarsDialog, QtGui.QDialog):

    def __init__(self, parent = None, modal = 0):
        QtGui.QDialog.__init__(self, parent)
        Ui_SelectVarsDialog.__init__(self)
        self.setupUi(self)
        self.connect(self.cancelButton, QtCore.SIGNAL("clicked()"), self.close)
        self.connect(self.OKButton, QtCore.SIGNAL("clicked()"), self.accept)
        self.connect(self.selectButton, QtCore.SIGNAL("clicked()"), self.initPotentialVariablesFromSelection)
        self.connect(self.addInputVarButton, QtCore.SIGNAL("clicked()"), self.addSelectedInputVar)
        self.connect(self.removeInputVarButton, QtCore.SIGNAL("clicked()"), self.removeSelectedInputVar)
        self.connect(self.newInputVarButton, QtCore.SIGNAL("clicked()"), self.newInputVar)
        self.connect(self.addOutputVarButton, QtCore.SIGNAL("clicked()"), self.addSelectedOutputVar)
        self.connect(self.removeOutputVarButton, QtCore.SIGNAL("clicked()"), self.removeSelectedOutputVar)
        self.connect(self.newOutputVarButton, QtCore.SIGNAL("clicked()"), self.newOutputVar)
        self.refEntry = None

    def setExchangeVariables(self, exchangeVariables):
        if exchangeVariables.refEntry is not None:
            self._initPotentialVariables(exchangeVariables.refEntry)
        self.selectedInputVarListWidget.addItems([x.name for x in exchangeVariables.inputVarList])
        self.selectedOutputVarListWidget.addItems([x.name for x in exchangeVariables.outputVarList])

    def initPotentialVariablesFromSelection(self):
        entries = salome.sg.getAllSelected()
        if len(entries) != 1 :
            QtGui.QMessageBox.warning(self, self.tr("Error"),
                                      self.tr("One item must be selected in the object browser"))
            return
        selectedEntry = entries[0]
        self._initPotentialVariables(selectedEntry)

    def _initPotentialVariables(self, entry):
        sobj = getStudyEditor().study.FindObjectID(entry)
        if sobj is None:
            QtGui.QMessageBox.warning(self, self.tr("Error"),
                                      self.tr('No item at entry %s' % entry))
            return
        exchangeVariables = varlist.getExchangeVariablesFromSObject(sobj)
        if exchangeVariables is None:
            QtGui.QMessageBox.warning(self, self.tr("Error"),
                                      self.tr('Item at entry %s is not a valid '
                                              '"Variable List" object' % entry))
            return
        self.refEntry = entry
        self.varListObjLineEdit.setText(sobj.GetName())
        self.allInputVarListWidget.clear()
        self.allOutputVarListWidget.clear()
        self.allInputVarListWidget.addItems([x.name for x in exchangeVariables.inputVarList])
        self.allOutputVarListWidget.addItems([x.name for x in exchangeVariables.outputVarList])

    def addSelectedInputVar(self):
        for item in self.allInputVarListWidget.selectedItems():
            self.selectedInputVarListWidget.addItem(QtGui.QListWidgetItem(item))

    def removeSelectedInputVar(self):
        for item in self.selectedInputVarListWidget.selectedItems():
            self.selectedInputVarListWidget.takeItem(self.selectedInputVarListWidget.row(item))

    def newInputVar(self):
        newItem = QtGui.QListWidgetItem("TO EDIT!")
        newItem.setFlags(Qt.ItemIsSelectable|Qt.ItemIsEditable|Qt.ItemIsUserCheckable|Qt.ItemIsEnabled)
        self.selectedInputVarListWidget.addItem(newItem);

    def addSelectedOutputVar(self):
        for item in self.allOutputVarListWidget.selectedItems():
            self.selectedOutputVarListWidget.addItem(QtGui.QListWidgetItem(item))

    def removeSelectedOutputVar(self):
        for item in self.selectedOutputVarListWidget.selectedItems():
            self.selectedOutputVarListWidget.takeItem(self.selectedOutputVarListWidget.row(item))

    def newOutputVar(self):
        newItem = QtGui.QListWidgetItem("TO EDIT!")
        newItem.setFlags(Qt.ItemIsSelectable|Qt.ItemIsEditable|Qt.ItemIsUserCheckable|Qt.ItemIsEnabled)
        self.selectedOutputVarListWidget.addItem(newItem);

    def getSelectedExchangeVariables(self):
        inputVarList = []
        outputVarList = []
        for row in range(self.selectedInputVarListWidget.count()):
            inputVarList.append(varlist.Variable(str(self.selectedInputVarListWidget.item(row).text())))
        for row in range(self.selectedOutputVarListWidget.count()):
            outputVarList.append(varlist.Variable(str(self.selectedOutputVarListWidget.item(row).text())))
        return varlist.ExchangeVariables(inputVarList, outputVarList, self.refEntry)