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

#include "SPV3D_ViewWindow.h"

#include "SPV3D_ViewModel.h"

#include <QMenu>
#include <QToolBar>
#include <QTimer>
#include <QEvent>
#include <QFileInfo>
#include <QSignalMapper>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>

#include <pqView.h>

#include "QtxAction.h"

#include "SUIT_Session.h"
#include "SUIT_MessageBox.h"
#include "SUIT_Accel.h"
#include "SUIT_Tools.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_Accel.h"
#include "SUIT_OverrideCursor.h"
#include "SUIT_ViewManager.h"
#include "QtxActionToolMgr.h"
#include "QtxMultiAction.h"
#include "QtxActionGroup.h"

#include <pqCameraReaction.h>
#include <pqParaViewBehaviors.h>
#include <pqRenderView.h>

#include "SALOME_ListIO.hxx"
#include "SPV3D_Prs.h"

#include <QToolBar>

// Use workaround for rendering transparent object over MESA
#define USE_WORKAROUND_FOR_MESA

/*!
  Constructor
*/
SPV3D_ViewWindow::SPV3D_ViewWindow(SUIT_Desktop* theDesktop, SPV3D_ViewModel* theModel):
  PV3DViewer_ViewWindow(theDesktop, theModel),
  myModel(theModel)
{
}

#include <pqRenderViewSelectionReaction.h>
#include <pqPipelineSource.h>
#include <vtkSMRenderViewProxy.h>
#include <vtkCollection.h>
#include <vtkSMRepresentationProxy.h>
#include <vtkSMPropertyHelper.h>
#include <vtkSMProxyManager.h>
#include <vtkRenderer.h>
#include <vtkSMSessionProxyManager.h>

#include "SPV3D_CADSelection.h"

void SPV3D_ViewWindow::init()
{
  pqView *view(myModel->getView());
  pqRenderView *renderView(qobject_cast<pqRenderView *>(view));
  QWidget *wid = view->widget();
  wid->setParent( this );
  setCentralWidget( wid );
  //
  this->
  myToolBar = toolMgr()->createToolBar( tr("LBL_TOOLBAR_LABEL"),                       // title (language-dependant)
                                        QString( "PV3DViewerViewOperations" ),          // name (language-independant)
                                        false );
  QAction* resetCenterAction =
    toolMgr()->toolBar(myToolBar)->addAction(QIcon(":/pqWidgets/Icons/pqResetCamera.svg"), tr( "MNU_FITALL" ) );
  new pqCameraReaction(resetCenterAction, pqCameraReaction::RESET_CAMERA);
  QAction *showCenterAction =
    toolMgr()->toolBar(myToolBar)->addAction(QIcon(":/pqWidgets/Icons/pqShowCenterAxes.svg"), tr( "DSC_SHOW_TRIHEDRON" ) );
  showCenterAction->setCheckable(true);
  QObject::connect(showCenterAction, &QAction::toggled, this, &SPV3D_ViewWindow::showCenterAxes);
  
  // Pick a new center of rotation on the surface of the mesh
  QAction* pickCenterAction = toolMgr()->toolBar(myToolBar)->addAction(QIcon(":/pqWidgets/Icons/pqPickCenter.svg"), tr( "MNU_CHANGINGROTATIONPOINT_VIEW" ) );
  pickCenterAction->setCheckable(true);
  pqRenderViewSelectionReaction* selectionReaction = new pqRenderViewSelectionReaction(
    pickCenterAction, renderView, pqRenderViewSelectionReaction::SELECT_CUSTOM_BOX);

  QObject::connect(selectionReaction,
    QOverload<int, int, int, int>::of(&pqRenderViewSelectionReaction::selectedCustomBox), this,
    &SPV3D_ViewWindow::pickCenterOfRotation);
  //
  mySelection = new SPV3D_CADSelection(this,renderView,SPV3D_CADSelection::SELECT_SOLIDS);
  QAction *selectionAction =  toolMgr()->toolBar(myToolBar)->addAction(SUIT_Session::session()->resourceMgr()->loadPixmap( "VTKViewer", tr( "ICON_SVTK_PRESELECTION_STANDARD" ) ), tr( "MNU_SVTK_PRESELECTION_STANDARD" ) );
  selectionAction->setCheckable(true);
  QObject::connect(selectionAction, &QAction::toggled, this, &SPV3D_ViewWindow::goSelect);
}

