// Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

#include "VTKViewer_OpenGLRenderer.h"
#include "VTKViewer_Texture.h"

#include <vtkCuller.h>
#include <vtkLightCollection.h>
#include <vtkObjectFactory.h>
#include <vtkOpenGLCamera.h>
#include <vtkOpenGLLight.h>
#include <vtkOpenGLProperty.h>
#include <vtkRenderWindow.h>
#ifndef VTK_OPENGL2
#include <vtkOpenGLExtensionManager.h>
#include <vtkgl.h> // vtkgl namespace
#endif
#include <vtkImageImport.h>
#include <vtkPNGWriter.h>
#include <vtkOpenGLTexture.h>
#include <vtkTimerLog.h>
#include <vtkObjectFactory.h>

#include <Basics_Utils.hxx>

vtkStandardNewMacro(VTKViewer_OpenGLRenderer)

VTKViewer_OpenGLRenderer::VTKViewer_OpenGLRenderer()
{
  this->GradientType = HorizontalGradient;

#ifdef VTK_OPENGL2
  this->BackgroundProgram        = 0;
  this->BackgroundVertexShader   = 0;
  this->BackgroundFragmentShader = 0;
  this->VertexArrayObject        = 0;

  this->OpenGLHelper.Init();
#endif
}

VTKViewer_OpenGLRenderer::~VTKViewer_OpenGLRenderer()
{
}

void VTKViewer_OpenGLRenderer::SetGradientType( const int theGradientType )
{
  this->GradientType = theGradientType;
}

void VTKViewer_OpenGLRenderer::GetCornersColor(Vec2D& cornersColor)
{
  if( this->GradientBackground )
  {
    double* corner1 = 0;
    double* corner2 = 0;
    double* corner3 = 0;
    double* corner4 = 0;
    double dcorner1[3];
    double dcorner2[3];

    switch( this->GradientType )
    {
    case HorizontalGradient:
      corner1 = this->Background;
      corner2 = this->Background;
      corner3 = this->Background2;
      corner4 = this->Background2;
      break;
    case VerticalGradient:
      corner1 = this->Background;
      corner2 = this->Background2;
      corner3 = this->Background2;
      corner4 = this->Background;
      break;
    case FirstDiagonalGradient:
      corner1 = this->Background;
      corner3 = this->Background2;
      dcorner1[0] = dcorner2[0] = 0.5F * ( corner1[0] + corner3[0] );
      dcorner1[1] = dcorner2[1] = 0.5F * ( corner1[1] + corner3[1] );
      dcorner1[2] = dcorner2[2] = 0.5F * ( corner1[2] + corner3[2] );
      corner2 = dcorner1;
      corner4 = dcorner2;
      break;
    case SecondDiagonalGradient:
      corner2 = this->Background2;
      corner4 = this->Background;
      dcorner1[0] = dcorner2[0] = 0.5F * ( corner2[0] + corner4[0] );
      dcorner1[1] = dcorner2[1] = 0.5F * ( corner2[1] + corner4[1] );
      dcorner1[2] = dcorner2[2] = 0.5F * ( corner2[2] + corner4[2] );
      corner1 = dcorner1;
      corner3 = dcorner2;
      break;
    case FirstCornerGradient:
      corner1 = this->Background2;
      corner2 = this->Background2;
      corner3 = this->Background;
      corner4 = this->Background2;
      break;
    case SecondCornerGradient:
      corner1 = this->Background2;
      corner2 = this->Background2;
      corner3 = this->Background2;
      corner4 = this->Background;
      break;
    case ThirdCornerGradient:
      corner1 = this->Background;
      corner2 = this->Background2;
      corner3 = this->Background2;
      corner4 = this->Background2;
      break;
    case FourthCornerGradient:
      corner1 = this->Background2;
      corner2 = this->Background;
      corner3 = this->Background2;
      corner4 = this->Background2;
      break;
    default: // just in case
      corner1 = this->Background;
      corner2 = this->Background;
      corner3 = this->Background;
      corner4 = this->Background;
      break;
    }
    cornersColor[0][0] = corner1[0]; cornersColor[0][1] = corner1[1]; cornersColor[0][2] = corner1[2];
    cornersColor[1][0] = corner2[0]; cornersColor[1][1] = corner2[1]; cornersColor[1][2] = corner2[2];
    cornersColor[2][0] = corner3[0]; cornersColor[2][1] = corner3[1]; cornersColor[2][2] = corner3[2];
    cornersColor[3][0] = corner4[0]; cornersColor[3][1] = corner4[1]; cornersColor[3][2] = corner4[2];
  }
  else
  {
    for (int i = 0; i < 4; i++)
      for(int j = 0; j < 3; j++)
        cornersColor[i][j] = this->Background[j];
  }
}

