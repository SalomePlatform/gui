//  KERNEL SALOME_Event : Define event posting mechanism
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

#include "LogWindow.h"

#include <QtCore/qfile.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qtextstream.h>

#include <QtGui/qmenu.h>
#include <QtGui/qlayout.h>
#include <QtGui/qaction.h>
#include <QtGui/qtextbrowser.h>
#include <QtGui/qapplication.h>

#include <SUIT_Tools.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>

#define DEFAULT_SEPARATOR "***"

/*!
  Converts rich text to plain text
*/
static QString plainText( const QString& richText )
{
  QString aText = richText;
  int startTag = aText.indexOf( '<' );
  while ( true )
  {
    if ( startTag < 0 )
      break;

    int finishTag = aText.indexOf( '>', startTag );
    if ( finishTag < 0 )
      break;

    aText = aText.remove( startTag, finishTag - startTag + 1 );
    startTag = aText.indexOf( '<' );
  }
  return aText;
}

/*!
  Default constructor
*/
LogWindow::LogWindow( QWidget* parent )
: QFrame( parent ),
SUIT_PopupClient(),
myOpFlags( All )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QString fntSet = resMgr ? resMgr->stringValue( "Log Window", "font", QString::null ) : QString::null;

  setFont( SUIT_Tools::stringToFont( fntSet ) );

  myView = new QTextEdit( this );
  myView->setReadOnly( true );
  myView->viewport()->installEventFilter( this );

  QVBoxLayout* main = new QVBoxLayout( this );
  main->setMargin( 5 );
  main->addWidget( myView );

  myBannerSize = 0;
  myBanner = "<b>Message Log</b>\n********************";
  mySeparator = DEFAULT_SEPARATOR;

  clear();

  createActions();
}

/*!
  Destructor
*/
LogWindow::~LogWindow()
{
}

/*!
  Custom event handler
*/
bool LogWindow::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myView->viewport() && e->type() == QEvent::ContextMenu )
  {
    contextMenuRequest( (QContextMenuEvent*)e );
    return true;
  }
  return QFrame::eventFilter( o, e );
}

/*!
  Sets banner (title of message log)
  \param banner - new title
*/
void LogWindow::setBanner( const QString& banner )
{
  myBanner = banner;

  clear( false );
}

/*!
  Set separator (line printing between messages)
  \param separator - new separator
*/
void LogWindow::setSeparator( const QString& separator )
{
  mySeparator = separator;

  clear( false );
}

/*!
  Puts message to log window
  \param message - text of message
  \addSeparator - if it is true, then separator is added to tail of message log
*/
void LogWindow::putMessage( const QString& message, bool addSeparator )
{
  myView->append( message );
  myHistory.append( plainText( message ) );

  if ( addSeparator && !mySeparator.isNull() )
  {
    myView->append( mySeparator );   // add separator
    myHistory.append( plainText( mySeparator ) );
  }
  myView->moveCursor( QTextCursor::End );
}

/*!
  Clears message log
  \param clearHistory - if it is true, then also history is cleared
*/
void LogWindow::clear( bool clearHistory )
{
  myView->clear();
  if ( clearHistory )
    myHistory.clear();

  if ( !myBanner.isEmpty() )
  {
    myView->append( myBanner );
    myBannerSize = myView->document()->blockCount();
  }
  else
    myBannerSize = 0;
}

/*!
  Saves log to file
  \param fileName - name of file
*/
bool LogWindow::saveLog( const QString& fileName )
{
  QFile file( fileName );
  if ( !file.open( QFile::WriteOnly ) )
    return false;

  QTextStream stream( &file );

  stream << "*****************************************"   << endl;
  stream << "Message Log"                                 << endl;
  stream << QDate::currentDate().toString( "dd.MM:yyyy" ) << "  ";
  stream << QTime::currentTime().toString( "hh:mm:ss" )   << endl;
  stream << "*****************************************"   << endl;

  for ( uint i = 0; i < myHistory.count(); i++ )
    stream << myHistory[ i ] << endl;

  file.close();
  return true;
}

