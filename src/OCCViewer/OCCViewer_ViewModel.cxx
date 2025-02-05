// Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
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

#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewFrame.h"
#include "OCCViewer_VService.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_ClippingDlg.h"
#include "OCCViewer_Utilities.h"
#include "OCCViewer_TrihedronSetup.h"

#include "SUIT_ViewWindow.h"
#include "SUIT_ViewManager.h"
#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SUIT_ResourceMgr.h"

#include "ViewerData_AISShape.hxx"

#include "QtxActionToolMgr.h"
#include "QtxBackgroundTool.h"

#include <QPainter>
#include <QApplication>
#include <QColorDialog>
#include <QFileDialog>
#include <QPalette>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QToolBar>
#include <QDesktopWidget>

#include <AIS_Axis.hxx>
#include <Prs3d_Drawer.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <StdSelect_ViewerSelector3d.hxx>

#include <Graphic3d_Texture2Dmanual.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_TextureParams.hxx>

#include <Geom_Axis2Placement.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_TextAspect.hxx>

#include <V3d_DirectionalLight.hxx>
#include <V3d_AmbientLight.hxx>

#include <Basics_OCCTVersion.hxx>

namespace
{
  void setCappingColor(const Handle(Graphic3d_ClipPlane)& plane, const QColor& color)
  {
    Quantity_Color qcolor( color.redF(), color.greenF(), color.blueF(), Quantity_TOC_RGB );
#if OCC_VERSION_LARGE < 0x07040000
    Graphic3d_MaterialAspect aspect;
    aspect.SetColor( qcolor );
    plane->SetCappingMaterial( aspect );
#else
    plane->SetCappingColor( qcolor );
#endif
  }
}

/*!
  Get data for supported background modes: gradient types, identifiers and supported image formats
*/
QString OCCViewer_Viewer::backgroundData( QStringList& gradList, QIntList& idList, QIntList& txtList )
{
  gradList << tr("GT_HORIZONTALGRADIENT")    << tr("GT_VERTICALGRADIENT")       <<
              tr("GT_FIRSTDIAGONALGRADIENT") << tr("GT_SECONDDIAGONALGRADIENT") <<
              tr("GT_FIRSTCORNERGRADIENT")   << tr("GT_SECONDCORNERGRADIENT")   <<
              tr("GT_THIRDCORNERGRADIENT")   << tr("GT_FORTHCORNERGRADIENT");
  idList   << HorizontalGradient             << VerticalGradient  <<
              Diagonal1Gradient              << Diagonal2Gradient <<
              Corner1Gradient                << Corner2Gradient   <<
              Corner3Gradient                << Corner4Gradient;
  txtList  << Qtx::CenterTexture << Qtx::TileTexture << Qtx::StretchTexture;
  return tr("BG_IMAGE_FILES");
}

/*!
  Get data for supported stereo pair modes: stereo types and identifiers
*/
void OCCViewer_Viewer::stereoData( QStringList& typeList, QIntList& idList)
{
  typeList << tr("ST_QUADBUFFER")    << tr("ST_ANAGLYPH")         <<
              tr("ST_ROWINTERLACED") << tr("ST_COLUMNINTERLACED") <<
              tr("ST_CHESSBOARD")    << tr("ST_SIDEBYSIDE")       <<
              tr("ST_OVERUNDER");
  idList   << QuadBufferType    << AnaglyphType         <<
              RowInterlacedType << ColumnInterlacedType <<
              ChessBoardType    << SideBySideType       <<
              OverUnderType;
}

/*!
  Constructor
  \param DisplayTrihedron - is trihedron displayed
*/
OCCViewer_Viewer::OCCViewer_Viewer( bool DisplayTrihedron)
: SUIT_ViewModel(),
  myIsRelative(true),
  myTopLayerId(0),
  myTrihedronSize(100),
  myBackgrounds(4, Qtx::BackgroundData(Qt::black)),
  myClippingDlg(0),
  myFitter(0)
{
  // init CasCade viewers
  myV3dViewer = OCCViewer_VService::CreateViewer();
  //myV3dViewer->Init(); // to avoid creation of the useless perspective view (see OCCT issue 0024267)
  setDefaultLights();

  // init selector
  myAISContext = new AIS_InteractiveContext( myV3dViewer );
  myAISContext->HighlightStyle(Prs3d_TypeOfHighlight_LocalSelected)->SetColor( Quantity_NOC_WHITE );
  myAISContext->HighlightStyle(Prs3d_TypeOfHighlight_Selected)->SetColor( Quantity_NOC_WHITE );

  // display isoline on planar faces (box for ex.)
  myAISContext->IsoOnPlane( true );

  // create color scale
  myColorScale = new AIS_ColorScale();
  myColorScale->SetZLayer( Graphic3d_ZLayerId_TopOSD );
#if OCC_VERSION_LARGE < 0x07070000
  myColorScale->SetTransformPersistence( Graphic3d_TransformPers::FromDeprecatedParams( Graphic3d_TMF_2d, gp_Pnt(-1, -1, 0) ) );
#else
  myColorScale->SetTransformPersistence( new Graphic3d_TransformPers (Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER) );
#endif

  if (DisplayTrihedron)
  {
    initTrihedron();
  }

  /* create view cube */
  myViewCube  = new AIS_ViewCube();
  setViewCubeParamsFromPreferences();

  // set interaction style to standard
  myInteractionStyle = 0;

  // set zooming style to standard
  myZoomingStyle = 0;

  // preselection
  myPreselectionEnabled = true;

  // selection
  mySelectionEnabled = true;
  myMultiSelectionEnabled = true;

  // set projection type to orthographic
  myProjectionType = 0;
  mySelectionStyle = OCCViewer_ViewWindow::RectStyle;
  // set stereo parameters
  myStereoType = 0;
  myAnaglyphFilter = 0;
  myToReverseStereo = 0;
  myVSyncMode = 1;
  myQuadBufferSupport = 0;
  myStereographicFocusType = 1;
  myInterocularDistanceType = 1;
  myStereographicFocusValue = 1.0;
  myInterocularDistanceValue = 0.05;
  //set clipping color and texture to standard
  myClippingColor = QColor( 50, 50, 50 );
  myDefaultTextureUsed = true;
  myClippingTexture = QString();
  myTextureModulated = true;
  myClippingTextureScale = 1.0;
  // By default, use the object color in the clipping plane
  myClippingUseObjColor = true;

}

/*!
  Destructor
*/
OCCViewer_Viewer::~OCCViewer_Viewer() 
{
  myAISContext.Nullify();
  myV3dViewer.Nullify();
}

/*!
  [obsolete]
  \return background color of viewer
*/
QColor OCCViewer_Viewer::backgroundColor() const
{
  return backgroundColor(0);
}

/*!
  \return background data of viewer
*/
Qtx::BackgroundData OCCViewer_Viewer::background() const
{
  return background(0);
}

/*!
  Sets background color [obsolete]
  \param c - new background color
*/
void OCCViewer_Viewer::setBackgroundColor( const QColor& c )
{
  setBackgroundColor( 0, c );
}

