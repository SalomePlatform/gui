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
// File   : QtxSplash.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "QtxSplash.h"
#include "QtxResourceMgr.h"

#include <QApplication>
#include <QPainter>
#include <QMessageBox>
#include <QDesktopWidget>

/*!
  \class ProgressEvent
  \internal
  \brief Progress change custom event.
*/

class ProgressEvent: public QEvent
{
public:
  /*!
    \brief Constructor.
    \param msg progress message
    \param progress current progress (for example, in %)
  */
  ProgressEvent( const QString& msg, const int progress )
    : QEvent( (QEvent::Type)id() ),
      myMessage( msg ),
      myProgress( progress )
  {}
  /*!
    \brief Get progress message.
    \return message
  */
  QString    message()  const { return myMessage; } 
  /*!
    \brief Get current progress.
    \return current progress
  */
  int        progress() const { return myProgress; }
  /*!
    \brief Get message identifier.
    \return custom message ID
  */
  static int id()             { return QEvent::User + 10; }

private:
  QString myMessage;
  int     myProgress;
};

/*!
  \class QtxSplash
  \brief The QtxSplash widget provides a splash screen that can be shown during application startup..

  A splash screen is a widget that is usually displayed when an application is being started. 
  Splash screens are often used for applications that have long start up times to provide 
  the user with feedback that the application is loading.

  Only one instance of the QtxSplash widget can be created. To access to the splash screen widget,
  use static method QtxSplash::splash(), which creates and instance of the QtxSplash widget if
  necessary and returns pointer to it. You should not destroy yhis instance - it is done automatically
  after application main window is shown. Just use methods finish() to make splash screen wait untill 
  main window is shown.

  The splash screen appears in the center of the screen. The most common usage is to show a splash 
  screen before the main widget is displayed on the screen. 
  For example,
  \code
  int main(int argc, char *argv[])
  {
    QApplication app(argc, argv);
    QPixmap pixmap(":/splash.png");
    QtxSplash* splash = QtxsSplash::splash(pixmap);
    splash->show();
    app.processEvents();
    ... // do application loading and initialization
    MainWindow window;
    window.show();
    splash->finish(&window);
    return app.exec();
  }
  \endcode
  
  The user can hide the splash screen by clicking on it with the mouse. Since the splash screen is 
  typically displayed before the event loop has started running, it is necessary to periodically call 
  QApplication::processEvents() to receive the mouse clicks.
  This feature can be switched off by using method setHideOnClick() with \c false parameter.

  It is sometimes useful to update the splash screen with messages and/or progress information, 
  for example, announcing connections established or modules loaded as the application starts up.
  QtxSplash class provides the functionality to show status messages and(or) progress bar.

  \code
  QPixmap pixmap(":/splash.png");
  QtxSplash* splash = QtxSplash::splash(pixmap);
  splash->setProgress(0, 5);
  splash->show();
  app.processEvents();
  // doing first step
  splash->setMessage("Step 1");
  splash->ress(1);
  qApp->processEvents();
  // ... perform some actions
  // doing second step
  splash->setMessage("Step 2");
  splash->setProgress(2);
  qApp->processEvents();
  // ... perform some actions
  ... et cetera
  \endcode

  There is a static function QtxSplash::setStatus() which allows to put next status message
  and progress with one call. It can substitue two calls: setMessage() and setProgress().

  QtxSplash class provides alos a lot of functions to set-up its behavior. Set progress
  bar width with setProgressWidth() method, its position and direction with setProgressFlags().
  It can be single-colored or gradient-colored. Use setProgressColors() method for this. 
  You can even set your own gradient scale with QLinearGradient and use it for the progress
  bar coloring: setProgressGradient(). In addition, it is possible to enable/disable displaying
  of the progress percentage with setPercentageVisible() method.

  To change the progress bar and status message transparency, use setOpacity() function.
  The methods setTextAlignment(), setTextColor() and setTextColors() can be used to change
  the attributes of the status message.

  The displayed message text includes constant info and status message. The constant info
  is set by setConstantInfo() method and status message is set by setMessage().

  Sometimes it is useful to splay an error message above the splash screen window.
  For example, it can be necessary if an error occurs when loading the application.
  Method setError() can be used to show the error message and set the error code which
  can be then retrieved with the error() function.
*/

//! The only one instance of splash screen
QtxSplash* QtxSplash::mySplash = 0;

