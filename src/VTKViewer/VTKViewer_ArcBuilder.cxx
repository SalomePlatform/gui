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
//  File   : VTKViewer_ArcBuilder.cxx
//  Author : Roman NIKOLAEV
//  Module : GUI
//
#include "VTKViewer_ArcBuilder.h"

#include <math.h>

//VTK includes
#include <vtkMath.h>
#include <vtkUnstructuredGrid.h>
#include <vtkTransformFilter.h>
#include <vtkTransform.h>
#include <vtkPoints.h>
#include <vtkVertex.h>
#include <vtkCellArray.h>
#include <vtkTriangle.h>
#include <vtkPolyData.h>

#define PRECISION 10e-4
#define ANGLE_PRECISION 0.5
//#define _MY_DEBUG_

#ifdef _MY_DEBUG_
#include <iostream>
#endif


bool CheckAngle(const double compare, const double angle){
  if((angle <= compare - ANGLE_PRECISION) || (angle >= compare + ANGLE_PRECISION))
    return true;
  else
    return false;
}

//------------------------------------------------------------------------
/*!
 *Class XYZ
 *Constructor
 */
XYZ::XYZ(double X, double Y, double Z):
 x(X),y(Y),z(Z) {}

/*!
 * Empty Constructor
 */
XYZ::XYZ(){}

/*!
 *  Destructor
 */
XYZ::~XYZ()
{}

/*
 * Calculate modulus
 */
double XYZ::Modulus () const {
  return sqrt (x * x + y * y + z * z);
}

//------------------------------------------------------------------------
/*!
 * Class Pnt
 * Constructor
 */
Pnt::Pnt(double X, 
         double Y, 
         double Z):
  coord(X,Y,Z) {}


Pnt::Pnt(){}
/*!
 * Destructor
 */
Pnt::~Pnt()
{}

//------------------------------------------------------------------------
/*!
 * Class Vec
 * Constructor
 */
Vec::Vec (const double Xv,
          const double Yv,
          const double Zv)
{
  double D = sqrt (Xv * Xv + Yv * Yv + Zv * Zv);
  if(D != 0) {
    coord.SetX(Xv / D);
    coord.SetY(Yv / D);
    coord.SetZ(Zv / D);
  }
}

/*!
 * Destructor
 */
Vec::~Vec(){}


/*!
 * Calculate angle between vectors in radians
 */
double Vec::AngleBetween(const Vec & Other)
{
  double res;
  double numerator = GetXYZ().X()*Other.GetXYZ().X()+GetXYZ().Y()*Other.GetXYZ().Y()+GetXYZ().Z()*Other.GetXYZ().Z();
  double denumerator = GetXYZ().Modulus()*Other.GetXYZ().Modulus();
  res = acos(numerator/denumerator);
  return res;
}

/*!
 * Calculate angle between vectors in degrees
 */
double Vec::AngleBetweenInGrad(const Vec & Other){
  return AngleBetween(Other)*vtkMath::DoubleRadiansToDegrees();
}

/*
 * Calculate vector multiplication
*/
Vec Vec::VectMultiplication(const Vec & Other) const{
  double x = GetXYZ().Y()*Other.GetXYZ().Z() - GetXYZ().Z()*Other.GetXYZ().Y();
  double y = GetXYZ().Z()*Other.GetXYZ().X() - GetXYZ().X()*Other.GetXYZ().Z();
  double z = GetXYZ().X()*Other.GetXYZ().Y() - GetXYZ().Y()*Other.GetXYZ().X();
  Vec *aRes  = new Vec(x,y,z);
  return *aRes;
}

/*---------------------Class Plane --------------------------------*/
/*!
 * Constructor
 */
Plane::Plane(const Pnt& thePnt1, const Pnt& thePnt2, const Pnt& thePnt3)
{
  CalculatePlane(thePnt1,thePnt2,thePnt3);
}

/*!
 * Destructor
 */
Plane::~Plane()
{}

/*
 * Return plane normale
 */
Vec Plane::GetNormale() const{
  return Vec(myA,myB,myC);
}

/*
 * Calculate A,B,C coeefs of plane
 */
