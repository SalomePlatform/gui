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

//static const char* expand_button_xpm[] = {
/* width height num_colors chars_per_pixel */
//"    18    18       16            1",
/* colors */
/*"` c none",
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
"m c #000000",*/
/* pixels */
/*"``````````````````",
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
*/
static const char* expand_button_xpm[] = {
/* width height num_colors chars_per_pixel */
"    18    18      256            2",
/* colors */
"`` c none",
"`. c #fefefe",
"`# c #fdfdfd",
"`a c #fcfcfc",
"`b c #fbfbfb",
"`c c #fafafa",
"`d c #f8f8f8",
"`e c #f6f6f6",
"`f c #f4f4f4",
"`g c #f9f9f9",
"`h c #f7f7f7",
"`i c #f1f1f1",
"`j c #eeeeee",
"`k c #ececec",
"`l c #ebebeb",
"`m c #efefef",
"`n c #5f5f5f",
"`o c #5c5c5c",
"`p c #a7a7a7",
"`q c #f2f2f2",
"`r c #b0b0b0",
"`s c #181818",
"`t c #d3d3d3",
"`u c #eaeaea",
"`v c #e8e8e8",
"`w c #929292",
"`x c #2d2d2d",
"`y c #000000",
"`z c #040404",
"`A c #3d3d3d",
"`B c #353535",
"`C c #0f0f0f",
"`D c #202020",
"`E c #727272",
"`F c #e9e9e9",
"`G c #dddddd",
"`H c #949494",
"`I c #494949",
"`J c #2a2a2a",
"`K c #787878",
"`L c #c4c4c4",
"`M c #e4e4e4",
"`N c #dbdbdb",
"`O c #e6e6e6",
"`P c #323232",
"`Q c #262626",
"`R c #6f6f6f",
"`S c #b7b7b7",
"`T c #f0f0f0",
"`U c #c8c8c8",
"`V c #797979",
"`W c #2b2b2b",
"`X c #cacaca",
"`Y c #e5e5e5",
"`Z c #e3e3e3",
"`0 c #b8b8b8",
"`1 c #606060",
"`2 c #0d0d0d",
"`3 c #0c0c0c",
"`4 c #505050",
"`5 c #a1a1a1",
"`6 c #c3c3c3",
"`7 c #7b7b7b",
"`8 c #5e5e5e",
".` c #a5a5a5",
".. c #e0e0e0",
".# c #ededed",
".a c #e1e1e1",
".b c #e2e2e2",
".c c #dfdfdf",
".d c #000000",
".e c #000000",
".f c #000000",
".g c #000000",
".h c #000000",
".i c #000000",
".j c #000000",
".k c #000000",
".l c #000000",
".m c #000000",
".n c #000000",
".o c #000000",
".p c #000000",
".q c #000000",
".r c #000000",
".s c #000000",
".t c #000000",
".u c #000000",
".v c #000000",
".w c #000000",
".x c #000000",
".y c #000000",
".z c #000000",
".A c #000000",
".B c #000000",
".C c #000000",
".D c #000000",
".E c #000000",
".F c #000000",
".G c #000000",
".H c #000000",
".I c #000000",
".J c #000000",
".K c #000000",
".L c #000000",
".M c #000000",
".N c #000000",
".O c #000000",
".P c #000000",
".Q c #000000",
".R c #000000",
".S c #000000",
".T c #000000",
".U c #000000",
".V c #000000",
".W c #000000",
".X c #000000",
".Y c #000000",
".Z c #000000",
".0 c #000000",
".1 c #000000",
".2 c #000000",
".3 c #000000",
".4 c #000000",
".5 c #000000",
".6 c #000000",
".7 c #000000",
".8 c #000000",
"#` c #000000",
"#. c #000000",
"## c #000000",
"#a c #000000",
"#b c #000000",
"#c c #000000",
"#d c #000000",
"#e c #000000",
"#f c #000000",
"#g c #000000",
"#h c #000000",
"#i c #000000",
"#j c #000000",
"#k c #000000",
"#l c #000000",
"#m c #000000",
"#n c #000000",
"#o c #000000",
"#p c #000000",
"#q c #000000",
"#r c #000000",
"#s c #000000",
"#t c #000000",
"#u c #000000",
"#v c #000000",
"#w c #000000",
"#x c #000000",
"#y c #000000",
"#z c #000000",
"#A c #000000",
"#B c #000000",
"#C c #000000",
"#D c #000000",
"#E c #000000",
"#F c #000000",
"#G c #000000",
"#H c #000000",
"#I c #000000",
"#J c #000000",
"#K c #000000",
"#L c #000000",
"#M c #000000",
"#N c #000000",
"#O c #000000",
"#P c #000000",
"#Q c #000000",
"#R c #000000",
"#S c #000000",
"#T c #000000",
"#U c #000000",
"#V c #000000",
"#W c #000000",
"#X c #000000",
"#Y c #000000",
"#Z c #000000",
"#0 c #000000",
"#1 c #000000",
"#2 c #000000",
"#3 c #000000",
"#4 c #000000",
"#5 c #000000",
"#6 c #000000",
"#7 c #000000",
"#8 c #000000",
"a` c #000000",
"a. c #000000",
"a# c #000000",
"aa c #000000",
"ab c #000000",
"ac c #000000",
"ad c #000000",
"ae c #000000",
"af c #000000",
"ag c #000000",
"ah c #000000",
"ai c #000000",
"aj c #000000",
"ak c #000000",
"al c #000000",
"am c #000000",
"an c #000000",
"ao c #000000",
"ap c #000000",
"aq c #000000",
"ar c #000000",
"as c #000000",
"at c #000000",
"au c #000000",
"av c #000000",
"aw c #000000",
"ax c #000000",
"ay c #000000",
"az c #000000",
"aA c #000000",
"aB c #000000",
"aC c #000000",
"aD c #000000",
"aE c #000000",
"aF c #000000",
"aG c #000000",
"aH c #000000",
"aI c #000000",
"aJ c #000000",
"aK c #000000",
"aL c #000000",
"aM c #000000",
"aN c #000000",
"aO c #000000",
"aP c #000000",
"aQ c #000000",
"aR c #000000",
"aS c #000000",
"aT c #000000",
"aU c #000000",
"aV c #000000",
"aW c #000000",
"aX c #000000",
"aY c #000000",
"aZ c #000000",
"a0 c #000000",
"a1 c #000000",
"a2 c #000000",
"a3 c #000000",
"a4 c #000000",
"a5 c #000000",
"a6 c #000000",
"a7 c #000000",
"a8 c #000000",
/* pixels */
"````````````````````````````````````",
"```````````.`#`a`b`c`d`d`d``````````",
"```````````.`.`#`a`c`d`e`f`f````````",
"`````````````.`#`a`c`g`h`f`i`j``````",
"`````````````.`#`a`c`g`h`f`i`j`k````",
"`````.`````.`.`#`a`c`d`e`f`i`j`l`m``",
"```#`.`.`n`o`p`l`b`g`q`r`n`s`t`u`v``",
"```a`#`#`w`x`y`z`A`B`C`y`D`E`F`u`v``",
"```b`a`a`a`a`G`H`I`J`K`L`i`j`l`v`M``",
"```g`c`c`c`c`g`g`h`e`f`q`m`N`u`O`M``",
"```g`d`g`P`Q`R`S`T`f`U`V`W`y`X`Y`Z``",
"```d`e`h`0`1`s`y`2`y`y`3`4`5`O`Z`Z``",
"```h`f`f`f`f`q`6`7`8.``M`F`O`Z..`v``",
"`````q`i`i`i`i`m`j.#`l`F`O`Z.a.b````",
"```````j`j`j.#`k`l`u`v`O`Z.a.c``````",
"````````.#`l`u`F`v`O`M`Z...c````````",
"```````````q`F`F`v`Y`Y`Z`m``````````",
"````````````````````````````````````"
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
  myExpandTimer->setInterval( 0 );
  //  myExpandTimer->setInterval( 5000 );

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

void QtxMenu::paintEvent( QPaintEvent* e )
{
  QPixmap pix( rect().size() );
  pix.fill( this, 0, 0 );

  QPainter::setRedirected( this, &pix );
  QMenu::paintEvent( e );
  QPainter::restoreRedirected( this );

  if ( isTopLevelMenu() ) {
    QRgb bg = palette().color( QPalette::Light ).rgb();
    QImage img = pix.toImage();

    QList<QAction*> lst = actions();
    QSet<QAction*> visible = collapsedActions();

    for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it ) {
      QAction* a = *it;
      QRect r = actionGeometry( a );

      int x, y, w, h;
      r.getRect( &x, &y, &w, &h );

      if ( a == myExpandAction || a == myTitleAction || !visible.contains( a ) )
	continue;

      QRgb rc = img.pixel( x, y );
      for ( int i = 0; i < w - 1; i++ ) {
	for ( int j = 0; j < h; j++ ) {
	  if ( img.pixel( x + i, y + j ) == rc ) {
	    img.setPixel( x + i, y + j, bg );
	  }
	}
      }
    }

    pix = QPixmap::fromImage( img );
  }

  QPainter p( this );
  p.drawPixmap( rect(), pix );
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

  QSet<QAction*> set = actions().toSet();

  QList<QAction*> bakList = myActionBackup;

  for ( QList<QAction*>::iterator itr = myActionBackup.begin(); itr != myActionBackup.end(); ++itr )
  {
    QAction* a = *itr;

    if ( set.contains( a ) )
      continue;

    QAction* before = 0;
    for ( QList<QAction*>::iterator it = itr; it != myActionBackup.end() && !before; ++it )
    {
      if ( set.contains( *it ) )
	before = *it;
    }

    insertAction( before, a );
  }

  myShortTimer->stop();
  myExpandTimer->stop();

  if ( myExpandAction )
    removeAction( myExpandAction );

  myActionBackup.clear();
}

