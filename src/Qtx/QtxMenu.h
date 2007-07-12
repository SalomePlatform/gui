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
// File:      QtxMenu.h
// Author:    Sergey TELKOV

#ifndef QTXMENU_H
#define QTXMENU_H

#include "Qtx.h"

#include <QMenu>

class QWidgetAction;

class QTX_EXPORT QtxMenu : public QMenu
{
  Q_OBJECT

  class Title;

public:
  typedef enum { TitleAuto, TitleOn, TitleOff } TitleMode;

public:
  QtxMenu( QWidget* = 0 );
  virtual ~QtxMenu();

  QIcon                  titleIcon() const;
  QString                titleText() const;

  TitleMode              titleMode() const;
  Qt::Alignment          titleAlignment() const;

  virtual void           setTitleIcon( const QIcon& );
  virtual void           setTitleText( const QString& );

  virtual void           setTitleMode( const TitleMode );
  virtual void           setTitleAlignment( const Qt::Alignment );

public slots:
  virtual void           setVisible( bool );

private:
  void                   updateTitle();
  void                   insertTitle();
  void                   removeTitle();

private:
  TitleMode              myMode;
  Title*                 myTitle;
  QWidgetAction*         myAction;
};

#endif
