// Copyright (C) 2007-2023  CEA, EDF, OPEN CASCADE
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

//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//  File   : SALOME_Selection.cxx
//  Author : Nicolas REJNERI
#include "SVTK_SelectorDef.h"

#include <VTKViewer_Filter.h>

#include "SALOME_Actor.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <vtkCallbackCommand.h>
#include <vtkActorCollection.h>
#include <vtkCellPicker.h>


/*!
  \return new SVTK_Selector
*/
SVTK_Selector* 
SVTK_Selector
::New()
{
  return new SVTK_SelectorDef();
}

/*!
  Default constructor
*/
SVTK_SelectorDef
::SVTK_SelectorDef():
  myPicker(vtkPicker::New()),
  myCellPicker(vtkCellPicker::New())
{
  mySelectionMode = ActorSelection;
  myDynamicPreselection = true;
  myPreselectionEnabled = true;
  mySelectionEnabled = true;

  myPicker->Delete();
  myCellPicker->Delete();
}

/*!
  Destructor
*/
SVTK_SelectorDef
::~SVTK_SelectorDef()
{
}

/*!
  To invoke selectionChanged signals
*/
void 
SVTK_SelectorDef
::StartPickCallback()
{
  this->InvokeEvent(vtkCommand::StartPickEvent,NULL);
}

/*!
  To invoke selectionChanged signals
*/
void 
SVTK_SelectorDef
::EndPickCallback()
{
  this->InvokeEvent(vtkCommand::EndPickEvent,NULL);
}

/*!
  To change current Selection_Mode (as outside effect, it invokes selectionChange signal)
*/
void 
SVTK_SelectorDef
::SetSelectionMode(Selection_Mode theMode)
{
  if(mySelectionMode != theMode){
    mySelectionMode = theMode;
    myMapIOSubIndex.clear();
    myMapIOSubCompositeIndex.clear();
    this->EndPickCallback();
  }
}

/*!
  Clear selection
*/
void 
SVTK_SelectorDef
::ClearIObjects() 
{
  myIO2Actors.clear();
  myIObjects.clear();
  myMapIOSubIndex.clear();
  myMapIOSubCompositeIndex.clear();
}

/*!
  \return true if the SALOME_InteractiveObject presents into selection
*/
bool
SVTK_SelectorDef
::IsSelected(const Handle(SALOME_InteractiveObject)& theIO) const
{
  return !theIO.IsNull() && (myIObjects.find(theIO) != myIObjects.end());
}

/*!
  \return true if the SALOME_Actor presents into selection
*/
bool
SVTK_SelectorDef
::IsSelected(SALOME_Actor* theActor) const
{
  const Handle(SALOME_InteractiveObject) anIO = theActor->getIO();
  return IsSelected(anIO) && myIO2Actors.find(anIO) != myIO2Actors.end();
}

/*!
  \return corresponding SALOME_Actor for SALOME_InteractiveObject
  \param theIO - SALOME_InteractiveObject
*/
SALOME_Actor*
SVTK_SelectorDef
::GetActor(const Handle(SALOME_InteractiveObject)& theIO) const
{
  TIO2Actors::const_iterator anIter = myIO2Actors.find(theIO);
  if(anIter != myIO2Actors.end())
    return anIter->second.GetPointer();
  return NULL;
}

/*!
  Adds SALOME_InteractiveObject into selection
  \param theIO - SALOME_InteractiveObject
*/
bool 
SVTK_SelectorDef
::AddIObject(const Handle(SALOME_InteractiveObject)& theIO) 
{
  if(!IsSelected(theIO)){
    myIObjects.insert(theIO);
    return true;
  }
  return false;
}

/*!
  Adds SALOME_Actor into selection
  \param theActor - SALOME_Actor
*/
bool 
SVTK_SelectorDef
::AddIObject(SALOME_Actor* theActor) 
{
  const Handle(SALOME_InteractiveObject) anIO = theActor->getIO();

  bool anIsIOBound = IsSelected(anIO);
  if(!anIsIOBound)
    myIObjects.insert(anIO);

  bool anIsActorBound = myIO2Actors.find(anIO) != myIO2Actors.end();
  if(!anIsActorBound)
    myIO2Actors[anIO] = theActor;
  
  return !anIsIOBound || !anIsActorBound;
}