void SPV3D_ViewWindow::goSelect(bool val)
{
  if(val)
    {
      pqView* activeView(myModel->getView());
      if (!activeView)
      {
        return;
      }
      for(const auto& elt : myPrs)
      {
        pqPipelineSource *geometrySource = elt.second->GetSourceProducer();
        if(geometrySource)
        {
          vtkSMProxy* repr = activeView->getViewProxy()->FindRepresentation(
            geometrySource->getSourceProxy(), 0);
          repr->InvokeCommand("Reset");
        }
      }
      activeView->forceRender();
      activeView->render();
      activeView->resetDisplay();
    }
  mySelection->actionTriggered(val);
}

//-----------------------------------------------------------------------------
void SPV3D_ViewWindow::pickCenterOfRotation(int posx, int posy)
{
  pqView *view(myModel->getView());
  pqRenderView *renderView(qobject_cast<pqRenderView *>(view));
  if (!renderView)
    return;

  // Taken from vtkSMRenderViewProxy::ConvertDisplayToPointOnSurface
  // TODO : we can use this function directly if we make a simple correction in ParaView :
  // replace the vtkSMPVRepresentationProxy::SafeDownCast(...) by
  // vtkSMRepresentationProxy::SafeDownCast(...), like done bellow
  int region[4] = { posx, posy, posx, posy };
  vtkNew<vtkCollection> representations;
  vtkNew<vtkCollection> sources;

  vtkSMRenderViewProxy* renderViewProxy = renderView->getRenderViewProxy();
  renderViewProxy->SelectSurfaceCells(region, representations, sources, false);

  // If a point has been selected
  if (representations->GetNumberOfItems() > 0 && sources->GetNumberOfItems() > 0)
  {
    vtkSMRepresentationProxy* rep =
      vtkSMRepresentationProxy::SafeDownCast(representations->GetItemAsObject(0));
    vtkSMProxy* input = vtkSMPropertyHelper(rep, "Input").GetAsProxy(0);
    vtkSMSourceProxy* selection = vtkSMSourceProxy::SafeDownCast(sources->GetItemAsObject(0));

    // Setup a ray that starts at the center of the camera and ends
    // at the end of the scene
    double nearDisplayPoint[3] = { (double)posx, (double)posy, 0.0 };
    double farDisplayPoint[3] = { (double)posx, (double)posy, 1.0 };
    double farLinePoint[3] = { 0.0, 0.0, 0.0 };
    double nearLinePoint[3] = { 0.0, 0.0, 0.0 };

    vtkRenderer* renderer = renderViewProxy->GetRenderer();

    // Compute the near ray point
    renderer->SetDisplayPoint(nearDisplayPoint);
    renderer->DisplayToWorld();
    const double* world = renderer->GetWorldPoint();
    for (int i = 0; i < 3; i++)
    {
      nearLinePoint[i] = world[i] / world[3];
    }

    // Compute the far ray point
    renderer->SetDisplayPoint(farDisplayPoint);
    renderer->DisplayToWorld();
    world = renderer->GetWorldPoint();
    for (int i = 0; i < 3; i++)
    {
      farLinePoint[i] = world[i] / world[3];
    }

    // Compute the intersection of the ray with the mesh
    vtkSMProxyManager* proxyManager = vtkSMProxyManager::GetProxyManager();
    vtkSMSessionProxyManager* sessionProxyManager = proxyManager->GetActiveSessionProxyManager();
    vtkSMProxy* pickingHelper = sessionProxyManager->NewProxy("misc", "PickingHelper");

    vtkSMPropertyHelper(pickingHelper, "Input").Set(input);
    vtkSMPropertyHelper(pickingHelper, "Selection").Set(selection);
    vtkSMPropertyHelper(pickingHelper, "PointA").Set(nearLinePoint, 3);
    vtkSMPropertyHelper(pickingHelper, "PointB").Set(farLinePoint, 3);
    vtkSMPropertyHelper(pickingHelper, "SnapOnMeshPoint").Set(false);
    pickingHelper->UpdateVTKObjects();
    pickingHelper->UpdateProperty("Update", 1);

    double center[3] = { 0.0, 0.0, 0.0 };
    vtkSMPropertyHelper(pickingHelper, "Intersection").UpdateValueFromServer();
    vtkSMPropertyHelper(pickingHelper, "Intersection").Get(center, 3);
    pickingHelper->Delete();

    // Update the camera
    vtkSMRenderViewProxy* renderViewProxy = renderView->getRenderViewProxy();
    vtkSMPropertyHelper(renderViewProxy, "CenterOfRotation").Set(center, 3);
    vtkSMPropertyHelper(renderViewProxy, "CameraFocalPoint").Set(center, 3);
    renderViewProxy->UpdateVTKObjects();
    renderView->render();
  }
}