void Plane::CalculatePlane(const Pnt& thePnt1, const Pnt& thePnt2, const Pnt& thePnt3){
  
  double x1 = thePnt1.GetXYZ().X();
  double x2 = thePnt2.GetXYZ().X();
  double x3 = thePnt3.GetXYZ().X();
  
  double y1 = thePnt1.GetXYZ().Y();
  double y2 = thePnt2.GetXYZ().Y();
  double y3 = thePnt3.GetXYZ().Y();
  
  double z1 = thePnt1.GetXYZ().Z();
  double z2 = thePnt2.GetXYZ().Z();
  double z3 = thePnt3.GetXYZ().Z();
  
  myA = y1*(z2 - z3) + y2*(z3 - z1) + y3*(z1 - z2);
  myB = z1*(x2 - x3) + z2*(x3 - x1) + z3*(x1 - x2);
  myC = x1*(y2 - y3) + x2*(y3 - y1) + x3*(y1 - y2);
  
#ifdef _MY_DEBUG_
  cout<<"Plane A: "<<myA<<endl;
  cout<<"Plane B: "<<myB<<endl;
  cout<<"Plane C: "<<myC<<endl;
#endif  

} 


/*---------------------Class VTKViewer_ArcBuilder --------------------------------*/
/*!
 * Constructor
 */
VTKViewer_ArcBuilder::VTKViewer_ArcBuilder(const Pnt& thePnt1,
                                           const Pnt& thePnt2,
                                           const Pnt& thePnt3,
                                           double theAngle):
  myStatus(Arc_Error),
  myAngle(theAngle)
{
  Vec V1(thePnt2.GetXYZ().X()-thePnt1.GetXYZ().X(),
         thePnt2.GetXYZ().Y()-thePnt1.GetXYZ().Y(),
         thePnt2.GetXYZ().Z()-thePnt1.GetXYZ().Z());
  
  Vec V2(thePnt2.GetXYZ().X()-thePnt3.GetXYZ().X(),
         thePnt2.GetXYZ().Y()-thePnt3.GetXYZ().Y(),
         thePnt2.GetXYZ().Z()-thePnt3.GetXYZ().Z());

  double angle = V1.AngleBetweenInGrad(V2);
  
  //Check that points are not belong one line
#ifdef _MY_DEBUG_
  cout<<"Angle for check: "<<angle<<endl;
#endif
  if(CheckAngle(180,angle)) {
    
    // Build plane by three points
    Plane aPlane(thePnt1,thePnt2,thePnt3);
    
    //Plane normales
    Vec aPlaneNormale = aPlane.GetNormale();
#ifdef _MY_DEBUG_
    std::cout<<"X Normale: "<<aPlaneNormale.GetXYZ().X()<<std::endl;
    std::cout<<"Y Normale: "<<aPlaneNormale.GetXYZ().Y()<<std::endl;
    std::cout<<"Z Normale: "<<aPlaneNormale.GetXYZ().Z()<<std::endl;
#endif
    //OZ vector
    Vec OZ(0,0,1);
    
    //Projection plane normale on XOY
    Vec aNormaleProjection (aPlaneNormale.GetXYZ().X(),
                            aPlaneNormale.GetXYZ().Y(),
                            0);
#ifdef _MY_DEBUG_
    std::cout<<"X Normale Projection: "<<aNormaleProjection.GetXYZ().X()<<std::endl;
    std::cout<<"Y Normale Projection: "<<aNormaleProjection.GetXYZ().Y()<<std::endl;
    std::cout<<"Z Normale Projection: "<<aNormaleProjection.GetXYZ().Z()<<std::endl;
#endif
    
    //Rotation axis
    Vec aAxis = aNormaleProjection.VectMultiplication(OZ);
#ifdef _MY_DEBUG_
    std::cout<<"X Axis: "<<aAxis.GetXYZ().X()<<std::endl;
    std::cout<<"Y Axis: "<<aAxis.GetXYZ().Y()<<std::endl;
    std::cout<<"Z Axis: "<<aAxis.GetXYZ().Z()<<std::endl;
#endif   
    //Rotation angle
    double anAngle = OZ.AngleBetweenInGrad(aPlaneNormale);
#ifdef _MY_DEBUG_
    std::cout<<"Rotation Angle :"<<anAngle<<endl;
#endif
    PntList aInputPnts;
    aInputPnts.push_back(thePnt1);
    aInputPnts.push_back(thePnt2);
    aInputPnts.push_back(thePnt3);
    
    vtkUnstructuredGrid* aGrid = BuildGrid(aInputPnts);
    
    bool needRotation = true;
    if(anAngle  == 0 || anAngle == 180)
      needRotation = false;
    
    if(aGrid) {
      vtkUnstructuredGrid* aTransformedGrid;
      if(needRotation) {
        aTransformedGrid = TransformGrid(aGrid,aAxis,anAngle);    
        aTransformedGrid->Update();
#ifdef _MY_DEBUG_
        cout<<"Need Rotation!!!"<<endl;
#endif
      }
      else {
        aTransformedGrid = aGrid;
#ifdef _MY_DEBUG_
        cout<<"Rotation does not need!!!"<<endl;
#endif
      }
      
      double coords[3];
      aTransformedGrid->GetPoint(0,coords);
      myPnt1 = Pnt(coords[0],coords[1],coords[2]);
      aTransformedGrid->GetPoint(1,coords);
      myPnt2 = Pnt(coords[0],coords[1],coords[2]);
      aTransformedGrid->GetPoint(2,coords);
      myPnt3 = Pnt(coords[0],coords[1],coords[2]);
      vtkUnstructuredGrid* anArc = BuildArc();
      vtkUnstructuredGrid* anTransArc;
      if(needRotation) {
        anTransArc = TransformGrid(anArc,aAxis,-anAngle);
        anTransArc->Update();
      }
      else
        anTransArc = anArc;
      
      myPoints = anTransArc->GetPoints();
      myStatus = Arc_Done;
    }
  }
  else{
#ifdef _MY_DEBUG_
    std::cout<<"Points lay on the one line !"<<endl;
#endif           
    PntList aList;
    aList.push_back(thePnt1);
    aList.push_back(thePnt2);
    aList.push_back(thePnt3);
    vtkUnstructuredGrid* aGrid = BuildGrid(aList);
    aGrid->Update();
    myPoints = aGrid->GetPoints();
  }
}

