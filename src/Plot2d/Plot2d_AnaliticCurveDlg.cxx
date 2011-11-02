// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File   : Plot2d_AnaliticCurveDlg.cxx
// Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)

//Local includes
#include "Plot2d_AnaliticCurveDlg.h"
#include "Plot2d_AnaliticCurve.h"
#include "Plot2d_ViewFrame.h"

//Qtx includes
#include <QtxIntSpinBox.h>
#include <QtxColorButton.h>

//SUIT includes
#include <SUIT_MessageBox.h>

//Qt includes
#include <QListWidget>
#include <QGroupBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>

//qwt includes
#include <qwt_plot.h>
#include <qwt_scale_div.h>


//debug 
#include <iostream>

// Controls
#define MIN  0
#define MAX  10000
#define STEP 1
#define MAX_LINE_WIDTH 10


/*!
  Constructor 
*/
Plot2d_AnaliticCurveDlg::Plot2d_AnaliticCurveDlg( QWidget* parent, QwtPlot* plot )
  : QDialog( parent , Qt::WindowTitleHint | Qt::WindowSystemMenuHint ),
    myPlot(plot)
{
  setWindowTitle( tr( "ANALITIC_CURVE_TLT" ) );
  QGridLayout* mainLayout = new QGridLayout(this);

  //Curves list widget
  myCurvesList = new QListWidget( this );
  myCurvesList->setSelectionMode(QAbstractItemView::SingleSelection);
  myCurvesList->setMaximumSize(QSize(150, 16777215));

  //Curve parameters group box
  myCurveParams =  new QGroupBox( tr( "AC_CURVE_PARAMS" ), this );
  QGridLayout* paramsLayout = new QGridLayout( myCurveParams );

  QLabel* formulaLabel = new QLabel( tr( "AC_FORMULA" ), myCurveParams );
  myFormula = new QLineEdit( myCurveParams );

  QLabel* nbIntervalsLabel = new QLabel( tr( "AC_NB_INTERVALS" ), myCurveParams );
  myNbIntervals = new QtxIntSpinBox( 1, MAX, STEP, myCurveParams );

  paramsLayout->addWidget(  formulaLabel, 0, 0, 1, 1 );
  paramsLayout->addWidget(     myFormula, 0, 1, 1, 1 );
  paramsLayout->addWidget( nbIntervalsLabel, 1, 0, 1, 1 );
  paramsLayout->addWidget(    myNbIntervals, 1, 1, 1, 1 );

  //Curve properties group box
  myCurveProps =  new QGroupBox( tr( "AC_CURVE_PROPS" ), this );
  QGridLayout* propsLayout = new QGridLayout( myCurveProps );

  myAutoAssign = new QCheckBox( tr( "AC_AUTO_ASSIGN" ), myCurveProps );

  QLabel* markerLabel = new QLabel( tr( "AC_MARKER_TYPE" ), myCurveProps );  
  myMarkerType = new QComboBox( myCurveProps );


  QLabel* lineTypeLabel = new QLabel( tr( "AC_LINE_TYPE" ), myCurveProps );  
  myLineType = new QComboBox( myCurveProps );


  QLabel* lineWidthLabel = new QLabel( tr( "AC_LINE_WIDTH" ), myCurveProps );  
  myLineWidth =  new QtxIntSpinBox( MIN, MAX_LINE_WIDTH, STEP, myCurveProps );



  QLabel* colorLabel = new QLabel( tr("AC_CURVE_COLOR"), myCurveProps );
  myColor  = new QtxColorButton(myCurveProps);

  propsLayout->addWidget(   myAutoAssign, 0, 0, 1, 1 );
  propsLayout->addWidget(    markerLabel, 1, 0, 1, 1 );
  propsLayout->addWidget(   myMarkerType, 1, 1, 1, 1 );
  propsLayout->addWidget(  lineTypeLabel, 2, 0, 1, 1 );
  propsLayout->addWidget(     myLineType, 2, 1, 1, 1 );
  propsLayout->addWidget( lineWidthLabel, 3, 0, 1, 1 );
  propsLayout->addWidget(    myLineWidth, 3, 1, 1, 1 );
  propsLayout->addWidget(     colorLabel, 4, 0, 1, 1 );
  propsLayout->addWidget(        myColor, 4, 1, 1, 1 );
  
  propsLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ), 3, 0, 1, 1 );
  propsLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ), 3, 1, 1, 1 );


  //Add && Remove buttons
  QPushButton* addButton    = new QPushButton( tr("AC_ADD_BTN"), this );
  QPushButton* updateButton = new QPushButton( tr("AC_UPD_BTN"), this );
  QPushButton* removeButton = new QPushButton( tr("AC_REM_BTN"), this );

  //Ok && Close buttons
  QFrame* frame = new QFrame( this );
  QHBoxLayout* horizontalLayout = new QHBoxLayout(frame);
  
  frame->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum ) );
  frame->setFrameShape( QFrame::StyledPanel );
  frame->setFrameShadow( QFrame::Raised );
  
  QPushButton* closeButton = new QPushButton( tr( "AC_CLOSE_BTN" ), frame );
  

  horizontalLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  horizontalLayout->addWidget( closeButton );
  horizontalLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );


  mainLayout->addWidget(  myCurvesList, 0, 0, 3, 1 );
  mainLayout->addWidget( myCurveParams, 0, 1, 1, 5 );
  mainLayout->addWidget(  myCurveProps, 1, 1, 1, 5 );
  mainLayout->addWidget(     addButton, 2, 1, 1, 1 );
  mainLayout->addItem( new QSpacerItem( 13, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ), 2, 2, 1, 1 );
  mainLayout->addWidget(  removeButton, 2, 3, 1, 1 );
  mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 2, 4, 1, 1);
  mainLayout->addWidget(  updateButton, 2, 5, 1, 1);  
  mainLayout->addWidget(frame, 3, 0, 1, 6);

  //Fill combobox
  QColor cl = myMarkerType->palette().color( QPalette::Text );
  QSize sz(16, 16);
  myMarkerType->setIconSize(sz);

  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::None ),      tr( "NONE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::Circle ),    tr( "CIRCLE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::Rectangle ), tr( "RECTANGLE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::Diamond ),   tr( "DIAMOND_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::DTriangle ), tr( "DTRIANGLE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::UTriangle ), tr( "UTRIANGLE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::LTriangle ), tr( "LTRIANGLE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::RTriangle ), tr( "RTRIANGLE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::Cross ),     tr( "CROSS_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::XCross ),    tr( "XCROSS_MARKER_LBL" ) );

  cl = myLineType->palette().color( QPalette::Text );
  QSize lsz( 40, 16 );
  myLineType->setIconSize( lsz );

  myLineType->addItem( Plot2d::lineIcon( lsz, cl, Plot2d::NoPen ),      tr( "NONE_LINE_LBL" ) );
  myLineType->addItem( Plot2d::lineIcon( lsz, cl, Plot2d::Solid ),      tr( "SOLID_LINE_LBL" ) );
  myLineType->addItem( Plot2d::lineIcon( lsz, cl, Plot2d::Dash ),       tr( "DASH_LINE_LBL" ) );
  myLineType->addItem( Plot2d::lineIcon( lsz, cl, Plot2d::Dot ),        tr( "DOT_LINE_LBL" ) );
  myLineType->addItem( Plot2d::lineIcon( lsz, cl, Plot2d::DashDot ),    tr( "DASHDOT_LINE_LBL" ) );
  myLineType->addItem( Plot2d::lineIcon( lsz, cl, Plot2d::DashDotDot ), tr( "DAHSDOTDOT_LINE_LBL" ) );

  //Connections
  connect( closeButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( addButton, SIGNAL( clicked() ), this, SLOT( onAddCurve() ) );
  connect( myAutoAssign,   SIGNAL( stateChanged(int) ), this, SLOT( onAutoAssign(int) ) );
  connect( removeButton, SIGNAL( clicked()), this, SLOT(onRemoveCurve()));
  connect( updateButton, SIGNAL( clicked()), this, SLOT(onUpdateCurve()));
  connectSelectionChanged();

  //Default values
  myNbIntervals->setValue(100);
  checkState();
}

/*!
  Destructor
*/
Plot2d_AnaliticCurveDlg::~Plot2d_AnaliticCurveDlg()
{
}

void Plot2d_AnaliticCurveDlg::setCurveList( AnaliticCurveList& theList ) {
  AnaliticCurveList::iterator it = theList.begin();
  QListWidgetItem* newItem = 0;
  Plot2d_AnaliticCurve* curve;
  for( ;it != theList.end(); it++ ) {
    curve = *it;
    if(!curve) continue;
    newItem = new QListWidgetItem(curve->getName());
    newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    newItem->setCheckState(Qt::Checked);
    myCurvesList->addItem(newItem);
    newItem->setCheckState(curve->isActive() ? Qt::Checked : Qt::Unchecked );
    QVariant var;
    var.setValue((void*)curve);
    newItem->setData(Qt::UserRole,var);
  }
  setCurrentCurve(newItem);
  checkState();
}

/*!
  Return list of the curves.
*/
/*void Plot2d_AnaliticCurveDlg::reject() {
  if( myCurvesList->count() == 0 )
    QDialog::reject();

  QList<QListWidgetItem*> items = myCurvesList->selectedItems();
  QListWidgetItem* item = items.size() > 0 ? items[0] : 0;
  if(item) {
    if(!checkItem(item)) {
      showErrorMsg();
      return;
    } else {
      updateInView(myCurrentItem);
      QDialog::reject();
    }
  }
}
*/

/*!
  Update curve parameters and curve properties.
*/
void Plot2d_AnaliticCurveDlg::setCurrentCurve(QListWidgetItem* item, bool select) {
  if(item) {
    QVariant var = item->data(Qt::UserRole);
    Plot2d_AnaliticCurve* curve = (Plot2d_AnaliticCurve*)var.value<void*>();
    if(curve) {
      myFormula->setText( curve->getExpression() );
      myNbIntervals->setValue( curve->getNbIntervals() );
      myAutoAssign->setChecked( curve->isAutoAssign() );
      if(!curve->isAutoAssign()) {
	myMarkerType->setCurrentIndex( curve->getMarker() );
	myLineType->setCurrentIndex( curve->getLine() );
	myLineWidth->setValue( curve->getLineWidth() );
	myColor->setColor( curve->getColor() );
      } else {
	myMarkerType->setCurrentIndex( 0 );
	myLineType->setCurrentIndex( 0 );
	myLineWidth->setValue( 1 );
	myColor->setColor( QColor() );	
      }
      myCurrentItem = item;      
      if( select ) {
	disconnectSelectionChanged();
	item->setSelected(true);
	connectSelectionChanged();
      }
    }
  }
}

/*!
  Enable/Disable "curve parameters" and "curve properties" widgets.
*/
void Plot2d_AnaliticCurveDlg::checkState() {
  bool isEnable = myCurvesList->count() > 0;
  myCurveParams->setEnabled(isEnable);
  myCurveProps->setEnabled(isEnable);
}
/*!
  Show error message
 */
void Plot2d_AnaliticCurveDlg::showErrorMsg() {
  SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "AC_CANT_CALCULATE" ) );
}

