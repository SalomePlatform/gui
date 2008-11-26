//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef OBJECT_BROWSER_FIND_DIALOG_HEADER
#define OBJECT_BROWSER_FIND_DIALOG_HEADER

#include <qgroupbox.h>

class OB_ObjSearch;
class QLineEdit;
class QPushButton;
class QCheckBox;

class OB_FindDlg : public QGroupBox
{
  Q_OBJECT

public:
  OB_FindDlg( QWidget* = 0 );
  virtual ~OB_FindDlg();

  OB_ObjSearch* getSearch() const;
  void setSearch( OB_ObjSearch* );

private slots:
  void onFind();
  void onClose();

private:
  QLineEdit* myData;
  QPushButton *myToFirst, *myToLast, *myNext, *myPrev, *myClose;
  QCheckBox *myIsCaseSens, *myIsRegExp;
  OB_ObjSearch* mySearch;
};

#endif
