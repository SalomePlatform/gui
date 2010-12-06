//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "OCCViewer_ViewPort3d.h"

#include "OCCViewer_VService.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewModel.h"

#include <SUIT_ViewManager.h>

#include <QColor>
#include <QRect>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QApplication>

#include <Visual3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_PerspectiveView.hxx>
#include <V3d_OrthographicView.hxx>

#if defined WNT
#include <WNT_Window.hxx>
#else
#include <Xw_Window.hxx>
#endif

static double rx = 0.;
static double ry = 0.;
static int sx = 0;
static int sy = 0;
static Standard_Boolean zRotation = Standard_False;

#include <Standard_Version.hxx>

#ifdef OCC_VERSION_SERVICEPACK
#define OCC_VERSION_LARGE (OCC_VERSION_MAJOR << 24 | OCC_VERSION_MINOR << 16 | OCC_VERSION_MAINTENANCE << 8 | OCC_VERSION_SERVICEPACK)
#else
#define OCC_VERSION_LARGE (OCC_VERSION_MAJOR << 24 | OCC_VERSION_MINOR << 16 | OCC_VERSION_MAINTENANCE << 8)
#endif

/*!
  Constructor
*/
OCCViewer_ViewPort3d::OCCViewer_ViewPort3d( QWidget* parent, const Handle( V3d_Viewer)& viewer, V3d_TypeOfView  type )
  : OCCViewer_ViewPort( parent ),
    myScale( 1.0 ),
    myDegenerated( true ),
    myAnimate( false ),
    myBusy( true ),
    myIsAdvancedZoomingEnabled( false )
{
  // VSR: 01/07/2010 commented to avoid SIGSEGV at SALOME exit
  //selectVisualId();

  if ( type == V3d_ORTHOGRAPHIC ) {
    myOrthoView = new V3d_OrthographicView( viewer );
    myActiveView = myOrthoView;
    myPerspView = 0;
  } else {
    myPerspView = new V3d_PerspectiveView( viewer );
    myActiveView = myPerspView;
  }
  if ( myDegenerated )
    activeView()->SetDegenerateModeOn();
}

/*!
  Destructor
*/
OCCViewer_ViewPort3d::~OCCViewer_ViewPort3d()
{
  Handle(V3d_View) aView = activeView();
  if (!aView.IsNull())
    aView->Remove();
}

/*!
  Activates the desired 'type' of view in the viewer
  ( view of 'type' is created if it doesn't exist ). [ public ]
*/
/*void OCCViewer_ViewPort3d::setActive( V3d_TypeOfView type )
{
  if ( activeView().IsNull() )
  return;

  if ( activeView()->Type() != type )
  {
  if ( type == V3d_ORTHOGRAPHIC )
  setView( myOrthoView );
  if ( type == V3d_PERSPECTIVE )
  setView( myPerspView );
  }
}*/

/*!
  Maps CasCade 'view' to this viewport. [ private ]
*/
bool OCCViewer_ViewPort3d::mapView( const Handle(V3d_View)& view )
{
  if ( !setWindow( view ) )
    return false;

  if ( !mapped( view ) ) {
    view->SetWindow( myWindow );
    if ( view != activeView() )
      view->View()->Deactivate();
  }

  /* create static trihedron (16551: EDF PAL 501) */
  OCCViewer_ViewWindow* aVW = dynamic_cast<OCCViewer_ViewWindow*>( parentWidget()->parentWidget()->parentWidget() );
  if ( aVW ) {
    OCCViewer_Viewer* aViewModel = dynamic_cast<OCCViewer_Viewer*>( aVW->getViewManager()->getViewModel() );
    if ( aViewModel && aViewModel->isStaticTrihedronDisplayed() ){
      view->ZBufferTriedronSetup();
      view->TriedronDisplay( Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.05, V3d_ZBUFFER );
    }
  }
  return true;
}

/*!
  Sets new CASCADE view on viewport. Returns the previous active view. [ public ]
*/
Handle( V3d_View ) OCCViewer_ViewPort3d::setView( const Handle( V3d_View )& view )
{
  /* map the new view */
  if ( view == activeView() || !mapView( view ) )
    return activeView();

  /* activate the new view*/
  Handle( V3d_View ) oldView = activeView();
  if ( !oldView.IsNull() ) {
    if (oldView->View()->IsDefined())
      oldView->View()->Deactivate();
    view->SetBackgroundColor( oldView->BackgroundColor() );
  }
  
  if ( myDegenerated )
    view->SetDegenerateModeOn();
  else
    view->SetDegenerateModeOff();

  view->View()->Activate();
  activeView() = view;
  return oldView;
}

