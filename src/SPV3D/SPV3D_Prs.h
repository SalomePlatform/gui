// Copyright (C) 2023  CEA, EDF
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

#pragma once

#include "SPV3D.h"
#include "SALOME_Prs.h"

class vtkActor;
class pqPipelineSource;
class pqDataRepresentation;

class SPV3D_EXPORTSPV3DData
{
public:
  SPV3D_EXPORTSPV3DData() = default;
  SPV3D_EXPORTSPV3DData *deepCopy() { return new SPV3D_EXPORTSPV3DData(*this); }
  void SetSourceProducer(pqPipelineSource *sourceProducer) const { _sourceProducer = sourceProducer; }
  pqPipelineSource *GetSourceProducer() const { return _sourceProducer; }
  
  void SetRepresentation(pqDataRepresentation *repr) const { _repr = repr; }
  pqDataRepresentation *GetRepresentation() const { return _repr; }
  
  bool IsNull() const { return !_sourceProducer && !_repr; }

  bool IsVisible() const;
  void Hide() const;
private:
  SPV3D_EXPORTSPV3DData(const SPV3D_EXPORTSPV3DData& other) = default;
private:
  mutable pqPipelineSource *_sourceProducer = nullptr;
  mutable pqDataRepresentation *_repr = nullptr;
};

class SPV3D_ViewWindow;

class SPV3D_EXPORT SPV3D_Prs : public SALOME_PV3DPrs
{
public:
  explicit SPV3D_Prs( const char* entry, SPV3D_ViewWindow *view);
  ~SPV3D_Prs();
  
  void SetName(const std::string& name) { _name = name; }
  
  SPV3D_Prs *deepCopy() const;
  
  void FillUsingActor(vtkActor *actor) const;

  void DisplayIn( SALOME_View* v ) const override;

  void CopyInfo(SPV3D_EXPORTSPV3DData *info) const;

  void SetPVRenderInfo(SPV3D_EXPORTSPV3DData *pvRendInfo) { _pvRendInfo = pvRendInfo; }

  void SetSourceProducer(pqPipelineSource *sourceProducer) const { if(_pvRendInfo) _pvRendInfo->SetSourceProducer(sourceProducer); }
  pqPipelineSource *GetSourceProducer() const { if(_pvRendInfo) return _pvRendInfo->GetSourceProducer(); return nullptr; }
  
  void SetRepresentation(pqDataRepresentation *repr) const { if(_pvRendInfo) _pvRendInfo->SetRepresentation(repr); }
  pqDataRepresentation *GetRepresentation() const { if(_pvRendInfo) return _pvRendInfo->GetRepresentation(); return nullptr; }

  bool IsNull() const override;

private:

  SPV3D_Prs(const SPV3D_Prs& other) = default;

public:
  void hide() const { if(_pvRendInfo) return _pvRendInfo->Hide(); }
  bool isVisible() const { if(_pvRendInfo) return _pvRendInfo->IsVisible(); return false; }

private:
  
  SPV3D_EXPORTSPV3DData *_pvRendInfo = nullptr;
  SPV3D_ViewWindow *_view = nullptr;
  //! Name attached to the displayable object in the study
  std::string _name;
};
