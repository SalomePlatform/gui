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
// File:      QtxDockWidget.cxx
// Author:    Sergey TELKOV

#include "QtxDockWidget.h"

#include <QAction>
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QResizeEvent>
#include <QStyleOption>
#include <QStylePainter>
#include <QToolButton>


/*!
  \class QtxDockWidgetTitle
  \internal
  \brief Internal class that implements customizable title bar for a dock widget.

  This is a convenient implementation of an application-defined title bar widget.
  It contains a label with the window title, Undock and Close buttons by default, 
  and any number of widgets can be inserted at required position into the title's QHBoxLayout.
  Support for vertical title bar is not yet implemented.
*/

class QtxDockWidgetTitle : public QWidget
{
  Q_OBJECT

public:

  QtxDockWidgetTitle( QtxDockWidget* parent );
  virtual ~QtxDockWidgetTitle();

  QWidget*            widget( const int ) const;
  void                insertWidget( const int, QWidget* widget, const int = QtxDockWidget::Last );
  void                removeWidget( const int );

protected:
  virtual void        paintEvent ( QPaintEvent* event );

private slots:
  virtual void        updateTitleBar();

private:
  void                initStyleOption( QStyleOptionDockWidget* ) const;
  void                setupButton( QToolButton*, const int, QStyleOptionDockWidget*, QDockWidget* );

private:
  typedef QPair< int, QWidget* > RoleWidget;
  typedef QList< RoleWidget > RoleWidgetList;

  RoleWidgetList      myWidgets;
};

static inline bool hasFeature(const QDockWidget *dockwidget, QDockWidget::DockWidgetFeature feature)
{ return (dockwidget->features() & feature) == feature; }

QtxDockWidgetTitle::QtxDockWidgetTitle( QtxDockWidget* parent )
: QWidget( parent )
{
  setAttribute( Qt::WA_NoSystemBackground );

  // TODO: vertical title bar not yet implemented
  QHBoxLayout* hbox = new QHBoxLayout( this );
  const int titleMargin  = 3;
  const int titleSpacing = 1;
  hbox->setContentsMargins( titleMargin, titleMargin, titleMargin, titleMargin );
  hbox->setSpacing( titleSpacing );

  QStyleOptionDockWidgetV2 opt;
  initStyleOption( &opt );

  QLabel* titleLab = new QLabel( parent->windowTitle(), this );
  titleLab->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );
  myWidgets.push_back( RoleWidget( QtxDockWidget::TitleText, titleLab ) );

  QToolButton* btn;
  for ( int i = QtxDockWidget::Undock; i < QtxDockWidget::Last; i++ ){
    btn = new QToolButton( this );
    setupButton( btn, i, &opt, parent );
    myWidgets.push_back( RoleWidget( i, btn ) );
  }

  RoleWidgetList::const_iterator anIt  = myWidgets.begin();
  RoleWidgetList::const_iterator anEnd = myWidgets.end();
  for ( ; anIt != anEnd; anIt++ )
    hbox->addWidget( (*anIt).second );

  btn = qobject_cast<QToolButton*>( widget( QtxDockWidget::Undock ) );
  connect( btn, SIGNAL( clicked( bool ) ), parent, SLOT( toggleTopLevel() ) );

  btn = qobject_cast<QToolButton*>( widget( QtxDockWidget::Close ) );
  connect( btn, SIGNAL( clicked( bool ) ), parent, SLOT( close() ) );

  connect( parent, SIGNAL( featuresChanged ( QDockWidget::DockWidgetFeatures ) ), this, SLOT( updateTitleBar() ) );
  connect( parent, SIGNAL( topLevelChanged( bool ) ), this, SLOT( updateTitleBar() ) );
}

QtxDockWidgetTitle::~QtxDockWidgetTitle()
{
  // NOTE: no need to delete the widget's contents here, as all inserted widgets are 
  // the title bar's children, see insertWidget() method.
}

QWidget* QtxDockWidgetTitle::widget( const int role ) const
{
  RoleWidgetList::const_iterator anIt  = myWidgets.begin();
  RoleWidgetList::const_iterator anEnd = myWidgets.end();
  for ( ; anIt != anEnd; anIt++ )
    if ( (*anIt).first == role )
      return (*anIt).second;
  return 0;
}

