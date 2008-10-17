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
// File:      QtxTable.cxx
// Author:    Sergey TELKOV

#include "QtxTable.h"

#ifndef QT_NO_TABLE

//#include "QtxStyleWrap.h"

#include <QStyle>
#include <QPainter>
#include <QTextEdit>
#include <QLineEdit>
//#include <qmemarray.h>
#include <QToolButton>
//#include <QStyleSheet>
#include <QApplication>
#include <QHeaderView>
#include <QObject>
#include <QAbstractItemModel>
//#include <qsimplerichtext.h>

/*!
  Class QtxTable::Header
  Internal header for QtxTable
*/

/*class QtxTable::Header : public QHeaderView
{
public:
  Header( int, QtxTable*, const char* = 0 );
  virtual ~Header();

  QtxTable*     table() const;

  void          clear();

  QColor        foregroundColor( const int ) const;
  QColor        backgroundColor( const int ) const;
  void          setForegroundColor( const int, const QColor& );
  void          setBackgroundColor( const int, const QColor& );

  void          removeSection( const int );
  virtual void  setLabel( int, const QString&, int = -1 );

  QRect         indexRect( const int, int* = 0 );
  void          swapSections( const int, const int );
  int           spanedSection( const int index );

  virtual QSize sizeHint() const;

protected:
  virtual void  mouseMoveEvent( QMouseEvent* );
  virtual void  mousePressEvent( QMouseEvent* );
  virtual void  mouseReleaseEvent( QMouseEvent* );
  virtual void  paintSection( QPainter*, int, const QRect& );
  virtual void  paintEvent( QPaintEvent * );

private:
  typedef QMap<int, QColor>     ColorMap;

private:
  QHeaderView*      mainHeader() const;
  bool          filterEvent( QMouseEvent* );

  QPoint        sectionCenter( const int, const QPoint& = QPoint() ) const;

private:
  QtxTable*     myTable;
  ColorMap      myFgColor;
  ColorMap      myBgColor;
  int           myPressed;
  int           mySection;
  friend class QtxTable::StyleItem;
};

QtxTable::Header::Header( int n, QtxTable* table, const char* name )
: QHeaderView( n, table, name ),
myTable( table ),
myPressed( -1 ),
mySection( -1 )
{
  setWFlags( WRepaintNoErase );
}

QtxTable::Header::~Header()
{
}

QtxTable* QtxTable::Header::table() const
{
  return myTable;
}

void QtxTable::Header::clear()
{
  for ( int i = 0; i < (int)count(); i++ )
    setLabel( i, orientation() == Qt::Horizontal ? QString( "" ) : QString::null );
  myFgColor.clear();
  myBgColor.clear();
}

QSize QtxTable::Header::sizeHint() const
{
  QSize sz = QHeaderView::sizeHint();

  int size = orientation() == Qt::Horizontal ? sz.height() : sz.width();

  for ( int i = 0; i < (int)count(); i++ )
  {
    QString lab = label( mapToSection( i ) );
    if ( !QStyleSheet::mightBeRichText( lab ) )
      continue;

    int s = 0;
    QIconSet* icon = iconSet( mapToSection( i ) );
    if ( icon )
    {
	    QPixmap pixmap = icon->pixmap( QIconSet::Small, QIconSet::Normal );
      s = orientation() == Qt::Horizontal ? pixmap.height() : pixmap.width();
    }

    QStyleSheet sheet;
    QStyleSheetItem* item = sheet.item( "p" );
    if ( item )
    {
      item->setMargin( QStyleSheetItem::MarginAll, 0 );
      item->setWhiteSpaceMode( QStyleSheetItem::WhiteSpaceNoWrap );
    }

    QSimpleRichText rt( QString( "<p>%1</p>" ).arg( lab ), font(), QString::null, &sheet );
    if ( orientation() == Qt::Horizontal )
      rt.setWidth( sectionSize( mapToSection( i ) ) );
    s += orientation() == Qt::Horizontal ? rt.height() : rt.width();
 
    size = QMAX( size, s );
  }

  if ( orientation() == Qt::Horizontal )
    sz.setHeight( size );
  else
    sz.setWidth( size );

  return sz;
}

void QtxTable::Header::removeSection( const int section )
{
  int old = count();

  removeLabel( section );

  if ( old == count() )
    return;

  ColorMap newFMap, newBMap;
  for ( ColorMap::ConstIterator fIt = myFgColor.begin(); fIt != myFgColor.end(); ++fIt )
  {
    if ( fIt.key() == section )
      continue;
    newFMap.insert( fIt.key() < section ? fIt.key() : fIt.key() - 1, fIt.data() );
  }
  myFgColor = newFMap;

  for ( ColorMap::ConstIterator bIt = myBgColor.begin(); bIt != myBgColor.end(); ++bIt )
  {
    if ( bIt.key() == section )
      continue;
    newBMap.insert( bIt.key() < section ? bIt.key() : bIt.key() - 1, bIt.data() );
  }
  myBgColor = newBMap;
}

void QtxTable::Header::setLabel( int section, const QString& s, int size )
{
  QHeaderView::setLabel( section, s, size );
  if ( table() && isUpdatesEnabled() )
    table()->updateHeaderGeometries( orientation() );
}

QColor QtxTable::Header::foregroundColor( const int section ) const
{
  return myFgColor.contains( section ) ? myFgColor[section] : QColor();
}

QColor QtxTable::Header::backgroundColor( const int section ) const
{
  return myBgColor.contains( section ) ? myBgColor[section] : QColor();
}

void QtxTable::Header::setForegroundColor( const int section, const QColor& c )
{
  if ( foregroundColor( section ) == c )
    return;

  myFgColor.insert( section, c );
  if ( isUpdatesEnabled() )
    repaint( indexRect( mapToIndex( section ) ) );
}

void QtxTable::Header::setBackgroundColor( const int section, const QColor& c )
{
  if ( backgroundColor( section ) == c )
    return;

  myBgColor.insert( section, c );
  if ( isUpdatesEnabled() )
    repaint( indexRect( mapToIndex( section ) ) );
}

void QtxTable::Header::swapSections( const int oldIdx, const int newIdx )
{
  QIconSet oldIconSet, newIconSet;
  if ( iconSet( oldIdx ) )
	  oldIconSet = *iconSet( oldIdx );
  if ( iconSet( newIdx ) )
	  newIconSet = *iconSet( newIdx );
  QString oldLabel = label( oldIdx );
  QString newLabel = label( newIdx );
  QHeaderView::setLabel( oldIdx, newIconSet, newLabel );
  QHeaderView::setLabel( newIdx, oldIconSet, oldLabel );

  QColor oldFg = foregroundColor( oldIdx );
  QColor newFg = foregroundColor( newIdx );
  setForegroundColor( oldIdx, newFg );
  setForegroundColor( newIdx, oldFg );

  QColor oldBg = backgroundColor( oldIdx );
  QColor newBg = backgroundColor( newIdx );
  setBackgroundColor( oldIdx, newBg );
  setBackgroundColor( newIdx, oldBg );
}

void QtxTable::Header::mouseMoveEvent( QMouseEvent* e )
{
  if ( e->button() == NoButton && filterEvent( e ) )
    return;

  QHeaderView::mouseMoveEvent( e );
  if ( mainHeader() )
    QApplication::sendEvent( mainHeader(), e );
}

void QtxTable::Header::mousePressEvent( QMouseEvent* e )
{
  if ( filterEvent( e ) )
    return;

  int index = mapToIndex( sectionAt( ( orientation() == Horizontal ? e->pos().x() : e->pos().y() ) + offset() ) );
  SpanRange range = findSpanRange( index );

  bool shift = e->state() & ShiftButton;
  if ( mainHeader() )
    QApplication::sendEvent( mainHeader(), e );

  for ( int i = range.second; i >= range.first && !shift; i-- )
  {
    QPoint pos = sectionCenter( i, e->pos() );
    QMouseEvent me( e->type(), pos, e->button(), e->state() | ControlButton );
    if ( i == range.first )
      QHeaderView::mousePressEvent( &me );
    if ( mainHeader() )
      QApplication::sendEvent( mainHeader(), &me );
  }

  if ( !shift )
  {
    QMouseEvent me( e->type(), sectionCenter( index, e->pos() ), e->button(), e->state() | ControlButton );
    if ( mainHeader() )
      QApplication::sendEvent( mainHeader(), &me );
  }

  myPressed = range.first;
  repaint( indexRect( range.first ) );
}

void QtxTable::Header::mouseReleaseEvent( QMouseEvent* e )
{
  QHeaderView::mouseReleaseEvent( e );
  if ( mainHeader() )
    QApplication::sendEvent( mainHeader(), e );

  repaint( indexRect( myPressed ) );
  myPressed = -1;
}

void QtxTable::Header::paintSection( QPainter* p, int index, const QRect& fr )
{
  int idx = index;
  QRect r = fr;
  if ( index < count() )
    r = indexRect( index, &idx );

  // Currently painted section number. This number will be used in the drawing style.
  // Because QHeaderView::paintSection() implementation doesn't send section id through style options :(
  mySection = mapToSection( index );

  QHeaderView::paintSection( p, idx, r );

  mySection = -1;
}

void QtxTable::Header::paintEvent( QPaintEvent *pe )
{
  QRect r = rect();
  QPixmap pix( r.width(), r.height() );
  QPainter::redirect( this, &pix );
  QHeaderView::paintEvent( pe );
  QPainter::redirect( this, 0 );
  QPainter painter( this );
  painter.drawPixmap( r, pix );
  //QHeaderView::paintEvent( pe );
}

bool QtxTable::Header::filterEvent( QMouseEvent* e )
{
  int c = orientation() == Qt::Horizontal ? e->pos().x() : e->pos().y();
  c += offset();
/*
  if ( reverse() )
	  c = d->lastPos - c;
*+/
  int section = sectionAt( c );
  if ( section < 0 )
    return false;

  int handleIdx = -1;
  int margin = style().pixelMetric( QStyle::PM_HeaderGripMargin );
  int index = mapToIndex( section );

  QRect r = sectionRect( section );
  int size = sectionSize( section );
  int pos = ( orientation() == Qt::Horizontal ? r.x() : r.y() ) + offset();
  if ( c < pos + margin )
	  handleIdx = index - 1;
  else if ( c > pos + size - margin )
	  handleIdx = index;

  if ( handleIdx == -1 )
    return false;

  return findSpanRange( handleIdx ).second != handleIdx;
}

QPoint QtxTable::Header::sectionCenter( const int index, const QPoint& p ) const
{
  QPoint pos = p;
  if ( orientation() == Qt::Horizontal )
    pos.setX( sectionPos( mapToSection( index ) ) + sectionSize( mapToSection( index ) ) / 2 - offset() );
  else
    pos.setY( sectionPos( mapToSection( index ) ) + sectionSize( mapToSection( index ) ) / 2 - offset() );

  if ( p.isNull() )
  {
    if ( orientation() == Qt::Horizontal )
      pos.setY( height() / 2 );
    else
      pos.setX( width() / 2 );
  }
  return pos;
}

QtxTable::Header::SpanRange QtxTable::Header::findSpanRange( const int index )
{
  SpanRange res( index, index );
  if ( mySpanRanges.isEmpty() )
  {
    // no span sections and no first simple sections stored in map
    // so we have to fill it.
    for ( int i = 0; i < (int)count(); i++ )
    {
      int sp = horizontalSpan( mapToSection( i ) );
      sp = QMAX( sp, 1 );
      SpanRange range( i, QMIN( i + sp - 1, count() - 1 ) );
      if ( range.first == range.second && !mySpanRanges.isEmpty() )
        continue; // do not store simple section without span (store only first)
      mySpanRanges.insert( i, range );
      for ( int j = i + 1; j < i + sp; j++ )
        mySpanRanges.insert( j, range );
      i += sp - 1;
    }
  }
  
  if ( mySpanRanges.contains( index ) )
    res = mySpanRanges[ index ];


  /* try to optimise by checking range during step from one to next
  SpanRangeList ranges;
  spanRanges( ranges );

  for ( SpanRangeList::const_iterator it = ranges.begin(); it != ranges.end() && res.first < 0; ++it )
  {
    if ( (*it).first <= index && index <= (*it).second )
      res = *it;
  }
  *+/
  for ( int i = 0; i < (int)count() && res.first < 0; i++ )
  {
    int sp = horizontalSpan( mapToSection( i ) );
    sp = QMAX( sp, 1 );
    SpanRange range( i, QMIN( i + sp - 1, count() - 1 ) );
    if ( range.first <= index && index <= range.second )
      res = range;
    i += sp - 1;
  }
  return res;
}

QRect QtxTable::Header::indexRect( const int index, int* start )
{
  SpanRange range = findSpanRange( index );

  if ( start )
    *start = range.first;

  QRect r;
  for ( int i = range.first; i <= range.second; i++ )
    r = r.unite( sRect( i ) );
  return r;
}

int QtxTable::Header::spanedSection( const int index )
{
  SpanRange range = findSpanRange( index );
  return range.first;
}

QHeaderView* QtxTable::Header::mainHeader() const
{
  if ( !table() )
    return 0;

  return orientation() == Qt::Horizontal ? table()->horizontalHeader() :
                                       table()->verticalHeader();
}
*/
/*!
  Class QtxTable::StyleItem
*/
/*
class QtxTable::StyleItem : public QtxStyleWrapItem
{
public:
  StyleItem( QtxStyleWrap* );
  ~StyleItem();
  virtual bool drawControl( QStyle::ControlElement, QPainter*, const QWidget*, const QRect&,
                            const QColorGroup&, QStyle::SFlags, const QStyleOption& ) const;
  virtual bool drawPrimitive( QStyle::PrimitiveElement, QPainter*, const QRect&,
                              const QColorGroup&, QStyle::SFlags, const QStyleOption& ) const;
};

QtxTable::StyleItem::StyleItem( QtxStyleWrap* wrap )
: QtxStyleWrapItem( wrap )
{
}

QtxTable::StyleItem::~StyleItem()
{
}

bool QtxTable::StyleItem::drawControl( QStyle::ControlElement element, QPainter* p, const QWidget* widget,
                                       const QRect& r, const QColorGroup& cg, QStyle::SFlags flags,
                                       const QStyleOption& opt ) const
{
  if ( element != QStyle::CE_HeaderLabel )
    return false;

	const QHeaderView* header = (const QHeaderView*)widget;
	int section = opt.headerSection();

  QColor fc;
  QTableWidget* table = ::qobject_cast<QTableWidget*>( header->parent() );
  if ( table && table->verticalHeader() != header && table->horizontalHeader() != header )
    fc = ((Header*)header)->foregroundColor( section );

  QColorGroup grp( cg );
  if ( fc.isValid() )
    grp.setColor( QColorGroup::ButtonText, fc );
  grp.setColor( QColorGroup::Text, grp.buttonText() );

  QString lab = header->label( section );
  if ( !QStyleSheet::mightBeRichText( lab ) )
  {
    if ( style() )
		  style()->drawControl( element, p, widget, r, grp, flags, opt );
  }
  else
  {
    QRect rect = r;
    QIconSet* icon = header->iconSet( section );
    if ( icon )
    {
	    QPixmap pixmap = icon->pixmap( QIconSet::Small, flags & QStyle::Style_Enabled ? QIconSet::Normal :
                                                                                      QIconSet::Disabled );
      int pixw = pixmap.width();
		  int pixh = pixmap.height();

		  QRect pixRect = rect;
		  pixRect.setY( rect.center().y() - (pixh - 1) / 2 );
      if ( style() )
		    style()->drawItem( p, pixRect, AlignVCenter, cg, flags & QStyle::Style_Enabled, &pixmap, QString::null );
      rect.setLeft( rect.left() + pixw + 2 );
    }

    QStyleSheet sheet;
    QStyleSheetItem* i = sheet.item( "p" );
    if ( i )
    {
      i->setMargin( QStyleSheetItem::MarginAll, 0 );
      i->setWhiteSpaceMode( QStyleSheetItem::WhiteSpaceNoWrap );
    }

    QSimpleRichText rt( QString( "<p>%1</p>" ).arg( lab ), header->font(), QString::null, &sheet );
    rt.setWidth( rect.width() );
    rt.draw( p, rect.x(), rect.y() + ( rect.height() - rt.height() ) / 2, rect, grp );
  }

  return true;
}

bool QtxTable::StyleItem::drawPrimitive( QStyle::PrimitiveElement pe, QPainter* p, const QRect& r,
                                         const QColorGroup& cg, QStyle::SFlags flags, const QStyleOption& opt ) const
{
  if ( pe != QStyle::PE_HeaderSection )
    return false;

  if ( opt.isDefault() )
    return false;

  QHeaderView* hdr = ::qobject_cast<QHeaderView*>( opt.widget() );
  if ( !hdr )
    return false;

  QTableWidget* table = ::qobject_cast<QTableWidget*>( hdr->parent() );
  if ( table && ( table->verticalHeader() == hdr || table->horizontalHeader() == hdr ) )
    return false;

  Header* h = (Header*)hdr;
  int section = h->mySection;

  if ( section < 0 )
    return false;

  QColorGroup grp( cg );
  QColor c = h->backgroundColor( section );
  if ( c.isValid() )
  {
    grp.setColor( QColorGroup::Button, c );
    grp.setColor( QColorGroup::Light, c );
  }
  if ( style() )
    style()->drawPrimitive( pe, p, r, grp, flags, opt );

  return true;
}
*/
/*!
  Class QtxTable
*/

