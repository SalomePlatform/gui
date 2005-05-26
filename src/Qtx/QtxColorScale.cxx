// File:      QtxColorScale.cxx
// Author:    Sergey TELKOV

#include "QtxColorScale.h"

#include <qmap.h>
#include <qimage.h>
#include <qregexp.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qmainwindow.h>
#include <qstringlist.h>
#include <qstylesheet.h>
#include <qsimplerichtext.h>

#include <math.h>

/*********************************************************************
**  Class:  QtxColorScale
**  Descr:  Color Scale widget.
**  Level:  Public
*********************************************************************/

QtxColorScale::QtxColorScale( QWidget* parent, const char* name, WFlags f )
: QFrame( parent, name, f | WResizeNoErase | WRepaintNoErase ),
myDock( 0 ),
myMin( 0.0 ),
myMax( 1.0 ),
myTitle( "" ),
myInterval( 10 ),
myStyleSheet( 0 ),
myFormat( "%.4g" ),
myColorMode( Auto ),
myLabelMode( Auto ),
myLabelPos( Right ),
myTitlePos( Center ),
myDumpMode( NoDump ),
myFlags( AtBorder | WrapTitle )
{
	setCaption( tr ( "Color scale" ) );
}

QtxColorScale::QtxColorScale( const int num, QWidget* parent, const char* name, WFlags f )
: QFrame( parent, name, f | WResizeNoErase | WRepaintNoErase ),
myDock( 0 ),
myMin( 0.0 ),
myMax( 1.0 ),
myTitle( "" ),
myInterval( num ),
myStyleSheet( 0 ),
myFormat( "%.4g" ),
myColorMode( Auto ),
myLabelMode( Auto ),
myLabelPos( Right ),
myTitlePos( Center ),
myDumpMode( NoDump ),
myFlags( AtBorder | WrapTitle )
{
	setCaption( tr ( "Color scale" ) );
}

#if QT_VER == 3

QtxColorScale::QtxColorScale( Dock* dock, const char* name, WFlags f )
: QFrame( dock, name, f | WResizeNoErase | WRepaintNoErase ),
myMin( 0.0 ),
myMax( 1.0 ),
myTitle( "" ),
myDock( dock ),
myInterval( 10 ),
myStyleSheet( 0 ),
myFormat( "%.4g" ),
myColorMode( Auto ),
myLabelMode( Auto ),
myLabelPos( Right ),
myTitlePos( Center ),
myDumpMode( NoDump ),
myFlags( AtBorder | WrapTitle )
{
	setCaption( tr ( "Color scale" ) );
}

#endif

QtxColorScale::~QtxColorScale()
{
}

//================================================================
// Function : minimum
// Purpose  : Returns minimal limit of scale.
//================================================================

double QtxColorScale::minimum() const
{
	return myMin;
}

//================================================================
// Function : maximum
// Purpose  : Returns maximal limit of scale.
//================================================================

double QtxColorScale::maximum() const
{
	return myMax;
}

//================================================================
// Function : range
// Purpose  : Returns range (minimal and maximal limits) of scale.
//================================================================

void QtxColorScale::range( double& min, double& max ) const
{
	min = myMin;
	max = myMax;
}

//================================================================
// Function : title
// Purpose  : Returns the current title string.
//================================================================

QString QtxColorScale::title() const
{
	return myTitle;
}

//================================================================
// Function : format
// Purpose  : Returns the current format of number presentation in
//            labels for Auto label mode (sprintf specification).
//================================================================

QString QtxColorScale::format() const
{
	return myFormat;
}

//================================================================
// Function : dumpMode
// Purpose  : Returns dump mode.
//================================================================

int QtxColorScale::dumpMode() const
{
	return myDumpMode;
}

//================================================================
// Function : labelMode
// Purpose  : Returns label mode.
//================================================================

int QtxColorScale::labelMode() const
{
	return myLabelMode;
}

//================================================================
// Function : colorMode
// Purpose  : Returns color mode.
//================================================================

int QtxColorScale::colorMode() const
{
	return myColorMode;
}

//================================================================
// Function : intervalsNumber
// Purpose  : Returns intervals number of color scale.
//================================================================