void QtxDockWidgetTitle::insertWidget( const int role, QWidget* widget, const int beforeRole )
{
  QHBoxLayout* hbox = qobject_cast<QHBoxLayout*>( layout() );

  removeWidget( role );

  // Clear the layout and re-fill it in proper order
  RoleWidgetList::iterator anIt  = myWidgets.begin();
  RoleWidgetList::iterator anEnd = myWidgets.end();
  RoleWidgetList::iterator insertIt = myWidgets.end();
  for ( ; anIt != anEnd; anIt++ ){
    hbox->removeWidget( (*anIt).second );
    if ( (*anIt).first == beforeRole )
      insertIt = anIt;
  }

  myWidgets.insert( insertIt, RoleWidget( role, widget ) );

  // NOTE: reparent the widget!
  widget->setParent( this );

  anIt  = myWidgets.begin();
  anEnd = myWidgets.end();
  for ( ; anIt != anEnd; anIt++ )
    hbox->addWidget( (*anIt).second );

  hbox->invalidate();
  update();
}

void QtxDockWidgetTitle::removeWidget( const int role )
{
  QHBoxLayout* hbox = qobject_cast<QHBoxLayout*>( layout() );

  RoleWidgetList::iterator anIt  = myWidgets.begin();
  RoleWidgetList::iterator anEnd = myWidgets.end();
  for ( ; anIt != anEnd; anIt++ )
    if ( (*anIt).first == role ){
      QWidget* w = (*anIt).second;
      myWidgets.erase( anIt );
      hbox->removeWidget( w );
      w->deleteLater();
      break;
    }

  hbox->invalidate();
  update();
}

void QtxDockWidgetTitle::paintEvent( QPaintEvent* /*e*/ )
{
  QDockWidget* aDock = qobject_cast<QDockWidget*>( parentWidget() );
  QLabel* caption = qobject_cast<QLabel*>( widget( QtxDockWidget::TitleText ) );
  if ( caption && caption->text() != aDock->windowTitle() )
    caption->setText( aDock->windowTitle() );

  QStylePainter p( this, aDock );

  QStyleOptionDockWidgetV2 titleOpt;
  initStyleOption( &titleOpt );

  p.drawControl( QStyle::CE_DockWidgetTitle, titleOpt );
}

void QtxDockWidgetTitle::initStyleOption( QStyleOptionDockWidget* titleOpt ) const
{
  QDockWidget* aDock = dynamic_cast<QDockWidget*>( parentWidget() );
  titleOpt->initFrom( aDock );
  titleOpt->rect             = rect();
  titleOpt->title            = "";// Title is displayed in QLabel
  titleOpt->closable         = hasFeature(aDock, QDockWidget::DockWidgetClosable);
  titleOpt->movable          = hasFeature(aDock, QDockWidget::DockWidgetMovable);
  titleOpt->floatable        = hasFeature(aDock, QDockWidget::DockWidgetFloatable);
  QStyleOptionDockWidgetV2 *v2 = qstyleoption_cast<QStyleOptionDockWidgetV2*>(titleOpt);
  if (v2 != 0) // TODO: Custom vertical title bar not supported yet
    v2->verticalTitleBar     = false;
}

void QtxDockWidgetTitle::setupButton( QToolButton* btn, 
                                      const int role, 
                                      QStyleOptionDockWidget* opt, 
                                      QDockWidget* w )
{
  QIcon btnIcon;

  switch ( role ){
    case QtxDockWidget::Undock:
      btnIcon = style()->standardIcon( QStyle::SP_TitleBarNormalButton, opt, w );
      break;
    case QtxDockWidget::Close:
      btnIcon = style()->standardIcon( QStyle::SP_TitleBarCloseButton, opt, w );
      break;
    default:
      break;
  }
  int size = 1.5 * style()->pixelMetric( QStyle::PM_DockWidgetTitleBarButtonMargin, 0, w );
  if (!btnIcon.isNull()) {
    int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, w);
    const QPixmap pm = btnIcon.pixmap(iconSize);
    size += qMax(pm.width(), pm.height());
  }
  btn->setIcon( btnIcon );
  btn->setFixedSize( QSize( size, size ) );
  btn->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  btn->setFocusPolicy( Qt::NoFocus );
}

void QtxDockWidgetTitle::updateTitleBar()
{
  QtxDockWidget* w = qobject_cast<QtxDockWidget*>( parentWidget() );

  // Suicide - we do not support custom vertical title bar yet
  if ( hasFeature( w, QDockWidget::DockWidgetVerticalTitleBar ) ){
    w->enableCustomTitleBar( false );
    return;
  }

  bool canClose = hasFeature( w, QDockWidget::DockWidgetClosable );
  bool canFloat = hasFeature( w, QDockWidget::DockWidgetFloatable ) && !w->isFloating();

  QLabel* caption = qobject_cast<QLabel*>( widget( QtxDockWidget::TitleText ) );
  // Style normally does similar things for regular title text, so we have to...
  if ( caption ){
    QFont f = caption->font();
    f.setBold( w->isFloating() );
    caption->setFont( f );
    caption->setForegroundRole( w->isFloating() ? QPalette::HighlightedText : QPalette::Text );
  }
  widget( QtxDockWidget::Undock )->setVisible( canFloat );
  widget( QtxDockWidget::Close  )->setVisible( canClose );
}