void VTKViewer_OpenGLRenderer::Clear(void)
{
  vtkOpenGLRenderer::Clear();
#ifdef VTK_OPENGL2
  if (this->OpenGLHelper.IsInitialized())
  {
    if (this->BackgroundProgram == 0)
    {
#if defined(WIN32) && defined(UNICODE)
      std::wstring wFilePath = std::wstring( _wgetenv(L"GUI_ROOT_DIR") ) + L"/share/salome/resources/gui/Background";
      std::string filePath = Kernel_Utils::utf8_encode_s( wFilePath );
#else
      std::string filePath = std::string(getenv("GUI_ROOT_DIR")) + "/share/salome/resources/gui/Background";
#endif

      if (!this->OpenGLHelper.CreateShaderProgram (filePath,
                                                   this->BackgroundProgram,
                                                   this->BackgroundVertexShader,
                                                   this->BackgroundFragmentShader))
      {
        return;
      }
      // Get uniform locations.
      this->OpenGLHelper.vglUseProgramObjectARB (this->BackgroundProgram);

      this->myLocations.UseTexture        = this->OpenGLHelper.vglGetUniformLocationARB (this->BackgroundProgram, "uUseTexture");
      this->myLocations.BackgroundTexture = this->OpenGLHelper.vglGetUniformLocationARB (this->BackgroundProgram, "uBackgroundTexture");

      this->OpenGLHelper.vglUseProgramObjectARB (0);
    }
  }
#endif

  GLbitfield clear_mask = 0;

  if( !this->Transparent() )
  {
    glClearColor( static_cast<GLclampf>(this->Background[0]),
                  static_cast<GLclampf>(this->Background[1]),
                  static_cast<GLclampf>(this->Background[2]),
                  static_cast<GLclampf>(this->BackgroundAlpha));
    clear_mask |= GL_COLOR_BUFFER_BIT;
  }

  if( !this->GetPreserveDepthBuffer() )
  {
    glClearDepth(static_cast<GLclampf>(1.0));
    clear_mask |= GL_DEPTH_BUFFER_BIT;
  }

  vtkDebugMacro(<< "glClear\n");
  glClear(clear_mask);

  // If gradient background is turned on, draw it now.
  if( !this->Transparent() &&
      ( this->GradientBackground || this->TexturedBackground ) )
  {
    double aTileViewport[4];
    this->GetRenderWindow()->GetTileViewport( aTileViewport );
    glPushAttrib( GL_ENABLE_BIT | GL_TRANSFORM_BIT | GL_TEXTURE_BIT );
    glDisable( GL_ALPHA_TEST );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glDisable( GL_TEXTURE_1D );
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_BLEND );

    GLint oldShadeModel;
    glGetIntegerv(GL_SHADE_MODEL, &oldShadeModel);
    glShadeModel( GL_SMOOTH ); // color interpolation

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    glOrtho( aTileViewport[0], aTileViewport[2], aTileViewport[1], aTileViewport[3], -1.0, 1.0 );

    if( this->GradientBackground )
    {
      Vec2D corners = {{0.0f,0.0f,0.0f},
                     {0.0f,0.0f,0.0f},
                     {0.0f,0.0f,0.0f},
                     {0.0f,0.0f,0.0f}};
      GetCornersColor(corners); 

#ifdef VTK_OPENGL2
      if (this->OpenGLHelper.IsInitialized())
      {
        if (this->VertexArrayObject == 0)
        {
          this->OpenGLHelper.vglGenVertexArraysARB (1, &this->VertexArrayObject);
        }

        this->OpenGLHelper.vglUseProgramObjectARB (this->BackgroundProgram);
        this->OpenGLHelper.vglBindVertexArrayARB  (this->VertexArrayObject);

        GLfloat data[10 * 4];
        if( this->GradientType != FirstCornerGradient && this->GradientType != ThirdCornerGradient )
        {
          const float tmpData[] = { (float)corners[0][0], (float)corners[0][1], (float)corners[0][2], 1.0f,       -1.0f,  1.0f, 0.0f,    0.0f, 0.0f, 0.0f,
                                    (float)corners[1][0], (float)corners[1][1], (float)corners[1][2], 1.0f,       -1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 0.0f,
                                    (float)corners[2][0], (float)corners[2][1], (float)corners[2][2], 1.0f,        1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 0.0f,
                                    (float)corners[3][0], (float)corners[3][1], (float)corners[3][2], 1.0f,        1.0f,  1.0f, 0.0f,    0.0f, 0.0f, 0.0f};
          memcpy (data, tmpData, sizeof(float) * 10 * 4);
        }
        else //if( this->GradientType == FirstCornerGradient || this->GradientType == ThirdCornerGradient )
        {
          const float tmpData[] = { (float)corners[1][0], (float)corners[1][1], (float)corners[1][2], 1.0f,       -1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 0.0f,
                                    (float)corners[2][0], (float)corners[2][1], (float)corners[2][2], 1.0f,       -1.0f,  1.0f, 0.0f,    0.0f, 0.0f, 0.0f,
                                    (float)corners[3][0], (float)corners[3][1], (float)corners[3][2], 1.0f,        1.0f,  1.0f, 0.0f,    0.0f, 0.0f, 0.0f,
                                    (float)corners[0][0], (float)corners[0][1], (float)corners[0][2], 1.0f,        1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 0.0f};
          memcpy (data, tmpData, sizeof(float) * 10 * 4);
        }

        GLuint vertexBuffer;
        this->OpenGLHelper.vglGenBuffersARB (1, &vertexBuffer);
        this->OpenGLHelper.vglBindBufferARB (GL_ARRAY_BUFFER_ARB, vertexBuffer);
        this->OpenGLHelper.vglBufferDataARB (GL_ARRAY_BUFFER_ARB, sizeof(data), data, GL_STATIC_DRAW_ARB);

        GLint colorAttrib  = this->OpenGLHelper.vglGetAttribLocationARB (this->BackgroundProgram, "Color");
        GLint vertexAttrib = this->OpenGLHelper.vglGetAttribLocationARB (this->BackgroundProgram, "Vertex");
        GLint bgColorAttrib = this->OpenGLHelper.vglGetAttribLocationARB (this->BackgroundProgram, "ColorBG");
        GLsizei vertexSize = sizeof(GLfloat) * 10;

        this->OpenGLHelper.vglVertexAttribPointerARB (colorAttrib, 4, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)0);
        this->OpenGLHelper.vglEnableVertexAttribArrayARB (colorAttrib);

        this->OpenGLHelper.vglVertexAttribPointerARB (vertexAttrib, 3, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)(sizeof(GLfloat) * 4));
        this->OpenGLHelper.vglEnableVertexAttribArrayARB (vertexAttrib);

        this->OpenGLHelper.vglVertexAttribPointerARB (bgColorAttrib, 3, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)(sizeof(GLfloat) * 7));
        this->OpenGLHelper.vglEnableVertexAttribArrayARB (bgColorAttrib);

        this->OpenGLHelper.vglUniform1iARB (this->myLocations.UseTexture, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        this->OpenGLHelper.vglDisableVertexAttribArrayARB (0);
        this->OpenGLHelper.vglBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
        this->OpenGLHelper.vglDeleteBuffersARB (1, &vertexBuffer);
        this->OpenGLHelper.vglBindVertexArrayARB (0);
        this->OpenGLHelper.vglUseProgramObjectARB (0);
      }
