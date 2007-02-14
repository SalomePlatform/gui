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
// File:      QtxColorScale.cxx
// Author:    Sergey TELKOV

#include "QtxColorScale.h"

#include <QtCore/qmap.h>
#include <QtCore/qregexp.h>
#include <QtCore/qstringlist.h>

#include <QtGui/qimage.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qbitmap.h>
#include <QtGui/qpainter.h>
#include <QtGui/qtextdocument.h>

#include <math.h>

/*!
  Constructor
*/
QtxColorScale::QtxColorScale( QWidget* parent, Qt::WindowFlags f )
: QFrame( parent, f ),
myMin( 0.0 ),
myMax( 1.0 ),
myTitle( "" ),
myInterval( 10 ),
myFormat( "%.4g" ),
myColorMode( Auto ),
myLabelMode( Auto ),
myLabelPos( Right ),
myTitlePos( Center ),
myDumpMode( NoDump ),
myFlags( AtBorder | WrapTitle )
{
	setWindowTitle( tr ( "Color scale" ) );
}

/*!
  Constructor
*/
QtxColorScale::QtxColorScale( const int num, QWidget* parent, Qt::WindowFlags f )
: QFrame( parent, f ),
myMin( 0.0 ),
myMax( 1.0 ),
myTitle( "" ),
myInterval( num ),
myFormat( "%.4g" ),
myColorMode( Auto ),
myLabelMode( Auto ),
myLabelPos( Right ),
myTitlePos( Center ),
myDumpMode( NoDump ),
myFlags( AtBorder | WrapTitle )
{
	setWindowTitle( tr ( "Color scale" ) );
}

/*!
  Destructor
*/
QtxColorScale::~QtxColorScale()
{
}

/*!
  \returns minimal limit of scale.
*/
double QtxColorScale::minimum() const
{
	return myMin;
}

/*!
  \return maximal limit of scale.
*/
double QtxColorScale::maximum() const
{
	return myMax;
}

/*!
  \return range (minimal and maximal limits) of scale.
*/
void QtxColorScale::range( double& min, double& max ) const
{
	min = myMin;
	max = myMax;
}

/*!
  \return the current title string.
*/
QString QtxColorScale::title() const
{
	return myTitle;
}

/*!
  \returns the current format of number presentation in labels for Auto label mode (sprintf specification).
*/
QString QtxColorScale::format() const
{
	return myFormat;
}

/*!
  \return dump mode.
*/
int QtxColorScale::dumpMode() const
{
	return myDumpMode;
}

/*!
  \return label mode.
*/
int QtxColorScale::labelMode() const
{
	return myLabelMode;
}

/*!
  \return color mode.
*/
int QtxColorScale::colorMode() const
{
	return myColorMode;
}

/*!
  \return intervals number of color scale.
*/
int QtxColorScale::intervalsNumber() const
{
	return myInterval;
}

/*!
  \return the user label of specified interval.
*/
QString QtxColorScale::label( const int idx ) const
{
	QString res;
	if ( idx >= 0 && idx < (int)myLabels.count() )
		res = myLabels[idx];
	return res;
}

/*!
  \return the user color of specified interval.
*/
QColor QtxColorScale::color( const int idx ) const
{
	QColor res;
	if ( idx >= 0 && idx < (int)myColors.count() )
		res = myColors[idx];
	return res;
}

/*!
  \return the user labels.
*/
void QtxColorScale::labels( QStringList& list ) const
{
	list = myLabels;
}

/*!
  \return the user color.
*/
void QtxColorScale::colors( QList<QColor>& list ) const
{
	list = myColors;
}

/*!
  \return the label position.
*/
int QtxColorScale::labelPosition() const
{
	return myLabelPos;
}

/*!
  \return the title position.
*/
int QtxColorScale::titlePosition() const
{
	return myTitlePos;
}

/*!
  Sets the minimum limit.
*/
void QtxColorScale::setMinimum( const double val )
{
	setRange( val, maximum() );
}

/*!
  Sets the maximum limit.
*/
void QtxColorScale::setMaximum( const double val )
{
	setRange( minimum(), val );
}