/*!
  Returns CasCade 3D view. [ public ]
*/
Handle(V3d_View) OCCViewer_ViewPort3d::getView() const
{
  return activeView();
}

/*!
  Returns CasCade 3D viewer [ public ]
*/
Handle(V3d_Viewer) OCCViewer_ViewPort3d::getViewer() const
{
  Handle(V3d_Viewer) viewer;
  if ( !activeView().IsNull() )
    viewer = activeView()->Viewer();
  return viewer;
}

/*!
  Syncronizes visual state of this viewport with 'ref'
  ( scale, projection, eye etc ) Returns 'true' if copied OK,
  'false' otherwise. [ virtual public ]
*/
bool OCCViewer_ViewPort3d::syncronize( const OCCViewer_ViewPort3d* ref )
{
  OCCViewer_ViewPort3d* ref3d = (OCCViewer_ViewPort3d*)ref;
  Handle(V3d_View) refView = ref3d->getView();
  Handle(V3d_View) tgtView = getView();

  /* Syncronize view types */
  /*    if ( tgtView->Type() != refView->Type() )
        {
        setActive( refView->Type() );
        tgtView = getView();
        }*/

  /*  The following params are copied:
      - view type( ortho/persp )
      - position of view point
      - orientation of high point
      - position of the eye
      - projection vector
      - view center ( 2D )
      - view twist
      - view scale
  */

  /* we'll update after setting all params */
  tgtView->SetImmediateUpdate( Standard_False );

  /* perspective */
  if ( refView->Type() == V3d_PERSPECTIVE )
    tgtView->SetFocale( refView->Focale() );

  /* copy params */
  Standard_Real x, y, z;
  refView->At( x, y, z ); tgtView->SetAt( x, y, z );
  refView->Up( x, y, z ); tgtView->SetUp( x, y, z );
  refView->Eye( x, y, z ); tgtView->SetEye( x, y, z );
  refView->Proj( x, y, z ); tgtView->SetProj( x, y, z );
  refView->Center( x, y ); tgtView->SetCenter( x, y );
  tgtView->SetScale( refView->Scale() );
  tgtView->SetTwist( refView->Twist() );

  /* update */
  tgtView->Update();
  tgtView->SetImmediateUpdate( Standard_True );
  return true;
}

/*!
  Returns Z-size of this view. [ public ]
*/
double OCCViewer_ViewPort3d::getZSize() const
{
  if ( !activeView().IsNull() )
    return activeView()->ZSize();
  return 0;
}

/*!
  Sets Z-size of this view ( for both orthographic and perspective ). [ public ]
*/
void OCCViewer_ViewPort3d::setZSize( double zsize )
{
  myActiveView->SetZSize( zsize );
  /*    if ( !myOrthoView.IsNull() )
        myOrthoView->SetZSize( zsize );
        if ( !myPerspView.IsNull() )
        myPerspView->SetZSize( zsize );*/
}

/*!
  Returns the background color [ virtual public ]
*/
QColor OCCViewer_ViewPort3d::backgroundColor() const
{
  if ( !activeView().IsNull() ) {
    Standard_Real aRed, aGreen, aBlue;
    activeView()->BackgroundColor( Quantity_TOC_RGB, aRed, aGreen, aBlue );
    int red = (int) (aRed * 255);
    int green = (int) (aGreen * 255);
    int blue = (int) (aBlue * 255);
    return QColor( red, green, blue );
  }
  return OCCViewer_ViewPort::backgroundColor();
}

/*!
  Sets the background color [ virtual public ]
*/
void OCCViewer_ViewPort3d::setBackgroundColor( const QColor& color )
{
  if ( !activeView().IsNull() ) {
    activeView()->SetBackgroundColor( Quantity_TOC_RGB, color.red()/255.,
                                      color.green()/255., color.blue()/255.);
    activeView()->Update();
    emit vpChangeBGColor( color );
  }
}

/*!
  Set animation mode
  \param theDegenerated - degenerated mode
*/
void OCCViewer_ViewPort3d::setAnimationMode(bool theDegenerated)
{
  if ( !activeView().IsNull() ) {
    myAnimate = theDegenerated;
    activeView()->SetAnimationMode(true, theDegenerated);
  }
}

