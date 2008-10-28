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
// File   : Plot2d_Object.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef PLOT2D_OBJECT_H
#define PLOT2D_OBJECT_H

#include "Plot2d.h"

#include <QList>
#include <qwt_plot.h>

struct PLOT2D_EXPORT Plot2d_Point
{
  double x;
  double y;
  QString text;
};

typedef QList<Plot2d_Point> pointList;

class PLOT2D_EXPORT Plot2d_Object
{
public:
  Plot2d_Object();
  Plot2d_Object( const Plot2d_Object& );

  virtual ~Plot2d_Object();
  Plot2d_Object& operator= ( const Plot2d_Object& );

  virtual int        rtti() = 0;
  virtual QwtPlotItem* createPlotItem() = 0;
  virtual void       autoFill( const QwtPlot* );
  virtual void       updatePlotItem( QwtPlotItem* );

  virtual QString    getTableTitle() const;
  
  void               setHorTitle( const QString& );
  QString            getHorTitle() const;
  void               setVerTitle( const QString& );
  QString            getVerTitle() const;

  void               setHorUnits( const QString& );
  QString            getHorUnits() const;
  void               setVerUnits( const QString& );
  QString            getVerUnits() const;

  void               setName( const QString& );
  QString            getName() const;

  void               addPoint( double, double, const QString& = QString() );
  void               insertPoint( int, double, double, const QString& = QString() );
  void               deletePoint( int );
  void               clearAllPoints();
  pointList          getPointList() const;
  void               setPointList( const pointList& points );

  void               setData( const double*, const double*, 
			      long, const QStringList& = QStringList() );
  double*            horData() const;
  double*            verData() const;

  void               setText( const int, const QString& );
  QString            text( const int ) const;

  int                nbPoints() const;
  bool               isEmpty() const;

  void               setAutoAssign( bool );
  bool               isAutoAssign() const;

  void               setYAxis( QwtPlot::Axis );
  QwtPlot::Axis      getYAxis() const;
  void               setXAxis( QwtPlot::Axis );
  QwtPlot::Axis      getXAxis() const;

  // Protection against QwtObject::drawLines() bug in Qwt 0.4.x: 
  // it crashes if switched to X/Y logarithmic mode, when one or more points have
  // non-positive X/Y coordinate
  double             getMinX() const;
  double             getMinY() const;

  static bool        closeColors( const QColor&, const QColor& );

protected:
  bool               myAutoAssign;
  QString            myHorTitle;
  QString            myVerTitle;
  QString            myHorUnits;
  QString            myVerUnits;
  QString            myName;
  QwtPlot::Axis      myYAxis;
  QwtPlot::Axis      myXAxis;

  pointList          myPoints;
};

typedef QList<Plot2d_Object*> objectList;

#endif