/*!
  Sets background data
  \param d - new background data
*/
void OCCViewer_Viewer::setBackground( const Qtx::BackgroundData& theBackground )
{
  setBackground( 0, theBackground );
}

/*!
  Start initialization of view window
  \param view - view window to be initialized
*/
void OCCViewer_Viewer::initView( OCCViewer_ViewWindow* view )
{
  if ( view ) {
    view->initLayout();
    view->initSketchers();
    view->setInteractionStyle( interactionStyle() );
    view->setProjectionType( projectionType() );
    view->setSelectionStyle( selectionStyle() );
    view->setStereoType( stereoType() );
    view->setAnaglyphFilter( anaglyphFilter() );
    view->setStereographicFocus( stereographicFocusType(), stereographicFocusValue() );
    view->setInterocularDistance( interocularDistanceType(), interocularDistanceValue() );
    view->setReverseStereo( isReverseStereo() );
    view->setVSync( isVSync() );
    view->setQuadBufferSupport( isQuadBufferSupport() );
    view->setZoomingStyle( zoomingStyle() );
    view->enablePreselection( isPreselectionEnabled() );
    view->enableSelection( isSelectionEnabled() );

    OCCViewer_ViewPort3d* vp3d = view->getViewPort();
    if ( vp3d )
    {
      // connect signal from viewport
      connect(vp3d, SIGNAL(vpClosed(OCCViewer_ViewPort3d*)), this, SLOT(onViewClosed(OCCViewer_ViewPort3d*)));
      connect(vp3d, SIGNAL(vpMapped(OCCViewer_ViewPort3d*)), this, SLOT(onViewMapped(OCCViewer_ViewPort3d*)));
    }
  }
}

/*!
  Creates new view window
  \param theDesktop - main window of application
*/
SUIT_ViewWindow* OCCViewer_Viewer::createView( SUIT_Desktop* theDesktop )
{
  // create view frame
  OCCViewer_ViewFrame* view = new OCCViewer_ViewFrame(theDesktop, this);
  // get main view window (created by view frame)
  OCCViewer_ViewWindow* vw = view->getView(OCCViewer_ViewFrame::MAIN_VIEW);
  // initialize main view window
  initView( vw );
  // set default background for view window
  vw->setBackground( background(0) ); // 0 means MAIN_VIEW (other views are not yet created here)

  return view;
}

/*!
  Sets new view manager
  \param theViewManager - new view manager
*/
void OCCViewer_Viewer::setViewManager(SUIT_ViewManager* theViewManager)
{
  SUIT_ViewModel::setViewManager(theViewManager);
  if (theViewManager) {
    connect(theViewManager, SIGNAL(mousePress(SUIT_ViewWindow*, QMouseEvent*)), 
            this, SLOT(onMousePress(SUIT_ViewWindow*, QMouseEvent*)));

    connect(theViewManager, SIGNAL(mouseMove(SUIT_ViewWindow*, QMouseEvent*)), 
            this, SLOT(onMouseMove(SUIT_ViewWindow*, QMouseEvent*)));

    connect(theViewManager, SIGNAL(mouseRelease(SUIT_ViewWindow*, QMouseEvent*)), 
            this, SLOT(onMouseRelease(SUIT_ViewWindow*, QMouseEvent*)));

    connect(theViewManager, SIGNAL(keyPress(SUIT_ViewWindow*, QKeyEvent*)), 
            this, SLOT(onKeyPress(SUIT_ViewWindow*, QKeyEvent*)));
  }
}

/*!
  SLOT: called on mouse button press, stores current mouse position as start point for transformations
*/
void OCCViewer_Viewer::onMousePress(SUIT_ViewWindow* /*theWindow*/, QMouseEvent* theEvent)
{
  myStartPnt.setX(theEvent->x()); myStartPnt.setY(theEvent->y());
}

/*!
  SLOT: called on mouse move, processes transformation or hilighting
*/
void OCCViewer_Viewer::onMouseMove(SUIT_ViewWindow* theWindow, QMouseEvent* theEvent)
{
  if (!mySelectionEnabled) return;
  if (!theWindow->inherits("OCCViewer_ViewWindow")) return;

  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*) theWindow;

  myCurPnt.setX(theEvent->x()); myCurPnt.setY(theEvent->y());

  if ( isSelectionEnabled() && isPreselectionEnabled() ) {
    if (aView->getViewPort()->isBusy()) {
      QCoreApplication::processEvents();
      return; // Check that the ViewPort initialization completed
                                                // To Prevent call move event if the View port is not initialized
                                                // IPAL 20883
    }
    Handle(V3d_View) aView3d = aView->getViewPort()->getView();
    if ( !aView3d.IsNull() ) {
      myAISContext->MoveTo( theEvent->x(), theEvent->y(), aView3d, Standard_True );
    }
  }
}


/*!
  SLOT: called on mouse button release, finishes transformation or selection
*/
void OCCViewer_Viewer::onMouseRelease(SUIT_ViewWindow* theWindow, QMouseEvent* theEvent)
{
  if (!mySelectionEnabled) return;
  if (theEvent->button() != Qt::LeftButton) return;
  if (!theWindow->inherits("OCCViewer_ViewWindow")) return;

  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*) theWindow;
  if (!aView )
    return;

  myEndPnt.setX(theEvent->x()); myEndPnt.setY(theEvent->y());
  bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
  
  if (myStartPnt == myEndPnt)
  {
    if (!aHasShift) {
      myAISContext->ClearCurrents( false ); // todo: ClearCurrents is deprecated
      emit deselection();
    }
    if ( !isPreselectionEnabled() ) {
      Handle(V3d_View) aView3d = aView->getViewPort()->getView();
      if ( !aView3d.IsNull() ) {
	      myAISContext->MoveTo( myEndPnt.x(), myEndPnt.y(), aView3d, Standard_True );
      }
    }

    if (aHasShift && myMultiSelectionEnabled)
      myAISContext->ShiftSelect( Standard_True );
    else 
      myAISContext->Select( Standard_True );
    emit selectionChanged();
  }

  //else
  //{
  //  if (aHasShift && myMultiSelectionEnabled)
  //    myAISContext->ShiftSelect(myStartPnt.x(), myStartPnt.y(),
  //                              myEndPnt.x(), myEndPnt.y(),
  //                              aView->getViewPort()->getView(), Standard_False );
  //  else
  //    myAISContext->Select(myStartPnt.x(), myStartPnt.y(),
  //                         myEndPnt.x(), myEndPnt.y(),
  //                         aView->getViewPort()->getView(), Standard_False );

  //  int Nb = myAISContext->NbSelected();
  //  if( Nb>1 && !myMultiSelectionEnabled )
  //  {
  //      myAISContext->InitSelected();
  //      Handle( SelectMgr_EntityOwner ) anOwner = myAISContext->SelectedOwner();
  //      if( !anOwner.IsNull() )
  //      {
  //          myAISContext->ClearSelected( Standard_False );
  //          myAISContext->AddOrRemoveSelected( anOwner, Standard_False );
  //      }
  //  }

  //  myAISContext->UpdateCurrentViewer();
  //}
}

