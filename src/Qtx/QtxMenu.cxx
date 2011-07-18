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
// File:      QtxMenu.cxx
// Author:    Sergey TELKOV
//

#include "QtxMenu.h"

#include <QSet>
#include <QLabel>
#include <QTimer>
#include <QLayout>
#include <QBitmap>
#include <QPixmap>
#include <QMenuBar>
#include <QPainter>
#include <QPaintEvent>
#include <QToolButton>
#include <QTextDocument>
#include <QWidgetAction>
#include <QLinearGradient>
#include <QAbstractTextDocumentLayout>

static const char* expand_button_xpm[] = {
/* width height num_colors chars_per_pixel */
"    18    18       16            1",
/* colors */
"` c none",
". c #f4f4f4",
"# c #f1f1f1",
"a c #f6f6f6",
"b c #f0f0f0",
"c c #ededed",
"d c #ebebeb",
"e c #eaeaea",
"f c #000000",
"g c #e8e8e8",
"h c #e7e7e7",
"i c #e5e5e5",
"j c #e4e4e4",
"k c #e2e2e2",
"l c #e1e1e1",
"m c #000000",
/* pixels */
"``````````````````",
"``````..####``````",
"````.aa..#bcc#````",
"```.aaa..#bcdeb```",
"``.aaaa..#bcdee#``",
"``.aaaf.##fcdegg``",
"``....ff#ffcdegh``",
"`#.....fffcdegghe`",
"`b#####bfcddeghhh`",
"`bbbbbfccdfegghii`",
"`bccccffdffeghijg`",
"`#dddddfffghiijjd`",
"``deeeeefhiijjkk``",
"``#ggghhiijjkkle``",
"```ciiijjkkklli```",
"````djjkkklllh````",
"``````gjklli``````",
"``````````````````"
};

 QtxMenu::PriorityMap QtxMenu::_actionPriority;

/*!
  \class QtxMenu::Title
  \brief Popup menu title item.
  \internal
*/

class QtxMenu::Title : public QWidget
{
public:
  Title( QtxMenu*, QWidget* = 0 );
  virtual ~Title();

  QIcon            icon() const;
  QString          text() const;
  QtxMenu*         menu() const;
  Qt::Alignment    alignment() const;

  QTextDocument&   textDocument() const;

  virtual QSize    sizeHint() const;
  virtual QSize    minimumSizeHint() const;

protected:
  virtual void     paintEvent( QPaintEvent* );

private:
  QtxMenu*         myMenu;
};

/*!
  \brief Constructor.
  \param parent parent widget
  \internal
*/
QtxMenu::Title::Title( QtxMenu* menu, QWidget* parent )
: QWidget( parent ),
myMenu( menu )
{
}

/*!
  \brief Destructor.
  \internal
*/
QtxMenu::Title::~Title()
{
}

/*!
  \brief Get the menu which manage this title.
  \return menu object
  \internal
*/
QtxMenu* QtxMenu::Title::menu() const
{
  return myMenu;
}

/*!
  \brief Get title menu icon.
  \return menu icon for the title item
  \internal
*/
QIcon QtxMenu::Title::icon() const
{
  QIcon ico;
  if ( menu() )
    ico = menu()->icon();
  return ico;
}

/*!
  \brief Get title menu text.
  \return menu text for the title item
  \internal
*/
QString QtxMenu::Title::text() const
{
  QString txt;
  if ( menu() )
    txt = menu()->title();
  return txt;
}

/*!
  \brief Get title alignment flags.
  \return title alignment flags
  \internal
*/
Qt::Alignment QtxMenu::Title::alignment() const
{
  Qt::Alignment align;
  if ( menu() )
    align = menu()->titleAlignment();
  return align;
}

/*!
  \brief Returns the prepared text document engine.
  \return text document reference
  \internal
*/
QTextDocument& QtxMenu::Title::textDocument() const
{
  static QTextDocument _doc;
  _doc.setHtml( text() );
  QFont f = font();
  f.setBold( true );
  _doc.setDefaultFont( f );
  return _doc;
}