/*!
  Constructor
*/
QtxTable::QtxTable( QWidget* parent, const char* name )
  : QTableWidget( parent )//,
  //myStyleWrapper( 0 ),
  //myHeaderEditor( 0 ),
  //myEditedHeader( 0 ),
  //myEditedSection( -1 ),
  //mySelectAll( 0 )
{
  setObjectName( name );

  /*myVerHeaders.setAutoDelete( true );
  myHorHeaders.setAutoDelete( true );

  verticalHeader()->setStyle( styleWrapper() );
  horizontalHeader()->setStyle( styleWrapper() );

  connect( verticalHeader(), SIGNAL( sizeChange( int, int, int ) ),
           this, SLOT( onHeaderSizeChange( int, int, int ) ) );
  connect( horizontalHeader(), SIGNAL( sizeChange( int, int, int ) ),
           this, SLOT( onHeaderSizeChange( int, int, int ) ) );
  connect( verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onScrollBarMoved( int ) ) );
  connect( horizontalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onScrollBarMoved( int ) ) );

  updateHeaders( Qt::Horizontal );
  updateHeaders( Qt::Vertical );
  */
}

/*!
  Constructor
*/
QtxTable::QtxTable( int numRows, int numCols, QWidget* parent, const char* name )
: QTableWidget( numRows, numCols, parent )//,
  //myStyleWrapper( 0 ),
  //myHeaderEditor( 0 ),
  //myEditedHeader( 0 ),
  //myEditedSection( -1 ),
  //mySelectAll( 0 )
{
  setObjectName( name );
  /*
  myVerHeaders.setAutoDelete( true );
  myHorHeaders.setAutoDelete( true );

  verticalHeader()->setStyle( styleWrapper() );
  horizontalHeader()->setStyle( styleWrapper() );

  connect( verticalHeader(), SIGNAL( sizeChange( int, int, int ) ),
           this, SLOT( onHeaderSizeChange( int, int, int ) ) );
  connect( horizontalHeader(), SIGNAL( sizeChange( int, int, int ) ),
           this, SLOT( onHeaderSizeChange( int, int, int ) ) );
  connect( verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onScrollBarMoved( int ) ) );
  connect( horizontalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onScrollBarMoved( int ) ) );
  */
}