//=================================================================================
/*!
  \class QtxDockWidget::Watcher
  \internal
  \brief Internal class which goal is to watch parent dockable widget state changing.
*/

class QtxDockWidget::Watcher : public QObject
{
public:
  Watcher( QtxDockWidget* );

  void           shown( QtxDockWidget* );
  void           hidden( QtxDockWidget* );

  virtual bool   eventFilter( QObject*, QEvent* );

  bool           isEmpty() const;
  bool           isVisible() const;

protected:
  enum { Update = QEvent::User, Remove };

  virtual void   customEvent( QEvent* );

private:
  void           installFilters();

  void           showContainer();
  void           hideContainer();

  void           updateIcon();
  void           updateCaption();
  void           updateVisibility();

  void           setEmpty( const bool );
  void           setVisible( const bool );

private:
  QtxDockWidget* myCont;
  bool           myState;
  bool           myEmpty;
  bool           myBlock;
  bool           myShown;
};

/*!
  \brief Constructor.
  \param cont dockable widget to be watched
*/
QtxDockWidget::Watcher::Watcher( QtxDockWidget* cont )
: QObject( cont ), myCont( cont ),
  myState( true ),
  myEmpty( false ),
  myBlock( false )
{
  myCont->installEventFilter( this );

  installFilters();

  myShown = myCont->isVisibleTo( myCont->parentWidget() );
}

/*!
  \brief Custom event filter.
  \param o event receiver object
  \param e event sent to object
  \return \c true if further event processing should be stopped
*/
bool QtxDockWidget::Watcher::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myCont && ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ||
                        e->type() == QEvent::Hide || e->type() == QEvent::HideToParent ) )
  {
    installFilters();
  }

  if ( o == myCont && e->type() == QEvent::ChildAdded )
  {
    QChildEvent* ce = (QChildEvent*)e;
    if ( ce->child()->isWidgetType() )
      ce->child()->installEventFilter( this );

    QApplication::postEvent( this, new QEvent( (QEvent::Type)Update ) );
  }

  if ( o != myCont && e->type() == QEvent::WindowIconChange )
    updateIcon();

  if ( o != myCont && e->type() == QEvent::WindowTitleChange )
    updateCaption();

  if ( o != myCont && ( e->type() == QEvent::HideToParent || e->type() == QEvent::ShowToParent ) )
    updateVisibility();

  if ( o == myCont && e->type() == QEvent::ChildRemoved )
  {
    QApplication::postEvent( this, new QEvent( (QEvent::Type)Remove ) );
  }

  return false;
}

/*!
  \brief Set internal status to "shown"
  \param dw dockable widget
*/
void QtxDockWidget::Watcher::shown( QtxDockWidget* dw )
{
  if ( dw != myCont )
    return;

  setVisible( true );
}

/*!
  \brief Set internal status to "hidden"
  \param dw dockable widget
*/
void QtxDockWidget::Watcher::hidden( QtxDockWidget* dw )
{
  if ( dw != myCont )
    return;

  setVisible( false );
}

bool QtxDockWidget::Watcher::isEmpty() const
{
  return myEmpty;
}

bool QtxDockWidget::Watcher::isVisible() const
{
  return myShown;
}

void QtxDockWidget::Watcher::setEmpty( const bool on )
{
  myEmpty = on;
}

void QtxDockWidget::Watcher::setVisible( const bool on )
{
  myShown = on;
}

/*!
  \brief Show the dock window being watched
*/
void QtxDockWidget::Watcher::showContainer()
{
  if ( !myCont )
    return;

  bool vis = isVisible();

  QtxDockWidget* cont = myCont;
  myCont = 0;
  cont->show();
  myCont = cont;

  setVisible( vis );
}

/*!
  \brief Hide the dock window being watched
*/
void QtxDockWidget::Watcher::hideContainer()
{
  if ( !myCont )
    return;

  bool vis = isVisible();

  QtxDockWidget* cont = myCont;
  myCont = 0;
  cont->hide();
  myCont = cont;

  setVisible( vis );
}