/*!
  \brief Get recommended size for the title item widget.
  \return title item widget size
  \internal
*/
QSize QtxMenu::Title::sizeHint() const
{
  int m = 3;
  QTextDocument& doc = textDocument();

  QSize sz = icon().isNull() ? QSize( 0, 0 ) : icon().actualSize( QSize( 16, 16 ) );
  sz.setWidth( 2 * m + sz.width() + (int)doc.size().width() );
  sz.setHeight( 2 * m + qMax( sz.height(), (int)doc.size().height() ) );
  return sz;
}

/*!
  \brief Get recommended minimum size for the title item widget.
  \return title item widget minimum size
  \internal
*/
QSize QtxMenu::Title::minimumSizeHint() const
{
  return sizeHint();
}

/*!
  \brief Paint the title item widget.
  \param e paint event (not used)
  \internal
*/
void QtxMenu::Title::paintEvent( QPaintEvent* )
{
  int m = 5;
  QIcon ico = icon();
  QString txt = text();
  Qt::Alignment align = alignment();

  QRect base = rect();
  base.setTop( base.top() + 1 );
  base.setLeft( base.left() + 1 );
  base.setRight( base.right() -1 );
  base.setBottom( base.bottom() - 1 );

  QTextDocument& doc = textDocument();

  QSize isz = ico.isNull() ? QSize( 0, 0 ) : ico.actualSize( QSize( 16, 16 ) );
  QSize sz( (int)doc.size().width(), (int)doc.size().height() );

  QPainter p( this );
  QAbstractTextDocumentLayout::PaintContext ctx;
  ctx.palette.setColor( QPalette::Text, palette().color( QPalette::Light ) );

  QLinearGradient linearGrad( base.topLeft(), base.topRight() );
  linearGrad.setColorAt( 0, palette().color( QPalette::Highlight ) );
  linearGrad.setColorAt( 1, palette().color( QPalette::Window ) );

  p.fillRect( base, linearGrad );

  QPoint start = base.topLeft() + QPoint( m, m );
  if ( align & Qt::AlignLeft )
    start.setX( base.left() + m );
  else if ( align & Qt::AlignRight )
    start.setX( base.right() - m - isz.width() - sz.width() );
  else if ( align & Qt::AlignHCenter )
    start.setX( base.left() + ( base.width() - isz.width() - sz.width() ) / 2 );

  if ( align & Qt::AlignTop )
    start.setY( base.top() + m );
  else if ( align & Qt::AlignBottom )
    start.setY( base.bottom() - m - qMax( isz.height(), - sz.height() ) );
  else if ( align & Qt::AlignVCenter )
    start.setY( base.top() + ( base.height() - qMax( isz.height(), sz.height() ) ) / 2 );

  if ( !ico.isNull() )
  {
    ico.paint( &p, QRect( start, isz ) );
    start.setX( start.x() + isz.width() );
  }

  p.save();
  p.translate( start );
  doc.documentLayout()->draw( &p, ctx );
  p.restore();
}

/*!
  \class QtxMenu::TitleMgr
  \brief Widget action for placing title widget into popup.
  \internal
*/

class QtxMenu::TitleMgr : public QWidgetAction
{
public:
  TitleMgr( QtxMenu* );
  virtual ~TitleMgr();

protected:
  virtual QWidget* createWidget( QWidget* );
  virtual void     deleteWidget( QWidget* );

private:
  QtxMenu* myMenu;
};

/*!
  \brief Constructor.
  \internal
*/
QtxMenu::TitleMgr::TitleMgr( QtxMenu* parent )
: QWidgetAction( parent ),
  myMenu( parent )
{
}

/*!
  \brief Destructor.
  \internal
*/
QtxMenu::TitleMgr::~TitleMgr()
{
}

/*!
  \brief Creates the menu title widget when action added into menu.
  \internal
*/
QWidget* QtxMenu::TitleMgr::createWidget( QWidget* parent )
{
  return new Title( myMenu, parent );
}

