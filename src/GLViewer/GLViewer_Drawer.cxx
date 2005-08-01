// File:      GLViewer_Drawer.cxx
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

//#include <GLViewerAfx.h>
#include "GLViewer_Drawer.h"
#include "GLViewer_Object.h"
#include "GLViewer_Text.h"
#include "GLViewer_ViewFrame.h"
#include "GLViewer_ViewPort2d.h"

#ifndef WIN32
#include <GL/glx.h>
#endif

#include <qimage.h>
#include <qpainter.h>

#define TEXT_GAP    5

GLboolean          TFLoaded = GL_FALSE;

GLdouble           modelMatrix[16], projMatrix[16];
GLint              viewport[4];
GLdouble           winx, winy, winz;
GLint              status;

GLViewer_TexFont*  staticGlFont;

//================================================================
// Class       : GLViewer_TexFont
// Description : 
//================================================================
//! code of first font symbol
static int FirstSymbolNumber = 32;
//! code of last font symbol
static int LastSymbolNumber = 127;

QMap<GLViewer_TexFindId,GLViewer_TexIdStored> GLViewer_TexFont::TexFontBase;
QMap<GLViewer_TexFindId,GLuint>               GLViewer_TexFont::BitmapFontCache; 

//=======================================================================
// Function: clearTextBases
// Purpose :
//=======================================================================
void GLViewer_TexFont::clearTextBases()
{
  //cout << "Clear font map" << endl;
  TexFontBase.clear();
  BitmapFontCache.clear();
}

//======================================================================
// Function: GLViewer_TexFont
// Purpose :
//=======================================================================
GLViewer_TexFont::GLViewer_TexFont()
{
    myQFont = QFont::defaultFont();
    QFontMetrics aFM( myQFont );        
    myWidths = new int[LastSymbolNumber - FirstSymbolNumber+1];
    myPositions = new int[LastSymbolNumber - FirstSymbolNumber+1];
    mySeparator = 2;
    for( int k = FirstSymbolNumber, aWidth = 0; k <= LastSymbolNumber; k++ )
    {
        myWidths[ k - FirstSymbolNumber ] = aFM.width( k );
        myPositions[ k - FirstSymbolNumber ] = aWidth;
        aWidth += myWidths[ k - FirstSymbolNumber ] + 2;//mySeparator;
    }

    myTexFontWidth = 0;
    myTexFontHeight = 0;        
}

//======================================================================
// Function: GLViewer_TexFont
// Purpose :
//=======================================================================
GLViewer_TexFont::GLViewer_TexFont( QFont* theFont, int theSeparator )
{
    myQFont = *theFont;
    QFontMetrics aFM( myQFont );        
    myWidths = new int[LastSymbolNumber - FirstSymbolNumber+1];
    myPositions = new int[LastSymbolNumber - FirstSymbolNumber+1];
    mySeparator = theSeparator;
    for( int k = FirstSymbolNumber, aWidth = 0; k <= LastSymbolNumber; k++ )
    {
        myWidths[ k - FirstSymbolNumber ] = aFM.width( k );
        myPositions[ k - FirstSymbolNumber ] = aWidth;
        aWidth += myWidths[ k - FirstSymbolNumber ] + 2;//mySeparator;
    }

    myTexFontWidth = 0;
    myTexFontHeight = 0;
    
}

//======================================================================
// Function: ~GLViewer_TexFont
// Purpose :
//=======================================================================
GLViewer_TexFont::~GLViewer_TexFont()
{
    delete[] myWidths;
    delete[] myPositions;
} 
  
