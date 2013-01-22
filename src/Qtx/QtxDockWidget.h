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
// File:      QtxDockWidget.h
// Author:    Sergey TELKOV

#ifndef QTXDOCKWIDGET_H
#define QTXDOCKWIDGET_H

#include "Qtx.h"
#include <QDockWidget>
#include <QWindowsStyle>
#include <QLabel>

class QToolButton;

class QTX_EXPORT QtxDockWidget : public QDockWidget
{
  Q_OBJECT

  class Watcher;

public:

  enum TitleWidgetRole { 
    TitleText,
    Undock, 
    Close,
    Last
  };

  QtxDockWidget( const QString&, QWidget* = 0, Qt::WindowFlags = 0 );
  QtxDockWidget( const bool, QWidget* = 0, Qt::WindowFlags = 0 );
  QtxDockWidget( QWidget*, Qt::WindowFlags = 0 );
  virtual ~QtxDockWidget();

  virtual QSize   sizeHint() const;
  virtual QSize   minimumSizeHint() const;

  Qt::Orientation orientation() const;

  void            enableCustomTitleBar( const bool );
  bool            customTitleBarEnabled() const;
  QWidget*        customTitleBarWidget( const int ) const;
  int             customTitleBarRole( QWidget* ) const;
  void            insertCustomTitleBarWidget( const int, QWidget*, const int = Last );
  void            removeCustomTitleBarWidget( const int );

  void            updateCustomTitleBar();

signals:
  void            orientationChanged( Qt::Orientation );

public slots:
  virtual void    setVisible( bool );
  void            toggleTopLevel();

protected:
  virtual void    resizeEvent( QResizeEvent* );

private:
  void            updateState();

private:
  Watcher*        myWatcher;       //!< watcher object
  Qt::Orientation myOrientation;   //!< dockable window orientation
};

/*!
  \class QtxDockTitleStyle
  \internal
  \brief Internal class that implements customizable label style for a dock widget.
*/
class QtxDockTitleStyle : public QWindowsStyle
{
  Q_OBJECT

public:
  QtxDockTitleStyle( const bool& isVertical );
  virtual ~QtxDockTitleStyle();

  void   setVertical( const bool& isVertical );
  bool   isVertical() const;

  virtual void drawItemText( QPainter* painter, const QRect& rectangle,
                      int alignment, const QPalette& palette,
                      bool enabled, const QString& text,
		      QPalette::ColorRole textRole = QPalette::NoRole ) const;
private:
  bool   m_bIsVertical;
};

/*!
  \class QtxDockTitleLabel
  \internal
  \brief Internal class that implements customizable title label for a dock widget.
*/
class QtxDockTitleLabel : public QLabel
{
  Q_OBJECT
public:

  QtxDockTitleLabel( const QString& theText, QWidget* theParent );
  virtual ~QtxDockTitleLabel();

  void          setVertical( const bool& isVertical );
  bool          isVertical() const;

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

private:
  bool          m_bIsVertical;
  bool          m_bIsModified;
};

/*!
  \class QtxDockWidgetTitle
  \internal
  \brief Internal class that implements customizable title bar for a dock widget.

  This is a convenient implementation of an application-defined title bar widget.
  It contains a label with the window title, Undock and Close buttons by default, 
  and any number of widgets can be inserted at required position into the title's QHBoxLayout.
  Support for vertical title bar is not yet implemented.
*/

class QtxDockWidgetTitle : public QWidget
{
  Q_OBJECT

public:

  QtxDockWidgetTitle( QtxDockWidget* parent );
  virtual ~QtxDockWidgetTitle();

  QWidget*            widget( const int ) const;
  int                 role( QWidget* ) const;

  void                insertWidget( const int, QWidget* widget, const int = QtxDockWidget::Last );
  void                removeWidget( const int );

protected:
  virtual void        paintEvent ( QPaintEvent* event );

public slots:
  virtual void        updateTitleBar();

private:
  void                initStyleOption( QStyleOptionDockWidget* ) const;
  void                setupButton( QToolButton*, const int, QStyleOptionDockWidget*, QDockWidget* );
  void                setLayout( const bool& );
  void                fillLayout();


private:
  typedef QPair< int, QWidget* > RoleWidget;
  typedef QList< RoleWidget > RoleWidgetList;

  RoleWidgetList      myWidgets;
};


#endif // QTXDOCKWIDGET_H
