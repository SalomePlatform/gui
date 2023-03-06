// Copyright (C) 2023  CEA/DEN, EDF R&D
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

//  SALOME PV3DViewer : build PV3D viewer into Salome desktop
//  File   : SPV3D_Prs.cxx

#include "SPV3D_Prs.h"
#include "SPV3D_ViewWindow.h"

#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkMapper.h"
#include "pqServer.h"
#include "pqApplicationCore.h"
#include "pqServerManagerModel.h"
#include "pqObjectBuilder.h"
#include "pqPipelineSource.h"
#include "vtkSMSourceProxy.h"
#include "vtkPVTrivialProducer.h"
#include <pqDataRepresentation.h>

bool SPV3D_EXPORTSPV3DData::IsVisible() const
{
  if( IsNull() )
    return false;
  return GetRepresentation()->isVisible();
}

void SPV3D_EXPORTSPV3DData::Hide() const
{
  if( GetRepresentation() )
    GetRepresentation()->setVisible(0);
}

SPV3D_Prs::SPV3D_Prs( const char* entry , SPV3D_ViewWindow *view) : SALOME_PV3DPrs(entry),_view(view)
{
}

SPV3D_Prs *SPV3D_Prs::deepCopy() const
{
  SPV3D_Prs *ret = new SPV3D_Prs( *this );
  return ret;
}

pqProxy *getPQProxy(vtkSMProxy *proxy)
{
  pqServerManagerModel* smmodel = pqApplicationCore::instance()->getServerManagerModel();
  return smmodel->findItem<pqProxy*>(proxy);
}

void SPV3D_Prs::FillUsingActor(vtkActor *actor) const
{
  SPV3D_EXPORTSPV3DData *alreadyExistingSrc = nullptr;
  alreadyExistingSrc = _view->isEntryAlreadyExist( GetEntry() );
  if(alreadyExistingSrc && !alreadyExistingSrc->GetSourceProducer())
  {
    actor->GetMapper()->Update();
    vtkDataObject *ds = actor->GetMapper()->GetInput();
    vtkPolyData *ds3 = vtkPolyData::SafeDownCast(ds);
    vtkNew<vtkPolyData> ds4;
    if(ds3)
    {
      ds4->ShallowCopy( ds3 );
      vtkNew<vtkIdTypeArray> solidIdArray;
      auto nbCells( ds4->GetNumberOfCells() );
      solidIdArray->SetNumberOfComponents(1);
      solidIdArray->SetNumberOfTuples( nbCells );
      solidIdArray->SetName("Solid id");
      vtkIdType *pt( solidIdArray->GetPointer(0) );
      std::for_each(pt,pt+nbCells,[](vtkIdType& elt) { elt = 0; });
      ds4->GetCellData()->AddArray( solidIdArray );
    }
    //
    pqServer *serv(pqApplicationCore::instance()->getServerManagerModel()->findServer(pqServerResource("builtin:")));
    pqObjectBuilder *builder(pqApplicationCore::instance()->getObjectBuilder());
    pqPipelineSource *mySourceProducer(builder->createSource("sources","PVTrivialProducer",serv));
    vtkSMProxy *producerBase = mySourceProducer->getProxy();
    vtkSMSourceProxy *producer(vtkSMSourceProxy::SafeDownCast(producerBase));
    vtkObjectBase *clientSideObject(producer->GetClientSideObject());
    vtkPVTrivialProducer *clientSideObjectCast = vtkPVTrivialProducer::SafeDownCast(clientSideObject);
    clientSideObjectCast->SetOutput(ds4);
    mySourceProducer->updatePipeline();
    pqProxy *producerBase2( getPQProxy(producerBase) );
    if(producerBase2 && !_name.empty())
      producerBase2->rename( _name.c_str() );
    this->SetSourceProducer( mySourceProducer );
  }
  else
  {
    this->CopyInfo( alreadyExistingSrc );
  }
}

SPV3D_Prs:: ~SPV3D_Prs() 
{
}

void SPV3D_Prs::DisplayIn( SALOME_View* v ) const
{
  SALOME_PV3DPrs::DisplayIn(v);
}

void SPV3D_Prs::CopyInfo(SPV3D_EXPORTSPV3DData *info) const
{
  if(!_pvRendInfo || !info)
    return ;
  if(_pvRendInfo == info)
    return ;
  *_pvRendInfo = *info;
}

bool SPV3D_Prs::IsNull() const
{
  if(_pvRendInfo)
    return _pvRendInfo->IsNull();
  return true;
}


