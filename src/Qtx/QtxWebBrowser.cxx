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

//!Internal data map to store resources of the browser.
QMap<QString, QVariant> QtxWebBrowser::myData;

/*!
  \class QtxWebBrowser

  \brief The QtxWebBrowser provides a window that can display a html pages.
  
  Only one instance of the QtxWebBrowser class can be created. To access the browser 
  window, use static method QtxWebBrowser::webBrowser(), which creates an
  instance of the QtxWebBrowser widget (if it is not yet creaed) and returns a
  pointer to it.

  You should not destroy this instance - it is done automatically after
  closing of the browser window. To closing window programmatically use 
  method close().

  To set visual properties of the browser use static method setData().

  Following code demonstrate how to use web browser:

  \code
  int main(int argc, char *argv[])
  {
    QApplication app(argc, argv);    

    //Set icon, title and menu items.
    QtxWebBrowser::setData("BROWSER_TITLE", tr("Web Browser"));

    QPixmap pixmap(":/icon.png");
    QtxWebBrowser::setData("BROWSER_ICON", pixmap);

    QtxWebBrowser::setData("BROWSER_FILEMENU", tr("&File"));
    QtxWebBrowser::setData("BROWSER_EXIT", tr("&Exit"));


    //Show web page
    QtxWebBrowser::loadUrl("file:///data/index.html", "anchor1");
    
    return app.exec();
  }
  
  \endcode

  This code will create the browser window and open /data/index.html 
  file in "anchor1" context.
*/

/*!
  \brief Constructor.
  
  Construct the web browser.
*/
QtxWebBrowser::QtxWebBrowser() 
: QMainWindow(0)
{
  setAttribute( Qt::WA_DeleteOnClose );
  myWebView = new QWebView(this);
  
  QToolBar *toolBar = addToolBar(tr("Navigation"));
  toolBar->addAction(myWebView->pageAction(QWebPage::Back));
  toolBar->addAction(myWebView->pageAction(QWebPage::Forward));

  QString title = getStringValue("BROWSER_TITLE");
  if(!title.isEmpty())
    setWindowTitle(title);
  
  QIcon icon = getIconValue("BROWSER_ICON");
  if(!icon.isNull())
    setWindowIcon(icon);

  QString fmenu = getStringValue("BROWSER_FILEMENU");
  if ( fmenu.isEmpty() ) {
    fmenu = tr("&File");
  }
  QMenu *menu = menuBar()->addMenu(fmenu);
  
  QString item = getStringValue("BROWSER_EXIT");
  if ( item.isEmpty() ) {
    item = tr("&Exit");
  }  
  menu->addAction( item, this, SLOT(close()));

  connect(myWebView, SIGNAL(titleChanged(QString)), SLOT(adjustTitle())); 
  setCentralWidget(myWebView);
}

/*!
  \brief Destructor.
*/
QtxWebBrowser::~QtxWebBrowser()
{
  myBrowser = 0;
}

/*!
  \brief Create instance of the QtxWebBrowser.
  \return instance of the QtxWebBrowser.
*/
QtxWebBrowser* QtxWebBrowser::webBrowser() {
  if( myBrowser == 0 )
    myBrowser = new QtxWebBrowser();
  return myBrowser;
}

/*!
  \brief Load given url addres and context.
  \param theUlr url address to load.
  \param theContext url context to load.
*/
void QtxWebBrowser::loadUrl(const QString& theUrl , const QString theContext ) {
  QString anUrl = theUrl;
  if( !theContext.isEmpty() )
    anUrl += "#" + theContext;

  Qtx::alignWidget(webBrowser(), (QWidget*)QApplication::desktop(),Qtx::AlignHCenter);
  webBrowser()->show();
  webBrowser()->myWebView->load(QUrl(anUrl));
  webBrowser()->setFocus();
  webBrowser()->activateWindow();
  webBrowser()->raise();
}

/*!
  \brief  Set browser settings from.

  This method can be used to setup the browser look-n-feel.

  All the browser parameters can be defined via resources file:
  - \c "BROWSER_TITLE" : title of the browser
  - \c "BROWSER_ICON" : icon of the browser
  - \c "BROWSER_FILEMENU" : name of the browser menu
  - \c "BROWSER_EXIT" : name of the browser menu item
  
  \param key name of the property
  \param val value of the property
  
*/
void QtxWebBrowser::setData( const QString& key, const QVariant& val ) {
  myData.insert(key, val);
}

/*!
  \brief Get string by name from the internal data map.
  \internal
*/
QString QtxWebBrowser::getStringValue(const QString& key) {
  QString val;
  if(myData.contains(key)) {
    QVariant v = myData[key];
    if( v.type() == QVariant::String ) {
      val = v.toString();
    }
  }
  return val;
}

/*!
  \brief Get icon by name from the internal data map.
  \internal
*/
QIcon QtxWebBrowser::getIconValue(const QString& key) {
  QIcon val;
  if(myData.contains(key)) {
    QVariant v = myData[key];
    if( v.type() == QVariant::Pixmap ) {
      val = v.value<QPixmap>();
    }
  }  
  return val;
}

/*!
  \brief Update title of the window.
  \internal
*/
void QtxWebBrowser::adjustTitle()
{
  QString title = getStringValue("BROWSER_TITLE");
  title.isEmpty() ? title = myWebView->title() : title += " : " + myWebView->title();
  setWindowTitle(title);
}