/*!
  \brief Constructor.
  \brief Construct a splash screen that will display the \a pixmap.
  \param pixmap splash screen pixmap
*/
QtxSplash::QtxSplash( const QPixmap& pixmap )
: QWidget( 0, Qt::SplashScreen | Qt::WindowStaysOnTopHint ),
  myAlignment( Qt::AlignBottom | Qt::AlignRight ),
  myColor( Qt::white ),
  myHideOnClick( false ),
  myProgress( 0 ),
  myTotal( 0 ),
  myStartColor( Qt::red ),
  myGradientType( Vertical ),
  myProgressWidth( 10 ),
  myProgressFlags( BottomSide | LeftToRight ),
  myMargin( 5 ),
  myOpacity( 1.0 ),
  myError( 0 ),
  myGradientUsed( false ),
  myShowPercent( true )
{
  setAttribute( Qt::WA_DeleteOnClose, true );
  setPixmap( pixmap );
}

/*!
  \brief Destructor.
*/
QtxSplash::~QtxSplash()
{
  mySplash = 0;
}

/*!
  \brief Get the only instance of the splash screen widget.

  If the splash screen widget does not exist yet, it is created with specified
  pixmap. Otherwise, pixmap \a px is set to existing widget.

  \param px splash screen pixmap
  \return splash screen widget
*/
QtxSplash* QtxSplash::splash( const QPixmap& px )
{
  if ( !mySplash )
    mySplash = new QtxSplash( px );
  else if ( !px.isNull() )
    mySplash->setPixmap( px );
  return mySplash;
}

/*!
  \brief Send the status message and (optionally) current progress 
  to the splash screen.

  If the second parameter is less than 0 (default) than it is ignored
  and only the status message is changed. If you want to modify progress
  also, pass positive value to the \a progress parameter explicitly.

  \param msg progress status message
  \param progress current progress
  \sa setMessage(), setProgress()
*/
void QtxSplash::setStatus( const QString& msg, const int progress )
{
  if ( mySplash ) {
    QApplication::postEvent( mySplash, new ProgressEvent( msg, progress ) );
    QApplication::instance()->processEvents();
  }
}

/*!
  \brief Set error status and show error message box to the user.
  \param error error message
  \param title message box title
  \param code error code
  \sa error()
*/
void QtxSplash::setError( const QString& error, const QString& title, const int code )
{
  if ( mySplash ) {
    mySplash->setError( code );
    QMessageBox::critical( mySplash, 
			   title.isEmpty() ? tr( "Error" ) : title,
			   error,
			   tr( "&OK" ) );
  }
  else {
    printf( "QtxSplash::error: %s\n",error.toLatin1().constData() );
  }
}

/*!
  \brief Set the pixmap that will be used as the splash screen's image.
  \param pixmap spash screen image pixmap
  \sa pixmap()
*/
void QtxSplash::setPixmap( const QPixmap& pixmap )
{
  if ( pixmap.hasAlpha() ) {
    QPixmap opaque( pixmap.size() );
    QPainter p( &opaque );
    p.fillRect( 0, 0, pixmap.width(), pixmap.height(), palette().background() );
    p.drawPixmap( 0, 0, pixmap );
    p.end();
    myPixmap = opaque;
  } 
  else {
    myPixmap = pixmap;
  }
  QRect r( 0, 0, myPixmap.size().width(), myPixmap.size().height() );
  resize( myPixmap.size() );
  move( QApplication::desktop()->screenGeometry().center() - r.center() );
  if ( !isVisible() )
    drawContents();
  else
    repaint();
}

/*!
  \brief Get the pixmap that is used as the splash screen's image.
  \return spash screen image pixmap
  \sa setPixmap()
*/
QPixmap QtxSplash::pixmap() const
{
  return myPixmap;
}

/*!
  \brief Set/clear the 'hide on mouse click' flag.

  When this flag is set, user can hide the splash screen window
  by clicking on it with mouse.
  But for this to work it is necessary to call periodically
  QApplication::processEvents() in order to allow event loop to process
  events because usually main application loop is not yet started
  at that moment.

  By default this flag is set to \c false.

  \param on new flag state
  \sa hideOnClick()
*/
void QtxSplash::setHideOnClick( const bool on )
{
  myHideOnClick = on;
}

/*!
  \brief Get the 'hide on mouse click' flag.
  \return 'hide on mouse click' flag
  \sa setHideOnClick()
*/
bool QtxSplash::hideOnClick() const
{
  return myHideOnClick;
}

