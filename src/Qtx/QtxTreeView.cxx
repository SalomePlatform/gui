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
// File:      QtxTreeView.cxx
// Author:    Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "QtxTreeView.h"
#include "QtxTreeModel.h"

#include <QApplication>
#include <QHeaderView>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>

/*!
  \class QtxTreeView::Header
  \brief Custom tree view header class.
  \internal
*/

class QtxTreeView::Header : public QHeaderView
{
  Q_OBJECT
public:
  Header( const bool, QWidget* = 0 );
  ~Header();

  void         setSortMenuEnabled( const bool );
  bool         sortMenuEnabled() const;

  void         setSortingEnabled( const bool );
  bool         sortingEnabled() const;
  void         setMultiSortEnabled( const bool );
  bool         multiSortEnabled() const;
  void         setSortIndicators( const QVector<int>&, const QVector<Qt::SortOrder>& );
  void         sortIndicators( QVector<int>&, QVector<Qt::SortOrder>& ) const;
  void         clearSortIndicators();
  bool         sortIndicatorsShown() const;

  void         addMenuAction( QAction* );
protected:
  virtual void contextMenuEvent( QContextMenuEvent* );
  virtual void paintSection( QPainter* painter, const QRect& rect, int logicalIndex ) const;

private slots:
  void         onSectionClicked( int logicalIndex );

private:
  typedef QMap<int, QAction*> ActionsMap;
  bool                   myEnableSortMenu;
  ActionsMap             myActions;
  bool                   myEnableMultiSort;
  QVector<int>           mySortSections;
  QVector<Qt::SortOrder> mySortOrders;
};

/*!
  \brief Constructor
  \param enableSortMenu show "Sorting" menu if \c true
  \param parent parent widget
  \internal
*/
QtxTreeView::Header::Header( const bool enableSortMenu, QWidget* parent )
: QHeaderView( Qt::Horizontal, parent ),
  myEnableSortMenu( enableSortMenu )
{
  setMultiSortEnabled( false );
  // This connection should be created as early as possible, to perform internal processing
  // before any external actions
  connect( this, SIGNAL( sectionClicked( int ) ), this, SLOT( onSectionClicked( int ) ) );
}

/*!
  \brief Destructor
  \internal
*/
QtxTreeView::Header::~Header()
{
}

/*!
  \brief Enable/disable sorting operation for the header.
  \param enable if \c true, makes the header clickable and shows sort indicator(s)
  \internal
*/
void QtxTreeView::Header::setSortingEnabled( const bool enable )
{
  // Suppress default Qt sort indicator for multi-sort case
  setSortIndicatorShown( enable && !multiSortEnabled() );

  // Clear multi-sort indicators
  if ( !enable )
    clearSortIndicators();

  setClickable( enable );

  QtxTreeView* view = qobject_cast<QtxTreeView*>( parent() );
  if ( view ) {
    view->emitSortingEnabled( enable );
    if ( enable ) {
      connect( this, SIGNAL( sectionClicked( int ) ), view, SLOT( onHeaderClicked() ) );
      if ( multiSortEnabled() )
      {
        QVector<int> columns;
        columns.push_back( 0 );
        QVector<Qt::SortOrder> orders;
        orders.push_back( Qt::AscendingOrder );
        setSortIndicators( columns, orders );
      }
      view->onHeaderClicked();
    }
    else {
      disconnect( this, SIGNAL( sectionClicked( int ) ), view, SLOT( onHeaderClicked() ) );
      view->sortByColumn( 0, Qt::AscendingOrder );
    }
  }
}

/*!
  \brief Query status of sorting (enabled or disabled).
  \return \c true if single or multiple sort indicators are shown
  \internal
*/
bool QtxTreeView::Header::sortingEnabled() const
{
  return sortIndicatorsShown();
}

/*!
  \brief Enable/disable "Sorting" popup menu command for the header.
  \param enableSortMenu if \c true, enable "Sorting" menu command
  \internal
*/
void QtxTreeView::Header::setSortMenuEnabled( const bool enableSortMenu )
{
  myEnableSortMenu = enableSortMenu;
}

