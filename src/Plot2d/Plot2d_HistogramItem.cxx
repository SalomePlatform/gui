// Name   : Plot2d_HistogramItem.cpp
// Purpose: Implementation of Plot2d_HistogramItem class.
//
// History:
// 11/12/07 - Natalia Ermolaeva - Creation of the file

#include <Plot2d_HistogramItem.h>

#include <QFontMetrics>

#include <qstring.h>
#include <qpainter.h>
#include <qwt_plot.h>
#include <qwt_interval_data.h>
#include <qwt_painter.h>
#include <qwt_scale_map.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_symbol.h>
#include <qwt_plot_dict.h>


/**
 * Constructor
 */
Plot2d_HistogramQwtItem::Plot2d_HistogramQwtItem( const QwtText& theTitle )
: QwtPlotItem( theTitle )
{
  init();
}

/**
 * Constructor
 */
Plot2d_HistogramQwtItem::Plot2d_HistogramQwtItem( const QString& theTitle )
: QwtPlotItem( QwtText( theTitle ) )
{
  init();
}

/**
 * Destructor
 */
Plot2d_HistogramQwtItem::~Plot2d_HistogramQwtItem()
{
  delete m_pData;
}

/**
 * Initialization of object
 */
void Plot2d_HistogramQwtItem::init()
{
  m_pData = new PrivateData();
  m_pData->reference = 0.0;
  m_pData->attributes = Plot2d_HistogramQwtItem::Auto;

  setItemAttribute( QwtPlotItem::AutoScale, true );
  setItemAttribute( QwtPlotItem::Legend, true );

  setZ( 20.0 );
}
/**
 * Sets base line to object
 * @param theRef
 */
void Plot2d_HistogramQwtItem::setBaseline( double theRef )
{
  if ( m_pData->reference != theRef ) {
      m_pData->reference = theRef;
      itemChanged();
  }
}

/**
 * Returns base line of object
 */
double Plot2d_HistogramQwtItem::baseline() const
{
    return m_pData->reference;
}

/**
 * Sets data to object
 */
void Plot2d_HistogramQwtItem::setData( const QwtIntervalData& theData )
{
  m_pData->data = theData;
  itemChanged();
}

/**
 * Returns data from object
 */
const QwtIntervalData &Plot2d_HistogramQwtItem::data() const
{
  return m_pData->data;
}

/**
 * Sets color to object
 */
void Plot2d_HistogramQwtItem::setColor( const QColor& theColor )
{
  if ( m_pData->color != theColor ) {
    m_pData->color = theColor;
    itemChanged();
  }
}
/**
 * Returns color from object
 */
QColor Plot2d_HistogramQwtItem::color() const
{
  return m_pData->color;
}

/**
 * Returns bounding rect of object
 */
QwtDoubleRect Plot2d_HistogramQwtItem::boundingRect() const
{
  QwtDoubleRect aRect = m_pData->data.boundingRect();
  if ( !aRect.isValid() ) 
      return aRect;

  if ( m_pData->attributes & Xfy ) {
    aRect = QwtDoubleRect( aRect.y(), aRect.x(),
                           aRect.height(), aRect.width() );
    if ( aRect.left() > m_pData->reference ) 
      aRect.setLeft( m_pData->reference );
    else if ( aRect.right() < m_pData->reference ) 
      aRect.setRight( m_pData->reference );
  } 
  else {
    if ( aRect.bottom() < m_pData->reference ) 
      aRect.setBottom( m_pData->reference );
    else if ( aRect.top() > m_pData->reference ) 
      aRect.setTop( m_pData->reference );
  }
  return aRect;
}

/**
 * Returns type of plot object
 */
int Plot2d_HistogramQwtItem::rtti() const
{
    return QwtPlotItem::Rtti_PlotHistogram;
}
/**
 * Sets histogram attributes
 */
void Plot2d_HistogramQwtItem::setHistogramAttribute( HistogramAttribute theAttr,
                                                   bool isOn )
{
  if ( bool( m_pData->attributes & theAttr ) == isOn )
    return;

  if ( isOn )
    m_pData->attributes |= theAttr;
  else
    m_pData->attributes &= ~theAttr;

  itemChanged();
}

