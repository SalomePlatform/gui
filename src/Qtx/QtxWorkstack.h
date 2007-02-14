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
// File:      QtxWorkstack.h
// Author:    Sergey TELKOV

#ifndef QTXWORKSTACK_H
#define QTXWORKSTACK_H

#include "Qtx.h"

#include <QtCore/qmap.h>
#include <QtGui/qevent.h>
#include <QtGui/qframe.h>
#include <QtGui/qtabbar.h>

class QAction;
class QTabBar;
class QPainter;
class QSplitter;
class QPushButton;
class QRubberBand;
class QStackedWidget;

class QtxWorkstackArea;
class QtxWorkstackDrag;
class QtxWorkstackChild;
class QtxWorkstackTabBar;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxWorkstack : public QWidget
{
  Q_OBJECT

public:
  enum { SplitVertical, SplitHorizontal, Close, Rename };
    
  enum SplitType
  {
    SPLIT_STAY, //!< given widget stays in its workarea, others are moved into a new one
    SPLIT_AT,   //!< widgets before a given widget stays in they workarea, others are moved into a new one
    SPLIT_MOVE  //!< given widget is moved into a new workarea, others stay in an old one
  };

public:
  QtxWorkstack( QWidget* = 0 );
  virtual ~QtxWorkstack();

  QWidgetList         windowList() const;
  QWidgetList         splitWindowList() const;

  QWidget*            activeWindow() const;

  int                 accel( const int ) const;
  void                setAccel( const int, const int );

  void                split( const int );

  QWidget*            addWindow( QWidget*, Qt::WindowFlags = 0 );

  void Split( QWidget* wid, const Qt::Orientation o, const SplitType type );
  void Attract( QWidget* wid1, QWidget* wid2, const bool all );
  void SetRelativePosition( QWidget* wid, const Qt::Orientation o, const double pos );
  void SetRelativePositionInSplitter( QWidget* wid, const double pos );

  // asv: Store/Restore visual parameters - geometry of inner windows
  QtxWorkstack& operator<<( const QString& );
  QtxWorkstack& operator>>( QString& );

Q_SIGNALS:
  void                windowActivated( QWidget* );

public Q_SLOTS:
  void                splitVertical();
  void                splitHorizontal();
  
private Q_SLOTS:
  void                onRename();
  void                onCloseWindow();
  void                onDestroyed( QObject* );
  void                onWindowActivated( QWidget* );
  void                onContextMenuRequested( QWidget*, QPoint );
  void                onDeactivated( QtxWorkstackArea* );

protected:
  virtual void        customEvent( QEvent* );

private:
  QSplitter*          splitter( QtxWorkstackArea* ) const;
  void                splitters( QSplitter*, QList<QSplitter*>&, const bool = false ) const;
  void                areas( QSplitter*, QList<QtxWorkstackArea*>&, const bool = false ) const;

  QSplitter*          wrapSplitter( QtxWorkstackArea* );
  void                insertWidget( QWidget*, QWidget*, QWidget* );

  QtxWorkstackArea*   areaAt( const QPoint& ) const;

  QtxWorkstackArea*   targetArea();
  QtxWorkstackArea*   activeArea() const;
  QtxWorkstackArea*   currentArea() const;

  void                setActiveArea( QtxWorkstackArea* );
  QtxWorkstackArea*   neighbourArea( QtxWorkstackArea* ) const;

  QtxWorkstackArea*   createArea( QWidget* ) const;

  void                updateState();
  void                updateState( QSplitter* );

  void                distributeSpace( QSplitter* ) const;
  int                 setPosition( QWidget* wid, QSplitter* split, const Qt::Orientation o,
				                           const int need_pos, const int splitter_pos );
  
  void                splitterInfo( QSplitter*, QString& ) const;
  void                setSplitter( QSplitter*, const QString&, QMap< QSplitter*, QList<int> >& );
  
private:
  QWidget*            myWin;
  QtxWorkstackArea*   myArea;
  QSplitter*          mySplit;
  QWidget*            myWorkWin;
  QtxWorkstackArea*   myWorkArea;

  QMap<int, QAction*> myActionsMap; //!< The map of the actions. Allows to get the QAction object by the key.

  friend class QtxWorkstackArea;
  friend class QtxWorkstackDrag;
};

class QtxWorkstackArea : public QFrame
{
  Q_OBJECT

  class WidgetEvent;

public:
  QtxWorkstackArea( QWidget* );
  virtual ~QtxWorkstackArea();

  bool                isEmpty() const;

  QWidget*            insertWidget( QWidget*, const int = -1, Qt::WindowFlags = 0 );
  void                removeWidget( QWidget*, const bool = true );

  QWidget*            activeWidget() const;
  void                setActiveWidget( QWidget* );

  bool                contains( QWidget* ) const;

  QWidgetList         widgetList() const;