/*!
  Check the curve attached to the item.
*/
bool Plot2d_AnaliticCurveDlg::checkItem(const QListWidgetItem* item) {
  bool res = false;
  if( !myFormula->text().isEmpty() && item ) {
    QVariant var = item->data(Qt::UserRole);
    Plot2d_AnaliticCurve* curve = (Plot2d_AnaliticCurve*)var.value<void*>();

    if( curve ) {
      curve->setExpression(myFormula->text());
      QwtScaleDiv* div = myPlot->axisScaleDiv(QwtPlot::xBottom);
      curve->setRangeBegin(div->lowerBound());
      curve->setRangeEnd(div->upperBound());
      curve->setNbIntervals(myNbIntervals->value());
      curve->calculate();
      if( curve->state() == Plot2d_AnaliticCurve::StateOk )
	res = true;
    }
  }
  return res;
}
/*!
  Store properties of the curve attached to the item from "Curve properties" widget.
*/
void Plot2d_AnaliticCurveDlg::storeProps( const QListWidgetItem* item) {
  if( item ) {  
    QVariant var = item->data(Qt::UserRole);
    Plot2d_AnaliticCurve* curve = (Plot2d_AnaliticCurve*)var.value<void*>();
    if(curve) {
      curve->setAutoAssign(myAutoAssign->isChecked());
      curve->setName(item->text());
      curve->setActive(item->checkState() == Qt::Checked);
      if(!myAutoAssign->isChecked()) {
	curve->setMarker((Plot2d::MarkerType)myMarkerType->currentIndex());
	curve->setLine((Plot2d::LineType)myLineType->currentIndex());
	curve->setLineWidth(myLineWidth->value());
	curve->setColor(myColor->color());
      } 
    }
  }
}

