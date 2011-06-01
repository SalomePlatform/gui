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
//  File   : Plot2d_Curve.cxx
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "Plot2d_Curve.h"
#include "Plot2d_PlotItems.h"
#include <qwt_plot_curve.h>

const int DEFAULT_LINE_WIDTH  =  0;     // (default) line width
const int DEFAULT_MARKER_SIZE =  9;     // default marker size
const int MAX_ATTEMPTS        = 10;     // max attempts

/*!
  Constructor
*/
Plot2d_Curve::Plot2d_Curve()
: Plot2d_Object(),
  myColor( 0, 0, 0 ), 
  myMarker( Plot2d::Circle ), 
  myMarkerSize( 0 ), 
  myLine( Plot2d::Solid ), 
  myLineWidth( 0 )
{
}

/*!
  Destructor
*/
Plot2d_Curve::~Plot2d_Curve()
{
}

/*!
  Copy constructor. Makes deep copy of data
*/
Plot2d_Curve::Plot2d_Curve( const Plot2d_Curve& curve )
: Plot2d_Object( curve )
{
  myColor      = curve.getColor();
  myMarker     = curve.getMarker();
  myMarkerSize = curve.getMarkerSize();
  myLine       = curve.getLine();
  myLineWidth  = curve.getLineWidth();
}

/*!
  operator=. Makes deep copy of data
*/
Plot2d_Curve& Plot2d_Curve::operator=( const Plot2d_Curve& curve )
{
  Plot2d_Object::operator=(curve);
  myColor      = curve.getColor();
  myMarker     = curve.getMarker();
  myMarkerSize = curve.getMarkerSize();
  myLine       = curve.getLine();
  myLineWidth  = curve.getLineWidth();
  return *this;
}

/*!
  Get typeid for the plot2d curve class
*/
int Plot2d_Curve::rtti()
{
  return QwtPlotItem::Rtti_PlotCurve;
}

/*!
  Create plot object for the curve
*/
QwtPlotItem* Plot2d_Curve::createPlotItem()
{
  QwtPlotCurve* aCurve = new Plot2d_QwtPlotCurve( getVerTitle(), getYAxis() );
  updatePlotItem( aCurve );
  return aCurve;
}

/*!
  Auto fill parameters of object by plot view
*/
void Plot2d_Curve::autoFill( const QwtPlot* thePlot )
{
  QwtSymbol::Style typeMarker;
  QColor           color;
  Qt::PenStyle     typeLine;
  getNextMarker( thePlot, typeMarker, color, typeLine );

  setColor( color );
  setLine( Plot2d::qwt2plotLine( typeLine ), DEFAULT_LINE_WIDTH );
  setMarker( Plot2d::qwt2plotMarker( typeMarker ) );
}

/*!
  Updates curve fields
*/
void Plot2d_Curve::updatePlotItem( QwtPlotItem* theItem )
{
  if ( theItem->rtti() != rtti() )
    return;

  QwtPlotCurve* aCurve = dynamic_cast<QwtPlotCurve*>( theItem );
  if ( !aCurve )
    return;

  Plot2d_Object::updatePlotItem( theItem );

  Qt::PenStyle     ps = Plot2d::plot2qwtLine( getLine() );
  QwtSymbol::Style ms = Plot2d::plot2qwtMarker( getMarker() );

  aCurve->setPen( QPen( getColor(), getLineWidth(), ps ) );
  aCurve->setSymbol( QwtSymbol( ms, QBrush( getColor() ), 
				QPen( getColor() ), 
				QSize( getMarkerSize(), getMarkerSize() ) ) );
  double *x, *y;
  long nb = getData( &x, &y );
  aCurve->setData( x, y, nb );
}

/*!
  Sets curve's color ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setColor( const QColor& color )
{
  myColor = color;
  setAutoAssign( false );
}

/*!
  Gets curve's color
*/
QColor Plot2d_Curve::getColor() const
{
  return myColor;
}

