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

#include <QDir>
#include <QMenu>
#include <QRegExp>
#include <QBitmap>
#include <QWidget>
#include <QLayout>
#include <QPainter>
#include <QDirModel>
#include <QFileInfo>
#include <QCompleter>
#include <QApplication>
#include <QDesktopWidget>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/*!
  \class Qtx
  \brief Set of helpful utility functions.

  The class implements set of static functions which can be used for different purpuses:
  - define tab order for set of widgets: setTabOrder()
  - align one widget to the other widget: alignWidget()
  - remove extra separators from menu or toolbar: simplifySeparators()
  - retrieve directory, file name and extension parts of the path: dir(), file(), extension()
  - get temporary directory name: tmpDir()
  - create and remove directory (recursively): mkDir(), rmDir()
  - convert text file from DOS to UNIX native format: dos2unix()
  - convert picture to the gray scale: grayscale()
  - and other
*/

/*!
  \brief Convert character array (ASCII string) to the QString.
  \param str character array
  \param len array length, if < 0, the array should be zero-terminated
  \return QString object
 */
QString Qtx::toQString( const char* str, const int len )
{
  return toQString( (unsigned char*)str, len );
}

/*!
  \brief Convert integer array (UNICODE string) to the QString.
  \param str integer array
  \param len array length, if < 0, the array should be zero-terminated
  \return QString object
 */
QString Qtx::toQString( const short* str, const int len )
{
  return toQString( (unsigned short*)str, len );
}

/*!
  \brief Convert character array (ASCII string) to the QString.
  \param str character array
  \param len array length, if < 0, the array should be zero-terminated
  \return QString object
 */
