// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
// Plot2d_ViewWindow.cxx: implementation of the Plot2d_ViewWindow class.

#include "Plot2d_ViewWindow.h"
#include "Plot2d_ViewFrame.h"

#include "SUIT_ViewManager.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_Session.h"
#include "SUIT_ToolButton.h"
#include "SUIT_Desktop.h"

#include "QtxAction.h"

#include <QStatusBar>
#include <QLayout>
#include <QApplication>
#include <QMenu>
#include <QImage>
#include <QToolBar>
#include <QPaintEvent>

/*!
  Constructor
*/
Plot2d_ViewWindow::Plot2d_ViewWindow(SUIT_Desktop* theDesktop, Plot2d_Viewer* theModel)
: SUIT_ViewWindow(theDesktop)
{
  myModel = theModel;
  myDumpImage = QImage();

  myViewFrame = new Plot2d_ViewFrame(this, "plotView");
  setCentralWidget(myViewFrame);

  myToolBar = addToolBar( tr("LBL_TOOLBAR_LABEL") );
  //myToolBar->setCloseMode(QDockWindow::Undocked); // toolbar has "Close" (X) button only if it's undocked, but there is no such functionality in Qt4
  createActions();
  createToolBar();

  connect(myViewFrame, SIGNAL(vpModeHorChanged()), this, SLOT(onChangeHorMode()));
  connect(myViewFrame, SIGNAL(vpModeVerChanged()), this, SLOT(onChangeVerMode()));
  connect(myViewFrame, SIGNAL(vpCurveChanged()),   this, SLOT(onChangeCurveMode()));
  connect(myViewFrame, SIGNAL(contextMenuRequested( QContextMenuEvent * )),
          this,        SIGNAL(contextMenuRequested( QContextMenuEvent * )) );

  myViewFrame->installEventFilter( this );
}

/*!
  Destructor
*/
Plot2d_ViewWindow::~Plot2d_ViewWindow()
{
}

/*!
  Puts message to status bar
  \param theMsg - message text
*/
void Plot2d_ViewWindow::putInfo(QString theMsg)
{
  QStatusBar*	aStatusBar = myDesktop->statusBar();
  aStatusBar->showMessage(theMsg/*, 3000*/);
}

/*!
  Fills popup menu with custom actions
 \param popup - popup menu to be filled with
*/
void Plot2d_ViewWindow::contextMenuPopup( QMenu* thePopup )
{
  // scaling
  QMenu* scalingPopup = new QMenu( thePopup );
  scalingPopup->addAction( myActionsMap[ PModeXLinearId ] );
  myActionsMap[ PModeXLinearId ]->addTo( scalingPopup );
  myActionsMap[ PModeXLogarithmicId ]->addTo( scalingPopup );
  onChangeHorMode();
  scalingPopup->addSeparator();
  myActionsMap[ PModeYLinearId ]->addTo( scalingPopup );
  myActionsMap[ PModeYLogarithmicId ]->addTo( scalingPopup );
  scalingPopup->setTitle( tr( "SCALING_POPUP" ) );
  thePopup->addMenu( scalingPopup );
  onChangeVerMode();

  thePopup->addAction(tr("TOT_PLOT2D_FITDATA"), myViewFrame, SLOT(onFitData()));
  // curve type
  QMenu* curTypePopup = new QMenu( thePopup );
  myActionsMap[ CurvPointsId ]->addTo( curTypePopup );
  myActionsMap[ CurvLinesId ]->addTo( curTypePopup );
  myActionsMap[ CurvSplinesId ]->addTo( curTypePopup );
  curTypePopup->setTitle( tr( "CURVE_TYPE_POPUP" ) );
  thePopup->addMenu( curTypePopup );

  // legend
  myActionsMap[ LegendId ]->addTo(thePopup);
  // settings
  myActionsMap[ CurvSettingsId ]->addTo(thePopup);
}