/*!
  Destructor
*/
QtxTable::~QtxTable()
{
  //delete myStyleWrapper;
}

bool QtxTable::isSelectAllEnabled() const
{
  return true;//mySelectAll;
}

void QtxTable::setSelectAllEnabled( const bool on )
{
  /*  if ( isSelectAllEnabled() == on )
    return;

  if ( on )
    connect( mySelectAll = new QToolButton( this ), SIGNAL( clicked() ), this, SLOT( selectAll() ) );
  else
  {
    delete mySelectAll;
    mySelectAll = 0;
  }
  updateSelectAllButton();
  */
}

/*!
  Select the all cells of the table.
*/
void QtxTable::selectAll()
{
  /*
  if ( selectionMode() != Multi && selectionMode() != MultiRow )
    return;

  selectCells( 0, 0, numRows() - 1, numCols() - 1 );
  */
}

/*!
  Reimplemented for 'SelectAll' button geometry updating.
*/
void QtxTable::setTopMargin( int m )
{
  //QTableWidget::setTopMargin( m );
  //updateSelectAllButton();
}

/*!
  Reimplemented for 'SelectAll' button geometry updating.
*/
void QtxTable::setLeftMargin( int m )
{
  //QTableWidget::setLeftMargin( m );
  //updateSelectAllButton();
}

