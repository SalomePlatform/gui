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
// File:      Style_Tools.h
// Author:    Natalia Ermolaeva

#ifndef STYLE_TOOLS_HEADER
#define STYLE_TOOLS_HEADER

#include <Style.h>
#include <QPainterPath>
#include <QRect>
#include <QColor>
#include <QStyle>

class STYLE_SALOME_EXPORT Style_Tools
{
public:
  typedef enum { None        = 0x00000000,
                 All         = 0x00000001,
                 Right       = 0x00000002, 
                 Left        = 0x00000004,
                 TopLeft     = 0x00000008,
                 TopRight    = 0x00000010,
                 BottomLeft  = 0x00000020,
                 BottomRight = 0x00000040
                 } RoundType;
  typedef enum { WholePath, BottomPath, TopPath } ShadowType;

  typedef enum { SlUp, SlDown, SlLeft, SlRight, SlNone } SliderType;

  static QPainterPath painterPath( const QRect& r );
  static QPainterPath substractPath( const QPainterPath& fromPath, const QPainterPath& path );

  static QPainterPath roundRect( const QRect& r, const double rad, int type = 0,
                                 int shType = 0 );

  static void         roundRect( QPainter* p, const QRect& r, const double rad, const int type,
                                 const QColor& c1, const QColor& c2, bool fill = true,
                                 bool antial = true );
  static void         shadowRect( QPainter* p, const QRect& r, const double rad,
                                  const double marg, const int shad, int type,
                                  const QColor& light, const QColor& dark, const QColor& border_top,
                                  const QColor& border_bot, const bool antialize, const bool isButton,
                                  const bool btnOn = false, const bool fill = true );
  static void         shadowCheck( QPainter* p, const QRect& r, const double rad, const int type,
                                   const QColor& light, const QColor& dark,
                                   const QColor& border_top, const QColor& border_bot );
  static void         arrowRect( QPainter* p, const QRect& re, const QColor& frame,
                                 const QColor& gr1, const QColor& gr2 );
  static void         fillRect( QPainter* p, const QRect& re, const QColor& _c1,
                                const QColor& _c2, const int alpha = 255 );
  static void         drawArrow( QStyle::PrimitiveElement type, QPainter* p, const QRect& r,
                                 const QColor& pen, const QColor& brush );
  static QPainterPath tabRect( QPainter* p, const QRect& r, const int position, const double rad,
                               const double delta, const QColor& light, const QColor& dark,
                               const QColor& border_top, const QColor& border_bot,
                               const bool selected, const bool isLast, const bool isHover,
                               const bool focusRect = false, const bool draw = true );
  static void         drawFocus( QPainter* p, const QRect& aRect, const double rad, const int type,
                                 const QColor& border );
  static void         drawFocus( QPainter* p, const QPainterPath& path, const QColor& border,
                                 const bool& line = true );
  static void         drawSlider( QPainter* p, const QRect& r, const double rad,
                                  SliderType type, const QColor& light, const QColor& dark,
                                  const QColor& border_top, const QColor& border_bot );
  static void         highlightRect( QPainter* p, const QRect& rect, const double rad, const int type,
                                     const double marg, const QColor& center, const QColor& out_center,
                                     const QColor& border );
  static int          getMinDelta( const QRect& rect, QSize size, const int defDelta );
  static int          getMaxRect( const QRect& rect, const int defRect );
};

#endif