/*!
 * Destructor
 */
VTKViewer_ArcBuilder::~VTKViewer_ArcBuilder()
{}


/*
 * Add to the vtkUnstructuredGrid points from input list
 */
vtkUnstructuredGrid* VTKViewer_ArcBuilder::BuildGrid(const PntList& theList) const
{
  int aListsize = theList.size();  
  vtkUnstructuredGrid* aGrid = NULL;
  
  if(aListsize != 0) {
    aGrid = vtkUnstructuredGrid::New();
    vtkPoints* aPoints = vtkPoints::New();
    aPoints->SetNumberOfPoints(aListsize);
    
    aGrid->Allocate(aListsize);
    
    PntList::const_iterator it = theList.begin();
    int aCounter = 0;
    for(;it != theList.end();it++) {
      Pnt aPnt =  *it;
      aPoints->InsertPoint(aCounter, 
                           aPnt.GetXYZ().X(), 
                           aPnt.GetXYZ().Y(),
                           aPnt.GetXYZ().Z());
      vtkVertex* aVertex = vtkVertex::New();
      aVertex->GetPointIds()->SetId(0, aCounter);
      aGrid->InsertNextCell(aVertex->GetCellType(), aVertex->GetPointIds());
      aCounter++;
      aVertex->Delete();
    }
    aGrid->SetPoints(aPoints);
    aPoints->Delete();
  }
  return aGrid;
}


vtkUnstructuredGrid* 
VTKViewer_ArcBuilder::TransformGrid(vtkUnstructuredGrid* theGrid, 
                                    const Vec& theAxis, const double angle) const
{
  vtkTransform *aTransform = vtkTransform::New();
  aTransform->RotateWXYZ(angle, theAxis.GetXYZ().X(), theAxis.GetXYZ().Y(), theAxis.GetXYZ().Z());
  vtkTransformFilter* aTransformFilter  = vtkTransformFilter::New();
  aTransformFilter->SetTransform(aTransform);
  aTransformFilter->SetInput(theGrid);
  aTransform->Delete();
  return aTransformFilter->GetUnstructuredGridOutput();
}


void VTKViewer_ArcBuilder::GetAngle(const double theAngle){
  myAngle = theAngle;
}