/*!
  \brief Proces custom events.
  \param e custom event (not used)
*/
void QtxDockWidget::Watcher::customEvent( QEvent* e )
{
  if ( e->type() == Update )
  {
    updateIcon();
    updateCaption();
    updateVisibility();
  }
  else if ( myCont && e->type() == Remove && !myCont->widget() )
  {
    myCont->deleteLater();
    myCont = 0;
  }
}

/*!
  \brief Install this object as event filter to all children widgets
         of the dockable widget being watched.
*/
void QtxDockWidget::Watcher::installFilters()
{
  if ( !myCont )
    return;

  QLayout* l = myCont->layout();
  if ( !l )
    return;

  for ( int i = 0; i < (int)l->count(); i++ )
  {
    if ( l->itemAt( i ) && l->itemAt( i )->widget() )
      l->itemAt( i )->widget()->installEventFilter( this );
  }
}

/*!
  \brief Update visibility state of all children widgets of the dockable widget
         being watched.
*/
void QtxDockWidget::Watcher::updateVisibility()
{
  if ( !myCont )
    return;

  bool vis = false;
  if ( myCont->widget() )
    vis = myCont->widget()->isVisibleTo( myCont );
  else
  {
    QLayout* l = myCont->layout();
    if ( l )
    {
      for ( int i = 0; i < (int)l->count() && !vis; i++ )
	vis = l->itemAt( i ) && l->itemAt( i )->widget() && l->itemAt( i )->widget()->isVisibleTo( myCont );
    }
  }

  bool empty = isEmpty();
  if ( empty == vis )
  {
    empty = !vis;
    setEmpty( empty );
    if ( !empty )
      myCont->toggleViewAction()->setVisible( myState );
    else
    {
      myState = myCont->toggleViewAction()->isVisible();
      myCont->toggleViewAction()->setVisible( false );
    }
  }

  vis = !empty && isVisible();
  if ( vis != myCont->isVisibleTo( myCont->parentWidget() ) )
    vis ? showContainer() : hideContainer();
}

/*!
  \brief Update the icon of dockable window being watched
*/
void QtxDockWidget::Watcher::updateIcon()
{
  if ( !myCont || !myCont->widget() || myBlock )
    return;

  myBlock = true;
  myCont->setWindowIcon( myCont->widget()->windowIcon() );
  myBlock = false;
}

/*!
  \brief Update the title of dockable window being watched
*/
void QtxDockWidget::Watcher::updateCaption()
{
  if ( myCont && myCont->widget() && !myCont->widget()->windowTitle().isNull() )
    myCont->setWindowTitle( myCont->widget()->windowTitle() );
}

/*!
  \class QtxDockWidget
  \brief Enhanced dockable widget class.
*/

/*!
  \brief Constructor.
  \param title dockable widget title
  \param parent parent widget
  \param f widget flags
*/
QtxDockWidget::QtxDockWidget( const QString& title, QWidget* parent, Qt::WindowFlags f )
: QDockWidget( title, parent, f ),
  myWatcher( 0 ),
  myOrientation( (Qt::Orientation)-1 )
{
  updateState();
}

/*!
  \brief Constructor.
  \param watch if \c true the event filter is installed to watch wigdet state changes
         to update it properly
  \param parent parent widget
  \param f widget flags
*/
QtxDockWidget::QtxDockWidget( const bool watch, QWidget* parent, Qt::WindowFlags f )
: QDockWidget( parent, f ),
  myWatcher( 0 ),
  myOrientation( (Qt::Orientation)-1 )
{
  if ( watch )
    myWatcher = new Watcher( this );

  updateState();
}

/*!
  \brief Constructor.
  \param parent parent widget
  \param f widget flags
*/
QtxDockWidget::QtxDockWidget( QWidget* parent, Qt::WindowFlags f )
: QDockWidget( parent, f ),
myWatcher( 0 )
{
}

/*!
  \brief Destructor.
*/
QtxDockWidget::~QtxDockWidget()
{
}

/*!
  \brief Get the recommended size for the widget.
  \return recommended dockable widget size
*/
QSize QtxDockWidget::sizeHint() const
{
  QSize sz = QDockWidget::sizeHint();

  return sz;
}

/*!
  \brief Get the recommended minimum size for the widget.
  \return recommended dockable widget minimum size
*/
QSize QtxDockWidget::minimumSizeHint() const
{
  QSize sz = QDockWidget::minimumSizeHint();

  return sz;
}

/*!
  \brief Show/hide the dockable window.
  \param on new visibility state
*/
void QtxDockWidget::setVisible( bool on )
{
  updateGeometry();
  if ( widget() )
    widget()->updateGeometry();

  QDockWidget::setVisible( on && ( myWatcher ? !myWatcher->isEmpty() : true )  );

  if ( myWatcher )
  {
    if ( on )
      myWatcher->shown( this );
    else
      myWatcher->hidden( this );
  }
}