/*!
  SLOT: called on key press, processes selection in "key free" interaction style
*/
void OCCViewer_Viewer::onKeyPress(SUIT_ViewWindow* theWindow, QKeyEvent* theEvent)
{
  if (!mySelectionEnabled) return;

  OCCViewer_ViewWindow* aView = qobject_cast<OCCViewer_ViewWindow*>( theWindow );
  if ( !aView ) return;

  bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);

  switch ( theEvent->key() ) {
  case  Qt::Key_S:
    if (!aHasShift) {
      myAISContext->ClearCurrents( false ); // todo: ClearCurrents is deprecated
      emit deselection();
    }

    if ( !isPreselectionEnabled() ) {
      Handle(V3d_View) aView3d = aView->getViewPort()->getView();
      if ( !aView3d.IsNull() ) {
	myAISContext->MoveTo(myCurPnt.x(), myCurPnt.y(), aView3d, Standard_True );
      }
    }

    if (aHasShift && myMultiSelectionEnabled)
      myAISContext->ShiftSelect( Standard_True );
    else
      myAISContext->Select( Standard_True );

    emit selectionChanged();

    break;
  case  Qt::Key_N:
    if ( isPreselectionEnabled() ) {
      getAISContext()->HilightNextDetected( aView->getViewPort()->getView() );
    }
    break;
  case  Qt::Key_P:
    if ( isPreselectionEnabled() ) {
      getAISContext()->HilightPreviousDetected( aView->getViewPort()->getView() );
    }
    break;
  default:
    break;
  }
}

void OCCViewer_Viewer::onViewClosed(OCCViewer_ViewPort3d*)
{
  Standard_Integer aViewsNb = 0;
  for ( myV3dViewer->InitActiveViews(); myV3dViewer->MoreActiveViews(); myV3dViewer->NextActiveViews())
    ++aViewsNb;
  if ( aViewsNb < 2 ) {
    //clean up presentations before last view is closed
    myAISContext->RemoveAll(Standard_False);
  }
}

void OCCViewer_Viewer::onViewMapped(OCCViewer_ViewPort3d* viewPort)
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  setTrihedronShown( true );

  bool showViewCube = true;
  if ( resMgr ) showViewCube = resMgr->booleanValue( "OCCViewer", "viewcube_show", true );
  setViewCubeShown( showViewCube );

  bool showStaticTrihedron = true;
  if ( resMgr ) showStaticTrihedron = resMgr->booleanValue( "3DViewer", "show_static_trihedron", true );
  viewPort->showStaticTrihedron( showStaticTrihedron );
}

int OCCViewer_Viewer::getTopLayerId()
{
  if ( myTopLayerId == 0 && !myAISContext->CurrentViewer().IsNull() )    
    myAISContext->CurrentViewer()->AddZLayer( myTopLayerId );

  return myTopLayerId;
}

/*!
  \return interaction style
*/
int OCCViewer_Viewer::interactionStyle() const
{
  return myInteractionStyle;
}

/*!
  Sets interaction style: 0 - standard, 1 - keyboard free interaction
  \param theStyle - new interaction style
*/
void OCCViewer_Viewer::setInteractionStyle( const int theStyle )
{
  myInteractionStyle = theStyle;
  //!! To be done for view windows
  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->setInteractionStyle( theStyle );
  }
}

/*!
  \return projection type
*/
int OCCViewer_Viewer::projectionType() const
{
  return myProjectionType;
}

/*!
  Sets projection type: 0 - orthographic, 1 - perspective
  \param theType - new projection type
*/
void OCCViewer_Viewer::setProjectionType( const int theType )
{
  if ( myProjectionType != theType ) {
    if ( theType != OCCViewer_ViewWindow::Stereo )
      myProjectionType = theType;

    if ( !myViewManager )
      return;

    QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
    for ( int i = 0; i < (int)wins.count(); i++ )
    {
      OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
      if ( win )
        win->setProjectionType( (OCCViewer_ViewWindow::ProjectionType)theType );
    }
  }
}


OCCViewer_ViewWindow::SelectionStyle OCCViewer_Viewer::selectionStyle() const
{
  return mySelectionStyle;
}

void OCCViewer_Viewer::setSelectionStyle(OCCViewer_ViewWindow::SelectionStyle theMode)
{
  if (mySelectionStyle != theMode) {
    mySelectionStyle = theMode;
    if (!myViewManager)
      return;

    QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
    for (int i = 0; i < (int)wins.count(); i++)
    {
      OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>(wins.at(i));
      if (win)
        win->setSelectionStyle(theMode);
    }
  }
}



/*!
  \return stereo type
*/
int OCCViewer_Viewer::stereoType() const
{
  return myStereoType;
}

/*!
  Sets stereo type
  \param theType - new stereo type
*/
void OCCViewer_Viewer::setStereoType( const int theType )
{
  myStereoType = theType;

  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->setStereoType( (OCCViewer_ViewWindow::StereoType)theType );
  }
}

/*!
  \return stereographic focus type
*/
int OCCViewer_Viewer::stereographicFocusType() const
{
  return myStereographicFocusType;
}

/*!
  \return stereographic focus value
*/
double OCCViewer_Viewer::stereographicFocusValue() const
{
  return myStereographicFocusValue;
}

/*!
  Sets stereographic focus parameters
  \param theType - new stereographic focus type
  \param theValue - new stereographic focus value
*/
void OCCViewer_Viewer::setStereographicFocus( const int theType, const double theValue )
{
  myStereographicFocusType = theType;
  myStereographicFocusValue = theValue;

  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->setStereographicFocus( (OCCViewer_ViewWindow::FocusIODType)theType, theValue );
  }
}

/*!
  \return stereographic focus type
*/
int OCCViewer_Viewer::interocularDistanceType() const
{
  return myInterocularDistanceType;
}

/*!
  \return stereographic focus value
*/
double OCCViewer_Viewer::interocularDistanceValue() const
{
  return myInterocularDistanceValue;
}

/*!
  Sets interocular distance parameters
  \param theType - new IOD type
  \param theValue - new IOD value
*/
void OCCViewer_Viewer::setInterocularDistance( const int theType, const double theValue )
{
  myInterocularDistanceType = theType;
  myInterocularDistanceValue = theValue;

  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->setInterocularDistance( (OCCViewer_ViewWindow::FocusIODType)theType, theValue );
  }
}

/*!
  \return anaglyph filter
*/
int OCCViewer_Viewer::anaglyphFilter() const
{
  return myAnaglyphFilter;
}

/*!
  Sets anaglyph filter
  \param theType - new anaglyph filter
*/
void OCCViewer_Viewer::setAnaglyphFilter( const int theType )
{
  myAnaglyphFilter = theType;

  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->setAnaglyphFilter( (OCCViewer_ViewWindow::AnaglyphFilter)theType );
  }
}