/*!
  \brief Set total progress steps to \a total.
  \param total total number of progress steps
  \sa totalSteps(), setProgress(), progress()
*/
void QtxSplash::setTotalSteps( const int total )
{
  myTotal = total;
  repaint();
}

/*!
  \brief Get total progress steps number.
  \return total number of progress steps
  \sa setTotalSteps(), setProgress(), progress()
*/
int QtxSplash::totalSteps() const
{
  return myTotal;
}
    
/*!
  \brief Set current progress.
  \param progress current progress
  \sa progress(), setTotalSteps(), setTotalSteps(), 
*/
void QtxSplash::setProgress( const int progress )
{
  myProgress = progress > 0 ? progress : 0;
  repaint();
}

/*!
  \brief Get current progress.
  \return current progress
  \sa setProgress(), setTotalSteps(), setTotalSteps(), 
*/
int QtxSplash::progress() const
{
  return myProgress;
}

/*!
  \brief Set current progress to \a progress and total number of 
  progress steps to \a total.
  \param progress current progress
  \param total total number of progress steps
*/
void QtxSplash::setProgress( const int progress, const int total )
{
  myTotal    = total;
  myProgress = progress > 0 ? progress : 0;
  repaint();
}

/*!
  \brief Set margin (a border width).
  \param margin new margin width
  \sa margin()
*/
void QtxSplash::setMargin( const int margin )
{
  myMargin = margin > 0 ? margin : 0;
  repaint();
}

/*!
  \brief Get margin (a border width).
  \return current margin width
  \sa setMargin()
*/
int QtxSplash::margin() const
{
  return myMargin;
}

/*!
  \brief Set progress bar width.
  \param width new progress bar width
  \sa progressWidth()
*/
void QtxSplash::setProgressWidth( const int width )
{
  myProgressWidth = width > 0 ? width : 0;
  repaint();
}

/*!
  \brief Get progress bar width.
  \return current progress bar width
  \sa setProgressWidth()
*/
int QtxSplash::progressWidth() const
{
  return myProgressWidth;
}

/*!
  \brief Enable/disable displaying progress percentage.
  \param enable if \c true, percentage will be displayed
  \sa percentageVisible()
*/
void QtxSplash::setPercentageVisible( const bool enable )
{
  myShowPercent = enable;
  repaint();
}

/*!
  \brief Check if the progress percentage is displayed.
  \return \c true if percentage displaying is enabled
  \sa setPercentageVisible()
*/
bool QtxSplash::percentageVisible() const
{
  return myShowPercent;
}

/*!
  \brief Set progress bar position and direction.

  By default, progress bar is displayed at the bottom side and
  shows progress from left to right.

  \param flags ORed progress bar flags (QtxSplash::ProgressBarFlags)
  \sa progressFlags()
*/
void QtxSplash::setProgressFlags( const int flags )
{
  myProgressFlags = flags;
  if ( !( myProgressFlags & ( LeftSide | RightSide | TopSide | BottomSide ) ) )
    myProgressFlags |= BottomSide;
  if ( !( myProgressFlags & ( LeftToRight | RightToLeft ) ) )
    myProgressFlags |= LeftToRight ;
  repaint();
}

/*!
  \brief Get progress bar flags: position and direction.
  \return ORed progress bar flags (QtxSplash::ProgressBarFlags)
  \sa setProgressFlags()
*/
int QtxSplash::progressFlags() const
{
  return myProgressFlags;
}

/*!
  \brief Set progress bar colors.

  If the colors differ the gradient color bar is drawn.
  
  If the \a endColor is not valid, \a startColor is used instead
  (no gradient coloring).
  
  The parameter \a gradientType defines the type of gradient 
  to be drawn - horizontal or vertical. Default is vertical.

  Note, that methods setProgressGradient() and setProgressColors() are
  alternative. Only the latest used is taken into account.

  \param startColor start gradient color (or mono-color)
  \param endColor end gradient color
  \param gradientType gradient type (QtxSplash::GradientType)
  \sa progressColors(), setProgressGradient()
*/
void QtxSplash::setProgressColors( const QColor&      startColor, 
				   const QColor&      endColor,
                                   const GradientType gradientType )
{
  if ( startColor.isValid() )
    myStartColor = startColor;
  myEndColor     = endColor;
  myGradientType = gradientType;
  myGradientUsed = false;
  repaint();
}