/**
 * Tests histogram attributes
 */
bool Plot2d_HistogramQwtItem::testHistogramAttribute(
                                         HistogramAttribute theAttr ) const
{
  return m_pData->attributes & theAttr;
}

/**
 * Draws histogram object
 */
void Plot2d_HistogramQwtItem::draw( QPainter *thePainter,
                                  const QwtScaleMap& theXMap, 
                                  const QwtScaleMap& theYMap,
                                  const QRect& ) const
{
  const QwtIntervalData &iData = m_pData->data;
  thePainter->setPen( QPen( m_pData->color ) );

  const int x0 = theXMap.transform( baseline() );
  const int y0 = theYMap.transform( baseline() );

  for ( int i = 0; i < (int)iData.size(); i++ ) {
    if ( m_pData->attributes & Plot2d_HistogramQwtItem::Xfy ) {
      const int x2 = theXMap.transform( iData.value( i ) );
      if ( x2 == x0 )
        continue;
      int y1 = theYMap.transform( iData.interval( i ).minValue() );
      int y2 = theYMap.transform( iData.interval( i ).maxValue() );
      if ( y1 > y2 )
        qSwap( y1, y2 );

      if ( i < (int)iData.size() - 2 ) {
        const int yy1 = theYMap.transform( iData.interval(i+1).minValue() );
        const int yy2 = theYMap.transform( iData.interval(i+1).maxValue() );
        if ( y2 == qwtMin( yy1, yy2 ) ) {
          const int xx2 = theXMap.transform( iData.interval(i+1).minValue() );
          if ( xx2 != x0 && ( ( xx2 < x0 && x2 < x0 ) ||
                              ( xx2 > x0 && x2 > x0 ) ) ) {
            // One pixel distance between neighboured bars
            y2++;
          }
        }
      }
      drawBar( thePainter, Qt::Horizontal, QRect( x0, y1, x2 - x0, y2 - y1 ) );
    }
    else {
      const int y2 = theYMap.transform( iData.value( i ) );
      if ( y2 == y0 )
        continue;
      int x1 = theXMap.transform( iData.interval( i ).minValue() );
      int x2 = theXMap.transform( iData.interval( i ).maxValue() );
      if ( x1 > x2 )
        qSwap( x1, x2 );

      if ( i < (int)iData.size() - 2 ) {
        const int xx1 = theXMap.transform( iData.interval(i+1).minValue() );
        const int xx2 = theXMap.transform( iData.interval(i+1).maxValue() );
        if ( x2 == qwtMin( xx1, xx2 ) ) {
          const int yy2 = theYMap.transform( iData.value(i+1) );
          if ( yy2 != y0 && ( ( yy2 < y0 && y2 < y0 ) ||
                              ( yy2 > y0 && y2 > y0 ) ) ) {
            // One pixel distance between neighboured bars
            x2--;
          }
        }
      }
      drawBar( thePainter, Qt::Vertical, QRect( x1, y0, x2 - x1, y2 - y0 ) );
    }
  }
}

/**
 * Draws bar of histogram
 */
