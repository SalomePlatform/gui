
#include "Plot2d_Legend.h"

#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_dyngrid_layout.h>

#include <QGridLayout>
#include <QLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QEvent>
#include <QMouseEvent>

/**
  Constructor
  @param thePlot the plot that the axis should be assigned to
*/
Plot2d_Legend::Plot2d_Legend( QwtPlot* thePlot )
: QwtLegend( thePlot ),
  myIsEnabled( false ),
  myIsOnCanvas( false ),
  myPosition( Bottom )
{
  init();
  setPlot( thePlot );
  QwtPlotCanvas* aCanvas = myPlot->canvas();
  if ( aCanvas )
    aCanvas->installEventFilter( this );
}

/**
  Destructor
*/
Plot2d_Legend::~Plot2d_Legend()
{
}

/**
  Fill the initial state of the legend
*/
void Plot2d_Legend::init()
{
  setFrameStyle( QFrame::Box );
  setWindowTitle( tr( "LEGEND_TLT" ) );
}

/**
  Return the legend position type
  @return the legend position type
*/
Plot2d_Legend::LegendPosition Plot2d_Legend::getPositionType() const
{
  return myPosition;
}

/**
  Set the legend position type
  @param thePositionType legend position type
*/
void Plot2d_Legend::setPositionType( const LegendPosition& thePositionType )
{
  myPosition = thePositionType;
}

/**
  Set the legend position type (Embedded or external)
  @param thePositionType legend position type
*/
void Plot2d_Legend::setOnCanvas( bool isOnCanvas )
{
  myIsOnCanvas = isOnCanvas;
}

/**
  Return true if the legend is shown on plot2d
  @return true if the legend shown
*/
bool Plot2d_Legend::isEnabled() const
{
  return myIsEnabled;
}

/**
  Show or hide legend
  @param isEnable if true the legend will be shown
  @param isUpdate if true the plot2d will be updated
*/
void Plot2d_Legend::setEnabled( bool isEnable )
{
  myIsEnabled = isEnable;
  if( myPlot )
  {
    if ( myIsEnabled )
      insertLegend();
    else if ( myIsOnCanvas ) {
      hide();
    } else {
      myPlot->insertLegend( 0 );
    }
  }
}


void Plot2d_Legend::setPlot( QwtPlot* thePlot )
{
  if( myPlot )
    disconnect( myPlot );
  if( thePlot != NULL )
    connect( thePlot, SIGNAL(destroyed(QObject*)), this, SLOT(onPlotDestroyed() ) );
  myPlot = thePlot;
}

/**
  SLOT: called when registered plot is destroyed
*/
void Plot2d_Legend::onPlotDestroyed()
{
  setPlot(NULL);
  deleteLater();
}

/*!
  Adjust contents widget and item layout to the size of the viewport().
*/
void Plot2d_Legend::layoutContents()
{
  QWidget* aContents = contentsWidget();
  if ( !aContents )
    return;

  // the qwt functionality of calculation the content of the legend use visible size and the legend could not be
  // less than the visible size, so the legend's size is grown but can not be reduced. The behaviour is corrected here.
  QSize aPrevSize = size();
  const QLayout* aLay = aContents->layout();
  if ( aLay && aLay->inherits( "QwtDynGridLayout" ) ) {
    const QwtDynGridLayout* aDynLay = ( const QwtDynGridLayout* )aLay;
    int aWidth = int( aDynLay->maxItemWidth() ) + 2*aDynLay->margin();
    QSize aSize = QSize( aWidth, aDynLay->heightForWidth( aWidth ) );
    aContents->resize( aSize );

    if ( !aSize.isEmpty() )
      aSize = correctContentSize( aSize );
    resize( aSize );
    updateLegendPosition();
  }
}

/**
  Return if the given point belongs to axis
  @param thePoint the point to be checked
  @return if point belongs to axis
*/
bool Plot2d_Legend::pick( const QPoint& thePoint ) const
{
  if( !myPlot || !isEnabled() )
    return false;

  return frameRect().contains( thePoint );
}

/**
  Return the parent class for the legend to be attached to in the standard position type
  @return legend font
*/
QWidget* Plot2d_Legend::parentToAttach() const
{
  return myPlot ? myPlot->canvas() : NULL;
}