/*!
  Connect currentItemChanged signal.
*/
void Plot2d_AnaliticCurveDlg::connectSelectionChanged() {
  connect( myCurvesList, SIGNAL( itemSelectionChanged ()),
	   this, SLOT( onCurveSelectionChanged() ) );
}

/*!
  Disconnect currentItemChanged signal.
*/
void Plot2d_AnaliticCurveDlg::disconnectSelectionChanged() {
  disconnect( myCurvesList, SIGNAL( itemSelectionChanged ()),
	      this, SLOT( onCurveSelectionChanged() ) );
}

/*!
  Private slot. Called then "Add curve" button is clicked
*/
void Plot2d_AnaliticCurveDlg::onAddCurve() {
  /*  QList<QListWidgetItem*> items = myCurvesList->selectedItems();
      QListWidgetItem* item = items.size() > 0 ? items[0] : 0;
      if(item) {
      if(!checkItem(item)) {
      showErrorMsg();
      return;
      }
      else {
      updateInView(item);
      }
      }
  */
  
  Plot2d_AnaliticCurve* curve = new Plot2d_AnaliticCurve();
  QListWidgetItem* newItem = new QListWidgetItem(curve->getName());
  newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
  newItem->setCheckState(Qt::Checked);
  myCurvesList->addItem(newItem);
  QVariant var;
  var.setValue((void*)curve);
  newItem->setData(Qt::UserRole,var);

  bool autoFlag = curve->isAutoAssign();
  if(autoFlag) {
    curve->autoFill(myPlot);
  }
  
  setCurrentCurve(newItem);
  checkState();
}