/*!
  \return reverse stereo
*/
bool OCCViewer_Viewer::isReverseStereo() const
{
  return myToReverseStereo;
}

/*!
  Sets reverse stereo
  \param theReverse - enable/disable reverse mode
*/
void OCCViewer_Viewer::setReverseStereo( const bool theReverse )
{
  myToReverseStereo = theReverse;

  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->setReverseStereo( theReverse );
  }
}

/*!
  \return V-Sync mode
*/
bool OCCViewer_Viewer::isVSync() const
{
  return myVSyncMode;
}

/*!
  Set V-Sync mode
  \param theEnable - enable/disable V-Sync mode
*/
void OCCViewer_Viewer::setVSync( const bool theEnable )
{
  myVSyncMode = theEnable;

  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->setVSync( theEnable );
  }
}

/*!
  \return support quad-buffered stereo
*/
bool OCCViewer_Viewer::isQuadBufferSupport() const
{
  return myQuadBufferSupport;
}

/*!
  Set support quad-buffered stereo
  \param theEnable - enable/disable support quad-buffered stereo
*/
void OCCViewer_Viewer::setQuadBufferSupport( const bool theEnable )
{
  myQuadBufferSupport = theEnable;

  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->setQuadBufferSupport( theEnable );
  }
}

/*!
  \return zooming style
*/
int OCCViewer_Viewer::zoomingStyle() const
{
  return myZoomingStyle;
}

/*!
  Sets zooming style: 0 - standard, 1 - advanced (at cursor)
  \param theStyle - new zooming style
*/
void OCCViewer_Viewer::setZoomingStyle( const int theStyle )
{
  myZoomingStyle = theStyle;
  //!! To be done for view windows
  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->setZoomingStyle( theStyle );
  }
}

/*!
  \return true if preselection is enabled
*/
bool OCCViewer_Viewer::isPreselectionEnabled() const 
{ 
  return myPreselectionEnabled; 
}

/*!
  Enables/disables preselection
  \param isEnabled - new status
*/
void OCCViewer_Viewer::enablePreselection(bool isEnabled)
{
  myPreselectionEnabled = isEnabled;

  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win ) {
      win->enablePreselection( isEnabled );
    }
  }
}

/*!
  \return true if selection is enabled
*/
bool OCCViewer_Viewer::isSelectionEnabled() const 
{ 
  return mySelectionEnabled; 
}

/*!
  Enables/disables selection
  \param isEnabled - new status
*/
void OCCViewer_Viewer::enableSelection(bool isEnabled)
{
  mySelectionEnabled = isEnabled;

  //!! To be done for view windows
  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win ) {
      win->updateEnabledDrawMode();
      win->enableSelection( isEnabled );
    }
  }

  
  //clear current selection in the viewer
  if(!isEnabled) {
    myAISContext->ClearSelected( Standard_True );
  }

}

/*!
  Sets multiselection enabled status
  \param isEnabled - new status
*/
void OCCViewer_Viewer::enableMultiselection(bool isEnable)
{
  myMultiSelectionEnabled = isEnable;
  //!! To be done for view windows
  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->updateEnabledDrawMode();
  }
}

/*!
  Sets whether to use the color of the clipped object for the clipped region
  \param theUseObjColor - whether to use color of clipped object
*/
void OCCViewer_Viewer::setClippingUseObjColor( bool theUseObjColor )
{
  myClippingUseObjColor = theUseObjColor;

  if( myInternalClipPlanes.IsEmpty() )
    return;

  for ( Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt ( myInternalClipPlanes ); aPlaneIt.More(); aPlaneIt.Next() )
    aPlaneIt.Value()->SetUseObjectMaterial(theUseObjColor);

  update();
}

/*!
  \return whether object color is used for clipping region
*/
bool OCCViewer_Viewer::isClippingObjColorUsed() const
{
  return myClippingUseObjColor;
}

/*!
  Sets a color of the clipped region
  \param theColor - a new color of the clipped region
*/
void OCCViewer_Viewer::setClippingColor( const QColor& theColor )
{
  myClippingColor = theColor;

  if( myInternalClipPlanes.IsEmpty() )
    return;

  for ( Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt ( myInternalClipPlanes ); aPlaneIt.More(); aPlaneIt.Next() )
    setCappingColor( aPlaneIt.Value(), theColor );

  update();
}

/*!
  \return clipping color
*/
QColor OCCViewer_Viewer::clippingColor() const
{
  return myClippingColor;
}

/*!
  Set the texture's scale factor
*/
inline void setTextureScale(Handle(Graphic3d_TextureMap) aTexture, double theScale)
{
  aTexture->GetParams()->SetScale( Graphic3d_Vec2( 1/( theScale*100 ), -1 / ( theScale*100 ) ) );
}

// initialize a texture for clipped region
Handle(Graphic3d_Texture2Dmanual) initClippingTexture( const bool isDefault, const QString& theTexture,
                                                       const bool isModulate, const double theScale )
{
  QString aTextureFile = isDefault ? ":images/hatch.png" : theTexture;
  QPixmap px( aTextureFile );
  const Handle(Image_PixMap) aPixmap = OCCViewer_Utilities::imageToPixmap( px.toImage() );
  Handle(Graphic3d_Texture2Dmanual) aTexture = new Graphic3d_Texture2Dmanual( aPixmap );
  if( aTexture->IsDone() ) {
    aTexture->EnableRepeat();
    isModulate ? aTexture->EnableModulate() : aTexture->DisableModulate();
    setTextureScale( aTexture, theScale );
  }
  return aTexture;
}

/*!
  Sets default texture parameters
  \param isDefault - use/non-use default texture
  \param theTexture - new texture of the clipped region
  \param isModulate - enable/disable texture modulate mode
  \param theScale - scale factor.
*/
void OCCViewer_Viewer::setClippingTextureParams( const bool isDefault, const QString& theTexture,
                                                 const bool isModulate, const double theScale )
{
  myDefaultTextureUsed = isDefault;
  myClippingTexture = theTexture;
  myTextureModulated = isModulate;
  myClippingTextureScale = theScale;

  if( myInternalClipPlanes.IsEmpty() )
    return;

  double hatchScale = computeHatchScale();
  Handle(Graphic3d_Texture2Dmanual) aTexture =
    initClippingTexture( myDefaultTextureUsed, myClippingTexture,
                         myTextureModulated, hatchScale);
  for ( Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt ( myInternalClipPlanes ); aPlaneIt.More(); aPlaneIt.Next() )
    aPlaneIt.Value()->SetCappingTexture( aTexture );

  update();
}

/*!
  \return true if default texture is used
*/
bool OCCViewer_Viewer::isDefaultTextureUsed() const
{
  return myDefaultTextureUsed;
}

/*!
  \return clipping texture
*/
QString OCCViewer_Viewer::clippingTexture() const
{
  return myClippingTexture;
}

/*!
  \return true if texture is modulated
*/
bool OCCViewer_Viewer::isTextureModulated() const
{
  return myTextureModulated;
}

