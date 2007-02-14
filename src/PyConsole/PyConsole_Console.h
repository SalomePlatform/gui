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
//  File   : PythonConsole_PyConsole.h
//  Author : Vadim SANDLER
//  Module : SALOME

#ifndef PYCONSOLE_CONSOLE_H
#define PYCONSOLE_CONSOLE_H

#include "PyConsole.h"

#include <QtCore/qmap.h>
#include <QtGui/qframe.h>

#include <SUIT_PopupClient.h>

class PyInterp_base;
class PyConsole_Editor;

class PYCONSOLE_EXPORT PythonConsole : public QFrame, public SUIT_PopupClient
{
  Q_OBJECT

public:
  enum
  {
    CopyId = 0x01,
    PasteId = 0x02,
    ClearId = 0x04,
    SelectAllId = 0x08,
    All = CopyId | PasteId | ClearId | SelectAllId
  };

public:
  PythonConsole( QWidget* parent, PyInterp_base* interp = 0 );
  virtual ~PythonConsole();

  //! \brief Get python interperter
  PyInterp_base* getInterp() { return myInterp; } 
  QFont          font() const;
  virtual void   setFont( const QFont& );

  void           exec( const QString& command );
  void           execAndWait( const QString& command );

  virtual bool   eventFilter( QObject* o, QEvent* e );

  virtual QString popupClientType() const { return QString( "PyConsole" ); }
  virtual void    contextMenuPopup( QMenu* );

private:
  void                createActions();
  void                updateActions();

private:
  PyInterp_base*      myInterp;    //!< python interpreter
  PyConsole_Editor*   myEditor;    //!< python console editor widget
  QMap<int, QAction*> myActions;
};



#endif
