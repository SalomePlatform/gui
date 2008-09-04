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

#include <OCCViewer_Trihedron.h>

#include <Graphic3d_CBounds.hxx>
#include <OpenGl_GraphicDriver.hxx>

void OCCViewer_Init();

extern "C"
{
#ifdef WIN32 /* disable MS VC++ warning on C-style function returning C++ object */
#pragma warning(push)
#pragma warning(disable:4190)
#endif
  Standard_EXPORT Handle( Graphic3d_GraphicDriver ) MetaGraphicDriverFactory( const Standard_CString AShrName )
  {
    OCCViewer_Init();
    Handle( OpenGl_GraphicDriver ) aDriver = new OpenGl_GraphicDriver( AShrName );
    return aDriver;
  }
#ifdef WIN32
#pragma warning(pop)
#endif

#include <OpenGl_tgl_all.h>
#include <OpenGl_tsm.h>
#include <OpenGl_callback.h>
#include <OpenGl_telem_highlight.h>
#include <OpenGl_telem_attri.h>

  TStatus OCCViewer_DisplayTrihedron ( TSM_ELEM_DATA, Tint, cmn_key* );
  TStatus OCCViewer_AddTrihedron     ( TSM_ELEM_DATA, Tint, cmn_key* );
  TStatus OCCViewer_DeleteTrihedron  ( TSM_ELEM_DATA, Tint, cmn_key* );
}


TStatus OCCViewer_AddTrihedron( TSM_ELEM_DATA d, Tint n, cmn_key* k )
{
  // Retrieve the userdraw structure
  Graphic3d_CUserDraw *userdraw = ( Graphic3d_CUserDraw* )( k[0]->data.pdata );
  if ( !userdraw )
    return TFailure;

  // Retrieve the user structure
  OCCViewer_Trihedron* userdata = ( OCCViewer_Trihedron* )( userdraw->Data );
  if ( !userdata )
    return TFailure;

  ( ( tsm_elem_data )( d.pdata ) )->pdata = ( void* )userdata;

  // Evaluate minmax if needed
  if( userdraw->Bounds && userdata )
    userdata->bounds( *( userdraw->Bounds ) );

  return TSuccess;
}

TStatus OCCViewer_DeleteTrihedron( TSM_ELEM_DATA d, Tint n, cmn_key* k )
{
  delete ( OCCViewer_Trihedron* )( d.pdata );
  return TSuccess;
}

TStatus OCCViewer_DisplayTrihedron( TSM_ELEM_DATA d, Tint n, cmn_key* k )
{
  ( ( OCCViewer_Trihedron* )( d.pdata ) )->display();
  return TSuccess;
}

void OCCViewer_Init()
{
  //PickTraverse=0
  //DisplayTraverse=1
  //Add=2
  //Delete=3
  //Print=4
  //Inquire=5
  MtblPtr theCallbacks = GetCallbackTable();
  theCallbacks[DisplayTraverse] = OCCViewer_DisplayTrihedron;
  theCallbacks[Add] = OCCViewer_AddTrihedron;
  theCallbacks[Delete] = OCCViewer_DeleteTrihedron;
}