/*!
  Reimplemented for 'SelectAll' button updating.
*/
void QtxTable::setSelectionMode( SelectionMode mode )
{
  QTableWidget::setSelectionMode( mode );
  //updateSelectAllButton();
}

/*!
  \return true if header is editable
  \param o - header orientation
*/
bool QtxTable::headerEditable( Qt::Orientation o, const int idx ) const
{
  //QHeaderView* hdr = idx < 0 ? header( o ) : header( o, idx );
  return true;//myHeaderEditable.contains( hdr ) ? myHeaderEditable[hdr] : false;
}

/*!
  Changes editable state of header
  \param o - header orientation
  \param on - new state
*/
void QtxTable::setHeaderEditable( Qt::Orientation o, const bool on, const int idx )
{
  /*if ( headerEditable( o, idx ) == on )
    return;

  QHeaderView* hdr = idx < 0 ? header( o ) : header( o, idx );
  myHeaderEditable.insert( hdr, on );

  if ( !on && myEditedHeader == hdr )
    endHeaderEdit( false );

  if ( on )
    hdr->installEventFilter( this );
  else
    hdr->removeEventFilter( this );
  */
}

void QtxTable::setHeadersEditable( Qt::Orientation o, bool on )
{
  /*  setHeaderEditable( o, on );
  for ( int i = 0; i < numHeaders( o ); i++ )
    setHeaderEditable( o, on, i );
  */
}

/*!
  Starts edition of header
  \param o - header orientation
  \param sec - column/row
*/
bool QtxTable::editHeader( Qt::Orientation o, const int sec )
{
  return true;//beginHeaderEdit( o, sec );
}

/*!
  Finishes edition of header
  \param accept - whether new value must be accepted
*/
void QtxTable::endEditHeader( const bool accept )
{
  //endHeaderEdit( accept );
}

/*!
  Custom event filter
  Starts edition of header by double click
  Finishes edition by escape/return/enter pressing
*/
bool QtxTable::eventFilter( QObject* o, QEvent* e )
{
  /*if ( e->type() == QEvent::MouseButtonDblClick )
  {
    QMouseEvent* me = (QMouseEvent*)e;
    QHeaderView* hdr = ::qobject_cast<QHeaderView*>( o );
    if ( hdr )
      return beginHeaderEdit( hdr, me->pos() );
  }

  if ( o == myHeaderEditor && e->type() == QEvent::KeyPress && isHeaderEditing() )
  {
	  QKeyEvent* ke = (QKeyEvent*)e;
    if ( ke->key() == Key_Escape )
    {
      endHeaderEdit( false );
      return true;
    }

    if ( ke->state() == NoButton && ( ke->key() == Key_Return || ke->key() == Key_Enter ) )
    {
      endHeaderEdit( true );
      return true;
    }

    return false;
  }

  if ( o == myHeaderEditor && e->type() == QEvent::FocusOut &&
       ((QFocusEvent*)e)->reason() != QFocusEvent::Popup )
  {
		  endHeaderEdit();
		  return true;
  }

  if ( e->type() == QEvent::Wheel && isHeaderEditing() )
    return true;
  */
  return QTableWidget::eventFilter( o, e );
}

void QtxTable::adjustRow( int row )
{
  //QTableWidget::adjustRow( row );

  /*int size = 0;
  for ( uint i = 0; i < myVerHeaders.count(); i++ )
  {
    QHeaderView* hdr = myVerHeaders.at( i );
    QString txt = hdr->label( row );
    int h = hdr->iconSet( row ) ? hdr->iconSet( row )->pixmap().height() : 0;
    if ( !QStyleSheet::mightBeRichText( txt ) )
      h = QMAX( h, hdr->fontMetrics().height() );
    else
    {
      QStyleSheet sheet;
      QStyleSheetItem* item = sheet.item( "p" );
      if ( item )
        item->setMargin( QStyleSheetItem::MarginAll, 0 );

      QSimpleRichText rt( txt, hdr->font(), QString::null, &sheet );
      rt.setWidth( hdr->width() );
      h = QMAX( h, rt.height() );
    }
    size = QMAX( size, h );
  }

  size = QMAX( size, rowHeight( row ) );
  setRowHeight( row, size );
  */
}

void QtxTable::adjustColumn( int col )
{
  //QTableWidget::adjustColumn( col );
  /*
  int size = 0;
  for ( uint i = 0; i < myHorHeaders.count(); i++ )
  {
    Header* hdr = (Header*)myHorHeaders.at( i );
    QString txt = hdr->label( col );
    int w = hdr->iconSet( col ) ?
            hdr->iconSet( col )->pixmap( QIconSet::Automatic, QIconSet::Normal ).width() + 5 : 0;
    int txtW = 0;
    if ( !QStyleSheet::mightBeRichText( txt ) )
      txtW += hdr->fontMetrics().width( txt );
    else
    {
      QStyleSheet sheet;
      QStyleSheetItem* item = sheet.item( "p" );
      if ( item )
      {
        item->setMargin( QStyleSheetItem::MarginAll, 0 );
        item->setWhiteSpaceMode( QStyleSheetItem::WhiteSpaceNoWrap );
      }

      QSimpleRichText rt( QString( "<p>%1</p>" ).arg( txt ), hdr->font(), QString::null, &sheet );
      txtW += rt.width();
    }

    if ( hdr->horizontalSpan( col ) > 1 )
      txtW = txtW / hdr->horizontalSpan( col );

    w = w + txtW + 10;

    size = QMAX( size, w );
  }

  for ( int i = 0; i < numRows(); i++ )
  {
    QFont fnt = cellFont( i, col );
    int sz = QFontMetrics( fnt ).width( text( i, col ) ) + 10;
    size = QMAX( size, sz );
  }

  size = QMAX( size, columnWidth( col ) );
  setColumnWidth( col, size );
  */
}

void QtxTable::rowHeightChanged( int row )
{
  //QTableWidget::rowHeightChanged( row );
  /*
  int h = rowHeight( row );
  for ( uint i = 0; i < myVerHeaders.count(); i++ )
  {
    QHeaderView* hdr = myVerHeaders.at( i );
    hdr->resizeSection( row, h );
  }
  */
}

void QtxTable::columnWidthChanged( int col )
{
  //QTableWidget::columnWidthChanged( col );
  /*
  int w = columnWidth( col );
  for ( uint i = 0; i < myHorHeaders.count(); i++ )
  {
    QHeaderView* hdr = myHorHeaders.at( i );
    hdr->resizeSection( col, w );
  }
  */
}