vtkUnstructuredGrid* VTKViewer_ArcBuilder::BuildArc(){
  double x1 = myPnt1.GetXYZ().X(); double x2 = myPnt2.GetXYZ().X(); double x3 = myPnt3.GetXYZ().X();
  double y1 = myPnt1.GetXYZ().Y(); double y2 = myPnt2.GetXYZ().Y(); double y3 = myPnt3.GetXYZ().Y();
  double z =  myPnt1.GetXYZ().Z();  //Points on plane || XOY
  
  
  double K1;
  double K2;
  K1 = (y2 - y1)/(x2 - x1);
  K2 = (y3 - y2)/(x3 - x2);
#ifdef _MY_DEBUG_   
  std::cout<<"K1 : "<< K1 <<endl; 
  std::cout<<"K2 : "<< K2 <<endl; 
#endif
  double xCenter;
  if(x3 == x2) //K2 --> infinity
    xCenter = (K1*(y1-y3) + (x1+x2))/2.0;
  
  else if(x2 == x1) //K1 --> infinity
    xCenter = (K2*(y1-y3) - (x2+x3))/(-2.0);
  
  else 
    xCenter = (K1*K2*(y1-y3) + K2*(x1+x2) - K1*(x2+x3))/ (2.0*(K2-K1));
  
  double yCenter;
  
  if ( K1 == 0 )
    yCenter =  (-1/K2)*(xCenter - (x2+x3)/2.0) + (y2 + y3)/2.0;
  else 
    yCenter =  (-1/K1)*(xCenter - (x1+x2)/2.0) + (y1 + y2)/2.0;
  
  double zCenter = z;
#ifdef _MY_DEBUG_   
  std::cout<<"xCenter : "<<xCenter<<endl;
  std::cout<<"yCenter : "<<yCenter<<endl;
  std::cout<<"zCenter : "<<zCenter<<endl;
#endif
  double aRadius = sqrt((x1 - xCenter)*(x1 - xCenter) + (y1 - yCenter)*(y1 - yCenter));
  
  double angle1 = GetPointAngleOnCircle(xCenter,yCenter,x1,y1);
  double angle2 = GetPointAngleOnCircle(xCenter,yCenter,x2,y2);
  double angle3 = GetPointAngleOnCircle(xCenter,yCenter,x3,y3);
  
  
  double aMaxAngle = vtkMath::DoubleDegreesToRadians()*myAngle*2;   
  
  /*  double aTotalAngle =  fabs(angle3 - angle1);
  
  if (aTotalAngle > vtkMath::DoublePi())
    aTotalAngle = 2*vtkMath::DoublePi()-aTotalAngle;
  */
  
  double aTotalAngle = 0;
  IncOrder aOrder = GetArcAngle(angle1,angle2,angle3,&aTotalAngle);
  
  vtkUnstructuredGrid *aC = NULL;

  if(aTotalAngle > aMaxAngle) {
    int nbSteps = int(aTotalAngle/aMaxAngle)+1;
    double anIncrementAngle = aTotalAngle/nbSteps;
    double aCurrentAngle = angle1;
    if(aOrder == VTKViewer_ArcBuilder::MINUS)
      aCurrentAngle-=anIncrementAngle;
    else
      aCurrentAngle+=anIncrementAngle;
#ifdef _MY_DEBUG_
    cout<<"Total angle :"<<aTotalAngle<<endl;
    cout<<"Max Increment Angle :"<<aMaxAngle<<endl;
    cout<<"Real Increment angle :"<<anIncrementAngle<<endl;
    cout<<"Nb Steps : "<<nbSteps<<endl;
#endif
  
    PntList aList;
    aList.push_back(myPnt1);
    for(int i=0;i<nbSteps-1;i++){
      double x = xCenter + aRadius*cos(aCurrentAngle);
      double y = yCenter + aRadius*sin(aCurrentAngle);
      Pnt aPnt(x,y,z);
      aList.push_back(aPnt);
      if(aOrder == VTKViewer_ArcBuilder::MINUS)
        aCurrentAngle-=anIncrementAngle;
      else
        aCurrentAngle+=anIncrementAngle;
    }
    aList.push_back(myPnt3);
    
    aC = BuildGrid(aList);
#ifdef _MY_DEBUG_
  cout<<"angle1 : "<<angle1*vtkMath::DoubleRadiansToDegrees()<<endl;
  cout<<"angle2 : "<<angle2*vtkMath::DoubleRadiansToDegrees()<<endl;
  cout<<"angle3 : "<<angle3*vtkMath::DoubleRadiansToDegrees()<<endl;
#endif
  }
  else{
    PntList aList;
    aList.push_back(myPnt1);
    aList.push_back(myPnt2);
    aList.push_back(myPnt3);
    aC = BuildGrid(aList);
  }
  return aC;
}

