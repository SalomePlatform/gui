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

// File:      QtxWebBrowser.h
// Author:    Roman NIKOLAEV
//

#ifndef QTXWEBBROWSER_H
#define QTXWEBBROWSER_H

#include "Qtx.h"

#include <QMainWindow>
#include <QWebView>
#include <QMap>


class QTX_EXPORT QtxWebBrowser : public QMainWindow
{
  Q_OBJECT

private:
  QtxWebBrowser();
  
public:
  virtual ~QtxWebBrowser();
  
  static                          QtxWebBrowser* webBrowser();
  static void                     loadUrl( const QString& theUrl, const QString theContext = QString() );
  static void                     setData( const QString& key, const QVariant& val );

private:
  QString                         getStringValue(const QString& key);
  QIcon                           getIconValue(const QString& key);

private slots:
    void                          adjustTitle();    
  
private:
 static QtxWebBrowser*            myBrowser;
  QWebView*                       myWebView;
  static QMap<QString, QVariant>  myData;
  
};

#endif // QTXWEBBROWSER_H