/*!
  \brief Get progress colors and gradient type.
  \param startColor start gradient color (or mono-color)
  \param endColor end gradient color
  \return gradient type (QtxSplash::GradientType)
  \sa setProgressColors()
*/
QtxSplash::GradientType QtxSplash::progressColors( QColor& startColor, 
						   QColor& endColor ) const
{
  startColor = myStartColor;
  endColor   = myEndColor;
  return myGradientType;
}

/*!
  \brief Set custom progress bar colors.

  The gradient start and final stops are scaled to the actual progress
  bar size. For example:
  \code
  QLinearGradient lg(0.5, 0, 1, 1);
  lg.setColorAt(0.2, Qt::blue);
  lg.setColorAt(0.6, Qt::red);
  lg.setSpread(QGradient::RepeatSpread);
  splash->setProgressGradient(lg);
  \endcode
  The above code creates linear gradient, which sets start stop to the
  center of the progress bar; the final stop is always in the end of
  the progress bar. The color scale (blue to red) is changed by the 
  progress bar diagonal.

  Note, that methods setProgressGradient() and setProgressColors() are
  alternative. Only the latest used is taken into account.

  \param gradient color gradient to be used for progress bar coloring
  \sa progressGradient(), setProgressColors()
*/
void QtxSplash::setProgressGradient( const QLinearGradient& gradient )
{
  myGradient = gradient;
  myGradientUsed = true;
  repaint();
}

/*!
  \brief Get custom progress bar colors.
  \return color gradient used for progress bar coloring
  \sa setProgressGradient()
*/
QLinearGradient QtxSplash::progressGradient() const
{
  return myGradient;
}

/*!
  \brief Set progress bar and status text message opacity.

  The value should be in the range 0.0 to 1.0, where 0.0 is fully 
  transparent and 1.0 is fully opaque.

  \param opacity new opacity value
  \sa opacity()
*/
void QtxSplash::setOpacity( const double opacity )
{
  myOpacity = opacity < 0.0 ? 0.0 : ( opacity > 1.0 ? 1.0 : opacity );
  repaint();
}

/*!
  \brief Get progress bar and status text message opacity.
  \return current opacity value
  \sa setOpacity()
*/
double QtxSplash::opacity() const
{
  return myOpacity;
}

/*!
  \brief Set message text alignment flags.

  Default flags are Qt::AlignBottom | Qt::AlignRight.

  \param alignment alignment flags (Qt::Alignment)
  \sa textAlignment()
*/
void QtxSplash::setTextAlignment( const int alignment )
{
  myAlignment = alignment;
  repaint();
}

/*!
  \brief Get message text alignment flags.
  \return alignment flags (Qt::Alignment)
  \sa setTextAlignment()
*/
int QtxSplash::textAlignment() const
{
  return myAlignment;
}

/*!
  \brief Set message text color.

  Default message color is white.
  
  \param color message text color
  \sa setTextColors()
*/
void QtxSplash::setTextColor( const QColor& color )
{
  if ( myColor.isValid() )
    myColor = color;
  myShadowColor = QColor();
  repaint();
}

/*!
  \brief Get message text color.
  \return color message text color
  \sa setTextColor()
*/
QColor QtxSplash::textColor() const
{
  return myColor;
}

/*!
  \brief Set message text color and text shadow color.
  \param color message text color
  \param shadow message text shadow color
  \sa textColors(), textColor(), setTextColor()
*/
void QtxSplash::setTextColors( const QColor& color, const QColor& shadow )
{
  if ( myColor.isValid() )
    myColor = color;
  myShadowColor = shadow;
  repaint();
}

/*!
  \brief Get message text color and text shadow color.
  \param color message text color
  \param shadow message text shadow color
  \sa setTextColors(), textColor(), setTextColor()
*/
void QtxSplash::textColors( QColor& color, QColor& shadow ) const
{
  color  = myColor;
  shadow = myShadowColor;
}

/*!
  \brief Set constant info text to be displayed on the splash screen.

  The displayed text includes constant info and status message.
  The constant message is set by setConstantInfo() method and status
  message is set by setMessage().

  \param info constant info text
  \sa constantInfo(), message(), setMessage()
*/
void QtxSplash::setConstantInfo( const QString& info )
{
  myInfo = info;
  repaint();
}

/*!
  \brief Get constant info text.
  \return constant info text
  \sa setConstantInfo(), message(), setMessage()
*/
QString QtxSplash::constantInfo() const
{
  return myInfo;
}