int QtxColorScale::intervalsNumber() const
{
	return myInterval;
}

//================================================================
// Function : label
// Purpose  : Returns the user label of specified interval.
//================================================================

QString QtxColorScale::label( const int idx ) const
{
	QString res;
	if ( idx >= 0 && idx < (int)myLabels.count() )
		res = *myLabels.at( idx );
	return res;
}

//================================================================
// Function : color
// Purpose  : Returns the user color of specified interval.
//================================================================

QColor QtxColorScale::color( const int idx ) const
{
	QColor res;
	if ( idx >= 0 && idx < (int)myColors.count() )
		res = *myColors.at( idx );
	return res;
}

//================================================================
// Function : labels
// Purpose  : Returns the user labels.
//================================================================

void QtxColorScale::labels( QStringList& list ) const
{
	list = myLabels;
}

//================================================================
// Function : colors
// Purpose  : Returns the user color.
//================================================================

void QtxColorScale::colors( QValueList<QColor>& list ) const
{
	list = myColors;
}

//================================================================
// Function : labelPosition
// Purpose  : Returns the label position.
//================================================================

int QtxColorScale::labelPosition() const
{
	return myLabelPos;
}

//================================================================
// Function : titlePosition
// Purpose  : Returns the title position.
//================================================================

int QtxColorScale::titlePosition() const
{
	return myTitlePos;
}

//================================================================
// Function : setMinimum
// Purpose  : Sets the minimum limit.
//================================================================

void QtxColorScale::setMinimum( const double val )
{
	setRange( val, maximum() );
}

//================================================================
// Function : setMaximum
// Purpose  : Sets the maximum limit.
//================================================================

void QtxColorScale::setMaximum( const double val )
{
	setRange( minimum(), val );
}

//================================================================
// Function : setRange
// Purpose  : Sets the minimum and maximum limits.
//================================================================

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

//================================================================
// Function : setTitle
// Purpose  : Sets the title string.
//================================================================

void QtxColorScale::setTitle( const QString& str )
{
	if ( myTitle == str )
		return;

	myTitle = str;
	updateScale();
}

//================================================================
// Function : setFormat
// Purpose  : Sets the format of number presentation in labels for
//            Auto label mode (sprintf specification).
//================================================================

void QtxColorScale::setFormat( const QString& format )
{
	if ( myFormat == format )
		return;

	myFormat = format;
	myPrecise = QString::null;
	if ( colorMode() == Auto )
		updateScale();
}

//================================================================
// Function : setIntervalsNumber
// Purpose  : Sets the number of intervals.
//================================================================

void QtxColorScale::setIntervalsNumber( const int num )
{
	if ( myInterval == num || num < 1 )
		return;

	myInterval = num;
	myPrecise = QString::null;

	updateScale();
}

//================================================================
// Function : setLabel
// Purpose  : Sets the user label for specified interval. If number
//            of interval is negative then user label will be added
//            as new at the end of list.
//================================================================