/*!
  Removes SALOME_InteractiveObject from selection
  \param theIO - SALOME_InteractiveObject
*/
bool 
SVTK_SelectorDef
::RemoveIObject(const Handle(SALOME_InteractiveObject)& theIO) 
{
  bool anIsIOBound = myIObjects.find(theIO) != myIObjects.end();

  myIObjects.erase(theIO);
  myIO2Actors.erase(theIO);
  myMapIOSubIndex.erase(theIO);
  myMapIOSubCompositeIndex.erase(theIO);

  return anIsIOBound;
}

/*!
  Removes SALOME_Actor from selection
  \param theActor - SALOME_Actor
*/
bool 
SVTK_SelectorDef
::RemoveIObject(SALOME_Actor* theActor) 
{
  const Handle(SALOME_InteractiveObject) anIO = theActor->getIO();

  bool anIsActorBound = myIO2Actors.find(anIO) != myIO2Actors.end();
  if(anIsActorBound)
    myIO2Actors.erase(anIO);

  return RemoveIObject(anIO) || anIsActorBound;
}

/*!
  \return list of all SALOME_InteractiveObject presenting in selection
*/
const SALOME_ListIO& 
SVTK_SelectorDef
::StoredIObjects() const
{
  myIObjectList.Clear();
  TIObjects::const_iterator anIter = myIObjects.begin();
  TIObjects::const_iterator anIterEnd = myIObjects.end();
  for(; anIter != anIterEnd; anIter++)
    myIObjectList.Append(*anIter);

  return myIObjectList;
}

/*!
  \return number of selected objects
*/
int
SVTK_SelectorDef
::IObjectCount() const
{
  return (int)myIObjects.size(); //!< TODO: conversion from size_t to int
}

/*!
  \return true if the SALOME_InteractiveObject has a subselection
  \param theIO - SALOME_InteractiveObject
*/
bool 
SVTK_SelectorDef
::HasIndex( const Handle(SALOME_InteractiveObject)& theIO) const
{
  return myMapIOSubIndex.find(theIO) != myMapIOSubIndex.end();
}

/*!
  Gets indices of subselection for SALOME_InteractiveObject
  \param theIO - SALOME_InteractiveObject
*/
void 
SVTK_SelectorDef
::GetIndex( const Handle(SALOME_InteractiveObject)& theIO, 
            SVTK_TIndexedMapOfVtkId& theIndex)
{
  TMapIOSubIndex::const_iterator anIter = myMapIOSubIndex.find(theIO);
  if(anIter != myMapIOSubIndex.end())
    theIndex = anIter->second.myMap;
  else
    theIndex.Clear();
}

/*!
  \return true if the index presents in subselection 
  \param theIO - SALOME_InteractiveObject
  \param theIndex - index
*/
bool
SVTK_SelectorDef
::IsIndexSelected(const Handle(SALOME_InteractiveObject)& theIO, 
                  int theIndex) const
{
  TMapIOSubIndex::const_iterator anIter = myMapIOSubIndex.find(theIO);
  if(anIter != myMapIOSubIndex.end()){
    const SVTK_TIndexedMapOfVtkId& aMapIndex = anIter->second.myMap;
    return aMapIndex.Contains( theIndex ) == Standard_True;
  }

  return false;
}

static bool removeIndex(SVTK_TIndexedMapOfVtkId& theMapIndex, const int theIndex)
{
  int anId = theMapIndex.FindIndex(theIndex); // i==0 if Index is not in the MapIndex
  if(anId){
    // only the last key can be removed
    int aLastId = theMapIndex.FindKey(theMapIndex.Extent());
    if(aLastId == anId)
      theMapIndex.RemoveLast();
    else{
      SVTK_TIndexedMapOfVtkId aNewMap;
      aNewMap.ReSize(theMapIndex.Extent()-1);
      for(int j = 1; j <= theMapIndex.Extent(); j++){
        int anIndex = theMapIndex(j);
        if ( anIndex != theIndex )
          aNewMap.Add( anIndex );
      }
      theMapIndex = aNewMap;
    }
  }
  return anId != 0;
}

static bool removeCompositeIndex( SVTK_IndexedMapOfVtkIds& theMapIndex, const SVTK_ListOfVtk theIds )
{
  int anId = theMapIndex.FindIndex( theIds ); // i==0 if Index is not in the MapIndex
  if( anId ) {
    // only the last key can be removed
    SVTK_ListOfVtk aLastIds = theMapIndex.FindKey( theMapIndex.Extent() );
    if( aLastIds == theIds )
      theMapIndex.RemoveLast();
    else {
      SVTK_IndexedMapOfVtkIds aNewMap;
      aNewMap.ReSize(theMapIndex.Extent()-1);
      for( int j = 1; j <= theMapIndex.Extent(); j++ ){
        SVTK_ListOfVtk anIds = theMapIndex( j );
        if ( anIds != theIds )
          aNewMap.Add( anIds );
      }
      theMapIndex = aNewMap;
    }
  }
  return anId != 0;
}