/*!
  Updates the active viewport. [ virtual public ]
*/
void OCCViewer_ViewPort3d::onUpdate()
{
  if ( !activeView().IsNull() )
    activeView()->Update();
}

/*!
  Called at 'window fit' transformation. [ virtual protected ]
*/
void OCCViewer_ViewPort3d::fitRect( const QRect& rect )
{
  if ( !activeView().IsNull() )
    activeView()->WindowFit( rect.left(), rect.top(), rect.right(), rect.bottom() );
}

/*!
  Inits 'zoom' transformation. [ protected ]
*/
void OCCViewer_ViewPort3d::startZoomAtPoint( int x, int y )
{
#if OCC_VERSION_LARGE > 0x0603000A // available only with OCC-6.3-sp11 and higher version
  if ( !activeView().IsNull() && isAdvancedZoomingEnabled() )
    activeView()->StartZoomAtPoint( x, y );
#endif
}

/*!
  Called at 'zoom' transformation. [ virtual protected ]
*/
void OCCViewer_ViewPort3d::zoom( int x0, int y0, int x, int y )
{
  if ( !activeView().IsNull() ) {
    // as OCCT respects a sign of only dx,
    // but we want both signes to be taken into account
    //activeView()->Zoom( x0, y0, x, y );
#if OCC_VERSION_LARGE > 0x0603000A // available only with OCC-6.3-sp11 and higher version
    if ( isAdvancedZoomingEnabled() )
      activeView()->ZoomAtPoint( x0, y0, x, y );
    else
#endif
      activeView()->Zoom( x0 + y0, 0, x + y, 0 );
  }
}

/*!
  Centers the viewport. [ virtual protected ]
*/
void OCCViewer_ViewPort3d::setCenter( int x, int y )
{
  if ( !activeView().IsNull() )
    activeView()->Place( x, y, myScale );
}

/*!
  Called at 'pan' transformation. [ virtual protected ]
*/
void OCCViewer_ViewPort3d::pan( int dx, int dy )
{
  if ( !activeView().IsNull() )
    activeView()->Pan( dx, dy, 1.0 );
}

/*!
  Inits 'rotation' transformation. [ protected ]
*/
void OCCViewer_ViewPort3d::startRotation( int x, int y, 
                                          int theRotationPointType,
                                          const gp_Pnt& theSelectedPoint )
{
  if ( !activeView().IsNull() ) {
    myDegenerated = activeView()->DegenerateModeIsOn();
    activeView()->SetDegenerateModeOn();
    if (myAnimate) activeView()->SetAnimationModeOn();
    
    //double gx, gy, gz;
    //double gx = activeView()->gx;
    //activeView()->Gravity(gx,gy,gz);
    
    switch ( theRotationPointType ) {
    case OCCViewer_ViewWindow::GRAVITY:
      activeView()->StartRotation( x, y, 0.45 );
      break;
    case OCCViewer_ViewWindow::SELECTED:
      sx = x; sy = y;
      
      double X,Y;
      activeView()->Size(X,Y);
      rx = Standard_Real(activeView()->Convert(X)); 
      ry = Standard_Real(activeView()->Convert(Y)); 
      
      activeView()->Rotate( 0., 0., 0., 
                            theSelectedPoint.X(),theSelectedPoint.Y(), theSelectedPoint.Z(), 
                            Standard_True );
      
      Quantity_Ratio zRotationThreshold;
      zRotation = Standard_False;
      zRotationThreshold = 0.45;
      if( zRotationThreshold > 0. ) {
        Standard_Real dx = Abs(sx - rx/2.);
        Standard_Real dy = Abs(sy - ry/2.);
        Standard_Real dd = zRotationThreshold * (rx + ry)/2.;
        if( dx > dd || dy > dd ) zRotation = Standard_True;
      }
      break;
    default:
      break;
    }
    activeView()->DepthFitAll();
  }
}