/*!
  Sets marker type and size ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setMarker( Plot2d::MarkerType marker, const int markerSize )
{
  setMarker( marker );
  setMarkerSize( markerSize );
  setAutoAssign( false );
}

/*!
  Sets marker type ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setMarker( Plot2d::MarkerType marker )
{
  myMarker = marker;
  setAutoAssign( false );
}

/*!
  Gets marker type
*/
Plot2d::MarkerType Plot2d_Curve::getMarker() const
{
  return myMarker;
}

/*!
  Sets new marker size ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setMarkerSize( const int theSize )
{
  myMarkerSize = theSize < 0 ? 0 : theSize;
  setAutoAssign( false );
}

/*!
  Gets marker size
*/
int Plot2d_Curve::getMarkerSize() const
{
  return myMarkerSize;
}

/*!
  Sets line type and width ( and resets AutoAssign flag )
  NOTE : A line width of 0 will produce a 1 pixel wide line using a fast algorithm for diagonals. 
         A line width of 1 will also produce a 1 pixel wide line, but uses a slower more accurate 
         algorithm for diagonals. 
         For horizontal and vertical lines a line width of 0 is the same as a line width of 1.
*/
void Plot2d_Curve::setLine( Plot2d::LineType line, const int lineWidth )
{
  setLine( line );
  setLineWidth( lineWidth );
  setAutoAssign( false );
}

/*!
  Sets line type ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setLine( Plot2d::LineType line )
{
  myLine = line;
  setAutoAssign( false );
}

/*!
  Gets line type
*/
Plot2d::LineType Plot2d_Curve::getLine() const
{
  return myLine;
}

/*!
  Sets line width ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setLineWidth( const int lineWidth )
{
  myLineWidth = lineWidth < 0 ? 0 : lineWidth;
  setAutoAssign( false );
}

/*!
  Gets line width
*/
int Plot2d_Curve::getLineWidth() const
{
  return myLineWidth;
}

/*!
  Gets new unique marker for item if possible
*/
void Plot2d_Curve::getNextMarker( const QwtPlot* thePlot, QwtSymbol::Style& typeMarker,
                                  QColor& color, Qt::PenStyle& typeLine ) 
{
  bool bOk = false;
  int cnt = 0;
  while ( !bOk ) {
    int aRed    = (int)( 256.0 * rand() / RAND_MAX );  // generate random color
    int aGreen  = (int)( 256.0 * rand() / RAND_MAX );  // ...
    int aBlue   = (int)( 256.0 * rand() / RAND_MAX );  // ...
    int aMarker = (int)( 9.0 * rand() / RAND_MAX ) + 1;// 9 markers types( not including empty )
    int aLine   = (int)( 5.0 * rand() / RAND_MAX ) + 1;// 5 line types ( not including empty )

    typeMarker = ( QwtSymbol::Style )aMarker;
    color      = QColor( aRed, aGreen, aBlue );
    typeLine   = ( Qt::PenStyle )aLine;

    bOk = ( ++cnt == MAX_ATTEMPTS ) || !existMarker( thePlot, typeMarker, color, typeLine );
  }
}

/*!
  Checks if marker belongs to any enitity
*/
bool Plot2d_Curve::existMarker( const QwtPlot* thePlot, const QwtSymbol::Style typeMarker,
				const QColor& color, const Qt::PenStyle typeLine ) 
{
  bool ok = false;

  QColor bgColor = thePlot->palette().color( QPalette::Background );
  if ( closeColors( color, bgColor ) ) {
    ok = true;
  }
  else {
    QwtPlotItemList anItems = thePlot->itemList();
    QwtPlotItemIterator anIt = anItems.begin(), aLast = anItems.end();
    QwtPlotItem* anItem;
    for ( ; anIt != aLast && !ok; anIt++ ) {
      anItem = *anIt;
      if ( anItem && anItem->rtti() == rtti() ) {
	QwtPlotCurve* crv = dynamic_cast<QwtPlotCurve*>( anItem );
	if ( crv ) {
	  QwtSymbol::Style aStyle = crv->symbol().style();
	  QColor           aColor = crv->pen().color();
	  Qt::PenStyle     aLine  = crv->pen().style();
	  ok = closeColors( aColor, color ) && aStyle == typeMarker && aLine == typeLine;
	}
      }
    }
  }
  return ok;
}