/*!
  Custom event filter
*/
bool Plot2d_ViewWindow::eventFilter(QObject* watched, QEvent* e)
{
  if (watched == myViewFrame) {
    int aType = e->type();
    switch(aType) {
    case QEvent::MouseButtonPress:
      emit mousePressed(this, (QMouseEvent*) e);
      return true;

    case QEvent::MouseButtonRelease:
      emit mouseReleased(this, (QMouseEvent*) e);
      return true;

    case QEvent::MouseMove:
      emit mouseMoving(this, (QMouseEvent*) e);
      return true;

    default:
      break;
    }
  }
  return SUIT_ViewWindow::eventFilter(watched, e);
}

/*!
  Create actions for Plot2d view window
*/
void Plot2d_ViewWindow::createActions()
{
  if ( !myActionsMap.isEmpty() )
    return;

  QtxAction* aAction;
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  // Dump view
  aAction = new QtxAction(tr("MNU_DUMP_VIEW"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_DUMP" ) ),
                           tr( "MNU_DUMP_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_DUMP_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onDumpView()));
  myActionsMap[ DumpId ] = aAction;

  // FitAll
  aAction = new QtxAction(tr("MNU_FITALL"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_FIT_ALL" ) ),
                           tr( "MNU_FITALL" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITALL"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onFitAll()));
  myActionsMap[ FitAllId ] = aAction;

  // FitRect
  aAction = new QtxAction(tr("MNU_FITRECT"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_FIT_AREA" ) ),
                           tr( "MNU_FITRECT" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITRECT"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onFitRect()));
  myActionsMap[ FitRectId ] = aAction;

  // Zoom
  aAction = new QtxAction(tr("MNU_ZOOM_VIEW"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_ZOOM" ) ),
                           tr( "MNU_ZOOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onZoom()));
  myActionsMap[ ZoomId ] = aAction;

  // Panning
  aAction = new QtxAction(tr("MNU_PAN_VIEW"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_PAN" ) ),
                           tr( "MNU_PAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onPanning()));
  myActionsMap[ PanId ] = aAction;

  // Global Panning
  aAction = new QtxAction(tr("MNU_GLOBALPAN_VIEW"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_GLOBALPAN" ) ),
                           tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onGlobalPanning()));
  myActionsMap[ GlobalPanId ] = aAction;

  // Curve type - points
  aAction = new QtxAction(tr("TOT_PLOT2D_CURVES_POINTS"),
                aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_CURVES_POINTS")),
                tr("MEN_PLOT2D_CURVES_POINTS"), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_CURVES_POINTS"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onCurves()));
  aAction->setCheckable(true);
  myActionsMap[ CurvPointsId ] = aAction;

  // Curve type - lines
  aAction = new QtxAction(tr("TOT_PLOT2D_CURVES_LINES"),
               aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_CURVES_LINES")),
               tr("MEN_PLOT2D_CURVES_LINES"), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_CURVES_LINES"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onCurves()));
  aAction->setCheckable(true);
  myActionsMap[ CurvLinesId ] = aAction;

  // Curve type - splines
  aAction = new QtxAction(tr("TOT_PLOT2D_CURVES_SPLINES"),
                 aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_CURVES_SPLINES")),
                 tr("MEN_PLOT2D_CURVES_SPLINES"), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_CURVES_SPLINES"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onCurves()));
  aAction->setCheckable(true);
  myActionsMap[ CurvSplinesId ] = aAction;

  // Mode for X (linear or logarithmic)
  aAction = new QtxAction(tr("TOT_PLOT2D_MODE_LINEAR_HOR"),
                 aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_MODE_LINEAR_HOR")),
                 tr("MEN_PLOT2D_MODE_LINEAR_HOR"), 0, this);
  aAction->setStatusTip (tr("PRP_PLOT2D_MODE_LINEAR_HOR"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewHorMode()));
  myActionsMap[ HorId ] = aAction;

  // Mode for Y (linear or logarithmic)
  aAction = new QtxAction(tr("TOT_PLOT2D_MODE_LINEAR_VER"),
                 aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_MODE_LINEAR_VER")),
                 tr("MEN_PLOT2D_MODE_LINEAR_VER" ), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_MODE_LINEAR_VER"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewVerMode()));
  myActionsMap[ VerId ] = aAction;

  // Legend
  aAction = new QtxAction(tr("TOT_PLOT2D_SHOW_LEGEND"),
                aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_SHOW_LEGEND")),
                tr("MEN_PLOT2D_SHOW_LEGEND"), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_SHOW_LEGEND"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onLegend()));
  aAction->setCheckable(true);
  myActionsMap[ LegendId ] = aAction;

  // Settings
  aAction = new QtxAction(tr( "TOT_PLOT2D_SETTINGS"),
                aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_SETTINGS")),
                tr("MEN_PLOT2D_SETTINGS"), 0, this);
  aAction->setStatusTip(tr( "PRP_PLOT2D_SETTINGS"));
  connect(aAction, SIGNAL(activated()), myViewFrame, SLOT(onSettings()));
  myActionsMap[ CurvSettingsId ] = aAction;

  // Clone
  aAction = new QtxAction(tr("MNU_CLONE_VIEW"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_CLONE_VIEW" ) ),
                           tr( "MNU_CLONE_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_CLONE_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SIGNAL(cloneView()));
  myActionsMap[ CloneId ] = aAction;

  /* Popup Actions */
  /* Linear/logarithmic mode */
  // Horizontal axis
  aAction = new QtxAction(tr("TOT_PLOT2D_MODE_LINEAR_HOR"),
                 aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_MODE_LINEAR_HOR")),
                 tr("MEN_PLOT2D_MODE_LINEAR_HOR"), 0, this);
  aAction->setStatusTip (tr("PRP_PLOT2D_MODE_LINEAR_HOR"));
  aAction->setCheckable(true);
  myActionsMap[PModeXLinearId] = aAction;
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewHorMode()));

  aAction = new QtxAction(tr("TOT_PLOT2D_MODE_LOGARITHMIC_HOR"),
              aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_MODE_LOGARITHMIC_HOR")),
              tr("MEN_PLOT2D_MODE_LOGARITHMIC_HOR"), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_MODE_LOGARITHMIC_HOR"));
  aAction->setCheckable(true);
  myActionsMap[PModeXLogarithmicId] = aAction;
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewHorMode()));

  // Vertical axis
  aAction = new QtxAction(tr("TOT_PLOT2D_MODE_LINEAR_VER"),
                 aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_MODE_LINEAR_VER")),
                 tr("MEN_PLOT2D_MODE_LINEAR_VER" ), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_MODE_LINEAR_VER"));
  aAction->setCheckable(true);
  myActionsMap[PModeYLinearId] = aAction;
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewVerMode()));

  aAction = new QtxAction(tr("TOT_PLOT2D_MODE_LOGARITHMIC_VER"),
                 aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_MODE_LOGARITHMIC_VER")),
                 tr("MEN_PLOT2D_MODE_LOGARITHMIC_VER" ), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_MODE_LOGARITHMIC_VER"));
  aAction->setCheckable(true);
  myActionsMap[PModeYLogarithmicId] = aAction;
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewVerMode()));
}

/*!
  Create toolbar for Plot2d view window
*/
void Plot2d_ViewWindow::createToolBar()
{
  myActionsMap[DumpId]->addTo(myToolBar);

  SUIT_ToolButton* aScaleBtn = new SUIT_ToolButton(myToolBar);
  aScaleBtn->AddAction(myActionsMap[FitAllId]);
  aScaleBtn->AddAction(myActionsMap[FitRectId]);
  aScaleBtn->AddAction(myActionsMap[ZoomId]);
  myToolBar->addWidget( aScaleBtn );

  SUIT_ToolButton* aPanBtn = new SUIT_ToolButton(myToolBar);
  aPanBtn->AddAction(myActionsMap[PanId]);
  aPanBtn->AddAction(myActionsMap[GlobalPanId]);
  myToolBar->addWidget( aPanBtn );

  myCurveBtn = new SUIT_ToolButton(myToolBar);
  myCurveBtn->AddAction(myActionsMap[CurvPointsId]);
  myCurveBtn->AddAction(myActionsMap[CurvLinesId]);
  myCurveBtn->AddAction(myActionsMap[CurvSplinesId]);
  myToolBar->addWidget( myCurveBtn );
  myActionsMap[CurvLinesId]->setChecked(true);
  onChangeCurveMode();

  myActionsMap[HorId]->addTo(myToolBar);
  onChangeHorMode();
  myActionsMap[VerId]->addTo(myToolBar);
  onChangeVerMode();

  myActionsMap[LegendId]->addTo(myToolBar);
  myActionsMap[CurvSettingsId]->addTo(myToolBar);
  myActionsMap[CloneId]->addTo(myToolBar);
  onChangeLegendMode();
}

/*!
  SLOT: called if scale mode for horizontal axis is changed
*/
void Plot2d_ViewWindow::onChangeHorMode()
{
  bool aLinear = myViewFrame->isModeHorLinear();
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  myActionsMap[PModeXLinearId]->setChecked( aLinear );
  myActionsMap[PModeXLogarithmicId]->setChecked( !aLinear );

  QPixmap pix = aResMgr->loadPixmap( "Plot2d", tr( aLinear ? "ICON_PLOT2D_MODE_LOGARITHMIC_HOR" :
                                                             "ICON_PLOT2D_MODE_LINEAR_HOR" ) );
  myActionsMap[HorId]->setIcon( pix );
  myActionsMap[HorId]->setToolTip( tr( aLinear ? "TOT_PLOT2D_MODE_LOGARITHMIC_HOR" :
                                                 "TOT_PLOT2D_MODE_LINEAR_HOR" ) );
  myActionsMap[HorId]->setStatusTip( tr( aLinear ? "PRP_PLOT2D_MODE_LOGARITHMIC_HOR" :
                                                   "PRP_PLOT2D_MODE_LINEAR_HOR" ) );

  myActionsMap[GlobalPanId]->setEnabled( myViewFrame->isModeVerLinear() && myViewFrame->isModeHorLinear() );
}

/*!
  SLOT: called if scale mode for vertical axis is changed
*/
void Plot2d_ViewWindow::onChangeVerMode()
{
  bool aLinear = myViewFrame->isModeVerLinear();
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  myActionsMap[PModeYLinearId]->setChecked( aLinear );
  myActionsMap[PModeYLogarithmicId]->setChecked( !aLinear );

  QPixmap pix = aResMgr->loadPixmap( "Plot2d", tr( aLinear ? "ICON_PLOT2D_MODE_LOGARITHMIC_VER" :
                                                             "ICON_PLOT2D_MODE_LINEAR_VER" ) );
  myActionsMap[VerId]->setIcon( pix );
  myActionsMap[VerId]->setToolTip( tr( aLinear ? "TOT_PLOT2D_MODE_LOGARITHMIC_VER" :
                                                 "TOT_PLOT2D_MODE_LINEAR_VER" ) );
  myActionsMap[VerId]->setStatusTip( tr( aLinear ? "PRP_PLOT2D_MODE_LOGARITHMIC_VER" :
                                                   "PRP_PLOT2D_MODE_LINEAR_VER" ) );

  myActionsMap[GlobalPanId]->setEnabled( myViewFrame->isModeVerLinear() && myViewFrame->isModeHorLinear() );
}

/*!
  SLOT: called if curve type is changed
*/
void Plot2d_ViewWindow::onChangeCurveMode()
{
  int aCurveType = myViewFrame->getCurveType();
  myCurveBtn->SetItem(aCurveType);

  myActionsMap[CurvPointsId]->setChecked(aCurveType == 0);
  myActionsMap[CurvLinesId]->setChecked(aCurveType == 1);
  myActionsMap[CurvSplinesId]->setChecked(aCurveType == 2);
}

/*!
  SLOT: called if legend mode is changed
*/
void Plot2d_ViewWindow::onChangeLegendMode()
{
  myActionsMap[ LegendId ]->setChecked(myViewFrame->isLegendShow());
}

/*!
  SLOT: called if action "Fit all" is activated
*/
void Plot2d_ViewWindow::onFitAll()
{
  myViewFrame->onViewFitAll();
}

/*!
  SLOT: called if action "Fit rect" is activated
*/
void Plot2d_ViewWindow::onFitRect()
{
  myViewFrame->onViewFitArea();
}

/*!
  SLOT: called if action "Zoom" is activated
*/
void Plot2d_ViewWindow::onZoom()
{
  myViewFrame->onViewZoom();
}

/*!
  SLOT: called if action "Panning" is activated
*/
void Plot2d_ViewWindow::onPanning()
{
  myViewFrame->onViewPan();
}

/*!
  SLOT: called if action "Global panning" is activated
*/
void Plot2d_ViewWindow::onGlobalPanning()
{
  myViewFrame->onViewGlobalPan();
}

/*!
  SLOT: called if action of scale mode for horizontal axis changing is activated
*/
void Plot2d_ViewWindow::onViewHorMode()
{
  if (myViewFrame->isModeHorLinear())
    myViewFrame->setHorScaleMode(1);
  else
    myViewFrame->setHorScaleMode(0);
}

/*!
  SLOT: called if action of scale mode for vertical axis changing is activated
*/
void Plot2d_ViewWindow::onViewVerMode()
{
  if (myViewFrame->isModeVerLinear())
    myViewFrame->setVerScaleMode(1);
  else
    myViewFrame->setVerScaleMode(0);
}

/*!
  SLOT: called if action "Show legend" is activated
*/
void Plot2d_ViewWindow::onLegend()
{
  myViewFrame->showLegend(!myViewFrame->isLegendShow());
  onChangeLegendMode();
}

/*!
  SLOT: called if action "Change curve type" is activated
*/
void Plot2d_ViewWindow::onCurves()
{
  QtxAction* aSender = (QtxAction*) sender();
  if(aSender == myActionsMap[CurvPointsId])
    myViewFrame->setCurveType(0);
  else if(aSender == myActionsMap[CurvLinesId])
    myViewFrame->setCurveType(1);
  else if(aSender == myActionsMap[CurvSplinesId])
    myViewFrame->setCurveType(2);
}
 
/*!
  SLOT: called if action "Dump view" is activated
*/
void Plot2d_ViewWindow::onDumpView()
{
  qApp->postEvent( myViewFrame, new QPaintEvent( QRect( 0, 0, myViewFrame->width(), myViewFrame->height() ) ) );
  SUIT_ViewWindow::onDumpView();
}

/*!
  \return QImage, containing all scene rendering in window
*/
QImage Plot2d_ViewWindow::dumpView()
{
  if ( getToolBar()->underMouse() || myDumpImage.isNull() )
    {
      QPixmap px = QPixmap::grabWindow( myViewFrame->winId() );
      return px.toImage();
    }
  
  return myDumpImage;
}

/*!
  Saves scene rendering in window to file
  \param fileName - name of file
  \param format - string contains name of format (for example, "BMP"(default) or "JPEG", "JPG")
*/
bool Plot2d_ViewWindow::dumpViewToFormat( const QImage& img, const QString& fileName, const QString& format )
{
  bool res = myViewFrame ? myViewFrame->print( fileName, format ) : false;
  if( !res )
    res = SUIT_ViewWindow::dumpViewToFormat( img, fileName, format );

  return res;
}

/*!
  \return filter of image files
*/
QString Plot2d_ViewWindow::filter() const
{
  return SUIT_ViewWindow::filter() + ";;" + tr( "POSTSCRIPT_FILES" );
}

/*!
  \return the visual parameters of this view as a formated string
 */
QString Plot2d_ViewWindow::getVisualParameters()
{
  return myViewFrame->getVisualParameters();
}

/*!
  The method restors visual parameters of this view from a formated string
*/
void Plot2d_ViewWindow::setVisualParameters( const QString& parameters )
{
  myViewFrame->setVisualParameters( parameters );
}

/*!
  \refresh QImage, containing all scene rendering in window
*/
void Plot2d_ViewWindow::RefreshDumpImage()
{
  QPixmap px = QPixmap::grabWindow( myViewFrame->winId() );
  myDumpImage = px.toImage();
}
