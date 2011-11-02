// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef PLOT2D_VIEWFRAME_H
#define PLOT2D_VIEWFRAME_H

#include "Plot2d.h"
#include "Plot2d_Curve.h"
#ifndef DISABLE_PYCONSOLE
#include "Plot2d_AnaliticCurve.h"
#endif

#include <QWidget>
#include <QMultiHash>
#include <QList>
#include <qwt_symbol.h>
#include <qwt_scale_draw.h>

class Plot2d_Plot2d;
class Plot2d_Prs;
class Plot2d_Curve;
class Plot2d_Object;
class QCustomEvent;
class QwtPlotItem;
class QwtPlotCurve;
class QwtPlotGrid;
class QwtPlotZoomer;

typedef QMultiHash<QwtPlotCurve*, Plot2d_Curve*>  CurveDict;
typedef QMultiHash<QwtPlotItem*,  Plot2d_Object*> ObjectDict;

class PLOT2D_EXPORT Plot2d_ViewFrame : public QWidget
{ 
  Q_OBJECT
  
  enum { NoOpId, FitAreaId, ZoomId, PanId, GlPanId, DumpId, 
	 ModeXLinearId, ModeXLogarithmicId, ModeYLinearId, ModeYLogarithmicId,
	 LegendId, CurvePointsId, CurveLinesId, CurveSplinesId };
public:
  /* Construction/destruction */
  Plot2d_ViewFrame( QWidget*, const QString& = "" );
  virtual ~Plot2d_ViewFrame();

  enum ObjectType { MainTitle, XTitle, YTitle, Y2Title, XAxis, YAxis, Y2Axis };

  QWidget*       getViewWidget();

  /* display */
  virtual void   DisplayAll();
  virtual void   EraseAll();
  void           Repaint();

  void           Display( const Plot2d_Prs* );
  void           Erase( const Plot2d_Prs*, const bool = false );
  Plot2d_Prs*    CreatePrs( const char* = 0 );

  virtual bool   eventFilter( QObject*, QEvent* );

  /* operations */
  void           updateTitles();
  void           setTitle( const QString& );
  QString        getTitle() const;

  /* curves operations [ obsolete ] */
  void           displayCurve( Plot2d_Curve*, bool = false );
  void           displayCurves( const curveList&, bool = false );
  void           eraseCurve( Plot2d_Curve*, bool = false );
  void           eraseCurves( const curveList&, bool = false );
  int            getCurves( curveList& ) const;
  CurveDict      getCurves() const;
  void           updateCurve( Plot2d_Curve*, bool = false );

  /* objects operations */
  void           displayObject( Plot2d_Object*, bool = false );
  void           displayObjects( const objectList&, bool = false );
  void           eraseObject( Plot2d_Object*, bool = false );
  void           eraseObjects( const objectList&, bool = false );
  int            getObjects( objectList& ) const;
  bool           isVisible( Plot2d_Object* ) const;
  void           updateObject( Plot2d_Object*, bool = false );

  void           updateLegend( const Plot2d_Prs* );
  void           updateLegend();
  void           fitAll();
  void           fitArea( const QRect& );
  void           fitData( const int, const double, const double,
			  const double, const double,
			  const double = 0, const double = 0 );
  
  void           getFitRanges( double&, double&, double&, double&,
			       double&, double&);
  
  void           getFitRangeByCurves( double&, double&, double&, double&,
				      double&, double& );
#ifndef DISABLE_PYCONSOLE
  void           updateAnaliticCurves();
  void           updateAnaliticCurve( Plot2d_AnaliticCurve*, bool = false );
#endif

  /* view parameters */
  void           copyPreferences( Plot2d_ViewFrame* );
  void           setCurveType( int, bool = true );
  int            getCurveType() const;
  void           setCurveTitle( Plot2d_Curve*, const QString& );
  void           setObjectTitle( Plot2d_Object*, const QString& );
  void           showLegend( bool, bool = true );
  void           setLegendPos( int );
  int            getLegendPos() const;
  void           setLegendFont( const QFont& );
  QFont          getLegendFont() const;
  void           setLegendFontColor( const QColor& );
  QColor         getLegendFontColor() const;
  void           setMarkerSize( const int, bool = true  );
  int            getMarkerSize() const;
  void           setBackgroundColor( const QColor& );
  QColor         backgroundColor() const;
  void           setXGrid( bool, const int, bool, const int, bool = true );
  void           setYGrid( bool, const int, bool, const int,
			   bool, const int, bool, const int, bool = true );
  void           setTitle( bool, const QString&, ObjectType, bool = true );
  QString        getTitle( ObjectType ) const;

  void           setFont( const QFont&, ObjectType, bool = true );
  void           setHorScaleMode( const int, bool = true );
  int            getHorScaleMode() const;
  void           setVerScaleMode( const int, bool = true );
  int            getVerScaleMode() const;

  bool           isModeHorLinear();
  bool           isModeVerLinear();
  bool           isLegendShow() const;

