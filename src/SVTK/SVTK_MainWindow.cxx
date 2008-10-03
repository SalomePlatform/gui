//  SALOME VTKViewer : build VTK viewer into Salome desktop
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : 
//  Author : 
//  Module : SALOME
//  $Header$

#include "SALOME_Actor.h"

#include <QImage>

#include <vtkGenericRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

#include <QtxAction.h>
#include <QtxMultiAction.h>
#include <QtxToolBar.h>
#include <QtxActionToolMgr.h>

#include <SUIT_MessageBox.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_Tools.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

#include "SVTK_NonIsometricDlg.h"
#include "SVTK_UpdateRateDlg.h"
#include "SVTK_CubeAxesDlg.h"
#include "SVTK_SetRotationPointDlg.h"
#include "SVTK_ViewParameterDlg.h"
#include "SVTK_MainWindow.h"
#include "SVTK_Event.h"
#include "SVTK_Renderer.h"
#include "SVTK_RenderWindowInteractor.h"
#include "SVTK_InteractorStyle.h"
#include "SVTK_KeyFreeInteractorStyle.h"
#include "SVTK_Selector.h"
#include "SVTK_ComboAction.h"
#include "SVTK_Recorder.h"
#include "SVTK_RecorderDlg.h"

/*!
  Constructor
*/
SVTK_MainWindow::SVTK_MainWindow(QWidget* theParent, 
				 const char* theName,
				 SUIT_ResourceMgr* theResourceMgr,
				 SUIT_ViewWindow* theViewWindow) :
  QMainWindow(theParent),
  myViewWindow(theViewWindow),
  myKeyFreeInteractorStyle(SVTK_KeyFreeInteractorStyle::New())
{
  setObjectName(theName);
  setWindowFlags( windowFlags() & ~Qt::Window );
  // specific of vtkSmartPointer
  myKeyFreeInteractorStyle->Delete();
}

/*!
  To initialize the class
*/
void SVTK_MainWindow::Initialize(SVTK_RenderWindowInteractor* theInteractor)
{
  myToolBar = toolMgr()->createToolBar( tr("LBL_TOOLBAR_LABEL"), -1, this );
  myRecordingToolBar = toolMgr()->createToolBar( tr("LBL_TOOLBAR_RECORD_LABEL"), -1, this );

  createActions( SUIT_Session::session()->activeApplication()->resourceMgr() );
  createToolBar();

  myInteractor = theInteractor;
  SetEventDispatcher(myInteractor->GetDevice());

  setCentralWidget(myInteractor);
  myInteractor->setBackgroundRole( QPalette::NoRole );//NoBackground

  myInteractor->setFocusPolicy(Qt::StrongFocus);
  myInteractor->setFocus();
  setFocusProxy(myInteractor);

  myUpdateRateDlg = new SVTK_UpdateRateDlg( action( UpdateRate ), this, "SVTK_UpdateRateDlg" );
  myNonIsometricDlg = new SVTK_NonIsometricDlg( action( NonIsometric ), this, "SVTK_NonIsometricDlg" );
  myCubeAxesDlg = new SVTK_CubeAxesDlg( action( GraduatedAxes ), this, "SVTK_CubeAxesDlg" );
  mySetRotationPointDlg = new SVTK_SetRotationPointDlg
    ( action( ChangeRotationPointId ), this, "SVTK_SetRotationPointDlg" );
  myViewParameterDlg = new SVTK_ViewParameterDlg
    ( action( ViewParametersId ), this, "SVTK_ViewParameterDlg" );

  myRecorder = SVTK_Recorder::New();
  //myRecorder->CheckExistAVIMaker();
  //if(myRecorder->ErrorStatus())
  //  myRecordingToolBar->setEnabled(false);

  myRecorder->SetNbFPS( 17.3 );
  myRecorder->SetQuality( 100 );
  myRecorder->SetProgressiveMode( true );
  myRecorder->SetUseSkippedFrames( true );
  myRecorder->SetRenderWindow( theInteractor->getRenderWindow() );
}

/*!
  Destructor
*/
SVTK_MainWindow::~SVTK_MainWindow()
{
  if(myRecorder)
    myRecorder->Delete();
}

