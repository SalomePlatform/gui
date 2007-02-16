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

#include <QtGui/qlayout.h>
#include <QtGui/qaction.h>
#include <QtGui/qapplication.h>

/*!
  \class QtxDockWidget::Watcher [Internal]
  Internal object with event filter.
*/
class QtxDockWidget::Watcher : public QObject
{
public:
  Watcher( QtxDockWidget* );

  void           shown( QtxDockWidget* );
  void           hided( QtxDockWidget* );

  virtual bool   eventFilter( QObject*, QEvent* );

protected:
  virtual void   customEvent( QEvent* );

private:
  void           installFilters();

  void           showContainer();
  void           hideContainer();

  void           updateIcon();
  void           updateCaption();
  void           updateVisibility();

private:
  QtxDockWidget* myCont;
  bool           myState;
  bool           myEmpty;
  bool           myVisible;
};

/*!
  Constructor
*/
QtxDockWidget::Watcher::Watcher( QtxDockWidget* cont )
: QObject( cont ), myCont( cont ),
myState( true ),
myEmpty( false )
{
  myCont->installEventFilter( this );
  myVisible = myCont->isVisibleTo( myCont->parentWidget() );

  installFilters();
}

/*!
  Custom event filter
*/
bool QtxDockWidget::Watcher::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myCont && ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ||
                        e->type() == QEvent::Hide || e->type() == QEvent::HideToParent ) )
  {
    installFilters();
    QApplication::postEvent( this, new QEvent( QEvent::User ) );
  }

  if ( o == myCont && e->type() == QEvent::ChildAdded )
  {
    QChildEvent* ce = (QChildEvent*)e;
    if ( ce->child()->isWidgetType() )
      ce->child()->installEventFilter( this );

    QApplication::postEvent( this, new QEvent( QEvent::User ) );
  }

  if ( o != myCont && e->type() == QEvent::WindowIconChange )
    updateIcon();

  if ( o != myCont && e->type() == QEvent::WindowTitleChange )
    updateCaption();

  if ( ( o != myCont && ( e->type() == QEvent::Hide || e->type() == QEvent::HideToParent ) ) ||
       ( o == myCont && ( e->type() == QEvent::ChildRemoved ) ) ||
       ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ) )
    updateVisibility();

  return false;
}

/*!
  Sets internal status to shown
*/
void QtxDockWidget::Watcher::shown( QtxDockWidget* dw )
{
  if ( dw != myCont )
    return;

  myVisible = true;
}

/*!
  Sets internal status to hidden
*/
void QtxDockWidget::Watcher::hided( QtxDockWidget* dw )
{
  if ( dw != myCont )
    return;

  myVisible = false;
}

/*!
  Shows corresponding dock window
*/
void QtxDockWidget::Watcher::showContainer()
{
  if ( !myCont )
    return;

  QtxDockWidget* cont = myCont;
  myCont = 0;
  cont->show();
  myCont = cont;
}

/*!
  Hides corresponding dock window
*/
void QtxDockWidget::Watcher::hideContainer()
{
  if ( !myCont )
    return;

  QtxDockWidget* cont = myCont;
  myCont = 0;
  cont->hide();
  myCont = cont;
}

/*!
  Event filter of custom events
*/
void QtxDockWidget::Watcher::customEvent( QEvent* e )
{
  updateIcon();
  updateCaption();
  updateVisibility();
}

/*!
  Installs this object as event filter to all widgets inside corresponding main window
*/
void QtxDockWidget::Watcher::installFilters()
{
  if ( !myCont )
    return;

  QLayout* l = myCont->layout();
  if ( !l )
    return;

  for ( uint i = 0; i < l->count(); i++ )
  {
    if ( l->itemAt( i ) && l->itemAt( i )->widget() )
      l->itemAt( i )->widget()->installEventFilter( this );
  }
}

/*!
  Updates visibility of all widgets inside corresponding main window
*/
void QtxDockWidget::Watcher::updateVisibility()
{
  if ( !myCont )
    return;

  QLayout* l = myCont->layout();
  if ( !l )
    return;

  bool vis = false;
  for ( uint i = 0; i < l->count() && !vis; i++ )
    vis = l->itemAt( i ) && l->itemAt( i )->widget() && l->itemAt( i )->widget()->isVisibleTo( myCont );

  if ( myEmpty == vis )
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
  Updates icon of corresponding main window
*/
void QtxDockWidget::Watcher::updateIcon()
{
  if ( !myCont || !myCont->widget() )
    return;
  
  myCont->setWindowIcon( myCont->widget()->windowIcon() );
}

/*!
  Updates caption of corresponding main window
*/
void QtxDockWidget::Watcher::updateCaption()
{
  if ( myCont && myCont->widget() && !myCont->widget()->windowTitle().isNull() )
    myCont->setWindowTitle( myCont->widget()->windowTitle() );
}

/*!
  Constructor
*/
QtxDockWidget::QtxDockWidget( const QString& title, QWidget* parent, Qt::WindowFlags f )
: QDockWidget( title, parent, f ),
myWatcher( 0 )
{
}

/*!
  Constructor
*/
QtxDockWidget::QtxDockWidget( const bool watch, QWidget* parent, Qt::WindowFlags f )
: QDockWidget( parent, f ),
myWatcher( 0 )
{
  if ( watch )
    myWatcher = new Watcher( this );
}

/*!
  Constructor
*/
QtxDockWidget::QtxDockWidget( QWidget* parent, Qt::WindowFlags f )
: QDockWidget( parent, f ),
myWatcher( 0 )
{
}

/*!
  Destructor
*/
QtxDockWidget::~QtxDockWidget()
{
}

/*!
  \return the recommended size for the widget
*/
QSize QtxDockWidget::sizeHint() const
{
  QSize sz = QDockWidget::sizeHint();
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
QSize QtxDockWidget::minimumSizeHint() const
{
  QSize sz = QDockWidget::minimumSizeHint();
/*
  if ( orientation() == Horizontal )
	  sz = QSize( 0, QDockWidget::minimumSizeHint().height() );
  else
    sz = QSize( QDockWidget::minimumSizeHint().width(), 0 );

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
  Shows/hides the window
*/
void QtxDockWidget::setVisible( bool on )
{
  if ( myWatcher )
  {
    if ( on )
      myWatcher->shown( this );
    else
      myWatcher->hided( this );
  }

  QDockWidget::setVisible( on );
}
