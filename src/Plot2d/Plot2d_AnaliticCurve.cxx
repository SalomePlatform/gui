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
//  File   : Plot2d_AnaliticCurve.cxx
//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)

#include "Plot2d_AnaliticParcer.h"
#include "Plot2d_AnaliticCurve.h"
#include "Plot2d_PlotItems.h"


//Init static data;

int Plot2d_AnaliticCurve::myNbCurves = 0;

/*!
  Constructor
*/
Plot2d_AnaliticCurve::Plot2d_AnaliticCurve() : 
  myAutoAssign(true),
  myColor( 0, 0, 0 ), 
  myMarker( Plot2d::Circle ), 
  myMarkerSize( 0 ), 
  myLine( Plot2d::Solid ), 
  myLineWidth( 0 ),
  myRangeBegin(0.0),
  myRangeEnd(100.0),
  myNbIntervals(100),
  myExpression(""),
  myAction(Plot2d_AnaliticCurve::ActAddInView),
  myState(Plot2d_AnaliticCurve::StateNeedUpdate),
  myCurve(0),
  myActive(true)
{
  myName = QString("Analitic Curve %1").arg(++myNbCurves);
}


/*!
  Destructor
*/
Plot2d_AnaliticCurve::~Plot2d_AnaliticCurve()
{
}

/*!
  Copy constructor. Makes deep copy of data
*/
Plot2d_AnaliticCurve::Plot2d_AnaliticCurve( const Plot2d_AnaliticCurve& curve )
{
  myAutoAssign = curve.isAutoAssign();
  myColor      = curve.getColor();
  myMarker     = curve.getMarker();
  myMarkerSize = curve.getMarkerSize();
  myLine       = curve.getLine();
  myLineWidth  = curve.getLineWidth();
  myRangeBegin = curve.getRangeBegin();
  myRangeEnd   = curve.getRangeEnd();
  myNbIntervals= curve.getNbIntervals();
  myPoints     = curve.myPoints;
  myAction     = curve.getAction();
  myName       = curve.getName();
  myExpression = curve.getExpression();
  myState      = curve.state();
  myCurve      = curve.myCurve;
  myActive     = curve.isActive();
}

/*!
  operator=. Makes deep copy of data
*/
Plot2d_AnaliticCurve& Plot2d_AnaliticCurve::operator=( const Plot2d_AnaliticCurve& curve )
{
  myAutoAssign = curve.isAutoAssign();
  myColor      = curve.getColor();
  myMarker     = curve.getMarker();
  myMarkerSize = curve.getMarkerSize();
  myLine       = curve.getLine();
  myLineWidth  = curve.getLineWidth();
  myRangeBegin = curve.getRangeBegin();
  myRangeEnd   = curve.getRangeEnd();
  myNbIntervals= curve.getNbIntervals();
  myPoints     = curve.myPoints;
  myAction     = curve.getAction();
  myName       = curve.getName();
  myExpression = curve.getExpression();
  myState      = curve.state();
  myCurve      = curve.myCurve;
  myActive     = curve.isActive();
  return *this;
}

/*!
  Create plot object for the curve
*/
QwtPlotItem* Plot2d_AnaliticCurve::plotItem()
{
  if(!myCurve) {
    myCurve = new Plot2d_QwtPlotCurve(QString(""));
    updatePlotItem();
  }
  return myCurve;
}

/*!
  Auto fill parameters of object by plot view
*/
void Plot2d_AnaliticCurve::autoFill( const QwtPlot* thePlot )
{
  QwtSymbol::Style typeMarker;
  QColor           color;
  Qt::PenStyle     typeLine;
  Plot2d::getNextMarker( QwtPlotItem::Rtti_PlotCurve, thePlot, typeMarker, color, typeLine );
  
  setColor( color );
  setLine( Plot2d::qwt2plotLine( typeLine ));
  setLineWidth(1);
  setMarker( Plot2d::qwt2plotMarker( typeMarker ) );
}

