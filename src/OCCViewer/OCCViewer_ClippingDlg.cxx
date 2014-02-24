// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "OCCViewer_ClippingDlg.h"

#include <QtxDoubleSpinBox.h>
#include <QtxDoubleSpinSlider.h>
#include <QtxIntSpinSlider.h>
#include <QtxAction.h>

#include "SUIT_Session.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_ViewManager.h"
#include "OCCViewer_ClipPlane.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewManager.h"
#include "OCCViewer_ClipPlaneInteractor.h"

#include <V3d_View.hxx>
#include <Visual3d_View.hxx>
#include <Geom_Plane.hxx>
#include <Prs3d_Presentation.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <math.h>

// QT Includes
#include <QApplication>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QStackedLayout>
#include <QSlider>
#include <QMenu>

/**********************************************************************************
 ************************        Internal functions        ************************
 *********************************************************************************/

void getMinMaxFromContext( Handle(AIS_InteractiveContext) ic,
			   double  theDefaultSize,
			   double& theXMin,
			   double& theYMin,
			   double& theZMin,
			   double& theXMax,
			   double& theYMax,
			   double& theZMax) {

  double aXMin, aYMin, aZMin, aXMax, aYMax, aZMax;
  aXMin = aYMin = aZMin = DBL_MAX;
  aXMax = aYMax = aZMax = -DBL_MAX;
  
  bool isFound = false;
  AIS_ListOfInteractive aList;
  ic->DisplayedObjects( aList );
  for ( AIS_ListIteratorOfListOfInteractive it( aList ); it.More(); it.Next() ) {
    Handle(AIS_InteractiveObject) anObj = it.Value();
    if ( !anObj.IsNull() && anObj->HasPresentation() &&
         !anObj->IsKind( STANDARD_TYPE(AIS_Plane) ) ) {
      Handle(Prs3d_Presentation) aPrs = anObj->Presentation();
      if ( !aPrs->IsEmpty() && !aPrs->IsInfinite() ) {
        isFound = true;
        double xmin, ymin, zmin, xmax, ymax, zmax;
        aPrs->MinMaxValues( xmin, ymin, zmin, xmax, ymax, zmax );
        aXMin = qMin( aXMin, xmin );  aXMax = qMax( aXMax, xmax );
        aYMin = qMin( aYMin, ymin );  aYMax = qMax( aYMax, ymax );
        aZMin = qMin( aZMin, zmin );  aZMax = qMax( aZMax, zmax );
      }
    }
  }

  if(!isFound) {
    if(theDefaultSize == 0.0)
      theDefaultSize = 100.;
    aXMin = aYMin = aZMin = -theDefaultSize;
    aXMax = aYMax = aZMax = theDefaultSize;
  }
  theXMin = aXMin;theYMin = aYMin;theZMin = aZMin;
  theXMax = aXMax;theYMax = aYMax;theZMax = aZMax;
}

/*!
  Compute the point of bounding box and current clipping plane
 */
void ComputeBoundsParam( double theBounds[6],
                         double theDirection[3],
                         double theMinPnt[3],
                         double& theMaxBoundPrj,
                         double& theMinBoundPrj )
{
  //Enlarge bounds in order to avoid conflicts of precision
  for(int i = 0; i < 6; i += 2) {
    static double EPS = 1.0E-3;
    double aDelta = (theBounds[i+1] - theBounds[i])*EPS;
    theBounds[i] -= aDelta;
    theBounds[i+1] += aDelta;
  }

  double aBoundPoints[8][3] = { { theBounds[0], theBounds[2], theBounds[4] },
                                { theBounds[1], theBounds[2], theBounds[4] },
                                { theBounds[0], theBounds[3], theBounds[4] },
                                { theBounds[1], theBounds[3], theBounds[4] },
                                { theBounds[0], theBounds[2], theBounds[5] },
                                { theBounds[1], theBounds[2], theBounds[5] },
                                { theBounds[0], theBounds[3], theBounds[5] },
                                { theBounds[1], theBounds[3], theBounds[5] } };

  int aMaxId = 0;
  theMaxBoundPrj = theDirection[0] * aBoundPoints[aMaxId][0] + theDirection[1] * aBoundPoints[aMaxId][1]
                   + theDirection[2] * aBoundPoints[aMaxId][2];
  theMinBoundPrj = theMaxBoundPrj;
  for(int i = 1; i < 8; i++) {
    double aTmp = theDirection[0] * aBoundPoints[i][0] + theDirection[1] * aBoundPoints[i][1]
                  + theDirection[2] * aBoundPoints[i][2];
    if(theMaxBoundPrj < aTmp) {
      theMaxBoundPrj = aTmp;
      aMaxId = i;
    }
    if(theMinBoundPrj > aTmp) {
      theMinBoundPrj = aTmp;
    }
  }
  double *aMinPnt = aBoundPoints[aMaxId];
  theMinPnt[0] = aMinPnt[0];
  theMinPnt[1] = aMinPnt[1];
  theMinPnt[2] = aMinPnt[2];
}

/*!
  Compute the position of current plane by distance
 */
void DistanceToPosition( double theBounds[6],
                         double theDirection[3],
                         double theDist,
                         double thePos[3] )
{
  double aMaxBoundPrj, aMinBoundPrj, aMinPnt[3];
  ComputeBoundsParam( theBounds,theDirection,aMinPnt,aMaxBoundPrj,aMinBoundPrj );
  double aLength = (aMaxBoundPrj - aMinBoundPrj)*theDist;
  thePos[0] = aMinPnt[0] - theDirection[0]*aLength;
  thePos[1] = aMinPnt[1] - theDirection[1]*aLength;
  thePos[2] = aMinPnt[2] - theDirection[2]*aLength;
}

/*!
  Compute the parameters of clipping plane
 */
