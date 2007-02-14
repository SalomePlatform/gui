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
// File:      Qtx.cxx
// Author:    Sergey TELKOV

#include "Qtx.h"

#include <QtCore/qdir.h>
#include <QtCore/qstring.h>
#include <QtCore/qfileinfo.h>

#include <QtGui/qmenu.h>
#include <QtGui/qbitmap.h>
#include <QtGui/qwidget.h>
#include <QtGui/qlayout.h>
#include <QtGui/qpainter.h>
#include <QtGui/qtoolbar.h>
#include <QtGui/qapplication.h>
#include <QtGui/qdesktopwidget.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

QString Qtx::toQString( const char* str, const int len )
{
  return toQString( (unsigned char*)str, len );
}

QString Qtx::toQString( const short* str, const int len )
{
  return toQString( (unsigned short*)str, len );
}

QString Qtx::toQString( const unsigned char* str, const int len )
{
  QString res;
  int l = len;
  const unsigned char* s = str;
  while ( len < 0 || res.length() < len )
  {
    if ( *s == '\0' )
      break;

    res.append( QChar( *s ) );
    s++;
  }
  return res;
}

QString Qtx::toQString( const unsigned short* str, const int len )
{
  QString res;
  int l = len;
  const unsigned short* s = str;
  while ( len < 0 || res.length() < len )
  {
    if ( *s == '\0' )
      break;

    res.append( QChar( *s ) );
    s++;
  }
  return res;
}

/*!
	Name: setTabOrder [static public]
	Desc: Set tab order for specified list of widgets. Last parameter should be null pointer.
*/

void Qtx::setTabOrder( QWidget* first, ... )
{
  va_list wids;
	va_start( wids, first );

	QWidgetList widList;

	QWidget* cur = first;
	while ( cur )
	{
	  widList.append( cur );
		cur = va_arg( wids, QWidget* );
  }

	setTabOrder( widList );
}

/*!
	Name: setTabOrder [static public]
	Desc: Set tab order for specified list of widgets.
*/

void Qtx::setTabOrder( const QWidgetList& widgets )
{
  if ( widgets.count() < 2 )
    return;

  QWidget* prev = 0;
  for ( QWidgetList::const_iterator it = widgets.begin(); it!= widgets.end(); ++it )
  {
    QWidget* next = *it;
    if ( prev && next )
      QWidget::setTabOrder( prev, next );
    prev = next;
  }
}

/*!
	Name: alignWidget [static public]
	Desc: Align widget 'src' relative to widget 'ref' acording to alignment flags.
	      Alignment flags:
			  Qtx::AlignLeft      - Align left side of 'src' to left side of 'ref'.
			  Qtx::AlignRight     - Align right side of 'src' to right side of 'ref'.
			  Qtx::AlignTop       - Align top side of 'src' to top side of 'ref'.
			  Qtx::AlignBottom    - Align bottom side of 'src' to bottom side of 'ref'.
			  Qtx::AlignHCenter   - Align 'src' to center of 'ref' in horizontal dimension.
			  Qtx::AlignVCenter   - Align 'src' to center of 'ref' in vertical dimension.
			  Qtx::AlignCenter    - Align 'src' to center of 'ref' in both dimensions.
			  Qtx::AlignOutLeft   - Align right side of 'src' to left side of 'ref'.
			  Qtx::AlignOutRight  - Align left side of 'src' to right side of 'ref'.
			  Qtx::AlignOutTop    - Align bottom side of 'src' to top side of 'ref'.
			  Qtx::AlignOutBottom - Align top side of 'src' to bottom side of 'ref'.
*/