/*!
  \brief Deletes the menu title widget when action removed from menu.
  \internal
*/
void QtxMenu::TitleMgr::deleteWidget( QWidget* w )
{
  delete w;
}

/*!
  \class QtxMenu::Expander
  \brief Widget action which represent expand button into popup menu.
  \internal
*/
class QtxMenu::Expander : public QWidgetAction
{
public:
  Expander( QWidget* = 0 );
  virtual ~Expander();

protected:
  virtual QWidget* createWidget( QWidget* );
  virtual void     deleteWidget( QWidget* );
};

/*!
  \brief Constructor.
  \internal
*/
QtxMenu::Expander::Expander( QWidget* parent )
: QWidgetAction( parent )
{
  setText( tr( "Expand" ) );
  setToolTip( tr( "Expand" ) );

  setEnabled( false );
}

/*!
  \brief Destructor.
  \internal
*/
QtxMenu::Expander::~Expander()
{
}

/*!
  \brief Creates the expanding button when action added into menu.
  \internal
*/
QWidget* QtxMenu::Expander::createWidget( QWidget* parent )
{
  class Button : public QToolButton
  {
  public:
    Button( QWidget* p = 0 ) : QToolButton( p ) {};
    virtual ~Button() {};

  protected:
    virtual void resizeEvent( QResizeEvent* e )
    {
      QToolButton::resizeEvent( e );

      QPixmap pix( size() );
      QPainter p( &pix );
      icon().paint( &p, rect() );
      p.end();

      if ( pix.mask().isNull() )
      {
        QBitmap bm;
        QImage img = pix.toImage();
	      if ( img.hasAlphaChannel() )
          bm = QPixmap::fromImage( img.createAlphaMask() );
	      else
          bm = QPixmap::fromImage( img.createHeuristicMask() );

	      pix.setMask( bm );
	    }

      if ( !pix.mask().isNull() )
	      setMask( pix.mask() );
    };
  };

  QToolButton* tb = new Button( parent );
  QPixmap pix( expand_button_xpm );
  tb->setIcon( pix );
  tb->setAutoRaise( true );

  connect( tb, SIGNAL( clicked( bool ) ), this, SIGNAL( triggered( bool ) ) );

  return tb;
}

/*!
  \brief Deletes the expanding button when action removed from menu.
  \internal
*/
void QtxMenu::Expander::deleteWidget( QWidget* w )
{
  delete w;
}

/*!
  \class QtxMenu
  \brief The class QtxMenu represents the popup menu with the title.

  The title for the popup menu can be set via setTitleText() method.
  In addition, title item can contain the icon, which can be set using
  setTitleIcon() method. Current title text and icon can be retrieved with
  titleText() and titleIcon() methods.

  The title text alignment flags can be changed using setTitleAlignment()
  method and retrieved with titleAlignment() method.

  By default, QtxMenu::TitleAuto mode is used. In this mode, the title item
  is shown only if it is not empty. To show title always (even empty), pass
  QtxMenu::TitleOn to the setTitleMode() method. To hide the title, use
  setTitleMode() method with QtxMenu::TitleOff parameter.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxMenu::QtxMenu( QWidget* parent )
: QMenu( parent ),
  myTitleMode( TitleOff ),
  myTitleAlign( Qt::AlignVCenter | Qt::AlignLeft ),
  myLimit( 7 ),
  myLimitMode( LimitTotal ),
  myExpandAction( 0 )
{
  myTitleAction = new TitleMgr( this );

  myShortTimer = new QTimer( this );
  myShortTimer->setSingleShot( true );
  myShortTimer->setInterval( 1000 );

  myExpandTimer = new QTimer( this );
  myExpandTimer->setSingleShot( true );
  myExpandTimer->setInterval( 5000 );

  connect( menuAction(), SIGNAL( changed() ), this, SLOT( onMenuActionChanged() ) );

  connect( myShortTimer, SIGNAL( timeout() ), this, SLOT( onExpandMenu() ) );
  connect( myExpandTimer, SIGNAL( timeout() ), this, SLOT( onExpandMenu() ) );

  connect( this, SIGNAL( hovered( QAction* ) ), this, SLOT( onActionHovered( QAction* ) ) );
  connect( this, SIGNAL( triggered( QAction* ) ), this, SLOT( onActionTriggered( QAction* ) ) );
}

/*!
  \brief Destructor.
*/
QtxMenu::~QtxMenu()
{
}