/*!
  \brief Set constant information option value.

  The option is a part of the constant information text,
  which is replaced at the time of the displaying.

  The following options are supported:
  - %A - could be used as application name
  - %V - could be used as application version
  - %L - could be used as application license information
  - %C - could be used as application copyright information

  \param name option name
  \param option value
  \sa option()
*/
void QtxSplash::setOption( const QString& name, const QString& value )
{
  myOptions[ name ] = value;
  repaint();
}

/*!
  \brief Get constant information option value.
  \param name option name
  \return option value or empty string if option is not set
  \sa setOption()
*/
QString QtxSplash::option( const QString& name ) const
{
  QString val;
  if ( myOptions.contains( name ) )
    val = myOptions[ name ];
  return val;
}

/*!
  \brief Get current status message.
  \return status message
  \sa setMessage(), constantInfo(), setConstantInfo()
*/
QString QtxSplash::message() const
{
  return myMessage;
}

/*!
  \brief Get error code.

  This function returns error code, set previoiusly with 
  error(const QString&, const QString&, const int) method.
  If no error code has been set, 0 is returned.

  \return last error code
  \sa setError()
*/
int QtxSplash::error() const
{
  return myError;
}

/*!
  \brief Wait until widget \a mainWin is displayed.

  Makes the splash screen wait until the widget \a mainWin is displayed
  and then hide and close splash window.

  \param mainWin application main window 
*/
void QtxSplash::finish( QWidget* mainWin )
{
  if ( mainWin ) {
#if defined(Q_WS_X11)
    extern void qt_x11_wait_for_window_manager(QWidget *mainWin);
    qt_x11_wait_for_window_manager(mainWin);
#endif
  }
  close();
}

/*!
  \brief Repaint the splash screen.
*/
void QtxSplash::repaint()
{
  drawContents();
  QWidget::repaint();
  QApplication::flush();
}