void Qtx::alignWidget( QWidget* src, const QWidget* ref, const int alignFlags )
{
	if ( !src || !ref || !alignFlags )
		return;

	QPoint srcOri = src->pos();
	QPoint refOri = ref->pos();
	if ( src->parentWidget() && !src->isTopLevel() )
		srcOri = src->parentWidget()->mapToGlobal( srcOri );
	if ( ref->parentWidget() && !ref->isTopLevel() )
		refOri = ref->parentWidget()->mapToGlobal( refOri );

	int x = srcOri.x(), y = srcOri.y();
	int refWidth = ref->frameGeometry().width(), refHei = ref->frameGeometry().height();
	int srcWidth = src->frameGeometry().width(), srcHei = src->frameGeometry().height();

	if ( srcWidth <= 0 )
		srcWidth = src->sizeHint().width();
  if ( srcHei <= 0 )
    srcHei = src->sizeHint().height();

	int border = 0;
  if ( ref->isTopLevel() && ref->isMaximized() &&
       src->isTopLevel() && !src->isMaximized() )
    border = ( src->frameGeometry().width() - src->width() ) / 2;

	if ( alignFlags & Qtx::AlignLeft )
		x = refOri.x() + border;
	if ( alignFlags & Qtx::AlignOutLeft )
		x = refOri.x() - srcWidth - border;
	if ( alignFlags & Qtx::AlignRight )
		x = refOri.x() + refWidth - srcWidth - border;
	if ( alignFlags & Qtx::AlignOutRight )
		x = refOri.x() + refWidth + border;
	if ( alignFlags & Qtx::AlignTop )
		y = refOri.y() + border;
	if ( alignFlags & Qtx::AlignOutTop )
		y = refOri.y() - srcHei - border;
	if ( alignFlags & Qtx::AlignBottom )
		y = refOri.y() + refHei - srcHei - border;
	if ( alignFlags & Qtx::AlignOutBottom )
		y = refOri.y() + refHei + border;
	if ( alignFlags & Qtx::AlignHCenter )
		x = refOri.x() + ( refWidth - srcWidth ) / 2;
	if ( alignFlags & Qtx::AlignVCenter )
		y = refOri.y() + ( refHei - srcHei ) / 2;

	if ( src->parentWidget() && !src->isTopLevel() )
	{
		QPoint pos = src->parentWidget()->mapFromGlobal( QPoint( x, y ) );
		x = pos.x();
		y = pos.y();
	}

	QWidget* desk = QApplication::desktop();
	if ( desk && x + srcWidth + border > desk->width() )
		x = desk->width() - srcWidth - border;
	if ( desk && y + srcHei + border > desk->height() )
		y = desk->height() - srcHei - border;

	x = qMax( x, 0 );
	y = qMax( y, 0 );

	src->move( x, y );
}

/*!
	Name: simplifySeparators [static public]
	Desc: Checks toolbar for unnecessary separators and removes them
*/
/*
void Qtx::simplifySeparators( QToolBar* toolbar )
{
  if ( !toolbar )
    return;

  const QObjectList& objList = toolbar->children();

  QObjectList delList;

  bool isPrevSep = true;
  QObject* lastVis = 0; // last visible
  for ( QObjectList::const_iterator it = objList.begin(); it != objList.end(); ++it )
  {
    QObject* obj = *it;
    if ( !obj || !obj->isWidgetType() )
      continue;
    bool isSep = obj->inherits( "QToolBarSeparator" );
    if ( !isSep && !((QWidget*)obj)->isVisibleTo( toolbar ) )
      continue;
    if ( isPrevSep && isSep )
      delList.append( obj );
    else
    {
      isPrevSep = isSep;
      lastVis = obj;
    }
  }
  // remove last visible separator
  if ( lastVis && lastVis->inherits( "QToolBarSeparator" ) )
      delList.append( lastVis );

  for ( QObjectList::iterator itr = delList.begin(); itr != delList.end(); ++itr )
    delete *itr;
}
*/

/*!
	Name: simplifySeparators [static public]
	Desc: Checks popup menu recursively for unnecessary separators and removes them
*/
void Qtx::simplifySeparators( QWidget* wid, const bool recursive )
{
  if ( !wid )
    return;

  QList<QAction*> items = wid->actions();
  if ( items.isEmpty() )
    return;

  QList<QAction*> toRemove;
  for ( uint i = 1; i < items.count(); i++ )
  {
    if ( items[i]->isSeparator() && items[i - 1]->isSeparator() )
      toRemove.append( items[i] );

    if ( recursive && items[i]->menu() )
      simplifySeparators( items[i]->menu(), recursive );
  }

  for ( QList<QAction*>::iterator it = toRemove.begin(); it != toRemove.end(); ++it )
    wid->removeAction( *it );

  items = wid->actions();
  if ( !items.isEmpty() && items[0]->isSeparator() )
    wid->removeAction( items[0] );

  items = wid->actions();
  if ( !items.isEmpty() && items[items.count() - 1]->isSeparator() )
    wid->removeAction( items[items.count() - 1] );
}