/*!
  \return scale factor of texture
*/
double OCCViewer_Viewer::clippingTextureScale() const
{
  return myClippingTextureScale;
}

/*!
  Builds popup for occ viewer
*/
void OCCViewer_Viewer::contextMenuPopup(QMenu* thePopup)
{
  thePopup->addAction( tr( "MEN_DUMP_VIEW" ), this, SLOT( onDumpView() ) );
  thePopup->addAction( tr( "MEN_CHANGE_BACKGROUND" ), this, SLOT( onChangeBackground() ) );

  thePopup->addSeparator();

  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*)(myViewManager->getActiveView());

  //Support of several toolbars in the popup menu
  QList<QToolBar*> lst = aView->findChildren<QToolBar*>();
  QList<QToolBar*>::const_iterator it = lst.begin(), last = lst.end();
  for ( ; it!=last; it++ ) {
    if ( (*it)->parentWidget()->isVisible() )
      thePopup->addAction( (*it)->toggleViewAction() );
  }
}

/*!
  SLOT: called on dump view operation is activated, stores scene to raster file
*/
void OCCViewer_Viewer::onDumpView()
{
  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->onDumpView();
}

/*!
  SLOT: called if background color is to be changed changed, passes new color to view port
*/
void OCCViewer_Viewer::onChangeBackground()
{
  OCCViewer_ViewWindow* aView = dynamic_cast<OCCViewer_ViewWindow*>(myViewManager->getActiveView());
  if ( !aView )
    return;

  // get supported gradient types
  QStringList gradList;
  QIntList    idList, txtList;
  QString     formats = backgroundData( gradList, idList, txtList );

  // invoke dialog box
  Qtx::BackgroundData bgData = QtxBackgroundDialog::getBackground( aView->background(),  // initial background
								   aView,                // parent for dialog box
								   txtList,              // allowed texture modes
								   true,                 // enable solid color mode
								   true,                 // enable gradient mode
								   false,                // disable custom gradient mode
								   !txtList.isEmpty(),   // enable/disable texture mode
								   gradList,             // gradient names
								   idList,               // gradient identifiers
								   formats );            // image formats

  // set chosen background data to the viewer
  if ( bgData.isValid() )
    aView->setBackground( bgData );
}

/*!
  Updates OCC 3D viewer
*/
void OCCViewer_Viewer::update()
{
  if (!myV3dViewer.IsNull())
    myV3dViewer->Update();

  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->updateGravityCoords();
}

/*!
  \return objects selected in 3D viewer
  \param theList - list to be filled with selected objects
*/
void OCCViewer_Viewer::getSelectedObjects(AIS_ListOfInteractive& theList)
{
  theList.Clear();
  for (myAISContext->InitSelected(); myAISContext->MoreSelected(); myAISContext->NextSelected())
    theList.Append(myAISContext->SelectedInteractive());
}

/*!
  Selects objects in 3D viewer. Other selected objects are left as selected
  \param theList - list objects to be selected
*/
void OCCViewer_Viewer::setObjectsSelected(const AIS_ListOfInteractive& theList)
{
  AIS_ListIteratorOfListOfInteractive aIt;
  for (aIt.Initialize(theList); aIt.More(); aIt.Next())
    myAISContext->AddOrRemoveSelected(aIt.Value(), false);
  myAISContext->UpdateCurrentViewer();
}

/*!
  Auxiliary method to emit signal selectionChanged()
*/
void OCCViewer_Viewer::performSelectionChanged()
{
    emit selectionChanged();
}

/*
 * Defines default lights
 */
void OCCViewer_Viewer::setDefaultLights()
{
  // clear all light sources
  myV3dViewer->InitDefinedLights();
  while ( myV3dViewer->MoreDefinedLights() )
  {
    myV3dViewer->DelLight( myV3dViewer->DefinedLight() );
    myV3dViewer->InitDefinedLights();
  }

  // get light source parameters from preferences
  QColor aColor = SUIT_Session::session()->resourceMgr()->colorValue( "OCCViewer", "light_color", QColor( 0, 0, 0 ) );
  double aDx = SUIT_Session::session()->resourceMgr()->doubleValue( "OCCViewer", "light_dx", 0.0 );
  double aDy = SUIT_Session::session()->resourceMgr()->doubleValue( "OCCViewer", "light_dy", 0.0 );
  double aDz = SUIT_Session::session()->resourceMgr()->doubleValue( "OCCViewer", "light_dz", -1.0 );

  Handle(V3d_DirectionalLight) aLight =
    new V3d_DirectionalLight( V3d_Zneg, OCCViewer::color( aColor ).Name(), Standard_True );
  myV3dViewer->AddLight( aLight );
  if( !( aDx == 0 && aDy == 0 && aDz == 0 ) )
    aLight->SetDirection( aDx, aDy, aDz );
  myV3dViewer->SetLightOn( aLight );
  Handle(V3d_AmbientLight) ambLight = new V3d_AmbientLight();
  myV3dViewer->AddLight( ambLight );
  myV3dViewer->SetLightOn( ambLight );
}

/*!
  Hilights/unhilights object in viewer
  \param obj - object to be updated
  \param hilight - if it is true, object will be hilighted, otherwise it will be unhilighted
  \param update - update current viewer
*/
bool OCCViewer_Viewer::highlight( const Handle(AIS_InteractiveObject)& obj,
                                  bool hilight, bool update )
{
  if( !obj.IsNull() ) {
    if ( hilight && !myAISContext->IsSelected( obj ) )
      myAISContext->AddOrRemoveSelected( obj, false );
    else if ( !hilight && myAISContext->IsSelected( obj ) )
      myAISContext->AddOrRemoveSelected( obj, false );
  }

  if ( update )
    myV3dViewer->Redraw();
  
  return false;
}

/*!
  Unhilights all objects in viewer
  \param updateviewer - update current viewer
*/
bool OCCViewer_Viewer::unHighlightAll( bool updateviewer, bool unselect )
{
  if ( unselect ) {
    myAISContext->ClearSelected( updateviewer );
  } else {
    myAISContext->UnhilightSelected( updateviewer );
  }
  return false;
}

/*!
  \return true if object is in viewer or in collector
  \param obj - object to be checked
  \param onlyInViewer - search object only in viewer (so object must be displayed)
*/
bool OCCViewer_Viewer::isInViewer( const Handle(AIS_InteractiveObject)& obj,
                                   bool /*onlyInViewer*/ )
{
  AIS_ListOfInteractive List;
  myAISContext->DisplayedObjects(List);

  AIS_ListIteratorOfListOfInteractive ite(List);
  for ( ; ite.More(); ite.Next() )
    if( ite.Value()==obj )
      return true;

  return false;
}

/*!
  \return true if object is displayed in viewer
  \param obj - object to be checked
*/
bool OCCViewer_Viewer::isVisible( const Handle(AIS_InteractiveObject)& obj )
{
  return myAISContext->IsDisplayed( obj );
}

