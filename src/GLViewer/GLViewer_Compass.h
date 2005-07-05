// File:      GLViewer_Compass.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_COMPASS_H
#define GLVIEWER_COMPASS_H

#include "GLViewer.h"

#include <qcolor.h>

#ifdef WNT
#pragma warning( disable:4251 )
#endif

/*! Class GLViewer_Compass
 * Class implement representatiof of compass in one of corner in GLViewer.
 * Compass it's a some round visual pointer rotated with view window and 
 * located in fixed place (in one fixed corner)
 */

class GLViewer_TexFont;

class GLVIEWER_API GLViewer_Compass
{
public:
  //! Position of compass
  enum Position
  {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
  };

  //! A constructor
  /*!
  *\param color        - a color of compass
  *\param size         - a diameter of compass
  *\param pos          - a position
  *\param WidthTop     - width of wide part of arrow
  *\param WidthBottom  - width of base part of arrow
  *\param HeightTop    - height of arrow header
  *\param HeightBottom - height of arrow cut on bottom
  */
  GLViewer_Compass( const QColor& color = QColor ( 0, 255, 0 ), 
                   const int size = 60,
                   const Position pos = TopRight,
                   const int WidthTop = 20,
                   const int WidthBottom = 10,
                   const int HeightTop = 25,
                   const int HeightBottom = 7 );
  //! A destructor
  ~GLViewer_Compass();

  //! A function installing main parameters of compass
  void              setCompass( const QColor& color, const int size, const Position pos );
  //! A function installing a visible status
  void              setVisible( const bool vis = true );
  //! Returns visible status
  bool              getVisible(){ return myIsVisible; };
  
  //! A function installing diameter
  void              setSize( const int size );
  //! Returns diameter
  int               getSize(){ return mySize; }
  
  //! A function installing position
  void              setPos( const Position pos ){ myPos = pos; }
  //! Returns position
  int               getPos(){ return myPos; }
  
  //! A function installing color
  void              setColor( const QColor& color ){ myCol = color; }
  //! Returns color
  QColor            getColor(){ return myCol; }

  //! A function installing width of wide part of arrow
  void              setArrowWidthTop( const int WidthTop );
  //! Returns width of wide part of arrow
  int               getArrowWidthTop(){ return myArrowWidthTop; }
  
  //! A function installing width of base part of arrow
  void              setArrowWidthBottom( const int WidthBot );
  //! Returns width of base part of arrow
  int               getArrowWidthBottom(){return myArrowWidthBottom;};

  //! A function installing height of arrow header
  void              setArrowHeightTop( const int HeightTop );
  //! Returns height of arrow header
  int               getArrowHeightTop(){return myArrowHeightTop;};
  
  //! A function installing height of arrow cut on bottom
  void              setArrowHeightBottom( const int HeightBot );
  //! Returns height of arrow cut on bottom
  int               getArrowHeightBottom(){return myArrowHeightBottom;};        
  
  //! A function installing
  void              setFont( QFont theFont );
  //! Returns
  GLViewer_TexFont* getFont();

protected:
  //! Compass color
  QColor             myCol;
  //! Compass diameter
  int                mySize;
  //! Compass position
  int                myPos;
  //! Compass invisible status
  bool               myIsVisible;
  //! Width of wide part of arrow
  int                myArrowWidthTop;
  //! Width of base part of arrow
  int                myArrowWidthBottom;
  //! Height of arrow header
  int                myArrowHeightTop;
  //! Height of arrow cut on bottom
  int                myArrowHeightBottom;
  //! Compass font
  GLViewer_TexFont*  myFont;
  //! Status of generated font
  bool               isGenereted;
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
