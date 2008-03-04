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
// File   : Plot2d_Curve.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "Plot2d_Curve.h"

#define DEFAULT_LINE_WIDTH     0     // (default) line width
#define DEFAULT_MARKER_SIZE    9     // default marker size

/*!
  Constructor
*/
Plot2d_Curve::Plot2d_Curve()
: Plot2d_Object(),
  myColor( 0,0,0 ),
  myMarkerSize( 0 ), 
  myMarker( Plot2d::Circle ), 
  myLine( Plot2d::Solid ), 
  myLineWidth( 0 )
{
}

/*!
  Copy constructor. Makes deep copy of data.
*/
Plot2d_Curve::Plot2d_Curve( const Plot2d_Curve& curve )
: Plot2d_Object( curve )
{
  myColor      = curve.getColor();
  myMarker     = curve.getMarker();
  myLine       = curve.getLine();
  myLineWidth  = curve.getLineWidth();
}

/*!
  operator=. Makes deep copy of data.
*/
Plot2d_Curve& Plot2d_Curve::operator=( const Plot2d_Curve& curve )
{
  myAutoAssign = curve.isAutoAssign();
  myHorTitle   = curve.getHorTitle();
  myVerTitle   = curve.getVerTitle();
  myHorUnits   = curve.getHorUnits();
  myVerUnits   = curve.getVerUnits();
  myName       = curve.getName();
  myColor      = curve.getColor();
  myMarker     = curve.getMarker();
  myLine       = curve.getLine();
  myLineWidth  = curve.getLineWidth();
  myPoints     = curve.getPointList();
  return *this;
}

/**
 *
 */
int Plot2d_Curve::rtti()
{
  return QwtPlotItem::Rtti_PlotCurve;
}

/*!
  Displays curve in the given plot.
*/
QwtPlotItem* Plot2d_Curve::createPlotItem()
{
  QwtPlotCurve* aCurve = new QwtPlotCurve();
  updatePlotItem( aCurve );
  return aCurve;
}

/**
 * Auto fill parameters of object by plot view
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
 * Updates curve fields
 */
void Plot2d_Curve::updatePlotItem( QwtPlotItem* theItem )
{
  if ( theItem->rtti() != rtti() )
    return;
  QwtPlotCurve* aCurve = dynamic_cast<QwtPlotCurve*>( theItem );
  if ( !aCurve )
    return;

  Qt::PenStyle     ps = Plot2d::plot2qwtLine( getLine() );
  QwtSymbol::Style ms = Plot2d::plot2qwtMarker( getMarker() );

  aCurve->setPen( QPen( getColor(), getLineWidth(), ps ) );
  aCurve->setSymbol( QwtSymbol( ms, QBrush( getColor() ), 
                                 QPen( getColor() ), 
                                 QSize( myMarkerSize, myMarkerSize ) ) );
  aCurve->setData( horData(), verData(), nbPoints() );
  aCurve->setTitle( !getName().isEmpty() ? getName() : getVerTitle() );
}

/*!
  Sets curve's color ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setColor( const QColor& color )
{
  myColor = color;
  myAutoAssign = false;
}

/*!
  Gets curve's color
*/
QColor Plot2d_Curve::getColor() const
{
  return myColor;
}

/*!
  Sets new marker size
*/
void Plot2d_Curve::setMarkerSize( const int theSize )
{
  myMarkerSize = theSize;
}

/*!
  Sets curve's marker ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setMarker( Plot2d::MarkerType marker )
{
  myMarker = marker;
  myAutoAssign = false;
}

/*!
  Gets curve's marker
*/
Plot2d::MarkerType Plot2d_Curve::getMarker() const
{
  return myMarker;
}
/*!
  Sets curve's line type and width ( and resets AutoAssign flag )
  NOTE : A line width of 0 will produce a 1 pixel wide line using a fast algorithm for diagonals. 
         A line width of 1 will also produce a 1 pixel wide line, but uses a slower more accurate 
         algorithm for diagonals. 
         For horizontal and vertical lines a line width of 0 is the same as a line width of 1.
*/
void Plot2d_Curve::setLine( Plot2d::LineType line, const int lineWidth )
{
  myLine = line;
  myLineWidth = lineWidth;
  if ( myLineWidth < 0 ) myLineWidth = 0;
  myAutoAssign = false;
}

/*!
  Gets curve's line type
*/
Plot2d::LineType Plot2d_Curve::getLine() const
{
  return myLine;
}

/*!
  Gets curve's line width
*/
int Plot2d_Curve::getLineWidth() const
{
  return myLineWidth;
}