/*!
  Reimplemented for internal reasons.
  Firstly end the edition of header.
*/
/*void QtxTable::setNumRows( int rows )
{
  endHeaderEdit();
  int old = numRows();

  QTableWidget::setNumRows( rows );

  for ( uint i = 0; i < myVerHeaders.count(); i++ )
  {
    Header* hdr = (Header*)myVerHeaders.at( i );
    for ( int d = hdr->count() - 1; d >= (int)numRows(); d-- )
      hdr->removeSection( d );
    for ( int c = hdr->count(); c < (int)numRows(); c++ )
      hdr->addLabel( QString::null );
  }
}
*/
/*!
  Reimplemented for internal reasons.
  Firstly end the edition of header.
*/
/*void QtxTable::setNumCols( int cols )
{

  endHeaderEdit();
  int old = numCols();

  QTableWidget::setNumCols( cols );

  for ( uint i = 0; i < myHorHeaders.count(); i++ )
  {
    Header* hdr = (Header*)myHorHeaders.at( i );
    for ( int d = hdr->count() - 1; d >= (int)numCols(); d-- )
      hdr->removeSection( d );
    for ( int c = hdr->count(); c < (int)numCols(); c++ )
      hdr->addLabel( QString::null );
  }
}
*/
/*!
  Reimplemented for internal reasons.
  Firstly end the edition of header.
*/
/*void QtxTable::insertRows( int row, int count )
{
  endHeaderEdit();

  QTableWidget::insertRows( row, count );

  for ( uint i = 0; i < myVerHeaders.count(); i++ )
  {
    Header* hdr = (Header*)myVerHeaders.at( i );
    for ( int j = numRows() - count - 1; j >= row; --j )
	    hdr->swapSections( j, j + count );
  }
}
*/
/*!
  Reimplemented for internal reasons.
  Firstly end the edition of header.
*/
/*void QtxTable::insertColumns( int col, int count )
{
  /*
  endHeaderEdit();

  QTableWidget::insertColumns( col, count );

  for ( uint i = 0; i < myHorHeaders.count(); i++ )
  {
    Header* hdr = (Header*)myHorHeaders.at( i );
    for ( int j = numCols() - count - 1; j >= col; --j )
	    hdr->swapSections( j, j + count );
  }
}
*/
/*!
  Reimplemented for internal reasons.
  Firstly end the edition of header.
*/
/*void QtxTable::removeRow( int row )
{
  endHeaderEdit();

  for ( uint i = 0; i < myVerHeaders.count(); i++ )
  {
    Header* hdr = (Header*)myVerHeaders.at( i );
    hdr->removeSection( row );
  }

  QTableWidget::removeRow( row );
}
*/
/*!
  Reimplemented for internal reasons.
  Firstly end the edition of header.
*/
/*void QtxTable::removeRows( const QVector<int>& rows )
{
  endHeaderEdit();

  int decr = 0;
  QMap<int, int> indexes;
  for ( int r = 0; r < numRows(); r++ )
  {
    if ( rows.contains( r ) )
      decr++;
    else
      indexes.insert( r, r - decr );
  }

  for ( uint i = 0; i < myVerHeaders.count(); i++ )
  {
    Header* hdr = (Header*)myVerHeaders.at( i );
    for ( int d = rows.count() - 1; d >= 0; d-- )
      hdr->removeSection( rows[d] );
  }
  QTableWidget::removeRows( rows );
}
*/
/*!
  Reimplemented for internal reasons.
  Firstly end the edition of header.
*/
/*void QtxTable::removeColumn( int col )
{
  endHeaderEdit();

  for ( uint i = 0; i < myHorHeaders.count(); i++ )
  {
    Header* hdr = (Header*)myHorHeaders.at( i );
    hdr->removeSection( col );
  }
  QTableWidget::removeColumn( col );
}
*/
/*!
  Reimplemented for internal reasons.
  Firstly end the edition of header.
*/
/*void QtxTable::removeColumns( const QVector<int>& cols )
{
  endHeaderEdit();

  int decr = 0;
  QMap<int, int> indexes;
  for ( int c = 0; c < numCols(); c++ )
  {
    if ( cols.contains( c ) )
      decr++;
    else
      indexes.insert( c, c - decr );
  }

  for ( uint i = 0; i < myHorHeaders.count(); i++ )
  {
    Header* hdr = (Header*)myHorHeaders.at( i );
    for ( int d = cols.count() - 1; d >= 0; d-- )
      hdr->removeSection( cols[d] );
  }
  QTableWidget::removeColumns( cols );
}
*/
void QtxTable::setUpdatesEnabled( bool enable )
{
  /*
  QTableWidget::setUpdatesEnabled( enable );

  if ( enable )
  {
    updateHeaderGeometries( Qt::Horizontal );
    updateHeaderGeometries( Qt::Vertical );
  }
  */
}

QHeaderView* QtxTable::header( const Qt::Orientation o, const int idx ) const
{
  /*
  HeaderVector* vec = headerVector( o );

  if ( idx < 0 || idx >= (int)vec->count() )
    return 0;

  return vec->at( idx );
  */
  return 0;
}

int QtxTable::numHeaders( const Qt::Orientation o ) const
{
  return 0;//headerVector( o )->count();
}

void QtxTable::setNumHeaders( const Qt::Orientation o, const int n )
{
  /*
  headerVector( o )->resize( QMAX( n, 0 ) );

  updateHeaders( o );
  updateHeaderSizes( o );
  updateHeaderSpace( o );

  updateGeometries();
  */
}

QVariant QtxTable::headerData( const Qt::Orientation o, const int section, const int role ) const
{
  QVariant res;
  if ( model() )
    res = model()->headerData( section, o, role );
  return res;
}

QFont QtxTable::headerFont( const Qt::Orientation o, const int section ) const
{
  QFont res = o == Qt::Horizontal ? horizontalHeader()->font() :
                                    verticalHeader()->font();
  QVariant aVar = headerData( o, section, Qt::FontRole );
  if ( aVar.isValid() && aVar.canConvert( QVariant::Font ) )
    res = aVar.value<QFont>();
  return res;
}

QColor QtxTable::headerForeground( const Qt::Orientation o, const int section ) const
{
  QColor res;
  QVariant aVar = headerData( o, section, Qt::ForegroundRole );
  if ( aVar.isValid() && aVar.canConvert( QVariant::Color ) )
    res = aVar.value<QColor>();
  return res;
}

QColor QtxTable::headerBackground( const Qt::Orientation o, const int section ) const
{
  QColor res;
  QVariant aVar = headerData( o, section, Qt::BackgroundRole );
  if ( aVar.isValid() && aVar.canConvert( QVariant::Color ) )
    res = aVar.value<QColor>();
  return res;
}

QIcon QtxTable::headerIcon( const Qt::Orientation o, const int section ) const
{
  QIcon res;
  QVariant aVar = headerData( o, section, Qt::DecorationRole );
  if ( aVar.isValid() && aVar.canConvert( QVariant::Icon ) )
    res = aVar.value<QIcon>();
  return res;
}

void QtxTable::setHeaderData( const Qt::Orientation o, const int section, const QVariant& var,
                              const int role )
{
  QTableWidgetItem* anItem = 0;
  if ( o == Qt::Horizontal )
    anItem = horizontalHeaderItem( section );
  else
    anItem = verticalHeaderItem( section );

  if ( anItem )
    anItem->setData( Qt::DisplayRole, var );
  else {
    anItem = new QTableWidgetItem();
    anItem->setData( Qt::DisplayRole, var );
    if ( o == Qt::Horizontal )
      setHorizontalHeaderItem( section, anItem );
    else
      setVerticalHeaderItem( section, anItem );
  }
}