/*!
	Name: isParent [static public]
	Desc: Returns 'true' if specified 'parent' is parent object of given 'child'.
*/
bool Qtx::isParent( QObject* child, QObject* parent )
{
  if ( !child || !parent )
    return false;

  bool res = false;
  QObject* obj = child;
  while ( !res && obj )
  {
    res = obj == parent;
    obj = obj->parent();
  }
  return res;
}

/*!
	Name: dir [static public]
	Desc: Returns dir name or null string.
*/
QString Qtx::dir( const QString& path, const bool abs )
{
  QDir aDir = QFileInfo( path ).dir();
  QString dirPath = abs ? aDir.absolutePath() : aDir.path();
  if ( dirPath == QString( "." ) )
    dirPath = QString::null;
  return dirPath;
}

/*!
	Name: file [static public]
	Desc: Returns file with or without extension.
*/
QString Qtx::file( const QString& path, bool withExt )
{
  if ( withExt )
    return QFileInfo( path ).fileName();
  else
    return QFileInfo( path ).baseName();
}

/*!
	Name: extension [static public]
	Desc: Returns the file extension only or null string.
*/
QString Qtx::extension( const QString& path, const bool full )
{
  return full ? QFileInfo( path ).completeSuffix() : QFileInfo( path ).suffix();
}

/*!
	Name: library [static public]
	Desc: Generate library file name.
        Append required prefix (lib) and suffix (.dll/.so) to the library file name.
*/
QString Qtx::library( const QString& str )
{
  QString path = dir( str, false );
  QString name = file( str, false );
  QString ext  = extension( str );

#ifndef WIN32
  if ( !name.startsWith( "lib" ) )
    name = QString( "lib" ) + name;
#endif

#ifdef WIN32
  QString libExt( "dll" );
#else
  QString libExt( "so" );
#endif

  if ( ext.toLower() != QString( "so" ) && ext.toLower() != QString( "dll" ) )
  {
    if ( !name.isEmpty() && !ext.isEmpty() )
      name += QString( "." );
    name += ext;
  }

  ext = libExt;

  QString fileName = addSlash( path ) + name + QString( "." ) + ext;

  return fileName;
}

/*!
	Name: tmpDir [static public]
	Desc: Returns path to temporary directory.
*/
QString Qtx::tmpDir()
{
	char* tmpdir = ::getenv( "TEMP" );
	if ( !tmpdir )
		tmpdir = ::getenv ( "TMP" );
	if ( !tmpdir )
	{
#ifdef WIN32
		tmpdir = "C:\\";
#else
		tmpdir = "/tmp";
#endif
	}
	return QString( tmpdir );
}

/*!
	Name: mkDir [static public]
	Desc: Creates directory with intermediate perent directories.
		    Returns true in successfull case.
*/
bool Qtx::mkDir( const QString& dirPath )
{
	return QDir().mkpath( dirPath );
}

/*!
	Name: rmDir [static public]
	Desc: Removes directory with its subdirectories and files.
		    Returns true in successfull case.
*/
bool Qtx::rmDir( const QString& thePath )
{
	QFileInfo fi( thePath );
	if ( !fi.exists() )
		return true;

	bool stat = true;
	if ( fi.isFile() )
		stat = QFile::remove( thePath );
	else if ( fi.isDir() )
	{
		QDir aDir( thePath );
		QFileInfoList anEntries = aDir.entryInfoList();
    for ( QFileInfoList::iterator it = anEntries.begin(); it != anEntries.end(); ++it )
		{
      QFileInfo inf = *it;
      if ( inf.fileName() == "." || inf.fileName() == ".." )
					continue;
      stat = stat && rmDir( inf.absoluteFilePath() );
		}
		stat = stat && aDir.rmdir( thePath );
	}
	return stat;
}

/*!
	Name: addSlash [static public]
	Desc: Adds a slash to the end of 'path' if it is not already there.
*/
QString Qtx::addSlash( const QString& path )
{
	QString res = path;
  if ( !res.isEmpty() && res.at( res.length() - 1 ) != QChar( '/' ) &&
	  res.at( res.length() - 1 ) != QChar( '\\' ) )
  res += QDir::separator();
  return res;
}