/*!
  \brief Read splash settings from the resources manager.
  \param resMgr resources manager
  \param section resources file section name (if empty, the default name is used).
*/
void QtxSplash::readSettings( QtxResourceMgr* resMgr, const QString& section )
{
  QString resSection = section.isEmpty() ? "splash" : section;
  
  // pixmap
  QString pxname;
  if ( resMgr->value( resSection, "image", pxname ) ) {
    QPixmap px( pxname );
    if ( !px.isNull() )
      setPixmap( px );
  }

  // hide-on-click
#ifdef _DEBUG_
  setHideOnClick( true );
#else
  bool bHide;
  if ( resMgr->value( resSection, "hide_on_click", bHide ) ) {
    setHideOnClick( bHide );
  }
#endif

  // margin
  int m;
  if ( resMgr->value( resSection, "margin", m ) ) {
    setMargin( m );
  }

  // progress bar width
  int pw;
  if ( resMgr->value( resSection, "progress_width", pw ) ) {
    setProgressWidth( pw );
  }

  // progress bar position and direction
  QString pf;
  if ( resMgr->value( resSection, "progress_flags", pf ) ) {
    bool bOk;
    int fl = pf.toInt( &bOk );
    if ( !bOk ) {
      fl = 0;
      QStringList opts = pf.split( QRegExp( "," ), QString::SkipEmptyParts );
      for ( int i = 0; i < opts.count(); i++ ) {
	QString opt = opts[i].trimmed().toLower();
	if ( opt == "left" )
	  fl = fl | LeftSide;
	else if ( opt == "right" )
	  fl = fl | RightSide;
	else if ( opt == "top" )
	  fl = fl | TopSide;
	else if ( opt == "bottom" )
	  fl = fl | BottomSide;
	else if ( opt == "left_to_right" )
	  fl = fl | LeftToRight;
	else if ( opt == "right_to_left" )
	  fl = fl | RightToLeft;
      }
    }
    setProgressFlags( fl );
  }
  
  // opacity
  double op;
  if ( resMgr->value( resSection, "opacity", op ) ) {
    setOpacity( op );
  }

  // font
  QFont f;
  if ( resMgr->value( resSection, "font", f ) ) {
    setFont( f );
  }

  // text alignment
  QString al;
  if ( resMgr->value( resSection, "alignment", al ) ) {
    bool bOk;
    int fl = al.toInt( &bOk );
    if ( !bOk ) {
      fl = 0;
      QStringList opts = al.split( QRegExp( "," ), QString::SkipEmptyParts );
      for ( int i = 0; i < opts.count(); i++ ) {
	QString opt = opts[i].trimmed().toLower();
	if ( opt == "left" )
	  fl = fl | Qt::AlignLeft;
	else if ( opt == "right" )
	  fl = fl | Qt::AlignRight;
	else if ( opt == "top" )
	  fl = fl | Qt::AlignTop;
	else if ( opt == "bottom" )
	  fl = fl | Qt::AlignBottom;
	else if ( opt == "hcenter" )
	  fl = fl | Qt::AlignHCenter;
	else if ( opt == "vcenter" )
	  fl = fl | Qt::AlignVCenter;
	else if ( opt == "justify" )
	  fl = fl | Qt::AlignJustify;
	else if ( opt == "center" )
	  fl = fl | Qt::AlignCenter;
      }
    }
    setTextAlignment( fl );
  }

  // progress color(s)
  QString pc;
  QLinearGradient grad;
  if ( resMgr->value( resSection, "progress_gradient", grad ) ) {
    // gradient-colored progress bar
    setProgressGradient( grad );
  }
  else if ( resMgr->value( resSection, "progress_color",  pc ) || 
	    resMgr->value( resSection, "progress_colors", pc ) ) {
    // one/two-colored progress bar
    QStringList colors = pc.split( "|", QString::SkipEmptyParts );
    QColor c1, c2;
    QtxSplash::GradientType gradType = QtxSplash::Vertical;
    if ( colors.count() > 0 ) c1 = QColor( colors[0] );
    if ( colors.count() > 1 ) c2 = QColor( colors[1] );
    int gt;
    if ( colors.count() > 2 ) {
      bool bOk;
      gt = colors[2].toInt( &bOk );
      if ( bOk ) {
	if ( gt >= QtxSplash::Horizontal && gt <= QtxSplash::Vertical )
	  gradType = (QtxSplash::GradientType)gt;
      }
      else {
	if ( colors[2].toLower() == "horizontal" )
	  gradType = QtxSplash::Horizontal;
	else if ( colors[2].toLower() == "vertical" )
	  gradType = QtxSplash::Vertical;
      }
    }
    setProgressColors( c1, c2, gradType );
  }

  // show percents
  bool bPercent;
  if ( resMgr->value( resSection, "show_percents", bPercent ) ) {
    setPercentageVisible( bPercent );
  }

  // text color(s)
  QString tc;
  if ( resMgr->value( resSection, "text_color",  tc ) || 
       resMgr->value( resSection, "text_colors", tc ) ) {
    QStringList colors = tc.split( "|", QString::SkipEmptyParts );
    QColor c1, c2;
    if ( colors.count() > 0 )
      c1 = QColor( colors[0] );
    if ( colors.count() > 1 )
      c2 = QColor( colors[1] );
    setTextColors( c1, c2 );
  }

  // const info
  QString cinfo;
  if ( resMgr->value( resSection, "constant_info", cinfo, false ) ||
       resMgr->value( resSection, "info", cinfo, false ) ) {
    setConstantInfo( cinfo.split( "|", QString::KeepEmptyParts ).join( "\n" ) );
  }
}

/*!
  \brief Set status message for the splash screen and define its color 
  and aligment flags.
  \param msg status message
  \param alignment message text alignment flags (Qt::Alignment)
  \param color message text color
  \sa message(), constantInfo(), setConstantInfo()
*/
void QtxSplash::setMessage( const QString& msg, 
			    int            alignment,
			    const QColor&  color )
{
  myMessage   = msg;
  myAlignment = alignment;
  if ( color.isValid() )
    myColor = color;
  repaint();
}

/*!
  \overload
  \brief Set status message for the splash screen.
  \param msg status message
  \sa message(), constantInfo(), setConstantInfo()
*/
void QtxSplash::setMessage( const QString& msg )
{
  myMessage = msg;
  repaint();
}

/*!
  \brief Remove the message being displayed on the splash screen.
  \sa message()
*/
void QtxSplash::clear()
{
  myMessage.clear();
  repaint();
}

/*!
  \brief Draw the contents of the splash screen.
  \param painter painter
*/
void QtxSplash::drawContents( QPainter* p )
{
  // draw progress bar
  if ( myTotal > 0 ) {
    p->save();
    drawProgressBar( p );
    p->restore();
  }

  // draw status message
  if ( !fullMessage().isEmpty() ) {
    p->save();
    drawMessage( p );
    p->restore();
  }
}

