//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : OpenGLUtils_FrameBuffer.cxx
//  Module : SALOME
//
#include "OpenGLUtils_FrameBuffer.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>

#ifndef WNT
# ifndef GLX_GLXEXT_LEGACY
#  define GLX_GLXEXT_LEGACY
# endif
# include <GL/glx.h>
# include <dlfcn.h>
#else
# include <wingdi.h>
#endif

PFNGLGENFRAMEBUFFERSEXTPROC vglGenFramebuffersEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC vglBindFramebufferEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC vglFramebufferTexture2DEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC vglCheckFramebufferStatusEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC vglDeleteFramebuffersEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC vglGenRenderbuffersEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC vglBindRenderbufferEXT = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC vglRenderbufferStorageEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC vglFramebufferRenderbufferEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC vglDeleteRenderbuffersEXT = NULL;

#ifndef WNT
#define GL_GetProcAddress( x ) glXGetProcAddressARB( (const GLubyte*)x )
#else
#define GL_GetProcAddress( x ) wglGetProcAddress( (const LPCSTR)x )
#endif

bool InitializeEXT()
{
  vglGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)GL_GetProcAddress( "glGenFramebuffersEXT" );
  vglBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)GL_GetProcAddress( "glBindFramebufferEXT" );
  vglFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)GL_GetProcAddress( "glFramebufferTexture2DEXT" );
  vglCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)GL_GetProcAddress( "glCheckFramebufferStatusEXT" );
  vglDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)GL_GetProcAddress( "glDeleteFramebuffersEXT" );
  vglGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)GL_GetProcAddress( "glGenRenderbuffersEXT" );
  vglBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)GL_GetProcAddress( "glBindRenderbufferEXT" );
  vglRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)GL_GetProcAddress( "glRenderbufferStorageEXT" );
  vglFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)GL_GetProcAddress( "glFramebufferRenderbufferEXT" );
  vglDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)GL_GetProcAddress( "glDeleteRenderbuffersEXT" );

  bool ok = vglGenFramebuffersEXT && vglBindFramebufferEXT && vglFramebufferTexture2DEXT &&
            vglCheckFramebufferStatusEXT && vglDeleteFramebuffersEXT && vglGenRenderbuffersEXT &&
            vglBindRenderbufferEXT && vglRenderbufferStorageEXT && vglFramebufferRenderbufferEXT &&
            vglDeleteRenderbuffersEXT;

  return ok;
}

static bool IsEXTInitialized = InitializeEXT();

OpenGLUtils_FrameBuffer::OpenGLUtils_FrameBuffer()
  : textureId( 0 ),
    fboId( 0 ),
    rboId( 0 )
{
}

OpenGLUtils_FrameBuffer::~OpenGLUtils_FrameBuffer()
{
  release();
}

bool OpenGLUtils_FrameBuffer::init( const GLsizei& xSize, const GLsizei& ySize )
{
  if( !IsEXTInitialized )
    return false;

  // create a texture object
  glEnable( GL_TEXTURE_2D );
  glGenTextures( 1, &textureId );
  glBindTexture( GL_TEXTURE_2D, textureId );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, xSize, ySize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
  glBindTexture( GL_TEXTURE_2D, 0 );

  // create a renderbuffer object to store depth info
  vglGenRenderbuffersEXT( 1, &rboId );
  vglBindRenderbufferEXT( GL_RENDERBUFFER_EXT, rboId );
  vglRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, xSize, ySize );
  vglBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );

  // create a framebuffer object
  vglGenFramebuffersEXT( 1, &fboId );
  vglBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fboId );

  // attach the texture to FBO color attachment point
  vglFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureId, 0 );

  // attach the renderbuffer to depth attachment point
  vglFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboId );

  // check FBO status
  GLenum status = vglCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );

  // Unbind FBO
  vglBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

  return status == GL_FRAMEBUFFER_COMPLETE_EXT;
}

void OpenGLUtils_FrameBuffer::release()
{
  glDeleteTextures( 1, &textureId );
  textureId = 0;

  vglDeleteFramebuffersEXT( 1, &fboId );
  fboId = 0;

  vglDeleteRenderbuffersEXT( 1, &rboId );
  rboId = 0;
}

void OpenGLUtils_FrameBuffer::bind()
{
  vglBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fboId );
}

void OpenGLUtils_FrameBuffer::unbind()
{
  vglBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}
