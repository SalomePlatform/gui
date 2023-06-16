#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2023  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# File   : test_table.py
# Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
#
import salome
import math
import SALOMEDS

# >>> Getting study builder ==================================================
myStudy = salome.myStudy
myBuilder = myStudy.NewBuilder()

# >>> Creating virtual component =============================================
myComponent = myStudy.FindComponent("VirtualComponent")
if not myComponent:
   myComponent = myBuilder.NewComponent("VirtualComponent")
   aName = myBuilder.FindOrCreateAttribute(myComponent, "AttributeName")
   aName.SetValue("VirtualComponent")

# >>> Creating object with Table of integer ==================================
myTIntObject = myBuilder.NewObject(myComponent)
AName = myBuilder.FindOrCreateAttribute(myTIntObject, "AttributeName")
AName.SetValue("Table Of Integer")
AIntTable = myBuilder.FindOrCreateAttribute(myTIntObject, "AttributeTableOfInteger")

a=[1,2,3,4,5,6,7,8,9,10]
AIntTable.AddRow(a)
a=[110,120,130,140,150,160,170,180,190,200]
AIntTable.AddRow(a)
a=[-1,272,0,0,-642,10000,13,578,-578,99]
AIntTable.AddRow(a)
AIntTable.SetTitle("TEST table of integer")
AIntTable.SetRowTitle(1,"FR")
AIntTable.SetRowUnit(1,"m/h")
AIntTable.SetRowTitle(2,"SR")
AIntTable.SetRowUnit(2,"s")
AIntTable.SetRowTitle(3,"TR")
AIntTable.SetRowUnit(3,"$")
c=["C1","C2","C3","C4","C5","C6","C7","C8","C9","C10"]
AIntTable.SetColumnTitles(c)

# >>> Creating object with Table of real =====================================
myTRealObject = myBuilder.NewObject(myComponent)
AName = myBuilder.FindOrCreateAttribute(myTRealObject, "AttributeName")
AName.SetValue("Table Of Real")
ARealTable = myBuilder.FindOrCreateAttribute(myTRealObject, "AttributeTableOfReal")

k={}
l={}
for j in range(0,20):
   k[j] = j*10+1
   l[j] = "C"+str(j+1)
ARealTable.AddRow(list(k.values()))
ARealTable.SetRowTitle(1, "Row 0")
ARealTable.SetRowUnit(1, "Hz")
ARealTable.SetColumnTitles(list(l.values()))
for i in range(1,11):
   for j in range(1,21):
      if j % 2 == 1:
         k[j] = math.log10(j*30*math.pi/180) * 20 + i * 15 + j*5
      else:
         k[j] = math.sin(j*30*math.pi/180) * 20 + i * 15 + j*5 
   ARealTable.AddRow(list(k.values()))
   ARealTable.SetRowTitle(i+1, "Row " + str(i))
   ARealTable.SetRowUnit(i+1, "Wt")
ARealTable.SetTitle("TEST table of real")

# >>> Creating object with integer attribute =================================
myIntObject = myBuilder.NewObject(myComponent)
AName = myBuilder.FindOrCreateAttribute(myIntObject, "AttributeName")
AName.SetValue("Integer")
AInt = myBuilder.FindOrCreateAttribute(myIntObject, "AttributeInteger")
AInt.SetValue(123)

# >>> Creating object with real attribute ====================================
myRealObject = myBuilder.NewObject(myComponent)
AName = myBuilder.FindOrCreateAttribute(myRealObject, "AttributeName")
AName.SetValue("Real")
AReal = myBuilder.FindOrCreateAttribute(myRealObject, "AttributeReal")
AReal.SetValue(-56.9634)

# >>> Creating object with comment attribute =================================
myCmtObject = myBuilder.NewObject(myComponent)
AName = myBuilder.FindOrCreateAttribute(myCmtObject, "AttributeName")
AName.SetValue("Comment")
ACmt = myBuilder.FindOrCreateAttribute(myCmtObject, "AttributeComment")
ACmt.SetValue("Just a comment")

# >>> Updating Object Browser ================================================
salome.sg.updateObjBrowser()

# ============================================================================