/*!
  \brief Check if "Sorting" popup menu command for the header is enabled.
  \return \c true if "Sorting" menu command is enabled
  \internal
*/
bool QtxTreeView::Header::sortMenuEnabled() const
{
  return myEnableSortMenu;
}

/*!
  \brief Enable/disable multi-column sorting. Sorting itself should be enabled using setSortingEnabled( true )
  \param enable if \c true, enables multi-column sorting
  \internal
*/
void QtxTreeView::Header::setMultiSortEnabled( const bool enable )
{
  myEnableMultiSort = enable;
  // Multi-sort and usual sort are mutually exclusive
  setSortIndicatorShown( isSortIndicatorShown() && !enable );
}

/*!
  \brief Check multi-column sorting status (enabled or disabled).
  \return \c true if multi-column sorting is enabled
  \internal
*/
bool QtxTreeView::Header::multiSortEnabled() const
{
  return myEnableMultiSort;
}

/*!
  \brief Set sorting columns and sort orders for multi-sorting operation.
  \param logicalIndices Zero-based logical indices of columns to be used for sorting
  \param orders orders[i] contains sort order for section logicalIndices[i]
  \internal
*/
void QtxTreeView::Header::setSortIndicators( const QVector<int>& logicalIndices,
                                             const QVector<Qt::SortOrder>& orders )
{
  mySortSections = logicalIndices;
  mySortOrders   = orders;
}

/*!
  \brief Query sorting columns and sort orders for multi-sorting operation.
  \param logicalIndices Out parameter for zero-based logical indices of columns currently used for sorting
  \param orders Out parameter, orders[i] contains the current sort order for section logicalIndices[i]
  \internal
*/
void QtxTreeView::Header::sortIndicators( QVector<int>& logicalIndices,
                                          QVector<Qt::SortOrder>& orders ) const
{
  logicalIndices.clear();
  orders.clear();

  if ( isSortIndicatorShown() ){
    logicalIndices.push_back( sortIndicatorSection() );
    orders.push_back        ( sortIndicatorOrder  () );
  }
  else{
    logicalIndices = mySortSections;
    orders         = mySortOrders;
  }
}

/*!
  \brief Clears sort indicators for multi-column sorting.
  \internal
*/
void QtxTreeView::Header::clearSortIndicators()
{
  mySortSections.clear();
  mySortOrders.clear();
}

/*!
  \brief Query sort indicators state (shown or not).
  \return \c true if usual or multi-column sorting indicator is shown
  \internal
*/
bool QtxTreeView::Header::sortIndicatorsShown() const
{
  return isSortIndicatorShown() || ( multiSortEnabled() && mySortSections.size() );
}

Qt::SortOrder operator~( const Qt::SortOrder& arg )
{
  return arg == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder;
}