/*!
  Changes indices of subselection for SALOME_InteractiveObject
  \param theIO - SALOME_InteractiveObject
  \param theIndices - indices
  \param theIsModeShift - if it is false, then map will be cleared before indices are added
*/
bool
SVTK_SelectorDef
::AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
                    const SVTK_TIndexedMapOfVtkId& theIndices, 
                    bool theIsModeShift)
{
  TMapIOSubIndex::iterator aMapIter = myMapIOSubIndex.find(theIO);
  if(aMapIter == myMapIOSubIndex.end()){
    TIndexedMapOfInteger anEmpty;
    aMapIter = myMapIOSubIndex.
      insert(TMapIOSubIndex::value_type(theIO,anEmpty)).first;
  }
  SVTK_TIndexedMapOfVtkId& aMapIndex = aMapIter->second.myMap;

  if(!theIsModeShift)
    aMapIndex.Clear();
  
  for(int i = 1, iEnd = theIndices.Extent(); i <= iEnd; i++)
    aMapIndex.Add(theIndices(i));
  
  if(aMapIndex.IsEmpty()) {
    myMapIOSubIndex.erase(theIO);
    return false;
  }

  return true;
}


/*!
  Changes indices of subselection for SALOME_InteractiveObject
  \param theIO - SALOME_InteractiveObject
  \param theIndices - indices
  \param theIsModeShift - if it is false, then map will be cleared before indices are added
*/
bool
SVTK_SelectorDef
::AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
                    const SVTK_TVtkIDsMap& theIndices, 
                    bool theIsModeShift)
{
  TMapIOSubIndex::iterator aMapIter = myMapIOSubIndex.find(theIO);
  if(aMapIter == myMapIOSubIndex.end()){
    TIndexedMapOfInteger anEmpty;
    aMapIter = myMapIOSubIndex.
      insert(TMapIOSubIndex::value_type(theIO,anEmpty)).first;
  }
  SVTK_TIndexedMapOfVtkId& aMapIndex = aMapIter->second.myMap;

  if(!theIsModeShift)
    aMapIndex.Clear();
  
  SVTK_TVtkIDsMapIterator anIter(theIndices);
  for(; anIter.More(); anIter.Next())
    aMapIndex.Add(anIter.Key());
  
  if(aMapIndex.IsEmpty()) {
    myMapIOSubIndex.erase(theIO);
    return false;
  }

  return true;
}


/*!
  Changes indices of subselection for SALOME_InteractiveObject
  \param theIO - SALOME_InteractiveObject
  \param theIndex - index
  \param theIsModeShift - if it is false, then map will be cleared before indices are added
*/
bool 
SVTK_SelectorDef
::AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
                    int theIndex, 
                    bool theIsModeShift)
{
  TMapIOSubIndex::iterator anIter = myMapIOSubIndex.find(theIO);
  if(anIter == myMapIOSubIndex.end()){
    TIndexedMapOfInteger anEmpty;
    anIter = myMapIOSubIndex.
      insert(TMapIOSubIndex::value_type(theIO,anEmpty)).first;
  }
  SVTK_TIndexedMapOfVtkId& aMapIndex = anIter->second.myMap;

  bool anIsConatains = aMapIndex.Contains( theIndex ) == Standard_True;
  if ( anIsConatains )
    removeIndex( aMapIndex, theIndex );
  
  if ( !theIsModeShift )
    aMapIndex.Clear();
  
  if ( !anIsConatains )
    aMapIndex.Add( theIndex );

  if ( aMapIndex.IsEmpty() )
    myMapIOSubIndex.erase( theIO );

  return false;
}


/*!
  Removes index of subselection for SALOME_InteractiveObject
  \param theIO - SALOME_InteractiveObject
  \param theIndex - index
*/
void
SVTK_SelectorDef
::RemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
               int theIndex)
{
  if(IsIndexSelected(theIO,theIndex)){
    TMapIOSubIndex::iterator anIter = myMapIOSubIndex.find(theIO);
    SVTK_TIndexedMapOfVtkId& aMapIndex = anIter->second.myMap;
    removeIndex(aMapIndex,theIndex);
  }
}