//======================================================================
// Function: generateTexture
// Purpose :
//=======================================================================
void GLViewer_TexFont::generateTexture()
{
    QFontMetrics aFM( myQFont );

    GLViewer_TexFindId aFindFont;
    aFindFont.myFontString = myQFont.toString();
    aFindFont.myViewPortId = (int)QGLContext::currentContext();
        
    if( TexFontBase.contains( aFindFont ) )
    {
        GLViewer_TexIdStored aTexture = TexFontBase[ aFindFont ];
        myTexFont = aTexture.myTexFontId;
        myTexFontWidth = aTexture.myTexFontWidth;
        myTexFontHeight = aTexture.myTexFontHeight;
    }    
    else    
    {
        QString aStr;
        int pixelsWidth = 0;
        int pixelsHight = aFM.height();
        myTexFontWidth = 64;
        myTexFontHeight = 64;
    
        pixelsWidth = myWidths[LastSymbolNumber - FirstSymbolNumber] + 
                      myPositions[LastSymbolNumber - FirstSymbolNumber];

        while( myTexFontWidth < pixelsWidth )
            myTexFontWidth = myTexFontWidth * 2;
        while( myTexFontHeight < pixelsHight )
            myTexFontHeight = myTexFontHeight * 2;

        QPixmap aPixmap( myTexFontWidth, myTexFontHeight );
        aPixmap.fill( QColor( 0, 0, 0) );
        QPainter aPainter( &aPixmap );
        aPainter.setFont( myQFont );
        for( int l = 0/*, gap = 0*/; l < LastSymbolNumber - FirstSymbolNumber; l++  )
        {
            QString aLetter;
            aLetter += (char)(FirstSymbolNumber + l);
            aPainter.setPen( QColor( 255,255,255) );
            aPainter.drawText ( myPositions[l], pixelsHight, aLetter );
        }
    
        QImage aImage = aPixmap.convertToImage();
        char* pixels = new char[myTexFontWidth * myTexFontHeight * 2];

        for( int i = 0; i < myTexFontHeight; i++ )
        {            
            for( int j = 0; j < myTexFontWidth;  j++ )
            {
                int aRed = qRed( aImage.pixel( j, myTexFontHeight - i - 1 ) );
                int aGreen = qGreen( aImage.pixel( j, myTexFontHeight - i - 1 ) );
                int aBlue = qBlue( aImage.pixel( j, myTexFontHeight - i - 1 ) );
          
                if( aRed != 0 || aGreen != 0 || aBlue != 0 )
                {
                    pixels[i * myTexFontWidth * 2 + j * 2] = (GLubyte)( (aRed + aGreen + aBlue)/3 );
                    pixels[i * myTexFontWidth * 2 + j * 2 + 1]= (GLubyte) 255;
                }
                else
                {
                    pixels[i * myTexFontWidth * 2 + j * 2] = (GLubyte) 0;
                    pixels[i * myTexFontWidth * 2 + j * 2 + 1]= (GLubyte) 0;
                }                
            }
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &myTexFont);
        glBindTexture(GL_TEXTURE_2D, myTexFont);  
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexImage2D(GL_TEXTURE_2D, 0, 2, myTexFontWidth,
            myTexFontHeight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, pixels);
    
        delete[] pixels;
        
        GLViewer_TexIdStored aTexture;
        aTexture.myTexFontId = myTexFont;
        aTexture.myTexFontWidth = myTexFontWidth;
        aTexture.myTexFontHeight = myTexFontHeight;

        TexFontBase.insert( aFindFont, aTexture );
    }
}

//======================================================================
// Function: drawString
// Purpose :
//=======================================================================
void GLViewer_TexFont::drawString( QString theStr, GLdouble theX , GLdouble theY )
{
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPixelTransferi(GL_MAP_COLOR, 0);
    glAlphaFunc(GL_GEQUAL, 0.5F);
    glEnable(GL_ALPHA_TEST);
    glBindTexture(GL_TEXTURE_2D, myTexFont);
    glBegin(GL_QUADS);

    QFontMetrics aFM( myQFont );
    int pixelsHeight = aFM.height();

    theY = theY - myTexFontHeight + pixelsHeight;

    for( int i = 0, aGap = 0; i < theStr.length(); i++ )
    {
        char aLetter = theStr.data()[i];
        int aLettIndex = (int)aLetter - FirstSymbolNumber;

        float aLettBegin = (float)myPositions[aLettIndex];
        float aLettEnd = aLettBegin + myWidths[aLettIndex]-1;

        aLettBegin = aLettBegin / myTexFontWidth;
        aLettEnd = aLettEnd / myTexFontWidth;

        glTexCoord2f( aLettBegin, 0.0 ); glVertex3f( theX + aGap, theY, 1.0 );
        glTexCoord2f( aLettBegin, 1.0 ); glVertex3f( theX + aGap, theY + myTexFontHeight, 1.0 );
        glTexCoord2f( aLettEnd, 1.0 ); glVertex3f( theX + aGap + myWidths[aLettIndex]-1, theY + myTexFontHeight, 1.0 );
        glTexCoord2f( aLettEnd, 0.0 ); glVertex3f( theX + aGap + myWidths[aLettIndex]-1, theY, 1.0 );

        aGap += myWidths[aLettIndex]-1 + mySeparator;
    }

    glEnd();
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
}

//======================================================================
// Function: getStringWidth
// Purpose :
//=======================================================================
int GLViewer_TexFont::getStringWidth( QString theStr )
{
    int aWidth = 0;
    for( int i = 0; i < theStr.length(); i ++ )
    {
        char aLetter = theStr.data()[i];
        int aLettIndex = (int)aLetter - FirstSymbolNumber;
        aWidth += myWidths[aLettIndex] + mySeparator;
    }

    return aWidth;
}