/*!
  Sets the minimum and maximum limits.
*/
void QtxColorScale::setRange( const double min, const double max )
{
	if ( myMin == min && myMax == max )
		return;

	myMin = min;
	myMax = max;

	myPrecise = QString::null;

	if ( colorMode() == Auto || labelMode() == Auto )
		updateScale();
}

/*!
  Sets the title string.
*/
void QtxColorScale::setTitle( const QString& str )
{
	if ( myTitle == str )
		return;

	myTitle = str;
	updateScale();
}

/*!
  Sets the format of number presentation in labels for
  Auto label mode (sprintf specification).
*/
void QtxColorScale::setFormat( const QString& format )
{
	if ( myFormat == format )
		return;

	myFormat = format;
	myPrecise = QString::null;
	if ( colorMode() == Auto )
		updateScale();
}

/*!
  Sets the number of intervals.
*/
void QtxColorScale::setIntervalsNumber( const int num )
{
	if ( myInterval == num || num < 1 )
		return;

	myInterval = num;
	myPrecise = QString::null;

	updateScale();
}

/*!
  Sets the user label for specified interval. If number
  of interval is negative then user label will be added
  as new at the end of list.
*/
void QtxColorScale::setLabel( const QString& txt, const int idx )
{
	bool changed = false;
	uint i = idx < 0 ? myLabels.count() : idx;
	if ( i < myLabels.count() )
	{
		changed = myLabels[i] != txt;
		myLabels[i] = txt;
	}
	else
	{
		changed = true;
		while ( i >= myLabels.count() )
		    myLabels.append( "" );
		myLabels[i] = txt;
	}
	if ( changed )
		updateScale();
}

/*!
  Sets the user color for specified interval. If number
  of interval is negative then user color will be added
  as new at the end of list.
*/
void QtxColorScale::setColor( const QColor& clr, const int idx )
{
	bool changed = false;
	uint i = idx < 0 ? myColors.count() : idx;
	if ( i < myColors.count() )
	{
		changed = myColors[i] != clr;
		myColors[i] = clr;
	}
	else
	{
		changed = true;
        while ( i >= myColors.count() )
            myColors.append( QColor() );
		myColors[i] = clr;
	}
	if ( changed )
		updateScale();
}

/*!
  Replace the all user label with specified list.
*/
void QtxColorScale::setLabels( const QStringList& list )
{
	if ( list.isEmpty() )
		return;

	myLabels = list;
	updateScale();
}

/*!
  Replace the all user colors with specified list.
*/
void QtxColorScale::setColors( const QList<QColor>& list )
{
	if ( list.isEmpty() )
	    return;

	myColors = list;
	updateScale();
}

/*!
  Sets the color mode (Auto or User).
*/
void QtxColorScale::setColorMode( const int mode )
{
	if ( myColorMode == mode )
		return;

	myColorMode = mode;
	updateScale();
}

/*!
  Sets the dump mode.
*/
void QtxColorScale::setDumpMode( const int mode )
{
	myDumpMode = mode;
}

/*!
  Sets the label mode (Auto or User).
*/
void QtxColorScale::setLabelMode( const int mode )
{
	if ( myLabelMode != mode )
	{
		myLabelMode = mode;
		updateScale();
	}
}

/*!
  Sets the label position.
*/
void QtxColorScale::setLabelPosition( const int pos )
{
	if ( myLabelPos != pos && pos >= None && pos <= Center )
	{
		myLabelPos = pos;
		updateScale();
	}
}

/*!
  Sets the title position.
*/
void QtxColorScale::setTitlePosition( const int pos )
{
	if ( myTitlePos != pos && pos >= None && pos <= Center )
	{
		myTitlePos = pos;
		updateScale();
	}
}

/*!
  Set the specified flags.
*/
void QtxColorScale::setFlags( const int flags )
{
	int prev = myFlags;
	myFlags |= flags;
	if ( prev != myFlags )
		updateScale();
}

/*!
  \return true if specified flags are setted.
*/
bool QtxColorScale::testFlags( const int flags ) const
{
	return ( myFlags & flags ) == flags;
}

