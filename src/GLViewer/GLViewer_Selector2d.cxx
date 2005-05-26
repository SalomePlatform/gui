// File:      GLViewer_Selector2d.cxx
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/****************************************************************************
**  Class:   GLViewer_Selector2d 
**  Descr:   OpenGL Selector 2D
**  Module:  GLViewer
**  Created: UI team, 20.09.02
*****************************************************************************/

#include "GLViewer_Selector2d.h"
#include "GLViewer_Viewer2d.h"
#include "GLViewer_Context.h"
#include "GLViewer_ViewPort2d.h"

GLViewer_Selector2d::GLViewer_Selector2d( GLViewer_Viewer2d* v2d, GLViewer_Context* glc ) :
GLViewer_Selector( v2d ),
myGLContext( glc )
{   
//  myGLContext->SetHighlightColor( Quantity_NOC_CYAN1 );
//  myGLContext->SetSelectionColor( Quantity_NOC_RED );
}

GLViewer_Selector2d::~GLViewer_Selector2d()
{
}

void GLViewer_Selector2d::setHilightColor( Quantity_NameOfColor color )
{
  myGLContext->SetHighlightColor( color );
}

void GLViewer_Selector2d::setSelectColor( Quantity_NameOfColor color )
{
  myGLContext->SetSelectionColor( color );
}

void GLViewer_Selector2d::detect( int x, int y )
{
  //cout << "GLViewer_Selector2d    : detect ( " << x << " , " << y << " )" << endl;
  if ( myLocked || !myGLContext || !myViewer || !myViewer->getActiveView() || 
       myViewer->getSelectionMode() == GLViewer_Viewer::NoSelection )
    return;

  GLViewer_ViewPort* vp = myViewer->getActiveView()->getViewPort();
  if( !vp->inherits( "GLViewer_ViewPort2d" ) )
    return;

  myGLContext->MoveTo( x, y );
}

void GLViewer_Selector2d::select( bool append )
{
  //cout << "GLViewer_Selector2d    : select ( " << (int)append << " )" << endl;
  GLViewer_Viewer::SelectionMode selMode = myViewer->getSelectionMode();
  if ( myLocked || !myGLContext || !myViewer || !myViewer->getActiveView() ||
       selMode == GLViewer_Viewer::NoSelection ) 
    return;

  int selBefore = numSelected();
  if ( selBefore && append && selMode != GLViewer_Viewer::Multiple )
    return;    

  GLViewer_ViewPort* vp = myViewer->getActiveView()->getViewPort();
  if( !vp->inherits( "GLViewer_ViewPort2d" ) )
      return;
  int status = myGLContext->Select( append );
  checkSelection( selBefore, append, status );
}

void GLViewer_Selector2d::select( const QRect& selRect, bool append )
{
    GLViewer_Viewer::SelectionMode selMode = myViewer->getSelectionMode();
    if ( myLocked || !myGLContext || !myViewer || !myViewer->getActiveView() ||
       selMode == GLViewer_Viewer::NoSelection ) 
    return;

    int selBefore = numSelected();
    if ( selBefore && append && selMode != GLViewer_Viewer::Multiple )
        return;    

    GLViewer_ViewPort* vp = myViewer->getActiveView()->getViewPort();
    if( !vp->inherits( "GLViewer_ViewPort2d" ) )
        return;
    int aStatus = myGLContext->SelectByRect( selRect, append );
    checkSelection( selBefore, append, aStatus );
}

void GLViewer_Selector2d::unselectAll()
{
  if ( myLocked || !myViewer ) 
    return;

//  bool updateViewer = true;
  bool hadSelection = ( numSelected() > 0 );
     
//     bool lcOpen = ( myAISContext->IndexOfCurrentLocal() != -1 );
//     lcOpen ? myAISContext->ClearSelected( updateViewer ) :    
//              myAISContext->ClearCurrent( updateViewer );     
  if ( hadSelection ) emit selSelectionCancel();
}

/*  Checks selection state and emits  'selSelectionDone' or 'selSelectionCancel'     
    Should be called by after non-interactive selection. */
void GLViewer_Selector2d::checkSelection( int selBefore, bool append, int aStatus )
{
    int selAfter = numSelected();
    if ( selBefore > 0 && selAfter < 1 )     
        emit selSelectionCancel();
    else if ( selAfter > 0 )
    {
        switch( aStatus )
        {
        case SS_LocalChanged:
            emit selSelectionDone( selAfter > 1, SCS_Local );
            break;
        case SS_GlobalChanged:
            emit selSelectionDone( selAfter > 1, SCS_Global );
            break;
        }
    }
}

int GLViewer_Selector2d::numSelected() const
{
  return myGLContext->NbSelected();
}