/*!
  Sets color of object
  \param obj - object to be updated
  \param color - new color
  \param update - update current viewer
*/
void OCCViewer_Viewer::setColor( const Handle(AIS_InteractiveObject)& obj,
                                 const QColor& color,
                                 bool update )
{
  if( !obj.IsNull() )
  {
    Quantity_Color CSFColor = Quantity_Color ( color.red() / 255.,
                                               color.green() / 255.,
                                               color.blue() / 255.,
                                               Quantity_TOC_RGB );
    obj->SetColor( CSFColor );
  }

  if( update )
    myV3dViewer->Update();
}

/*!
  Changes display mode of object
  \param obj - object to be processed
  \param mode - new display mode
  \param update - update current viewer
*/
void OCCViewer_Viewer::switchRepresentation( const Handle(AIS_InteractiveObject)& obj,
                                             int mode, bool update )
{
  myAISContext->SetDisplayMode( obj, (Standard_Integer)mode, update );
  if( update )
    myV3dViewer->Update();
}

/*!
  Changes transparency of object
  \param obj - object to be processed
  \param trans - new transparency
  \param update - update current viewer
*/
void OCCViewer_Viewer::setTransparency( const Handle(AIS_InteractiveObject)& obj,
                                        float trans, bool update )
{
  myAISContext->SetTransparency( obj, trans, false );
  myAISContext->Redisplay( obj, Standard_False, Standard_True );
  if( update )
    myV3dViewer->Update();
}

bool OCCViewer_Viewer::isColorScaleVisible() const
{
  return !myColorScale.IsNull() && !myAISContext.IsNull() && myAISContext->IsDisplayed( myColorScale );
}

void OCCViewer_Viewer::setColorScaleShown( const bool on )
{
  if ( myColorScale.IsNull() )
    return;
  if ( on )
  {
    if ( !myAISContext->IsDisplayed( myColorScale ) )
      myAISContext->Display( myColorScale, Standard_True );
    myAISContext->Redisplay( myColorScale, Standard_True, Standard_True );
  }
  else
  {
    if ( myAISContext->IsDisplayed( myColorScale ) )
      myAISContext->Erase( myColorScale, Standard_True );
  }
}

/*!
  Inits trihedron if it's enabled
*/
void OCCViewer_Viewer::initTrihedron()
{
  // Basic params
  Handle(Geom_Axis2Placement) anAxis = new Geom_Axis2Placement(gp::XOY());
  myTrihedron = new AIS_Trihedron(anAxis);
  myTrihedron->SetInfiniteState(Standard_True);
  myTrihedron->SetSize(100);

  // Init default colors
  const Quantity_Color rColor(1.0, 0.0, 0.0, Quantity_TOC_RGB);
  const Quantity_Color gColor(0.0, 1.0, 0.0, Quantity_TOC_RGB);
  const Quantity_Color bColor(0.0, 0.0, 1.0, Quantity_TOC_RGB);

#if OCC_VERSION_LARGE >= 0x07070000
  // Set colors for axes
  myTrihedron->SetDatumPartColor(Prs3d_DP_XAxis, rColor);
  myTrihedron->SetDatumPartColor(Prs3d_DP_YAxis, gColor);
  myTrihedron->SetDatumPartColor(Prs3d_DP_ZAxis, bColor);

  // Set the same colors for axes' arrows
  myTrihedron->SetArrowColor(Prs3d_DP_XAxis, rColor);
  myTrihedron->SetArrowColor(Prs3d_DP_YAxis, gColor);
  myTrihedron->SetArrowColor(Prs3d_DP_ZAxis, bColor);
#else
  Quantity_Color Col(193/255., 205/255., 193/255., Quantity_TOC_RGB);
  myTrihedron->SetArrowColor( Col.Name() );
  Handle(Prs3d_Drawer) drawer = myTrihedron->Attributes();
  if (drawer->HasOwnDatumAspect())
  {
    Handle(Prs3d_DatumAspect) daspect = drawer->DatumAspect();

    daspect->LineAspect(Prs3d_DP_XAxis)->SetColor(rColor);
    daspect->LineAspect(Prs3d_DP_YAxis)->SetColor(gColor);
    daspect->LineAspect(Prs3d_DP_ZAxis)->SetColor(bColor);
  }
#endif

  setTrihedronTextFont();
  setTrihedronTextColor();
}

/*!
  Sets font for trihedron labels from the 3D Viewer settings
*/
void OCCViewer_Viewer::setTrihedronTextFont()
{
  OCCViewer_TrihedronSetupAIS trihedronSetup(myTrihedron);
  trihedronSetup.setTextFont();
}

/*!
  Sets color for trihedron labels from the 3D Viewer settings
*/
void OCCViewer_Viewer::setTrihedronTextColor()
{
  OCCViewer_TrihedronSetupAIS trihedronSetup(myTrihedron);
  trihedronSetup.setTextColor();
}

/*!
  \return true if trihedron is visible
*/
bool OCCViewer_Viewer::isTrihedronVisible() const
{
  return !myTrihedron.IsNull() && !myAISContext.IsNull() && myAISContext->IsDisplayed( myTrihedron );
}

/*!
  \return true if view cube is visible
*/
bool OCCViewer_Viewer::isViewCubeVisible() const
{
  return !myViewCube.IsNull() && !myAISContext.IsNull() && myAISContext->IsDisplayed( myViewCube );
}

/*!
  Sets visibility state of trihedron
  \param on - new state
*/

void OCCViewer_Viewer::setTrihedronShown( const bool on )
{
  if ( myTrihedron.IsNull() )
    return;

  if ( on ) {
    myAISContext->Display( myTrihedron,
                           0 /*wireframe*/,
                           -1 /* selection mode */,
                           Standard_True /* update viewer*/,
                           AIS_DS_Displayed /* display status */);
    myAISContext->Deactivate( myTrihedron );
  }
  else {
    myAISContext->Erase( myTrihedron , Standard_True );
  }
}

/*!
  Sets visibility state of view cube
  \param on - new state
*/

void OCCViewer_Viewer::setViewCubeShown( const bool on )
{
  if ( myViewCube.IsNull() )
    return;

  if ( on ) {
    myAISContext->Display( myViewCube,
                           0 /*wireframe*/,
                           0 /* selection mode */,
                           Standard_True /* update viewer*/,
                           AIS_DS_Displayed /* display status */);
  }
  else {
    myAISContext->Erase( myViewCube, Standard_True );
  }
}