void QtxTable::setHeaderFont( const Qt::Orientation o, const int section, const QFont& font )
{
  setHeaderData( o, section, QVariant( font ), Qt::FontRole );
}

void QtxTable::setHeaderForeground( const Qt::Orientation o, const int section, const QColor& c )
{
  setHeaderData( o, section, QVariant( c ), Qt::ForegroundRole );
}

void QtxTable::setHeaderBackground( const Qt::Orientation o, const int section, const QColor& c )
{
  setHeaderData( o, section, QVariant( c ), Qt::BackgroundRole );
}

void QtxTable::setHeaderIcon( const Qt::Orientation o, const int section, const QIcon& icon )
{
  setHeaderData( o, section, QVariant( icon ), Qt::DecorationRole );
}

QVariant QtxTable::cellData( const int row, const int col ) const
{
  QVariant res;
  QTableWidgetItem* anItem = item( row, col );
  if ( anItem )
    res = anItem->data( Qt::DisplayRole );
  
  return res;
}

QFont QtxTable::cellFont( const int row, const int col ) const
{
  QFont res = font();
  QTableWidgetItem* anItem = item( row, col );
  if ( anItem )
    res = anItem->font();
  
  return res;
}

QColor QtxTable::cellForeground( const int row, const int col ) const
{
  QColor res = Qt::black;
  QTableWidgetItem* anItem = item( row, col );
  if ( anItem && anItem->foreground().style() != Qt::NoBrush )
    res = anItem->foreground().color();
  return res;
}

QColor QtxTable::cellBackground( const int row, const int col ) const
{
  QColor res = Qt::white;
  QTableWidgetItem* anItem = item( row, col );
  if ( anItem && anItem->background().style() != Qt::NoBrush )
    res = anItem->background().color();
  return res;
}

QIcon QtxTable::cellIcon( const int row, const int col ) const
{
  QIcon res;
  QTableWidgetItem* anItem = item( row, col );
  if ( anItem )
    res = anItem->icon();
  
  return res;
}

void QtxTable::setCellData( const int row, const int col, const QVariant& val )
{
  if ( !val.isValid() )
    return;
  QTableWidgetItem* anItem = getItem( row, col );
  if ( anItem )
    anItem->setData( Qt::DisplayRole, val );
}

void QtxTable::setCellFont( const int row, const int col, const QFont& f )
{
  QTableWidgetItem* anItem = getItem( row, col );
  if ( anItem )
    anItem->setFont( f );
}

void QtxTable::setCellForeground( const int row, const int col, const QColor& c )
{
  if ( !c.isValid() )
    return;

  QTableWidgetItem* anItem = getItem( row, col );
  if ( anItem )
    anItem->setForeground( c );
}

void QtxTable::setCellBackground( const int row, const int col, const QColor& c )
{
  if ( !c.isValid() )
    return;

  QTableWidgetItem* anItem = getItem( row, col );
  if ( anItem )
    anItem->setBackground( c );
}

void QtxTable::setCellIcon( const int row, const int col, QIcon& icon )
{
  QTableWidgetItem* anItem = getItem( row, col );
  if ( anItem )
    anItem->setIcon( icon );
}
/*!
  Return item from cell 
  \param row - table row
  \param col - table column
  \param created - if item not exit and create=true, it will be created
*/
QTableWidgetItem* QtxTable::getItem( const int row, const int col, const bool created )
{
  QTableWidgetItem* anItem = item( row, col );
  if ( !anItem && created && row < rowCount() && col < columnCount() ) {
    anItem = new QTableWidgetItem();
    setItem( row, col, anItem );
  }
  return anItem;
}

QModelIndexList QtxTable::getSelectedIndexes()
{
  return selectedIndexes();
}

bool QtxTable::indexPosition( const QModelIndex& theIndex, int& theRow,
                              int& theCol ) const
{
  theRow = -1;
  theCol = -1;
  QAbstractTableModel* aModel = ::qobject_cast<QAbstractTableModel*>
                                ( model() );
  if ( aModel ) {
    for ( int i = 0, aNbCols = columnCount(); i < aNbCols; i++ )
      for ( int j = 0, aNbRows = rowCount(); j < aNbRows; j++ )
        if ( aModel->index( j, i ) == theIndex ) {
          theRow = i;
          theCol = j;
          break;
        }
  }
  return theRow != -1 && theCol != -1;
}

/*void QtxTable::paintCell( QPainter* p, int row, int col, const QRect& cr,
                          bool selected, const QColorGroup& cg )
{
  QColor fg = cellForeground( row, col );
  QColor bg = cellBackground( row, col );

  QColorGroup cGroup = cg;
  if ( fg.isValid() )
    cGroup.setColor( QColorGroup::Text, fg );
  if ( bg.isValid() )
    cGroup.setColor( QColorGroup::Base, bg );

  p->save();
  p->setFont( cellFont( row, col ) );

  QTableWidget::paintCell( p, row, col, cr, selected, cGroup );

  p->restore();
}
*/

void QtxTable::clear( const bool withHeaders )
{
  if ( withHeaders )
    QTableWidget::clear();
  else
    clearContents();

  /*
  if ( !withHeaders )
    return;

  QHeaderView* hh = horizontalHeader();
  for ( int hi = 0; hi < hh->count(); hi++ )
    hh->setLabel( hi, QString( "" ) );
  QHeaderView* vh = verticalHeader();
  for ( int vi = 0; vi < vh->count(); vi++ )
    vh->setLabel( vi, QString( "" ) );

  for ( int h = 0; h < (int)myHorHeaders.size(); h++ )
    ((Header*)myHorHeaders.at( h ))->clear();
  for ( int v = 0; v < (int)myVerHeaders.size(); v++ )
    ((Header*)myVerHeaders.at( v ))->clear();
  */
}

/*!
  SLOT: called on scroll
*/
void QtxTable::onScrollBarMoved( int )
{
  //updateHeaderEditor();
}

/*!
  SLOT: called on header size changing
*/
void QtxTable::onHeaderSizeChange( int section, int oldSize, int newSize )
{
  /*
  const QHeaderView* header = ::qobject_cast<const QHeaderView*>( sender() );
  if ( !header )
    return;

  HeaderVector* vec = headerVector( header->orientation() );
  for ( int i = 0; i < (int)vec->size(); i++ )
  {
    QHeaderView* hdr = vec->at( i );
    hdr->resizeSection( section, newSize );
  }

  if ( header == myEditedHeader )
    updateHeaderEditor();
  */
}

/*!
  Custom hide event handler
*/
void QtxTable::hideEvent( QHideEvent* e )
{
  //endHeaderEdit();

  QTableWidget::hideEvent( e );
}

/*!
  Custom resize event handler
*/
void QtxTable::resizeEvent( QResizeEvent* e )
{
  QTableWidget::resizeEvent( e );

  //updateHeaderEditor();

  //updateGeometries();
}