void Plot2d_HistogramQwtItem::drawBar( QPainter* thePainter,
                                     Qt::Orientation,
                                     const QRect& theRect ) const
{
  thePainter->save();

  const QColor color( thePainter->pen().color() );
#if QT_VERSION >= 0x040000
  QRect r = theRect.normalized();
#else
  QRect r = theRect.normalize();
#endif

  const int factor = 125;
  const QColor light( color.light( factor ) );
  const QColor dark( color.dark( factor ) );

  thePainter->setBrush( color );
  thePainter->setPen( Qt::NoPen );
  QwtPainter::drawRect( thePainter, r.x() + 1, r.y() + 1,
                        r.width() - 2, r.height() - 2 );
  thePainter->setBrush( Qt::NoBrush );

  thePainter->setPen( QPen( light, 2 ) );
#if QT_VERSION >= 0x040000
  QwtPainter::drawLine( thePainter, r.left() + 1, r.top() + 2,
                        r.right() + 1, r.top() + 2 );
#else
  QwtPainter::drawLine( thePainter, r.left(), r.top() + 2,
                        r.right() + 1, r.top() + 2 );
#endif

  thePainter->setPen( QPen( dark, 2 ) );
#if QT_VERSION >= 0x040000
  QwtPainter::drawLine( thePainter, r.left() + 1, r.bottom(),
                        r.right() + 1, r.bottom() );
#else
  QwtPainter::drawLine( thePainter, r.left(), r.bottom(),
                        r.right() + 1, r.bottom());
#endif
  thePainter->setPen( QPen( light, 1 ) );

#if QT_VERSION >= 0x040000
  QwtPainter::drawLine( thePainter, r.left(), r.top() + 1,
                        r.left(), r.bottom() );
  QwtPainter::drawLine( thePainter, r.left() + 1, r.top() + 2,
                         r.left() + 1, r.bottom() - 1 );
#else
  QwtPainter::drawLine( thePainter, r.left(), r.top() + 1,
                        r.left(), r.bottom() + 1 );
  QwtPainter::drawLine( thePainter, r.left() + 1, r.top() + 2,
                        r.left() + 1, r.bottom() );
#endif
  thePainter->setPen( QPen( dark, 1 ) );

#if QT_VERSION >= 0x040000
  QwtPainter::drawLine( thePainter, r.right() + 1, r.top() + 1,
                        r.right() + 1, r.bottom() );
  QwtPainter::drawLine(thePainter, r.right(), r.top() + 2,
                        r.right(), r.bottom() - 1 );
#else
  QwtPainter::drawLine( thePainter, r.right() + 1, r.top() + 1,
                        r.right() + 1, r.bottom() + 1 );
  QwtPainter::drawLine( thePainter, r.right(), r.top() + 2,
                        r.right(), r.bottom() );
#endif
  thePainter->restore();
}

/**
 * Constructor
 */
Plot2d_HistogramItem::Plot2d_HistogramItem( const QwtText& theTitle )
: Plot2d_HistogramQwtItem( theTitle ), m_bCrossed( true )
{
}

/**
 * Constructor
 */
Plot2d_HistogramItem::Plot2d_HistogramItem( const QString& theTitle )
: Plot2d_HistogramQwtItem( theTitle ), m_bCrossed( true )
{
}

/**
 * Destructor
 */
Plot2d_HistogramItem::~Plot2d_HistogramItem()
{
}

/**
 * Set to legend item symbol with color of item
 */
void Plot2d_HistogramItem::updateLegend( QwtLegend* theLegend ) const
{
  if ( !theLegend )
    return;
  QwtPlotItem::updateLegend( theLegend );
  QWidget* theWidget = theLegend->find( this );
  if ( !theWidget || !theWidget->inherits( "QwtLegendItem" ) )
    return;
  QwtLegendItem* anItem = ( QwtLegendItem* )theWidget;
  QFontMetrics aFMetrics( anItem->font() );
  int aSize = aFMetrics.height();
  QwtSymbol aSymbol( QwtSymbol::Rect, QBrush( color() ),
                     QPen( color() ), QSize( aSize, aSize ) );
  anItem->setSymbol( aSymbol );
  anItem->setIdentifierMode( theLegend->identifierMode()
                           | QwtLegendItem::ShowSymbol ); 
  anItem->update();
}

/**
 * Draws histogram object
 */
