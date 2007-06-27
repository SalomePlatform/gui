// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
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
//  SALOME Plot2d : implementation of desktop and GUI kernel
//
//  File   : Plot2d_SetupCurveDlg.cxx
//  Author : Vadim SANDLER
//  Module : SALOME
//  $Header$

#include "Plot2d_SetupCurveDlg.h"
#include "SUIT_Tools.h"
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qgroupbox.h>
#include <qcolordialog.h>

#ifndef WIN32
using namespace std;
#endif

#define MARGIN_SIZE      11
#define SPACING_SIZE     6
#define MIN_COMBO_WIDTH  100
#define MIN_SPIN_WIDTH   50
#define MAX_LINE_WIDTH   100

/*!
  Constructor
*/
Plot2d_SetupCurveDlg::Plot2d_SetupCurveDlg( QWidget* parent )
     : QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint )
{
  setObjectName( "Plot2d_SetupCurveDlg" );
  setModal( true );
  setWindowTitle( tr("TLT_SETUP_CURVE") );
  setSizeGripEnabled( TRUE );
  QGridLayout* topLayout = new QGridLayout( this ); 
  topLayout->setSpacing( SPACING_SIZE );
  topLayout->setMargin( MARGIN_SIZE );

  QGroupBox* TopGroup = new QGroupBox( this );
  QGridLayout* TopGroupLayout = new QGridLayout( TopGroup );
  TopGroup->setLayout( TopGroupLayout );
  TopGroupLayout->setAlignment( Qt::AlignTop );
  TopGroupLayout->setSpacing( SPACING_SIZE ); TopGroupLayout->setMargin( MARGIN_SIZE );

  QLabel* aLineTypeLab = new QLabel( tr( "CURVE_LINE_TYPE_LAB" ), TopGroup );
  myLineCombo = new QComboBox( TopGroup );
  myLineCombo->setEditable( false );
  myLineCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myLineCombo->setMinimumWidth( MIN_COMBO_WIDTH );
  myLineCombo->addItem( tr( "NONE_LINE_LBL" ) );
  myLineCombo->addItem( tr( "SOLID_LINE_LBL" ) );
  myLineCombo->addItem( tr( "DASH_LINE_LBL" ) );
  myLineCombo->addItem( tr( "DOT_LINE_LBL" ) );
  myLineCombo->addItem( tr( "DASHDOT_LINE_LBL" ) );
  myLineCombo->addItem( tr( "DAHSDOTDOT_LINE_LBL" ) );
  myLineCombo->setCurrentIndex( 1 ); // SOLID by default

  QLabel* aLineWidthLab = new QLabel( tr( "CURVE_LINE_WIDTH_LAB" ), TopGroup );
  myLineSpin = new QSpinBox( TopGroup );
  myLineSpin->setMinimum( 0 );
  myLineSpin->setMaximum( MAX_LINE_WIDTH );
  myLineSpin->setSingleStep( 1 );
  myLineSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myLineSpin->setMinimumWidth( MIN_SPIN_WIDTH );
  myLineSpin->setValue( 0 );        // default width is 0

  QLabel* aMarkerLab = new QLabel( tr( "CURVE_MARKER_TYPE_LAB" ), TopGroup );
  myMarkerCombo = new QComboBox( TopGroup );
  myMarkerCombo->setEditable( false );
  myMarkerCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myMarkerCombo->setMinimumWidth( MIN_COMBO_WIDTH );
  myMarkerCombo->addItem( tr( "NONE_MARKER_LBL" ) );
  myMarkerCombo->addItem( tr( "CIRCLE_MARKER_LBL" ) );
  myMarkerCombo->addItem( tr( "RECTANGLE_MARKER_LBL" ) );
  myMarkerCombo->addItem( tr( "DIAMOND_MARKER_LBL" ) );
  myMarkerCombo->addItem( tr( "DTRIANGLE_MARKER_LBL" ) );
  myMarkerCombo->addItem( tr( "UTRIANGLE_MARKER_LBL" ) );
  myMarkerCombo->addItem( tr( "LTRIANGLE_MARKER_LBL" ) );
  myMarkerCombo->addItem( tr( "RTRIANGLE_MARKER_LBL" ) );
  myMarkerCombo->addItem( tr( "CROSS_MARKER_LBL" ) );
  myMarkerCombo->addItem( tr( "XCROSS_MARKER_LBL" ) );
  myMarkerCombo->setCurrentIndex( 1 ); // CIRCLE by default

  QLabel* aColorLab = new QLabel( tr( "CURVE_COLOR_LAB" ), TopGroup );
  myColorBtn = new QToolButton( TopGroup );
  myColorBtn->setMinimumSize(25, 25);

  TopGroupLayout->addWidget( aLineTypeLab, 0, 0 );
  TopGroupLayout->addWidget( myLineCombo, 0, 1, 1, 2 );
  TopGroupLayout->addWidget( aLineWidthLab, 1, 0 );
  TopGroupLayout->addWidget( myLineSpin, 1, 1, 1, 2 );
  TopGroupLayout->addWidget( aMarkerLab, 2, 0 );
  TopGroupLayout->addWidget( myMarkerCombo, 2, 1, 1, 2 );
  TopGroupLayout->addWidget( aColorLab, 3, 0 );
  TopGroupLayout->addWidget( myColorBtn, 3, 1 );
  TopGroupLayout->setColumnStretch( 2, 5 );

  QGroupBox* GroupButtons = new QGroupBox( this );
  QHBoxLayout* GroupButtonsLayout = new QHBoxLayout( GroupButtons );
  GroupButtons->setLayout( GroupButtonsLayout );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setSpacing( SPACING_SIZE ); GroupButtonsLayout->setMargin( MARGIN_SIZE );

  myOkBtn = new QPushButton( tr( "BUT_OK" ), GroupButtons );
  myOkBtn->setAutoDefault( true ); myOkBtn->setDefault( true );
  myCancelBtn = new QPushButton( tr( "BUT_CANCEL" ) , GroupButtons );
  myCancelBtn->setAutoDefault( true );

  GroupButtonsLayout->addWidget( myOkBtn );
  GroupButtonsLayout->addStretch();
  GroupButtonsLayout->addWidget( myCancelBtn );

  connect( myColorBtn,  SIGNAL( clicked() ), this, SLOT( onColorChanged() ) );
  connect( myOkBtn,     SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( myCancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );
  setColor( QColor( 0, 0, 0 ) );

  topLayout->addWidget( TopGroup,     0, 0 );
  topLayout->addWidget( GroupButtons, 1, 0 );

  SUIT_Tools::centerWidget( this, parent );
}
/*!
  Destructor
*/
Plot2d_SetupCurveDlg::~Plot2d_SetupCurveDlg()
{
}
/*!
  Sets line style and width
*/
void Plot2d_SetupCurveDlg::setLine( const int line, const int width )
{
  myLineCombo->setCurrentIndex( line );
  myLineSpin->setValue( width );
}
/*!
  Gets line style
*/
int Plot2d_SetupCurveDlg::getLine() const
{
  return myLineCombo->currentIndex();
}
/*!
  Gets line width
*/
int Plot2d_SetupCurveDlg::getLineWidth() const
{
  return myLineSpin->value();
}
/*!
  Sets marker style
*/
void Plot2d_SetupCurveDlg::setMarker( const int marker )
{
  myMarkerCombo->setCurrentIndex( marker );
}
/*!
  Gets marker style
*/
int Plot2d_SetupCurveDlg::getMarker() const 
{
  return myMarkerCombo->currentIndex();
}
/*!
  Sets color
*/
void Plot2d_SetupCurveDlg::setColor( const QColor& color )
{
  QPalette pal = myColorBtn->palette();
  pal.setColor( QPalette::Active, QPalette::Button, color );
  pal.setColor( QPalette::Inactive, QPalette::Button, color );

  myColorBtn->setPalette( pal );
}
/*!
  Gets color
*/
QColor Plot2d_SetupCurveDlg::getColor() const 
{
  return myColorBtn->palette().color( QPalette::Active, QPalette::Button );
}
/*!
  <Color> button slot, invokes color selection dialog box
*/
void Plot2d_SetupCurveDlg::onColorChanged()
{
  QColor color = QColorDialog::getColor( getColor() );
  if ( color.isValid() ) {
    setColor( color );
  }
}





