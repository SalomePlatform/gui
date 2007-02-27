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
//  File   : PythonConsole_PyConcole.cxx
//  Author : Vadim SANDLER
//  Module : SALOME

/*!
  \class PythonConsole
  \brief Python console widget.
*/  

#include <Python.h>

#include "PyConsole_Console.h"
#include "PyConsole_Editor.h"

#include <PyInterp_base.h>

#include <QtGui/qmenu.h>
#include <QtGui/qevent.h>
#include <QtGui/qaction.h>
#include <QtGui/qlayout.h>
#include <QtGui/qclipboard.h>
#include <QtGui/qapplication.h>

using namespace std;

/*!
  \brief Constructor.

  Creates new python console widget.
  \param parent parent widget
  \param interp python interpreter
*/
PythonConsole::PythonConsole( QWidget* parent, PyInterp_base* interp )
: QFrame( parent ),
myEditor( 0 )
{
  // create python interpreter
  myInterp = interp;
  if ( !myInterp )
    myInterp = new PyConsole_Interp();
  
  // initialize Python interpretator
  myInterp->initialize();
  
  // create editor console
  QVBoxLayout* lay = new QVBoxLayout( this );
  lay->setMargin( 5 );
  myEditor = new PyConsole_Editor( myInterp, this );
  myEditor->viewport()->installEventFilter( this );
  lay->addWidget( myEditor );

  createActions();
}

/*!
  \brief Destructor.

  Does nothing for the moment.
*/
PythonConsole::~PythonConsole()
{
}

/*!
  \brief Execute python command in the interpreter.
  \param command - string with command and arguments
*/
void PythonConsole::exec( const QString& command )
{
  if ( myEditor )
    myEditor->exec( command );
}

/*!
  \brief Execute python command in the interpreter 
         and wait until it is finished.
  
  Block execution of main application until the python command is executed.
  \param command - string with command and arguments
*/
void PythonConsole::execAndWait( const QString& command )
{
  if ( myEditor )
    myEditor->execAndWait( command );
}

bool PythonConsole::isSync() const
{
  return myEditor->isSync();
}

void PythonConsole::setIsSync( const bool s )
{
  myEditor->setIsSync( s );
}

/*!
  \brief Change the python console's font.
  \param f - new font
*/
void PythonConsole::setFont( const QFont& f )
{
  if( myEditor )
    myEditor->setFont( f );
}

/*!
  \brief Get python console font.
  \return current python console's font
*/
QFont PythonConsole::font() const
{
  QFont res;
  if( myEditor )
    res = myEditor->font();
  return res;
}

/*!
  Custom event handler
*/
bool PythonConsole::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myEditor->viewport() && e->type() == QEvent::ContextMenu )
  {
    contextMenuRequest( (QContextMenuEvent*)e );
    return true;
  }
  return QFrame::eventFilter( o, e );
}

/*!
  \brief Process context popup menu event.

  Show popup menu which includes standard copy/paste operations.
  \param event context menu event
*/
void PythonConsole::contextMenuPopup( QMenu* menu )
{
  if ( myEditor->isReadOnly() )
    return;

  updateActions();

  menu->addAction( myActions[CopyId] );
  menu->addAction( myActions[PasteId] );
  menu->addAction( myActions[ClearId] );

  menu->addSeparator();

  menu->addAction( myActions[SelectAllId] );
}

void PythonConsole::createActions()
{
  QAction* copyAction = new QAction( tr( "EDIT_COPY_CMD" ), this );
  connect( copyAction, SIGNAL( triggered( bool ) ), myEditor, SLOT( copy() ) );
  myActions.insert( CopyId, copyAction );

  QAction* pasteAction = new QAction( tr( "EDIT_PASTE_CMD" ), this );
  connect( pasteAction, SIGNAL( triggered( bool ) ), myEditor, SLOT( paste() ) );
  myActions.insert( PasteId, pasteAction );

  QAction* clearAction = new QAction( tr( "EDIT_CLEAR_CMD" ), this );
  connect( clearAction, SIGNAL( triggered( bool ) ), myEditor, SLOT( clear() ) );
  myActions.insert( ClearId, clearAction );

  QAction* selAllAction = new QAction( tr( "EDIT_SELECTALL_CMD" ), this );
  connect( selAllAction, SIGNAL( triggered( bool ) ), myEditor, SLOT( selectAll() ) );
  myActions.insert( SelectAllId, selAllAction );
}

void PythonConsole::updateActions()
{
  myActions[CopyId]->setEnabled( myEditor->textCursor().hasSelection() );
  myActions[PasteId]->setEnabled( !myEditor->isReadOnly() && !QApplication::clipboard()->text().isEmpty() );
  myActions[SelectAllId]->setEnabled( !myEditor->document()->isEmpty() );
}