/*!
  Clear (reset) the specified flags.
*/
void QtxColorScale::clearFlags( const int flags )
{
	int prev = myFlags;
	myFlags &= ~flags;
	if ( prev != myFlags )
		updateScale();
}

/*!
  \return minimum size hint
*/
QSize QtxColorScale::minimumSizeHint() const
{
  QSize sz = calculateSize( true, myFlags, titlePosition() != None, labelPosition() != None, true );
	return sz + QSize( frameWidth(), frameWidth() );
}

/*!
  \return size hint
*/
QSize QtxColorScale::sizeHint() const
{
  QSize sz = calculateSize( false, myFlags, titlePosition() != None, labelPosition() != None, true );
	return sz + QSize( frameWidth(), frameWidth() );
}

/*!
  Dump color scale into pixmap with current size.
*/
QSize QtxColorScale::calculateSize( const bool min, const int flags, const bool title,
								                    const bool labels, const bool colors ) const
{
	int num = intervalsNumber();

	int spacer = 5;
	int textWidth = 0;
	int textHeight = fontMetrics().height();
	int colorWidth = 20;

	if ( labels && colors )
  {
    QtxColorScale* that = (QtxColorScale*)this;
    QString fmt = that->myFormat;

		for ( int idx = 0; idx < num; idx++ )
			textWidth = qMax( textWidth, fontMetrics().width( getLabel( idx ) ) );

    if ( !min )
      that->myFormat = that->myFormat.replace( QRegExp( "g" ), "f" );

		for ( int index = 0; index < num; index++ )
			textWidth = qMax( textWidth, fontMetrics().width( getLabel( index ) ) );

    that->myFormat = fmt;
  }

	int scaleWidth = 0;
	int scaleHeight = 0;

	int titleWidth = 0;
	int titleHeight = 0;

	if ( flags & AtBorder )
	{
		num++;
		if ( min && title && !myTitle.isEmpty() )
			titleHeight += 10;
	}

	if ( colors )
	{
		scaleWidth = colorWidth + textWidth + ( textWidth ? 3 : 2 ) * spacer;
		if ( min )
			scaleHeight = qMax( 2 * num, 3 * textHeight );
		else
			scaleHeight = (int)( 1.5 * ( num + 1 ) * textHeight );
	}

	if ( title )
	{
		QTextDocument* srt = textDocument( flags );
		if ( srt )
		{
			QPainter p( (QtxColorScale*)this );
			if ( scaleWidth )
				srt->setTextWidth( scaleWidth );

			titleHeight = srt->size().height() + spacer;
			titleWidth = srt->size().width() + 10;

		}
    delete srt;
	}

	int W = qMax( titleWidth, scaleWidth ) + width() - contentsRect().width();
	int H = scaleHeight + titleHeight + height() - contentsRect().height();

	return QSize( W, H );
}

/*!
  Dump color scale into pixmap with current size.
*/
QPixmap QtxColorScale::dump() const
{
	QPixmap aPix;
  
	if ( dumpMode() != NoDump )
	{
		aPix = QPixmap( size() );
		if ( !aPix.isNull() )
		{
			bool scale = ( myDumpMode == ScaleDump || myDumpMode == FullDump );
			bool label = ( myDumpMode == ScaleDump || myDumpMode == FullDump ) &&
						 labelPosition() != None;
			bool title = ( myDumpMode == TitleDump || myDumpMode == FullDump ) &&
						 titlePosition() != None;
      QColor bgc = palette().color( backgroundRole() );
			QPainter p;
			p.begin( &aPix );
			p.fillRect( 0, 0, aPix.width(), aPix.height(), bgc );
			drawScale( &p, bgc, false, 0, 0, aPix.width(), aPix.height(), title, label, scale );
			p.end();
		}
	}

	return aPix;
}

/*!
  Dump color scale into pixmap with specified size.
*/
QPixmap QtxColorScale::dump( const int w, const int h ) const
{
	return dump( palette().color( backgroundRole() ), w, h );
}