/*!
  Starts edition of header
  \param o - header orientation
  \param sec - column/row
*/
bool QtxTable::beginHeaderEdit( Qt::Orientation o, const int section, const int idx )
{
  /*
  QHeaderView* hdr = 0;
  if ( idx < 0 )
    hdr = header( o );
  if ( !hdr->isVisibleTo( this ) )
    hdr = header( o, idx );
  
  return beginHeaderEdit( hdr, section );
  */
  return true;
}

bool QtxTable::beginHeaderEdit( QHeaderView* hdr, const int s )
{
  /*
  if ( !hdr || !myHeaderEditable.contains( hdr ) || !myHeaderEditable[hdr] || !hdr->isVisibleTo( this ) )
    return false;

  endHeaderEdit();

  int section = s;
  QRect r = headerSectionRect( hdr, s, &section );
  if ( !r.isValid() )
    return false;

  if ( hdr->orientation() == Qt::Horizontal )
    r.setLeft( QMAX( r.left(), leftMargin() ) );
  else
    r.setTop( QMAX( r.top(), topMargin() ) );

  myHeaderEditor = createHeaderEditor( hdr, section );
  if ( !myHeaderEditor )
    return false;

  myEditedHeader = hdr;
  myEditedSection = section;

  myHeaderEditor->reparent( this, QPoint( 0, 0 ), false );

  updateHeaderEditor();

  myHeaderEditor->show();

  myHeaderEditor->setActiveWindow();
  myHeaderEditor->setFocus();

  myHeaderEditor->installEventFilter( this );
  */
  return true;
}

/*!
  Finishes edition of header
  \param accept - whether new value must be accepted
*/
void QtxTable::endHeaderEdit( const bool accept )
{
  /*
  if ( !isHeaderEditing() )
    return;

  QString oldTxt = myEditedHeader ? myEditedHeader->label( myEditedSection ) : QString();

  if ( accept && myEditedHeader )
    setHeaderContentFromEditor( myEditedHeader, myEditedSection, myHeaderEditor );

  QString newTxt = myEditedHeader ? myEditedHeader->label( myEditedSection ) : QString();

  int sec = myEditedSection;
  QHeaderView* hdr = myEditedHeader;

  myEditedHeader = 0;
  myEditedSection = -1;

  myHeaderEditor->hide();
  myHeaderEditor->deleteLater();
  myHeaderEditor->removeEventFilter( this );
  myHeaderEditor = 0;

  if ( oldTxt != newTxt )
  {
    emit headerEdited( hdr, sec );
    emit headerEdited( hdr == horizontalHeader() ? Qt::Horizontal : Qt::Vertical, sec );
  }
  */
}

/*!
  \return true if header is being edited
*/
bool QtxTable::isHeaderEditing() const
{
  return true;//myHeaderEditor && myEditedHeader && myEditedSection != -1;
}

/*!
  Creates and \return header editor
  \param hdr - header
  \param sec - column/row
  \param init - init editor with value
*/
QWidget* QtxTable::createHeaderEditor( QHeaderView* hdr, const int sec, const bool init )
{

  //QString lab = hdr ? hdr->label( sec ) : QString::null;

  QWidget* wid = 0;
  /*
  if ( !QStyleSheet::mightBeRichText( lab ) && !lab.contains( "\n" ) )
  {
    QLineEdit* le = new QLineEdit( 0 );
    if ( init )
      le->setText( lab );
    wid = le;
  }
  else
  {
    QTextEdit* te = new QTextEdit( 0 );
    te->setVScrollBarMode( QTextEdit::AlwaysOff );
    te->setHScrollBarMode( QTextEdit::AlwaysOff );
    QStyleSheet* sheet = new QStyleSheet( te );
    QStyleSheetItem* i = sheet->item( "p" );
    if ( i )
      i->setMargin( QStyleSheetItem::MarginAll, 0 );
    te->setStyleSheet( sheet );

    if ( init )
      te->setText( lab );
    wid = te;
  }
  */
  return wid;
}

/*!
  Initialize editor with value
  \param hdr - header
  \param sec - column/row
  \param editor - editor
*/
void QtxTable::setHeaderContentFromEditor( QHeaderView* hdr, const int sec, QWidget* editor )
{
  /*
  if ( !hdr || !editor )
    return;

  QLineEdit* le = ::qobject_cast<QLineEdit*>( editor );
  if ( le )
    hdr->setLabel( sec, le->text() );
  else
  {
    QTextEdit* te = ::qobject_cast<QTextEdit*>( editor );
    if ( te )
      hdr->setLabel( sec, te->text().remove( "\n" ) );
  }
  */
}

/*!
  \return header
  \param o - orientation
*/
QHeaderView* QtxTable::header( Qt::Orientation o ) const
{
  return o == Qt::Horizontal ? horizontalHeader() : verticalHeader();
}

/*!
  Starts edition of header
  \param o - header orientation
  \param p - point
*/
bool QtxTable::beginHeaderEdit( QHeaderView* hdr, const QPoint& p )
{
  /*
  if ( !hdr )
    return false;

  int pos = hdr->orientation() == Qt::Horizontal ? p.x() : p.y();
  int sec = hdr->sectionAt( hdr->offset() + pos );
  if ( sec < 0 )
    return false;

  bool ok = false;
  QPoint pnt = mapFromGlobal( hdr->mapToGlobal( p ) );
  int cur = hdr->orientation() == Qt::Horizontal ? pnt.x() : pnt.y();

  int bid = 0;
  QRect sRect = headerSectionRect( hdr, sec, &bid );
  if ( sRect.isValid() )
  {
    int m = 4;
    int from = hdr->orientation() == Qt::Horizontal ? sRect.left() : sRect.top();
    int to = hdr->orientation() == Qt::Horizontal ? sRect.right() - m : sRect.bottom() - m;
    if ( sec > 0 )
      from += m;

    ok = from <= cur && cur <= to;
  }

  if ( ok )
    beginHeaderEdit( hdr, sec );

  return ok;
  */
  return true;
}

/*!
  \return rectangle of header section
  \param hdr - header
  \param sec - column/row
*/
QRect QtxTable::headerSectionRect( QHeaderView* hdr, const int sec, int* main ) const
{
  QRect r( -1, -1, -1, -1 );

  /*if ( !hdr )
    return r;

  r = hdr->sectionRect( sec );
  if ( hdr != verticalHeader() && hdr != horizontalHeader() )
    r = ((Header*)hdr)->indexRect( hdr->mapToIndex( sec ), main );

  if ( r.isValid() )
    r = QRect( mapFromGlobal( hdr->mapToGlobal( r.topLeft() ) ), r.size() );
  */

  return r;
}

/*!
  Updates header editor
*/
void QtxTable::updateHeaderEditor()
{
  /*
  if ( !myHeaderEditor || !myEditedHeader || myEditedSection < 0 )
    return;

  QRect r = headerSectionRect( myEditedHeader, myEditedSection );
  if ( !r.isValid() )
    return;

  if ( myEditedHeader->orientation() == Qt::Horizontal )
  {
    r.setLeft( QMAX( r.left(), leftMargin() ) );
    r.setRight( QMIN( r.right(), width() - rightMargin() - 2 ) );
  }
  else
  {
    r.setTop( QMAX( r.top(), topMargin() ) );
    r.setBottom( QMIN( r.bottom(), height() - bottomMargin() - 2 ) );
  }

  myHeaderEditor->resize( r.size() );
  myHeaderEditor->move( r.topLeft() );
  */
}