/*!
  \brief Paints the tree view header section.

  Re-implementation of paintSection() handles multiple sort indicators.
  It lacks selection support (that is not important for the tree view header in most cases)
  and some dynamic features such as mouse hover feedback. This can be added if necessary.

  \internal
*/
void QtxTreeView::Header::paintSection( QPainter* painter, const QRect& rect, int logicalIndex ) const
{
  if (!rect.isValid())
    return;
  // get the state of the section
  QStyleOptionHeader opt;
  initStyleOption(&opt);

  if (isEnabled())
    opt.state |= QStyle::State_Enabled;
  if (window()->isActiveWindow())
    opt.state |= QStyle::State_Active;

  QVector<int>           sortIndices;
  QVector<Qt::SortOrder> sortOrders;
  sortIndicators( sortIndices, sortOrders );

  int sortIndex = sortIndices.indexOf( logicalIndex );

  if ( ( isSortIndicatorShown() || multiSortEnabled() ) && sortIndex != -1 )
    opt.sortIndicator = ( sortOrders[sortIndex] == Qt::AscendingOrder )
    ? QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;

  // setup the style options structure
  QVariant textAlignment   = model()->headerData( logicalIndex, Qt::Horizontal, Qt::TextAlignmentRole );
  opt.rect = rect;
  opt.section = logicalIndex;
  opt.textAlignment = Qt::Alignment( textAlignment.isValid()
    ? Qt::Alignment( textAlignment.toInt() ) : defaultAlignment() );

  opt.iconAlignment = Qt::AlignVCenter;
  opt.text                 = model()->headerData( logicalIndex, Qt::Horizontal, Qt::DisplayRole ).toString();

  QVariant variant         = model()->headerData( logicalIndex, Qt::Horizontal, Qt::DecorationRole );
  opt.icon = qvariant_cast<QIcon>(variant);
  if (opt.icon.isNull())
    opt.icon = qvariant_cast<QPixmap>(variant);

  QVariant foregroundBrush = model()->headerData( logicalIndex, Qt::Horizontal, Qt::ForegroundRole);
  if (qVariantCanConvert<QBrush>(foregroundBrush))
    opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));

  QPointF oldBO = painter->brushOrigin();
  QVariant backgroundBrush = model()->headerData( logicalIndex, Qt::Horizontal, Qt::BackgroundRole );
  if ( qVariantCanConvert<QBrush>( backgroundBrush ) ) {
    opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
    opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
    painter->setBrushOrigin( opt.rect.topLeft() );
  }

  // the section position
  int visual = visualIndex( logicalIndex );
  Q_ASSERT(visual != -1);
  if ( count() == 1 )
    opt.position = QStyleOptionHeader::OnlyOneSection;
  else if ( visual == 0 )
    opt.position = QStyleOptionHeader::Beginning;
  else if ( visual == count() - 1 )
    opt.position = QStyleOptionHeader::End;
  else
    opt.position = QStyleOptionHeader::Middle;

  opt.orientation = Qt::Horizontal;

  // draw the section (background, label, pixmap and sort indicator)
  style()->drawControl( QStyle::CE_Header, &opt, painter, this );

  // Optionally, draw the sort priority for the current column
  if ( sortIndex >=0 && sortIndices.size() > 1 ){
    QRegion oldClip = painter->clipRegion();
    painter->setClipRect( rect );

    QFont oldFont = painter->font();
    QFont fnt( oldFont );
    fnt.setPointSize( fnt.pointSize() - 2 );
    painter->setFont(fnt);
    style()->drawItemText( painter,
                           //rect,
                           QRect( rect.left(), rect.top(), rect.width() - 2, rect.height() ),
                           Qt::AlignRight,
                           opt.palette,
                           ( opt.state & QStyle::State_Enabled ),
                           QString( "%1" ).arg( ++sortIndex ),
                           QPalette::ButtonText );

    painter->setFont( oldFont );
    painter->setClipRegion( oldClip );
  }

  painter->setBrushOrigin(oldBO);
}

/*!
  \brief Custom mouse click handler, supports clicking multiple header sections with <Ctrl> held.
  \param logicalIndex the logical index of section
  \internal
*/
void QtxTreeView::Header::onSectionClicked( int logicalIndex )
{
  if ( multiSortEnabled() ){
    // Already using section for sorting -> reverse the order
    int sortIndex = mySortSections.indexOf( logicalIndex );
    Qt::SortOrder aDefSortOrder = sortIndex != -1 ? ~mySortOrders[sortIndex] : Qt::AscendingOrder;

    // <Ctrl> not pressed -> clear all sort columns first
    if ( !( QApplication::keyboardModifiers() & Qt::ControlModifier ) ){
      mySortSections.clear();
      mySortOrders.clear();
    }

    if ( sortIndex >=0 && sortIndex < mySortOrders.size() ){
      mySortOrders[sortIndex] = aDefSortOrder;
    }
    else{
      mySortSections.push_back( logicalIndex );
      mySortOrders.push_back  ( mySortOrders.empty() ? aDefSortOrder : mySortOrders.last() );
    }
  }
}

/*!
  \brief Appends action to header popup menu.
  \param action the action
  \internal
*/
void QtxTreeView::Header::addMenuAction( QAction* action )
{
  bool aFind = false;
  ActionsMap::const_iterator anIt = myActions.begin(), aLast = myActions.end();
  for ( ; anIt != aLast && !aFind; anIt++ )
    aFind = anIt.value() == action;
  if ( !aFind )
    myActions[myActions.size()] = action;
}

