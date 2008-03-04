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
// File:      Plot2d_ToolTip.cxx
// Author:    Alexandre SOLOVYOV

#include <Plot2d_ToolTip.h>
#include <Plot2d_ViewFrame.h>
#include <Plot2d_Curve.h>

#include <QFontMetrics>
#include <QEvent>
#include <QMouseEvent>

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>

const int maxDist = 3, tip_margin = 10;


Plot2d_ToolTip::Plot2d_ToolTip( Plot2d_ViewFrame* frame )
: QtxToolTip( frame->getPlotCanvas() ),
  myFrame( frame )
{
  connect( this, SIGNAL( maybeTip( QPoint, QString&, QFont&, QRect&, QRect& ) ),
	   this, SLOT( onToolTip( QPoint, QString&, QFont&, QRect&, QRect& ) ) );
}

Plot2d_ToolTip::~Plot2d_ToolTip()
{
}

void Plot2d_ToolTip::onToolTip( QPoint p, QString& str, QFont& f, QRect& txtRect, QRect& rect )
{
  int pInd;
  double dist;

  Plot2d_Curve* c = myFrame->getClosestCurve( p, dist, pInd );
  if( !c || dist>maxDist )
    return;

  str = c->text( pInd );
  if( str.isEmpty() )
    return;

  QFontMetrics m( f );
  QStringList lst = str.split( "\n", QString::SkipEmptyParts );
  QStringList::const_iterator anIt = lst.begin(), aLast = lst.end();
  int w = 0, h = 0;
  for( ; anIt!=aLast; anIt++ )
  {
    if( h>0 )
      h+= m.lineSpacing();

    QRect r = m.boundingRect( *anIt );
    if( r.width()>w )
      w = r.width();
    h+=r.height();
  }

  txtRect = QRect( p.x(), p.y(), w, h );
  rect = txtRect;
}

bool Plot2d_ToolTip::eventFilter( QObject* o, QEvent* e )
{
  bool res = QtxToolTip::eventFilter( o, e );
  if( e && e->type() == QEvent::MouseMove )
  {
    QMouseEvent* me = ( QMouseEvent* )e;
    if( me->modifiers()==0 )
      return true;
  }
  return res;
}