void QtxColorScale::setLabel( const QString& txt, const int idx )
{
	bool changed = false;
	uint i = idx < 0 ? myLabels.count() : idx;
	if ( i < myLabels.count() )
	{
		changed = *myLabels.at( i ) != txt;
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

//================================================================
// Function : setColor
// Purpose  : Sets the user color for specified interval. If number
//            of interval is negative then user color will be added
//            as new at the end of list.
//================================================================

void QtxColorScale::setColor( const QColor& clr, const int idx )
{
	bool changed = false;
	uint i = idx < 0 ? myColors.count() : idx;
	if ( i < myColors.count() )
	{
		changed = *myColors.at( i ) != clr;
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

//================================================================
// Function : setLabels
// Purpose  : Replace the all user label with specified list.
//================================================================

void QtxColorScale::setLabels( const QStringList& list )
{
	if ( list.isEmpty() )
		return;

	myLabels = list;
	updateScale();
}

//================================================================
// Function : setColors
// Purpose  : Replace the all user colors with specified list.
//================================================================

void QtxColorScale::setColors( const QValueList<QColor>& list )
{
	if ( list.isEmpty() )
	    return;

	myColors = list;
	updateScale();
}

//================================================================
// Function : setColorMode
// Purpose  : Sets the color mode (Auto or User).
//================================================================

void QtxColorScale::setColorMode( const int mode )
{
	if ( myColorMode == mode )
		return;

	myColorMode = mode;
	updateScale();
}

//================================================================
// Function : setDumpMode
// Purpose  : Sets the dump mode.
//================================================================

void QtxColorScale::setDumpMode( const int mode )
{
	myDumpMode = mode;
}

//================================================================
// Function : setLabelMode
// Purpose  : Sets the label mode (Auto or User).
//================================================================

void QtxColorScale::setLabelMode( const int mode )
{
	if ( myLabelMode != mode )
	{
		myLabelMode = mode;
		updateScale();
	}
}

//================================================================
// Function : setLabelPosition
// Purpose  : Sets the label position.
//================================================================

void QtxColorScale::setLabelPosition( const int pos )
{
	if ( myLabelPos != pos && pos >= None && pos <= Center )
	{
		myLabelPos = pos;
		updateScale();
	}
}

//================================================================
// Function : setTitlePosition
// Purpose  : Sets the title position.
//================================================================

void QtxColorScale::setTitlePosition( const int pos )
{
	if ( myTitlePos != pos && pos >= None && pos <= Center )
	{
		myTitlePos = pos;
		updateScale();
	}
}

//================================================================
// Function : setFlags
// Purpose  : Set the specified flags.
//================================================================

void QtxColorScale::setFlags( const int flags )
{
	int prev = myFlags;
	myFlags |= flags;
	if ( prev != myFlags )
		updateScale();
}

//================================================================
// Function : testFlags
// Purpose  : Returns true if specified flags are setted.
//================================================================

bool QtxColorScale::testFlags( const int flags ) const
{
	return ( myFlags & flags ) == flags;
}

//================================================================
// Function : clearFlags
// Purpose  : Clear (reset) the specified flags.
//================================================================

void QtxColorScale::clearFlags( const int flags )
{
	int prev = myFlags;
	myFlags &= ~flags;
	if ( prev != myFlags )
		updateScale();
}

//================================================================
// Function : minimumSizeHint
// Purpose  : 
//================================================================

QSize QtxColorScale::minimumSizeHint() const
{
  QSize sz = calculateSize( true, myFlags, titlePosition() != None, labelPosition() != None, true );
	return sz + QSize( frameWidth(), frameWidth() );
}

//================================================================
// Function : sizeHint
// Purpose  : 
//================================================================

QSize QtxColorScale::sizeHint() const
{
  QSize sz = calculateSize( false, myFlags, titlePosition() != None, labelPosition() != None, true );
	return sz + QSize( frameWidth(), frameWidth() );
}

//================================================================
// Function : calculateSize
// Purpose  : Dump color scale into pixmap with current size.
//================================================================

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
			textWidth = QMAX( textWidth, fontMetrics().width( getLabel( idx ) ) );

    if ( !min )
      that->myFormat = that->myFormat.replace( QRegExp( "g" ), "f" );

		for ( int index = 0; index < num; index++ )
			textWidth = QMAX( textWidth, fontMetrics().width( getLabel( index ) ) );

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
			scaleHeight = QMAX( 2 * num, 3 * textHeight );
		else
			scaleHeight = (int)( 1.5 * ( num + 1 ) * textHeight );
	}

	if ( title )
	{
		QSimpleRichText* srt = simpleRichText( flags );
		if ( srt )
		{
			QPainter p( this );
			if ( scaleWidth )
				srt->setWidth( &p, scaleWidth );

			titleHeight = srt->height() + spacer;
			titleWidth = srt->widthUsed() + 10;

			delete srt;
		}
	}

	int W = QMAX( titleWidth, scaleWidth ) + width() - contentsRect().width();
	int H = scaleHeight + titleHeight + height() - contentsRect().height();

	return QSize( W, H );
}

//================================================================
// Function : dump
// Purpose  : Dump color scale into pixmap with current size.
//================================================================

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

#if QT_VER < 3
            QColor bgc = backgroundColor();
#else
            QColor bgc = paletteBackgroundColor();
#endif
			QPainter p;
			p.begin( &aPix );
			p.fillRect( 0, 0, aPix.width(), aPix.height(), bgc );
			drawScale( &p, bgc, false, 0, 0, aPix.width(), aPix.height(), title, label, scale );
			p.end();
		}
	}

	return aPix;
}