/*!
  Clears all indices of subselection
*/
void 
SVTK_SelectorDef
::ClearIndex()
{
  myMapIOSubIndex.clear();  
}

/*!
  \return true if the SALOME_InteractiveObject has a composite index subselection
  \param theIO - SALOME_InteractiveObject
*/
bool 
SVTK_SelectorDef
::HasCompositeIndex( const Handle(SALOME_InteractiveObject)& theIO ) const
{
  return myMapIOSubCompositeIndex.find( theIO ) != myMapIOSubCompositeIndex.end();
}

/*!
  Gets composite indices of subselection for SALOME_InteractiveObject
  \param theIO - SALOME_InteractiveObject
*/
void 
SVTK_SelectorDef
::GetCompositeIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		     SVTK_IndexedMapOfVtkIds& theIds )
{
  TMapIOSubCompositeIndex::const_iterator anIter = myMapIOSubCompositeIndex.find( theIO );
  if( anIter != myMapIOSubCompositeIndex.end() )
    theIds = anIter->second;
  else
    theIds.Clear();
}

/*!
  Changes composite indices of subselection for SALOME_InteractiveObject
  \param theIO - SALOME_InteractiveObject
  \param theIndices - composite id
  \param theIsModeShift - if it is false, then map will be cleared before indices are added
*/
bool
SVTK_SelectorDef
::AddOrRemoveCompositeIndex( const Handle( SALOME_InteractiveObject )& theIO, 
                    const SVTK_IndexedMapOfVtkIds& theIds, 
                    bool theIsModeShift)
{
  TMapIOSubCompositeIndex::iterator aMapIter = myMapIOSubCompositeIndex.find( theIO );
  if( aMapIter == myMapIOSubCompositeIndex.end() ) {
    SVTK_IndexedMapOfVtkIds anEmpty;
    aMapIter = myMapIOSubCompositeIndex.insert( TMapIOSubCompositeIndex::value_type( theIO, anEmpty ) ).first;
  }
  SVTK_IndexedMapOfVtkIds& aMapIndex = aMapIter->second;

  if( !theIsModeShift )
    aMapIndex.Clear();
  
  for( int i = 1, iEnd = theIds.Extent(); i <= iEnd; i++ )
    aMapIndex.Add( theIds( i ) );
  
  if( aMapIndex.IsEmpty() ) {
    myMapIOSubCompositeIndex.erase( theIO );
    return false;
  }
  return true;
}

/*!
  Changes indices of subselection for SALOME_InteractiveObject
  \param theIO - SALOME_InteractiveObject
  \param theIds - composite ids
  \param theIsModeShift - if it is false, then map will be cleared before indices are added
*/
bool 
SVTK_SelectorDef
::AddOrRemoveCompositeIndex( const Handle(SALOME_InteractiveObject)& theIO, 
			     SVTK_ListOfVtk theIds,
			     bool theIsModeShift)
{
  TMapIOSubCompositeIndex::iterator anIter = myMapIOSubCompositeIndex.find( theIO );
  if( anIter == myMapIOSubCompositeIndex.end() ) {
    SVTK_IndexedMapOfVtkIds anEmpty;
    anIter = myMapIOSubCompositeIndex.insert(TMapIOSubCompositeIndex::value_type( theIO,anEmpty ) ).first;
  }

  SVTK_IndexedMapOfVtkIds& aMapIndex = anIter->second;

  bool anIsContains = aMapIndex.Contains( theIds ) == Standard_True;
  if ( anIsContains )
    removeCompositeIndex( aMapIndex, theIds );
  
  if ( !theIsModeShift )
    aMapIndex.Clear();
  
  if ( !anIsContains )
    aMapIndex.Add( theIds );

  if ( aMapIndex.IsEmpty() )
    myMapIOSubIndex.erase( theIO );

  return false;
}

/*!
  Removes composite index of subselection for SALOME_InteractiveObject
  \param theIO - SALOME_InteractiveObject
  \param theIds - index
*/
void
SVTK_SelectorDef
::RemoveCompositeIndex( const Handle(SALOME_InteractiveObject)& theIO, 
			SVTK_ListOfVtk theIds )
{
  if(IsCompositeIndexSelected( theIO, theIds ) ) {
    TMapIOSubCompositeIndex::iterator anIter = myMapIOSubCompositeIndex.find( theIO );
    SVTK_IndexedMapOfVtkIds& aMapIndex = anIter->second;
    removeCompositeIndex( aMapIndex,theIds );
  }
}

