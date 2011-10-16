// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
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

// File:      QtxWebBrowser.cxx
// Author:    Roman NIKOLAEV
//
#include "QtxWebBrowser.h"

#include <QApplication>
#include <QWebView>
#include <QMenuBar>
#include <QToolBar>
#include <QMenu>

//! The only one instance of web browser
QtxWebBrowser* QtxWebBrowser::myBrowser = 0;

//! Internal data map to store resources of the browser.
QMap<QString, QVariant> QtxWebBrowser::myData;

/*!
  \class QtxWebBrowser

  \brief The QtxWebBrowser provides a window that can display html pages.
  
  Only one instance of the QtxWebBrowser class can be created. To access the browser 
  window, use static method QtxWebBrowser::webBrowser(), which creates an
  instance of the QtxWebBrowser widget (if it is not yet created) and returns a
  pointer to it.

  You should not destroy this instance - it is done automatically after
  closing of the browser window. To close window programmatically use 
  method close().

  To set visual properties of the browser use static method setData().

  The following sample demonstrates how to use web browser.
  In this code the browser window is created, /data/index.html file is opened
  and scrolled to the "anchor1" anchor on this page.

  \code
  int main(int argc, char *argv[])
  {
    QApplication app(argc, argv);    

    // set icon, title and menu items.
    QtxWebBrowser::setData("browser:title",      tr("Web Browser"));
    QtxWebBrowser::setData("browser:icon",       QPixmap(":/icon.png"));
    QtxWebBrowser::setData("menu:file:title",    tr("&File"));
    QtxWebBrowser::setData("action:close:title", tr("&Close"));

    // show HTML page
    QtxWebBrowser::loadUrl("file:///data/index.html", "anchor1");
    
    return app.exec();
  }
  \endcode

*/

/*!
  \brief Constructor.
 
  Construct the web browser.
*/
QtxWebBrowser::QtxWebBrowser() : QMainWindow( 0 )
{
  setAttribute( Qt::WA_DeleteOnClose );
  myWebView = new QWebView(this);
  
  myToolbar = addToolBar( tr( "Navigation" ) );
  myToolbar->addAction( myWebView->pageAction( QWebPage::Back ) );
  myToolbar->addAction( myWebView->pageAction( QWebPage::Forward ) );

  myMenus[ File ]    = menuBar()->addMenu( tr( "&File" ) );
  myActions[ Close ] = myMenus[ File ]->addAction( tr( "&Close" ), this, SLOT( close() ) );

  connect( myWebView, SIGNAL( titleChanged( QString ) ), SLOT( adjustTitle() ) ); 
  setCentralWidget( myWebView );

  updateData();
}

/*!
  \brief Destructor.
*/
QtxWebBrowser::~QtxWebBrowser()
{
  myBrowser = 0;
}

/*!
  \brief Return the only instance of the QtxWebBrowser
  \return instance of the QtxWebBrowser
*/
QtxWebBrowser* QtxWebBrowser::webBrowser()
{
  if ( !myBrowser )
    myBrowser = new QtxWebBrowser();
  return myBrowser;
}

/*!
  \brief Load given url address and optional scroll to the specified anchor
  \param url an url address to load
  \param anchor an anchor to scroll page to
*/
void QtxWebBrowser::loadUrl( const QString& url, const QString& anchor )
{
  QString anUrl = url;
  if( !anchor.isEmpty() ) anUrl += "#" + anchor;

  Qtx::alignWidget( webBrowser(), (QWidget*)QApplication::desktop(), Qtx::AlignCenter );

  webBrowser()->show();
  webBrowser()->myWebView->load( QUrl( anUrl ) );
  webBrowser()->setFocus();
  webBrowser()->activateWindow();
  webBrowser()->raise();
}