/*!
  \brief Customize context menu event.
  \internal

  Shows popup menu with the list of the available columns allowing the user to
  show/hide the specified column.

  \param e context menu event
*/
void QtxTreeView::Header::contextMenuEvent( QContextMenuEvent* e )
{
  QMenu menu;
  QMap<QAction*, int> actionMap;
  for ( int i = 0; i < count(); i++ ) {
    QString  lab         = model()->headerData( i, orientation(), Qt::DisplayRole ).toString();
    QVariant iconData    = model()->headerData( i, orientation(), Qt::DecorationRole );
    QVariant appropriate = model()->headerData( i, orientation(), Qtx::AppropriateRole );
    QIcon icon;
    if ( iconData.isValid() ) {
      if ( qVariantCanConvert<QIcon>( iconData ) )
	icon = qVariantValue<QIcon>( iconData );
      else if ( qVariantCanConvert<QPixmap>( iconData ) )
	icon = qVariantValue<QPixmap>( iconData );
    }
    if ( ( !lab.isEmpty() || !icon.isNull() ) &&
	 appropriate.isValid() ? appropriate.toBool() : true ) {
      QAction* a = menu.addAction( icon, lab );
      a->setCheckable( true );
      a->setChecked( !isSectionHidden( i ) );
      actionMap.insert( a, i );
    }
  }
  QAction* sortAction = 0;
  if ( count() > 0 && myEnableSortMenu ) {
    menu.addSeparator();
    sortAction = menu.addAction( tr( "Enable sorting" ) );
    sortAction->setCheckable( true );
    sortAction->setChecked( sortingEnabled() );
  }
  if ( myActions.size() > 0 ) {
    menu.addSeparator();
    ActionsMap::const_iterator anIt = myActions.begin(), aLast = myActions.end();
    for ( ; anIt != aLast; anIt++ )
      menu.addAction( anIt.value() );
  }
  if ( !menu.isEmpty() ) {
    Qtx::simplifySeparators( &menu );
    QAction* a = menu.exec( e->globalPos() );
    if ( a && actionMap.contains( a ) ) {
      setSectionHidden( actionMap[ a ], !isSectionHidden( actionMap[ a ] ) );
    }
    else if ( a && a == sortAction ) {
      setSortingEnabled( a->isChecked() );
    }
  }
  e->accept();
}

/*!
  \class QtxTreeView
  \brief Tree view class with possibility to display columns popup menu.

  The QtxTreeView class represents a customized tree view class. In addition to the
  base functionality inherited from the QTreeView class, clicking at the tree view
  header with the right mouse button displays the popup menu allowing the user
  to show/hide specified columns.

  By default the popup menu contains items corresponding to all the tree view columns.
  In order to disable some columns from being shown in the popup menu one may customize
  the data model (see QAbstractItemModel class). The custom model should implement
  headerData() method and return \c true for the Qtx::AppropriateRole role for
  those columns which should be available in the popup menu and \c false for the columns
  which should not be added to it.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxTreeView::QtxTreeView( QWidget* parent )
  : QTreeView( parent ),
    myKeySearchEnabled( true )
{
  setHeader( new Header( false, this ) );
}

/*!
  \brief Constructor.
  \param enableSortMenu show "Sorting" header menu command if \c true
  \param parent parent widget
*/
QtxTreeView::QtxTreeView( const bool enableSortMenu, QWidget* parent )
  : QTreeView( parent ),
    myKeySearchEnabled( true )

{
  setHeader( new Header( enableSortMenu, this ) );
}

/*!
  \brief Destructor.
*/
QtxTreeView::~QtxTreeView()
{
}

/*!
  \brief Expand all branches for specified number of levels.

  If \c levels < 0, all branches are expanded (the same results can
  be achieved with expandAll() method).

  \param levels number of levels to be opened
  \sa collapseLevels(), setOpened()
*/
void QtxTreeView::expandLevels( const int levels )
{
  setOpened( rootIndex(), levels+1, true );
}