/*!
  Private slot. Called then "Remove curve" button is clicked
*/
void Plot2d_AnaliticCurveDlg::onRemoveCurve() {
  disconnectSelectionChanged();
  QList<QListWidgetItem*> items = myCurvesList->selectedItems();
  QListWidgetItem* selected = items.size() > 0 ? items[0] : 0;
  int row = myCurvesList->row(selected);
  int prevRow = (row == 0) ? 1 : row - 1;
  if( selected ) {
    QVariant var = selected->data(Qt::UserRole);
    Plot2d_AnaliticCurve* curve = (Plot2d_AnaliticCurve*)var.value<void*>();
    if( curve ) {
      if( curve->getAction() == Plot2d_AnaliticCurve::ActAddInView ) {
	delete curve;
      } else {
	curve->setAction(Plot2d_AnaliticCurve::ActRemoveFromView);
	updateInView(selected);
      }
    }
  }

  if( prevRow >= 0 && prevRow < myCurvesList->count() ) {
    myCurvesList->item(prevRow)->setSelected(true);
    setCurrentCurve(myCurvesList->item(prevRow));
  }
    
  selected = myCurvesList->takeItem(row);
  delete selected;
  connectSelectionChanged();
  checkState();
}

/*!
  Private slot. Called then selection in the curve list is changed.
*/
void Plot2d_AnaliticCurveDlg::onCurveSelectionChanged() {  

  QList<QListWidgetItem*> items = myCurvesList->selectedItems();
  QListWidgetItem* selected = items.size() > 0 ? items[0] : 0;
  
  /*  bool ok = myCurrentItem ? checkItem(myCurrentItem) : true;

  if(!ok) {
  showErrorMsg();
  disconnectSelectionChanged();
  
  if( selected )
  selected->setSelected(false);
  
  if( myCurrentItem ) {
  myCurrentItem->setSelected(true);
  myCurvesList->setCurrentItem(myCurrentItem);
  }
  
  connectSelectionChanged();
  
  } else {    
  updateInView(myCurrentItem);    
  */
  
  if(selected) {
    setCurrentCurve(selected, false);
  }
  //}
}

/*!
  Private slot. Called then "Auto assign" checkbox is clicked
*/
void Plot2d_AnaliticCurveDlg::onAutoAssign(int state){
  bool flag = !state;
  myMarkerType->setEnabled(flag);
  myLineType->setEnabled(flag);
  myLineWidth->setEnabled(flag);
  myColor->setEnabled(flag);
  
  QList<QListWidgetItem*> items = myCurvesList->selectedItems();
  QListWidgetItem* selected = items.size() > 0 ? items[0] : 0;    
  if(selected) {
    QVariant var = selected->data(Qt::UserRole);
    Plot2d_AnaliticCurve* curve = (Plot2d_AnaliticCurve*)var.value<void*>();
    if(curve) {
      curve->setAutoAssign(state);
    }
    setCurrentCurve(selected);
  }
}

/*!
  Update curve in the view
*/
void Plot2d_AnaliticCurveDlg::updateInView( QListWidgetItem* item ) {
  storeProps(item);
  Plot2d_ViewFrame* fr = qobject_cast<Plot2d_ViewFrame*>(parent());
  QVariant var = item->data(Qt::UserRole);
  
  Plot2d_AnaliticCurve* curve = (Plot2d_AnaliticCurve*)var.value<void*>();
  if( fr && curve ) {
    fr->updateAnaliticCurve(curve, true);
  }
}

/*!
  Private slot. Called then "Update Curve" checkbox is clicked
*/
void Plot2d_AnaliticCurveDlg::onUpdateCurve() {
  QList<QListWidgetItem*> items = myCurvesList->selectedItems();
  int sz = items.size();
  if( sz > 0 ) {
    QListWidgetItem* selected = items[0];
  
    bool ok = myCurrentItem ? checkItem(myCurrentItem) : false;
    if(!ok) {
      showErrorMsg();
      return;
    } else {
      updateInView(selected);
    }
  }
}
