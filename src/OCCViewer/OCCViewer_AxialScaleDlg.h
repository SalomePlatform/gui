// Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
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

#include <qdialog.h>

class OCCViewer_ViewWindow;

class QtxDblSpinBox;
class QPushButton;

/*!
  \class OCCViewer_AxialScaleDlg
  \brief Dialog allowing to assign parameters of axes scaling
*/
class OCCViewer_AxialScaleDlg : public QDialog
{
    Q_OBJECT
    
    public:
    OCCViewer_AxialScaleDlg(OCCViewer_ViewWindow* , QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~OCCViewer_AxialScaleDlg();

    void Update();

protected:
    OCCViewer_ViewWindow *myView;
    
    QtxDblSpinBox* m_sbXcoeff;
    QtxDblSpinBox* m_sbYcoeff;
    QtxDblSpinBox* m_sbZcoeff;
    QPushButton* m_bReset;

protected slots:
    void onClickApply();
    void onClickReset();
    void onClickOk();
    void onClickClose();

    void onViewHide();
};

#endif // OCCVIEWER_AXIALSCALEDLG_H
