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
// File:      QtxDockAction.cxx
// Author:    Sergey TELKOV

#include "QtxDockAction.h"

#include <QtGui/qmenu.h>
#include <QtGui/qtoolbar.h>
#include <QtGui/qdockwidget.h>
#include <QtGui/qmainwindow.h>

/*!
	Name: QtxDockAction [public]
	Desc: Constructs an Dock action with given main window and name.
*/

QtxDockAction::QtxDockAction( QMainWindow* mw )
: QtxAction( "Windows and Toolbars", "Windows and Toolbars", 0, mw ),
myMain( mw ),
myType( Both )
{
  initialize();
}

/*!
	Name: QtxDockAction [public]
	Desc: This constructor creates an action with the following properties: the
		    description text, the menu text and. It is a child of given main window
        and named specified name.
*/

QtxDockAction::QtxDockAction( const QString& text, const QString& menuText, QMainWindow* mw )
: QtxAction( text, menuText, 0, mw ),
myMain( mw ),
myType( Both )
{
  initialize();
}

/*!
	Name: QtxDockAction [public]
	Desc: This constructor creates an action with the following properties: the
		    description text, the menu text, the icon or iconset icon and keyboard
        accelerator. It is a child of given main window and named specified name.
*/

QtxDockAction::QtxDockAction( const QString& text, const QIcon& icon, const QString& menuText, QMainWindow* mw )
: QtxAction( text, icon, menuText, 0, mw ),
myMain( mw ),
myType( Both )
{
  initialize();
}

/*!
	Name: ~QtxDockAction [public]
	Desc: Removes all added popup items.
*/

QtxDockAction::~QtxDockAction()
{
}

/*!
	Name: mainWindow [public]
	Desc: Returns the main window which contains managed dock windows.
*/

QMainWindow* QtxDockAction::mainWindow() const
{
  return myMain;
}

int QtxDockAction::dockType() const
{
  return myType;
}

void QtxDockAction::setDockType( const int type )
{
  myType = type;
}

/*!
	Name: onAboutToShow [private slots]
	Desc: Prepare sub popup with dock windows list when parent popup is shown.
*/

void QtxDockAction::onAboutToShow()
{
  updateMenu();
  setVisible( menu() && !menu()->isEmpty() );
}

/*!
	Name: toolBars [private]
	Desc: Returns all toolbars of the main window.
*/

void QtxDockAction::toolBars( QList<QToolBar*>& lst ) const
{
  lst.clear();

  QMainWindow* mw = mainWindow();
  if ( !mw )
    return;

  QList<QToolBar*> toolbars = qFindChildren<QToolBar*>( mw );
  for ( QList<QToolBar*>::iterator it = toolbars.begin(); it != toolbars.end(); ++it )
  {
    QToolBar* tb = *it;
    if ( tb->parentWidget() == mw )
      lst.append( tb );
  }
}

/*!
	Name: dockWidgets [private]
	Desc: Returns all dock widgets of the main window.
*/

void QtxDockAction::dockWidgets( QList<QDockWidget*>& lst ) const
{
  lst.clear();

  QMainWindow* mw = mainWindow();
  if ( !mw )
    return;

  QList<QDockWidget*> dockwidgets = qFindChildren<QDockWidget*>( mw );
  for ( QList<QDockWidget*>::iterator it = dockwidgets.begin(); it != dockwidgets.end(); ++it )
  {
    QDockWidget* dw = *it;
    if ( dw->parentWidget() == mw )
      lst.append( dw );
  }
}

/*!
	Name: updateInfo [private]
	Desc: Updates icon and caption info of dock window in the corresponded action.
*/
/*
void QtxDockAction::updateInfo( QDockWindow* dw )
{
  QAction* a = action( dw );
  if ( !a )
    return;

  a->setText( dw->caption() );
  a->setMenuText( dw->caption() );

  if ( isToolBar( dw ) )
    a->setStatusTip( tr( "Toggles toolbar \"%1\" on/off" ).arg( dw->caption() ) );
  else
    a->setStatusTip( tr( "Toggles window \"%1\" on/off" ).arg( dw->caption() ) );

  const QPixmap* icon = dw->icon();
  if ( icon )
    a->setIconSet( *icon );
}
*/

void QtxDockAction::addedTo( QWidget* w )
{
  if ( w->inherits( "QMenu" ) )
    connect( w, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

void QtxDockAction::removedFrom( QWidget* w )
{
  if ( w->inherits( "QMenu" ) )
    disconnect( w, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

/*!
	Name: initialize [private]
	Desc: Initialisation of the object.
*/

void QtxDockAction::initialize()
{
  setMenu( new QMenu( 0 ) );
}

/*!
  Updates menu of action
*/
void QtxDockAction::updateMenu()
{
  QMenu* pm = menu();
  if ( !pm )
    return;

  pm->clear();

  if ( dockType() == DockWidget || dockType() == Both )
  {
    QList<QDockWidget*> dwList;
    dockWidgets( dwList );
    for ( QList<QDockWidget*>::iterator it = dwList.begin(); it != dwList.end(); ++it )
      pm->addAction( (*it)->toggleViewAction() );
  }

  pm->addSeparator();

  if ( dockType() == ToolBar || dockType() == Both )
  {
    QList<QToolBar*> tbList;
    toolBars( tbList );
    for ( QList<QToolBar*>::iterator it = tbList.begin(); it != tbList.end(); ++it )
      pm->addAction( (*it)->toggleViewAction() );
  }

  Qtx::simplifySeparators( pm );
}