/*!
  \brief Get title item display mode.
  \return popup menu title display mode (QtxMenu::TitleMode)
*/
QtxMenu::TitleMode QtxMenu::titleMode() const
{
  return myTitleMode;
}

/*!
  \brief Get title alignment flags.
  \return title alignment flags
*/
Qt::Alignment QtxMenu::titleAlignment() const
{
  return myTitleAlign;
}

/*!
  \brief Set title item display mode.
  \param m popup menu title display mode (QtxMenu::TitleMode)
*/
void QtxMenu::setTitleMode( const QtxMenu::TitleMode m )
{
  if ( myTitleMode == m )
    return;

  myTitleMode = m;

  updateTitle();
}

/*!
  \brief Set title alignment flags.
  \param a title alignment flags
*/
void QtxMenu::setTitleAlignment( const Qt::Alignment a )
{
  if ( titleAlignment() == a )
    return;

  myTitleAlign = a;

  updateTitle();
}

/*!
  \brief Returns 'true' if the menu should be collapsed when displayed.
*/
bool QtxMenu::menuCollapsible() const
{
  return myExpandAction;
}

/*!
  \brief Sets the menu collapsible property.
*/
void QtxMenu::setMenuCollapsible( bool on )
{
  if ( menuCollapsible() == on )
    return;

  if ( on )
    myExpandAction = new Expander( this );
  else
  {
    if ( isMenuCollapsed() )
      expandMenu();

    delete myExpandAction;
    myExpandAction = 0;
  }
}

/*!
  \brief Returns the delay in milliseconds for automatic menu expanding.
         If delay is zero then menu doesn't perform automatic expanding.
*/
int QtxMenu::expandingDelay() const
{
  return myExpandTimer->interval();
}

/*!
  \brief Sets the automatic menu expanding delay in milliseconds.
         If delay is zero then menu doesn't perform automatic expanding.
*/
void QtxMenu::setExpandingDelay( int msec )
{
  myExpandTimer->setInterval( msec );
  if ( msec == 0 )
  {
    myShortTimer->stop();
    myExpandTimer->stop();
  }
}

/*!
  \brief Returns number of visible menu items in collapsed state.
*/
int QtxMenu::collapseLimit() const
{
  return myLimit;
}

/*!
  \brief Sets number of visible menu items in collapsed state.
  \param num - number of visible items.
*/
void QtxMenu::setCollapseLimit( int num )
{
  myLimit = num;
}

/*!
  \brief Returns collapse limit mode.
*/
QtxMenu::CollapseLimitMode QtxMenu::collapseLimitMode() const
{
  return myLimitMode;
}

/*!
  \brief Sets collapse limit mode. If mode is 'LimitFrequent' then parameter 'collapse limit'
         will be applied to most frequent items only. If mode is 'LimitTotal' then parameter
         'collapse limit' will be limit total quantity of menu items (permanent and most frequent).
  \param mode - setted collapse limit mode.
*/
void QtxMenu::setCollapseLimitMode( const QtxMenu::CollapseLimitMode mode )
{
  myLimitMode = mode;
}

/*!
  \brief Returns 'true' if the menu in expanded (full) state.
*/
bool QtxMenu::isMenuExpanded() const
{
  return !isMenuCollapsed();
}

/*!
  \brief Returns 'true' if the menu in collapsed state.
*/
bool QtxMenu::isMenuCollapsed() const
{
  return myExpandAction && myExpandAction->isVisible()
      && actions().contains( myExpandAction );
}

