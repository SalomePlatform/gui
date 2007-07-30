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
// File   : LightApp.h
// Author : OCC team
// 

#ifndef LIGHTAPP_H
#define LIGHTAPP_H

#ifdef WIN32
#  ifdef LIGHTAPP_EXPORTS
#    define LIGHTAPP_EXPORT __declspec(dllexport)
#  else
#    define LIGHTAPP_EXPORT __declspec(dllimport)
#  endif
#else
#  define LIGHTAPP_EXPORT
#endif // WIN32

#if defined WNT
#pragma warning ( disable:4251 )
#pragma warning ( disable:4786 )
#pragma warning ( disable:4503 )
#endif

#define APP_VERSION "0.1"

#endif  // LIGHTAPP_H
