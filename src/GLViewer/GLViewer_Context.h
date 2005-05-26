// File:      GLViewer_Context.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/***************************************************************************
**  Class:   GLViewer_Context
**  Descr:   OpenGL Context
**  Module:  GLViewer
**  Created: UI team, 04.09.02
****************************************************************************/
#ifndef GLVIEWER_CONTEXT_H
#define GLVIEWER_CONTEXT_H

#ifdef WNT
#include "windows.h"
#endif

//#include "QAD.h"
#include "GLViewer_Object.h"

#include <qmap.h>
#include <qvaluelist.h>
#include <qrect.h>
#include <qobject.h>
#include <qregion.h>

#include <Quantity_NameOfColor.hxx>
#include <Quantity_Color.hxx>

#include <GL/gl.h>

typedef QMap<GLViewer_Object*,int> ObjectMap;
typedef QValueList<GLViewer_Object*> ObjList;

class GLViewer_Viewer2d;

#ifdef WNT
#pragma warning( disable:4251 )
#endif

enum SelectionStatus
{
    SS_Invalid,
    SS_LocalChanged,
    SS_GlobalChanged,
    SS_NoChanged
};

class GLVIEWER_EXPORT GLViewer_Context : public QObject
{
  Q_OBJECT

public:
  GLViewer_Context( GLViewer_Viewer2d* );
  ~GLViewer_Context();

  int                   MoveTo( int x, int y, bool byCircle = FALSE );
  int                   Select( bool Append = FALSE, bool byCircle = FALSE );
  int                   SelectByRect( const QRect&, bool Append = FALSE );

  void                  SetHighlightColor( Quantity_NameOfColor aCol );
  void                  SetSelectionColor( Quantity_NameOfColor aCol );
  Quantity_NameOfColor  HighlightColor() { return myHighlightColor; }
  Quantity_NameOfColor  SelectionColor() { return mySelectionColor; } 
  int                   NbSelected();
  void                  InitSelected();
  bool                  MoreSelected();
  bool                  NextSelected();
  GLViewer_Object*      SelectedObject();

  bool                  isSelected( GLViewer_Object* );

  int                   insertObject( GLViewer_Object*, bool display = FALSE );
  bool                  replaceObject( GLViewer_Object*, GLViewer_Object* );
  void                  updateScales( GLfloat, GLfloat );
  void                  setTolerance( int tol ) { myTolerance = tol; }
  const ObjectMap&      getObjects() { return myObjects; }
  const ObjList&        getObjList() { return myObjList; }
  GLViewer_Object*      getFirstObject() { return myObjects.begin().key(); }

  void                  clearSelected( bool updateViewer );
  void                  setSelected( GLViewer_Object*, bool updateViewer );
  void                  remSelected( GLViewer_Object*, bool updateViewer );

  GLViewer_Object*      getCurrentObject() { return myLastPicked; }
  bool                  currentObjectIsChanged() { return isLastPickedChanged; }

  void                  eraseObject( GLViewer_Object*, bool updateViewer = true );
  void                  deleteObject( GLViewer_Object*, bool updateViewer = true );

protected:
  GLViewer_Viewer2d*    myGLViewer2d;
  GLViewer_Object*      myLastPicked;
  bool                  isLastPickedChanged;
  ObjectMap             myObjects;
  ObjList               myObjList;
  int                   myNumber;
  QValueList<int>       mySelNumbers;
  int                   mySelCurIndex;
  int                   myHNumber;
  GLfloat               myXhigh;
  GLfloat               myYhigh;
  Quantity_NameOfColor  myHighlightColor;
  Quantity_NameOfColor  mySelectionColor;
  GLboolean             myHFlag;
  GLboolean             mySFlag;
  int                   myTolerance;
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