//================================================================
// Function : dump
// Purpose  : Dump color scale into pixmap with specified size.
//================================================================

QPixmap QtxColorScale::dump( const int w, const int h ) const
{
#if QT_VER < 3
	return dump( backgroundColor(), w, h );
#else
	return dump( paletteBackgroundColor(), w, h );
#endif
}

//================================================================
// Function : dump
// Purpose  : Dump color scale into pixmap with specified size
//            and background color.
//================================================================

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

//================================================================
// Function : show
// Purpose  : Show the color scale. [Reimplemented]
//================================================================

void QtxColorScale::show()
{
#if QT_VER == 3
	if ( myDock )
		myDock->activate();
	else
#endif
	QFrame::show();
}

//================================================================
// Function : hide
// Purpose  : Hides the color scale. [Reimplemented]
//================================================================

void QtxColorScale::hide()
{
#if QT_VER == 3
	if ( myDock )
		myDock->deactivate();
	else
#endif
	QFrame::hide();
}

//================================================================
// Function : drawContents
// Purpose  : Draw color scale contents. [Reimplemented]
//================================================================

void QtxColorScale::drawContents( QPainter* p )
{
	if ( !isUpdatesEnabled() )
		return;

	QRect aDrawRect = contentsRect();

	drawScale( p, false/*testFlags( Transparent )*/, aDrawRect.x(),
			   aDrawRect.y(), aDrawRect.width(), aDrawRect.height(),
			   titlePosition() != None, labelPosition() != None, true );
}

//================================================================
// Function : drawScale
// Purpose  : Draw color scale contents.
//================================================================

void QtxColorScale::drawScale( QPainter* p, const bool transp, const int X, const int Y,
                               const int W, const int H, const bool title,
                               const bool label, const bool scale ) const
{
	QPixmap cache( W, H );
	QPainter cp( &cache );

#if QT_VER < 3
	drawScale( &cp, backgroundColor(), transp, 0, 0, W, H, title, label, scale );
#else
	drawScale( &cp, paletteBackgroundColor(), transp, 0, 0, W, H, title, label, scale );
#endif
	cp.end();

	p->drawPixmap( X, Y, cache );
}

//================================================================
// Function : drawScale
// Purpose  : Draw color scale contents.
//================================================================

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
		QSimpleRichText* srt = simpleRichText( myFlags );
		if ( srt )
		{
			srt->setWidth( p, W - 10 );
			titleHeight = srt->height() + spacer;
			titleWidth = srt->widthUsed();
			QColorGroup cg = colorGroup();
			cg.setColor( QColorGroup::Text, p->pen().color() );
			srt->draw( p, X + 5, Y, QRect( 0, 0, srt->width(), srt->height() ), cg );

			delete srt;
		}
	}

	bool reverse = testFlags( Reverse );

	QValueList<QColor>  colors;
	QValueList<QString> labels;
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
			textWidth = QMAX( textWidth, p->fontMetrics().width( labels.last() ) );
	}

	if ( drawLabel )
	{
		const QFontMetrics& fm = p->fontMetrics();
		for ( QStringList::ConstIterator it = labels.begin(); it != labels.end(); ++it )
			textWidth = QMAX( textWidth, fm.width( *it) );
	}

	int lab = labels.count();

	double spc = ( H - ( ( QMIN( lab, 2 ) + QABS( lab - num - 1 ) ) * textHeight ) - titleHeight );
	double val = spc != 0 ? 1.0 * ( lab - QMIN( lab, 2 ) ) * textHeight / spc : 0;
	double iPart;
	double fPart = modf( val, &iPart );
	int filter = (int)iPart + ( fPart != 0 ? 1 : 0 );
	filter = QMAX( filter, 1 );

	double step = 1.0 * ( H - ( lab - num + QABS( lab - num - 1 ) ) * textHeight - titleHeight ) / num;

	int ascent = p->fontMetrics().ascent();
	int colorWidth = QMAX( 5, QMIN( 20, W - textWidth - 3 * spacer ) );
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

	double offset = 1.0 * textHeight / 2 * ( lab - num + QABS( lab - num - 1 ) ) + titleHeight;
	QValueList<QColor>::Iterator cit = colors.begin();
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
	offset = 1.0 * QABS( lab - num - 1 ) * ( step - textHeight ) / 2 +
						 1.0 * QABS( lab - num - 1 ) * textHeight / 2;
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
				p->drawText( x, (int)( Y + i1 * step + ascent + offset ), *labels.at( i1 ) );
				last1 = i1;
			}
			if ( filter && !( pos2 % filter ) )
			{
				p->drawText( x, (int)( Y + i2 * step + ascent + offset ), *labels.at( i2 ) );
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
				 QABS( pos - last1 ) >= filter && QABS( pos - last2 ) >= filter )
				i0 = pos;
			pos++;
		}

		if ( i0 != -1 )
			p->drawText( x, (int)( Y + i0 * step + ascent + offset ), *labels.at( i0 ) );
	}
}

