// Copyright (C) 2011-2023  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef _GenericDialog_HXX
#define _GenericDialog_HXX

#include <QDialog>

#include "ui_GenericDialog.h"

class QFrame;
class QDialogButtonBox;

class GenericDialog : public QDialog
{
  Q_OBJECT

public:
  GenericDialog(QDialog* parent = 0);

protected:
  QFrame* getPanel();
  QDialogButtonBox * getButtonBox();

protected slots:    
  void accept();

private:
  Ui_GenericDialog ui; // instance of the class defined in ui_GenericDialog.h
};


#endif // _GenericDialog_HXX