/*!
  Updates curve fields
*/
void Plot2d_AnaliticCurve::updatePlotItem()
{
  if ( !myCurve )
    return;

  Plot2d_QwtPlotCurve* aCurve = dynamic_cast<Plot2d_QwtPlotCurve*>(myCurve);

  if(!aCurve)
    return;

  QwtPlot* aPlot = aCurve->plot();
  if ( aPlot ) {
    aCurve->detach();
    aCurve->attach( aPlot );
  }
  
  Qt::PenStyle     ps = Plot2d::plot2qwtLine( getLine() );
  QwtSymbol::Style ms = Plot2d::plot2qwtMarker( getMarker() );

  aCurve->setPen( QPen(getColor() , getLineWidth(), ps ) );

  aCurve->setSymbol( QwtSymbol( ms, QBrush( getColor() ), 
				QPen( getColor()),
 				QSize( getMarkerSize() , getMarkerSize() ) ) );
  double *x, *y;
  long nb = getData( &x, &y );
  aCurve->setData( x, y, nb );
  aCurve->setTitle(getName());
}


/*!
  Calculate the curve points.
*/
void Plot2d_AnaliticCurve::calculate() {
  if( state() == Plot2d_AnaliticCurve::StateOk )
    return;

  if(myRangeBegin > myRangeEnd)
    return;

  Plot2d_AnaliticParcer* parcer = Plot2d_AnaliticParcer::parcer();
  double* x = 0;
  double* y = 0;
  int nb = parcer->calculate(getExpression(), getRangeBegin(), getRangeEnd(),
			     getNbIntervals(),&x,&y);
  if( nb > 0 ) {
    myPoints.clear();
    for( int i = 0; i < nb; i++ ) {
      Plot2d_Point pnt(x[i], y[i]);
      myPoints.append(pnt);
    }
    delete x;
    delete y;
    myState = Plot2d_AnaliticCurve::StateOk;
    setAction(Plot2d_AnaliticCurve::ActUpdateInView);
  }
}

/*!
  Gets object's data
*/
long Plot2d_AnaliticCurve::getData( double** theX, double** theY ) const
{
  int aNPoints = getNbIntervals() + 1;
  *theX = new double[aNPoints];
  *theY = new double[aNPoints];
  for (int i = 0; i < aNPoints; i++) {
    (*theX)[i] = myPoints[i].x;
    (*theY)[i] = myPoints[i].y;
  }
  return aNPoints;
}

/*!
  Sets curves's AutoAssign flag - in this case attributes will be set automatically
*/
void Plot2d_AnaliticCurve::setAutoAssign( bool on )
{
  if( myAutoAssign != on ) {
    myAutoAssign = on;
    setAction(Plot2d_AnaliticCurve::ActUpdateInView);
  }
}

/*!
  Gets curve's AutoAssign flag state
*/
bool Plot2d_AnaliticCurve::isAutoAssign() const
{
  return myAutoAssign;
}

/*!
  Sets curve's color.
*/
void Plot2d_AnaliticCurve::setColor( const QColor& color )
{
  if(myColor != color) {
    myColor = color;
    setAction(Plot2d_AnaliticCurve::ActUpdateInView);
  }
}

/*!
  Gets curve's color
*/
QColor Plot2d_AnaliticCurve::getColor() const
{
  return myColor;
}


/*!
  Sets marker type ( and resets AutoAssign flag )
*/
void Plot2d_AnaliticCurve::setMarker( Plot2d::MarkerType marker )
{
  if(myMarker != marker) {
    myMarker = marker;
    setAction(Plot2d_AnaliticCurve::ActUpdateInView);
  }
}

/*!
  Gets marker type
*/
Plot2d::MarkerType Plot2d_AnaliticCurve::getMarker() const
{
  return myMarker;
}

/*!
  Sets new marker size
*/
void Plot2d_AnaliticCurve::setMarkerSize( const int theSize )
{
  if( myMarkerSize != theSize ) {
    myMarkerSize = theSize < 0 ? 0 : theSize;
    setAction(Plot2d_AnaliticCurve::ActUpdateInView);
  }
}

/*!
  Gets marker size
*/
int Plot2d_AnaliticCurve::getMarkerSize() const
{
  return myMarkerSize;
}

