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
// File   : QtxSplash.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef QTXSPLASH_H
#define QTXSPLASH_H

#include "Qtx.h"

#include <QWidget>
#include <QPixmap>
#include <QLinearGradient>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxSplash : public QWidget
{
  Q_OBJECT

private:
  QtxSplash( const QPixmap& );

public:
  //! Gradient type
  typedef enum { 
    Horizontal, //!< horizontal
    Vertical    //!< vertical
  } GradientType;
  
  //! Progress bar position and direction
  typedef enum {
    LeftSide     = 0x0001,      //!< progress bar is displayed at the left side
    RightSide    = 0x0002,      //!< progress bar is displayed at the right side
    TopSide      = 0x0004,      //!< progress bar is displayed at the top side
    BottomSide   = 0x0008,      //!< progress bar is displayed at the bottom side
    LeftToRight  = 0x0010,      //!< show progress from left to right (from top to bottom)
    RightToLeft  = 0x0020       //!< show progress from right to left (from bottom to top)
  } ProgressBarFlags;
  
  virtual ~QtxSplash();
  
  static QtxSplash* splash( const QPixmap& = QPixmap() );
  
  static void       setStatus( const QString&, const int = 0 );
  static void       error( const QString&, const QString& = QString::null, const int = -1 );
  
  void              setPixmap( const QPixmap& );
  QPixmap           pixmap() const;
  
  void              setHideOnClick( const bool );
  bool              hideOnClick() const;
  
  void              setTotalSteps( const int );
  int               totalSteps() const;
  
  void              setProgress( const int );
  void              setProgress( const int, const int );
  int               progress() const;
  
  void              setMargin( const int );
  int               margin() const;
  
  void              setProgressWidth( const int );
  int               progressWidth() const; 
  
  void              setProgressFlags( const int );
  int               progressFlags() const;
  
  void              setProgressColors( const QColor&, 
				       const QColor& = QColor(),
				       const GradientType = Vertical );
  GradientType      progressColors( QColor&, QColor& ) const;
  
  void              setProgressGradient( const QLinearGradient& );
  QLinearGradient   progressGradient() const;
  
  void              setOpacity( const double );
  double            opacity() const;
  
  void              setTextAlignment( const int );
  int               textAlignment() const;
  
  void              setTextColor( const QColor& );
  QColor            textColor() const;
  void              setTextColors( const QColor&, const QColor& = QColor() );
  void              textColors( QColor&, QColor& ) const;
  
  void              setConstantInfo( const QString& info );
  QString           constantInfo() const;

  QString           message() const;
  
  int               error() const;
  
  void              finish( QWidget* );
  void              repaint();
  
public slots:
  void              setMessage( const QString&, 
				const int,
				const QColor& = QColor() );
  void              setMessage( const QString& );
  void              clear();
  
protected:
  virtual void      mousePressEvent( QMouseEvent* );
  virtual void      customEvent( QEvent* );
  virtual void      paintEvent( QPaintEvent* );

  virtual void      drawContents( QPainter* );
  
  virtual void      drawProgressBar( QPainter* );
  virtual void      drawMessage( QPainter* );

private:
  void              drawContents();
  void              setError( const int );
  QString           fullMessage() const;

private:
  static QtxSplash* mySplash;
  
  QPixmap           myPixmap;           //!< splash pixmap
  QString           myInfo;             //!< constant info
  QString           myMessage;          //!< current status message
  int               myAlignment;        //!< text alignment flags (Qt::Alignment)
  QColor            myColor;            //!< text color
  QColor            myShadowColor;      //!< text shadow color
  bool              myHideOnClick;      //!< 'hide on click' flag
  int               myProgress;         //!< current progress
  int               myTotal;            //!< total progress steps
  QColor            myStartColor;       //!< progress bar gradient starting color
  QColor            myEndColor;         //!< progress bar gradient ending color
  GradientType      myGradientType;     //!< progress bar gradient direction
  QLinearGradient   myGradient;         //!< progress bar custom gradient
  int               myProgressWidth;    //!< progress bar width
  int               myProgressFlags;    //!< progress bar flags (QtxSplash::ProgressBarFlags)
  int               myMargin;           //!< margin (for progress bar and status message)
  double            myOpacity;          //!< progress bar / status message opacity
  int               myError;            //!< error code
  bool              myGradientUsed;     //!< 'use custom gradient color scale' flag
};

#endif