//================================================================
// Function : getFormat
// Purpose  : Returns the format for number labels.
//================================================================

QString QtxColorScale::getFormat() const
{
	QString aFormat = format();

	if ( !testFlags( PreciseFormat ) || testFlags( Integer ) )
		return aFormat;

	if ( !myPrecise.isEmpty() )
		return myPrecise;

	if ( aFormat.find( QRegExp( "^(%[0-9]*.?[0-9]*[fegFEG])$" ) ) != 0 )
		return aFormat;

	int pos1 = aFormat.find( '.' );
	int pos2 = aFormat.find( QRegExp( "[fegFEG]") );

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
			QString tmpname = QString().sprintf( aTmpFormat, val );
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

//================================================================
// Function : getNumber
// Purpose  : Returns the number for specified interval.
//================================================================

double QtxColorScale::getNumber( const int idx ) const
{
	double val = 0;
	if ( intervalsNumber() > 0 )
		val = minimum() + idx * ( QABS( maximum() - minimum() ) / intervalsNumber() );
	return val;
}

//================================================================
// Function : getLabel
// Purpose  : Returns the label for specified interval according
//            to the current label mode.
//================================================================

QString QtxColorScale::getLabel( const int idx ) const
{
	QString res;
	if ( labelMode() == User )
		res = label( idx );
	else
	{
		double val = getNumber( idx );
		res = QString().sprintf( getFormat(), testFlags( Integer ) ? (int)val : val );
	}
	return res;
}

//================================================================
// Function : getColor
// Purpose  : Returns the color for specified interval according
//            to the current color mode.
//================================================================

QColor QtxColorScale::getColor( const int idx ) const
{
	QColor res;
	if ( colorMode() == User )
		res = color( idx );
	else
		res = QColor( hueFromValue( idx, 0, intervalsNumber() - 1 ), 255, 255, QColor::Hsv );
	return res;
}

//================================================================
// Function : updateScale
// Purpose  : Update color scale if it required.
//================================================================

void QtxColorScale::updateScale()
{
  update();
	updateGeometry();
}

//================================================================
// Function : simpleRichText
// Purpose  : Return QSimpleRichText object for title. If title
//            not defined (empty string) then return null pointer.
//            Object should be deleted by caller function.
//================================================================

QSimpleRichText* QtxColorScale::simpleRichText( const int flags ) const
{
	QSimpleRichText* srt = 0;

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

		aTitle = aTitle.arg( title() );
		srt = new QSimpleRichText( aTitle, font(), QString::null, myStyleSheet );
	}

	return srt;
}

//================================================================
// Function : hueFromValue
// Purpose  : 
//================================================================

int QtxColorScale::hueFromValue( const int value, const int min, const int max ) const
{
  static const int HUE[10] = {230, 210, 195, 180, 160, 80, 60, 50, 30, 0};

	if ( min == max )
    return HUE[0];

  double aPosition = 9.0 * ( value - min ) / ( max - min );

  if ( aPosition <= 0. )
    return HUE[0];

  if ( aPosition >= 9. )
    return HUE[9];

  int idx = (int)aPosition;

  return HUE[idx] + int( ( aPosition - idx ) * ( HUE[idx + 1] - HUE[idx] ) );
}

