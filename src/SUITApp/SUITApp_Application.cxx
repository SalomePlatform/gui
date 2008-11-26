//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "SUITApp_Application.h"

#include "SUIT_Session.h"
#include "SUIT_MessageBox.h"
#include "SUIT_ExceptionHandler.h"

#include <qdir.h>
#include <qfileinfo.h>

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#else
#include <unistd.h>
#endif

/*!
  Constructor
*/
SUITApp_Application::SUITApp_Application( int& argc, char** argv, SUIT_ExceptionHandler* hand )
: QApplication( argc, argv ),
myExceptHandler( hand )
{
  QString path = QFileInfo( argv[0] ).dirPath() + QDir::separator() + "../../resources";
  path = QDir::convertSeparators( QDir( path ).canonicalPath() );

  QTranslator* strTbl = new QTranslator( 0 );
  if ( strTbl->load( "SUITApp_msg_en.po", path  ) )
    installTranslator( strTbl );
  else
    delete strTbl;
}

/*!
  Constructor
*/
SUITApp_Application::SUITApp_Application( int& argc, char** argv, Type type, SUIT_ExceptionHandler* hand )
: QApplication( argc, argv, type ),
myExceptHandler( hand )
{
    QTranslator* strTbl = new QTranslator( 0 );
    strTbl->load( "resources\\SUITApp_msg_en.po" );
    installTranslator( strTbl );
}

/*!
  Sends event to receiver
  \return the value that is returned from the receiver's event handler
  \param e - event
  \param receiver - receiver
*/
bool SUITApp_Application::notify( QObject* receiver, QEvent* e )
{
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) < 0x060101
  // Disable GUI user actions while python command is executed
  if (SUIT_Session::IsPythonExecuted()) {
    // Disable mouse and keyboard events
    QEvent::Type aType = e->type();
    if (aType == QEvent::MouseButtonPress || aType == QEvent::MouseButtonRelease ||
        aType == QEvent::MouseButtonDblClick || aType == QEvent::MouseMove ||
        aType == QEvent::Wheel || aType == QEvent::ContextMenu ||
        aType == QEvent::KeyPress || aType == QEvent::KeyRelease ||
        aType == QEvent::Accel || aType == QEvent::AccelOverride)
      return false;
  }
#endif

  return myExceptHandler ? myExceptHandler->handle( receiver, e ) :
                           QApplication::notify( receiver, e );
}

/*!
  Changes exception handler
  \param hand - new handler
*/
void SUITApp_Application::setHandler( SUIT_ExceptionHandler* hand )
{
	myExceptHandler = hand;
}

/*!
  \return exception handler
*/
SUIT_ExceptionHandler* SUITApp_Application::handler() const
{
  return myExceptHandler;
}
