//  SALOME VTKViewer : build VTK viewer into Salome desktop
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : 
//  Author : 
//  Module : SALOME
//  $Header$

#include "SVTK_TextRegionDlg.h"

#include "SVTK_CaptionActor2DWidget.h"
#include "SVTK_RenderWindowInteractor.h"

#include "SVTK_MainWindow.h"
#include "SVTK_FontWidget.h"

#include "QtxAction.h"

#include <QGroupBox>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLayout>
#include <QLineEdit>

#include <vtkCaptionActor2D.h>
#include <vtkGenericRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkTextProperty.h>


/*!
  Constructor
*/
SVTK_TextRegionDlg
::SVTK_TextRegionDlg(QtxAction* theAction,
		     SVTK_MainWindow* theParent,
		     const char* theName):
  SVTK_DialogBase(theAction,
		  theParent, 
		  theName),
  myMainWindow( theParent ),
  myPriority(0.0),
  myEventCallbackCommand(vtkCallbackCommand::New())
{
  myEventCallbackCommand->Delete();
  myEventCallbackCommand->SetClientData(this);
  myEventCallbackCommand->SetCallback(SVTK_TextRegionDlg::ProcessEvents);

  setWindowTitle(tr("DLG_TITLE"));
 
  QVBoxLayout* aFormLayout = new QVBoxLayout( this );
  aFormLayout->setSpacing( 5 );
  aFormLayout->setMargin( 5 );
  {
    QGroupBox* aGroupBox = new QGroupBox( this );
    aGroupBox->setTitle( tr( "TEXT_PROPERTIES" ) );

    QVBoxLayout* aGrLayout = new QVBoxLayout( aGroupBox );
    aGrLayout->setSpacing( 6 );
    aGrLayout->setMargin( 11 );
    {
      aGrLayout->setAlignment( Qt::AlignTop );
  
      {
	SVTK_FontWidget* aWidget = new SVTK_FontWidget(aGroupBox);
	aGrLayout->addWidget( aWidget );
	connect(aWidget, SIGNAL(Modified()), SLOT(onAutoApply()));
	myFontWidget = aWidget;
      }

      {
	QTextEdit* aTextEdit = new QTextEdit( aGroupBox );
	aTextEdit->setLineWrapMode( QTextEdit::WidgetWidth );
	aGrLayout->addWidget( aTextEdit );
	connect(aTextEdit, SIGNAL(textChanged()), SLOT(onAutoApply()));
	myTextEdit = aTextEdit;
      }
    }

    aFormLayout->addWidget( aGroupBox );
  }
  {
    QGroupBox* aGroupBox = new QGroupBox( this );
    QVBoxLayout* aGrLayout = new QVBoxLayout( aGroupBox );
    aGrLayout->setSpacing( 6 );
    aGrLayout->setMargin( 11 );

    {
      aGrLayout->setAlignment( Qt::AlignTop );
      {
	QCheckBox* aCheckBox = new QCheckBox( aGroupBox );
	aCheckBox->setText( tr( "AUTO_APPLY" ) );
	aCheckBox->setChecked( TRUE );
	aGrLayout->addWidget( aCheckBox );
	myAutoApplyCheckBox = aCheckBox;
      }
    }

    aFormLayout->addWidget( aGroupBox );
  }
  {
    QGroupBox* aGroupBox = new QGroupBox( this );
    QVBoxLayout* aGrLayout = new QVBoxLayout( aGroupBox );
    aGrLayout->setSpacing( 6 );
    aGrLayout->setMargin( 11 );

    {
      aGrLayout->setAlignment( Qt::AlignTop );

      {
	QPushButton* aPushButton = new QPushButton( aGroupBox );
	aPushButton->setText( tr( "OK" ) );
	aGrLayout->addWidget( aPushButton );
	connect(aPushButton, SIGNAL(clicked()), SLOT(onOk()));
      }
      {
	QPushButton* aPushButton = new QPushButton( aGroupBox );
	aPushButton->setText( tr( "APPLY" ) );
	aGrLayout->addWidget( aPushButton );
	connect(aPushButton, SIGNAL(clicked()), SLOT(onApply()));
      }
      {
	QSpacerItem* aSpacer = new QSpacerItem( 157, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	aGrLayout->addItem( aSpacer );
      }
      {
	QPushButton* aPushButton = new QPushButton( aGroupBox );
	aPushButton->setText( tr( "CLOSE" ) );
	aGrLayout->addWidget( aPushButton );
	connect(aPushButton, SIGNAL(clicked()), SLOT(onClose()));
      }
    }

    aFormLayout->addWidget( aGroupBox );
  }
}

/*!
  Destroys the object and frees any allocated resources
 */
SVTK_TextRegionDlg
::~SVTK_TextRegionDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

/*!
  Processes events
*/
void 
SVTK_TextRegionDlg
::ProcessEvents(vtkObject* theObject, 
		unsigned long theEvent,
		void* theClientData, 
		void* vtkNotUsed(theCallData))
{
  SVTK_TextRegionDlg* self = reinterpret_cast<SVTK_TextRegionDlg*>(theClientData);

  if(theEvent == SVTK_Actor2DWidget::EditEvent){
    SVTK_CaptionActor2DWidget* aWidget = dynamic_cast<SVTK_CaptionActor2DWidget*>(theObject);
    self->Update( aWidget );
  }
}

/*!
  Update
*/
void 
SVTK_TextRegionDlg
::Update(SVTK_CaptionActor2DWidget* theWidget)
{
  if(!theWidget)
    return;

  vtkCaptionActor2D* aCaptionActor2D = theWidget->GetCaptionActor2D();
  if(!aCaptionActor2D)
    return;

  myWidget = NULL; // To avoid call of onApply function

  //myFontWidget->SetData(aCaptionActor2D->GetCaptionTextProperty());
  QColor aTitleColor(255, 255, 255);
  int aTitleFontFamily = VTK_ARIAL;
  bool isTitleBold = false;
  bool isTitleItalic = false;
  bool isTitleShadow = false;

  vtkTextProperty* aTitleProp = aCaptionActor2D->GetCaptionTextProperty();
  if (aTitleProp !=0)
  {
    vtkFloatingPointType c[ 3 ];
    aTitleProp->GetColor(c);
    aTitleColor.setRgb((int)(c[ 0 ] * 255), (int)(c[ 1 ] * 255), (int)(c[ 2 ] * 255));
    aTitleFontFamily = aTitleProp->GetFontFamily();
    isTitleBold = aTitleProp->GetBold() ? true : false;
    isTitleItalic = aTitleProp->GetItalic() ? true : false;
    isTitleShadow = aTitleProp->GetShadow() ? true : false;
  }
  myFontWidget->SetData(aTitleColor, aTitleFontFamily, isTitleBold,
                        isTitleItalic, isTitleShadow);

  myTextEdit->setText(aCaptionActor2D->GetCaption());

  myWidget = theWidget;
  
  show();
}

/*!
  PublishNew
*/
void 
SVTK_TextRegionDlg
::PublishNew(SVTK_RenderWindowInteractor* theInteractor)
{
  SVTK_CaptionActor2DWidget* aWidget = SVTK::PublishCaptionActor2D(theInteractor->GetDevice());

  aWidget->AddObserver( SVTK_Actor2DWidget::EditEvent, 
			myEventCallbackCommand.GetPointer(), 
			myPriority );

  Update(aWidget);
}

/*!
  Verify validity of entry data
*/
bool 
SVTK_TextRegionDlg
::onApply()
{
  if(!myWidget)
    return false;

  vtkCaptionActor2D* aCaptionActor2D = myWidget->GetCaptionActor2D();
  if(!aCaptionActor2D)
    return false;

  QColor aTitleColor(255, 255, 255);
  int aTitleFontFamily = VTK_ARIAL;
  bool isTitleBold = false;
  bool isTitleItalic = false;
  bool isTitleShadow = false;

  //myFontWidget->GetData(aCaptionActor2D->GetCaptionTextProperty());
  vtkTextProperty* aCaptionProp = aCaptionActor2D->GetCaptionTextProperty();
  myFontWidget->GetData( aTitleColor, aTitleFontFamily, isTitleBold,
                         isTitleItalic, isTitleShadow );
  if ( aCaptionProp ) {
    aCaptionProp->SetColor(aTitleColor.red() / 255.,
                          aTitleColor.green() / 255.,
                          aTitleColor.blue() / 255.);
    aCaptionProp->SetFontFamily(aTitleFontFamily);
    aCaptionProp->SetBold(isTitleBold ? 1 : 0);
    aCaptionProp->SetItalic(isTitleItalic ? 1 : 0);
    aCaptionProp->SetShadow(isTitleShadow ? 1 : 0);

    aCaptionActor2D->SetCaptionTextProperty(aCaptionProp);
  }

  aCaptionActor2D->SetCaption(myTextEdit->toPlainText().toLatin1());

  myMainWindow->Repaint();

  return true;
}

/*!
  Verify validity of entry data
*/
bool 
SVTK_TextRegionDlg
::onAutoApply()
{
  if(!myAutoApplyCheckBox->isChecked())
    return false;

  return onApply();
}

/*!
  SLOT called when "Ok" button pressed.
*/
void 
SVTK_TextRegionDlg
::onOk()
{
  done(onApply());
}

/*!
  SLOT: called when "Close" button pressed. Close dialog
*/
void 
SVTK_TextRegionDlg
::onClose()
{
  reject();
}
