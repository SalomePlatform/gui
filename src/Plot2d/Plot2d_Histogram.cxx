// Name   : Plot2d_Histogram.cpp
// Purpose: Implementation of Plot2d_Histogram class.
//
// History:
// 11/12/07 - Natalia Ermolaeva - Creation of the file

#include <Plot2d_Histogram.h>

#include <qwt_series_data.h>
#include <qwt_plot_curve.h>
//#include <qwt_compat.h>
#include <QColor>
#include <math.h>

/**
 * Constructor.
 */
Plot2d_Histogram::Plot2d_Histogram()
  : m_dWidth( 0 ), m_dDefWidth( 0 )
{
}

/**
 * Destructor.
 */
Plot2d_Histogram::~Plot2d_Histogram()
{
}

/**
 *
 */
int Plot2d_Histogram::rtti()
{
  return QwtPlotItem::Rtti_PlotHistogram;
}

/*!
  Displays curve in the given plot.
*/
QwtPlotItem* Plot2d_Histogram::createPlotItem()
{
  Plot2d_HistogramItem* anItem = new Plot2d_HistogramItem();
  updatePlotItem( anItem );
  return anItem;
}

/**
 * Auto fill parameters of object by plot view
 */
void Plot2d_Histogram::autoFill( const QwtPlot* thePlot )
{
  setColor( getNextColor( thePlot ) );
}

/*!
 * Updates curve fields
 */
void Plot2d_Histogram::updatePlotItem( QwtPlotItem* theItem )
{
  if ( theItem->rtti() != rtti() )
    return;
  Plot2d_HistogramItem* anItem = dynamic_cast<Plot2d_HistogramItem*>
                                  ( theItem );
  if ( !anItem )
    return;
  Plot2d_Object::updatePlotItem( theItem );

  anItem->setData( getData() );
  anItem->setColor( getColor() );
}

/**
 * Sets data to object
 */
void Plot2d_Histogram::setData( const QList<double>& theXVals,
                                 const QList<double>& theYVals )
{
  pointList aPoints;
  int aSize = theXVals.size();
  for ( int i = 0; i < aSize; i++ ) {
    Plot2d_Point aPoint;
    aPoint.x = theXVals[i];
    aPoint.y = theYVals[i];
    aPoint.text = QString();
    aPoints.append( aPoint );
  }
  setPointList( aPoints );

  m_dDefWidth = getMinInterval( theXVals )*(2./3.);
  m_dWidth = m_dDefWidth;
}

/**
 * Returns data
 */
QwtIntervalSeriesData Plot2d_Histogram::getData() const
{
  pointList aPoints = getPointList();
  int aSize = aPoints.size();
  QVector<QwtIntervalSample> anIntervalsVect;
  QwtArray<QwtDoubleInterval> anIntervals( aSize );
  QwtArray<double> aValues( aSize );
  double aX;
  double aWidth = isAutoAssign() ? m_dDefWidth : m_dWidth;
  for ( int i = 0; i < aSize; i++ ) {
    aX = aPoints[i].x;
    //anIntervals[i] = QwtDoubleInterval( aX - aWidth/2, aX + aWidth/2 );
    //aValues[i] = aPoints[i].y;
    QwtIntervalSample anInt( aPoints[i].y, QwtDoubleInterval( aX - aWidth/2, aX + aWidth/2 ) );
    anIntervalsVect.append( anInt );
  }

  return QwtIntervalSeriesData( anIntervalsVect );
}

/**
 * Returns color of histogram
 */
QColor Plot2d_Histogram::getColor() const
{
  return m_Color;
}

/**
 * Sets color of histogram
 */
void Plot2d_Histogram::setColor( const QColor& theColor )
{
  m_Color = theColor;
  setAutoAssign( false );
}

/**
 * Returns width for a histogram bar
 */
double Plot2d_Histogram::getWidth( const bool& isDef ) const
{
  return isDef ? m_dDefWidth : m_dWidth;
}

/**
 * Sets width of a histogram bar
 */
void Plot2d_Histogram::setWidth( const double theWidth )
{
  m_dWidth = theWidth;
  setAutoAssign( false );
}

/**
 *
 */
QColor Plot2d_Histogram::getNextColor( const QwtPlot* thePlot )
{
  bool bOk = false;
  QColor aColor;
  while ( !bOk ) {
    int aRed    = (int)( 256.0 * rand() / RAND_MAX);  // generate random color
    int aGreen  = (int)( 256.0 * rand() / RAND_MAX);  // ...
    int aBlue   = (int)( 256.0 * rand() / RAND_MAX);  // ...
    aColor      = QColor( aRed, aGreen, aBlue );
    bOk = !existColor( thePlot, aColor );
  }
  return aColor;
}

/**
 *
 */
bool Plot2d_Histogram::existColor( const QwtPlot* thePlot, const QColor& theColor)
{
  QColor aColor = thePlot->palette().color( QPalette::Background );
  if ( closeColors( theColor, aColor ) )
      return true;
  QwtPlotItemList anItems = thePlot->itemList();
  QwtPlotItemIterator anIt = anItems.begin(), aLast = anItems.end();
  QwtPlotItem* anItem;
  for( ; anIt != aLast; anIt++ ) {
    anItem = *anIt;
    if ( !anItem )
      continue;
    if( anItem->rtti() == rtti() ) {
      Plot2d_HistogramItem* aHItem = dynamic_cast<Plot2d_HistogramItem*>
                                                  ( anItem );
      if ( aHItem && aColor == aHItem->color() )
        return true;
    }
    else if ( anItem->rtti() == QwtPlotItem::Rtti_PlotCurve ) {
      QwtPlotCurve* aCurve = dynamic_cast<QwtPlotCurve*>( anItem );
      if ( aCurve ) {
        if ( aCurve && aCurve->pen().color() == aColor )
          return true;
      }
    }
  }
  return false;
}

/**
 * Return min interval from values
 */
double Plot2d_Histogram::getMinInterval( const QList<double>& theVals )
{
  double aValue = -1;
  int aSize = theVals.size();
  if ( aSize < 2 )
    return aValue;

  aValue = fabs( theVals[1] - theVals[0] );
  double aDelta;
  for ( int i = 2; i < aSize; i++ ) {
    aDelta = fabs( theVals[i] - theVals[i-1] );
    if ( aDelta < aValue && aDelta )
      aValue = aDelta;
  }
  return aValue/2;
}

