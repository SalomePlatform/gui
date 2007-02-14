//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
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
//
//  File   : PyConsole_Editor.h
//  Author : Vadim SANDLER
//  Module : SALOME

#ifndef PYCONSOLE_EDITOR_H
#define PYCONSOLE_EDITOR_H

#include "PyConsole_Interp.h" // this include must be first (see PyInterp_base.h)!

#include <QtGui/qtextedit.h>

class QMenu;
class QEventLoop;
class PyConsole_Interp;

class PYCONSOLE_EXPORT PyConsole_Editor : public QTextEdit
{
  Q_OBJECT;

public:
  PyConsole_Editor( PyInterp_base* theInterp, QWidget *theParent = 0 );
  ~PyConsole_Editor();
  
  virtual void   addText( const QString& str, const bool newBlock = false ); 
  bool           isCommand( const QString& str ) const;

  virtual void   exec( const QString& command );
  void           execAndWait( const QString& command );

protected:
  virtual void   dropEvent( QDropEvent* event );
  virtual void   mouseReleaseEvent( QMouseEvent* event );
  virtual void   keyPressEvent ( QKeyEvent* event);
  virtual void   customEvent( QEvent* event);
  
public slots:
  void           cut();
  void           paste();
  void           clear();
  void           handleReturn();
  void           onPyInterpChanged( PyInterp_base* );
  
private:
  PyInterp_base* myInterp;           //!< python interpreter

  QString        myCommandBuffer;    //!< python comman buffer
  QString        myCurrentCommand;   //!< currently being printed command
  QString        myPrompt;           //!< current command line prompt
  int            myCmdInHistory;     //!< current history command index
  QStringList    myHistory;          //!< commands history buffer
  QEventLoop*    myEventLoop;        //!< internal event loop
  QString        myBanner;           //!< current banner
  QStringList    myQueue;            //!< python commands queue
};

#endif
