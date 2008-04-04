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
// File   : Style_Tools.h
// Author : Natalia Ermolaeva, Open CASCADE S.A.S.
//

#ifndef STYLE_TOOLS_H
#define STYLE_TOOLS_H

#include "Style.h"
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

  static QPainterPath painterPath( const QRect& );
  static QPainterPath substractPath( const QPainterPath&, const QPainterPath& );

  static QPainterPath roundRect( const QRect&, const double, int = 0, int = 0 );

  static void         roundRect( QPainter*, const QRect&, const double, const int,
                                 const QColor&, const QColor&, bool = true, bool = true );

  static void         shadowRect( QPainter*, const QRect&, const double,
                                  const double, const int, int,
                                  const QColor&, const QColor&, const QColor&,
                                  const QColor&, const bool, const bool,
                                  const bool = false, const bool = true );
  static void         shadowCheck( QPainter*, const QRect&, const double, const int,
                                   const QColor&, const QColor&,
                                   const QColor&, const QColor& );
  static void         arrowRect( QPainter*, const QRect&, const QColor&,
                                 const QColor&, const QColor& );
  static void         fillRect( QPainter*, const QRect&, const QColor&,
                                const QColor&, const int = 255 );
  static void         drawArrow( QStyle::PrimitiveElement, QPainter*, const QRect&,
                                 const QColor&, const QColor& );
  static void         drawSign( QStyle::PrimitiveElement, QPainter*, const QRect&,
				const QColor&, const QColor& );
  static QPainterPath tabRect( QPainter*, const QRect&, const int, const double,
                               const double, const QColor&, const QColor&,
                               const QColor&, const QColor&,
                               const bool, const bool, const bool,
                               const bool = false, const bool = true );
  static void         drawFocus( QPainter*, const QRect&, const double, const int,
                                 const QColor& );
  static void         drawFocus( QPainter*, const QPainterPath&, const QColor&,
                                 const bool = true );
  static void         drawSlider( QPainter*, const QRect&, const double,
                                  SliderType, const QColor&, const QColor&,
                                  const QColor&, const QColor& );
  static void         highlightRect( QPainter*, const QRect&, const double, const int,
                                     const double, const QColor&, const QColor&,
                                     const QColor& );
  static int          getMinDelta( const QRect&, const QSize&, const int );
  static int          getMaxRect( const QRect&, const int );
};

#endif // STYLE_TOOLS_H
