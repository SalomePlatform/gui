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
//

#ifndef QTXMENU_H
#define QTXMENU_H

#include "Qtx.h"

#include <QMenu>
#include <QPointer>

class QWidgetAction;

class QTX_EXPORT QtxMenu : public QMenu
{
  Q_OBJECT

  Q_PROPERTY( bool menuCollapsible READ menuCollapsible WRITE setMenuCollapsible )
  Q_PROPERTY( int expandingDelay READ expandingDelay WRITE setExpandingDelay )
  Q_PROPERTY( TitleMode titleMode READ titleMode WRITE setTitleMode )
  Q_PROPERTY( Qt::Alignment titleAlignment READ titleAlignment WRITE setTitleAlignment )
  Q_PROPERTY( int collapseLimit READ collapseLimit WRITE setCollapseLimit )
  Q_PROPERTY( CollapseLimitMode collapseLimitMode READ collapseLimitMode WRITE setCollapseLimitMode )

  class Title;
  class TitleMgr;
  class Expander;

public:
  //! Popup menu title mode
  typedef enum {
    TitleAuto,        //!< auto mode
    TitleOn,          //!< always on (display title)
    TitleOff          //!< always off (do not display title)
  } TitleMode;

  typedef enum {
    LimitAuto,
    LimitFrequent,
    LimitTotal
  } CollapseLimitMode;

  typedef enum {
    HighlightNone,
    HighlightFrequent,
    HighlightPermanent
  } HighlightMode;

public:
  QtxMenu( QWidget* = 0 );
  virtual ~QtxMenu();

  TitleMode              titleMode() const;
  Qt::Alignment          titleAlignment() const;

  virtual void           setTitleMode( const TitleMode );
  virtual void           setTitleAlignment( const Qt::Alignment );

  // Methods for collapsing/expanding functionality
  bool                   menuCollapsible() const;
  void                   setMenuCollapsible( bool );

  bool                   isMenuExpanded() const;
  bool                   isMenuCollapsed() const;

  int                    expandingDelay() const;
  void                   setExpandingDelay( int );

  int                    collapseLimit() const;
  void                   setCollapseLimit( int );

  CollapseLimitMode      collapseLimitMode() const;
  void                   setCollapseLimitMode( const CollapseLimitMode );

  HighlightMode          highlightMode() const;
  void                   setHighlightMode( HighlightMode );

  static int             actionPriority( QAction* );
  static void            setActionPriority( QAction*, int );

  static bool            isPermanentAction( QAction* );
  static void            setPermanentAction( QAction*, bool );

public slots:
  virtual void           setVisible( bool );

private slots:
  void                   onExpandMenu();
  void                   onMenuActionChanged();
  void                   onActionHovered( QAction* );
  void                   onActionTriggered( QAction* );

protected:
  virtual void           paintEvent( QPaintEvent* );
  virtual void           keyPressEvent( QKeyEvent* );

private:
  void                   updateTitle();
  void                   insertTitle();
  void                   removeTitle();

  void                   expandMenu();
  void                   collapseMenu();
  void                   updateExpander();

  int                    collapseQuantity() const;
  QSet<QAction*>         collapsedActions() const;

  QWidget*               topLevelMenu() const;
  bool                   isTopLevelMenu() const;
  QWidget*               topLevelMenu( const QMenu* ) const;

private:
  typedef QPointer<QAction>   ActionPtr;
  typedef QList<ActionPtr>    ActionList;
  typedef QMap<QAction*, int> PriorityMap;
  typedef QSet<QAction*>      PermanentMap;

private:
  TitleMode              myTitleMode;
  Qt::Alignment          myTitleAlign;
  QWidgetAction*         myTitleAction;

  int                    myLimit;
  CollapseLimitMode      myLimitMode;
  HighlightMode          myHighlightMode;

  QTimer*                myShortTimer;
  QTimer*                myExpandTimer;
  Expander*              myExpandAction;
  ActionList             myActionBackup;

  static PriorityMap     _actionPriority;
  static PermanentMap    _permanentActions;
};

#endif // QTXMENU_H