/*!
  \return assigned tool manager
*/
QtxActionToolMgr* SVTK_MainWindow::toolMgr() const
{
  return myViewWindow->toolMgr();
}

/*!
  \return used SVTK_RenderWindowInteractor
*/
SVTK_RenderWindowInteractor* SVTK_MainWindow::GetInteractor()
{
  return myInteractor;
}

/*!
  \return used #vtkRenderWindowInteractor (obsolete)
*/
vtkRenderWindowInteractor* SVTK_MainWindow::getInteractor()
{
  return GetInteractor()->GetDevice();
}

/*!
  \return used vtkRenderWindow (obsolete)
*/
vtkRenderWindow* SVTK_MainWindow::getRenderWindow()
{
  return GetInteractor()->getRenderWindow();
}

/*!
  To repaint the view
  \param theUpdateTrihedron - adjust trihedron
*/
void SVTK_MainWindow::Repaint(bool theUpdateTrihedron)
{
  if(theUpdateTrihedron) 
    GetRenderer()->OnAdjustTrihedron();

  GetInteractor()->update();

  if ( (SVTK_InteractorStyle*)GetInteractorStyle() )
    ((SVTK_InteractorStyle*)GetInteractorStyle())->OnTimer();
}

/*!
  To invoke a VTK event on SVTK_RenderWindowInteractor instance
*/
void SVTK_MainWindow::InvokeEvent(unsigned long theEvent, void* theCallData)
{
  GetInteractor()->InvokeEvent(theEvent,theCallData);
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::GetInteractorStyle
*/
vtkInteractorStyle* SVTK_MainWindow::GetInteractorStyle()
{
  return GetInteractor()->GetInteractorStyle();
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::PushInteractorStyle
*/
void SVTK_MainWindow::PushInteractorStyle(vtkInteractorStyle* theStyle)
{
  GetInteractor()->PushInteractorStyle(theStyle);
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::PopInteractorStyle
*/
void SVTK_MainWindow::PopInteractorStyle()
{
  GetInteractor()->PopInteractorStyle();
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::GetSelector
*/
SVTK_Selector* SVTK_MainWindow::GetSelector()
{
  return GetInteractor()->GetSelector();
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::SelectionMode
*/
Selection_Mode SVTK_MainWindow::SelectionMode()
{
  return GetSelector()->SelectionMode();
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::SetSelectionMode
*/
void SVTK_MainWindow::SetSelectionMode(Selection_Mode theMode)
{
  GetSelector()->SetSelectionMode(theMode);
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::GetRenderer
*/
SVTK_Renderer* SVTK_MainWindow::GetRenderer()
{
  return GetInteractor()->GetRenderer();
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::getRenderer
*/
vtkRenderer* SVTK_MainWindow::getRenderer()
{
  return GetInteractor()->getRenderer();
}

/*!
  Sets background color of the view
  \param theColor - new background color
*/
void SVTK_MainWindow::SetBackgroundColor(const QColor& theColor)
{
  getRenderer()->SetBackground(theColor.red()/255.0, 
			       theColor.green()/255.0,
			       theColor.blue()/255.0);
}

/*!
  \return background color of the view
*/
QColor SVTK_MainWindow::BackgroundColor()
{
  vtkFloatingPointType aBackgroundColor[3];
  getRenderer()->GetBackground(aBackgroundColor);
  return QColor(int(aBackgroundColor[0]*255), 
		int(aBackgroundColor[1]*255), 
		int(aBackgroundColor[2]*255));
}

/*!
  Redirect the request to SVTK_Renderer::GetScale
*/
void SVTK_MainWindow::GetScale( double theScale[3] ) 
{
  GetRenderer()->GetScale( theScale );
}

/*!
  Redirect the request to SVTK_Renderer::SetScale
*/
void SVTK_MainWindow::SetScale( double theScale[3] ) 
{
  GetRenderer()->SetScale( theScale );
  Repaint();
}

/*!
  Redirect the request to SVTK_Renderer::AddActor
*/
void SVTK_MainWindow::AddActor(VTKViewer_Actor* theActor, 
			       bool theIsUpdate)
{
  GetRenderer()->AddActor(theActor);
  if(theIsUpdate) 
    Repaint();
}

/*!
  Redirect the request to SVTK_Renderer::RemoveActor
*/
void SVTK_MainWindow::RemoveActor(VTKViewer_Actor* theActor, 
				  bool theIsUpdate)
{
  GetRenderer()->RemoveActor(theActor);
  if(theIsUpdate) 
    Repaint();
}

/*!
  Redirect the request to SVTK_InteractorStyle::SetTrihedronSize
*/
void SVTK_MainWindow::SetIncrementalSpeed( const int theValue, const int theMode )
{
  if ( (SVTK_InteractorStyle*)GetInteractorStyle() )
    ((SVTK_InteractorStyle*)GetInteractorStyle())->SetIncrementSpeed(theValue, theMode);
}

/*!
  Redirect the request to SVTK_InteractorStyle
*/
void SVTK_MainWindow::SetSMButtons( const int theBtn1, const int theBtn2, const int theBtn3 )
{
  int val = theBtn1;
  myEventDispatcher->InvokeEvent(SVTK::SetSMDecreaseSpeedEvent, &val);
  val = theBtn2;
  myEventDispatcher->InvokeEvent(SVTK::SetSMIncreaseSpeedEvent, &val);
  val = theBtn3;
  myEventDispatcher->InvokeEvent(SVTK::SetSMDominantCombinedSwitchEvent, &val);
}


/*!
  Redirect the request to SVTK_Renderer::GetTrihedronSize
*/
vtkFloatingPointType SVTK_MainWindow::GetTrihedronSize()
{
  return GetRenderer()->GetTrihedronSize();
}

/*!
  Redirect the request to SVTK_Renderer::SetTrihedronSize
*/
void SVTK_MainWindow::SetTrihedronSize( const vtkFloatingPointType theSize, const bool theRelative )
{
  GetRenderer()->SetTrihedronSize(theSize, theRelative);
  Repaint();
}


/*! If parameter theIsForcedUpdate is true, recalculate parameters for
 *  trihedron and cube axes, even if trihedron and cube axes is invisible.
 */
void SVTK_MainWindow::AdjustActors()
{
  GetRenderer()->AdjustActors();
  Repaint();
}

/*!
  Redirect the request to SVTK_Renderer::IsTrihedronDisplayed
*/
bool SVTK_MainWindow::IsTrihedronDisplayed()
{
  return GetRenderer()->IsTrihedronDisplayed();
}

/*!
  Redirect the request to SVTK_Renderer::IsCubeAxesDisplayed
*/
bool SVTK_MainWindow::IsCubeAxesDisplayed()
{
  return GetRenderer()->IsCubeAxesDisplayed();
}

/*!
  Redirect the request to SVTK_Renderer::GetTrihedron
*/
VTKViewer_Trihedron* SVTK_MainWindow::GetTrihedron() 
{ 
  return GetRenderer()->GetTrihedron(); 
}

/*!
  Redirect the request to SVTK_Renderer::GetCubeAxes
*/
SVTK_CubeAxesActor2D* SVTK_MainWindow::GetCubeAxes() 
{ 
  return GetRenderer()->GetCubeAxes(); 
}

/*!
  \return toolbar of svtk main window
*/
QToolBar* SVTK_MainWindow::getToolBar()
{
  return toolMgr()->toolBar( myToolBar );
}

void SVTK_MainWindow::SetEventDispatcher(vtkObject* theDispatcher)
{
  myEventDispatcher = theDispatcher;
}

#if defined(WIN32) && !defined(_DEBUG)
#pragma optimize( "", off )
#endif

/*!
  Creates all actions of svtk main window
*/
void SVTK_MainWindow::createActions(SUIT_ResourceMgr* theResourceMgr)
{
  QtxAction* anAction;
  QtxActionToolMgr* mgr = toolMgr();

  // Dump view
  anAction = new QtxAction(tr("MNU_DUMP_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_DUMP" ) ),
			   tr( "MNU_DUMP_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_DUMP_VIEW"));
  connect(anAction, SIGNAL(activated()), myViewWindow, SLOT(onDumpView()));
  mgr->registerAction( anAction, DumpId );

  // FitAll
  anAction = new QtxAction(tr("MNU_FITALL"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITALL" ) ),
			   tr( "MNU_FITALL" ), 0, this);
  anAction->setStatusTip(tr("DSC_FITALL"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onFitAll()));
  mgr->registerAction( anAction, FitAllId );

  // FitRect
  anAction = new QtxAction(tr("MNU_FITRECT"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITAREA" ) ),
			   tr( "MNU_FITRECT" ), 0, this);
  anAction->setStatusTip(tr("DSC_FITRECT"));
  connect(anAction, SIGNAL(activated()), this, SLOT(activateWindowFit()));
  mgr->registerAction( anAction, FitRectId );

  // Zoom
  anAction = new QtxAction(tr("MNU_ZOOM_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ZOOM" ) ),
			   tr( "MNU_ZOOM_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(activateZoom()));
  mgr->registerAction( anAction, ZoomId );

  // Panning
  anAction = new QtxAction(tr("MNU_PAN_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_PAN" ) ),
			   tr( "MNU_PAN_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(activatePanning()));
  mgr->registerAction( anAction, PanId );

  // Global Panning
  anAction = new QtxAction(tr("MNU_GLOBALPAN_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_GLOBALPAN" ) ),
			   tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(activateGlobalPanning()));
  mgr->registerAction( anAction, GlobalPanId );

  // Change rotation point
  anAction = new QtxAction(tr("MNU_CHANGINGROTATIONPOINT_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_ROTATION_POINT" ) ),
			   tr( "MNU_CHANGINGROTATIONPOINT_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_CHANGINGROTATIONPOINT_VIEW"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onChangeRotationPoint(bool)));
  mgr->registerAction( anAction, ChangeRotationPointId );

  // Rotation
  anAction = new QtxAction(tr("MNU_ROTATE_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ROTATE" ) ),
			   tr( "MNU_ROTATE_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_ROTATE_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(activateRotation()));
  mgr->registerAction( anAction, RotationId );

  // Projections
  anAction = new QtxAction(tr("MNU_FRONT_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FRONT" ) ),
			   tr( "MNU_FRONT_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_FRONT_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onFrontView()));
  mgr->registerAction( anAction, FrontId );

  anAction = new QtxAction(tr("MNU_BACK_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_BACK" ) ),
			   tr( "MNU_BACK_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_BACK_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onBackView()));
  mgr->registerAction( anAction, BackId );

  anAction = new QtxAction(tr("MNU_TOP_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_TOP" ) ),
			   tr( "MNU_TOP_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_TOP_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onTopView()));
  mgr->registerAction( anAction, TopId );

  anAction = new QtxAction(tr("MNU_BOTTOM_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_BOTTOM" ) ),
			   tr( "MNU_BOTTOM_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_BOTTOM_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onBottomView()));
  mgr->registerAction( anAction, BottomId );

  anAction = new QtxAction(tr("MNU_LEFT_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_LEFT" ) ),
			   tr( "MNU_LEFT_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_LEFT_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onLeftView()));
  mgr->registerAction( anAction, LeftId );

  anAction = new QtxAction(tr("MNU_RIGHT_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_RIGHT" ) ),
			   tr( "MNU_RIGHT_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_RIGHT_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onRightView()));
  mgr->registerAction( anAction, RightId );

  // Reset
  anAction = new QtxAction(tr("MNU_RESET_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_RESET" ) ),
			   tr( "MNU_RESET_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_RESET_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onResetView()));
  mgr->registerAction( anAction, ResetId );

  // onViewTrihedron: Shows - Hides Trihedron
  anAction = new QtxAction(tr("MNU_SHOW_TRIHEDRON"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_TRIHEDRON" ) ),
			   tr( "MNU_SHOW_TRIHEDRON" ), 0, this);
  anAction->setStatusTip(tr("DSC_SHOW_TRIHEDRON"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onViewTrihedron()));
  mgr->registerAction( anAction, ViewTrihedronId );

  // onNonIsometric: Manage non-isometric params
  anAction = new QtxAction(tr("MNU_SVTK_SCALING"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_SCALING" ) ),
			   tr( "MNU_SVTK_SCALING" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_SCALING"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onNonIsometric(bool)));
  mgr->registerAction( anAction, NonIsometric );

  // onGraduatedAxes: Manage graduated axes params
  anAction = new QtxAction(tr("MNU_SVTK_GRADUATED_AXES"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_GRADUATED_AXES" ) ),
			   tr( "MNU_SVTK_GRADUATED_AXES" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_GRADUATED_AXES"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onGraduatedAxes(bool)));
  mgr->registerAction( anAction, GraduatedAxes );

  // onGraduatedAxes: Manage graduated axes params
  anAction = new QtxAction(tr("MNU_SVTK_UPDATE_RATE"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_UPDATE_RATE" ) ),
			   tr( "MNU_SVTK_UPDATE_RATE" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_UPDATE_RATE"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onUpdateRate(bool)));
  mgr->registerAction( anAction, UpdateRate );

  // Set projection mode
  SVTK_ComboAction* aModeAction = new SVTK_ComboAction(tr("MNU_SVTK_PROJECTION_MODE"), this);
  aModeAction->setStatusTip(tr("DSC_SVTK_PROJECTION_MODE"));
  aModeAction->insertItem(theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_VIEW_PARALLEL" ) ) );
  aModeAction->insertItem(theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_VIEW_PERSPECTIVE" ) ) );
  connect(aModeAction, SIGNAL(triggered(int)), this, SLOT(onProjectionMode(int)));
  mgr->registerAction( aModeAction, ProjectionModeId );

  // View Parameters
  anAction = new QtxAction(tr("MNU_VIEWPARAMETERS_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_VIEW_PARAMETERS" ) ),
			   tr( "MNU_VIEWPARAMETERS_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_VIEWPARAMETERS_VIEW"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onViewParameters(bool)));
  mgr->registerAction( anAction, ViewParametersId );

  // Switch between interaction styles
  anAction = new QtxAction(tr("MNU_SVTK_STYLE_SWITCH"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_STYLE_SWITCH" ) ),
			   tr( "MNU_SVTK_STYLE_SWITCH" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_STYLE_SWITCH"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onSwitchInteractionStyle(bool)));
  mgr->registerAction( anAction, SwitchInteractionStyleId );

  // Start recording
  myStartAction = new QtxAction(tr("MNU_SVTK_RECORDING_START"), 
				theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_RECORDING_START" ) ),
				tr( "MNU_SVTK_RECORDING_START" ), 0, this);
  myStartAction->setStatusTip(tr("DSC_SVTK_RECORDING_START"));
  connect( myStartAction, SIGNAL( triggered ( bool ) ), this, SLOT( onStartRecording() ) );
  mgr->registerAction( myStartAction, StartRecordingId );

  // Play recording
  myPlayAction = new QtxAction(tr("MNU_SVTK_RECORDING_PLAY"), 
			       theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_RECORDING_PLAY" ) ),
			       tr( "MNU_SVTK_RECORDING_PLAY" ), 0, this);
  myPlayAction->setStatusTip(tr("DSC_SVTK_RECORDING_PLAY"));
  myPlayAction->setEnabled( false );
  connect( myPlayAction, SIGNAL( triggered ( bool ) ), this, SLOT( onPlayRecording() ) );
  mgr->registerAction( myPlayAction, PlayRecordingId );

  // Pause recording
  myPauseAction = new QtxAction(tr("MNU_SVTK_RECORDING_PAUSE"), 
				theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_RECORDING_PAUSE" ) ),
				tr( "MNU_SVTK_RECORDING_PAUSE" ), 0, this);
  myPauseAction->setStatusTip(tr("DSC_SVTK_RECORDING_PAUSE"));
  myPauseAction->setEnabled( false );
  connect( myPauseAction, SIGNAL( triggered ( bool ) ), this, SLOT( onPauseRecording() ) );
  mgr->registerAction( myPauseAction, PauseRecordingId );

  // Stop recording
  myStopAction = new QtxAction(tr("MNU_SVTK_RECORDING_STOP"), 
			       theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_RECORDING_STOP" ) ),
			       tr( "MNU_SVTK_RECORDING_STOP" ), 0, this);
  myStopAction->setStatusTip(tr("DSC_SVTK_RECORDING_STOP"));
  myStopAction->setEnabled( false );
  connect( myStopAction, SIGNAL( triggered ( bool ) ), this, SLOT( onStopRecording() ) );
  mgr->registerAction( myStopAction, StopRecordingId );
}

#if defined(WIN32) && !defined(_DEBUG)
#pragma optimize( "", on )
#endif

/*!
  Creates toolbar of svtk main window
*/
void SVTK_MainWindow::createToolBar()
{
  QtxActionToolMgr* mgr = toolMgr();
  
  mgr->append( DumpId, myToolBar );
  mgr->append( SwitchInteractionStyleId, myToolBar );
  mgr->append( ViewTrihedronId, myToolBar );

  QtxMultiAction* aScaleAction = new QtxMultiAction( this );
  aScaleAction->insertAction( action( FitAllId ) );
  aScaleAction->insertAction( action( FitRectId ) );
  aScaleAction->insertAction( action( ZoomId ) );
  mgr->append( aScaleAction, myToolBar );

  QtxMultiAction* aPanningAction = new QtxMultiAction( this );
  aPanningAction->insertAction( action( PanId ) );
  aPanningAction->insertAction( action( GlobalPanId ) );
  mgr->append( aPanningAction, myToolBar );

  mgr->append( ChangeRotationPointId, myToolBar );

  mgr->append( RotationId, myToolBar );

  QtxMultiAction* aViewsAction = new QtxMultiAction( this );
  aViewsAction->insertAction( action( FrontId ) );
  aViewsAction->insertAction( action( BackId ) );
  aViewsAction->insertAction( action( TopId ) );
  aViewsAction->insertAction( action( BottomId ) );
  aViewsAction->insertAction( action( LeftId ) );
  aViewsAction->insertAction( action( RightId ) );
  mgr->append( aViewsAction, myToolBar );

  mgr->append( ResetId, myToolBar );

  mgr->append( UpdateRate, myToolBar );
  mgr->append( NonIsometric, myToolBar );
  mgr->append( GraduatedAxes, myToolBar );

  mgr->append( ViewParametersId, myToolBar );
  mgr->append( ProjectionModeId, myToolBar );

  mgr->append( StartRecordingId, myRecordingToolBar );
  mgr->append( PlayRecordingId, myRecordingToolBar );
  mgr->append( PauseRecordingId, myRecordingToolBar );
  mgr->append( StopRecordingId, myRecordingToolBar );
}

/*!
  Custom show event handler
*/
void SVTK_MainWindow::showEvent( QShowEvent * theEvent ) 
{
  emit Show( theEvent );
}

/*!
  Custom hide event handler
*/
void SVTK_MainWindow::hideEvent( QHideEvent * theEvent ) 
{
  emit Hide( theEvent );
}

/*!
  Starts zoom transformation
*/
void SVTK_MainWindow::activateZoom()
{
  myEventDispatcher->InvokeEvent(SVTK::StartZoom,0);
}

/*!
  Starts panning transformation
*/
void SVTK_MainWindow::activatePanning()
{
  myEventDispatcher->InvokeEvent(SVTK::StartPan,0);
}

/*!
  Starts rotation transformation
*/
void SVTK_MainWindow::activateRotation()
{
  myEventDispatcher->InvokeEvent(SVTK::StartRotate,0);
}

/*!
  Change rotation point
*/
void SVTK_MainWindow::onChangeRotationPoint(bool theIsActivate)
{
  if(theIsActivate){
    mySetRotationPointDlg->addObserver();
    if ( mySetRotationPointDlg->IsFirstShown() )
      activateSetRotationGravity();
    mySetRotationPointDlg->show();
  }else
    mySetRotationPointDlg->hide();
}

/*!
  Set the gravity center as a rotation point
*/
void SVTK_MainWindow::activateSetRotationGravity()
{
  myEventDispatcher->InvokeEvent(SVTK::SetRotateGravity,0);
}

/*!
  Set the selected point as a rotation point
*/
void SVTK_MainWindow::activateSetRotationSelected(void* theData)
{
  myEventDispatcher->InvokeEvent(SVTK::ChangeRotationPoint,theData);
}

/*!
  Set the point selected by user as a rotation point
*/
void SVTK_MainWindow::activateStartPointSelection()
{
  myEventDispatcher->InvokeEvent(SVTK::StartPointSelection,0);
}

/*!
  Set the view projection mode: orthogonal or perspective
*/
void SVTK_MainWindow::onProjectionMode(int mode)
{
  vtkCamera* aCamera = getRenderer()->GetActiveCamera();
  aCamera->SetParallelProjection(mode==0);
  GetInteractor()->GetDevice()->CreateTimer(VTKI_TIMER_FIRST);
}

/*!
  Modify view parameters
*/
void SVTK_MainWindow::onViewParameters(bool theIsActivate)
{
  if(theIsActivate){
    myViewParameterDlg->addObserver();
    myViewParameterDlg->show();
  }else
    myViewParameterDlg->hide();
}

/*!
  Set the gravity center as a focal point
*/
void SVTK_MainWindow::activateSetFocalPointGravity()
{
  myEventDispatcher->InvokeEvent(SVTK::SetFocalPointGravity, 0);
}

/*!
  Set the selected point as a focal point
*/
void SVTK_MainWindow::activateSetFocalPointSelected()
{
  myEventDispatcher->InvokeEvent(SVTK::SetFocalPointSelected, 0);
}

/*!
  Set the point selected by user as a focal point
*/
void
SVTK_MainWindow
::activateStartFocalPointSelection()
{
  myEventDispatcher->InvokeEvent(SVTK::StartFocalPointSelection,0);
}

void SVTK_MainWindow::activateProjectionMode(int mode)
{
  SVTK_ComboAction* a = ::qobject_cast<SVTK_ComboAction*>( toolMgr()->action( ProjectionModeId ) );
  if ( a ) a->setCurrentIndex(mode);
}

/*!
  Starts global panning transformation
*/
void SVTK_MainWindow::activateGlobalPanning()
{
  myEventDispatcher->InvokeEvent(SVTK::StartGlobalPan,0);
}

/*!
  Starts window fit transformation
*/
void SVTK_MainWindow::activateWindowFit()
{
  myEventDispatcher->InvokeEvent(SVTK::StartFitArea,0);
}

/*!
  Switches "keyboard free" interaction style on/off
*/
void SVTK_MainWindow::onSwitchInteractionStyle(bool theOn)
{
  if (theOn) {
    // check if style is already set
    if ( GetInteractorStyle() != myKeyFreeInteractorStyle.GetPointer() )
    {
      // keep the same style extensions
      SVTK_InteractorStyle* aStyle = (SVTK_InteractorStyle*)GetInteractorStyle();
      if ( aStyle ) {
	myKeyFreeInteractorStyle->SetControllerIncrement(aStyle->ControllerIncrement());
	myKeyFreeInteractorStyle->SetControllerOnKeyDown(aStyle->ControllerOnKeyDown());
      }

      PushInteractorStyle(myKeyFreeInteractorStyle.GetPointer());
    }
  }
  else {
    // pop only key free  style
    if ( GetInteractorStyle() == myKeyFreeInteractorStyle.GetPointer() )
      PopInteractorStyle();
  }

  // update action state if method is called outside
  QtxAction* a = action( SwitchInteractionStyleId );
  if ( a->isChecked() != theOn ) a->setChecked( theOn );
}

/*!
  Processes transformation "front view"
*/
void SVTK_MainWindow::onFrontView()
{
  GetRenderer()->OnFrontView();
  Repaint();
}

/*!
  Processes transformation "back view"
*/
void SVTK_MainWindow::onBackView()
{
  GetRenderer()->OnBackView();
  Repaint();
}

/*!
  Processes transformation "top view"
*/
void SVTK_MainWindow::onTopView()
{
  GetRenderer()->OnTopView();
  Repaint();
}

/*!
  Processes transformation "bottom view"
*/
void SVTK_MainWindow::onBottomView()
{
  GetRenderer()->OnBottomView();
  Repaint();
}

/*!
  Processes transformation "left view"
*/
void SVTK_MainWindow::onLeftView()
{
  GetRenderer()->OnLeftView();
  Repaint();
}

/*!
  Processes transformation "right view"
*/
void SVTK_MainWindow::onRightView()
{
  GetRenderer()->OnRightView();
  Repaint();
}

/*!
  Processes transformation "reset view": sets default orientation of viewport camera
*/
void SVTK_MainWindow::onResetView()
{
  GetRenderer()->OnResetView();
  Repaint();
}

/*!
  Processes transformation "fit all"
*/
void SVTK_MainWindow::onFitAll()
{
  GetRenderer()->OnFitAll();
  Repaint();
}

/*!
  Shows trihedron
*/
void SVTK_MainWindow::onViewTrihedron()
{
  GetRenderer()->OnViewTrihedron();
  Repaint();
}

/*!
  Shows cube axes
*/
void SVTK_MainWindow::onViewCubeAxes()
{
  GetRenderer()->OnViewCubeAxes();
  Repaint();
}

void SVTK_MainWindow::onUpdateRate(bool theIsActivate)
{
  if(theIsActivate){
    myUpdateRateDlg->Update();
    myUpdateRateDlg->show();
  }else
    myUpdateRateDlg->hide();
}

void SVTK_MainWindow::onNonIsometric(bool theIsActivate)
{
  if(theIsActivate){
    myNonIsometricDlg->Update();
    myNonIsometricDlg->show();
  }else
    myNonIsometricDlg->hide();
}

void SVTK_MainWindow::onGraduatedAxes(bool theIsActivate)
{
  if(theIsActivate){
    myCubeAxesDlg->Update();
    myCubeAxesDlg->show();
  }else
    myCubeAxesDlg->hide();
}

void SVTK_MainWindow::onAdjustTrihedron()
{   
  GetRenderer()->OnAdjustTrihedron();
}

void SVTK_MainWindow::onAdjustCubeAxes()
{   
  GetRenderer()->OnAdjustCubeAxes();
}

/*!
  \return QImage, containing all scene rendering in window
*/
QImage SVTK_MainWindow::dumpView()
{
  vtkRenderWindow* aWindow = GetInteractor()->getRenderWindow();
  int* aSize = aWindow->GetSize();
  int aWidth = aSize[0];
  int aHeight = aSize[1];
  
  unsigned char *aData = 
    aWindow->GetRGBACharPixelData( 0, 0, aWidth-1, aHeight-1, 0 );
  
  QImage anImage( aData, aWidth, aHeight, QImage::Format_ARGB32 );

  anImage = anImage.rgbSwapped();
  anImage = anImage.mirrored();
  return anImage;
}

/*!
  \return action by it's id
*/
QtxAction* SVTK_MainWindow::action( int id ) const
{
  return dynamic_cast<QtxAction*>( toolMgr()->action( id ) );
}

void SVTK_MainWindow::onStartRecording()
{
  myRecorder->CheckExistAVIMaker();
  if (myRecorder->ErrorStatus()) {
    SUIT_MessageBox::warning(this, tr("ERROR"), tr("MSG_NO_AVI_MAKER") );
  }
  else {
    SVTK_RecorderDlg* aRecorderDlg = new SVTK_RecorderDlg( this, myRecorder );

    if( !aRecorderDlg->exec() )
      return;

    myStartAction->setEnabled( false );
    myPlayAction->setEnabled( false );
    myPauseAction->setEnabled( true );
    myStopAction->setEnabled( true );

    // to prevent resizing the window while recording
    myPreRecordingMinSize = minimumSize();
    myPreRecordingMaxSize = maximumSize();
    setFixedSize( size() );

    myRecorder->Record();
  }
}

void SVTK_MainWindow::onPlayRecording()
{
  myStartAction->setEnabled( false );
  myPlayAction->setEnabled( false );
  myPauseAction->setEnabled( true );
  myStopAction->setEnabled( true );

  myRecorder->Pause();
}

void SVTK_MainWindow::onPauseRecording()
{
  myStartAction->setEnabled( false );
  myPlayAction->setEnabled( true );
  myPauseAction->setEnabled( false );
  myStopAction->setEnabled( true );

  myRecorder->Pause();
}

void SVTK_MainWindow::onStopRecording()
{
  myStartAction->setEnabled( true );
  myPlayAction->setEnabled( false );
  myPauseAction->setEnabled( false );
  myStopAction->setEnabled( false );

  myRecorder->Stop();

  setMinimumSize( myPreRecordingMinSize );
  setMaximumSize( myPreRecordingMaxSize );
}