/*!
  \brief Process mouse button pressing event.

  Hides splash screen if the 'hide on mouse click' flag is set.

  \param me mouse event (not used)
  \sa hideOnClick(), setHideOnClick()
*/
void QtxSplash::mousePressEvent( QMouseEvent* /*me*/ )
{
  if ( myHideOnClick )
    hide();
}

/*!
  \brief Customize paint event.

  This function is implemented to work-around the Qt bug
  on some Linux distribututions when the drawing on the 
  splash screen widget is not allowed.

  \param pe paint event (not used)
*/
void QtxSplash::paintEvent( QPaintEvent* /*pe*/ )
{
  QPainter p( this );
  QPixmap pix = palette().brush( backgroundRole() ).texture();
  p.drawPixmap( 0, 0, pix );
}

/*!
  \brief Process custom event sent by setStatus() method.
  \param ce custom event
  \sa setStatus().
*/
void QtxSplash::customEvent( QEvent* ce )
{
  if ( ce->type() == ProgressEvent::id() ) {
    ProgressEvent* pe = (ProgressEvent*)ce;
    pe->message().isEmpty() ? clear() : setMessage( pe->message() );
    if ( pe->progress() >= 0 )
      setProgress( pe->progress() );
    QApplication::instance()->processEvents();
  }
}

/*!
  \brief Draw progress bar.
  \param p painter
*/
void QtxSplash::drawProgressBar( QPainter* p )
{
  // get rect, margin, progress bar width
  QRect r = rect();
  int m   = margin();
  int pw  = progressWidth();

  // calculate drawing rect
  // ... first set default position (if none or wrong position is set)
  if ( myProgressFlags & BottomSide )
    r = QRect( r.x() + m, r.height() - (m + pw), r.width() - 2 * m, pw );
  else if ( myProgressFlags & TopSide )
    r = QRect( r.x() + m, r.y() + m, r.width() - 2 * m, pw );
  else if ( myProgressFlags & LeftSide )
    r = QRect( r.x() + m, r.y() + m, pw, r.height() - 2 * m );
  else if ( myProgressFlags & RightSide )
    r = QRect( r.width() - (m + pw), r.y() + m, pw, r.height() - 2 * m );

  QRect cr = r;
  if ( myProgressFlags & TopSide || myProgressFlags & BottomSide ) {
    cr.setWidth( (int)( r.width() * ( myProgress > 0 ? myProgress : 0 ) / myTotal ) );
    if ( myProgressFlags & RightToLeft )
      cr.translate( r.width() - cr.width(), 0 );
  }
  else if ( myProgressFlags & LeftSide || myProgressFlags & RightSide ) {
    cr.setHeight( (int)( r.height() * ( myProgress > 0 ? myProgress : 0 ) / myTotal ) );
    if ( myProgressFlags & RightToLeft)
      cr.translate( 0, r.height() - cr.height() );
  }
  int x1, x2, y1, y2;
  if ( myGradientType == Horizontal ) {
    x1 = r.left(); x2 = r.right(); y1 = y2 = 0;
  }
  else {
    x1 = x2 = 0; y1 = r.top(); y2 = r.bottom();
  }
  QLinearGradient lg;
  if ( myGradientUsed ) {
    QPointF start = myGradient.start();
    QPointF final = myGradient.finalStop();
    qreal xd = final.x() - start.x();
    qreal yd = final.y() - start.y();
    lg.setStart( xd != 0 ? r.left() + r.width() * start.x() / xd : 0, 
                 yd != 0 ? r.top() + r.height() * start.y() / yd : 0 );
    lg.setFinalStop( xd != 0 ? r.right() : 0, yd != 0 ? r.bottom() : 0 );
    lg.setStops( myGradient.stops() );
    lg.setSpread( myGradient.spread() );
  }
  else {
    lg.setStart( x1, y1 );
    lg.setFinalStop( x2, y2 );
    lg.setColorAt( 0, myStartColor );
    lg.setColorAt( 1, myEndColor.isValid() ? myEndColor : myStartColor );
  }

  p->setOpacity( myOpacity );

  // draw progress bar outline rectangle
  p->setPen( palette().color( QPalette::Dark ) );
  p->drawLine( r.left(), r.top(), r.right(), r.top() );
  p->drawLine( r.left(), r.top(), r.left(), r.bottom() );
  p->setPen( palette().color( QPalette::Light ) );
  p->drawLine( r.left(), r.bottom(), r.right(), r.bottom() );
  p->drawLine( r.right(), r.top(), r.right(), r.bottom() );

  r.setCoords( r.left()+1, r.top()+1, r.right()-1, r.bottom()-1 );
  p->setClipRect( cr );
  p->fillRect( r, lg );
  p->setClipping( false );

  if ( myShowPercent ) {
    int percent = ( int )( ( myProgress > 0 ? myProgress : 0 ) * 100 / myTotal );
    QFont f = font();
    f.setPixelSize( r.height() - 4 );
    p->setFont( f );
    // draw shadow status text
    if ( myShadowColor.isValid() ) {
      QRect rs = r;
      rs.moveTopLeft( rs.topLeft() + QPoint( 1,1 ) );
      p->setPen( myShadowColor );
      p->drawText( rs, Qt::AlignCenter, QString( "%1%" ).arg( percent ) );
    }
    p->setPen( myColor );
    p->drawText( r, Qt::AlignCenter, QString( "%1%" ).arg( percent ) );
  }
}