/*!
  \return true if the composite index presents in subselection 
  \param theIO - SALOME_InteractiveObject
  \param theIds - index
*/
bool
SVTK_SelectorDef
::IsCompositeIndexSelected( const Handle(SALOME_InteractiveObject)& theIO, 
			    SVTK_ListOfVtk theIds ) const
{
  TMapIOSubCompositeIndex::const_iterator anIter = myMapIOSubCompositeIndex.find( theIO );
  if( anIter != myMapIOSubCompositeIndex.end() ) {
    const SVTK_IndexedMapOfVtkIds& aMapIndex = anIter->second;
    return aMapIndex.Contains( theIds ) == Standard_True;
  }
  return false;
}

/*!
  Clears all composite indices of subselection
*/
void 
SVTK_SelectorDef
::ClearCompositeIndex()
{
  myMapIOSubCompositeIndex.clear();  
}


/*!
  To apply a filter on the selection
  \param theFilter - new filter
*/
void
SVTK_SelectorDef
::SetFilter(const Handle(VTKViewer_Filter)& theFilter)
{
  myFilters.insert(TFilters::value_type(theFilter->GetId(),theFilter));
}

/*!
  \return true if filter with given number is applyed
  \param theId - filter id
*/
bool
SVTK_SelectorDef
::IsFilterPresent(const TFilterID theId) const
{
  return myFilters.find(theId) != myFilters.end();
}

/*!
  To remove a filter from the selection
  \param theId - filter id
*/
void  
SVTK_SelectorDef
::RemoveFilter(const TFilterID theId)
{
  if(IsFilterPresent(theId))
    myFilters.erase(theId);
}

/*!
  \return true if the index satisfy installed filters
  \param theActor - actor
  \param theId - filter id
  \param theIsNode - whether it is node
*/
bool
SVTK_SelectorDef
::IsValid(SALOME_Actor* theActor,
          const TFilterID theId,
          const bool theIsNode) const
{
  TFilters::const_iterator anIter = myFilters.begin();
  for(; anIter != myFilters.end(); ++anIter){
    const Handle(VTKViewer_Filter)& aFilter = anIter->second;
    if(theIsNode == aFilter->IsNodeFilter() &&
       !aFilter->IsValid(theActor,theId))
      return false;
  }
  return true;
}

/*!
  \return filter by it's id
  \param theId - filter id
*/
Handle(VTKViewer_Filter) 
SVTK_SelectorDef
::GetFilter(const TFilterID theId) const
{
  TFilters::const_iterator anIter = myFilters.find(theId);
  if(anIter != myFilters.end()){
    const Handle(VTKViewer_Filter)& aFilter = anIter->second;
    return aFilter;
  }
  return Handle(VTKViewer_Filter)();
}

vtkActorCollection*
SVTK_SelectorDef
::Pick(const SVTK_SelectionEvent* theEvent, vtkRenderer* theRenderer) const
{
  vtkActorCollection* aListActors = NULL;

  if ( GetDynamicPreSelection() ) {
    myCellPicker->Pick(theEvent->myX,
                       theEvent->myY, 
                       0.0,
                       theRenderer);
  
    aListActors = myCellPicker->GetActors();
  }

  if ( !aListActors || !aListActors->GetNumberOfItems() ) {
    myPicker->Pick(theEvent->myX,
                   theEvent->myY, 
                   0.0,
                   theRenderer);
    aListActors = myPicker->GetActors();
  }
  
  return aListActors;
}

void
SVTK_SelectorDef
::SetTolerance(const double& theTolerance) 
{
  myPicker->SetTolerance(theTolerance);         
  myCellPicker->SetTolerance(theTolerance);
}

void
SVTK_SelectorDef
::SetDynamicPreSelection( bool theIsDynPreselect )
{
  myDynamicPreselection = theIsDynPreselect;
}

bool
SVTK_SelectorDef
::GetDynamicPreSelection() const
{
  return myDynamicPreselection;
}

void
SVTK_SelectorDef
::SetPreSelectionEnabled( bool theEnabled )
{
  myPreselectionEnabled = theEnabled;
}

bool
SVTK_SelectorDef
::IsPreSelectionEnabled() const
{
  return mySelectionEnabled && myPreselectionEnabled;
}

void
SVTK_SelectorDef
::SetSelectionEnabled( bool theEnabled )
{
  mySelectionEnabled = theEnabled;
}

bool
SVTK_SelectorDef
::IsSelectionEnabled() const
{
  return mySelectionEnabled;
}