void Plot2d_HistogramItem::draw( QPainter *thePainter,
                                  const QwtScaleMap& theXMap, 
                                  const QwtScaleMap& theYMap,
                                  const QRect& ) const
{
  // nds: clear list of bar items
  Plot2d_HistogramItem* anItem = (Plot2d_HistogramItem*)this;
  anItem->m_BarItems.clear();

  const QwtIntervalData &iData = m_pData->data;
  thePainter->setPen( QPen( m_pData->color ) );
  const int x0 = theXMap.transform( baseline() );
  const int y0 = theYMap.transform( baseline() );

  for ( int i = 0; i < (int)iData.size(); i++ ) {
    if ( m_pData->attributes & Plot2d_HistogramItem::Xfy ) {
      const int x2 = theXMap.transform( iData.value( i ) );
      if ( x2 == x0 )
        continue;
      int y1 = theYMap.transform( iData.interval( i ).minValue() );
      int y2 = theYMap.transform( iData.interval( i ).maxValue() );
      if ( y1 > y2 )
        qSwap( y1, y2 );

      if ( i < (int)iData.size() - 2 ) {
        const int yy1 = theYMap.transform( iData.interval(i+1).minValue() );
        const int yy2 = theYMap.transform( iData.interval(i+1).maxValue() );
        if ( y2 == qwtMin( yy1, yy2 ) ) {
          const int xx2 = theXMap.transform( iData.interval(i+1).minValue() );
          if ( xx2 != x0 && ( ( xx2 < x0 && x2 < x0 ) ||
                              ( xx2 > x0 && x2 > x0 ) ) ) {
            // One pixel distance between neighboured bars
            y2++;
          }
        }
      }
      // nds: draw rect with the other lower rects
      QRect aRect( x0, y1, x2 - x0, y2 - y1 );
      drawRectAndLowers( thePainter, Qt::Horizontal, aRect );
      anItem->m_BarItems.append( aRect );
    }
    else {
      const int y2 = theYMap.transform( iData.value( i ) );
      if ( y2 == y0 )
        continue;
      int x1 = theXMap.transform( iData.interval( i ).minValue() );
      int x2 = theXMap.transform( iData.interval( i ).maxValue() );
      if ( x1 > x2 )
        qSwap( x1, x2 );

      if ( i < (int)iData.size() - 2 ) {
        const int xx1 = theXMap.transform( iData.interval(i+1).minValue() );
        const int xx2 = theXMap.transform( iData.interval(i+1).maxValue() );
        if ( x2 == qwtMin( xx1, xx2 ) ) {
          const int yy2 = theYMap.transform( iData.value(i+1) );
          if ( yy2 != y0 && ( ( yy2 < y0 && y2 < y0 ) ||
                              ( yy2 > y0 && y2 > y0 ) ) ) {
            // One pixel distance between neighboured bars
            x2--;
          }
        }
      }
      // nds: draw rect with the other lower rects
      QRect aRect(x1, y0, x2 - x1, y2 - y0 );
      drawRectAndLowers( thePainter, Qt::Vertical, aRect );
      anItem->m_BarItems.append( aRect );
    }
  }
}

/**
 * Draws bar of histogram and on it bars of histograms with lower height.
 */
void Plot2d_HistogramItem::drawRectAndLowers( QPainter* thePainter,
                                               Qt::Orientation theOr,
                                               const QRect& theRect ) const
{
  QRect aRect = theRect;
  // theRect has inversed coordinates on Y axis. The top of the rect is bottom in standard QRect coordinates, and it bottom is the top.
  if ( m_bCrossed && theOr == Qt::Horizontal )
    aRect.setTop( getCrossedTop( theRect ) );

  drawBar( thePainter, Qt::Horizontal, aRect );
}

/**
 * Returns top value of the given rect in the context of other bars.
 * It's necessary to remember, that theRecs has inverted coordinate Y.
 */
int Plot2d_HistogramItem::getCrossedTop( const QRect& theRect ) const
{
  int aRes = theRect.top();
  QwtPlot* aPlot = plot();
  int aHeight = theRect.height();
  if ( aPlot ) {
    QwtPlotItemList anItems = aPlot->itemList();
    QwtPlotItemIterator anIt = anItems.begin(), aLast = anItems.end();
    Plot2d_HistogramItem* anItem;
    QList<QRect> aRects;
    for ( ; anIt != aLast; anIt++ ) {
      if ( !(*anIt)->rtti() == QwtPlotItem::Rtti_PlotHistogram )
        continue;
      anItem = dynamic_cast<Plot2d_HistogramItem*>( *anIt );
      if( !anItem || anItem == this )
        continue;
      aRects.clear();
      aRects = anItem->getBars();
      for ( int i = 0, aSize = aRects.size(); i < aSize; i++ ) {
        if ( qMax( theRect.x(), aRects[i].x() ) <=
             qMin( theRect.left(), aRects[i].left() ) ) {
          if ( theRect.bottom() < aRects[i].bottom() )
            if ( aRects[i].bottom() < aRes )
	      aRes = aRects[i].bottom();
        }
      }
    }
  }
  return aRes;
}

