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
// File   : Style_Salome.h
// Author : Natalia Ermolaeva, Open CASCADE S.A.S.
//

#ifndef STYLE_SALOME_H
#define STYLE_SALOME_H

#include "Style.h"

#include <QWindowsStyle>
#include <QMap>

class Style_Model;

class QApplication;
class QWidget;
class QStyleOption;
class QPainter;
class QSize;
class QVariant;

class STYLE_SALOME_EXPORT Style_Salome : public QWindowsStyle
{
  Q_OBJECT
public:
  Style_Salome();
  virtual ~Style_Salome();

  Style_Model*    getModel() const;

  void            updateSettings( QApplication* );

  virtual void    polish( QApplication* );
  virtual void    polish( QWidget* );
  virtual void    unpolish( QWidget* );
  virtual void    drawComplexControl( ComplexControl, const QStyleOptionComplex*,
				      QPainter*, const QWidget* = 0 ) const;
  
  virtual void    drawControl( ControlElement, const QStyleOption*, QPainter*, const QWidget* ) const;
  virtual void    drawPrimitive( PrimitiveElement, const QStyleOption*,
				 QPainter*, const QWidget* = 0 ) const;
  virtual int     pixelMetric( PixelMetric, const QStyleOption* = 0,
			       const QWidget* = 0 ) const;
  virtual QSize   sizeFromContents ( ContentsType, const QStyleOption*,
				     const QSize&, const QWidget* = 0 ) const;
  virtual QPixmap standardPixmap( StandardPixmap, const QStyleOption*,
				  const QWidget* = 0) const;
  virtual int     styleHint( StyleHint, const QStyleOption* = 0,
			     const QWidget* = 0, QStyleHintReturn* = 0 ) const;
  virtual QRect   subControlRect( ComplexControl, const QStyleOptionComplex*,
				  SubControl, const QWidget* = 0 ) const;
  virtual QRect   subElementRect( SubElement, const QStyleOption*, const QWidget* = 0 ) const;

protected slots:
  QIcon standardIconImplementation( StandardPixmap, const QStyleOption* = 0,
				    const QWidget* = 0 ) const;

private:
  void            updatePaletteColors();
  void            updateAllWidgets( QApplication* );
  bool            hasHover() const;
  void            drawHoverRect( QPainter*, const QRect&, const QColor&, const double,
                                 const int, const bool ) const;
  void            drawHandle( QPainter*, const QRect&, bool, bool = true ) const;
  void            drawBackground( QPainter*, const QRect&, const QColor&, const bool,
				  const bool = false, const bool = true ) const;
  void            drawBorder( QPainter*, const QRect&, bool ) const;

  QColor          getColor( int ) const;
  double          getDblValue( int ) const;
  int             getIntValue( int ) const;
  bool            getBoolValue( int ) const;
  QString         getStringValue( int ) const;
  QString         titleText( const QString&, const int, const int, QFont& ) const;

private:
  Style_Model*    myModel;
  QColor          myTTipWnd, myTTipText;
};

#endif // STYLE_SALOME_H
