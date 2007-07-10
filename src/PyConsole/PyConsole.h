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
// File   : PyConsole.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#if !defined ( PYCONSOLE_H )
#define PYCONSOLE_H

// ========================================================
// set dllexport type for Win platform 
#ifdef WIN32
#  ifdef PYCONSOLE_EXPORTS
#    define PYCONSOLE_EXPORT __declspec(dllexport)
#  else
#    define PYCONSOLE_EXPORT __declspec(dllimport)
#  endif
#else   // WIN32
#  define PYCONSOLE_EXPORT
#endif  // WIN32

// ========================================================
// avoid warning messages
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4251)
#endif

#endif // PYCONSOLE_H