/*!
  \brief Toggles "floating" state of this widget.
*/
void QtxDockWidget::toggleTopLevel()
{
  setFloating( !isFloating() );
}

/*!
  \brief Process resize event
  \param e event
*/
void QtxDockWidget::resizeEvent( QResizeEvent* e )
{
  QDockWidget::resizeEvent( e );
  updateState();
}

/*!
  \brief Get dockable window orientation.
  \return orientation type
*/
Qt::Orientation QtxDockWidget::orientation() const
{
  QMainWindow* mw = 0;
  QWidget* wid = parentWidget();
  while ( wid && !mw )
  {
    mw = ::qobject_cast<QMainWindow*>( wid );
    wid = wid->parentWidget();
  }

  Qt::Orientation res = (Qt::Orientation)-1;

  if ( !mw )
    return res;

  Qt::DockWidgetArea area = mw->dockWidgetArea( (QtxDockWidget*)this );
  switch ( area )
  {
  case Qt::LeftDockWidgetArea:
  case Qt::RightDockWidgetArea:
    res = Qt::Vertical;
    break;
  case Qt::TopDockWidgetArea:
  case Qt::BottomDockWidgetArea:
    res = Qt::Horizontal;
    break;
  default:
    break;
  }

  return res;
}

/*!
  \brief Enables/disables customizable title bar widget. Vertical title bar is not yet supported.
  \param enable If true, makes the custom title bar widget appear, otherwise deletes it.
*/
void QtxDockWidget::enableCustomTitleBar( const bool enable )
{
  // TODO: Custom vertical title bar not yet supported...
  if ( hasFeature( this, QDockWidget::DockWidgetVerticalTitleBar ) )
    return;
  if ( enable == customTitleBarEnabled() )
    return;

  QWidget* w = titleBarWidget();
  QtxDockWidgetTitle* titleBar = qobject_cast<QtxDockWidgetTitle*>( w );
  if ( enable ){
    // Have to delete any "foreign" title bar widget previously set
    if ( w && !titleBar ){
      setTitleBarWidget( 0 );
      w->deleteLater();
      w = 0;
    }

    titleBar = new QtxDockWidgetTitle( this );
    setTitleBarWidget( titleBar );
  }
  else {
    if ( titleBar ){
      setTitleBarWidget( 0 );
      titleBar->deleteLater();
      titleBar = 0;
    }
  }
}

/*!
  \brief Query custom title bar availability.
  \return True if the custom title bar is available.
*/
bool QtxDockWidget::customTitleBarEnabled() const
{
  // TODO: Custom vertical title bar not yet supported...
  return qobject_cast<QtxDockWidgetTitle*>( titleBarWidget() );
}

/*!
  \brief Query custom title bar widget for a given role.
  \param role Widget role in the custom title bar.
  \return Widget for a given role, or null if it does not exist.
*/
QWidget* QtxDockWidget::customTitleBarWidget( const int role ) const
{
  if ( !customTitleBarEnabled() )
    return 0;
  return qobject_cast<QtxDockWidgetTitle*>( titleBarWidget() )->widget( role );
}

/*!
  \brief Insert a custom title bar widget for a given role. The title bar takes ownership of the inserted widget.
  \param role Role for widget to be inserted, should be unique
  \param w Widget to be inserted
  \param beforeRole Optional role of a widget to insert the new one before.
*/
void QtxDockWidget::insertCustomTitleBarWidget( const int role, QWidget* w, const int beforeRole )
{
  if ( !customTitleBarEnabled() )
    return;
  qobject_cast<QtxDockWidgetTitle*>( titleBarWidget() )->insertWidget( role, w, beforeRole );
}

/*!
  \brief Remove custom title bar widget for a given role. The widget is deleted.
  \param role Widget role in the custom title bar.
*/
void QtxDockWidget::removeCustomTitleBarWidget( const int role )
{
  if ( !customTitleBarEnabled() )
    return;
  qobject_cast<QtxDockWidgetTitle*>( titleBarWidget() )->removeWidget( role );
}

/*!
  \brief Update dockable window state.
*/
void QtxDockWidget::updateState()
{
  Qt::Orientation o = orientation();
  if ( myOrientation == o )
    return;

  myOrientation = o;

  emit orientationChanged( myOrientation );
}

/*!
  \fn QtxDockWidget::orientationChanged(Qt::Orientation o)
  \brief Emitted when the dockable window orientation is changed.
  \param o new window orientation
*/

#include <QtxDockWidget.moc>
