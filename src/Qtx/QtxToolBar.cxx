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
// File:      QtxToolBar.cxx
// Author:    Sergey TELKOV

#include "QtxToolBar.h"

#include <QtGui/qaction.h>
#include <QtGui/qlayout.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qmainwindow.h>
#include <QtGui/qapplication.h>

/*!
    Class: QtxToolBar::Watcher [Internal]
    Descr: Internal object with event filter.
*/

class QtxToolBar::Watcher : public QObject
{
public:
  Watcher( QtxToolBar* );

  void         shown( QtxToolBar* );
  void         hided( QtxToolBar* );

  virtual bool eventFilter( QObject*, QEvent* );

protected:
  virtual void customEvent( QEvent* );

private:
  enum { Install = QEvent::User, Update };

private:
  void         installFilters();

  void         showContainer();
  void         hideContainer();

  void         updateVisibility();

private:
  QtxToolBar*  myCont;
  bool         myState;
  bool         myEmpty;
  bool         myVisible;
};

/*!
  Constructor
*/
QtxToolBar::Watcher::Watcher( QtxToolBar* cont )
: QObject( cont ),
myCont( cont ),
myState( true ),
myEmpty( false )
{
/*
  if ( myCont->mainWindow() )
    myState = myCont->mainWindow()->appropriate( myCont );
*/
  myCont->installEventFilter( this );
  myVisible = myCont->isVisibleTo( myCont->parentWidget() );

  installFilters();
}

/*!
  Custom event filter
*/
bool QtxToolBar::Watcher::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myCont && e->type() == QEvent::ChildAdded )
    QApplication::postEvent( this, new QEvent( (QEvent::Type)Install ) );

  bool updVis = ( o != myCont && ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ||
                                   e->type() == QEvent::Hide || e->type() == QEvent::HideToParent ) ) ||
                ( o == myCont && ( e->type() == QEvent::ChildRemoved || e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ) );

  if ( updVis )
  {
    QtxToolBar* cont = myCont;
    myCont = 0;
    QApplication::sendPostedEvents( this, Update );
    myCont = cont;
    QApplication::postEvent( this, new QEvent( (QEvent::Type)Update ) );
  }

  return false;
}

/*!
  Sets internal visibility state to true
*/
void QtxToolBar::Watcher::shown( QtxToolBar* tb )
{
  if ( tb != myCont )
    return;

  myVisible = true;
}

/*!
  Sets internal visibility state to false
*/
void QtxToolBar::Watcher::hided( QtxToolBar* tb )
{
  if ( tb != myCont )
    return;

  myVisible = false;
}

/*!
  Shows corresponding QtxToolBar
*/
void QtxToolBar::Watcher::showContainer()
{
  if ( !myCont )
    return;

  QtxToolBar* cont = myCont;
  myCont = 0;
  cont->show();
  myCont = cont;
}

/*!
  Hides corresponding QtxToolBar
*/
void QtxToolBar::Watcher::hideContainer()
{
  if ( !myCont )
    return;

  QtxToolBar* cont = myCont;
  myCont = 0;
  cont->hide();
  myCont = cont;
}

/*!
  Event handler of custom events
*/
void QtxToolBar::Watcher::customEvent( QEvent* e )
{
  switch ( e->type() )
  {
  case Install:
    installFilters();
  case Update:
    updateVisibility();
  }
}

/*!
  Installs event filters
*/
void QtxToolBar::Watcher::installFilters()
{
  if ( !myCont )
    return;

  const QObjectList& objList = myCont->children();
  for ( QObjectList::const_iterator it = objList.begin(); it != objList.end(); ++it )
  {
    if ( (*it)->isWidgetType() && qstrcmp( "qt_dockwidget_internal", (*it)->objectName().toLatin1() ) )
      (*it)->installEventFilter( this );
  }
}

/*!
  Update visibility state
*/
void QtxToolBar::Watcher::updateVisibility()
{
  if ( !myCont )
    return;

  bool vis = false;

  const QObjectList& objList = myCont->children();
  for ( QObjectList::const_iterator it = objList.begin(); it != objList.end() && !vis; ++it )
  {
    QObject* obj = *it;
    if ( !obj->isWidgetType() || !qstrcmp( "qt_dockwidget_internal", obj->objectName().toLatin1() ) )
      continue;

    if ( obj->inherits( "QToolBarHandle" ) || obj->inherits( "QToolBarExtension" ) )
      continue;

    QWidget* wid = (QWidget*)*it;
    vis = wid->isVisibleTo( wid->parentWidget() );
  }

  QMainWindow* mw = myCont->mainWindow();
  if ( mw && myEmpty == vis )
  {
    myEmpty = !vis;
    if ( !myEmpty )
      myCont->toggleViewAction()->setVisible( myState );
    else
    {
      myState = myCont->toggleViewAction()->isVisible();
      myCont->toggleViewAction()->setVisible( false );
    }
  }

  vis = !myEmpty && myVisible;
  if ( vis != myCont->isVisibleTo( myCont->parentWidget() ) )
    vis ? showContainer() : hideContainer();
}

/*!
  Constructor
*/
QtxToolBar::QtxToolBar( const bool watch, const QString& label, QWidget* parent )
: QToolBar( label, parent ),
myWatcher( 0 ),
myStretch( false )
{
  if ( watch )
    myWatcher = new Watcher( this );
}

/*!
  Constructor
*/
QtxToolBar::QtxToolBar( const QString& label, QWidget* parent )
: QToolBar( label, parent ),
myWatcher( 0 ),
myStretch( false )
{
}

/*!
  Constructor
*/
QtxToolBar::QtxToolBar( const bool watch, QWidget* parent )
: QToolBar( parent ),
myWatcher( 0 ),
myStretch( false )
{
  if ( watch )
    myWatcher = new Watcher( this );
}

/*!
  Constructor
*/
QtxToolBar::QtxToolBar( QWidget* parent )
: QToolBar( parent ),
myWatcher( 0 ),
myStretch( false )
{
}

/*!
  Destructor
*/
QtxToolBar::~QtxToolBar()
{
}

/*!
  \return the recommended size for the widget
*/
QSize QtxToolBar::sizeHint() const
{
  QSize sz = QToolBar::sizeHint();
/*
  if ( place() == InDock && isStretchable() && area() )
  {
    if ( orientation() == Horizontal )
      sz.setWidth( area()->width() );
    else
      sz.setHeight( area()->height() );
  }
*/
  return sz;
}

/*!
  \return the recommended minimum size for the widget
*/
QSize QtxToolBar::minimumSizeHint() const
{
  QSize sz = QToolBar::minimumSizeHint();
/*
  if ( place() == InDock && isStretchable() && area() )
  {
    if ( orientation() == Horizontal )
      sz.setWidth( area()->width() );
    else
      sz.setHeight( area()->height() );
  }
*/
  return sz;
}

/*!
  Shows toolbar
*/
void QtxToolBar::setVisible( bool visible )
{
  if ( myWatcher )
  {
    if ( visible )
      myWatcher->shown( this );
    else
      myWatcher->hided( this );
  }

  QToolBar::setVisible( visible );
}

/*!
  Returns the main window
*/
QMainWindow* QtxToolBar::mainWindow() const
{
  QMainWindow* mw = 0;
  QWidget* wid = parentWidget();
  while ( !mw && wid )
  {
    mw = ::qobject_cast<QMainWindow*>( wid );
    wid = wid->parentWidget();
  }
  return mw;
}