/*!
  \brief  Set browser settings from.

  This method can be used to setup the browser properties.
  - \c "browser:title"        : title of the browser window
  - \c "browser:icon"         : icon of the browser window
  - \c "toolbar:title"        : title of the toolbar
  - \c "menu:file:title"      : File menu of the browser
  - \c "action:close:title"   : File/Close menu item title
  - \c "action:close:icon"    : File/Close menu item icon
  - \c "action:back:title"    : Navigation/Back menu item title
  - \c "action:back:icon"     : Navigation/Back menu item icon
  - \c "action:forward:title" : Navigation/Forward menu item title
  - \c "action:forward:icon"  : Navigation/Forward menu item icon
  
  \param key name of the property
  \param val value of the property
  
*/
void QtxWebBrowser::setData( const QString& key, const QVariant& val )
{
  myData.insert( key, val );
  if ( myBrowser ) myBrowser->updateData();
}

/*!
  \brief Get string value by key from the internal data map
  \param key data key identifier
  \return string value assigned to the key (null string if data is not assigned to the key)
  \internal
*/
QString QtxWebBrowser::getStringValue( const QString& key )
{
  QString val;
  if ( myData.contains( key ) && myData[key].canConvert( QVariant::String ) )
    val = myData[key].toString();
  return val;
}

/*!
  \brief Get icon value by key from the internal data map
  \param key data key identifier
  \return icon assigned to the key (null icon if data is not assigned to the key)
  \internal
*/
QIcon QtxWebBrowser::getIconValue(const QString& key)
{
  QIcon val;
  if ( myData.contains( key ) ) {
    if ( myData[key].canConvert( QVariant::Pixmap ) )
      val = myData[key].value<QPixmap>();
    else if ( myData[key].canConvert( QVariant::Icon ) )
      val = myData[key].value<QIcon>();
  }
  return val;
}

/*!
  \brief Update web browser properties from internal data map
*/
void QtxWebBrowser::updateData()
{
  // main title
  adjustTitle();

  // window icon
  QIcon icon = getIconValue( "browser:icon" );
  if ( !icon.isNull() )
    setWindowIcon( icon );

  // toolbar title
  QString tbTitle = getStringValue( "toolbar:title" );
  if ( myToolbar && !tbTitle.isEmpty() )
    myToolbar->setWindowTitle( tbTitle );

  // File menu
  QString fmenu = getStringValue( "menu:file:title" );
  if ( myMenus.contains( File ) && !fmenu.isEmpty() )
    myMenus[ File ]->setTitle( fmenu );

  // File/Close menu
  QString closeTlt = getStringValue( "action:close:title" );
  QIcon closeIco = getIconValue( "action:close:icon" );
  if ( myActions.contains( Close ) ) {
    if ( !closeTlt.isEmpty() )
      myActions[ Close ]->setText( closeTlt );
    if ( !closeIco.isNull() )
      myActions[ Close ]->setIcon( closeIco );
  }

  // Navigation/Go Back menu
  QString backTlt = getStringValue( "action:back:title" );
  QIcon backIco = getIconValue( "action:back:icon" );
  if ( !backTlt.isEmpty() )
    myWebView->pageAction( QWebPage::Back )->setText( backTlt );
  if ( !backIco.isNull() )
    myWebView->pageAction( QWebPage::Back )->setIcon( backIco );

  // Navigation/Go Forward menu
  QString fwdTlt = getStringValue( "action:forward:title" );
  QIcon fwdIco = getIconValue( "action:forward:icon" );
  if ( !fwdTlt.isEmpty() )
    myWebView->pageAction( QWebPage::Forward )->setText( fwdTlt );
  if ( !fwdIco.isNull() )
    myWebView->pageAction( QWebPage::Forward )->setIcon( fwdIco );
}

/*!
  \brief Update title of the window
  \internal
*/
void QtxWebBrowser::adjustTitle()
{
  QString title = getStringValue( "browser:title" );
  setWindowTitle( title.isEmpty() ? myWebView->title() : title + QString( " [%1]" ).arg( myWebView->title() ) );
}
