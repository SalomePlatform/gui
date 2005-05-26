// File:      QtxDockAction.h
// Author:    Sergey TELKOV

#ifndef QTX_DOCKACTION_H
#define QTX_DOCKACTION_H

#include "QtxAction.h"

#include <qevent.h>
#include <qptrlist.h>
#include <qpopupmenu.h>

class QAction;
class QMainWindow;
class QDockWindow;
class QtxResourceMgr;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxDockAction : public QtxAction
{
  Q_OBJECT

  Q_PROPERTY( bool autoAdd   READ isAutoAdd   WRITE setAutoAdd )
  Q_PROPERTY( bool autoPlace READ isAutoPlace WRITE setAutoPlace )
  Q_PROPERTY( bool separate  READ isSeparate  WRITE setSeparate )

public:
  QtxDockAction( QMainWindow*, const char* = 0 );
  QtxDockAction( const QString&, const QString&, QMainWindow*, const char* = 0 );
  QtxDockAction( const QString&, const QIconSet&, const QString&, QMainWindow*, const char* = 0 );
  virtual ~QtxDockAction();

  QMainWindow* mainWindow() const;

  bool         isAutoAdd() const;
  void         setAutoAdd( const bool );

  bool         isAutoPlace() const;
  void         setAutoPlace( const bool );

  bool         isSeparate() const;
  void         setSeparate( const bool );

  virtual bool addTo( QWidget* );
  virtual bool addTo( QWidget*, const int );
  virtual bool removeFrom( QWidget* );

  bool         addDockWindow( QDockWindow* );
  bool         removeDockWindow( QDockWindow* );

  virtual bool eventFilter( QObject*, QEvent* );

  virtual void storeGeometry( QDockWindow* = 0 );
  virtual void restoreGeometry( QDockWindow* = 0 ) const;

  virtual void loadGeometry( QtxResourceMgr*, const QString&, const bool = true );
  virtual void saveGeometry( QtxResourceMgr*, const QString&, const bool = true ) const;

private slots:
  void         onAboutToShow();
  void         onToggled( bool );
  void         onVisibilityChanged( bool );
  void         onPopupDestroyed( QObject* );
  void         onWindowDestroyed( QObject* );
  void         onDockWindowPositionChanged( QDockWindow* );

protected:
  virtual bool event( QEvent* );

private:
  QAction*     action( QDockWindow* ) const;
  QDockWindow* dockWindow( const QAction* ) const;

  void         checkPopup( QPopupMenu* );
  void         fillPopup( QPopupMenu* ) const;
  int          findId( QPopupMenu*, QPopupMenu* ) const;

  void         initialize( QMainWindow* );
  void         updateInfo( QDockWindow* );
  bool         isToolBar( QDockWindow* ) const;
  void         dockWindows( QPtrList<QDockWindow>&, QMainWindow* = 0 ) const;

  QString      windowName( QDockWindow* ) const;
  void         savePlaceInfo( QDockWindow* );
  void         loadPlaceInfo( QDockWindow* ) const;

  bool         autoAddDockWindow( QDockWindow* );
  void         autoLoadPlaceInfo( QDockWindow* );

  void         splitMenuText( QString&, QString& ) const;
  QStringList  splitText( const QString&, const QString& ) const;

  int          dockPlace( const QString& ) const;

private:
  enum { AutoAdd = QEvent::User };

  typedef struct { bool vis, newLine;
                   int place, index, offset;
                   int x, y, w, h, fixW, fixH; } GeomInfo;
  typedef struct { QString name; QAction* a; }   DockInfo;
  typedef struct { QPopupMenu *dock, *tool; }    MenuInfo;

  typedef QMap<QDockWindow*, DockInfo>   InfoMap;
  typedef QMap<QString, GeomInfo>        GeomMap;
  typedef QMap<QPopupMenu*, MenuInfo>    MenuMap;

private:
  bool         loadGeometry( QtxResourceMgr*, const QString&,
                             const QString&, GeomInfo& ) const;
  bool         saveGeometry( QtxResourceMgr*, const QString&,
                             const QString&, const GeomInfo& ) const;

private:
  InfoMap      myInfo;
  MenuMap      myMenu;
  GeomMap      myGeom;
  QMainWindow* myMain;

  bool         myAutoAdd;
  bool         mySeparate;
  bool         myAutoPlace;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
