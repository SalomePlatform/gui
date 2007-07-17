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

#if defined WIN32
#  if defined QTX_EXPORTS
#    define QTX_EXPORT _declspec( dllexport )
#  else
#    define QTX_EXPORT _declspec( dllimport )
#  endif
#else
#  define QTX_EXPORT  
#endif

#if defined SOLARIS
#define bool  int
#define false 0
#define true  1
#endif

#include <QString>
#include <QList>
#include <QColor>
#include <QImage>
#include <QPixmap>

class QObject;
class QWidget;
class QCompleter;

typedef QList<int>    QIntList;       //!< list of int values
typedef QList<short>  QShortList;     //!< list of short int values
typedef QList<double> QDoubleList;    //!< list of double values
typedef QList<QColor> QColorList;     //!< list of colors

class QTX_EXPORT Qtx
{
public:
  //! Widget alignment flags
  typedef enum
  {
    AlignLeft            = Qt::AlignLeft,            //!< align left side of one widget to the left side of another widget
    AlignLeading         = Qt::AlignLeading,         //!< synonim for AlignLeft
    AlignRight           = Qt::AlignRight,           //!< align right side of one widget to the right side of another widget
    AlignTrailing        = Qt::AlignTrailing,        //!< synonim for AlignRight
    AlignHCenter         = Qt::AlignHCenter,         //!< align one widget to the center of another widget in horizontal dimension
    AlignJustify         = Qt::AlignJustify,         //!< synonym of Qt::AlignJustify
    AlignAbsolute        = Qt::AlignAbsolute,        //!< synonym of Qt::AlignAbsolute
    AlignHorizontal_Mask = Qt::AlignHorizontal_Mask, //!< synonym of Qt::AlignHorizontal_Mask

    AlignTop             = Qt::AlignTop,             //!< align top side of one widget to the top side of another widget
    AlignBottom          = Qt::AlignBottom,          //!< align bottom side of one widget to the bottom side of another widget
    AlignVCenter         = Qt::AlignVCenter,         //!< align one widget to the center of another widget in vertical dimension
    AlignVertical_Mask   = Qt::AlignVertical_Mask,   //!< synonym of Qt::AlignVertical_Mask

    AlignCenter          = Qt::AlignCenter,          //!< align one widget to the center of another widget in both dimensions

    AlignOutLeft         = Qt::AlignVCenter  << 2,   //!< align right side of one widget to the left side of another widget
    AlignOutRight        = AlignOutLeft      << 2,   //!< align left side of one widget to the right side of another widget
    AlignOutTop          = AlignOutRight     << 2,   //!< align bottom side of one widget to the top side of another widget
    AlignOutBottom       = AlignOutTop       << 2    //!< align top side of one widget to the bottom side of another widget
  } AlignmentFlags;

  //! Path type, indicates required directory/file operation
  typedef enum { 
    PT_OpenFile,      //!< the file is opened
    PT_SaveFile,      //!< the file is saved
    PT_Directory      //!< the directory path is required
  } PathType;

  //! Custom data roles
  enum { 
    AppropriateRole = Qt::UserRole + 100   //!< can be used to return \c true if data is appropriate
  };

  static QString     toQString( const char*, const int = -1 );
  static QString     toQString( const short*, const int = -1 );
  static QString     toQString( const unsigned char*, const int = -1 );
  static QString     toQString( const unsigned short*, const int = -1 );

  static void        setTabOrder( QWidget*, ... );
  static void        setTabOrder( const QWidgetList& );
  static void        alignWidget( QWidget*, const QWidget*, const int );

  static void        simplifySeparators( QWidget*, const bool = true );

  static bool        isParent( QObject*, QObject* );

  static QString     dir( const QString&, const bool = true );
  static QString     file( const QString&, const bool = true );
  static QString     extension( const QString&, const bool = false );

  static QString     library( const QString& );

  static QString     tmpDir();
  static bool        mkDir( const QString& );
  static bool        rmDir( const QString& );
  static bool        dos2unix( const QString& );
  static QString     addSlash( const QString& );

  static QCompleter* pathCompleter( const PathType, const QString& = QString() );

  static int         rgbSet( const QColor& );
  static int         rgbSet( const int, const int, const int );

  static QColor      rgbSet( const int );
  static void        rgbSet( const int, int&, int&, int& );

  static QColor      scaleColor( const int, const int, const int );
  static void        scaleColors( const int, QColorList& );

  static QImage      grayscale( const QImage& );
  static QPixmap     grayscale( const QPixmap& );
  static QImage      transparentImage( const int, const int, const int = -1 );
  static QPixmap     transparentPixmap( const int, const int, const int = -1 );
  static QPixmap     composite( const QPixmap&, const int, const int, const QPixmap& = QPixmap() );
};

#endif
