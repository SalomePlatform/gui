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
#ifndef SALOMEAPP_EVENTFILTER_H
#define SALOMEAPP_EVENTFILTER_H

#include "SalomeApp.h"
#include <qobject.h>

#if defined WNT
#pragma warning( disable: 4251 )
#endif

class SALOME_Event;

/*!
  Class provide event filter.
*/
class SALOMEAPP_EXPORT SalomeApp_EventFilter: public QObject 
{
public:
  static void Init();
  static void Destroy();

protected:
  SalomeApp_EventFilter();
  virtual ~SalomeApp_EventFilter();

private:
  /*! global event filter for qapplication */
  virtual bool eventFilter( QObject* o, QEvent* e );
  void processEvent( SALOME_Event* );

private:
  static SalomeApp_EventFilter* myFilter;
};

#if defined WNT
#pragma warning( default: 4251 )
#endif

#endif