  bool                isActive() const;
  void                updateActiveState();

  QtxWorkstack*       workstack() const;

  virtual bool        eventFilter( QObject*, QEvent* );

  QRect               floatRect() const;
  QRect               floatTab( const int ) const;

  int                 tabAt( const QPoint& ) const;

Q_SIGNALS:
  void                activated( QWidget* );
  void                contextMenuRequested( QWidget*, QPoint );
  void                deactivated( QtxWorkstackArea* );

public Q_SLOTS:
  virtual void        setVisible( bool );

private Q_SLOTS:
  void                onClose();
  void                onCurrentChanged( int );

  void                onWidgetDestroyed();

  void                onChildDestroyed( QObject* );
  void                onChildShown( QtxWorkstackChild* );
  void                onChildHided( QtxWorkstackChild* );
  void                onChildActivated( QtxWorkstackChild* );
  void                onChildCaptionChanged( QtxWorkstackChild* );

  void                onDragActiveTab();
  void                onContextMenuRequested( QPoint );

protected:
  virtual void        customEvent( QEvent* );
  virtual void        focusInEvent( QFocusEvent* );
  virtual void        mousePressEvent( QMouseEvent* );

private:
  enum { ActivateWidget = QEvent::User, FocusWidget, RemoveWidget };

private:
  void                updateState();
  void                updateCurrent();
  void                updateTab( QWidget* );

  QWidget*            widget( const int ) const;
  int                 widgetId( QWidget* ) const;
  bool                widgetVisibility( QWidget* ) const;

  void                setWidgetActive( QWidget* );
  void                setWidgetShown( QWidget*, const bool );

  int                 generateId() const;

  bool                isBlocked( QWidget* ) const;
  void                setBlocked( QWidget*, const bool );

  QtxWorkstackChild*  child( QWidget* ) const;

private:
  struct WidgetInfo
  {
    WidgetInfo() : id( 0 ), vis( false ) {}
    int id; bool vis;
  };

  typedef QMap<QWidget*, bool>               BlockMap;
  typedef QMap<QWidget*, QtxWorkstackChild*> ChildMap;
  typedef QMap<QWidget*, WidgetInfo>         WidgetInfoMap;

private:
  QtxWorkstackTabBar* myBar;
  QPushButton*        myClose;
  QStackedWidget*     myStack;

  QWidgetList         myList;
  WidgetInfoMap       myInfo;
  ChildMap            myChild;
  BlockMap            myBlock;
};

class QtxWorkstackChild : public QWidget
{
  Q_OBJECT

public:
  QtxWorkstackChild( QWidget*, QWidget* = 0, Qt::WindowFlags = 0 );
  virtual ~QtxWorkstackChild();

  QWidget*            widget() const;

  virtual bool        eventFilter( QObject*, QEvent* );

Q_SIGNALS:
  void                shown( QtxWorkstackChild* );
  void                hided( QtxWorkstackChild* );
  void                activated( QtxWorkstackChild* );
  void                captionChanged( QtxWorkstackChild* );

private Q_SLOTS:
  void                onDestroyed( QObject* );

protected:
  virtual void        childEvent( QChildEvent* );

private:
  QWidget*            myWidget;
};

class QtxWorkstackTabBar : public QTabBar
{
  Q_OBJECT

public:
  QtxWorkstackTabBar( QWidget* = 0 );
  virtual ~QtxWorkstackTabBar();

  void                setActive( const bool );

  int                 tabId( const int ) const;
  int                 indexOf( const int ) const;
  void                setTabId( const int, const int );

Q_SIGNALS:
  void                dragActiveTab();
  void                contextMenuRequested( QPoint );

protected:
  virtual void        mouseMoveEvent( QMouseEvent* );
  virtual void        mousePressEvent( QMouseEvent* );
  virtual void        mouseReleaseEvent( QMouseEvent* );
  virtual void        contextMenuEvent( QContextMenuEvent* );

//  virtual void        paintLabel( QPainter*, const QRect&, QTab*, bool ) const;

private:
  int                 myId;
};

class QtxWorkstackDrag : public QObject
{
  Q_OBJECT

public:
  QtxWorkstackDrag( QtxWorkstack*, QtxWorkstackChild* );
  virtual ~QtxWorkstackDrag();

  virtual bool        eventFilter( QObject*, QEvent* );

private:
  void                dropWidget();

  void                updateTarget( const QPoint& );
  QtxWorkstackArea*   detectTarget( const QPoint&, int& ) const;
  void                setTarget( QtxWorkstackArea*, const int );

  void                drawRect();
  void                endDrawRect();
  void                startDrawRect();

private:
  QtxWorkstack*       myWS;
  QtxWorkstackChild*  myChild;

  int                 myTab;
  QtxWorkstackArea*   myArea;
  QRubberBand*        myTabRect;
  QRubberBand*        myAreaRect;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
