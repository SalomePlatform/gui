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

#ifndef OCCVIEWER_VIEWMODEL_H
#define OCCVIEWER_VIEWMODEL_H

#include <QColor>
#include <QPoint>
#include <QVector>

#include "OCCViewer.h"

#include "SUIT_ViewModel.h"

#include <V3d_View.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_InteractiveContext.hxx>

class QKeyEvent;
class QMouseEvent;

class SUIT_ViewWindow;
class SUIT_Desktop;
class OCCViewer_ViewWindow;

class AIS_ListOfInteractive;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class OCCVIEWER_EXPORT OCCViewer_Viewer: public SUIT_ViewModel
{
  Q_OBJECT

public:
  static QString Type() { return "OCCViewer"; }

  OCCViewer_Viewer( bool DisplayTrihedron = true, bool DisplayStaticTrihedron = true );
  virtual ~OCCViewer_Viewer();

  void update();

  virtual SUIT_ViewWindow*        createView(SUIT_Desktop* theDesktop);
  
  virtual void                    setViewManager(SUIT_ViewManager* theViewManager);
  virtual QString                 getType() const { return Type(); }

  virtual void                    contextMenuPopup(QMenu*);
  
  void                            getSelectedObjects(AIS_ListOfInteractive& theList);
  void                            setObjectsSelected(const AIS_ListOfInteractive& theList);
  void                            setSelected(const Handle(AIS_InteractiveObject)& theIO) { myAISContext->SetSelected(theIO);}

  void                            performSelectionChanged();
  // emit signal selectionChanged

  QColor                          backgroundColor() const;
  void                            setBackgroundColor( const QColor& );

  QColor                          backgroundColor(int theViewId) const;
  void                            setBackgroundColor( int theViewId, const QColor& );

  //! returns true if 3d Trihedron in viewer was created
  bool                            trihedronActivated() const { return !myTrihedron.IsNull(); }

  void                            toggleTrihedron();
  bool                            isTrihedronVisible() const;
  virtual void                    setTrihedronShown( const bool );

  double                          trihedronSize() const;
  virtual void                    setTrihedronSize( const double );

  virtual OCCViewer_ViewWindow*   createSubWindow();

public:
  Handle(V3d_Viewer)              getViewer3d()    const { return myV3dViewer;}
  Handle(V3d_Viewer)              getCollector3d() const { return myV3dCollector; }
  Handle(AIS_InteractiveContext)  getAISContext()  const { return myAISContext; }
  Handle(AIS_Trihedron)           getTrihedron()   const { return myTrihedron; }

  int                             interactionStyle() const;
  void                            setInteractionStyle( const int );

  int                             zoomingStyle() const;
  void                            setZoomingStyle( const int );

  void                            enableSelection(bool isEnabled);
  bool                            isSelectionEnabled() const { return mySelectionEnabled; }

  void                            enableMultiselection(bool isEnable);
  bool                            isMultiSelectionEnabled() const { return myMultiSelectionEnabled; }

  int                             getSelectionCount() const { return (!myAISContext.IsNull())? myAISContext->NbSelected():0; }
  bool                            isStaticTrihedronDisplayed() { return myShowStaticTrihedron; }

  /* Selection management */
  bool    highlight( const Handle(AIS_InteractiveObject)&, bool, bool=true );
  bool    unHighlightAll( bool=true ); 
  bool    isInViewer( const Handle(AIS_InteractiveObject)&, bool=false );
  bool    isVisible( const Handle(AIS_InteractiveObject)& );

  void    setColor( const Handle(AIS_InteractiveObject)&, const QColor&, bool=true );
  void    switchRepresentation( const Handle(AIS_InteractiveObject)&, int, bool=true );
  void    setTransparency( const Handle(AIS_InteractiveObject)&, float, bool=true );
  void    setIsos( const int u, const int v ); // number of isolines
  void    isos( int& u, int& v ) const;

  void    initView( OCCViewer_ViewWindow* view );

signals:
  void selectionChanged();
  void deselection();

protected:

protected slots:
  void onMousePress(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseMove(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseRelease(SUIT_ViewWindow*, QMouseEvent*);
  void onKeyPress(SUIT_ViewWindow*, QKeyEvent*);

  void onDumpView();
  void onChangeBgColor();

private:
  Handle(V3d_Viewer)              myV3dViewer;
  Handle(V3d_Viewer)              myV3dCollector;

  Handle(AIS_Trihedron)           myTrihedron;
  Handle(AIS_InteractiveContext)  myAISContext;

  int                             myInteractionStyle;
  int                             myZoomingStyle;

  bool                            mySelectionEnabled;
  bool                            myMultiSelectionEnabled;

  //QColor                          myBgColor;
  QPoint                          myStartPnt, myEndPnt;

  bool                            myShowStaticTrihedron;

  QVector<QColor>                 myColors;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