/*!
  Rotates the viewport. [ protected ]
*/
void OCCViewer_ViewPort3d::rotate( int x, int y, 
                                   int theRotationPointType,
                                   const gp_Pnt& theSelectedPoint )
{
  if ( !activeView().IsNull() ) {
    switch ( theRotationPointType ) {
    case OCCViewer_ViewWindow::GRAVITY:
      activeView()->Rotation( x, y );
      break;
    case OCCViewer_ViewWindow::SELECTED:
      double dx, dy, dz;
      if( zRotation ) {
        dz = atan2(Standard_Real(x)-rx/2., ry/2.-Standard_Real(y)) - 
          atan2(sx-rx/2.,ry/2.-sy);
        dx = dy = 0.;
      }
      else {
        dx = (Standard_Real(x) - sx) * Standard_PI/rx;
        dy = (sy - Standard_Real(y)) * Standard_PI/ry;
        dz = 0.;
      }
      
      activeView()->Rotate( dx, dy, dz, 
                            theSelectedPoint.X(),theSelectedPoint.Y(), theSelectedPoint.Z(),
                            Standard_False );
      break;
    default:
      break;
    }
  }
  //  setZSize( getZSize() );
}

/*!
  Resets the viewport after 'rotation'. [ protected ]
*/
void OCCViewer_ViewPort3d::endRotation()
{
  if ( !activeView().IsNull() ) {
    if (myAnimate) activeView()->SetAnimationModeOff();
    if ( !myDegenerated )
      activeView()->SetDegenerateModeOff();
    activeView()->ZFitAll(1.);
    activeView()->SetZSize(0.);
    activeView()->Update();
  }
}

/*!
  Repaints the viewport. [ virtual protected ]
*/
void OCCViewer_ViewPort3d::paintEvent( QPaintEvent* e )
{
#ifndef WNT
  /* X11 : map before show doesn't work */
  if ( !mapped( activeView() ) )
    mapView( activeView() );
#endif
  if ( !myWindow.IsNull() ) {
    QApplication::syncX();
    QRect rc = e->rect();
    if ( !myPaintersRedrawing )
      activeView()->Redraw( rc.x(), rc.y(), rc.width(), rc.height() );
  }
  OCCViewer_ViewPort::paintEvent( e );
  myBusy = false;
}

/*!
  Resizes the viewport. [ virtual protected ]
*/
void OCCViewer_ViewPort3d::resizeEvent( QResizeEvent* e )
{
#ifdef WNT
  /* Win32 : map before first show to avoid flicker */
  if ( !mapped( activeView() ) )
    mapView( activeView() );
#endif
  QApplication::syncX();
  if ( !activeView().IsNull() )
    activeView()->MustBeResized();
}

/*!
  Fits all objects in view. [ virtual protected ]
*/
void OCCViewer_ViewPort3d::fitAll( bool keepScale, bool withZ, bool upd )
{
  if ( activeView().IsNull() )
    return;

  if ( keepScale )
    myScale = activeView()->Scale();

  Standard_Real margin = 0.01;
  activeView()->FitAll( margin, withZ, upd );
  activeView()->SetZSize(0.);
}

/*!
  Resets the view. [ virtual protected ]
*/
void OCCViewer_ViewPort3d::reset()
{
  //  double zsize = getZSize();
  if ( !activeView().IsNull() )
    activeView()->Reset();
  //    setZSize( zsize );
}

/*!
  Passed the handle of native window of the component to CASCADE view. [ private ]
*/
bool OCCViewer_ViewPort3d::setWindow( const Handle(V3d_View)& view )
{
  if ( !myWindow.IsNull() )
    return true;

  if ( view.IsNull() )
    return false;

  int hwnd = (int)winId();
  if ( !hwnd )
    return false;

  /* set this widget as the drawing window */
  short lo = (short)hwnd;
  short hi = (short)( hwnd >> 16 );

  attachWindow( view, OCCViewer_VService::CreateWindow( view, (int)hi, (int)lo, Xw_WQ_SAMEQUALITY ) );

  myWindow = view->Window();
  return !myWindow.IsNull();
}

void OCCViewer_ViewPort3d::attachWindow( const Handle(V3d_View)& view, 
                                         const Handle(Aspect_Window)& window)
{
  if (!view.IsNull())
    view->SetWindow( window );
}

/*!
  Returns the current active view. [ private ]
*/
Handle(V3d_View) OCCViewer_ViewPort3d::activeView() const
{
  return myActiveView;
}

/*!
  Returns the current inactive view [ private ]
*/
/*Handle(V3d_View) OCCViewer_ViewPort3d::inactiveView() const
  {
  return ( activeView() == myOrthoView ? myPerspView : myOrthoView );
  }*/

/*!
  Returns 'true' if the given view is mapped to window. [ private ]
*/
bool OCCViewer_ViewPort3d::mapped( const Handle(V3d_View)& view ) const
{
  return ( !view.IsNull() && view->View()->IsDefined() );
}