/*!
  Dump color scale into pixmap with specified size and background color.
*/
QPixmap QtxColorScale::dump( const QColor& bg, const int w, const int h ) const
{
	QPixmap aPix;
	if ( dumpMode() != NoDump )
	{
		bool scale = ( myDumpMode == ScaleDump || myDumpMode == FullDump );
		bool label = ( myDumpMode == ScaleDump || myDumpMode == FullDump ) &&
					 labelPosition() != None;
		bool title = ( myDumpMode == TitleDump || myDumpMode == FullDump ) &&
					 titlePosition() != None;

		int W = w;
		int H = h;
		if ( W < 0 || H < 0 )
		{
			QSize sz = calculateSize( false, myFlags & ~WrapTitle, title, label, scale );

			if ( W < 0 )
				W = sz.width();
			if ( H < 0 )
				H = sz.height();
		}

		aPix = QPixmap( W, H );
		if ( !aPix.isNull() )
		{
			QPainter p;
			p.begin( &aPix );
			p.fillRect( 0, 0, aPix.width(), aPix.height(), bg );
			drawScale( &p, bg, false, 0, 0, aPix.width(), aPix.height(), title, label, scale );
			p.end();
		}
	}

	return aPix;
}

/*!
  Show the color scale. [Reimplemented]
*/
void QtxColorScale::show()
{
	QFrame::show();
}

/*!
  Hides the color scale. [Reimplemented]
*/
void QtxColorScale::hide()
{
	QFrame::hide();
}

/*!
  Draw color scale contents. [Reimplemented]
*/
void QtxColorScale::drawContents( QPainter* p )
{
	if ( !updatesEnabled() )
		return;

	QRect aDrawRect = contentsRect();

	drawScale( p, false/*testFlags( Transparent )*/, aDrawRect.x(),
			   aDrawRect.y(), aDrawRect.width(), aDrawRect.height(),
			   titlePosition() != None, labelPosition() != None, true );
}

/*!
  Draw color scale contents.
*/
void QtxColorScale::drawScale( QPainter* p, const bool transp, const int X, const int Y,
                               const int W, const int H, const bool title,
                               const bool label, const bool scale ) const
{
	QPixmap cache( W, H );
	QPainter cp( &cache );

	drawScale( &cp, palette().color( backgroundRole() ), transp, 0, 0, W, H, title, label, scale );
	cp.end();

	p->drawPixmap( X, Y, cache );
}

