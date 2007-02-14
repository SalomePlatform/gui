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

#define QT_VER 4

#include <Qt/qnamespace.h>

#include <QtGui/qcolor.h>
#include <QtGui/qimage.h>
#include <QtGui/qpixmap.h>

class QMenu;
class QObject;
class QString;
class QWidget;
class QToolBar;

template <class> class QList;

typedef QList<int>    QIntList;
typedef QList<short>  QShortList;
typedef QList<double> QDoubleList;

/*!
  \class Qtx
  \brief Set of auxiliary static methods
*/

#ifndef QT_MOC_RUN
class QTX_EXPORT Qtx
#else
class QTX_EXPORT Qtx : public Qt
#endif
{
public:
  enum AlignmentFlags
  {
    AlignLeft = Qt::AlignLeft,
    AlignLeading = Qt::AlignLeading,
    AlignRight = Qt::AlignRight,
    AlignTrailing = Qt::AlignTrailing,
    AlignHCenter = Qt::AlignHCenter,
    AlignJustify = Qt::AlignJustify,
    AlignAbsolute = Qt::AlignAbsolute,
    AlignHorizontal_Mask = Qt::AlignHorizontal_Mask,

    AlignTop = Qt::AlignTop,
    AlignBottom = Qt::AlignBottom,
    AlignVCenter = Qt::AlignVCenter,
    AlignVertical_Mask = Qt::AlignVertical_Mask,

    AlignCenter = Qt::AlignCenter,

    AlignOutLeft   = Qt::AlignVCenter  << 2,
    AlignOutRight  = AlignOutLeft  << 2,
    AlignOutTop    = AlignOutRight << 2,
    AlignOutBottom = AlignOutTop   << 2
  };

  static QString toQString( const char*, const int = -1 );
  static QString toQString( const short*, const int = -1 );
  static QString toQString( const unsigned char*, const int = -1 );
  static QString toQString( const unsigned short*, const int = -1 );

  static void    setTabOrder( QWidget*, ... );
  static void    setTabOrder( const QWidgetList& );
  static void    alignWidget( QWidget*, const QWidget*, const int );

//  static void    simplifySeparators( QToolBar* );
  static void    simplifySeparators( QWidget*, const bool = true );

  static bool    isParent( QObject*, QObject* );

  static QString dir( const QString&, const bool = true );
  static QString file( const QString&, const bool = true );
  static QString extension( const QString&, const bool = false );

  static QString library( const QString& );

  static QString tmpDir();
  static bool    mkDir( const QString& );
  static bool    rmDir( const QString& );
  static bool    dos2unix( const QString& );
  static QString addSlash( const QString& );

  static int     rgbSet( const QColor& );
  static int     rgbSet( const int, const int, const int );

  static QColor  rgbSet( const int );
  static void    rgbSet( const int, int&, int&, int& );

  static QColor  scaleColor( const int, const int, const int );
  static void    scaleColors( const int, QList<QColor>& );

  static QImage  grayscale( const QImage& );
  static QPixmap grayscale( const QPixmap& );
  static QImage  transparentImage( const int, const int, const int = -1 );
  static QPixmap transparentPixmap( const int, const int, const int = -1 );
  static QPixmap composite( const QPixmap&, const int, const int, const QPixmap& = QPixmap() );
};

#endif