bool ComputeClippingPlaneParameters( double theNormal[3],
                                     double theDist,
                                     double theOrigin[3],
                                     Handle(AIS_InteractiveContext) ic,
				     double theDefaultSize)
{
  double aBounds[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  getMinMaxFromContext(ic,theDefaultSize,aBounds[0], aBounds[2], aBounds[4], aBounds[1], aBounds[3], aBounds[5]);

  DistanceToPosition( aBounds, theNormal, theDist, theOrigin );
  return true;
}

/*!
  Cross product of two 3-vectors. Result vector in result[3].
 */
void Cross(const double first[3], const double second[3], double result[3])
{
  result[0] = first[1]*second[2] - first[2]*second[1];
  result[1] = first[2]*second[0] - first[0]*second[2];
  result[2] = first[0]*second[1] - first[1]*second[0];
}

/*!
  Compute relative clipping plane in absolute coordinates
 */
void relativePlaneToAbsolute (OCCViewer_ClipPlane& thePlane, Handle(AIS_InteractiveContext) ic, double theDefaultSize )
{
  double aNormal[3];
  double aDir[2][3] = { { 0, 0, 0 }, { 0, 0, 0 } };
  {
    static double aCoeff = M_PI/180.0;

    double anU[2] = { cos( aCoeff * thePlane.RelativeMode.Rotation1 ), cos( aCoeff * thePlane.RelativeMode.Rotation2 ) };
    double aV[2] = { sqrt( 1.0 - anU[0]*anU[0] ), sqrt( 1.0 - anU[1] * anU[1] ) };
    aV[0] = thePlane.RelativeMode.Rotation1 > 0? aV[0]: -aV[0];
    aV[1] = thePlane.RelativeMode.Rotation2 > 0? aV[1]: -aV[1];

    switch ( thePlane.RelativeMode.Orientation ) {
    case 0:
      aDir[0][1] = anU[0];
      aDir[0][2] = aV[0];
      aDir[1][0] = anU[1];
      aDir[1][2] = aV[1];
      break;
    case 1:
      aDir[0][2] = anU[0];
      aDir[0][0] = aV[0];
      aDir[1][1] = anU[1];
      aDir[1][0] = aV[1];
      break;
    case 2:
      aDir[0][0] = anU[0];
      aDir[0][1] = aV[0];
      aDir[1][2] = anU[1];
      aDir[1][1] = aV[1];
      break;
    }

    Cross( aDir[1], aDir[0], aNormal );
    // Normalize
    double den;
    den = sqrt( aNormal[0] * aNormal[0] + aNormal[1] * aNormal[1] + aNormal[2] * aNormal[2] );
    if ( den != 0.0 ) {
      for (int i=0; i < 3; i++) {
        aNormal[i] /= den;
      }
    }
  }

  double anOrigin[3];

  anOrigin[0] = anOrigin[1] = anOrigin[2] = 0;
  bool anIsOk = true;

  anIsOk = ComputeClippingPlaneParameters( aNormal,
                                           thePlane.RelativeMode.Distance,
                                           anOrigin,
                                           ic,
					   theDefaultSize );
  if( !anIsOk )
	  return;
  thePlane.X = anOrigin[0];
  thePlane.Y = anOrigin[1];
  thePlane.Z = anOrigin[2];
  thePlane.Dx = aNormal[0];
  thePlane.Dy = aNormal[1];
  thePlane.Dz = aNormal[2];
}

/*!
  \brief Converts absolute plane definition to relative system.
  \param thePlane [in/out] the plane to convert.
  \param theIC [in] the interactive context.
  \param theDefaultSize [in] the default trihedron size.
 */
void absolutePlaneToRelative( OCCViewer_ClipPlane& thePlane, Handle(AIS_InteractiveContext) theIC, double theDefaultSize )
{
  gp_Pnt aPlaneP( thePlane.X, thePlane.Y, thePlane.Z );
  gp_Dir aPlaneN( thePlane.Dx, thePlane.Dy, thePlane.Dz );

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;

  getMinMaxFromContext( theIC, theDefaultSize, aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

  Bnd_Box aMinMax;
  aMinMax.Update( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

  gp_Trsf aRelativeTransform;
  aRelativeTransform.SetTransformation( gp_Ax3(), gp_Ax3( aPlaneP, aPlaneN ) );
  Bnd_Box aRelativeBounds = aMinMax.Transformed( aRelativeTransform );

  aRelativeBounds.Get( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

  double aLength   = aZmax - aZmin;
  double aDistance = aZmax;

  double aRelativeDistance = aLength > 0.01 ? aDistance / aLength : 0.0;
  aRelativeDistance = qMin( aRelativeDistance, aLength );
  aRelativeDistance = qMax( aRelativeDistance, 0.0 );
  thePlane.RelativeMode.Distance = aRelativeDistance;

  const gp_Dir& aDX = gp::DX();
  const gp_Dir& aDY = gp::DY();
  const gp_Dir& aDZ = gp::DZ();
  double anAng1 = 0.0;
  double anAng2 = 0.0;
  switch ( thePlane.RelativeMode.Orientation )
  {
    case 0:
    {
      if ( aDY.IsParallel( aPlaneN, Precision::Angular() ) )
      {
        anAng1 = 0.0;
        anAng2 = 0.0;
        break;
      }

      if ( aDX.IsParallel( aPlaneN, Precision::Angular() ) )
      {
        anAng1 = 0.0;
        anAng2 = 0.0;
        break;
      }

      gp_Dir aDir1 = aPlaneN ^ aDX;
      gp_Dir aDir2 = aDY ^ aPlaneN;
      gp_Ax3 aRightHand( gp::Origin(), aPlaneN, aDY ^ aPlaneN );

      if ( aDir1 * aRightHand.YDirection() < 0.0 )
      {
        aDir1.Reverse();
      }
      if ( aDir2 * aRightHand.XDirection() < 0.0 )
      {
        aDir2.Reverse();
      }

      anAng1 = aDY.AngleWithRef( aDir1,  aDX );
      anAng2 = aDX.AngleWithRef( aDir2, -aDY );
      break;
    }
    case 1:
    {
      if ( aDZ.IsParallel( aPlaneN, Precision::Angular() ) )
      {
        anAng1 = 0.0;
        anAng2 = 0.0;
        break;
      }

      if ( aDY.IsParallel( aPlaneN, Precision::Angular() ) )
      {
        anAng1 = 0.0;
        anAng2 = 0.0;
        break;
      }

      gp_Dir aDir1 = aPlaneN ^ aDY;
      gp_Dir aDir2 = aDZ ^ aPlaneN;
      gp_Ax3 aRightHand( gp::Origin(), aPlaneN, aDZ ^ aPlaneN );

      if ( aDir1 * aRightHand.YDirection() < 0.0 )
      {
        aDir1.Reverse();
      }
      if ( aDir2 * aRightHand.XDirection() < 0.0 )
      {
        aDir2.Reverse();
      }

      anAng1 = aDZ.AngleWithRef( aDir1,  aDY );
      anAng2 = aDY.AngleWithRef( aDir2, -aDZ );
      break;
    }
    case 2:
    {
      if ( aDX.IsParallel( aPlaneN, Precision::Angular() ) )
      {
        anAng1 = 0.0;
        anAng2 = 0.0;
        break;
      }

      if ( aDZ.IsParallel( aPlaneN, Precision::Angular() ) )
      {
        anAng1 = 0.0;
        anAng2 = 0.0;
        break;
      }

      gp_Dir aDir1 = aPlaneN ^ aDZ;
      gp_Dir aDir2 = aDX ^ aPlaneN;
      gp_Ax3 aRightHand( gp::Origin(), aPlaneN, aDX ^ aPlaneN );

      if ( aDir1 * aRightHand.YDirection() < 0.0 )
      {
        aDir1.Reverse();
      }
      if ( aDir2 * aRightHand.XDirection() < 0.0 )
      {
        aDir2.Reverse();
      }

      anAng1 = aDX.AngleWithRef( aDir1,  aDZ );
      anAng2 = aDZ.AngleWithRef( aDir2, -aDX );
      break;
    }
  }

  thePlane.RelativeMode.Rotation1 = anAng1 * ( 180.0 / M_PI );
  thePlane.RelativeMode.Rotation2 = anAng2 * ( 180.0 / M_PI );
}

/*!
  Compute clipping plane size base point and normal
 */

void clipPlaneParams(OCCViewer_ClipPlane& theClipPlane, Handle(AIS_InteractiveContext) theContext,
                     double& theSize, gp_Pnt& theBasePnt, gp_Dir& theNormal, double defaultSize) {
  double aXMin, aYMin, aZMin, aXMax, aYMax, aZMax;
  aXMin = aYMin = aZMin = DBL_MAX;
  aXMax = aYMax = aZMax = -DBL_MAX;
  
  getMinMaxFromContext(theContext,defaultSize,aXMin, aYMin, aZMin, aXMax, aYMax, aZMax);
  double aSize = 50;


  gp_Pnt aBasePnt(theClipPlane.X ,  theClipPlane.Y ,  theClipPlane.Z);
  gp_Dir aNormal(theClipPlane.Dx, theClipPlane.Dy, theClipPlane.Dz );

  // compute clipping plane size
  gp_Pnt aCenter = gp_Pnt( ( aXMin + aXMax ) / 2, ( aYMin + aYMax ) / 2, ( aZMin + aZMax ) / 2 );
  double aDiag = aCenter.Distance( gp_Pnt( aXMax, aYMax, aZMax ) )*2;
  aSize = aDiag * 1.1;
  
  // compute clipping plane center ( redefine the base point )
  IntAna_IntConicQuad intersector = IntAna_IntConicQuad();
  
  intersector.Perform( gp_Lin( aCenter, aNormal), gp_Pln( aBasePnt, aNormal), Precision::Confusion() );

  if ( intersector.IsDone() && intersector.NbPoints() == 1 )
    aBasePnt = intersector.Point( 1 );
  
  theSize = aSize;
  theBasePnt = aBasePnt;
  theNormal = aNormal;
}


/*********************************************************************************
 *********************      class OCCViewer_ClippingDlg      *********************
 *********************************************************************************/
/*!
  Constructor
  \param view - view window
  \param parent - parent widget
*/
OCCViewer_ClippingDlg::OCCViewer_ClippingDlg(OCCViewer_ViewWindow* parent , OCCViewer_Viewer* model)
  : QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint ),
    myCurrentClipPlaneMode (Absolute)
{
  setObjectName( "OCCViewer_ClippingDlg" );
  setModal( false );

  setWindowTitle( tr( "Clipping" ) );

  setAttribute (Qt::WA_DeleteOnClose, true);
  
  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 11 ); topLayout->setSpacing( 6 );

  /***************************************************************/
  // Controls for selecting, creating, deleting planes
  QGroupBox* GroupPlanes = new QGroupBox( tr("CLIPPING_PLANES"), this );
  QHBoxLayout* GroupPlanesLayout = new QHBoxLayout( GroupPlanes );
  ComboBoxPlanes = new QComboBox( GroupPlanes );
  isActivePlane = new QCheckBox( tr("IS_ACTIVE_PLANE"), this );
  buttonNew = new QPushButton( tr("BTN_NEW"), GroupPlanes );
  buttonDelete = new QPushButton( tr("BTN_DELETE"), GroupPlanes );
  buttonDisableAll = new QPushButton( tr("BTN_DISABLE_ALL"), GroupPlanes );
  MenuMode = new QMenu( "MenuMode", buttonNew );
  MenuMode->addAction( tr( "ABSOLUTE" ), this, SLOT( onModeAbsolute() ) );
  MenuMode->addAction( tr( "RELATIVE" ), this, SLOT( onModeRelative() ) );
  buttonNew->setMenu( MenuMode );

  GroupPlanesLayout->addWidget( ComboBoxPlanes );
  GroupPlanesLayout->addWidget( isActivePlane );
  GroupPlanesLayout->addWidget( buttonNew );
  GroupPlanesLayout->addWidget( buttonDelete );
  GroupPlanesLayout->addWidget( buttonDisableAll );

  ModeStackedLayout = new QStackedLayout();

  /**********************   Mode Absolute   **********************/
  /* Controls for absolute mode of clipping plane:
     X, Y, Z - coordinates of the intersection of cutting plane and the three axes
     Dx, Dy, Dz - components of normal to the cutting plane
     Orientation - direction of cutting plane
   */
  const double min = -1e+7;
  const double max =  1e+7;
  const double step = 5;
  const int precision = -7;

  // Croup Point
  QGroupBox* GroupAbsolutePoint = new QGroupBox( this );
  GroupAbsolutePoint->setObjectName( "GroupPoint" );
  GroupAbsolutePoint->setTitle( tr("BASE_POINT") );
  QGridLayout* GroupPointLayout = new QGridLayout( GroupAbsolutePoint );
  GroupPointLayout->setAlignment( Qt::AlignTop );
  GroupPointLayout->setSpacing( 6 ); GroupPointLayout->setMargin( 11 );

  TextLabelX = new QLabel( GroupAbsolutePoint );
  TextLabelX->setObjectName( "TextLabelX" );
  TextLabelX->setText( tr("X:") );
  GroupPointLayout->addWidget( TextLabelX, 0, 0 );
  
  SpinBox_X = new QtxDoubleSpinBox( min, max, step, GroupAbsolutePoint );
  SpinBox_X->setObjectName("SpinBox_X" );
  SpinBox_X->setPrecision( precision );
  GroupPointLayout->addWidget( SpinBox_X, 0, 1 );

  TextLabelY = new QLabel( GroupAbsolutePoint );
  TextLabelY->setObjectName( "TextLabelY" );
  TextLabelY->setText( tr("Y:") );
  GroupPointLayout->addWidget( TextLabelY, 0, 2 );

  SpinBox_Y = new QtxDoubleSpinBox( min, max, step, GroupAbsolutePoint );
  SpinBox_Y->setObjectName("SpinBox_Y" );
  SpinBox_Y->setPrecision( precision );
  GroupPointLayout->addWidget( SpinBox_Y, 0, 3 );

  TextLabelZ = new QLabel( GroupAbsolutePoint );
  TextLabelZ->setObjectName( "TextLabelZ" );
  TextLabelZ->setText( tr("Z:") );
  GroupPointLayout->addWidget( TextLabelZ, 0, 4 );

  SpinBox_Z = new QtxDoubleSpinBox( min, max, step, GroupAbsolutePoint );
  SpinBox_Z->setObjectName("SpinBox_Z" );
  SpinBox_Z->setPrecision( precision );
  GroupPointLayout->addWidget( SpinBox_Z, 0, 5 );

  resetButton  = new QPushButton( GroupAbsolutePoint );
  resetButton->setObjectName( "resetButton" );
  resetButton->setText( tr( "RESET"  ) );
  GroupPointLayout->addWidget( resetButton, 0, 6 );

  // Group Direction
  GroupAbsoluteDirection = new QGroupBox( this );
  GroupAbsoluteDirection->setObjectName( "GroupDirection" );
  GroupAbsoluteDirection->setTitle( tr("DIRECTION") );
  QGridLayout* GroupDirectionLayout = new QGridLayout( GroupAbsoluteDirection );
  GroupDirectionLayout->setAlignment( Qt::AlignTop );
  GroupDirectionLayout->setSpacing( 6 );
  GroupDirectionLayout->setMargin( 11 );
  
  TextLabelDx = new QLabel( GroupAbsoluteDirection );
  TextLabelDx->setObjectName( "TextLabelDx" );
  TextLabelDx->setText( tr("Dx:") );
  GroupDirectionLayout->addWidget( TextLabelDx, 0, 0 );
  
  SpinBox_Dx = new QtxDoubleSpinBox( min, max, step, GroupAbsoluteDirection );
  SpinBox_Dx->setObjectName("SpinBox_Dx" );
  SpinBox_Dx->setPrecision( precision );
  GroupDirectionLayout->addWidget( SpinBox_Dx, 0, 1 );

  TextLabelDy = new QLabel( GroupAbsoluteDirection );
  TextLabelDy->setObjectName( "TextLabelDy" );
  TextLabelDy->setText( tr("Dy:") );
  GroupDirectionLayout->addWidget( TextLabelDy, 0, 2 );
  
  SpinBox_Dy = new QtxDoubleSpinBox( min, max, step, GroupAbsoluteDirection );
  SpinBox_Dy->setObjectName("SpinBox_Dy" );
  SpinBox_Dy->setPrecision( precision );
  GroupDirectionLayout->addWidget( SpinBox_Dy, 0, 3 );

  TextLabelDz = new QLabel( GroupAbsoluteDirection );
  TextLabelDz->setObjectName( "TextLabelDz" );
  TextLabelDz->setText( tr("Dz:") );
  GroupDirectionLayout->addWidget( TextLabelDz, 0, 4 );
  
  SpinBox_Dz = new QtxDoubleSpinBox( min, max, step, GroupAbsoluteDirection );
  SpinBox_Dz->setObjectName("SpinBox_Dz" );
  SpinBox_Dz->setPrecision( precision );
  GroupDirectionLayout->addWidget( SpinBox_Dz, 0, 5 );

  invertButton  = new QPushButton( GroupAbsoluteDirection );
  invertButton->setObjectName( "invertButton" );
  invertButton->setText( tr( "INVERT"  ) );
  GroupDirectionLayout->addWidget( invertButton, 0, 6 );
 
  CBAbsoluteOrientation = new QComboBox( GroupAbsoluteDirection );
  CBAbsoluteOrientation->setObjectName( "AbsoluteOrientation" );
  CBAbsoluteOrientation->insertItem( CBAbsoluteOrientation->count(), tr( "CUSTOM" ) );
  CBAbsoluteOrientation->insertItem( CBAbsoluteOrientation->count(), tr( "||X-Y" ) );
  CBAbsoluteOrientation->insertItem( CBAbsoluteOrientation->count(), tr( "||Y-Z" ) );
  CBAbsoluteOrientation->insertItem( CBAbsoluteOrientation->count(), tr( "||Z-X" ) );
  GroupDirectionLayout->addWidget( CBAbsoluteOrientation, 1, 0, 1, 6 );

  QVBoxLayout* ModeActiveLayout = new QVBoxLayout();
  ModeActiveLayout->setMargin( 11 ); ModeActiveLayout->setSpacing( 6 );
  ModeActiveLayout->addWidget( GroupAbsolutePoint );
  ModeActiveLayout->addWidget( GroupAbsoluteDirection );

  QWidget* ModeActiveWidget = new QWidget( this );
  ModeActiveWidget->setLayout( ModeActiveLayout );

  /**********************   Mode Relative   **********************/
  /* Controls for relative mode of clipping plane:
     Distance - Value from 0 to 1.
     Specifies the distance from the minimum value in a given direction of bounding box to the current position
     Rotation1, Rotation2 - turn angles of cutting plane in given directions
     Orientation - direction of cutting plane
   */
  QGroupBox* GroupParameters = new QGroupBox( tr("PARAMETERS"), this );
  QGridLayout* GroupParametersLayout = new QGridLayout( GroupParameters );
  GroupParametersLayout->setMargin( 11 ); GroupParametersLayout->setSpacing( 6 );

  TextLabelOrientation = new QLabel( tr("ORIENTATION"), GroupParameters);
  TextLabelOrientation->setObjectName( "TextLabelOrientation" );
  GroupParametersLayout->addWidget( TextLabelOrientation, 0, 0 );

  CBRelativeOrientation = new QComboBox(GroupParameters);
  CBRelativeOrientation->setObjectName( "RelativeOrientation" );
  CBRelativeOrientation->addItem( tr("ALONG_XY") );
  CBRelativeOrientation->addItem( tr("ALONG_YZ") );
  CBRelativeOrientation->addItem( tr("ALONG_ZX") );
  GroupParametersLayout->addWidget( CBRelativeOrientation, 0, 1 );

  TextLabelDistance = new QLabel( tr("DISTANCE"), GroupParameters );
  TextLabelDistance->setObjectName( "TextLabelDistance" );
  GroupParametersLayout->addWidget( TextLabelDistance, 1, 0 );
  
  SpinSliderDistance = new QtxDoubleSpinSlider( 0., 1., 0.01, GroupParameters );
  SpinSliderDistance->setObjectName( "SpinSliderDistance" );
  SpinSliderDistance->setPrecision( precision );
  QFont fnt = SpinSliderDistance->font(); fnt.setBold( true ); SpinSliderDistance->setFont( fnt );
  GroupParametersLayout->addWidget( SpinSliderDistance, 1, 1 );

  QString aUnitRot = "\xB0";

  TextLabelRotation1 = new QLabel( tr("ROTATION_AROUND_X_Y2Z"), GroupParameters );
  TextLabelRotation1->setObjectName( "TextLabelRotation1" );
  GroupParametersLayout->addWidget( TextLabelRotation1, 2, 0 );
  
  SpinSliderRotation1 = new QtxIntSpinSlider( -180, 180, 1, GroupParameters );
  SpinSliderRotation1->setObjectName( "SpinSliderRotation1" );
  SpinSliderRotation1->setUnit( aUnitRot );
  SpinSliderRotation1->setFont( fnt );
  GroupParametersLayout->addWidget( SpinSliderRotation1, 2, 1 );

  TextLabelRotation2 = new QLabel(tr("ROTATION_AROUND_Y_X2Z"), GroupParameters);
  TextLabelRotation2->setObjectName( "TextLabelRotation2" );
  TextLabelRotation2->setObjectName( "TextLabelRotation2" );
  GroupParametersLayout->addWidget( TextLabelRotation2, 3, 0 );

  SpinSliderRotation2 = new QtxIntSpinSlider( -180, 180, 1, GroupParameters );
  SpinSliderRotation2->setObjectName( "SpinSliderRotation2" );
  SpinSliderRotation2->setUnit( aUnitRot );
  SpinSliderRotation2->setFont( fnt );
  GroupParametersLayout->addWidget( SpinSliderRotation2, 3, 1 );

  /***************************************************************/
  QGroupBox* CheckBoxWidget = new QGroupBox( this );
  QHBoxLayout* CheckBoxLayout = new QHBoxLayout( CheckBoxWidget );
  
  PreviewCheckBox = new QCheckBox( tr("PREVIEW"), CheckBoxWidget );
  PreviewCheckBox->setObjectName( "PreviewCheckBox" );
  PreviewCheckBox->setChecked( true );
  CheckBoxLayout->addWidget( PreviewCheckBox, 0, Qt::AlignCenter );
  
  AutoApplyCheckBox = new QCheckBox( tr("AUTO_APPLY"), CheckBoxWidget );
  AutoApplyCheckBox->setObjectName( "AutoApplyCheckBox" );
  CheckBoxLayout->addWidget( AutoApplyCheckBox, 0, Qt::AlignCenter );

  /***************************************************************/
  QGroupBox* GroupButtons = new QGroupBox( this );
  QHBoxLayout* GroupButtonsLayout = new QHBoxLayout( GroupButtons );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setMargin( 11 ); GroupButtonsLayout->setSpacing( 6 );
  
  buttonOk = new QPushButton( GroupButtons );
  buttonOk->setObjectName( "buttonOk" );
  buttonOk->setText( tr( "BUT_APPLY_AND_CLOSE"  ) );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );
  GroupButtonsLayout->addWidget( buttonOk );

  buttonApply = new QPushButton( GroupButtons );
  buttonApply->setObjectName( "buttonApply" );
  buttonApply->setText( tr( "BUT_APPLY"  ) );
  buttonApply->setAutoDefault( TRUE );
  buttonApply->setDefault( TRUE );
  GroupButtonsLayout->addWidget( buttonApply );

  GroupButtonsLayout->addStretch();
  
  buttonClose = new QPushButton( GroupButtons );
  buttonClose->setObjectName( "buttonClose" );
  buttonClose->setText( tr( "BUT_CLOSE"  ) );
  buttonClose->setAutoDefault( TRUE );
  GroupButtonsLayout->addWidget( buttonClose );

  QPushButton* buttonHelp = new QPushButton( tr( "SMESH_BUT_HELP" ), GroupButtons );
  buttonHelp->setAutoDefault( TRUE );
  GroupButtonsLayout->addWidget( buttonHelp );

  /***************************************************************/
  ModeStackedLayout->addWidget( ModeActiveWidget );
  ModeStackedLayout->addWidget( GroupParameters );

  topLayout->addWidget( GroupPlanes );
  topLayout->addLayout( ModeStackedLayout );
  topLayout->addWidget( CheckBoxWidget );
  topLayout->addWidget( GroupButtons );

  this->setLayout( topLayout );

  // Initializations
  initParam();

  // Signals and slots connections
  connect( ComboBoxPlanes, SIGNAL( activated( int ) ), this, SLOT( onSelectPlane( int ) ) );
  connect( isActivePlane,  SIGNAL ( toggled ( bool ) ),  this, SLOT( onValueChanged() ) );
  connect( buttonNew, SIGNAL( clicked() ), buttonNew, SLOT( showMenu() ) );
  connect( buttonDelete, SIGNAL( clicked() ), this, SLOT( ClickOnDelete() ) );
  connect( buttonDisableAll, SIGNAL( clicked() ), this, SLOT( ClickOnDisableAll() ) );

  connect( resetButton,  SIGNAL (clicked() ), this, SLOT( onReset() ) );
  connect( invertButton, SIGNAL (clicked() ), this, SLOT( onInvert() ) ) ;
  connect( SpinBox_X,  SIGNAL ( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( SpinBox_Y,  SIGNAL ( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( SpinBox_Z,  SIGNAL ( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( SpinBox_Dx, SIGNAL ( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( SpinBox_Dy, SIGNAL ( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( SpinBox_Dz, SIGNAL ( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( CBAbsoluteOrientation, SIGNAL ( activated ( int ) ), this, SLOT( onOrientationAbsoluteChanged( int ) ) ) ;

  connect( CBRelativeOrientation, SIGNAL( activated( int ) ), this, SLOT( onOrientationRelativeChanged( int ) ) );
  connect( SpinSliderDistance, SIGNAL( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( SpinSliderRotation1, SIGNAL( valueChanged( int ) ), this, SLOT( onValueChanged() ) );
  connect( SpinSliderRotation2, SIGNAL( valueChanged( int ) ), this, SLOT( onValueChanged() ) );

  connect( PreviewCheckBox, SIGNAL ( toggled ( bool ) ), this, SLOT( onPreview( bool ) ) ) ;
  connect( AutoApplyCheckBox, SIGNAL ( toggled( bool ) ), this, SLOT( onAutoApply( bool ) ) );
  
  connect( buttonClose, SIGNAL( clicked() ), this, SLOT( ClickOnClose() ) ) ;
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply, SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( ClickOnHelp() ) );

  myBusy = false;
  myIsSelectPlane = false;
  myIsPlaneCreation = false;
  myIsUpdatingControls = false;
  myModel = model;

  myModel->getViewer3d()->InitActiveViews();

  OCCViewer_ViewManager* aViewMgr = (OCCViewer_ViewManager*) myModel->getViewManager();
  myInteractor = new OCCViewer_ClipPlaneInteractor( aViewMgr, this );
  connect( myInteractor, SIGNAL( planeClicked( const Handle_AIS_Plane& ) ), SLOT( onPlaneClicked( const Handle_AIS_Plane& ) ) );
  connect( myInteractor, SIGNAL( planeDragged( const Handle_AIS_Plane& ) ), SLOT( onPlaneDragged( const Handle_AIS_Plane& ) ) );

  myLocalPlanes = myModel->getClipPlanes();
  synchronize();
}

/*!
  Destructor
  Destroys the object and frees any allocated resources
*/
OCCViewer_ClippingDlg::~OCCViewer_ClippingDlg()
{
  myLocalPlanes.clear();
}

/*!
  Custom handling of close event: erases preview
*/
void OCCViewer_ClippingDlg::closeEvent( QCloseEvent* e )
{
  erasePreview();
  QDialog::closeEvent( e );
  OCCViewer_ViewWindow* v = qobject_cast<OCCViewer_ViewWindow*>(parent());
  if(v)
    v->onClipping(false);
}

/*!
  Custom handling of show event: displays preview
*/
void OCCViewer_ClippingDlg::showEvent( QShowEvent* e )
{
  QDialog::showEvent( e );
  onPreview( PreviewCheckBox->isChecked() );
}

/*!
  Custom handling of hide event: erases preview
*/
void OCCViewer_ClippingDlg::hideEvent( QHideEvent* e )
{
  erasePreview();
  QDialog::hideEvent( e );
  OCCViewer_ViewWindow* v = qobject_cast<OCCViewer_ViewWindow*>(parent());
  if(v)
    v->onClipping(false);

}

/*!
  Initialization of initial values of widgets
*/
void OCCViewer_ClippingDlg::initParam()
{
  SpinBox_X->setValue( 0.0 );
  SpinBox_Y->setValue( 0.0 );
  SpinBox_Z->setValue( 0.0 );

  SpinBox_Dx->setValue( 1.0 );
  SpinBox_Dy->setValue( 1.0 );
  SpinBox_Dz->setValue( 1.0 );

  CBAbsoluteOrientation->setCurrentIndex(0);

  SpinSliderDistance->setValue( 0.5 );
  SpinSliderRotation1->setValue( 0 );
  SpinSliderRotation2->setValue( 0 );
  CBRelativeOrientation->setCurrentIndex( 0 );
}

/*!
  Synchronize dialog's widgets with data
*/
void OCCViewer_ClippingDlg::synchronize()
{
  ComboBoxPlanes->clear();
  int aNbPlanesAbsolute = myLocalPlanes.size();

  QString aName;
  for(int i = 1; i<=aNbPlanesAbsolute; i++ ) {
    aName = QString("Plane %1").arg(i);
    ComboBoxPlanes->addItem( aName );
  }

  int aPos = ComboBoxPlanes->count() - 1;
  ComboBoxPlanes->setCurrentIndex( aPos );

  bool anIsControlsEnable = ( aPos >= 0 );
  if ( anIsControlsEnable ) {
    onSelectPlane( aPos );
  }
  else {
    ComboBoxPlanes->addItem( tr( "NO_PLANES" ) );
    initParam();
  }
  if ( myCurrentClipPlaneMode == Absolute ) {
    SpinBox_X->setEnabled( anIsControlsEnable );
    SpinBox_Y->setEnabled( anIsControlsEnable );
    SpinBox_Z->setEnabled( anIsControlsEnable );
    SpinBox_Dx->setEnabled( anIsControlsEnable );
    SpinBox_Dy->setEnabled( anIsControlsEnable );
    SpinBox_Dz->setEnabled( anIsControlsEnable );
    CBAbsoluteOrientation->setEnabled( anIsControlsEnable );
    invertButton->setEnabled( anIsControlsEnable );
    resetButton->setEnabled( anIsControlsEnable );
  }
  else if( myCurrentClipPlaneMode == Relative ) {
    CBRelativeOrientation->setEnabled( anIsControlsEnable );
    SpinSliderDistance->setEnabled( anIsControlsEnable );
    SpinSliderRotation1->setEnabled( anIsControlsEnable );
    SpinSliderRotation2->setEnabled( anIsControlsEnable );
    isActivePlane->setEnabled( anIsControlsEnable );
  }
  isActivePlane->setEnabled( anIsControlsEnable );
}

/*!
  Displays preview of clipping plane
*/
void OCCViewer_ClippingDlg::displayPreview()
{
  if ( myBusy || !isValid() || !myModel)
    return;

  Handle(AIS_InteractiveContext) ic = myModel->getAISContext();
  
  int aCurPlaneIndex = ComboBoxPlanes->currentIndex();

  for ( int i=0; i < clipPlanesCount(); i++ ) {
    OCCViewer_ClipPlane& aClipPlane = getClipPlane(i);
    if ( aClipPlane.IsOn ) {
      Handle(AIS_Plane) myPreviewPlane;
      double aSize;
      gp_Pnt aBasePnt;
      gp_Dir aNormal;
      clipPlaneParams(aClipPlane, ic, aSize, aBasePnt, aNormal, myModel->trihedronSize());
      myPreviewPlane = new AIS_Plane( new Geom_Plane( aBasePnt, aNormal ), aBasePnt );
      myPreviewPlane->SetTypeOfSensitivity( Select3D_TOS_INTERIOR );
      myPreviewPlane->SetSize( aSize, aSize );
      ic->SetWidth( myPreviewPlane, 10, false );
      ic->SetMaterial( myPreviewPlane, Graphic3d_NOM_PLASTIC, false );
      ic->SetTransparency( myPreviewPlane, 0.5, false );
      Quantity_Color c = (aCurPlaneIndex == i) ? Quantity_Color( 255. / 255., 70. / 255., 0. / 255., Quantity_TOC_RGB ) : Quantity_Color( 85 / 255., 85 / 255., 255 / 255., Quantity_TOC_RGB );
      ic->SetColor( myPreviewPlane, c , false );
      ic->Display( myPreviewPlane, 1, 0, false );
      myPreviewPlaneVector.push_back( myPreviewPlane );
    }
  }
  myModel->update();

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  getMinMaxFromContext( ic, myModel->trihedronSize(), aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  gp_Pnt aRotationCenter( (aXmax + aXmin) * 0.5,
                          (aYmax + aYmin) * 0.5,
                          (aZmax + aZmin) * 0.5 );
  Bnd_Box aMinMax;
  aMinMax.Update( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

  myInteractor->setPlanes( myPreviewPlaneVector );
  myInteractor->setRotationCenter( aRotationCenter );
  myInteractor->setMinMax( aMinMax );
  myInteractor->setEnabled( true );
}

void OCCViewer_ClippingDlg::updatePreview() {
  int aCurPlaneIndex = ComboBoxPlanes->currentIndex();
  int count = clipPlanesCount();
  if ( myBusy || 
       !isValid() || 
       myIsPlaneCreation ||
       !myModel || 
       count == 0 || 
       (aCurPlaneIndex +1 > count) ||
       !PreviewCheckBox->isChecked())
    return;
  
  Handle(AIS_InteractiveContext) ic = myModel->getAISContext();
  
  OCCViewer_ClipPlane& aClipPlane = getClipPlane(aCurPlaneIndex);
  Handle(AIS_Plane) myPreviewPlane;

  if (aClipPlane.IsOn) {
    double aSize;
    gp_Pnt aBasePnt;
    gp_Dir aNormal;
    clipPlaneParams(aClipPlane, ic, aSize, aBasePnt, aNormal, myModel->trihedronSize());
    if(myPreviewPlaneVector.size() < clipPlanesCount()) {
      myPreviewPlaneVector.resize(clipPlanesCount());
    }
    myPreviewPlane = myPreviewPlaneVector[aCurPlaneIndex];
    if(myPreviewPlane.IsNull()) {
      //Plane was not created
      myPreviewPlane = new AIS_Plane( new Geom_Plane( aBasePnt, aNormal ), aBasePnt );
      myPreviewPlane->SetTypeOfSensitivity( Select3D_TOS_INTERIOR );
      myPreviewPlane->SetSize( aSize, aSize );
      ic->Display( myPreviewPlane, 1, 0, false );
      ic->SetWidth( myPreviewPlane, 10, false );
      ic->SetMaterial( myPreviewPlane, Graphic3d_NOM_PLASTIC, false );
      ic->SetTransparency( myPreviewPlane, 0.5, false );
      myPreviewPlaneVector[aCurPlaneIndex] = myPreviewPlane;
    } else {      
      myPreviewPlane->SetComponent( new Geom_Plane( aBasePnt, aNormal ) );
      myPreviewPlane->SetCenter( aBasePnt );
      myPreviewPlane->SetSize( aSize, aSize );	
    }

    ic->SetColor( myPreviewPlane, Quantity_Color( 255. / 255., 70. / 255., 0. / 255., Quantity_TOC_RGB ), false );
    ic->Update( myPreviewPlane, Standard_False );
  } else {
    if(myPreviewPlaneVector.size() > aCurPlaneIndex ) {
      myPreviewPlane = myPreviewPlaneVector[aCurPlaneIndex];
      if(ic->IsDisplayed(myPreviewPlane)) {
	ic->Erase( myPreviewPlane, false );
	ic->Remove( myPreviewPlane, false );
      }
      myPreviewPlaneVector[aCurPlaneIndex].Nullify();
    }
  }
  for(int i = 0; i < myPreviewPlaneVector.size(); i++) {
    if( i == aCurPlaneIndex ) continue;
    if(!myPreviewPlaneVector[i].IsNull())
      ic->SetColor( myPreviewPlaneVector[i], Quantity_Color( 85 / 255., 85 / 255., 255 / 255., Quantity_TOC_RGB ), false );
  }
  myModel->update();

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  getMinMaxFromContext( ic, myModel->trihedronSize(), aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  gp_Pnt aRotationCenter( (aXmax + aXmin) * 0.5,
                          (aYmax + aYmin) * 0.5,
                          (aZmax + aZmin) * 0.5 );
  Bnd_Box aMinMax;
  aMinMax.Update( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

  myInteractor->setPlanes( myPreviewPlaneVector );
  myInteractor->setRotationCenter( aRotationCenter );
  myInteractor->setMinMax( aMinMax );
}

/*!
  Erases preview of clipping plane
*/
void OCCViewer_ClippingDlg::erasePreview()
{
  if ( !myModel )
    return;

  Handle(AIS_InteractiveContext) ic = myModel->getAISContext();

  for ( int i=0; i < myPreviewPlaneVector.size(); i++ ) {
  Handle(AIS_Plane) myPreviewPlane = myPreviewPlaneVector[i];
    if ( !myPreviewPlane.IsNull() && ic->IsDisplayed( myPreviewPlane ) ) {
      ic->Erase( myPreviewPlane, false );
      ic->Remove( myPreviewPlane, false );
      myPreviewPlane.Nullify();
    }
  }
  myPreviewPlaneVector.clear();
  myModel->update();
  myInteractor->setEnabled( false );
}

/*!
  Return true if plane parameters are valid
*/
bool OCCViewer_ClippingDlg::isValid()
{
  return ( SpinBox_Dx->value() !=0 || SpinBox_Dy->value() !=0 || SpinBox_Dz->value() !=0 );
}

/*!
  Update view after changes
*/
void OCCViewer_ClippingDlg::updateClipping()
{
  if (PreviewCheckBox->isChecked() || AutoApplyCheckBox->isChecked())
  {
    if (AutoApplyCheckBox->isChecked()) {
      onApply();
    }
    
    if (!PreviewCheckBox->isChecked())
      myModel->update();
    else 
      updatePreview();
  }
}

/*!
  Updates state of user controls.
*/
void OCCViewer_ClippingDlg::updateControls()
{
  if ( clipPlanesCount() == 0 )
  {
    initParam();
    return;
  }

  int aPlaneIdx = ComboBoxPlanes->currentIndex();

  OCCViewer_ClipPlane& aPlane = getClipPlane( aPlaneIdx );

  if ( aPlane.PlaneMode == Absolute )
  {
    ModeStackedLayout->setCurrentIndex( 0 );
    myCurrentClipPlaneMode = Absolute;
    int anOrientation = aPlane.Orientation;
    // Set plane parameters in the dialog
    SpinBox_X->setValue( aPlane.X );
    SpinBox_Y->setValue( aPlane.Y );
    SpinBox_Z->setValue( aPlane.Z );
    SpinBox_Dx->setValue( aPlane.Dx );
    SpinBox_Dy->setValue( aPlane.Dy );
    SpinBox_Dz->setValue( aPlane.Dz );
    CBAbsoluteOrientation->setCurrentIndex( anOrientation );
    onOrientationAbsoluteChanged( anOrientation );
  }
  else if( aPlane.PlaneMode == Relative )
  {
    ModeStackedLayout->setCurrentIndex( 1 );
    myCurrentClipPlaneMode = Relative;
    int anOrientation = aPlane.RelativeMode.Orientation;

    // Set plane parameters in the dialog
    double aFmtDistance  = double(aPlane.RelativeMode.Distance);
    int aFmtRotation1 = int(aPlane.RelativeMode.Rotation1);
    int aFmtRotation2 = int(aPlane.RelativeMode.Rotation2);

    SpinSliderDistance->setValue( aFmtDistance );
    SpinSliderRotation1->setValue( aFmtRotation1 );
    SpinSliderRotation2->setValue( aFmtRotation2 );
    CBRelativeOrientation->setCurrentIndex( anOrientation );
    onOrientationRelativeChanged( anOrientation );
  }

  isActivePlane->setChecked( aPlane.IsOn );
}

/*!
  SLOT on new button click: create a new clipping plane
*/
void OCCViewer_ClippingDlg::ClickOnNew()
{
  OCCViewer_ClipPlane aPlane;
  aPlane.PlaneMode = (ClipPlaneMode )myCurrentClipPlaneMode;
  myLocalPlanes.push_back(aPlane);
  synchronize();
}

/*!
  SLOT on delete button click: Delete selected clipping plane
*/
void OCCViewer_ClippingDlg::ClickOnDelete()
{
  int aPlaneIndex = ComboBoxPlanes->currentIndex();
  if ( (clipPlanesCount() == 0) || (aPlaneIndex+1 > clipPlanesCount()))
    return;

  myLocalPlanes.erase(myLocalPlanes.begin() + aPlaneIndex);

  Handle(AIS_InteractiveContext) ic = myModel->getAISContext();

  if(aPlaneIndex+1 <= myPreviewPlaneVector.size()) {
    Handle(AIS_Plane) myPreviewPlane = myPreviewPlaneVector[aPlaneIndex];
    if ( !myPreviewPlane.IsNull() && ic->IsDisplayed( myPreviewPlane ) ) {
      ic->Erase( myPreviewPlane, false );
      ic->Remove( myPreviewPlane, false );
    }
    myPreviewPlaneVector.erase(myPreviewPlaneVector.begin() + aPlaneIndex);
  }
  synchronize();
  if (AutoApplyCheckBox->isChecked()) {
    onApply();
  }
  myModel->update();
}

/*!
  SLOT on disable all button click: Restore initial state of viewer,
  erase all clipping planes
*/
void OCCViewer_ClippingDlg::ClickOnDisableAll()
{
  AutoApplyCheckBox->setChecked (false);
  int aClipPlanesCount = clipPlanesCount();
  for ( int anIndex = 0; anIndex < aClipPlanesCount; anIndex++)
  {
    OCCViewer_ClipPlane& aPlane = getClipPlane(anIndex);
    aPlane.IsOn = false;
  }
  erasePreview();
  isActivePlane->setChecked(false);
  myModel->setClipPlanes(myLocalPlanes);
  myModel->update();
}

/*!
  SLOT on ok button click: sets cutting plane and closes dialog
*/
void OCCViewer_ClippingDlg::ClickOnOk()
{
  onApply();
  ClickOnClose();
}

/*!
  SLOT on Apply button click: sets cutting plane and update viewer
*/
void OCCViewer_ClippingDlg::ClickOnApply()
{
  onApply();
  myModel->update();
}

/*!
  SLOT on close button click: erases preview and rejects dialog
*/
void OCCViewer_ClippingDlg::ClickOnClose()
{
  erasePreview();
  OCCViewer_ViewWindow* v = qobject_cast<OCCViewer_ViewWindow*>(parent());
  if(v)
    v->onClipping(false);
}

/*!
  SLOT on help button click: opens a help page
*/
void OCCViewer_ClippingDlg::ClickOnHelp()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( app )
    app->onHelpContextModule( "GUI", "occ_3d_viewer_page.html", "clipping_planes" );
}

/*!
  Set absolute mode of clipping plane
*/
void OCCViewer_ClippingDlg::onModeAbsolute()
{
  myIsPlaneCreation = true;
  ModeStackedLayout->setCurrentIndex(0);
  myCurrentClipPlaneMode = Absolute;
  ClickOnNew();
  myIsPlaneCreation = false;
  updateClipping();
}

/*!
  Set relative mode of clipping plane
*/
void OCCViewer_ClippingDlg::onModeRelative()
{
  myIsPlaneCreation = true;
  ModeStackedLayout->setCurrentIndex(1);
  myCurrentClipPlaneMode = Relative;
  ClickOnNew();
  myIsPlaneCreation = false;
  SetCurrentPlaneParam();
  updateClipping();
}

/*!
  SLOT: called on value of clipping plane changed
*/
void OCCViewer_ClippingDlg::onValueChanged()
{
  if ( myIsUpdatingControls )
  {
    return;
  }

  SetCurrentPlaneParam();

  if ( myIsSelectPlane )
  {
    return;
  }

  updateClipping();
}

/*!
  Set current parameters of selected plane
*/
void OCCViewer_ClippingDlg::onSelectPlane ( int theIndex )
{
  if ( clipPlanesCount() == 0 )
  {
    return;
  }

  OCCViewer_ClipPlane& aClipPlane = getClipPlane( theIndex );

  myIsSelectPlane = true;
  updateControls();
  ComboBoxPlanes->setCurrentIndex( theIndex );
  myIsSelectPlane = false;
}

/*!
  Restore parameters of selected plane
*/
void OCCViewer_ClippingDlg::SetCurrentPlaneParam()
{
  if ( clipPlanesCount() == 0 || myIsSelectPlane || myBusy)
    return;

  int aCurPlaneIndex = ComboBoxPlanes->currentIndex();

  OCCViewer_ClipPlane& aPlane = getClipPlane( aCurPlaneIndex );

  if ( aPlane.PlaneMode == Absolute )
  {
    if( qFuzzyIsNull( SpinBox_Dx->value() ) && 
        qFuzzyIsNull( SpinBox_Dy->value() ) && 
        qFuzzyIsNull( SpinBox_Dz->value() ) ) {
      return;
    }
    aPlane.Orientation = CBAbsoluteOrientation->currentIndex();
    aPlane.X  = SpinBox_X->value();
    aPlane.Y  = SpinBox_Y->value();
    aPlane.Z  = SpinBox_Z->value();
    aPlane.Dx = SpinBox_Dx->value();
    aPlane.Dy = SpinBox_Dy->value();
    aPlane.Dz = SpinBox_Dz->value();
    absolutePlaneToRelative( aPlane, myModel->getAISContext(),myModel->trihedronSize() );
  }
  else if( aPlane.PlaneMode == Relative )
  {
    aPlane.RelativeMode.Orientation = CBRelativeOrientation->currentIndex();
    aPlane.RelativeMode.Distance = SpinSliderDistance->value();
    aPlane.RelativeMode.Rotation1 = SpinSliderRotation1->value();
    aPlane.RelativeMode.Rotation2 = SpinSliderRotation2->value();
    relativePlaneToAbsolute( aPlane, myModel->getAISContext(),myModel->trihedronSize() );
  }
  aPlane.IsOn  = isActivePlane->isChecked();
}

/*!
  SLOT on reset button click: sets default values
*/
void OCCViewer_ClippingDlg::onReset()
{
  myBusy = true;
  SpinBox_X->setValue(0);
  SpinBox_Y->setValue(0);
  SpinBox_Z->setValue(0);
  myBusy = false;

  updateClipping();
}

/*!
  SLOT on invert button click: inverts normal of cutting plane
*/
void OCCViewer_ClippingDlg::onInvert()
{
  double Dx = SpinBox_Dx->value();
  double Dy = SpinBox_Dy->value();
  double Dz = SpinBox_Dz->value();

  myBusy = true;
  SpinBox_Dx->setValue( -Dx );
  SpinBox_Dy->setValue( -Dy );
  SpinBox_Dz->setValue( -Dz );
  myBusy = false;

  if ( clipPlanesCount() != 0 ) {
    int aCurPlaneIndex = ComboBoxPlanes->currentIndex();
    OCCViewer_ClipPlane& aPlane = getClipPlane( aCurPlaneIndex );
    aPlane.IsInvert = !aPlane.IsInvert;
  }
  updateClipping();
}

/*!
  SLOT: called on orientation of clipping plane in absolute mode changed
*/
void OCCViewer_ClippingDlg::onOrientationAbsoluteChanged( int mode )
{
  bool isUserMode = (mode==0);

  TextLabelX->setEnabled( isUserMode );
  TextLabelY->setEnabled( isUserMode );
  TextLabelZ->setEnabled( isUserMode );

  SpinBox_X->setEnabled( isUserMode );
  SpinBox_Y->setEnabled( isUserMode );
  SpinBox_Z->setEnabled( isUserMode );

  TextLabelDx->setEnabled( isUserMode );
  TextLabelDy->setEnabled( isUserMode );
  TextLabelDz->setEnabled( isUserMode );

  SpinBox_Dx->setEnabled( isUserMode );
  SpinBox_Dy->setEnabled( isUserMode );
  SpinBox_Dz->setEnabled( isUserMode );

  if ( !isUserMode ) {

    double aDx = 0, aDy = 0, aDz = 0;

    if ( mode == 1 )
      {
	aDz = 1;
	TextLabelZ->setEnabled( true );
	SpinBox_Z->setEnabled( true );
	SpinBox_Z->setFocus();
      }
    else if ( mode == 2 )
      {
	aDx = 1;
	TextLabelX->setEnabled( true );
	SpinBox_X->setEnabled( true );
	SpinBox_X->setFocus();
      }
    else if ( mode == 3 )
      {
	aDy = 1;
	TextLabelY->setEnabled( true );
	SpinBox_Y->setEnabled( true );
	SpinBox_Y->setFocus();
      }
    
    int aCurPlaneIndex = ComboBoxPlanes->currentIndex();
    OCCViewer_ClipPlane& aPlane = getClipPlane( aCurPlaneIndex );
    if ( aPlane.IsInvert == true ) {
      aDx = -aDx; aDy = -aDy; aDz = -aDz;
    }
    
    myBusy = true;
    SpinBox_Dx->setValue( aDx );
    SpinBox_Dy->setValue( aDy );
    SpinBox_Dz->setValue( aDz );
    myBusy = false;
  }

  if ( !myIsUpdatingControls )
  {
    SetCurrentPlaneParam();
    updateClipping();
  }
}

/*!
  SLOT: called on orientation of clipping plane in relative mode changed
*/
void OCCViewer_ClippingDlg::onOrientationRelativeChanged (int theItem)
{
  if ( clipPlanesCount() == 0 )
    return;
  
  if ( theItem == 0 ) {
    TextLabelRotation1->setText( tr( "ROTATION_AROUND_X_Y2Z" ) );
    TextLabelRotation2->setText( tr( "ROTATION_AROUND_Y_X2Z" ) );
  }
  else if ( theItem == 1 ) {
    TextLabelRotation1->setText( tr( "ROTATION_AROUND_Y_Z2X" ) );
    TextLabelRotation2->setText( tr( "ROTATION_AROUND_Z_Y2X" ) );
  }
  else if ( theItem == 2 ) {
    TextLabelRotation1->setText( tr( "ROTATION_AROUND_Z_X2Y" ) );
    TextLabelRotation2->setText( tr( "ROTATION_AROUND_X_Z2Y" ) );
  }

  if ( !myIsUpdatingControls )
  {
    if( (QComboBox*)sender() == CBRelativeOrientation )
    {
      SetCurrentPlaneParam();
    }

    updateClipping();
  }
}

/*!
  SLOT: called on preview check box toggled
*/
void OCCViewer_ClippingDlg::onPreview( bool on )
{
  erasePreview();
  if ( on ) 
    displayPreview();
}

/*!
  SLOT: called on Auto Apply check box toggled
*/
void OCCViewer_ClippingDlg::onAutoApply( bool toggled )
{
  if ( toggled ) {
    onApply();
    myModel->update();
  }  
}

/*!
  SLOT on Apply button click: sets cutting plane
*/
void OCCViewer_ClippingDlg::onApply()
{
  if ( myBusy )
    return;
  myIsSelectPlane = true;

  qApp->processEvents();
  QApplication::setOverrideCursor( Qt::WaitCursor );
  qApp->processEvents();

  myModel->setClipPlanes(myLocalPlanes);

  QApplication::restoreOverrideCursor();
  myIsSelectPlane = false;
}

/*!
  SLOT: Called when clip plane is clicked in viewer.
*/
void OCCViewer_ClippingDlg::onPlaneClicked( const Handle(AIS_Plane)& thePlane )
{
  for ( int aPlaneIt = 0; aPlaneIt < myPreviewPlaneVector.size(); aPlaneIt++ )
  {
    Handle(AIS_Plane)& aPlane = myPreviewPlaneVector.at( aPlaneIt );
    if ( aPlane != thePlane )
    {
      continue;
    }

    ComboBoxPlanes->setCurrentIndex( aPlaneIt );

    break;
  }
}

/*!
  SLOT: Called when clip plane is changed by dragging in viewer.
*/
void OCCViewer_ClippingDlg::onPlaneDragged( const Handle(AIS_Plane)& thePlane )
{
  for ( int aPlaneIt = 0; aPlaneIt < myPreviewPlaneVector.size(); aPlaneIt++ )
  {
    Handle(AIS_Plane)& aPlane = myPreviewPlaneVector.at( aPlaneIt );
    if ( aPlane != thePlane )
    {
      continue;
    }

    OCCViewer_ClipPlane& aClipPlane = getClipPlane( aPlaneIt );

    gp_Pln aPln = thePlane->Component()->Pln();
    const gp_Pnt& aPlaneP = aPln.Location();
    const gp_Dir& aPlaneN = aPln.Axis().Direction();

    aClipPlane.X  = aPlaneP.X();
    aClipPlane.Y  = aPlaneP.Y();
    aClipPlane.Z  = aPlaneP.Z();
    aClipPlane.Dx = aPlaneN.X();
    aClipPlane.Dy = aPlaneN.Y();
    aClipPlane.Dz = aPlaneN.Z();
    absolutePlaneToRelative( aClipPlane, myModel->getAISContext(), myModel->trihedronSize() );

    myIsUpdatingControls = true;
    updateControls();
    myIsUpdatingControls = false;

    if ( AutoApplyCheckBox->isChecked() )
    {
      onApply();
    }

    break;
  }
}

OCCViewer_ClipPlane& OCCViewer_ClippingDlg::getClipPlane( int theIdx )
{
  return myLocalPlanes[theIdx];
}

int OCCViewer_ClippingDlg::clipPlanesCount()
{
  return myLocalPlanes.size();
}