/*!
  Gets new unique marker for item if possible
*/
const int  MAX_ATTEMPTS   = 10;
void Plot2d_Curve::getNextMarker( const QwtPlot* thePlot, QwtSymbol::Style& typeMarker,
                                  QColor& color, Qt::PenStyle& typeLine ) 
{
  bool bOk = false;
  int cnt = 1;
  while ( !bOk ) {
    int aRed    = (int)( 256.0 * rand() / RAND_MAX);  // generate random color
    int aGreen  = (int)( 256.0 * rand() / RAND_MAX);  // ...
    int aBlue   = (int)( 256.0 * rand() / RAND_MAX);  // ...
    int aMarker = (int)( 9.0 * rand() / RAND_MAX) + 1;// 9 markers types( not including empty )
    int aLine   = (int)( 5.0 * rand() / RAND_MAX) + 1;// 5 line types ( not including empty )

    typeMarker = ( QwtSymbol::Style )aMarker;
    color      = QColor( aRed, aGreen, aBlue );
    typeLine   = ( Qt::PenStyle )aLine;

    cnt++;
    if ( cnt == MAX_ATTEMPTS )
      bOk = true;
    else
      bOk = !existMarker( thePlot, typeMarker, color, typeLine );
  }
/*
  static int aMarker = -1;
  static int aColor  = -1;
  static int aLine   = -1;

  if ( myColors.isEmpty() ) {
    // creating colors list
    myColors.append( Qt::white );
    myColors.append( Qt::blue );
    myColors.append( Qt::gray );
    myColors.append( Qt::darkGreen );
    myColors.append( Qt::magenta );
    myColors.append( Qt::darkGray );
    myColors.append( Qt::red );
    myColors.append( Qt::darkBlue );
    myColors.append( Qt::darkYellow );
    myColors.append( Qt::cyan );
    myColors.append( Qt::darkRed );
    myColors.append( Qt::darkCyan );
    myColors.append( Qt::yellow );
    myColors.append( Qt::darkMagenta );
    myColors.append( Qt::green );
    myColors.append( Qt::black );
  }

  int nbMarkers = 11;                   // QwtSymbol supports 11 marker types
  int nbLines   = 6;                    // Qt supports 6 line types
  int nbColors  = myColors.count();     // number of default colors supported

  aMarker = ( aMarker + 1 ) % nbMarkers;  
  if ( aMarker == QwtSymbol::None || aMarker == QwtSymbol::Triangle ) aMarker++;
  aColor  = ( aColor  + 1 ) % nbColors;
  aLine   = ( aLine   + 1 ) % nbLines;    
  if ( aLine == Qt::NoPen ) aLine++;             

  typeMarker = ( QwtSymbol::Style )aMarker;
  color      = myColors[ aColor ];
  typeLine   = ( Qt::PenStyle )aLine;
  if ( !existMarker( thePlot, typeMarker, color, typeLine ) )
    return;

  int i, j, k;
  for ( i = 0; i < nbMarkers; i++ ) {
    aMarker = ( aMarker + 1 ) % nbMarkers;
    if ( aMarker == QwtSymbol::None || aMarker == QwtSymbol::Triangle ) aMarker++;
    for ( j = 0; j < nbColors; j++ ) {
      aColor  = ( aColor  + 1 ) % nbColors;
      for ( k = 0; k < nbLines; k++ ) {
        aLine = ( aLine + 1 ) % nbLines;
  if ( aLine == Qt::NoPen ) aLine++;             
        if ( !existMarker( ( QwtSymbol::Style )aMarker, aColor, ( Qt::PenStyle )aLine ) ) {
          typeMarker = ( QwtSymbol::Style )aMarker;
          color      = myColors[ aColor ];
          typeLine   = ( Qt::PenStyle )aLine;
          return;
        }
      }
    }
  }
*/
}

/*!
  Checks if marker belongs to any enitity
*/
bool Plot2d_Curve::existMarker( const QwtPlot* thePlot, const QwtSymbol::Style typeMarker,
                                 const QColor& color, const Qt::PenStyle typeLine ) 
{
  QColor aColor = thePlot->palette().color( QPalette::Background );
  if ( closeColors( color, aColor ) )
      return true;
  QwtPlotItemList anItems = thePlot->itemList();
  QwtPlotItemIterator anIt = anItems.begin(), aLast = anItems.end();
  QwtPlotItem* anItem;
  for( ; anIt != aLast; anIt++ ) {
    anItem = *anIt;
    if( anItem && anItem->rtti() == rtti() ) {
      QwtPlotCurve* crv = dynamic_cast<QwtPlotCurve*>( anItem );
      if ( crv ) {
        QwtSymbol::Style aStyle = crv->symbol().style();
        QColor           aColor = crv->pen().color();
        Qt::PenStyle     aLine  = crv->pen().style();
        if ( aStyle == typeMarker && closeColors( aColor,color ) && aLine == typeLine )
          return true;
      }
    }
  }
  return false;
}