//======================================================================
// Function: getStringHeight
// Purpose :
//=======================================================================
int GLViewer_TexFont::getStringHeight()
{
    QFontMetrics aFM( myQFont );
    return aFM.height();
}

//! function for generation list base for bitmap fonts
static GLuint displayListBase( QFont* theFont )
{
  GLuint aList = 0;
  //static QMap<GLViewer_TexFindId, GLuint> fontCache;
  GLViewer_TexFindId aFindFont;
  aFindFont.myFontString = theFont->toString();

#ifdef WIN32
  HGLRC ctx = ::wglGetCurrentContext();
  if ( !ctx )
    return aList;  
  
  aFindFont.myViewPortId = (int)ctx;

  if ( GLViewer_TexFont::BitmapFontCache.contains( aFindFont ) )
    aList = GLViewer_TexFont::BitmapFontCache[aFindFont];
  else
  {
    GLuint listBase = 0;
    QMap<GLViewer_TexFindId, GLuint>::iterator it = GLViewer_TexFont::BitmapFontCache.begin();
    for ( ; it != GLViewer_TexFont::BitmapFontCache.end(); ++it )
    {
      if ( it.key().myViewPortId == (int)ctx && it.data() > listBase )
        listBase = it.data();
    }
    listBase += 256;

    HDC glHdc = ::wglGetCurrentDC();
    ::SelectObject( glHdc, theFont->handle() );
    if ( !::wglUseFontBitmaps( glHdc, 0, 256, listBase ) )
      listBase = 0;
    aList = listBase;
    GLViewer_TexFont::BitmapFontCache[aFindFont] = aList;
  }
#else //X Window
  Display* aDisp = glXGetCurrentDisplay();
  if( !aDisp )
  {
#ifdef _DEBUG_
    printf( "Can't find current dislay\n" );
#endif
    return aList;
  }
  
  GLXContext aCont = glXGetCurrentContext();
  if( !aCont )
  {
#ifdef _DEBUG_
    printf( "Can't find current context\n" );
#endif
    return aList;
  }

  aFindFont.myViewPortId = (int)aCont;

  if ( GLViewer_TexFont::BitmapFontCache.contains( aFindFont ) )
    aList = GLViewer_TexFont::BitmapFontCache[aFindFont];
  else
  {
    GLuint listBase = 0;
    QMap<GLViewer_TexFindId, GLuint>::iterator it = GLViewer_TexFont::BitmapFontCache.begin();
    for ( ; it != GLViewer_TexFont::BitmapFontCache.end(); ++it )
    {
      if ( it.key().myViewPortId == (int)aCont && it.data() > listBase )
        listBase = it.data();
    }
    listBase += 256;
    
    //glXUseXFont( (Font)(theFont->handle()), 0, 256, listBase );
    int aFontCont = 0;
    char** xFontList = XListFonts( aDisp, aFindFont.myFontString.data(), 1, &aFontCont  );
    if( !theFont->handle() )
    {       
#ifdef _DEBUG_
      printf( "Can't load font %s. loading default font....\n", aFindFont.myFontString.data() );
#endif
      QString aFontMask ("-*-*-*-r-*-*-");
      aFontMask += aFindFont.myFontString.section( ',', 1, 1 );
#ifdef _DEBUG_
      printf( "Height of Default font: %s\n", aFindFont.myFontString.section( ',', 1, 1 ).data() );
#endif
      aFontMask += "-*-*-*-m-*-*-*";
      xFontList = XListFonts( aDisp, aFontMask.data()/*"-*-*-*-r-*-*-12-*-*-*-m-*-*-*"*/, 1, &aFontCont  );
      if( aFontCont == 0 )
      {      
#ifdef _DEBUG_
        printf( "Can't load default font\n" );
#endif
        return 0;
      }
      glXUseXFont( (Font)(XLoadFont( aDisp,xFontList[0] )), 0, 256, listBase );
    }
    else
      glXUseXFont( (Font)(theFont->handle()), 0, 256, listBase );
    
    aList = listBase;
    GLViewer_TexFont::BitmapFontCache[aFindFont] = aList;
  }

#endif

  return aList;
}