double VTKViewer_ArcBuilder::
GetPointAngleOnCircle(const double theXCenter, const double theYCenter,
                      const double theXPoint, const double theYPoint){
  double result = atan2(theYCenter - theYPoint, theXPoint - theXCenter);
  if(result < 0 )
    result = result+vtkMath::DoublePi()*2;
  return vtkMath::DoublePi()*2-result;
  return result;
}

vtkPoints* VTKViewer_ArcBuilder::GetPoints(){
  return myPoints;
}

VTKViewer_ArcBuilder::IncOrder VTKViewer_ArcBuilder::GetArcAngle( const double& P1, const double& P2, const double& P3,double* Ang){
  IncOrder aResult;
  if(P1 < P2 && P2 < P3){
    *Ang = P3 - P1;
    aResult = VTKViewer_ArcBuilder::PLUS;
  }
  else if((P1 < P3 && P3 < P2) || (P2 < P1 && P1 < P3)){
    *Ang = 2*vtkMath::DoublePi() - P3 + P1;
    aResult = VTKViewer_ArcBuilder::MINUS;
  }
  else if((P2 < P3 && P3 < P1) || (P3 < P1 && P1 < P2)){
    *Ang = 2*vtkMath::DoublePi() - P1 + P3;
    aResult = VTKViewer_ArcBuilder::PLUS;
  }
  else if(P3 < P2 && P2 < P1){
    *Ang = P1 - P3;
    aResult = VTKViewer_ArcBuilder::MINUS;
  }
  return aResult;
}

//------------------------------------------------------------------------
vtkIdType Build1DArc(vtkIdType cellId, vtkUnstructuredGrid* input, 
                     vtkPolyData *output,vtkIdType *pts, 
                     vtkFloatingPointType myMaxArcAngle){
  
  vtkFloatingPointType coord[3];
  vtkIdType aResult = -1;
  vtkIdType *aNewPoints;

  vtkCell* aCell = input->GetCell(cellId);
  //Get All points from input cell
  aCell->GetPoints()->GetPoint(0,coord);
  Pnt P0(coord[0],coord[1],coord[2]);
  aCell->GetPoints()->GetPoint(1,coord);
  Pnt P1(coord[0],coord[1],coord[2]);
  aCell->GetPoints()->GetPoint(2,coord);
  Pnt P2(coord[0],coord[1],coord[2]);

  VTKViewer_ArcBuilder aBuilder(P0,P2,P1,myMaxArcAngle);
  if (aBuilder.GetStatus() != VTKViewer_ArcBuilder::Arc_Done) {
    return aResult;
  }
  else{
    vtkPoints* aPoints = aBuilder.GetPoints();
    vtkIdType aNbPts = aPoints->GetNumberOfPoints();
    aNewPoints = new vtkIdType[aNbPts];
    vtkIdType curID;
    vtkIdType aCellType = VTK_POLY_LINE;
    
    aNewPoints[0] = pts[0];
    for(vtkIdType idx = 1; idx < aNbPts-1;idx++) {
      curID = output->GetPoints()->InsertNextPoint(aPoints->GetPoint(idx));
      aNewPoints[idx] = curID;
    }
    aNewPoints[aNbPts-1] = pts[1];
    
    aResult = output->InsertNextCell(aCellType,aNbPts,aNewPoints);
    return aResult;
  } 
}

/*!
 * Add all points from the input vector theCollection into thePoints. 
 * Array theIds - it is array with ids of added points.
 */
vtkIdType MergevtkPoints(const std::vector<vtkPoints*>& theCollection, vtkPoints* thePoints, vtkIdType* &theIds){
  vtkIdType aNbPoints = 0;
  vtkIdType anIdCounter = 0;
  vtkIdType aNewPntId = 0;
  
  //Compute number of points
  std::vector<vtkPoints*>::const_iterator it = theCollection.begin();
  for(;it != theCollection.end();it++){
    vtkPoints* aPoints = *it;
    if(aPoints) { 
      aNbPoints += aPoints->GetNumberOfPoints()-1; //Because we add all points except last
    }
  }
  it = theCollection.begin();
  theIds = new vtkIdType[aNbPoints];
  // ..and add all points
  for(;it != theCollection.end();it++){
    vtkPoints* aPoints = *it;
    
    if(aPoints){
      for(vtkIdType idx = 0;idx < aPoints->GetNumberOfPoints()-1;idx++){
        aNewPntId = thePoints->InsertNextPoint(aPoints->GetPoint(idx));
        theIds[anIdCounter] = aNewPntId;
        anIdCounter++;
      }
    }
  }
  return aNbPoints;
}
