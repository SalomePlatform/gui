// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
#include <vtkOpenGLExtensionManager.h>
#include <vtkgl.h> // vtkgl namespace
#include <vtkImageImport.h>
#include <vtkPNGWriter.h>
#include <vtkOpenGLTexture.h>
#include <vtkTimerLog.h>
#include <vtkOpenGL.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(VTKViewer_OpenGLRenderer);

VTKViewer_OpenGLRenderer::VTKViewer_OpenGLRenderer()
{
}

VTKViewer_OpenGLRenderer::~VTKViewer_OpenGLRenderer()
{
}

void VTKViewer_OpenGLRenderer::Clear(void)
{
  GLbitfield clear_mask = 0;

  if( !this->Transparent() )
  {
    glClearColor( static_cast<GLclampf>(this->Background[0]),
                  static_cast<GLclampf>(this->Background[1]),
                  static_cast<GLclampf>(this->Background[2]),
                  static_cast<GLclampf>(0.0));
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
      glBegin( GL_QUADS );

      glColor4d( this->Background[0], this->Background[1], this->Background[2], 0.0 );
      glVertex2f( 0.F, 0.F );
      glVertex2f( 1.F, 0.F );

      glColor4d( this->Background2[0], this->Background2[1], this->Background2[2], 0.0 );
      glVertex2f( 1.F, 1.F );
      glVertex2f( 0.F, 1.F );

      glEnd();
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
        GLfloat x_offset = 0.5, y_offset = 0.5;
        GLfloat coeff = 0.5;

        // OCCT issue 0023102: Change the algorithm of rendering the
        // 3d viewer background using tiled texture
        // Setting this coefficient to -1.F allows to tile textures relatively
        // to the top-left corner of the view (value 1.F corresponds to the
        // initial behaviour - tiling from the bottom-left corner)
        GLfloat aCoef = -1.F;

        int aPosition = aTexture->GetPosition();
        int aWidth = aTexture->GetWidth();
        int aHeight = aTexture->GetHeight();
        int aViewWidth = this->RenderWindow->GetSize()[0];
        int aViewHeight = this->RenderWindow->GetSize()[1];                    
        if( aPosition == VTKViewer_Texture::Centered )
        {
          x_offset = ( (GLfloat)aWidth / (GLfloat)aViewWidth ) / 2.;
          y_offset = ( (GLfloat)aHeight / (GLfloat)aViewHeight ) / 2.;
        }
        else if( aPosition == VTKViewer_Texture::Tiled )
        {
          texX = (GLfloat)aViewWidth / (GLfloat)aWidth;
          texY = (GLfloat)aViewHeight / (GLfloat)aHeight;
        }

        // Note that texture is mapped using GL_REPEAT wrapping mode so integer part
        // is simply ignored, and negative multiplier is here for convenience only
        // and does not result e.g. in texture mirroring
        glBegin( GL_QUADS );
        glTexCoord2f(  0.F,          0.F ); glVertex2f( -x_offset + coeff, -aCoef * y_offset + coeff );
        glTexCoord2f( texX,          0.F ); glVertex2f(  x_offset + coeff, -aCoef * y_offset + coeff );
        glTexCoord2f( texX, aCoef * texY ); glVertex2f(  x_offset + coeff,  aCoef * y_offset + coeff );
        glTexCoord2f(  0.F, aCoef * texY ); glVertex2f( -x_offset + coeff,  aCoef * y_offset + coeff );
        glEnd();
      }
    }

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    glPopAttrib();
  }
}