#if QT_VER == 3

/*********************************************************************
**  Class:  QtxColorScale::Dock
**  Descr:  Dockable window contains the color scale.
**  Level:  Public
*********************************************************************/

//================================================================
// Function : Dock
// Purpose  : Constructor.
//================================================================

QtxColorScale::Dock::Dock( Place p, QWidget* parent, const char* name, WFlags f )
: QDockWindow( p, parent, name, f ),
myBlockShow( false ),
myBlockResize( false )
{
	myScale = new QtxColorScale( this );

	setWidget( myScale );

	setCloseMode( Always );
	setMovingEnabled( true );
	setResizeEnabled( true );
	setHorizontalStretchable( false );

	setCaption( tr ( "Color scale" ) );
}

//================================================================
// Function : ~Dock
// Purpose  : Destructor.
//================================================================

QtxColorScale::Dock::~Dock()
{
}

//================================================================
// Function : colorScale
// Purpose  : Returns color scale widget.
//================================================================

QtxColorScale* QtxColorScale::Dock::colorScale() const
{
	return myScale;
}

//================================================================
// Function : activate
// Purpose  : Set the dockable window is visible for main window.
//================================================================

void QtxColorScale::Dock::activate()
{
	if ( myBlockShow )
		return;

	QMainWindow* mw = 0;
	QWidget* p = parentWidget();
	while ( !mw && p )
	{
		if ( p->inherits( "QMainWindow" ) )
			mw = (QMainWindow*)p;
		p = p->parentWidget();
	}
	if ( mw )
		mw->setAppropriate( this, true );
}

//================================================================
// Function : deactivate
// Purpose  : Set the dockable window is hidden for main window.
//================================================================

void QtxColorScale::Dock::deactivate()
{
	if ( myBlockShow )
		return;

	QMainWindow* mw = 0;
	QWidget* p = parentWidget();
	while ( !mw && p )
	{
		if ( p->inherits( "QMainWindow" ) )
			mw = (QMainWindow*)p;
		p = p->parentWidget();
	}
	if ( mw )
		mw->setAppropriate( this, false );
}

//================================================================
// Function : isActive
// Purpose  : Returns true if the dockable window is visible.
//================================================================

bool QtxColorScale::Dock::isActive() const
{
	QMainWindow* mw = 0;
	QWidget* p = parentWidget();
	while ( !mw && p )
	{
		if ( p->inherits( "QMainWindow" ) )
			mw = (QMainWindow*)p;
		p = p->parentWidget();
	}
	if ( mw )
		return mw->appropriate( (QDockWindow*)this );
	else
		return false;
}

//================================================================
// Function : show
// Purpose  : Reimplemented for internal reasons.
//================================================================

void QtxColorScale::Dock::show()
{
	bool f = myBlockShow;
	myBlockShow = true;
	QDockWindow::show();
	myBlockShow = f;
}

//================================================================
// Function : hide
// Purpose  : Reimplemented for internal reasons.
//================================================================

void QtxColorScale::Dock::hide()
{
	bool f = myBlockShow;
	myBlockShow = false;
	QDockWindow::hide();
	myBlockShow = f;
}

//================================================================
// Function : resize
// Purpose  : Make extent width as maximum value of widget width.
//================================================================

void QtxColorScale::Dock::resize( int w, int h )
{
	QDockWindow::resize( w, h );

	if ( myBlockResize )
		return;

	if ( orientation() == Qt::Vertical )
		setFixedExtentWidth( QMAX( fixedExtent().width(), w ) );
	else if ( orientation() == Qt::Horizontal )
		setFixedExtentHeight( QMAX( fixedExtent().height(), h ) );
}

//================================================================
// Function : setOrientation
// Purpose  : 
//================================================================

void QtxColorScale::Dock::setOrientation( Orientation o )
{
	bool b = myBlockResize;
	myBlockResize = true;
	QDockWindow::setOrientation( o );
	myBlockResize = b;
}

#endif