/*!
  Sets line type
*/
void Plot2d_AnaliticCurve::setLine( Plot2d::LineType line )
{
  if(myLine != line) {
    myLine = line;
    setAction(Plot2d_AnaliticCurve::ActUpdateInView);
  }
}

/*!
  Gets line type
*/
Plot2d::LineType Plot2d_AnaliticCurve::getLine() const
{
  return myLine;
}


/*!
  Sets line width
*/
void Plot2d_AnaliticCurve::setLineWidth( const int lineWidth )
{
  if( myLineWidth != lineWidth ) {
    myLineWidth = lineWidth < 0 ? 0 : lineWidth;
    setAction(Plot2d_AnaliticCurve::ActUpdateInView);
  }
}

/*!
  Gets line width
*/
int Plot2d_AnaliticCurve::getLineWidth() const
{
  return myLineWidth;
}

/*!
  Sets number of points
*/
void Plot2d_AnaliticCurve::setNbIntervals( const long nb )
{
  if( myNbIntervals != nb ) {
    myNbIntervals = nb < 1 ? 1 : nb;
    myState = Plot2d_AnaliticCurve::StateNeedUpdate;
  }
}

/*!
  Gets number of points
*/
long Plot2d_AnaliticCurve::getNbIntervals() const
{
  return myNbIntervals;
}

/*!
  Sets X coordinate of the first curve points
*/
void Plot2d_AnaliticCurve::setRangeBegin( const double coord) {
  if( myRangeBegin != coord ) {
    myRangeBegin = coord;
    myState = Plot2d_AnaliticCurve::StateNeedUpdate;
  }
}

/*!
  Gets X coordinate of the first curve points
*/
double Plot2d_AnaliticCurve::getRangeBegin() const {
  return myRangeBegin;
}

/*!
  Sets X coordinate of the last curve points
*/
void Plot2d_AnaliticCurve::setRangeEnd( const double coord) {
  if( myRangeEnd != coord ) {
    myRangeEnd = coord;
    myState = Plot2d_AnaliticCurve::StateNeedUpdate;
  }
}

/*!
  Gets X coordinate of the last curve points
*/
double Plot2d_AnaliticCurve::getRangeEnd() const {
  return myRangeEnd;
}

/*!
  Sets the curve expression.
*/
void Plot2d_AnaliticCurve::setExpression( const QString& expr ) {
  if( myExpression != expr ) {
    myExpression = expr;
    myState = Plot2d_AnaliticCurve::StateNeedUpdate;
  }
}

/*!
  Gets the curve expression.
*/
QString Plot2d_AnaliticCurve::getExpression() const {
  return  myExpression;
}

/*!
  Sets the curve name.
*/
void Plot2d_AnaliticCurve::setName( const QString& name ) {
  if( myName != name ) {    
    myName = name;
    setAction(Plot2d_AnaliticCurve::ActUpdateInView);
  }
}

/*!
  Gets the curve name.
*/
QString Plot2d_AnaliticCurve::getName() const {
  return myName;
}


/*!
  Sets the curve action.
*/
void Plot2d_AnaliticCurve::setAction(const int act) {
  if( act == Plot2d_AnaliticCurve::ActNothing ) {
    myAction = act;
    return;
  }
  
  if(myAction != Plot2d_AnaliticCurve::ActAddInView && 
     myAction != Plot2d_AnaliticCurve::ActRemoveFromView) {
    myAction = act;  
  }
}

/*!
  Gets the curve action.
*/
int Plot2d_AnaliticCurve::getAction() const {
  return myAction;
}

/*!
  Gets the curve state.
*/
int Plot2d_AnaliticCurve::state() const {
  return myState;
}

/*!
  Sets the curve active status.
*/
void Plot2d_AnaliticCurve::setActive(const bool on) {
  if( myActive != on ) {    
    myActive = on;
    setAction(Plot2d_AnaliticCurve::ActUpdateInView);
  }  
}

/*!
  Gets the curve active status.
*/
bool Plot2d_AnaliticCurve::isActive() const {
  return myActive;
}