/*!
  Set View Cube parameters from preferences
*/
void OCCViewer_Viewer::setViewCubeParamsFromPreferences()
{
  // Resource manager
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if (!resMgr || myViewCube.IsNull())
    return;

  bool isVisibleVC = isVisible(myViewCube);

  if (resMgr->booleanValue("OCCViewer", "viewcube_custom", false)) {
    // Use custom settings from preferences
    QColor aColor;

    // Box color
    aColor = resMgr->colorValue("OCCViewer", "viewcube_color", QColor(255, 255, 255));
    myViewCube->SetBoxColor(OCCViewer::color(aColor));

    // Size
    myViewCube->SetSize(resMgr->doubleValue("OCCViewer", "viewcube_size", 70.0));

    // Text color
    aColor = resMgr->colorValue("OCCViewer", "viewcube_text_color", QColor(0, 0, 0));
    myViewCube->SetTextColor(OCCViewer::color(aColor));
  }
  else {
    myViewCube->ResetStyles();
  }

  // Axes
  myViewCube->SetDrawAxes(resMgr->booleanValue("OCCViewer", "viewcube_axes", false));

  // Animation duration (sec)
  myViewCube->SetDuration(resMgr->doubleValue("OCCViewer", "viewcube_duration", 0.5));

  // Update the viewer
  if (isVisibleVC)
    myAISContext->Redisplay( myViewCube, Standard_True);
}

/*!
  \return trihedron size
*/
double OCCViewer_Viewer::trihedronSize() const
{
  double sz = 0;
  if ( !myTrihedron.IsNull() )
    sz = myTrihedron->Size();
  return sz;
}

/*!
  Changes trihedron size
  \param sz - new size
*/
void OCCViewer_Viewer::setTrihedronSize( const double sz, bool isRelative )
{
  if ( myTrihedronSize != sz || isRelative != myIsRelative) {
    myTrihedronSize = sz; 
    myIsRelative = isRelative;
    updateTrihedron();
  }
}

/*!
  Set number of isolines
  \param u - u-isolines (first parametric co-ordinate)
  \param v - v-isolines (second parametric co-ordinate)
*/
void OCCViewer_Viewer::setIsos( const int u, const int v )
{
  Handle(AIS_InteractiveContext) ic = getAISContext();
  if ( ic.IsNull() )
  return;

  ic->SetIsoNumber( u, AIS_TOI_IsoU );
  ic->SetIsoNumber( v, AIS_TOI_IsoV );
}

/*!
  \return number of isolines
  \param u - to return u-isolines (first parametric co-ordinate)
  \param v - to return v-isolines (second parametric co-ordinate)
*/
void OCCViewer_Viewer::isos( int& u, int& v ) const
{
  Handle(AIS_InteractiveContext) ic = getAISContext();
  if ( !ic.IsNull() )
  {
    u = ic->IsoNumber( AIS_TOI_IsoU );
    v = ic->IsoNumber( AIS_TOI_IsoV );
  }
}

/* 
 * Returns a new OCCViewer_ViewWindow instance which will be placed as a sub window in ViewFrame
 */
OCCViewer_ViewWindow* OCCViewer_Viewer::createSubWindow()
{
  return new OCCViewer_ViewWindow(0,  this);
}

// obsolete  
QColor OCCViewer_Viewer::backgroundColor( int theViewId ) const
{
  return background( theViewId ).color();
}

Qtx::BackgroundData OCCViewer_Viewer::background( int theViewId ) const
{
  return ( theViewId >= 0 && theViewId < myBackgrounds.count() ) ? myBackgrounds[theViewId] : Qtx::BackgroundData();
}

// obsolete
void OCCViewer_Viewer::setBackgroundColor( int theViewId, const QColor& theColor )
{
  if ( theColor.isValid() ) {
    Qtx::BackgroundData bg = background( theViewId );
    bg.setColor( theColor );
    setBackground( theViewId, bg );
  }
}

void OCCViewer_Viewer::setBackground( int theViewId, const Qtx::BackgroundData& theBackground )
{
  if ( theBackground.isValid() && theViewId >= 0 && theViewId < myBackgrounds.count() )
    myBackgrounds[theViewId] = theBackground;    
}


/*!
  Set the show static trihedron flag
*/
void OCCViewer_Viewer::setStaticTrihedronDisplayed(const bool on)
{
  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView ) aView->showStaticTrihedron( on );
}

/*!
  Set the font for axes labels of static trihedron
*/
void OCCViewer_Viewer::setStaticTrihedronTextFont()
{
  OCCViewer_ViewPort3d* viewport = getViewPort();
  if (!viewport)
    return;

  viewport->setStaticTrihedronTextFont();
}


/*!
  Set the color for axes labels of static trihedron
*/
void OCCViewer_Viewer::setStaticTrihedronTextColor()
{
  OCCViewer_ViewPort3d* viewport = getViewPort();
  if (!viewport)
    return;

  viewport->setStaticTrihedronTextColor();
}

/*!
  Get the viewport for active view
*/
OCCViewer_ViewPort3d* OCCViewer_Viewer::getViewPort()
{
  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*)(myViewManager->getActiveView());
  if (!aView)
    return nullptr;

  return aView->getViewPort();
}

/*!
  Get new and current trihedron size corresponding to the current model size
*/
bool OCCViewer_Viewer::computeTrihedronSize( double& theNewSize, double& theSize )
{
  theNewSize = 100;
  theSize = 100;

  //SRN: BUG IPAL8996, a usage of method ActiveView without an initialization
  Handle(V3d_Viewer) viewer = getViewer3d();
  viewer->InitActiveViews();
  if(!viewer->MoreActiveViews()) return false;

  Handle(V3d_View) view3d = viewer->ActiveView();
  //SRN: END of fix

  if ( view3d.IsNull() )
    return false;

  double aMaxSide = computeSceneSize( view3d );
  setClippingTextureParams(myDefaultTextureUsed, myClippingTexture, myTextureModulated, myClippingTextureScale);

  // IPAL21687
  // The boundary box of the view may be initialized but nullified
  // (case of infinite objects)
  if ( aMaxSide < Precision::Confusion() )
    return false;

  float aSizeInPercents = SUIT_Session::session()->resourceMgr()->doubleValue("3DViewer","trihedron_size", 100.);

  static float EPS = 5.0E-3f;
  theSize = getTrihedron()->Size();
  theNewSize = aMaxSide*aSizeInPercents / 100.0;

  return fabs( theNewSize - theSize ) > theSize    * EPS ||
         fabs( theNewSize - theSize ) > theNewSize * EPS;
}

/*!
 * Compute scene size
 */
double OCCViewer_Viewer::computeSceneSize(const Handle(V3d_View)& view3d) const
{
  double aMaxSide = 0;
  double Xmin = 0, Ymin = 0, Zmin = 0, Xmax = 0, Ymax = 0, Zmax = 0;

  Bnd_Box aBox = view3d->View()->MinMaxValues();
  Xmin = aBox.IsVoid() ? RealFirst() : aBox.CornerMin().X();
  Ymin = aBox.IsVoid() ? RealFirst() : aBox.CornerMin().Y();
  Zmin = aBox.IsVoid() ? RealFirst() : aBox.CornerMin().Z();
  Xmax = aBox.IsVoid() ? RealLast()  : aBox.CornerMax().X();
  Ymax = aBox.IsVoid() ? RealLast()  : aBox.CornerMax().Y();
  Zmax = aBox.IsVoid() ? RealLast()  : aBox.CornerMax().Z();

  if ( Xmin != RealFirst() && Ymin != RealFirst() && Zmin != RealFirst() &&
       Xmax != RealLast()  && Ymax != RealLast()  && Zmax != RealLast() )
  {
    aMaxSide = Xmax - Xmin;
    if ( aMaxSide < Ymax -Ymin ) aMaxSide = Ymax -Ymin;
    if ( aMaxSide < Zmax -Zmin ) aMaxSide = Zmax -Zmin;
  }

  return aMaxSide;
}