/*!
  \brief Returns the priority for specified action. Priority of the action
         will be automatically increased by 1 during action activation.
  \param a - action.
*/
int QtxMenu::actionPriority( QAction* a )
{
  int p = 0;
  if ( _actionPriority.contains( a ) )
    p = _actionPriority[a];

  if ( a->menu() )
  {
    QList<QAction*> lst = a->menu()->actions();
    for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it )
      p = qMax( p, _actionPriority.contains( *it ) ? _actionPriority[*it] : 0 );
  }
  return p;
}

/*!
  \brief Sets the priority for specified action. Action with negative value of priority
         will be always displayed in menu (permanent actions).
  \param a - action.
*/
void QtxMenu::setActionPriority( QAction* a, int p )
{
  if ( a )
    _actionPriority.insert( a, p );
}

/*!
  \brief Customize show/hide menu operation.
  \param on new popup menu visibility state
*/
void QtxMenu::setVisible( bool on )
{
  if ( on )
  {
    if ( menuCollapsible() && isTopLevelMenu() )
    {
      if ( isMenuExpanded() )
        collapseMenu();
      else
      {
        updateExpander();
        if ( expandingDelay() > 0 )
          myExpandTimer->start();
      }
    }

    insertTitle();
  }

  QMenu::setVisible( on );

  if ( !on )
  {
    if ( !isTearOffMenuVisible() )
    {
      removeTitle();
      expandMenu();
    }

    myShortTimer->stop();
    myExpandTimer->stop();
  }
}

/*!
  \brief Reimplemented for internal reasons.
         Activation the expand item by keys perform menu expanding.
*/
void QtxMenu::keyPressEvent( QKeyEvent* e )
{
  QAction* cur = activeAction();

  QMenu::keyPressEvent( e );

  if ( cur != activeAction() && isMenuCollapsed() &&
       activeAction() == myExpandAction )
    expandMenu();
}

/*!
  \brief Reimplemented for internal reasons.
*/
void QtxMenu::actionEvent( QActionEvent* e )
{
  myVisibilityState.insert( e->action(),
                            e->action()->isVisible() );

  QMenu::actionEvent( e );
}

/*!
  \brief Insert title item to the popup menu.
*/
void QtxMenu::insertTitle()
{
  if ( titleMode() == TitleOff || ( titleMode() == TitleAuto && title().trimmed().isEmpty() ) )
    return;

  myTitleAction->setIcon( icon() );
  myTitleAction->setText( title() );

  if ( actions().contains( myTitleAction ) )
    removeAction( myTitleAction );

  if ( actions().isEmpty() )
    addAction( myTitleAction );
  else
    insertAction( actions().first(), myTitleAction );
}

/*!
  \brief Remove title item from the popup menu.
*/
void QtxMenu::removeTitle()
{
  if ( actions().contains( myTitleAction ) )
    removeAction( myTitleAction );
}

/*!
  \brief Update title item.
*/
void QtxMenu::updateTitle()
{
  if ( !actions().contains( myTitleAction ) )
    return;

  removeTitle();
  insertTitle();
}

/*!
  \brief Returns the top level menu.
*/
QWidget* QtxMenu::topLevelMenu() const
{
  return topLevelMenu( this );
}

/*!
  \brief Returns the top level menu for given menu.
  \param menu - start menu which will be used for top level menu search.
  \internal
*/
QWidget* QtxMenu::topLevelMenu( const QMenu* menu ) const
{
  if ( !menu )
     return 0;

  QList<QWidget*> lst;
  if ( menu->menuAction() )
    lst = menu->menuAction()->associatedWidgets();

  QWidget* w = 0;

  QMenu* pm = 0;
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end() && !pm; ++it )
  {
    QMenu* m = ::qobject_cast<QMenu*>( *it );
    if ( m && m->isVisible() )
      pm = m;
  }

  if ( pm )
    w = topLevelMenu( pm );

  if ( !w )
    w = (QWidget*)menu;

  return w;
}