/*!
	Name: dos2unix [static public]
	Desc: Convert text file. Replace symbols "LF/CR" by symbol "LF".
*/
bool Qtx::dos2unix( const QString& absName )
{
  FILE* src = ::fopen( absName.toLatin1(), "rb" );
  if ( !src )
		return false;

  /* we'll use temporary file */
  char temp[512] = { '\0' };
  QString dir = Qtx::dir( absName );
  FILE* tgt = ::fopen( strcpy( temp, ::tempnam( dir.toLatin1(), "__x" ) ), "wb" );
  if ( !tgt )
		return false;

  /* temp -> result of conversion */
  const char CR = 0x0d;
  const char LF = 0x0a;
  bool waitingLF = false;

  while( true )
  {
    int  outcnt = 0;
    char inbuf[512], outbuf[512];

    /* convert buffer */
    int nbread = ::fread( inbuf, 1, sizeof( inbuf ), src );
    for ( int incnt = 0; incnt < nbread; incnt++  )
    {
      if ( waitingLF )
      {
        waitingLF = false;
        if ( inbuf[incnt] == LF )
          outbuf[outcnt++] = LF;
        else
          outbuf[outcnt++] = CR;
      }
      else if ( inbuf[incnt] == CR )
        waitingLF = true;
      else
        outbuf[outcnt++] = inbuf[incnt];
    }

    /* check last sym in buffer */
    waitingLF = ( inbuf[nbread - 1] == CR );

    /* write converted buffer to temp file */
    int nbwri = ::fwrite( outbuf, 1, outcnt, tgt );
    if ( nbwri != outcnt )
    {
      ::fclose( src );
			::fclose( tgt );
      QFile::remove( QString( temp ) );
      return false;
    }
    if ( nbread != sizeof( inbuf ) )
      break;              /* converted ok */
  }
  ::fclose( src );
	::fclose( tgt );

  /* rename temp -> src */
  if ( !QFile::remove( absName ) )
    return false;

  return QDir().rename( QString( temp ), absName );
}

/*!
	Name: rgbSet [static public]
	Desc: Pack the specified color into one integer RGB set.
*/
int Qtx::rgbSet( const QColor& c )
{
  return rgbSet( c.red(), c.green(), c.blue() );
}

/*!
	Name: rgbSet [static public]
	Desc: Pack the specified color components into one integer RGB set.
*/
int Qtx::rgbSet( const int r, const int g, const int b )
{
  return ( ( ( 0xff & r ) << 16 ) + ( ( 0xff & g ) << 8 ) + ( 0xff & b ) );
}

/*!
	Name: rgbSet [static public]
	Desc: Unpack the specified integer RGB set into the color.
*/
QColor Qtx::rgbSet( const int rgb )
{
  int r, g, b;
  rgbSet( rgb, r, g, b );
  return QColor( r, g, b );
}

/*!
	Name: rgbSet [static public]
	Desc: Unpack the specified integer RGB set into the color components.
*/
void Qtx::rgbSet( const int rgb, int& r, int& g, int& b )
{
  r = ( rgb >> 16 ) & 0xff;
  g = ( rgb >> 8 ) & 0xff;
  b = rgb & 0xff;
}

/*!
	Name: scaleColor [static public]
	Desc: Returns the color specified by the index between min (blue) and max (red).
*/
QColor Qtx::scaleColor( const int index, const int min, const int max )
{
  static const int HUE[10] = {230, 210, 195, 180, 160, 80, 60, 50, 30, 0};

  int hue = HUE[0];

	if ( min != max )
  {
    double aPosition = 9.0 * ( index - min ) / ( max - min );
    if ( aPosition > 0.0 )
    {
      if ( aPosition >= 9.0 )
        hue = HUE[9];
      else
      {
        int idx = (int)aPosition;
        hue = HUE[idx] + int( ( aPosition - idx ) * ( HUE[idx + 1] - HUE[idx] ) );
      }
    }
  }

  return QColor::fromHsv( hue, 255, 255 );
}

/*!
	Name: scaleColors [static public]
	Desc: Returns the 'num' number of colors from blue to red.
*/
void Qtx::scaleColors( const int num, QList<QColor>& lst )
{
  lst.clear();
  for ( int i = 0; i < num; i++ )
    lst.append( scaleColor( i, 0, num - 1 ) );
}