/*! 
 * Update the size of the trihedron
 */
void OCCViewer_Viewer::updateTrihedron()
{
  if ( myTrihedron.IsNull() )
    return;

  if(myIsRelative){
    double newSz, oldSz;
    
    if(computeTrihedronSize(newSz, oldSz))
      myTrihedron->SetSize(newSz);
    
  } else if(myTrihedron->Size() != myTrihedronSize) {
    myTrihedron->SetSize(myTrihedronSize);
  }
}

/*!
  Set number of isolines
  \param u - u-isolines (first parametric co-ordinate)
  \param v - v-isolines (second parametric co-ordinate)
*/
void OCCViewer_Viewer::setSelectionOptions( bool isPreselectionEnabled, bool isSelectionEnabled )
{
  myPreselectionEnabled = isPreselectionEnabled;
  mySelectionEnabled = isSelectionEnabled;
  //clear current selection in the viewer
  
  if(!mySelectionEnabled) {
    myAISContext->ClearSelected( Standard_True );
  }
}

/*!
  Creates clipping plane based on the incoming plane
*/
Handle(Graphic3d_ClipPlane) OCCViewer_Viewer::createClipPlane(const gp_Pln& thePlane, const Standard_Boolean theIsOn)
{
  Handle(Graphic3d_ClipPlane) aGraphic3dPlane = new Graphic3d_ClipPlane( thePlane );
  aGraphic3dPlane->SetOn( theIsOn );
  aGraphic3dPlane->SetCapping( Standard_True );

  // set capping color
  setCappingColor( aGraphic3dPlane, myClippingColor );

  // set capping texture
  double hatchScale = computeHatchScale();
  aGraphic3dPlane->SetCappingTexture( initClippingTexture( myDefaultTextureUsed, myClippingTexture,
                                                           myTextureModulated, hatchScale ) );
  aGraphic3dPlane->SetUseObjectMaterial(myClippingUseObjColor);

  return aGraphic3dPlane;
}
/*!
  Applies clipping planes to clippable objects
*/
void OCCViewer_Viewer::setClipPlanes(ClipPlanesList theList)
{
  // 1. Remove existing clipping planes
  myClipPlanes.clear();
  myInternalClipPlanes.Clear();

  // 2. Create new clipping planes
  ClipPlanesList::iterator inIt = theList.begin();
  for (;inIt != theList.end(); inIt++ )
  {
    OCCViewer_ClipPlane aPlane = *inIt;

    double aDx = 0.0, aDy = 0.0, aDz = 0.0;
    aPlane.OrientationToXYZ( aDx, aDy, aDz );

    gp_Pnt anOrigin( aPlane.X, aPlane.Y, aPlane.Z );
    gp_Dir aDirection( aDx, aDy, aDz );

    myInternalClipPlanes.Append( createClipPlane( gp_Pln( anOrigin, aDirection ), aPlane.IsOn ) );
    myClipPlanes.push_back( aPlane );
  }

  applyClippingPlanes(false);
}

/*!
  Returns the clipping planes applied to the displayed objects.
*/
ClipPlanesList OCCViewer_Viewer::getClipPlanes() const {
  return myClipPlanes;
}
/*!
  Applies clipping planes to given object objects
*/
void OCCViewer_Viewer::applyExistingClipPlanesToObject (const Handle(AIS_InteractiveObject)& theObject)
{
  Handle(ViewerData_AISShape) aShape = Handle(ViewerData_AISShape)::DownCast (theObject);
  if (!aShape.IsNull() && aShape->IsClippable())
  {
    aShape->SetClipPlanes (new Graphic3d_SequenceOfHClipPlane(myInternalClipPlanes)); // todo: store clipping planes in a handle?
  }
}

/*!
  Applies clipping planes to all visible (and clippable) objects
*/
void OCCViewer_Viewer::applyClippingPlanes(bool theUpdateHatch)
{
  // Do not return immediately, if there are no local clipping planes.
  // This is necessary to clear the clipping planes from the objects.
  // Otherwise, the objects will be clipped by the last set of clipping planes.

  if (theUpdateHatch) {
    double hatchScale = computeHatchScale();
    for ( Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt ( myInternalClipPlanes ); aPlaneIt.More(); aPlaneIt.Next() ) {
      Handle(Graphic3d_TextureMap) aTexture = aPlaneIt.Value()->CappingTexture();
      setTextureScale(aTexture, hatchScale);
    }
  }

  AIS_ListOfInteractive aList;
  myAISContext->DisplayedObjects (aList);
  for ( AIS_ListIteratorOfListOfInteractive anIter (aList); anIter.More(); anIter.Next() ) {
    Handle(AIS_InteractiveObject) anObj = anIter.Value();
    Handle(ViewerData_AISShape) aShape = Handle(ViewerData_AISShape)::DownCast (anObj);
    if (!aShape.IsNull() && aShape->IsClippable()) {
      // Update the clipping plane of each clippable object.
      // If the internal clipping planes list is empty, the object will be displayed as is (unclipped).
      aShape->SetClipPlanes(new Graphic3d_SequenceOfHClipPlane(myInternalClipPlanes));
    }
  }
}

/*!
  Returns the pointer to the clipping dialog box.
*/
OCCViewer_ClippingDlg* OCCViewer_Viewer::getClippingDlg() const{
  return myClippingDlg;
}


/*!
  Stores pointer to the clipping dialog box.
*/
void OCCViewer_Viewer::setClippingDlg(OCCViewer_ClippingDlg* theDlg) {
  if(myClippingDlg != theDlg) {
    myClippingDlg = theDlg;
  }
}


bool OCCViewer_Viewer::enableDrawMode( bool on )
{
  //!! To be done for view windows
  if ( !myViewManager )
    return false;

  bool prev = false;
  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win ) {
      prev = prev || win->enableDrawMode( on ); 
    }
  }
  return prev;
}

 double OCCViewer_Viewer::computeHatchScale() const
 {
  // bos#40617:
  // Update the hatch scale of the clipping texture based on the total size of all visible objects
  double hatchScale = myClippingTextureScale;
  Handle(V3d_Viewer) viewer = getViewer3d();
  viewer->InitActiveViews();
  if (viewer->MoreActiveViews())
  {
    Handle(V3d_View) view3d = viewer->ActiveView();
    if ( !view3d.IsNull() )
    {
      double aMaxSide = computeSceneSize( view3d );
      hatchScale = aMaxSide * myClippingTextureScale * 0.01;
    }
  }
  return hatchScale;
 }