#else

      glBegin( GL_TRIANGLE_FAN );
      if( this->GradientType != FirstCornerGradient && this->GradientType != ThirdCornerGradient )
      {
        glColor3f( corners[0][0], corners[0][1], corners[0][2] ); glVertex2f( 0.F, 0.F );
        glColor3f( corners[1][0], corners[1][1], corners[1][2] ); glVertex2f( 1.F, 0.F );
        glColor3f( corners[2][0], corners[2][1], corners[2][2] ); glVertex2f( 1.F, 1.F );
        glColor3f( corners[3][0], corners[3][1], corners[3][2] ); glVertex2f( 0.F, 1.F );
      }
      else //if( this->GradientType == FirstCornerGradient || this->GradientType == ThirdCornerGradient )
      {
        glColor3f( corners[1][0], corners[1][1], corners[1][2] ); glVertex2f( 1.F, 0.F );
        glColor3f( corners[2][0], corners[2][1], corners[2][2] ); glVertex2f( 1.F, 1.F );
        glColor3f( corners[3][0], corners[3][1], corners[3][2] ); glVertex2f( 0.F, 1.F );
        glColor3f( corners[0][0], corners[0][1], corners[0][2] ); glVertex2f( 0.F, 0.F );
      }
      glEnd();
#endif
    }

    if( this->TexturedBackground && this->BackgroundTexture )
    {
      if( VTKViewer_Texture* aTexture = VTKViewer_Texture::SafeDownCast( this->BackgroundTexture ) )
      {
        glEnable( GL_TEXTURE_2D );

        aTexture->Render( this );

        // NOTE: By default the mode is GL_MODULATE. Since the user
        // cannot set the mode, the default is set to replace.
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

        // NOTE: vtkTexture Render enables the alpha test
        // so that no buffer is affected if alpha of incoming fragment is
        // below the threshold. Here we have to enable it so that it won't
        // rejects the fragments of the quad as the alpha is set to 0 on it.
        glDisable( GL_ALPHA_TEST );

        GLfloat texX = 1.F; // texture <s> coordinate
        GLfloat texY = 1.F; // texture <t> coordinate

        int aPosition = aTexture->GetPosition();
        int aWidth = aTexture->GetWidth();
        int aHeight = aTexture->GetHeight();
        int aViewWidth = this->RenderWindow->GetSize()[0];
        int aViewHeight = this->RenderWindow->GetSize()[1];

        if( aPosition == VTKViewer_Texture::Tiled )
        {
          texX = (GLfloat)aViewWidth / (GLfloat)aWidth;
          texY = (GLfloat)aViewHeight / (GLfloat)aHeight; texY *= -1;
        }
#ifdef VTK_OPENGL2
        if (this->OpenGLHelper.IsInitialized())
        {
          if (this->VertexArrayObject == 0)
          {
            this->OpenGLHelper.vglGenVertexArraysARB (1, &this->VertexArrayObject);
          }

          this->OpenGLHelper.vglUseProgramObjectARB (this->BackgroundProgram);
          this->OpenGLHelper.vglBindVertexArrayARB  (this->VertexArrayObject);

          GLfloat dx = (aPosition == VTKViewer_Texture::Centered) ? (( (GLfloat)aWidth / (GLfloat)aViewWidth )) : 1.0f;
          GLfloat dy = (aPosition == VTKViewer_Texture::Centered) ? (( (GLfloat)aHeight / (GLfloat)aViewHeight )) : (aPosition == VTKViewer_Texture::Stretched) ? 1.0f : -1.0f;

          // call to get backcolor
          Vec2D bgColor = {{0.0f,0.0f,0.0f},
                           {0.0f,0.0f,0.0f},
                           {0.0f,0.0f,0.0f},
                           {0.0f,0.0f,0.0f}};
          GetCornersColor(bgColor);            
          // First 4 components of Vertex is TexCoords now.
          GLfloat data[10 * 4];
          if( this->GradientType != FirstCornerGradient && this->GradientType != ThirdCornerGradient )
          {
            const float tmpData[] = { 0.0f, texY, 0.0f, 1.0f,       -dx,  dy, 0.0f, (float)bgColor[0][0], (float)bgColor[0][1], (float)bgColor[0][2],
                                      0.0f, 0.0f, 0.0f, 1.0f,       -dx, -dy, 0.0f, (float)bgColor[1][0], (float)bgColor[1][1], (float)bgColor[1][2],
                                      texX, 0.0f, 0.0f, 1.0f,        dx, -dy, 0.0f, (float)bgColor[2][0], (float)bgColor[2][1], (float)bgColor[2][2],
                                      texX, texY, 0.0f, 1.0f,        dx,  dy, 0.0f, (float)bgColor[3][0], (float)bgColor[3][1], (float)bgColor[3][2] };
            memcpy (data, tmpData, sizeof(float) * 10 * 4);
          }
          else //if( this->GradientType == FirstCornerGradient || this->GradientType == ThirdCornerGradient )
          {
            const float tmpData[] = { 0.0f, texY, 0.0f, 1.0f,       -dx,  dy, 0.0f, (float)bgColor[2][0], (float)bgColor[2][1], (float)bgColor[2][2],
                                      0.0f, 0.0f, 0.0f, 1.0f,       -dx, -dy, 0.0f, (float)bgColor[1][0], (float)bgColor[1][1], (float)bgColor[1][2],
                                      texX, 0.0f, 0.0f, 1.0f,        dx, -dy, 0.0f, (float)bgColor[0][0], (float)bgColor[0][1], (float)bgColor[0][2],
                                      texX, texY, 0.0f, 1.0f,        dx,  dy, 0.0f, (float)bgColor[3][0], (float)bgColor[3][1], (float)bgColor[3][2] };
            memcpy (data, tmpData, sizeof(float) * 10 * 4);
          }

          GLuint vertexBuffer;
          this->OpenGLHelper.vglGenBuffersARB (1, &vertexBuffer);
          this->OpenGLHelper.vglBindBufferARB (GL_ARRAY_BUFFER_ARB, vertexBuffer);
          this->OpenGLHelper.vglBufferDataARB (GL_ARRAY_BUFFER_ARB, sizeof(data), data, GL_STATIC_DRAW_ARB);

          GLint colorAttrib  = this->OpenGLHelper.vglGetAttribLocationARB (this->BackgroundProgram, "Color");
          GLint vertexAttrib = this->OpenGLHelper.vglGetAttribLocationARB (this->BackgroundProgram, "Vertex");
          GLint bgColorAttrib = this->OpenGLHelper.vglGetAttribLocationARB (this->BackgroundProgram, "ColorBG");
          GLsizei vertexSize = sizeof(GLfloat) * 10;

          this->OpenGLHelper.vglVertexAttribPointerARB (colorAttrib, 4, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)0);
          this->OpenGLHelper.vglEnableVertexAttribArrayARB (colorAttrib);

          this->OpenGLHelper.vglVertexAttribPointerARB (vertexAttrib, 3, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)(sizeof(GLfloat) * 4));
          this->OpenGLHelper.vglEnableVertexAttribArrayARB (vertexAttrib);

          this->OpenGLHelper.vglVertexAttribPointerARB (bgColorAttrib, 3, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)(sizeof(GLfloat) * 7));
          this->OpenGLHelper.vglEnableVertexAttribArrayARB (bgColorAttrib);

          this->OpenGLHelper.vglUniform1iARB (this->myLocations.UseTexture, 1);
          this->OpenGLHelper.vglUniform1iARB (this->myLocations.BackgroundTexture, GL_TEXTURE0);

          glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

          this->OpenGLHelper.vglDisableVertexAttribArrayARB (0);
          this->OpenGLHelper.vglBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
          this->OpenGLHelper.vglDeleteBuffersARB (1, &vertexBuffer);
          this->OpenGLHelper.vglBindVertexArrayARB (0);
          this->OpenGLHelper.vglUseProgramObjectARB (0);
        }