/*!
  \brief Draw status message.
  \param p painter
*/
void QtxSplash::drawMessage( QPainter* p )
{
  // get rect, margin, progress bar width
  QRect r = rect();
  int m   = margin();
  int pw  = progressWidth();

  // calculate drawing rect
  QFontMetrics f( font() );
  int spacing = f.lineSpacing();
  // ... base rect
  QRect r1( r.x() + m, r.y() + m, r.width() - 2 * m, r.height() - 2 * m );
  r1.setY( r1.y() - f.leading() );
  // ... take into account progress bar
  if ( 1 ) {              // if ( myTotal > 0 ) : vsr changed: otherwise text is jumping
    if ( myProgressFlags & BottomSide )
      r1.setHeight( r1.height() - pw );
    else if ( myProgressFlags & TopSide )
      r1.setY( r1.y() + pw );
    else if ( myProgressFlags & LeftSide )
      r1.setX( r1.x() + pw );
    else if ( myProgressFlags & RightSide )
      r1.setWidth( r1.width() - pw );
  }
  
  // ... take into account trailing '\n' symbols
  int shift = 0;
  QString msg = fullMessage();
  int i = msg.length() - 1;
  while( i >= 0 && msg[ i-- ] == '\n' )
    shift += spacing;
  r1.setHeight( r1.height() - shift );

  p->setOpacity( myOpacity );

  // draw shadow status text
  if ( myShadowColor.isValid() ) {
    QRect r2 = r1;
    if ( myAlignment & Qt::AlignLeft   ) r2.setLeft  ( r2.left()   + 1 );
    if ( myAlignment & Qt::AlignTop    ) r2.setTop   ( r2.top()    + 1 );
    if ( myAlignment & Qt::AlignRight  ) r2.setRight ( r2.right()  + 1 );
    if ( myAlignment & Qt::AlignBottom ) r2.setBottom( r2.bottom() + 1 );
    p->setPen( myShadowColor );
    p->drawText( r2, myAlignment, msg );
  }

  // draw foreground status text
  p->setPen( myColor );
  p->drawText( r1, myAlignment, msg );
}

/*!
  \brief Draw the splash screen window contents.
*/
void QtxSplash::drawContents()
{
  QPixmap textPix = myPixmap;
  QPainter painter( &textPix );
  painter.initFrom( this );
  drawContents( &painter );
  QPalette pal = palette();
  pal.setBrush( backgroundRole(), QBrush( textPix ) );
  setPalette( pal );
}

/*!
  \brief Sets error code.
  \param code error code
*/
void QtxSplash::setError( const int code )
{
  myError = code;
}

/*!
  \brief Get full message which includes constant info and status message.
  \return get fill message text
  \sa constantInfo(), setConstantInfo(), message(), setMessage()
*/
QString QtxSplash::fullMessage() const
{
  QStringList info;

  QString cinfo = myInfo;
  cinfo = cinfo.replace( QRegExp( "%A" ), option( "%A" ) );
  cinfo = cinfo.replace( QRegExp( "%V" ), option( "%V" ) );
  cinfo = cinfo.replace( QRegExp( "%L" ), option( "%L" ) );
  cinfo = cinfo.replace( QRegExp( "%C" ), option( "%C" ) );

  if ( !cinfo.isEmpty() )
    info << cinfo;
  if ( !myMessage.isEmpty() )
    info << myMessage;
  return info.join( "\n" );
}
