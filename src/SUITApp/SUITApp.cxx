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
#if defined WIN32

#undef SUIT_ENABLE_PYTHON
//#else
//#include "SUITconfig.h"
#endif

#include "SUITApp_Application.h"

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>

// TODO
//#include <QtxSplash.h>

#ifdef SUIT_ENABLE_PYTHON
#include <Python.h>
#endif

#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qregexp.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

#include <stdlib.h>

QString salomeVersion()
{
  QString path( ::getenv( "GUI_ROOT_DIR" ) );
  if ( !path.isEmpty() )
    path += QDir::separator();

  path += QString( "bin/salome/VERSION" );

  QFile vf( path );
  if ( !vf.open( QFile::ReadOnly ) )
    return QString::null;

  QString line = vf.readLine( 1024 );
  vf.close();

  if ( line.isEmpty() )
    return QString::null;

  while ( !line.isEmpty() && line.at( line.length() - 1 ) == QChar( '\n' ) )
    line.remove( line.length() - 1, 1 );

  QString ver;
  int idx = line.lastIndexOf( ":" );
  if ( idx != -1 )
    ver = line.mid( idx + 1 ).trimmed();

  return ver;
}


/* XPM */
static const char* pixmap_not_found_xpm[] = {
"16 16 3 1",
"       c None",
".      c #000000",
"+      c #A80000",
"                ",
"                ",
"    .     .     ",
"   .+.   .+.    ",
"  .+++. .+++.   ",
"   .+++.+++.    ",
"    .+++++.     ",
"     .+++.      ",
"    .+++++.     ",
"   .+++.+++.    ",
"  .+++. .+++.   ",
"   .+.   .+.    ",
"    .     .     ",
"                ",
"                ",
"                "};

class SUITApp_Session: public SUIT_Session
{
public:
  SUITApp_Session( bool theIniFormat ) : SUIT_Session(), myIniFormat ( theIniFormat ) {}
  virtual ~SUITApp_Session() {}

  virtual SUIT_ResourceMgr* createResourceMgr( const QString& appName ) const
  {
    SUIT_ResourceMgr* resMgr = 0;
    if ( myIniFormat )
    {
      resMgr = new SUIT_ResourceMgr( appName, QString( "%1Config" ) );
      resMgr->setCurrentFormat( "ini" );
    }
    else
    {
      resMgr = new SUIT_ResourceMgr( appName, QString( "%1Config" ) );
      resMgr->setVersion( salomeVersion() );
      resMgr->setCurrentFormat( "xml" );
    }

    if ( resMgr )
    {
      static QPixmap defaultPixmap( pixmap_not_found_xpm );
      resMgr->setDefaultPixmap( defaultPixmap );
      resMgr->setOption( "translators", QString( "%P_msg_%L.qm|%P_icons.qm|%P_images.qm" ) );
    }
    return resMgr;
  }

private:
  bool  myIniFormat;
};

int main( int args, char* argv[] )
{
#ifdef SUIT_ENABLE_PYTHON
  Py_Initialize();
  PySys_SetArgv( args, argv );
#endif

  QStringList argList;
  bool noExceptHandling = false;
  bool iniFormat        = false;
  bool noSplash         = false;
  for ( int i = 1; i < args /*&& !noExceptHandling*/; i++ )
  {
    if ( !strcmp( argv[i], "--noexcepthandling" ) )
      noExceptHandling = true;
    else if ( !strcmp( argv[i], "--format=ini") )
      iniFormat = true;
    else if ( !strcmp( argv[i], "--nosplash") )
      noSplash = true;
    else
      argList.append( QString( argv[i] ) );
  }

  SUITApp_Application app( args, argv );

  int result = -1;
  if ( !argList.isEmpty() )
  {
    SUITApp_Session* aSession = new SUITApp_Session( iniFormat );
    // TODO
/*
    QtxSplash* splash = 0;
    if ( !noSplash )
    {
      SUIT_ResourceMgr* resMgr = aSession->createResourceMgr( argList.first() );
      if ( resMgr )
      {
	      resMgr->loadLanguage();
	      QString splashIcon, splashInfo, splashTextColors;
	      resMgr->value( "splash", "image",       splashIcon );
	      resMgr->value( "splash", "info",        splashInfo, false );
	      resMgr->value( "splash", "text_colors", splashTextColors );
	      QString appName    = QObject::tr( "APP_NAME" ).stripWhiteSpace();
	      QString appVersion = QObject::tr( "APP_VERSION" ).stripWhiteSpace();
	      if ( appVersion == "APP_VERSION" )
        {
	        if ( appName == "APP_NAME" || appName.toLower() == "salome" )
	          appVersion = salomeVersion();
	        else
	          appVersion = "";
	      }
	      QPixmap px( splashIcon );
	      if ( !px.isNull() )
        {
	        splash = QtxSplash::splash( px );
	        if ( !splashTextColors.isEmpty() )
          {
	          QStringList colors = QStringList::split( "|", splashTextColors );
	          QColor c1, c2;
	          if ( colors.count() > 0 )
              c1 = QColor( colors[0] );
	          if ( colors.count() > 1 )
              c2 = QColor( colors[1] );
	          splash->setTextColors( c1, c2 );
	        }
	        else
          {
	          splash->setTextColors( Qt::white, Qt::black );
	        }
#ifdef _DEBUG_
	        splash->setHideOnClick( true );
#endif
	        QFont f = splash->font();
	        f.setBold( true );
	        splash->setFont( f );
	        if ( !splashInfo.isEmpty() )
          {
	          splashInfo.replace( QRegExp( "%A" ),  appName );
	          splashInfo.replace( QRegExp( "%V" ),  QObject::tr( "ABOUT_VERSION" ).arg( appVersion ) );
	          splashInfo.replace( QRegExp( "%L" ),  QObject::tr( "ABOUT_LICENSE" ) );
	          splashInfo.replace( QRegExp( "%C" ),  QObject::tr( "ABOUT_COPYRIGHT" ) );
	          splashInfo.replace( QRegExp( "\\\\n" ), "\n" );
	          splash->message( splashInfo );
	        }
	        splash->show();
          QApplication::instance()->processEvents();
	      }
      }
    }
*/
    SUIT_Application* theApp = aSession->startApplication( argList.first() );
    if ( theApp )
    {
      if ( !noExceptHandling )
        app.setHandler( aSession->handler() );

// TODO
//      if ( splash )
//	      splash->finish( theApp->desktop() );

      result = app.exec();
// TODO
//      delete splash;
    }
    delete aSession;
  }

  return result;
}
