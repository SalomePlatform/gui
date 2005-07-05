// File:      GLViewer_Compass.cxx
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

//================================================================
// Class       : GLViewer_Compass
// Description : Class implement representatiof of compass in one of corner in GLViewer
//================================================================

#include "GLViewer_Compass.h"
#include "GLViewer_Drawer.h"

//=======================================================================
// Function: GLViewer_Compass
// Purpose :
//=======================================================================
GLViewer_Compass::GLViewer_Compass ( const QColor& color, const int size, const Position pos,
                               const int WidthTop, const int WidthBottom, const int HeightTop,
                               const int HeightBottom )
{
    myCol = color;
    mySize = size;
    myPos = pos;
    myArrowWidthTop = WidthTop;
    myArrowWidthBottom = WidthBottom;
    myArrowHeightTop = HeightTop;
    myArrowHeightBottom = HeightBottom;
    myIsVisible = true;
    QFont* aFont = new QFont("Times",16);
    myFont = new GLViewer_TexFont( aFont );
    isGenereted = false;
    //myFont->generateTexture();
}

//=======================================================================
// Function: ~GLViewer_Compass
// Purpose :
//=======================================================================
GLViewer_Compass::~GLViewer_Compass()
{
    delete myFont;
}

//=======================================================================
// Function: setCompass
// Purpose :
//=======================================================================
void GLViewer_Compass::setCompass( const QColor& color, const int size, const Position pos )
{
  myCol = color;
  mySize = size;
  myPos = pos;
}

//=======================================================================
// Function: setVisible
// Purpose :
//=======================================================================
void GLViewer_Compass::setVisible( const bool vis )
{
  myIsVisible = vis;
}

//=======================================================================
// Function: setSize
// Purpose :
//=======================================================================
void GLViewer_Compass::setSize( const int size )
{
  if( size > 0 )
    mySize=size;
}

//=======================================================================
// Function: setArrowWidthTop
// Purpose :
//=======================================================================
void GLViewer_Compass::setArrowWidthTop( const int WidthTop )
{
  if( WidthTop < myArrowWidthBottom || WidthTop > mySize )
    return;
  myArrowWidthTop=WidthTop;
}

//=======================================================================
// Function: setArrowWidthBottom
// Purpose :
//=======================================================================
void GLViewer_Compass::setArrowWidthBottom( const int WidthBot )
{ 
  if( WidthBot > myArrowWidthTop || WidthBot < 1 )
    return;	
  myArrowWidthBottom=WidthBot;
}

//=======================================================================
// Function: setArrowHeightTop
// Purpose :
//=======================================================================
void GLViewer_Compass::setArrowHeightTop( const int HeightTop )
{
  if( HeightTop > (2*mySize-myArrowHeightBottom ) || HeightTop < 1 )
    return;
  myArrowHeightTop=HeightTop;
}

//=======================================================================
// Function: setArrowHeightBottom
// Purpose :
//=======================================================================
void GLViewer_Compass::setArrowHeightBottom( const int HeightBot )
{
  if( HeightBot > ( 2*mySize-myArrowHeightTop ) || HeightBot < 1)
    return;
  myArrowHeightBottom=HeightBot;
}

//=======================================================================
// Function: getFont
// Purpose :
//=======================================================================
GLViewer_TexFont* GLViewer_Compass::getFont()
{ 
    if(!isGenereted) 
    {
        myFont->generateTexture();
        isGenereted = true;
    }    
    return myFont;
}

//=======================================================================
// Function: setFont
// Purpose :
//=======================================================================
void GLViewer_Compass::setFont( QFont theFont )
{
    delete myFont;
    myFont = new GLViewer_TexFont( &theFont );
} 