/*!
  \brief Collaps the menu. Display menu in compact state. Shows most frequently used menu items only.
  \internal
*/
void QtxMenu::collapseMenu()
{
  ActionList lst = actions();
  ActionList anActionBackup = lst;

  QSet<QAction*> visible = collapsedActions();

  int hidden = 0;
  for ( QList<QAction*>::iterator itr = lst.begin(); itr != lst.end(); ++itr )
  {
    QAction* a = *itr;
    bool vis = a == myExpandAction || a->isSeparator() || visible.contains( a );
    if ( !vis && a->isVisible() )
    {
      hidden++;
      removeAction( a );
    }
  }

  if ( myExpandAction )
    myExpandAction->setVisible( hidden );

  updateExpander();

  if ( expandingDelay() > 0 )
    myExpandTimer->start();

  myActionBackup = anActionBackup;
}

/*!
  \brief Returns the set of the action which will be visible in collapsed state.
  \internal
*/
QSet<QAction*> QtxMenu::collapsedActions() const
{
  QList<QAction*> lst;
  
  lst = !myActionBackup.isEmpty() ? myActionBackup : actions();

  QSet<QAction*> visible;
  QMap<int, QList<QAction*> > freqMap;
  for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QAction* a = *it;
    if ( a->isSeparator() || !a->isVisible() )
      continue;

    int priority = a == defaultAction() ? -1 : actionPriority( a );
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

  return visible;
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