/*!
  Remove selected rows or columns if any of cell selected in it
*/
void QtxTable::removeSelected( const bool row )
{
  /*
  QValueList<int> idsList;
  QMap<int, int> idMap;
  int i = 0;
  for ( int nb = numSelections(); i < nb; i++ )
  {
    QTableSelection sel = selection( i );
    int minId = row ? sel.topRow() : sel.leftCol();
    int maxId = row ? sel.bottomRow() : sel.rightCol();
    for ( ; minId <= maxId; minId++ )
      if ( !idMap.contains( minId ) )
      {
        idMap[ minId ] = 1;
        idsList.append( minId );
      }
  }
  if ( idMap.isEmpty() )
    return;

  qHeapSort( idsList );
  QVector<int> idsArr( idsList.count() );
  QValueList<int>::const_iterator it = idsList.begin();
  QValueList<int>::const_iterator itEnd = idsList.end();
  for ( i = 0; it != itEnd; ++it, ++i )
    idsArr[ i ] = *it;

  if ( row )
    removeRows( idsArr );
  else
    removeColumns( idsArr );
  */
}

QtxTable::HeaderVector* QtxTable::headerVector( const Qt::Orientation o ) const
{
  return 0;//o == Qt::Horizontal ? (HeaderVector*)&myHorHeaders : (HeaderVector*)&myVerHeaders;
}

QtxStyleWrap* QtxTable::styleWrapper()
{
  /*  if ( !myStyleWrapper )
  {
    myStyleWrapper = new QtxStyleWrap( &style() );
    new StyleItem( myStyleWrapper );
  }
  return myStyleWrapper;
  */
  return 0;
}

void QtxTable::updateHeaders( const Qt::Orientation o )
{
  /*
  HeaderVector& vec = o == Qt::Vertical ? myVerHeaders : myHorHeaders;
  QScrollBar* sb = o == Qt::Vertical ? verticalScrollBar() : horizontalScrollBar();
  QHeaderView* main = o == Qt::Vertical ? verticalHeader() : horizontalHeader();

  main->setShown( !vec.size() );

  for ( uint i = 0; i < vec.size(); i++ )
  {
    if ( vec.at( i ) )
      continue;

    QHeaderView* hdr = new Header( o == Qt::Horizontal ? numCols() : numRows(), this );
    vec.insert( i, hdr );
    hdr->setStyle( styleWrapper() );
    hdr->setMovingEnabled( false );
    hdr->setOrientation( o );
    hdr->installEventFilter( this );
    hdr->show();

    connect( sb, SIGNAL( valueChanged( int ) ), hdr, SLOT( setOffset( int ) ) );
    if ( o == Qt::Horizontal )
      connect( hdr, SIGNAL( sectionHandleDoubleClicked( int ) ), this, SLOT( adjustColumn( int ) ) );
    else
      connect( hdr, SIGNAL( sectionHandleDoubleClicked( int ) ), this, SLOT( adjustRow( int ) ) );
  }

  if ( o == Qt::Horizontal )
  {
    for ( uint j = 0; j < vec.size(); j++ )
    {
      QHeaderView* hdr = vec.at( j );
      bool upd = hdr->isUpdatesEnabled();
      hdr->setUpdatesEnabled( false );
      for ( int s = 0; s < (int)hdr->count(); s++ )
        hdr->setLabel( s, QString( "" ) );
      hdr->setUpdatesEnabled( upd );
    }
  }

  // slot onScrollBarMoved should be last connection.
  // QHeaderView::setOffset() should be invoked before it for every header.
  // Otherwise header editor geometry will not properly updated.
  disconnect( verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onScrollBarMoved( int ) ) );
  disconnect( horizontalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onScrollBarMoved( int ) ) );
  connect( verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onScrollBarMoved( int ) ) );
  connect( horizontalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onScrollBarMoved( int ) ) );
  */
}

void QtxTable::updateHeaderSpace( const Qt::Orientation o )
{
  /*
  HeaderVector& vec = o == Qt::Vertical ? myVerHeaders : myHorHeaders;

  int size = 0;
  for ( uint i = 0; i < vec.size(); i++ )
  {
    QHeaderView* hdr = vec.at( i );
    int sz = o == Qt::Vertical ? hdr->width() : hdr->height();
    if ( o == Qt::Vertical )
      hdr->move( size, 0 );
    else
      hdr->move( 0, size );
    size += sz;
  }

  if ( vec.size() == 0 )
    size = o == Qt::Vertical ? verticalHeader()->width() : horizontalHeader()->height();

  if ( o == Qt::Vertical )
    setLeftMargin( size );
  else
    setTopMargin( size );
  */
}

void QtxTable::updateHeaderSizes( const Qt::Orientation o )
{
  /*
  HeaderVector& vec = o == Qt::Vertical ? myVerHeaders : myHorHeaders;

  for ( uint j = 0; j < vec.count(); j++ )
  {
    QHeaderView* hdr = vec.at( j );
    for ( int s = 0; s < (int)hdr->count(); s++ )
      hdr->resizeSection( s, o == Qt::Vertical ? rowHeight( s ) : columnWidth( s ) );
    if ( o == Qt::Vertical )
      hdr->resize( hdr->sizeHint().width(), hdr->height() );
    else
      hdr->resize( hdr->width(), hdr->sizeHint().height() );
  }
  */
}

/*
void QtxTable::updateGeometries()
{
  QSize sz = horizontalHeader()->size();
  int size = frameWidth();
  for ( int h = 0; h < (int)myHorHeaders.size(); h++ )
  {
    QHeaderView* hdr = myHorHeaders.at( h );
    hdr->setGeometry( frameWidth() + leftMargin(), size, sz.width(), hdr->height() );
    size += hdr->height();
  }

  sz = verticalHeader()->size();
  size = frameWidth();
  for ( int v = 0; v < (int)myVerHeaders.size(); v++ )
  {
    QHeaderView* hdr = myVerHeaders.at( v );
    hdr->setGeometry( size, frameWidth() + topMargin(), hdr->width(), sz.height() );
    size += hdr->width();
  }
}
*/

void QtxTable::updateHeaderGeometries( const Qt::Orientation o )
{
  /*
  if ( !isUpdatesEnabled() )
    return;

  updateHeaderSizes( o );
  updateHeaderSpace( o );
  updateGeometries();
  */
}

void QtxTable::updateSelectAllButton()
{
  /*
  if ( !mySelectAll )
    return;

  mySelectAll->setShown( selectionMode() == Multi || selectionMode() == MultiRow );
  if ( mySelectAll->isVisibleTo( this  ) )
    mySelectAll->setGeometry( frameWidth(), frameWidth(), leftMargin(), topMargin() );
  */
}

#endif