  // Protection against QwtCurve::drawLines() bug in Qwt 0.4.x: 
  // it crashes if switched to X/Y logarithmic mode, when one or more points have
  // non-positive X/Y coordinate
  bool           isXLogEnabled() const;
  bool           isYLogEnabled() const;
  void           setEnableAxis( QwtPlot::Axis, bool );

  virtual bool   print( const QString&, const QString& ) const;
  void           printPlot( QPainter*, const QRect&,
			    const QwtPlotPrintFilter& = QwtPlotPrintFilter() ) const;

  QString        getVisualParameters();
  void           setVisualParameters( const QString& );

  void           incrementalPan ( const int, const int );
  void           incrementalZoom( const int, const int );

  QwtPlotCanvas* getPlotCanvas() const;
  Plot2d_Curve*  getClosestCurve( QPoint, double&, int& ) const;
  
  Plot2d_Object* getPlotObject( QwtPlotItem* ) const;

protected:
  int            testOperation( const QMouseEvent& );
  void           readPreferences();
  void           writePreferences();
  QString        getInfo( const QPoint& );
  virtual void   wheelEvent( QWheelEvent* );
  QwtPlotCurve*  getPlotCurve( Plot2d_Curve* ) const;
  bool           hasPlotCurve( Plot2d_Curve* ) const;
  void           setCurveType( QwtPlotCurve*, int );
  QwtPlotItem*   getPlotObject( Plot2d_Object* ) const;
  bool           hasPlotObject( Plot2d_Object* ) const;


  
  

public slots:
  void           onViewPan(); 
  void           onViewZoom();
  void           onViewFitAll();
  void           onViewFitArea();
  void           onViewGlobalPan(); 
  void           onSettings();
#ifndef DISABLE_PYCONSOLE
  void           onAnaliticCurve();
#endif
  void           onFitData();
  void           onChangeBackground();
  void           onPanLeft();
  void           onPanRight();
  void           onPanUp();
  void           onPanDown();
  void           onZoomIn();
  void           onZoomOut();

protected:
  virtual void   customEvent( QEvent* );
  void           plotMousePressed( const QMouseEvent& );
  bool           plotMouseMoved( const QMouseEvent& );
  void           plotMouseReleased( const QMouseEvent& );

signals:
  void           vpModeHorChanged();
  void           vpModeVerChanged();
  void           vpCurveChanged();
  void           contextMenuRequested( QContextMenuEvent* );
  void           legendClicked( QwtPlotItem* );

protected:
  Plot2d_Plot2d*      myPlot;
  int                 myOperation;
  QPoint              myPnt;

  int                 myCurveType;
  bool                myShowLegend;
  int                 myLegendPos;
  QFont               myLegendFont;
  QColor              myLegendColor;
  int                 myMarkerSize;
  QColor              myBackground;
  QString             myTitle, myXTitle, myYTitle, myY2Title;
  bool                myTitleEnabled, myXTitleEnabled, myYTitleEnabled, myY2TitleEnabled;
  bool                myXGridMajorEnabled, myYGridMajorEnabled, myY2GridMajorEnabled;
  bool                myXGridMinorEnabled, myYGridMinorEnabled, myY2GridMinorEnabled;
  int                 myXGridMaxMajor, myYGridMaxMajor, myY2GridMaxMajor;
  int                 myXGridMaxMinor, myYGridMaxMinor, myY2GridMaxMinor;
  int                 myXMode, myYMode;
  double              myXDistance, myYDistance, myYDistance2;
  bool                mySecondY;
  ObjectDict          myObjects;
#ifndef DISABLE_PYCONSOLE
  AnaliticCurveList   myAnaliticCurves;
#endif
  bool                myIsDefTitle;
};

class Plot2d_Plot2d : public QwtPlot 
{
  Q_OBJECT
public:
  Plot2d_Plot2d( QWidget* );
  virtual ~Plot2d_Plot2d();

  void           setLogScale( int, bool );

  void           replot();
  QwtLegend*     getLegend();
  QSize          sizeHint() const;
  QSize          minimumSizeHint() const;
  void           defaultPicker();
  void           setPickerMousePattern( int, int = Qt::NoButton );

  bool           polished() const;
  QwtPlotGrid*   grid() const;
  QwtPlotZoomer* zoomer() const;

  virtual void   updateYAxisIdentifiers();

public slots:
  virtual void   polish();

protected slots:
  void           onScaleDivChanged();

protected:
  QwtPlotGrid*   myGrid;
  QList<QColor>  myColors;
  bool           myIsPolished;
  QwtPlotZoomer* myPlotZoomer;
};

class Plot2d_ScaleDraw: public QwtScaleDraw
{
public:
  Plot2d_ScaleDraw( char f = 'g', int prec = 6 );
  Plot2d_ScaleDraw( const QwtScaleDraw& scaleDraw, char f = 'g', int prec = 6 );

  virtual QwtText label( double value ) const;

  int precision() const { return myPrecision; }

private:
  char myFormat;
  int  myPrecision;
};

#endif