/*!
  Draw color scale contents.
*/
void QtxColorScale::drawScale( QPainter* p, const QColor& bg, const bool transp,
                               const int X, const int Y, const int W, const int H,
                               const bool drawTitle, const bool drawLabel, const bool drawColors ) const
{
	if ( !transp )
		p->fillRect( X, Y, W, H, bg );

	int num = intervalsNumber();

	int labPos = labelPosition();

	int spacer = 5;
	int textWidth = 0;
	int textHeight = p->fontMetrics().height();

	QString aTitle = title();

	int titleWidth = 0;
	int titleHeight = 0;

	if ( qGray( bg.rgb() ) < 128 )
		p->setPen( QColor( 255, 255, 255 ) );
	else
    p->setPen( QColor( 0, 0, 0 ) );

	// Draw title
	if ( drawTitle )
	{
		QTextDocument* srt = textDocument( myFlags );
		if ( srt )
		{
			srt->setTextWidth( W - 10 );
			titleHeight = srt->size().height() + spacer;
			titleWidth = srt->size().width();
      p->save();
      p->translate( X + 5, Y );
			srt->drawContents( p );
      p->restore();
		}
    delete srt;
	}

	bool reverse = testFlags( Reverse );

	QList<QColor>  colors;
	QList<QString> labels;
	for ( int idx = 0; idx < num; idx++ )
	{
		if ( reverse )
		{
			colors.append( getColor( idx ) );
			labels.append( getLabel( idx ) );
		}
		else
		{
			colors.prepend( getColor( idx ) );
			labels.prepend( getLabel( idx ) );
		}
	}

	if ( testFlags( AtBorder ) )
	{
		if ( reverse )
			labels.append( getLabel( num ) );
		else
			labels.prepend( getLabel( num ) );
		if ( drawLabel )
			textWidth = qMax( textWidth, p->fontMetrics().width( labels.last() ) );
	}

	if ( drawLabel )
	{
		const QFontMetrics& fm = p->fontMetrics();
		for ( QStringList::ConstIterator it = labels.begin(); it != labels.end(); ++it )
			textWidth = qMax( textWidth, fm.width( *it) );
	}

	int lab = labels.count();

	double spc = ( H - ( ( qMin( lab, 2 ) + qAbs( lab - num - 1 ) ) * textHeight ) - titleHeight );
	double val = spc != 0 ? 1.0 * ( lab - qMin( lab, 2 ) ) * textHeight / spc : 0;
	double iPart;
	double fPart = modf( val, &iPart );
	int filter = (int)iPart + ( fPart != 0 ? 1 : 0 );
	filter = qMax( filter, 1 );

	double step = 1.0 * ( H - ( lab - num + qAbs( lab - num - 1 ) ) * textHeight - titleHeight ) / num;

	int ascent = p->fontMetrics().ascent();
	int colorWidth = qMax( 5, qMin( 20, W - textWidth - 3 * spacer ) );
	if ( labPos == Center || !drawLabel )
		colorWidth = W - 2 * spacer;

	// Draw colors
	int x = X + spacer;
	switch ( labPos )
	{
	case Left:
	    x += textWidth + ( textWidth ? 1 : 0 ) * spacer;
		break;
	}

	double offset = 1.0 * textHeight / 2 * ( lab - num + qAbs( lab - num - 1 ) ) + titleHeight;
	QList<QColor>::Iterator cit = colors.begin();
  uint ci = 0;
	for ( ci = 0; cit != colors.end() && drawColors; ++cit, ci++ )
	{
		int y = (int)( Y + ci * step + offset );
		int h = (int)( Y + ( ci + 1 ) * step + offset ) - y;
		p->fillRect( x, y, colorWidth, h, *cit );
	}

	if ( drawColors )
		p->drawRect( int( x - 1 ), int( Y + offset - 1 ), int( colorWidth + 2 ), int( ci * step + 2 ) );

	// Draw labels
	offset = 1.0 * qAbs( lab - num - 1 ) * ( step - textHeight ) / 2 +
						 1.0 * qAbs( lab - num - 1 ) * textHeight / 2;
	offset += titleHeight;
	if ( drawLabel && !labels.isEmpty() )
	{
		int i1 = 0;
		int i2 = lab - 1;
		int last1( i1 ), last2( i2 );
		int x = X + spacer;
		switch ( labPos )
		{
		case Center:
			x += ( colorWidth - textWidth ) / 2;
			break;
		case Right:
			x += colorWidth + spacer;
			break;
		}
		while ( i2 - i1 >= filter || ( i2 == 0 && i1 == 0 ) )
		{
			int pos1 = i1;
			int pos2 = lab - 1 - i2;
			if ( filter && !( pos1 % filter ) )
			{
				p->drawText( x, (int)( Y + i1 * step + ascent + offset ), labels[i1] );
				last1 = i1;
			}
			if ( filter && !( pos2 % filter ) )
			{
				p->drawText( x, (int)( Y + i2 * step + ascent + offset ), labels[i2] );
				last2 = i2;
			}
			i1++;
			i2--;
		}
		int pos = i1;
		int i0 = -1;
		while ( pos <= i2 && i0 == -1 )
		{
			if ( filter && !( pos % filter ) &&
				 qAbs( pos - last1 ) >= filter && qAbs( pos - last2 ) >= filter )
				i0 = pos;
			pos++;
		}

		if ( i0 != -1 )
			p->drawText( x, (int)( Y + i0 * step + ascent + offset ), labels[i0] );
	}
}