/*!
  \brief Returns 'true' if the menu is top level.
         Works correctly only when menu is shown.
*/
bool QtxMenu::isTopLevelMenu() const
{
  return topLevelMenu() == this;
}

/*!
  \brief Expand the menu. Display menu in full state. Shows all required menu items.
  \internal
*/
void QtxMenu::expandMenu()
{
  if ( isMenuExpanded() )
    return;

  QList<QAction*> lst = actions();
  for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QAction* a = *it;
    if ( a == myTitleAction || a == myExpandAction )
      continue;

    a->setVisible( myVisibilityState.contains( a ) ? myVisibilityState[a] : false );
  }

  myShortTimer->stop();
  myExpandTimer->stop();

  if ( myExpandAction )
    removeAction( myExpandAction );

  myVisibilityState.clear();
}

/*!
  \brief Collaps the menu. Display menu in compact state. Shows most frequently used menu items only.
  \internal
*/
void QtxMenu::collapseMenu()
{
  QList<QAction*> lst = actions();

  QSet<QAction*> visible;
  QMap<int, QList<QAction*> > freqMap;

  VisibilityMap aVisBackup;
  QList< QPair<int, QAction*> > freqList;

  for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QAction* a = *it;
    aVisBackup.insert( a, a->isVisible() );

    if ( a->isSeparator() || !a->isVisible() )
      continue;

    int priority = actionPriority( a );
    if ( priority < 0 )
      visible.insert( a );
    else
    {
      if ( !freqMap.contains( priority ) )
        freqMap.insert( priority, QList<QAction*>() );
      freqMap[priority].append( a );
    }
  }

  int limit = collapseLimit();
  if ( collapseLimitMode() == LimitTotal )
    limit -= visible.count();

  if ( limit > 0 )
  {
    QList<int> freqList = freqMap.keys();
    int i = freqList.count() - 1;
    for ( int c = 0; c < limit && i >= 0; i-- )
    {
      QList<QAction*> lst = freqMap[freqList[i]];
      for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end() && c < limit; ++it, c++ )
        visible.insert( *it );
    }
  }

  int hidden = 0;
  for ( QList<QAction*>::iterator itr = lst.begin(); itr != lst.end(); ++itr )
  {
    QAction* a = *itr;
    bool vis = a == myExpandAction || a->isSeparator() || visible.contains( a );
    a->setVisible( vis );
    if ( !vis && aVisBackup[a] )
      hidden++;
  }

  if ( myExpandAction )
    myExpandAction->setVisible( hidden );

  updateExpander();

  if ( expandingDelay() > 0 )
    myExpandTimer->start();

  myVisibilityState = aVisBackup;
}

/*!
  \brief Updates the expand item in menu.
  \internal
*/
void QtxMenu::updateExpander()
{
  if ( !myExpandAction )
    return;

  if ( actions().contains( myExpandAction ) )
    removeAction( myExpandAction );

  addAction( myExpandAction );
}

/*!
  \brief Updates title widget when menu action changed.
  \internal
*/
void QtxMenu::onMenuActionChanged()
{
  updateTitle();
}

/*!
  \brief Expand menu by timeout signal.
  \internal
*/
void QtxMenu::onExpandMenu()
{
  expandMenu();
}

/*!
  \brief Start the short automatic expanding timer when mouse cursor hover expand item.
  \internal
*/
void QtxMenu::onActionHovered( QAction* a )
{
  if ( a == myExpandAction && expandingDelay() )
    myShortTimer->start();
  else
    myShortTimer->stop();
}

/*!
  \brief Expanding menu when exanding button activated.
         Increase priority for activated action.
  \internal
*/
void QtxMenu::onActionTriggered( QAction* a )
{
  if ( a == myExpandAction )
    expandMenu();
  else
  {
    int priority = actionPriority( a );
    if ( priority >= 0 )
      priority++;
    setActionPriority( a, priority );
  }
}