#else
        GLfloat x_offset = 0.5f, y_offset = 0.5f;
        GLfloat coeff = 0.5f;
        if( aPosition == VTKViewer_Texture::Centered )
        {
          x_offset = ( (GLfloat)aWidth / (GLfloat)aViewWidth ) / 2.;
          y_offset = ( (GLfloat)aHeight / (GLfloat)aViewHeight ) / 2.;
        }

        // OCCT issue 0023102: Change the algorithm of rendering the
        // 3d viewer background using tiled texture
        // Setting this coefficient to -1.F allows to tile textures relatively
        // to the top-left corner of the view (value 1.F corresponds to the
        // initial behaviour - tiling from the bottom-left corner)
        GLfloat aCoef = -1.F;

        // Note that texture is mapped using GL_REPEAT wrapping mode so integer part
        // is simply ignored, and negative multiplier is here for convenience only
        // and does not result e.g. in texture mirroring
        glBegin( GL_QUADS );
        glTexCoord2f(  0.F,          0.F ); glVertex2f( -x_offset + coeff, -aCoef * y_offset + coeff );
        glTexCoord2f( texX,          0.F ); glVertex2f(  x_offset + coeff, -aCoef * y_offset + coeff );
        glTexCoord2f( texX, aCoef * texY ); glVertex2f(  x_offset + coeff,  aCoef * y_offset + coeff );
        glTexCoord2f(  0.F, aCoef * texY ); glVertex2f( -x_offset + coeff,  aCoef * y_offset + coeff );
        glEnd();
#endif
      }
    }

    // Restore settings.
    {
      glEnable( GL_ALPHA_TEST );
      glEnable( GL_DEPTH_TEST );
      glEnable( GL_LIGHTING );
      glEnable( GL_TEXTURE_1D );
      glEnable( GL_TEXTURE_2D );
      glEnable( GL_BLEND );

      glShadeModel( oldShadeModel ); // color interpolation
    }

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    glPopAttrib();
  }
}