/**
  Insert legend in the plot widget
*/
void Plot2d_Legend::insertLegend()
{
  if( !myPlot )
    return;

  //It's workaround - the legend should totaly rebuilded
  //But if the plot is the same the legend doesn't rebuilded
  if( myIsOnCanvas ) {
    myPlot->insertLegend( this, QwtPlot::ExternalLegend );
    setParent( parentToAttach() );
    layoutContents();
    updateLegendPosition();
    show();
  } else {
    setParent( myPlot );
    myPlot->insertLegend( this, qwtLegendPosition() );
  }
}

/**
  Update legend position taking into account the position type
*/
void Plot2d_Legend::updateLegendPosition()
{
  if ( !parentWidget() )
    return;

  QRect aRect = parentWidget()->contentsRect();
  int aX = aRect.x();
  int anY = aRect.y();
  switch( myPosition ) {
    case TopLeft:
    break;
    case BottomLeft:
      anY = anY + aRect.height() - height();
    break;
    case Top:
    case Bottom: {
      aX = aX + aRect.width()/2 - width()/2;
      if ( myPosition == Bottom )
        anY = anY + aRect.height() - height();
    }
    break;
    case TopRight:
    case BottomRight: {
      aX = aX + aRect.width() - width();
      if ( myPosition == BottomRight )
        anY = anY + aRect.height() - height();
    }
    break;
    case Left:
    case Right: {
      anY = anY + aRect.height()/2 - height()/2;
      if ( myPosition == Right )
        aX = aX + aRect.width() - width();
      }
    break;
    default:
    break;
  }
  move( aX, anY );
}

/**
  Correct the content size according to the parent widget size
  @param theSize the size to be corrected
  @return the rectangle
*/
QSize Plot2d_Legend::correctContentSize( const QSize& theSize ) const
{
  if ( !myPlot || theSize.width() <=0 || theSize.height() <= 0 )
    return theSize;

  QWidget* aCont = const_cast<QWidget*>( contentsWidget() );
  if ( !myPlot || !aCont )
    return theSize;

  int aWidth = theSize.width();
  int aHeight = theSize.height();

  if ( parentWidget() ) {
    int aCw = theSize.width();
    int aCh = theSize.height();

    QRect aBoundRect = parentWidget()->contentsRect();
    int aPw = aBoundRect.width();
    int aPh = aBoundRect.height();

    aWidth = qMin( aPw, aCw );
    aHeight = qMin( aPh, aCh );

    // correction the legend width
    if ( aPw > aCw ) {
      aWidth = aCw;
      if ( aCh > aPh ) {
        QScrollBar* aScroll = new QScrollBar( Qt::Horizontal );
        aWidth += aScroll->sizeHint().height();
        aHeight = aPh;
        delete aScroll;
      }
    }
    // correction the legend height
    if ( aPh > aCh ) {
      aHeight = aCh;
      if ( aCw > aPw ) {
        QScrollBar* aScroll = new QScrollBar( Qt::Vertical );
        aHeight += aScroll->sizeHint().width();
        aWidth = aPw;
        delete aScroll;
      }
    }
  }
  int aFrameW = 2*frameWidth();
  aWidth += aFrameW;
  aHeight += aFrameW;

  return QSize( aWidth, aHeight );
}

QwtPlot::LegendPosition Plot2d_Legend::qwtLegendPosition()
{
  QwtPlot::LegendPosition result;
  switch ( myPosition ) {
    case Plot2d_Legend::Left:
      result = QwtPlot::LeftLegend;
      break;
    case Plot2d_Legend::Right:
      result = QwtPlot::RightLegend;
      break;
    case Plot2d_Legend::Top:
      result = QwtPlot::TopLegend;
      break;
    case Plot2d_Legend::Bottom:
      result = QwtPlot::BottomLegend;
      break;
    default:
      result = QwtPlot::BottomLegend;
      break;
  }
  return result;
}

/**
  Reimplementation of event filter (cavas actions process to calculate position of the legend)
  @param theObj the object whose events are handled
  @param theEvent the event instance
  @return if event if handled by this event filter
*/
bool Plot2d_Legend::eventFilter( QObject *theObj, QEvent *theEvent )
{
  bool isProcessed = QwtLegend::eventFilter( theObj, theEvent );
  if ( myPlot && myIsOnCanvas && theObj == parentToAttach() ) {
    if ( theEvent->type() == QEvent::Resize ) {
      layoutContents();
      updateLegendPosition();
    }
  }
  return isProcessed;
}