/***************************************************************************
**  Class:   GLViewer_Drawer
**  Descr:   Drawer for GLViewer_Object
**  Module:  GLViewer
**  Created: UI team, 01.10.01
****************************************************************************/
//======================================================================
// Function: GLViewer_Drawer
// Purpose :
//=======================================================================
GLViewer_Drawer::GLViewer_Drawer()
{
  myXScale = myYScale = 0.0;
  myObjects.clear();
  myTextList = 0/*-1*/;
  myObjectType = "GLViewer_Object";
  myPriority = 0;
}

//======================================================================
// Function: ~GLViewer_Drawer
// Purpose :
//=======================================================================
GLViewer_Drawer::~GLViewer_Drawer()
{
  myObjects.clear();
  glDeleteLists( myTextList, 1 );
}

//======================================================================
// Function: destroyAllTextures
// Purpose :
//=======================================================================
void GLViewer_Drawer::destroyAllTextures()
{
    QMap<GLViewer_TexFindId,GLViewer_TexIdStored>::Iterator anIt= GLViewer_TexFont::TexFontBase.begin();
    QMap<GLViewer_TexFindId,GLViewer_TexIdStored>::Iterator anEndIt= GLViewer_TexFont::TexFontBase.end();

    for( ; anIt != anEndIt; anIt++ )
        glDeleteTextures( 1, &(anIt.data().myTexFontId) );
}

//=======================================================================
// Function: setAntialiasing
// Purpose : The function enables and disables antialiasing in Open GL (for points, lines and polygons).
//=======================================================================
void GLViewer_Drawer::setAntialiasing(const bool on)
{
	if (on)
	{
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		glEnable (GL_BLEND);
	}
	else
	{
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH);
		glBlendFunc (GL_ONE, GL_ZERO);
		glDisable (GL_BLEND);
	}
}

//======================================================================
// Function: loadTexture
// Purpose :
//=======================================================================
GLuint GLViewer_Drawer::loadTexture( const QString& fileName )
{
    QImage buf;
    if ( fileName.isEmpty() || !buf.load( fileName ) )
        return 0;

    int w = buf.width();
    int h = buf.height();

    int size = 16;
    while( size < w || size < h )
        size = size * 2;

    GLuint texture;
    GLubyte* pixels = new GLubyte[ size * size * 4 ];

    for( int i = 0; i < size; i++ )
    {            
        for( int j = 0; j < size; j++ )
        {
            GLubyte r, g, b, a;
            if( j < w && i < h )
            {
                QRgb pixel = buf.pixel( j, h - i - 1 );
                r = (GLubyte)qRed( pixel );
                g = (GLubyte)qGreen( pixel );
                b = (GLubyte)qBlue( pixel );
                a = (GLubyte)qAlpha( pixel );
            }
            else
            {
                r = (GLubyte)255;
                g = (GLubyte)255;
                b = (GLubyte)255;
                a = (GLubyte)255;
            }

            int index = 4 * ( i * size + j );
            pixels[ index ] = r;
            pixels[ index + 1 ] = g;
            pixels[ index + 2 ] = b;
            pixels[ index + 3 ] = a;
        }
    }

    //initialize texture
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    delete[] pixels;

    return texture;
}

//======================================================================
// Function: drawTexture
// Purpose :
//=======================================================================
void GLViewer_Drawer::drawTexture( GLuint texture, GLint size, GLfloat x, GLfloat y )
{
    if( !texture )
        return;

    glColor4f( 1.0, 1.0, 1.0, 1.0 );

    glEnable( GL_TEXTURE_2D );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glAlphaFunc( GL_GREATER, 0.95F );
    glEnable( GL_ALPHA_TEST );
    
    glBindTexture( GL_TEXTURE_2D, texture );
    glBegin( GL_QUADS );

    glTexCoord2f( 0.0, 0.0 );
    glVertex3f( x-size/2., y-size/2., 0.0 );

    glTexCoord2f( 0.0, 1.0 );
    glVertex3f( x-size/2., y+size/2., 0.0 );

    glTexCoord2f( 1.0, 1.0 );
    glVertex3f( x+size/2., y+size/2., 0.0 );

    glTexCoord2f( 1.0, 0.0 );
    glVertex3f( x+size/2., y-size/2., 0.0 );
    
    glEnd();
    glFlush();

    glDisable( GL_ALPHA_TEST );
    glDisable( GL_TEXTURE_2D );
}

