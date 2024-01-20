// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

#ifndef OCCVIEWER_AXIALSCALEDLG_H
#define OCCVIEWER_AXIALSCALEDLG_H

#include "OCCViewer.h"

#include <QDialog>

class OCCViewer_ViewWindow;
class QtxDoubleSpinBox;
class QPushButton;
class OCCViewer_Viewer;

class OCCVIEWER_EXPORT OCCViewer_AxialScaleDlg : public QDialog
{
  Q_OBJECT

public:
  OCCViewer_AxialScaleDlg( OCCViewer_ViewWindow* , OCCViewer_Viewer* );
  ~OCCViewer_AxialScaleDlg();

  void Update();

protected slots:
  bool apply();
  void reset();
  void accept();

private:
  OCCViewer_ViewWindow* myView;
  QtxDoubleSpinBox*     m_sbXcoeff;
  QtxDoubleSpinBox*     m_sbYcoeff;
  QtxDoubleSpinBox*     m_sbZcoeff;
  QPushButton*          m_bReset;
  OCCViewer_Viewer*     myModel;
};

#endif // OCCVIEWER_AXIALSCALEDLG_H