void SPV3D_ViewWindow::showCenterAxes(bool show_axes)
{
  if(!myModel)
    return;
  pqRenderView* renderView = qobject_cast<pqRenderView*>(myModel->getView());
  if (!renderView)
    return;
  renderView->setCenterAxesVisibility(show_axes);
  renderView->render();
}

SPV3D_Prs *SPV3D_ViewWindow::findOrCreatePrs( const char* entry )
{
  std::string entryCpp( entry );
  SPV3D_Prs *prsOut( new SPV3D_Prs( entry, this ) );
  for(auto& prs : myPrs)
  {
    if(entryCpp == prs.first)
    {
      prsOut->SetPVRenderInfo( prs.second.get() );
      return prsOut;
    }
  }
  std::unique_ptr<SPV3D_EXPORTSPV3DData> data(new SPV3D_EXPORTSPV3DData);
  prsOut->SetPVRenderInfo( data.get() );
  std::pair<std::string, std::unique_ptr<SPV3D_EXPORTSPV3DData> > p(entryCpp,std::move(data));
  myPrs.emplace_back( std::move(p) );
  return prsOut;
}

SPV3D_EXPORTSPV3DData *SPV3D_ViewWindow::isEntryAlreadyExist( const char* entry ) const
{
  std::string entryCpp( entry );
  for(const auto& prs : myPrs)
  {
    if(entryCpp == prs.first)
      return prs.second.get();
  }
  return nullptr;
}

/*!
  Destructor
*/
SPV3D_ViewWindow::~SPV3D_ViewWindow()
{
}

bool SPV3D_ViewWindow::isVisible(const Handle(SALOME_InteractiveObject)& theIObject)
{
  std::string entryCpp( theIObject->getEntry() );
  for(auto& prs : myPrs)
  {
    if(entryCpp == prs.first )
      return prs.second->IsVisible();
  }
  return false;
}

/*!
  Display object
  \param theIO - object
  \param theImmediatly - update viewer
*/
void SPV3D_ViewWindow::Display(const Handle(SALOME_InteractiveObject)& /*theIO*/,
                              bool /*theImmediatly*/) 
{
}

/*!
  Erase object
  \param theIO - object
  \param theImmediatly - update viewer
*/
void SPV3D_ViewWindow::Erase(const Handle(SALOME_InteractiveObject)& /*theIO*/,
                            bool /*theImmediatly*/)
{
}

/*!
  Display only passed object
  \param theIO - object
*/
void SPV3D_ViewWindow::DisplayOnly(const Handle(SALOME_InteractiveObject)& /*theIO*/)
{
}


/*!
  Display all objects in view
*/
void SPV3D_ViewWindow::DisplayAll() 
{
}

/*!
  Erase all objects in view
*/
void SPV3D_ViewWindow::EraseAll() 
{
  for(auto& prs : myPrs)
  {
    prs.second->Hide();
  }
  if(myModel)
    myModel->render();
}

/*!
  Updates current viewer
*/
void SPV3D_ViewWindow::Repaint(bool )//(bool theUpdateTrihedron)
{
  if(myModel)
    myModel->render();
}

/*!
  Enables/disables selection.
  \param theEnable if true - selection will be enabled
*/
void SPV3D_ViewWindow::SetSelectionEnabled( bool /*theEnable*/ )
{
}

/*!
  Emits key pressed
*/
void SPV3D_ViewWindow::onKeyPressed(QKeyEvent* event)
{
  emit keyPressed( this, event );
}

/*!
  Emits key released
*/
void SPV3D_ViewWindow::onKeyReleased(QKeyEvent* event)
{
  emit keyReleased( this, event );
}

/*!
  Emits mouse pressed
*/
void SPV3D_ViewWindow::onMousePressed(QMouseEvent* event)
{
  emit mousePressed(this, event);
}

/*!
  Emits mouse released
*/
void SPV3D_ViewWindow::onMouseReleased(QMouseEvent* event)
{
  emit mouseReleased( this, event );
}

/*!
  Emits mouse moving
*/
void SPV3D_ViewWindow::onMouseMoving(QMouseEvent* event)
{
  emit mouseMoving( this, event );
}

/*!
  Emits mouse double clicked
*/
void SPV3D_ViewWindow::onMouseDoubleClicked( QMouseEvent* event )
{
  emit mouseDoubleClicked( this, event );
}

/*!
  Custom show event handler
*/
void SPV3D_ViewWindow::showEvent( QShowEvent * theEvent ) 
{
  emit Show( theEvent );
}

/*!
  Custom hide event handler
*/
void SPV3D_ViewWindow::hideEvent( QHideEvent * theEvent ) 
{
  emit Hide( theEvent );
}