/*!
  Creates actions
*/
void LogWindow::createActions()
{
  QAction* a = new QAction( tr( "&Copy" ), this );
  a->setStatusTip( tr( "&Copy" ) );
  connect( a, SIGNAL( triggered( bool ) ), SLOT( onCopy() ) );
  myActions.insert( CopyId, a );

  a = new QAction( tr( "Clea&r" ), this );
  a->setStatusTip( tr( "Clea&r" ) );
  connect( a, SIGNAL( triggered( bool ) ), SLOT( onClear() ) );
  myActions.insert( ClearId, a );

  a = new QAction( tr( "Select &All" ), this );
  a->setStatusTip( tr( "Select &All" ) );
  connect( a, SIGNAL( triggered( bool ) ), SLOT( onSelectAll() ) );
  myActions.insert( SelectAllId, a );

  a = new QAction( tr( "&Save log to file..." ), this );
  a->setStatusTip( tr( "&Save log to file..." ) );
  connect( a, SIGNAL( triggered( bool ) ), SLOT( onSaveToFile() ) );
  myActions.insert( SaveToFileId, a );
}

/*!
  Redefined virtual method for popup filling
*/
void LogWindow::contextMenuPopup( QMenu* popup )
{
  if ( myOpFlags & CopyId )
    popup->addAction( myActions[ CopyId ] );
  if ( myOpFlags & ClearId )
    popup->addAction( myActions[ ClearId ] );
  
  popup->addSeparator();
  
  if ( myOpFlags & SelectAllId )
    popup->addAction( myActions[ SelectAllId ] );
  
  popup->addSeparator();
  
  if ( myOpFlags & SaveToFileId )
    popup->addAction( myActions[ SaveToFileId ] );

  Qtx::simplifySeparators( popup );

  updateActions();
}

/*!
  Updates enable status of actions
*/
void LogWindow::updateActions()
{
/*
  int paraFrom, paraTo, indexFrom, indexTo;
  myView->getSelection( &paraFrom, &indexFrom, &paraTo, &indexTo );
  bool allSelected = myView->hasSelectedText() &&
                     !paraFrom && paraTo == myView->paragraphs() - 1 && 
                     !indexFrom && indexTo == myView->paragraphLength( paraTo );
  myActions[ CopyId ]->setEnabled( ( myOpFlags & CopyId )&& myView->hasSelectedText() );
  myActions[ ClearId ]->setEnabled( ( myOpFlags & ClearId ) && myView->document()->blockCount() > myBannerSize );
  myActions[ SelectAllId ]->setEnabled( ( myOpFlags & SelectAllId ) && !allSelected );
  myActions[ SaveToFileId ]->setEnabled( ( myOpFlags & SaveToFileId ) && myHistory.count() > 0 );
*/
}

/*!
  SLOT: called if user click "Save" in popup
*/
void LogWindow::onSaveToFile()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( !app )
    return;

  // call application-specific "Save file" dialog box
  QString aName = app->getFileName( false, QString::null, QString( "*.log" ), QString::null, 0 );
  if ( aName.isNull() )
    return;

  QApplication::setOverrideCursor( Qt::WaitCursor );
    
  bool bOk = saveLog( aName );

  QApplication::restoreOverrideCursor();

  if ( !bOk )
    SUIT_MessageBox::error1( this, tr( "Error" ), tr( "Can't save file" ), tr( "OK" ) );
}

/*!
  SLOT: called if user click "Select all" in popup
*/
void LogWindow::onSelectAll()
{
  if ( myView )
    myView->selectAll();
}

/*!
  SLOT: called if user click "Clear" in popup
*/
void LogWindow::onClear()
{
  clear( false );
}

/*!
  SLOT: called if user click "Copy" in popup
*/
void LogWindow::onCopy()
{
  if ( myView )
    myView->copy();
}

void LogWindow::setOperationsFlags( int flags )
{
  myOpFlags = flags;
}