/*!
	Name: grayscale [static public]
	Desc: Convert color image to grayscale image.
*/
QImage Qtx::grayscale( const QImage& img )
{
  QImage res = img;

  int colNum = res.numColors();
  if ( colNum )
  {
    for ( int i = 0; i < colNum; i++ )
      res.setColor( i, qGray( res.color( i ) ) );
  }
  else
  {
    for ( int y = 0; y < res.height(); y++ )
    {
      for ( int x = 0; x < res.width(); x++ )
      {
        QRgb pix = res.pixel( x, y );
        res.setPixel( x, y, qRgba( qGray( pix ), qGray( pix ), qGray( pix ), qAlpha( pix ) ) );
      }
    }
  }

  return res;
}

/*!
	Name: grayscale [static public]
	Desc: Convert color pixmap to grayscale pixmap.
*/
QPixmap Qtx::grayscale( const QPixmap& pix )
{
  QPixmap res;
  res.fromImage( grayscale( pix.toImage() ) );
  return res;
}

/*!
	Name: transparentImage [static public]
	Desc: Create transparent image with specified width \aw, height \ah and color depth \ad.
*/
QImage Qtx::transparentImage( const int w, const int h, const int d )
{
  QImage::Format fmt;
  switch ( d )
  {
  case 1:
    fmt = QImage::Format_Mono;
    break;
  case 8:
    fmt = QImage::Format_Indexed8;
    break;
  case 16:
  case 24:
  case 32:
  default:
    fmt = QImage::Format_ARGB32;
    break;
  }

  QImage img( w, h, fmt );
  if ( !img.isNull() )
  {
//    img.setAlphaBuffer( true );
    for ( int i = 0; i < img.height(); i++ )
      for ( int j = 0; j < img.width(); j++ )
        img.setPixel( j, i, qRgba( 0, 0, 0, 0 ) );
  }
  return img;
}

/*!
	Name: transparentPixmap [static public]
	Desc: Create transparent pixmap with specified width \aw, height \ah and color depth \ad.
*/
QPixmap Qtx::transparentPixmap( const int w, const int h, const int d )
{
  QPixmap pix;
  QImage img = transparentImage( w, h, d );
  if ( !img.isNull() )
    pix.fromImage( img );
  return pix;
}

/*!
	Name: composite [static public]
	Desc: Create composite pixmap. Pixmap 'pix' draws over pixmap 'dest' with coordinates
        specified relative upper left corner of 'dest'. If 'dest' not given then new empty
        pixmap with appropriate size created.
*/
QPixmap Qtx::composite( const QPixmap& pix, const int x, const int y, const QPixmap& dest )
{
  if ( pix.isNull() )
    return dest;

  int width = qMax( pix.width() + x, dest.width() );
  int height = qMax( pix.height() + y, dest.height() );

  QPixmap res( width, height );
  QImage img = transparentImage( width, height, 32 );

  QPainter p;
  p.begin( &res );
  p.fillRect( 0, 0, width, height, QBrush( Qt::white ) );

  if ( !dest.isNull() )
  {
    p.drawPixmap( 0, 0, dest );
    QImage temp = dest.toImage();
    for ( int i = 0; i < temp.width() && i < img.width(); i++ )
    {
      for ( int j = 0; j < temp.height() && j < img.height(); j++ )
      {
        if ( temp.hasAlphaChannel() )
          img.setPixel( i, j, temp.pixel( i, j ) );
        else
        {
          QRgb p = temp.pixel( i, j );
          img.setPixel( i, j, qRgba( qRed( p ), qGreen( p ), qBlue( p ), 255 ) );
        }
      }
    }
  }

  p.drawPixmap( x, y, pix );
  QImage temp = pix.toImage();
  for ( int c = x; c < temp.width() + x && c < img.width(); c++ )
  {
    for ( int r = y; r < temp.height() + y && r < img.height(); r++ )
    {
      if ( qAlpha( temp.pixel( c - x, r - y ) ) > 0 )
        img.setPixel( c, r, temp.pixel( c - x, r - y ) );
    }
  }

  p.end();

  for ( int ai = 0; ai < img.width(); ai++ )
  {
    for ( int aj = 0; aj < img.height(); aj++ )
    {
      if ( qAlpha( img.pixel( ai, aj ) ) < 1 )
        img.setPixel( ai, aj, qRgba( 255, 255, 255, 255 ) );
      else
        img.setPixel( ai, aj, qRgba( 0, 0, 0, 0 ) );
    }
  }

  QBitmap bmp( width, height );
  bmp.fromImage( img, Qt::ColorMode_Mask | Qt::ThresholdDither );
  res.setMask( bmp );

  return res;
}