/*!
  \brief Collapse all branches for specified number of levels.

  If \c levels < 0, all branches are collapsed (the same results can
  be achieved with collapseAll() method).

  \param levels number of levels to be collapsed
  \sa expandLevels(), setOpened()
*/
void QtxTreeView::collapseLevels( const int levels )
{
  setOpened( rootIndex(), levels+1, false );
}

/*!
  \brief Expand the branch specifed by the \index and all its
  children recursively.
  \param index model index to be expanded
  \sa collapseAll()
*/
void QtxTreeView::expandAll( const QModelIndex& index )
{
  setOpened( index, -1, true );
}

/*!
  \brief Collapse the branch specifed by the \index and all its
  children recursively.
  \param index model index to be collapsed
  \sa expandAll()
*/
void QtxTreeView::collapseAll( const QModelIndex& index )
{
  setOpened( index, -1, false );
}

/*!
  \brief Expand the all parent items of item specifed by the \index.
  \param index model index to be opened
  \sa expandAll()
*/
void QtxTreeView::expandTo( const QModelIndex& index )
{
  if ( !index.isValid() )
    return;

  QModelIndex idx = index.parent();
  while ( idx.isValid() ) {
    expand( idx );
    idx = idx.parent();
  }
}

/*
  \brief Enable/disable "Sorting" popup menu command for the header.
  \param enableSortMenu if \c true, enable "Sorting" menu command
  \sa sortMenuEnabled()
*/
void QtxTreeView::setSortMenuEnabled( const bool enableSortMenu )
{
  Header* h = dynamic_cast<Header*>( header() );
  if ( h )
    h->setSortMenuEnabled( enableSortMenu );
}

/*
  \brief Check if "Sorting" popup menu command for the header is enabled.
  \return \c true if "Sorting" menu command is enabled
  \sa setSortMenuEnabled()
*/
bool QtxTreeView::sortMenuEnabled() const
{
  Header* h = dynamic_cast<Header*>( header() );
  return h ? h->sortMenuEnabled() : false;
}

/*
  \brief Appends header menu action.
  \param action the action
*/
void QtxTreeView::addHeaderMenuAction( QAction* action )
{
  Header* h = dynamic_cast<Header*>( header() );
  if ( h )
    h->addMenuAction( action );
}

/*!
  \brief Resizes the given column in order to enclose its contents.
  The size will be changed only if it is smaller than the size of
  contents.
  \param column number of column
*/
void QtxTreeView::resizeColumnToEncloseContents( int column )
{
  if (column < 0 || column >= header()->count())
    return;

  int contentsSizeHint = sizeHintForColumn(column);
  int headerSizeHint = header()->isHidden() ? 0 : header()->sectionSizeHint(column);
  int sizeHint = qMax(contentsSizeHint, headerSizeHint);

  int currentSize = columnWidth( column );
  if (currentSize < sizeHint)
    setColumnWidth( column, sizeHint );
}

/*!
  \brief Enable/disable sorting operation for the view.
  \param enable if \c true, makes the tree view header clickable and shows sort indicator(s)
  \sa setMultiSortEnabled()
*/
void QtxTreeView::setSortingEnabled( const bool enable )
{
  Header* h = dynamic_cast<Header*>( header() );
  if ( h )
    h->setSortingEnabled( enable );
}

/*!
  \brief Query status of sorting (enabled or disabled).
  \return \c true if single or multiple sort indicators are shown
  \sa multiSortEnabled()
*/
bool QtxTreeView::sortingEnabled() const
{
  Header* h = dynamic_cast<Header*>( header() );
  return h && ( h->sortingEnabled() );
}

/*!
  \brief Enables multi-column sorting.
  As soon as multi-column sorting is enabled, the user can click
  several header sections in required order while holding <Ctrl> key so as to sort the contents
  of the tree view on a basis of these columns' values. The column clicked first
  has maximum sort priority, the column clicked last has minimum sort priority.
  Each column used for sorting has a sort indicator and sort priority value displayed
  in its header section.
  Note that sorting in general should be enabled first using setSortingEnabled().
  \param enable true to enable, otherwise false.
  \sa setSortingEnabled()
*/
void QtxTreeView::setMultiSortEnabled( const bool enable )
{
  Header* h = dynamic_cast<Header*>( header() );
  if ( h )
    h->setMultiSortEnabled( enable );
}

