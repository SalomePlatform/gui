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
#include "SUIT_Desktop.h"

#include "SUIT_ViewWindow.h"

#include <QtxLogoMgr.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>

#include <QApplication>
#include <QCloseEvent>

/*!\class SUIT_Desktop
 * Provide desktop management:\n
 * \li menu manager
 * \li tool manager
 * \li windows
 */

class SUIT_Desktop::ReparentEvent : public QEvent
{
public:
  ReparentEvent( Type t, QObject* obj ) : QEvent( t ), myObj( obj ) {};

  QObject* object() const { return myObj; }

private:
  QPointer<QObject> myObj;
};

/*!
  Constructor.
*/
SUIT_Desktop::SUIT_Desktop()
: QtxMainWindow()
{
  myMenuMgr = new QtxActionMenuMgr( this );
  myToolMgr = new QtxActionToolMgr( this );
  myLogoMgr = new QtxLogoMgr( menuBar() );
}

/*!
  Destructor.
*/
SUIT_Desktop::~SUIT_Desktop()
{
}

/*!
  Emit on event \a e.
*/
bool SUIT_Desktop::event( QEvent* e )
{
  if ( !e )
    return false;

  switch ( e->type() )
  {
  case QEvent::WindowActivate:
    emit activated();
    break;
  case QEvent::WindowDeactivate:
    emit deactivated();
    break;
  default:
    break;
  }

  return QMainWindow::event( e );
}

/*!
  Close event \a e.
*/
void SUIT_Desktop::closeEvent( QCloseEvent* e )
{
  emit closing( this, e );
  e->ignore();
}

/*!
  Child event.
*/
void SUIT_Desktop::childEvent( QChildEvent* e )
{
  if ( e->type() == QEvent::ChildAdded && e->child()->isWidgetType() )
    QApplication::postEvent( this, new ReparentEvent( QEvent::Type( Reparent ), e->child() ) );
  else
    QtxMainWindow::childEvent( e );
}

void SUIT_Desktop::customEvent( QEvent* e )
{
  if ( (int)e->type() != Reparent )
    return;

  ReparentEvent* re = (ReparentEvent*)e;
  if ( re->object() && re->object()->inherits( "SUIT_ViewWindow" ) )
  {
    QWidget* wid = (QWidget*)re->object();
    bool invis = wid->testAttribute( Qt::WA_WState_ExplicitShowHide ) &&
                 wid->testAttribute( Qt::WA_WState_Hidden );

    addWindow( wid );
    wid->setShown( !invis );
  }
}

/*!
  Gets menu manager.
*/
QtxActionMenuMgr* SUIT_Desktop::menuMgr() const
{
  return myMenuMgr;
}

/*!
  Gets tool manager.
*/
QtxActionToolMgr* SUIT_Desktop::toolMgr() const
{
  return myToolMgr;
}

/*!
  Gets logo manager.
*/
QtxLogoMgr* SUIT_Desktop::logoMgr() const
{
  return myLogoMgr;
}

/*!
  Returns the count of the existed logos.
*/
int SUIT_Desktop::logoCount() const
{
  return 0;

  if ( !myLogoMgr )
    return 0;
  else
    return myLogoMgr->count();
}

/*!
  Adds new logo to the menu bar area
*/
void SUIT_Desktop::logoInsert( const QString& logoID, QMovie* logo, const int idx )
{
  if ( myLogoMgr )
    myLogoMgr->insert( logoID, logo, idx );
}

/*!
  Adds new logo to the menu bar area
*/
void SUIT_Desktop::logoInsert( const QString& logoID, const QPixmap& logo, const int idx )
{
  if ( myLogoMgr )
    myLogoMgr->insert( logoID, logo, idx );
}

/*!
  Removes a logo
*/
void SUIT_Desktop::logoRemove( const QString& logoID )
{
  if ( myLogoMgr )
    myLogoMgr->remove( logoID );
}

/*!
  Removes all logos 
*/
void SUIT_Desktop::logoClear()
{
  if ( myLogoMgr )
    myLogoMgr->clear();
}

/*!
  Emits activated signal
*/
void SUIT_Desktop::emitActivated()
{
  emit activated();
}

/*!
  Emits message signal
*/
void SUIT_Desktop::emitMessage( const QString& theMessage )
{
  emit message( theMessage );
}
