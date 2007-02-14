//  KERNEL SALOME_Event : Define event posting mechanism
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
//  File   : LogWindow.h
//  Author : 
//  Module : KERNEL

#ifndef LogWindow_HeaderFile
#define LogWindow_HeaderFile

#if defined LOGWINDOW_EXPORTS
#if defined WIN32
#define LOGWINDOW_EXPORT __declspec( dllexport )
#else
#define LOGWINDOW_EXPORT
#endif
#else
#if defined WIN32
#define LOGWINDOW_EXPORT __declspec( dllimport )
#else
#define LOGWINDOW_EXPORT
#endif
#endif

#include <SUIT_PopupClient.h>

#include <QtCore/qmap.h>
#include <QtCore/qstringlist.h>

#include <QtGui/qframe.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QAction;
class QTextEdit;

/*!
  \class LogWindow
  Widget, showing logs message. Allows to show, to clear, to copy messages and to save then to file
*/
class LOGWINDOW_EXPORT LogWindow : public QFrame, public SUIT_PopupClient
{
  Q_OBJECT

public:
  //! popup operation flags
  enum
  {
    CopyId = 0x01,
    ClearId = 0x02,
    SelectAllId = 0x04,
    SaveToFileId = 0x08,
    All = CopyId | ClearId | SelectAllId | SaveToFileId,
  };

public:
  LogWindow( QWidget* theParent );
  virtual ~LogWindow();

  virtual             QString popupClientType() const { return QString( "LogWindow" ); }
  virtual void        contextMenuPopup( QMenu* );

  virtual bool        eventFilter( QObject* o, QEvent* e );

  void                setBanner( const QString& banner );
  void                setSeparator( const QString& separator );

  virtual void        putMessage( const QString& message, bool addSeparator = true );
  void                clear( bool clearHistory = false );

  bool                saveLog( const QString& fileName );

  void                setOperationsFlags( int flags );

protected slots:
  void                onSaveToFile();
  void                onSelectAll();
  void                onClear();
  void                onCopy();

private:
  void                createActions();
  void                updateActions();

private:
  QTextEdit*          myView;
  QString             myBanner;
  QStringList         myHistory;
  QString             mySeparator;
  int                 myBannerSize;
  QMap<int, QAction*> myActions;
  int                 myOpFlags;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
