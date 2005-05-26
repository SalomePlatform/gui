// File:      Qtx.h
// Author:    Sergey TELKOV

#ifndef QTX_H
#define QTX_H

#if defined QTX_EXPORTS
#if defined WIN32
#define QTX_EXPORT _declspec( dllexport )
#else
#define QTX_EXPORT  
#endif
#else
#if defined WIN32
#define QTX_EXPORT _declspec( dllimport )
#else
#define QTX_EXPORT  
#endif
#endif

#if defined SOLARIS
#define bool  int
#define false 0
#define true  1
#endif

#ifndef INCLUDE_MENUITEM_DEF
#define INCLUDE_MENUITEM_DEF
#endif

#include <qnamespace.h>

#ifndef QT_VERSION
#define QT_VER 0
#else
#if QT_VERSION >= 0x30000
#define QT_VER 3
#else
#if QT_VERSION >= 300
#define QT_VER 3
#else
#if QT_VERSION >= 200
#define QT_VER 2
#else
#if QT_VERSION >= 100
#define QT_VER 1
#endif
#endif
#endif
#endif
#endif

class QObject;
class QString;
class QWidget;
class QToolBar;
class QGroupBox;
class QPopupMenu;
class QWidgetList;

template <class> class QValueList;

#if QT_VER < 3
#define QPtrList QList
#define QPtrListIterator QListIterator
#endif

typedef QValueList<int>    QIntList;
typedef QValueList<short>  QShortList;
typedef QValueList<double> QDoubleList;

class QTX_EXPORT Qtx : public Qt
{
public:
  enum AlignmentFlags
	{
	  AlignOutLeft   = AlignVCenter  << 2,
		AlignOutRight  = AlignOutLeft  << 2,
    AlignOutTop    = AlignOutRight << 2,
    AlignOutBottom = AlignOutTop   << 2
  };

  static void    setTabOrder( QWidget*, ... );
	static void    setTabOrder( const QWidgetList& );
	static void    alignWidget( QWidget*, const QWidget*, const int );

  static void    simplifySeparators( QToolBar* );
  static void    simplifySeparators( QPopupMenu*, const bool = true );

  static bool    isParent( QObject*, QObject* );

	static QString extension( const QString& );
	static QString dir( const QString&, const bool = true );
	static QString file( const QString&, const bool = true );

  static QString library( const QString& );

	static QString tmpDir();
	static bool    mkDir( const QString& );
	static bool    rmDir( const QString& );
	static bool    dos2unix( const QString& );
	static QString addSlash( const QString& );

	static int     rgbSet( const QColor& );
	static int     rgbSet( const int, const int, const int );

	static void    rgbSet( const int, QColor& );
	static void    rgbSet( const int, int&, int&, int& );
};

#endif
