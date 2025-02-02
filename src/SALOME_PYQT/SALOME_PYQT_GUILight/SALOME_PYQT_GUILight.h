// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

// File   : SALOME_PYQT_GUI.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#if !defined ( SALOME_PYQT_GUILIGHT_H )
#define SALOME_PYQT_GUIILIGHT_H

// ========================================================
// set dllexport type for Win platform 
#ifdef WIN32

#if defined SALOME_PYQT_LIGHT_EXPORTS || defined SalomePyQtGUILight_EXPORTS
#define SALOME_PYQT_LIGHT_EXPORT __declspec(dllexport)
#else
#define SALOME_PYQT_LIGHT_EXPORT __declspec(dllimport)
#endif

#else   // WIN32

#define SALOME_PYQT_LIGHT_EXPORT

#endif  // WIN32

// ========================================================
// avoid warning messages
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4251)
#endif

#endif // SALOME_PYQT_GUIILIGHT_H