QString Qtx::toQString( const unsigned char* str, const int len )
{
  QString res;
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

/*!
  \brief Convert integer array (UNICODE string) to the QString.
  \param str integer array
  \param len array length, if < 0, the array should be zero-terminated
  \return QString object
 */
QString Qtx::toQString( const unsigned short* str, const int len )
{
  QString res;
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
  \brief Set tab order for specified list of widgets.

  The function has arbitrary number of parameters, each should be
  hovewer of QWidget* type. Last parameter should be null pointer.

  \param first first widget in the sequence
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
  \brief Set tab order for specified list of widgets.
  \param widgets list of widgets
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
  \brief Align widget \a src relative to widget \a ref acording to the 
         alignment flags \a alignFlags.
  \param src source widget (being aligned)
  \param ref reference widget (source widget being aligned to)
  \param alignFlags alignment flags (Qtx::AlignmentFlags)
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
  \brief Remove (recursively) unnecessary separators from the menu or toolbar.
  \param wid widget, should be of QMenu* or QToolBar* class
*/
void Qtx::simplifySeparators( QWidget* wid, const bool recursive )
{
  if ( !wid )
    return;

  QList<QAction*> items = wid->actions();
  if ( items.isEmpty() )
    return;

  QList<QAction*> toRemove;
  for ( int i = 1; i < items.count(); i++ )
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
  \brief Return \c true if specified \a parent is a parent object
         of given \a child (in terms of QObject).

  This function works recursively. It means that \a true is also
  returned if \a parent is a grand-father, grand-grand-father, etc
  of \a child. If the same object is given as both \a parent and
  \a child, \c true is also returned.

  \param child child object
  \param parent parent object
  \return \c true if the \a parent is a parent of \a child
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
  \brief Return directory part of the file path.

  If the file path does not include directory part (the file is in the
  current directory), null string is returned.

  \param path file path
  \param abs if true (default) \a path parameter is treated as absolute file path
  \return directory part of the file path
*/
QString Qtx::dir( const QString& path, const bool abs )
{
  QDir aDir = QFileInfo( path ).dir();
  QString dirPath = abs ? aDir.absolutePath() : aDir.path();
  if ( dirPath == QString( "." ) )
    dirPath = QString();
  return dirPath;
}

/*!
  \brief Return file name part of the file path.

  \param path file path
  \param withExt if true (default) complete file name (with all
         extension except the last) is returned, otherwise only base name
         is returned
  \return file name part of the file path
*/
QString Qtx::file( const QString& path, bool withExt )
{
  QString fPath = path;
  while ( !fPath.isEmpty() && ( fPath[fPath.length() - 1] == '\\' || fPath[fPath.length() - 1] == '/' ) )
    fPath.remove( fPath.length() - 1, 1 );

  if ( withExt )
    return QFileInfo( fPath ).fileName();
  else
    return QFileInfo( fPath ).baseName();
}

/*!
  \brief Return extension part of the file path.

  \param path file path
  \param full if true complete extension (all extensions, dot separated)
         is returned, otherwise (default) only last extension is returned
  \return extension part of the file path
*/
QString Qtx::extension( const QString& path, const bool full )
{
  return full ? QFileInfo( path ).completeSuffix() : QFileInfo( path ).suffix();
}

/*!
  \brief Convert the given parameter to the platform-specific library name.

  The function appends platform-specific prefix (lib) and suffix (.dll/.so)
  to the library file name.
  For example, if \a str = "mylib", "libmylib.so" is returned for Linux and
  mylib.dll for Windows.

  \param str short library name
  \return full library name
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
  \brief Get the temporary directory name.
  \return temporary directory (platform specific)
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
  \brief Create directory recursively including all intermediate sub directories.
  \return \c true if the directory is successfully created and \c false otherwise
*/
bool Qtx::mkDir( const QString& dirPath )
{
  return QDir().mkpath( dirPath );
}

/*!
  \brief Remove directory recursively including all subdirectories and files.
  \return \c true if the directory is successfully removed and \c false otherwise
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
  \brief Add a slash (platform-specific) to the end of \a path
         if it is not already there.
  \param path directory path
  \return modified path (with slash added to the end)
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
  \brief Convert text file from DOS format to UNIX.

  The function replaces "LF/CR" symbols sequence by "LF" symbol.

  \param absName file name
  \return \c true if the file is converted successfully and \c false in
         case of any error
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

QCompleter* Qtx::pathCompleter( const PathType type, const QString& filter )
{
  QStringList extList;
  QStringList filterList = filter.split( ";;" );
  for ( QStringList::const_iterator it = filterList.begin(); it != filterList.end(); ++it )
  {
    QRegExp rx( "[\\s\\w,;]*\\(?\\*\\.([\\w]+)\\)?[\\d\\s\\w]*" );
    int index = 0;
    while ( ( index = rx.indexIn( *it, index ) ) != -1 )
    {
      extList.append( QString( "*.%1" ).arg( rx.cap( 1 ) ) );
      index += rx.matchedLength();
    }
  }

  QDir::Filters filters = 0;
  switch ( type )
  {
  case PT_OpenFile:
  case PT_SaveFile:
    filters = QDir::AllEntries | QDir::AllDirs | QDir::NoDotAndDotDot;
    break;
  case PT_Directory:
    filters = QDir::Drives | QDir::Dirs | QDir::NoDotAndDotDot;
    break;
  }

  QDirModel* dm = new QDirModel( extList, filters, QDir::Unsorted );
  QCompleter* cmp = new QCompleter( dm, 0 );
  dm->setParent( cmp );

  return cmp;
}

/*!
  \brief Pack the specified color into integer RGB set.
  \param c unpacked color
  \return packed color
*/
int Qtx::rgbSet( const QColor& c )
{
  return rgbSet( c.red(), c.green(), c.blue() );
}

/*!
  \brief Pack the specified RGB color components into integer RGB set.
  \param r red component
  \param g green component
  \param b blue component
  \return packed color
*/
int Qtx::rgbSet( const int r, const int g, const int b )
{
  return ( ( ( 0xff & r ) << 16 ) + ( ( 0xff & g ) << 8 ) + ( 0xff & b ) );
}

/*!
  \brief Unpack the specified integer RGB set to the color.
  \param rgb packed color
  \return unpacked color (QColor)
*/
QColor Qtx::rgbSet( const int rgb )
{
  int r, g, b;
  rgbSet( rgb, r, g, b );
  return QColor( r, g, b );
}

/*!
  \brief Unpack the specified integer RGB set to the three RGB components.
  \param rgb packed color
  \param r returned unpacked red component
  \param g returned unpacked green component
  \param b returned unpacked blue component
*/
void Qtx::rgbSet( const int rgb, int& r, int& g, int& b )
{
  r = ( rgb >> 16 ) & 0xff;
  g = ( rgb >> 8 ) & 0xff;
  b = rgb & 0xff;
}

/*!
  \brief Return the color specified by the index between min (blue) and max (red).
  \param index color index
  \param min required minimum hue value
  \param max required maximum hue value
  \return resulting color
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
  \brief Generate required number of colors aligned from blue to red.
  \param num required number of colors
  \param lst returned set of colors
*/
void Qtx::scaleColors( const int num, QColorList& lst )
{
  lst.clear();
  for ( int i = 0; i < num; i++ )
    lst.append( scaleColor( i, 0, num - 1 ) );
}

/*!
  \brief Convert given image to the grayscale format.
  \param img initial image
  \return converted to the grayscale image
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
  \brief Convert given pixmap to the grayscale format.
  \param pix initial pixmap
  \return converted to the grayscale pixmap
*/
QPixmap Qtx::grayscale( const QPixmap& pix )
{
  QPixmap res;
  res.fromImage( grayscale( pix.toImage() ) );
  return res;
}

/*!
  \brief Create transparent image.
  \param w required image width
  \param h required image height
  \param d required image depth
  \return generated image
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
  \brief Create transparent pixmap.
  \param w required image width
  \param h required pixmap height
  \param d required pixmap depth
  \return generated pixmap
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
  \brief Create composite pixmap. 

  Pixmap \a pix is drawn over pixmap \a dest with coordinates
  specified relatively to the upper left corner of \a dest.
  If \a dest is not given, the new empty pixmap with appropriate size created instead.

  \param pix source pixmap
  \param x horizontal shift
  \param y vertical shift
  \param dest background pixmap
  \return resulting pixmap
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