//======================================================================
// Function: drawText
// Purpose :
//=======================================================================
void GLViewer_Drawer::drawText( const QString& text, GLfloat xPos, GLfloat yPos,
                                const QColor& color, QFont* theFont, int theSeparator, DisplayTextFormat theFormat )
{
  if( theFormat == DTF_TEXTURE )
  {
    GLViewer_TexFont aTexFont( theFont, theSeparator );
    aTexFont.generateTexture();

    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);
    status = gluProject (xPos, yPos, 0, modelMatrix, projMatrix, viewport, &winx, &winy, &winz);

    glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT | GL_LIST_BIT );
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,viewport[2],0,viewport[3],-100,100);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f( ( GLfloat )color.red() / 255, 
               ( GLfloat )color.green() / 255, 
               ( GLfloat )color.blue() / 255 );

    aTexFont.drawString( text, winx, winy );

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
  }
  else if( theFormat == DTF_BITMAP )
  {
    glColor3f( ( GLfloat )color.red() / 255, 
               ( GLfloat )color.green() / 255, 
               ( GLfloat )color.blue() / 255 );
    glRasterPos2f( xPos, yPos );
    glListBase( displayListBase( theFont ) );
    glCallLists( text.length(), GL_UNSIGNED_BYTE, text.local8Bit().data() );
  }
}

//======================================================================
// Function: drawText
// Purpose :
//=======================================================================
void GLViewer_Drawer::drawText( GLViewer_Object* theObject )
{
  if( !theObject )
    return;

  GLViewer_Text* aText = theObject->getGLText();
  if( !aText )
    return;

  GLfloat aPosX, aPosY;
  aText->getPosition( aPosX, aPosY );
  // get temporary copy of font
  QFont aTmpVarFont = aText->getFont();
  drawText( aText->getText(), aPosX, aPosY, aText->getColor(), &aTmpVarFont, aText->getSeparator(), aText->getDisplayTextFormat() );
}

//======================================================================
// Function: drawGLText
// Purpose :
//=======================================================================
void GLViewer_Drawer::drawGLText( QString text, float x, float y,
                                  int hPosition, int vPosition, QColor color, bool smallFont )
{
  QFont aFont( "Arial", 10, QFont::Bold );
  if( smallFont )
    aFont.setPointSize( 8 );

  QFontMetrics aFontMetrics( aFont );
  float width = aFontMetrics.width( text ) / myXScale;
  float height = aFontMetrics.height() / myXScale;
  float gap = 5 / myXScale;

  switch( hPosition )
  {
      case GLText_Left   : x -= ( gap + width ); break;
      case GLText_Center : x -= width / 2; break;
      case GLText_Right  : x += gap; break;
      default : break;
  }

  switch( vPosition )
  {
      case GLText_Top    : y += height * 0.5; break;
      case GLText_Center : y -= height * 0.5; break;
      case GLText_Bottom : y -= height * 1.5; break;
      default : break;
  }

  drawText( text, x, y, color, &aFont, 2, DTF_BITMAP ); // DTF_BITMAP or DTF_TEXTURE
}

//======================================================================
// Function: drawRectangle
// Purpose :
//=======================================================================
void GLViewer_Drawer::drawRectangle( GLViewer_Rect* rect, QColor color )
{
  if( !rect )
    return;

  float x1 = rect->left();
  float x2 = rect->right();
  float y1 = rect->bottom();
  float y2 = rect->top();
  
  glColor3f( ( GLfloat )color.red() / 255,
    ( GLfloat )color.green() / 255,
    ( GLfloat )color.blue() / 255 );
  glLineWidth( 1.0 );
  
  glBegin( GL_LINE_LOOP );
  glVertex2f( x1, y1 );
  glVertex2f( x1, y2 );
  glVertex2f( x2, y2 );
  glVertex2f( x2, y1 );
  glEnd();
}

//======================================================================
// Function: translateToHPGL
// Purpose :
//=======================================================================
bool GLViewer_Drawer::translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS )
{
    bool result = true;
    for( int i=0, n=myObjects.count(); i<n; i++ ) 
        result &= myObjects[i]->translateToHPGL( hFile, aViewerCS, aHPGLCS );
    return result;
}

//======================================================================
// Function: translateToPS
// Purpose :
//=======================================================================
bool GLViewer_Drawer::translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS )
{
    bool result = true;
    for( int i=0, n=myObjects.count(); i<n; i++ ) 
        result &= myObjects[i]->translateToPS( hFile, aViewerCS, aPSCS );
    return result;
}

#ifdef WIN32
//======================================================================
// Function: translateToEMF
// Purpose :
//=======================================================================
bool GLViewer_Drawer::translateToEMF( HDC hDC, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS )
{
    bool result = true;
    for( int i=0, n=myObjects.count(); i<n; i++ ) 
        result &= myObjects[i]->translateToEMF( hDC, aViewerCS, aEMFCS );
    return result;
}
#endif
