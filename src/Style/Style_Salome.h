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
// File:      Style_Salome.h
// Author:    Natalia Ermolaeva

#ifndef STYLE_SALOME_HEADER
#define STYLE_SALOME_HEADER

#include <Style.h>

#include <QWindowsStyle>
#include <QStylePlugin>
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
public:
  Style_Salome();
  virtual ~Style_Salome();

  Style_Model* getModel() { return myModel; }

  void         updateSettings( QApplication* app );

  virtual void polish( QApplication* app );
  virtual void polish( QWidget *widget );
  virtual void unpolish( QWidget *widget );
  virtual void drawComplexControl( ComplexControl, const QStyleOptionComplex*,
                                   QPainter*, const QWidget* = 0 ) const;

  virtual void drawControl( ControlElement, const QStyleOption*, QPainter*, const QWidget* ) const;
  virtual void drawPrimitive( PrimitiveElement, const QStyleOption*,
                                   QPainter*, const QWidget* = 0 ) const;
  virtual int pixelMetric ( PixelMetric metric, const QStyleOption* opt = 0,
                            const QWidget* w = 0 ) const;
  virtual QSize sizeFromContents ( ContentsType ct, const QStyleOption* opt,
                                   const QSize& contentsSize, const QWidget* w = 0 ) const;
  virtual QPixmap standardPixmap(StandardPixmap stPixmap, const QStyleOption *opt,
                         const QWidget *widget = 0) const;
  virtual int styleHint ( StyleHint hint, const QStyleOption * option = 0,
                          const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const;
  virtual QRect subControlRect( ComplexControl, const QStyleOptionComplex*,
                                SubControl, const QWidget* = 0 ) const;
  virtual QRect subElementRect( SubElement, const QStyleOption*, const QWidget* = 0 ) const;

private:
  void          updatePaletteColors();
  void          updateAllWidgets( QApplication* app );
  bool          hasHover() const;
  void          drawHoverRect( QPainter* p, const QRect& r, const double rad, const int type,
                               const bool border  ) const;
  void          drawHandle( QPainter* p, const QRect& r, bool horiz, bool isRect = true ) const;
  void          drawBackground( QPainter* p, const QRect& r, const bool fill,
                                const bool grad = false, const bool horiz = true ) const;
  void          drawBorder( QPainter* p, const QRect& r, bool horiz ) const;

  QColor        getColor( int id ) const;
  double        getDblValue( int id ) const;
  int           getIntValue( int id ) const;
  bool          getBoolValue( int id ) const;
  QString       getStringValue( int id ) const;
  QString       titleText( const QString&, const int, const int, QFont& f ) const;

private:
  Style_Model*  myModel;
};

#endif
