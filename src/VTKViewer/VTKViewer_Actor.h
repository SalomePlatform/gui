#ifndef VTKVIEVER_ACTOR_H
#define VTKVIEVER_ACTOR_H

#include "VTKViewer.h"

#include <vtkLODActor.h>
#include <vtkProperty.h>
#include <vtkShrinkFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>

#include <vector>

class vtkCell;
class vtkDataSet;
class vtkPolyData;

class VTKViewer_Transform;
class VTKViewer_GeometryFilter;
class VTKViewer_TransformFilter;
class VTKViewer_PassThroughFilter;

#define VTKViewer_POINT_SIZE 3

class VTKVIEWER_EXPORT VTKViewer_Actor : public vtkLODActor
{
public:
  static VTKViewer_Actor* New();

  vtkTypeMacro( VTKViewer_Actor, vtkLODActor );

  virtual const char* getName() { return myName.c_str(); }
  virtual void setName(const char* theName){ myName = theName;}

  // To generate highlight automaticaly
  virtual bool hasHighlight() { return false; } 
  virtual void highlight(bool theHighlight) { myIsHighlighted = theHighlight; }  
  virtual bool isHighlighted() { return myIsHighlighted; }

  virtual void SetOpacity(float theOpacity);
  virtual float GetOpacity();

  virtual void SetColor(float r,float g,float b);
  virtual void GetColor(float& r,float& g,float& b);
  void SetColor(const float theRGB[3]){ SetColor(theRGB[0],theRGB[1],theRGB[2]); }

  vtkSetObjectMacro(PreviewProperty,vtkProperty);

  virtual void SetPreSelected(bool thePreselect = false) { myIsPreselected = thePreselect;}

  // Used to obtain all dependent actors
  virtual void GetChildActors(vtkActorCollection*) {};
  
  virtual void AddToRender(vtkRenderer* theRenderer); 
  virtual void RemoveFromRender(vtkRenderer* theRenderer);


  // For selection mapping purpose
  virtual int GetNodeObjId(int theVtkID) { return theVtkID;}
  virtual float* GetNodeCoord(int theObjID);

  virtual int GetElemObjId(int theVtkID) { return theVtkID;}
  virtual vtkCell* GetElemCell(int theObjID);

  virtual int GetObjDimension( const int theObjId );

  virtual void SetMapper(vtkMapper* theMapper); 
  virtual vtkDataSet* GetInput(); 


  virtual void SetTransform(VTKViewer_Transform* theTransform); 
  virtual unsigned long int GetMTime();

  virtual void SetRepresentation(int theMode);
  virtual int GetRepresentation();

  virtual int getDisplayMode();
  virtual void setDisplayMode(int theMode);

  // Infinitive means actor without size (point for example),
  // which is not taken into account in calculation of boundaries of the scene
  void SetInfinitive(bool theIsInfinite) { myIsInfinite = theIsInfinite; }
  virtual bool IsInfinitive();
    
  void SetResolveCoincidentTopology(bool theIsResolve);
  void SetPolygonOffsetParameters(float factor, float units);
  void GetPolygonOffsetParameters(float& factor, float& units);

  virtual void Render(vtkRenderer *, vtkMapper *);

protected:
  bool myIsResolveCoincidentTopology;
  float myPolygonOffsetFactor;
  float myPolygonOffsetUnits;

  std::string myName;

  vtkProperty *PreviewProperty;
  bool myIsPreselected;

  float myOpacity;
  bool myIsHighlighted;
  int myDisplayMode;
  bool myIsInfinite;

  bool myStoreMapping;
  VTKViewer_GeometryFilter *myGeomFilter;
  VTKViewer_TransformFilter *myTransformFilter;
  std::vector<VTKViewer_PassThroughFilter*> myPassFilter;

  int myRepresentation;
  vtkProperty *myProperty;

  void InitPipeLine(vtkMapper* theMapper); 

  VTKViewer_Actor();
  ~VTKViewer_Actor();
};

#endif