/*!
  \return the format for number labels.
*/
QString QtxColorScale::getFormat() const
{
	QString aFormat = format();

	if ( !testFlags( PreciseFormat ) || testFlags( Integer ) )
		return aFormat;

	if ( !myPrecise.isEmpty() )
		return myPrecise;

	if ( !aFormat.contains( QRegExp( "^(%[0-9]*.?[0-9]*[fegFEG])$" ) ) )
		return aFormat;

	int pos1 = aFormat.indexOf( '.' );
	int pos2 = aFormat.indexOf( QRegExp( "[fegFEG]") );

	QString aLocFormat;
	int precision = 1;
	if ( pos1 > 0 )
	{
		aLocFormat = aFormat.mid( 0, pos1 + 1 );
		precision = aFormat.mid( pos1 + 1, pos2 - pos1 - 1 ).toInt();
		if ( precision < 1 )
			precision = 1;
	}
	else
		return aFormat;
  
	QtxColorScale* that = (QtxColorScale*)this;

	// calculate format, maximum precision limited
	// to 7 digits after the decimal point.
	while ( myPrecise.isEmpty() && precision < 7 )
	{
		QString aTmpFormat = aLocFormat;
		aTmpFormat += QString( "%1" ).arg( precision );
		aTmpFormat += aFormat.mid( pos2 );

		QMap<QString, int> map;
		bool isHasTwinz = false;

		for ( int idx = 0; idx < intervalsNumber() && !isHasTwinz; idx++ )
		{
			double val = getNumber( idx );
			QString tmpname = QString().sprintf( aTmpFormat.toLatin1(), val );
			isHasTwinz = map.contains( tmpname );
			map.insert( tmpname, 1 );
		}

		if ( !isHasTwinz )
			that->myPrecise = aTmpFormat;
		precision++;
	}

	if ( !myPrecise.isEmpty() )
		aFormat = myPrecise;

	return aFormat;
}

/*!
  \return the number for specified interval.
*/
double QtxColorScale::getNumber( const int idx ) const
{
	double val = 0;
	if ( intervalsNumber() > 0 )
		val = minimum() + idx * ( qAbs( maximum() - minimum() ) / intervalsNumber() );
	return val;
}

/*!
  \return the label for specified interval according to the current label mode.
*/
QString QtxColorScale::getLabel( const int idx ) const
{
	QString res;
	if ( labelMode() == User )
		res = label( idx );
	else
	{
		double val = getNumber( idx );
		res = QString().sprintf( getFormat().toLatin1(), testFlags( Integer ) ? (int)val : val );
	}
	return res;
}

/*!
  \return the color for specified interval according to the current color mode.
*/
QColor QtxColorScale::getColor( const int idx ) const
{
	QColor res;
	if ( colorMode() == User )
		res = color( idx );
	else
    res = Qtx::scaleColor( idx, 0, intervalsNumber() - 1 );
	return res;
}

/*!
  Update color scale if it required.
*/
void QtxColorScale::updateScale()
{
  update();
	updateGeometry();
}

/*!
  \return QSimpleRichText object for title. If title
  not defined (empty string) then return null pointer.
  Object should be deleted by caller function.
*/
QTextDocument* QtxColorScale::textDocument( const int flags ) const
{
	QTextDocument* doc = 0;

	QString aTitle;
	switch ( titlePosition() )
	{
	case Left:
		aTitle = QString( "<p align=\"left\">%1</p>" );
		break;
	case Right:
		aTitle = QString( "<p align=\"right\">%1</p>" );
		break;
	case Center:
		aTitle = QString( "<p align=\"center\">%1</p>" );
		break;
	case None:
	default:
		break;
	}

	if ( !aTitle.isEmpty() && !title().isEmpty() )
	{
    /*
		if ( !myStyleSheet )
		{
			QtxColorScale* that = (QtxColorScale*)this;
			that->myStyleSheet = new QStyleSheet( that );
		}

		if ( myStyleSheet )
		{
			QStyleSheetItem* item = myStyleSheet->item( "p" );
			if ( item )
				item->setWhiteSpaceMode( flags & WrapTitle ? QStyleSheetItem::WhiteSpaceNormal :
														     QStyleSheetItem::WhiteSpaceNoWrap );
		}
*/
		aTitle = aTitle.arg( title() );
		doc = new QTextDocument( aTitle );
	}

	return doc;
}
