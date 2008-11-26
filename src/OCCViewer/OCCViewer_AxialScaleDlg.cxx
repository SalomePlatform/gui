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
#include "OCCViewer_AxialScaleDlg.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewPort3d.h"

#include <Standard_Real.hxx>

#include "QtxDblSpinBox.h"

#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>

/*!
  Constructor
  \param view - view window
  \param parent - parent widget
  \param name - dialog name
  \param modal - is this dialog modal
  \param fl - flags
*/
OCCViewer_AxialScaleDlg::OCCViewer_AxialScaleDlg( OCCViewer_ViewWindow* view, QWidget* parent, const char* name, bool modal, WFlags fl )
: QDialog( parent, "OCCViewer_AxialScaleDlg", modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu ),
  myView( view )
{
  setCaption( tr( "DLG_SCALING" ) );
  
  setSizeGripEnabled(TRUE);

  // Create layout for this dialog
  QGridLayout* layoutDlg = new QGridLayout (this);
  layoutDlg->setSpacing(6);
  layoutDlg->setMargin(11);

  // Create croup box with grid layout
  QGroupBox* aGroupBox = new QGroupBox(this, "GroupBox");
  QHBoxLayout* aHBoxLayout = new QHBoxLayout(aGroupBox);
  aHBoxLayout->setMargin(11);
  aHBoxLayout->setSpacing(6);

  // "X" scaling
  QLabel* TextLabelX = new QLabel (tr("LBL_X"), aGroupBox, "TextLabelX");
  TextLabelX->setFixedWidth(15);
  m_sbXcoeff = new QtxDblSpinBox(1e-7, RealLast(), 0.1, aGroupBox);
  m_sbXcoeff->setMinimumWidth(80);
  m_sbXcoeff->setValue(1.0);

  // "Y" scaling
  QLabel* TextLabelY = new QLabel (tr("LBL_Y"), aGroupBox, "TextLabelY");
  TextLabelY->setFixedWidth(15);
  m_sbYcoeff = new QtxDblSpinBox(1e-7, RealLast(), 0.1, aGroupBox);
  m_sbYcoeff->setMinimumWidth(80);
  m_sbYcoeff->setValue(1.0);

  // "Z" scaling
  QLabel* TextLabelZ = new QLabel (tr("LBL_Z"), aGroupBox, "TextLabelZ");
  TextLabelZ->setFixedWidth(15);
  m_sbZcoeff = new QtxDblSpinBox(1e-7, RealLast(), 0.1, aGroupBox);
  m_sbZcoeff->setMinimumWidth(80);
  m_sbZcoeff->setValue(1.0);

  // Create <Reset> button
  m_bReset = new QPushButton(tr("&Reset"), aGroupBox, "m_bReset");

  // Layout widgets in the group box
  aHBoxLayout->addWidget(TextLabelX);
  aHBoxLayout->addWidget(m_sbXcoeff);
  aHBoxLayout->addWidget(TextLabelY);
  aHBoxLayout->addWidget(m_sbYcoeff);
  aHBoxLayout->addWidget(TextLabelZ);
  aHBoxLayout->addWidget(m_sbZcoeff);
  //aHBoxLayout->addStretch();
  aHBoxLayout->addWidget(m_bReset);

  // OK, CANCEL, Apply button
  QGroupBox* aGroupBox2 = new QGroupBox(this);
  QHBoxLayout* aHBoxLayout2 = new QHBoxLayout(aGroupBox2);
  aHBoxLayout2->setMargin(11);
  aHBoxLayout2->setSpacing(6);
  // Create <OK> button
  QPushButton* m_bOk = new QPushButton(tr("O&K"), aGroupBox2, "m_bOk");
  m_bOk->setDefault(TRUE);
  m_bOk->setAutoDefault(TRUE);
  // Create <Apply> button
  QPushButton* m_bApply = new QPushButton(tr("&Apply"), aGroupBox2, "m_bApply");
  m_bApply->setAutoDefault(TRUE);
  // Create <Cancel> button
  QPushButton* m_bCancel = new QPushButton(tr("&Cancel"), aGroupBox2, "m_bCancel");
  m_bCancel->setAutoDefault(TRUE);

  // Layout buttons
  aHBoxLayout2->addWidget(m_bOk);
  aHBoxLayout2->addWidget(m_bApply);
  aHBoxLayout2->addStretch();
  aHBoxLayout2->addWidget(m_bCancel);

  // Layout top level widgets
  layoutDlg->addWidget(aGroupBox,0,0);
  layoutDlg->addWidget(aGroupBox2,1,0);

  // signals and slots connections
  connect(m_bCancel, SIGNAL(clicked()), this, SLOT(onClickClose()));
  connect(m_bOk,     SIGNAL(clicked()), this, SLOT(onClickOk()));
  connect(m_bApply,  SIGNAL(clicked()), this, SLOT(onClickApply()));
  connect(m_bReset,  SIGNAL(clicked()), this, SLOT(onClickReset()));

  connect(view, SIGNAL(Hide( QHideEvent * )), this, SLOT(onViewHide()));

  this->resize(100, this->sizeHint().height());
}

/*!
  Destructor
  Destroys the object and frees any allocated resources
*/
OCCViewer_AxialScaleDlg::~OCCViewer_AxialScaleDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void 
OCCViewer_AxialScaleDlg
::Update()
{
  // Get values from the OCC view
  Standard_Real aScaleFactor[3];
  myView->getViewPort()->getView()->AxialScale(aScaleFactor[0],aScaleFactor[1],aScaleFactor[2]);
  m_sbXcoeff->setValue(aScaleFactor[0]);
  m_sbYcoeff->setValue(aScaleFactor[1]);
  m_sbZcoeff->setValue(aScaleFactor[2]);
}

void 
OCCViewer_AxialScaleDlg
::onClickOk()
{
  //apply changes
  onClickApply();
  //Close dialog
  accept();
}

void
OCCViewer_AxialScaleDlg
::onClickApply()
{
  double aScaleFactor[3] = {m_sbXcoeff->value(), m_sbYcoeff->value(), m_sbZcoeff->value()};
  myView->getViewPort()->getView()->SetAxialScale(aScaleFactor[0],aScaleFactor[1],aScaleFactor[2]);
}

void
OCCViewer_AxialScaleDlg
::onClickReset()
{
  m_bReset->setFocus();
  m_sbXcoeff->setValue(1.0);
  m_sbYcoeff->setValue(1.0);
  m_sbZcoeff->setValue(1.0);
}

void 
OCCViewer_AxialScaleDlg
::onClickClose()
{
  reject();
}

void OCCViewer_AxialScaleDlg::onViewHide()
{
  hide();
}