/*!
  \brief Returns true if multi-sorting is enabled, otherwise returns false.
  \return Multi-sorting status.
*/
bool QtxTreeView::multiSortEnabled() const
{
  Header* h = dynamic_cast<Header*>( header() );
  return h ? h->multiSortEnabled() : false;
}

/*
  \brief Called when the header section is clicked.
*/
void QtxTreeView::onHeaderClicked()
{
  Header* h            = dynamic_cast<Header*>( header() );
  QtxMultiSortModel* m = dynamic_cast<QtxMultiSortModel*>( model() );

  QApplication::setOverrideCursor( Qt::WaitCursor );

  if ( h && m && multiSortEnabled() ){
    QVector<int>           columns;
    QVector<Qt::SortOrder> orders;
    h->sortIndicators( columns, orders );
    m->multiSort( columns, orders );
  }
  else
    sortByColumn( header()->sortIndicatorSection(), header()->sortIndicatorOrder() );

  QApplication::restoreOverrideCursor();
}

/*!
  \brief Called when the selection is changed.

  Emits selectionChanged() signal.

  \param selected new selection
  \param deselected previous selection
*/
void QtxTreeView::selectionChanged( const QItemSelection& selected,
				    const QItemSelection& deselected )
{
  QTreeView::selectionChanged( selected, deselected );
  emit( selectionChanged() );
}

void QtxTreeView::drawRow( QPainter* painter, const QStyleOptionViewItem& option,
                           const QModelIndex& index ) const
{
  QTreeView::drawRow( painter, option, index );

  QtxTreeView* aThis = (QtxTreeView*)this;
  aThis->drawRow( index );
}

void QtxTreeView::drawRow( const QModelIndex& index )
{
  emit drawedRow( index );
}

/*!
  \brief Called when rows are about to be removed.
  \param parent model index
  \param start first row to remove
  \param end last row to remove
*/
void QtxTreeView::rowsAboutToBeRemoved( const QModelIndex& parent, int start, int end )
{
  setCurrentIndex( QModelIndex() );
  QTreeView::rowsAboutToBeRemoved( parent, start, end );
}

/*!
  \brief Expand/collapse the specified item (recursively).
  \param index model index
  \param levels number of levels to be expanded/collapsed
  \param open if \c true, item is expanded, otherwise it is collapsed
  \sa expandLevels(), collapseLevels()
*/
void QtxTreeView::setOpened( const QModelIndex& index, const int levels, bool open )
{
  if ( !levels )
    return;

  if ( !index.isValid() && index != rootIndex() )
    return;

  setExpanded( index, open );

  for ( int i = 0; i < model()->rowCount( index ); i++ ) {
    QModelIndex child = model()->index( i, 0, index );
    setOpened( child, levels-1, open );
  }
}

/*!
  \fn QtxTreeView::sortingEnabled( bool on );
  \brief Emitted when "Sorting" commans is enabled/disabled from the popup menu.
  \param on \c true if sorting is enabled and \c false otherwise
*/

/*!
  \fn QtxTreeView::selectionChanged();
  \brief Emitted when selection is changed in the tree view.
*/

/*!
  \brief Emit sortingEnabled(bool) signal.
  \param enabled "enable sorting" flag state
*/
void QtxTreeView::emitSortingEnabled( bool enabled )
{
  emit( sortingEnabled( enabled ) );
}

/*!
  \brief Returns true if the keyboard search is enabled.
*/
bool QtxTreeView::isKeyboardSearchEnabled() const
{
  return myKeySearchEnabled;
}

/*!
  \brief Enable/disable the keyboard search.
*/
void QtxTreeView::setKeyboardSearchEnabled( bool on )
{
  myKeySearchEnabled = on;
}

/*!
  \brief Moves to and selects the item best matching the string search.
  If keyboard search is disabled or no item is found nothing happens.
  \param search is pattern string
*/
void QtxTreeView::keyboardSearch( const QString& search )
{
  if ( isKeyboardSearchEnabled() )
    QAbstractItemView::keyboardSearch( search );
}

#include <QtxTreeView.moc>
