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

#ifndef OCCVIEWER_VIEWWINDOW_H
#define OCCVIEWER_VIEWWINDOW_H

#include "OCCViewer.h"
#include "SUIT_ViewWindow.h"
#include <gp_Pnt.hxx>
#include <V3d_Plane.hxx>

class QtxRectRubberBand;
class SUIT_Desktop;
class OCCViewer_ViewPort3d;
class OCCViewer_ViewSketcher;
class OCCViewer_ClippingDlg;
class OCCViewer_AxialScaleDlg;
class OCCViewer_SetRotationPointDlg;
class OCCViewer_Viewer;
struct viewAspect;
class QtxAction;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class OCCVIEWER_EXPORT OCCViewer_ViewWindow : public SUIT_ViewWindow  
{
  Q_OBJECT

public:
  enum { DumpId, FitAllId, FitRectId, ZoomId, PanId, GlobalPanId,
         ChangeRotationPointId, RotationId,
         FrontId, BackId, TopId, BottomId, LeftId, RightId, ResetId, CloneId, ClippingId, MemId, RestoreId,
         TrihedronShowId, AxialScaleId, AmbientId, SwitchInteractionStyleId, MaximizedId, UserId };

  enum OperationType{ NOTHING, PANVIEW, ZOOMVIEW, ROTATE, 
                      PANGLOBAL, WINDOWFIT, FITALLVIEW, RESETVIEW,
                      FRONTVIEW, BACKVIEW, TOPVIEW, BOTTOMVIEW, LEFTVIEW, RIGHTVIEW };

  enum RotationPointType{ GRAVITY, SELECTED };

  enum SketchingType { NoSketching, Rect, Polygon };

  enum Mode2dType { No2dMode, XYPlane, XZPlane, YZPlane};


  OCCViewer_ViewWindow(SUIT_Desktop* theDesktop, OCCViewer_Viewer* theModel);
  virtual ~OCCViewer_ViewWindow();

  virtual OCCViewer_ViewPort3d* getViewPort();

  virtual bool eventFilter(QObject* watched, QEvent* e);

  virtual void performRestoring( const viewAspect& );
  
  virtual void initLayout();

  virtual void updateEnabledDrawMode();

  virtual void setCuttingPlane( bool on, const double x = 0 , const double y = 0 , const double z = 0,
				const double dx = 0, const double dy = 0, const double dz = 1);

  virtual void setCuttingPlane( bool on, const gp_Pln thePln );

  virtual bool isCuttingPlane();

  virtual QString   getVisualParameters();
  virtual void      setVisualParameters( const QString& parameters );

  virtual void                    initSketchers();
  virtual OCCViewer_ViewSketcher* getSketcher( const int );

  virtual void                    activateSketching( int );

  virtual int                     interactionStyle() const;
  virtual void                    setInteractionStyle( const int );
 
  void setTransformEnabled( const OperationType, const bool );
  bool transformEnabled( const OperationType ) const;


  void            set2dMode( Mode2dType );
  Mode2dType      get2dMode() const { return my2dMode; }

  void            setMaximized( bool, bool = true );
  bool            isMaximized() const;

  virtual QColor  backgroundColor() const;
  virtual void    setBackgroundColor( const QColor& );


public slots:
  virtual void onFrontView();
  virtual void onViewFitAll();
  virtual void onBackView();
  virtual void onTopView();
  virtual void onBottomView();
  virtual void onLeftView();
  virtual void onRightView();
  virtual void onResetView();
  virtual void onFitAll();
  virtual void activateZoom();
  virtual void activateWindowFit();
  virtual void activateRotation();
  virtual void activatePanning();
  virtual void activateGlobalPanning();
  virtual void onSetRotationPoint( bool on );
  virtual void onCloneView();
  virtual void onClipping( bool on );
  virtual void onAxialScale();
  virtual void onAmbientToogle();
  virtual void onMemorizeView();
  virtual void onRestoreView();
  virtual void onTrihedronShow();
  virtual void setRestoreFlag();
  virtual void onSwitchInteractionStyle( bool on );

  virtual void activateSetRotationGravity();
  virtual void activateSetRotationSelected( double theX, double theY, double theZ );
  virtual void activateStartPointSelection();
  virtual void updateGravityCoords();
   
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );

  void onMaximizedView();


signals:
  void vpTransformationStarted(OCCViewer_ViewWindow::OperationType type);
  void vpTransformationFinished(OCCViewer_ViewWindow::OperationType type);
  void viewCloned( SUIT_ViewWindow* );

  void Show( QShowEvent * );
  void Hide( QHideEvent * );
  void maximized( OCCViewer_ViewWindow*, bool );

protected:
  virtual QImage dumpView();
  virtual bool   dumpViewToFormat( const QImage&, const QString& fileName, const QString& format );
  virtual QString  filter() const;

  /* Transformation selected but not started yet */
  bool transformRequested() const;
  bool setTransformRequested ( OperationType );

  /* Transformation is selected and already started */
  bool          transformInProcess() const;
  void          setTransformInProcess( bool );

  void vpMousePressEvent(QMouseEvent* theEvent);
  void vpMouseReleaseEvent(QMouseEvent* theEvent);
  void vpMouseMoveEvent(QMouseEvent* theEvent);

  void resetState();
  void drawRect();
  void endDrawRect();

  void createActions();
  void createToolBar();
 
  virtual OperationType getButtonState(QMouseEvent* theEvent, int theInteractionStyle);

  viewAspect getViewParams() const;

  bool computeGravityCenter( double& theX, double& theY, double& theZ );

  virtual void                          onSketchingStarted();
  virtual void                          onSketchingFinished();

  virtual OCCViewer_ViewSketcher*       createSketcher( int );

  OCCViewer_ViewSketcher*               mypSketcher;
  QList<OCCViewer_ViewSketcher*>        mySketchers;

  int                                   myCurSketch;

  OperationType         myOperation;
  OCCViewer_Viewer*     myModel;
  OCCViewer_ViewPort3d* myViewPort;

  RotationPointType     myCurrPointType;
  RotationPointType     myPrevPointType;
  gp_Pnt                mySelectedPoint;
  bool                  myRotationPointSelection;

  int                                   myRestoreFlag;

  int                                   myStartX;
  int                                   myStartY;
  int                                   myCurrX;
  int                                   myCurrY;

  bool                  myEventStarted;       // set when transformation is in process 
  bool                  myCursorIsHand;                 
  bool                  myDrawRect;           // set when a rect is used for selection or magnify 
  bool                  myEnableDrawMode;
  bool                  myPaintersRedrawing;  // set to draw with external painters 
 
  QCursor               myCursor;

  double myCurScale;

private:
  OCCViewer_ClippingDlg* myClippingDlg;
  QtxAction* myClippingAction;

  OCCViewer_AxialScaleDlg* myScalingDlg;

  OCCViewer_SetRotationPointDlg* mySetRotationPointDlg;
  QtxAction* mySetRotationPointAction;

  QtxRectRubberBand* myRectBand; //!< selection rectangle rubber band

  int myInteractionStyle;

  typedef QMap<OperationType, bool> MapOfTransformStatus;
  MapOfTransformStatus myStatus;

  Mode2dType my2dMode;

  Handle(V3d_Plane) myReserveClippingPlane;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
