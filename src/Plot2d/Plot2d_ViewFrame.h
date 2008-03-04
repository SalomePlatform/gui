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
#ifndef PLOT2D_VIEWFRAME_H
#define PLOT2D_VIEWFRAME_H

#include "Plot2d_Curve.h"
#include <QWidget>
#include <QMultiHash>
#include <QList>
#include <qwt_symbol.h>

class Plot2d_Plot2d;
class Plot2d_Prs;
class QCustomEvent;
class QwtPlotCurve;
class QwtPlotGrid;
class QwtPlotZoomer;

typedef QMultiHash<QwtPlotCurve*, Plot2d_Curve*> CurveDict; // obsolete
typedef QMultiHash<QwtPlotItem*, Plot2d_Object*> ObjectDict;

class PLOT2D_EXPORT Plot2d_ViewFrame : public QWidget
{ 
  Q_OBJECT

  enum { NoOpId, FitAreaId, ZoomId, PanId, GlPanId, DumpId, 
   ModeXLinearId, ModeXLogarithmicId, ModeYLinearId, ModeYLogarithmicId,
   LegendId, CurvePointsId, CurveLinesId, CurveSplinesId };
public:
  /* Construction/destruction */
  Plot2d_ViewFrame( QWidget* parent, const QString& title = "" );
  virtual ~Plot2d_ViewFrame();

  enum ObjectType { MainTitle, XTitle, YTitle, Y2Title, XAxis, YAxis, Y2Axis };

public:
  QWidget* getViewWidget();

  /* display */
  virtual void DisplayAll();
  virtual void EraseAll();
  void    Repaint();

  void    Display( const Plot2d_Prs* );
  void    Erase( const Plot2d_Prs*, const bool = false );
  Plot2d_Prs* CreatePrs( const char* entry = 0 );

  virtual bool eventFilter(QObject* watched, QEvent* e);

  /* operations */
  void    updateTitles();
  void    setTitle( const QString& title );
  QString getTitle() const { return myTitle; }

  // obsolete operation on curves
  void    displayCurve( Plot2d_Curve* curve, bool update = false ); // obsolete
  void    displayCurves( const curveList& curves, bool update = false ); // obsolete
  void    eraseCurve( Plot2d_Curve* curve, bool update = false ); // obsolete
  void    eraseCurves( const curveList& curves, bool update = false ); // obsolete
  int     getCurves( curveList& clist ); // obsolete
  CurveDict getCurves(); // osolete
  bool    isVisible( Plot2d_Curve* curve ); // obsolete
  void    updateCurve( Plot2d_Curve* curve, bool update = false ); // osolete

  // operations on objects
  void    displayObject( Plot2d_Object* curve, bool update = false );
  void    displayObjects( const objectList& objects, bool update = false );
  void    eraseObject( Plot2d_Object* object, bool update = false );
  void    eraseObjects( const objectList& objects, bool update = false );

  int     getObjects( objectList& clist );
  bool    isVisible( Plot2d_Object* curve );
  void    updateObject( Plot2d_Object* object, bool update = false );


  // operations on view frame
  void    updateLegend( const Plot2d_Prs* prs );
  void    fitAll();
  void    fitArea( const QRect& area );
  void    fitData(const int mode,
		  const double xMin, const double xMax,
		  const double yMin, const double yMax,
		  const double y2Min = 0, const double y2Max = 0);

  void    getFitRanges(double& xMin, double& xMax,
		       double& yMin, double& yMax,
		       double& y2Min, double& y2Max);

  /* view parameters */
  void    copyPreferences( Plot2d_ViewFrame* );
  void    setCurveType( int curveType, bool update = true );
  int     getCurveType() const { return myCurveType; }
  void    setCurveTitle( Plot2d_Curve* curve, const QString& title ); // obsolete
  void    setObjectTitle( Plot2d_Object* curve, const QString& title );
  void    showLegend( bool show, bool update = true );
  void    setLegendPos( int pos );
  int     getLegendPos() const { return myLegendPos; }
  void    setMarkerSize( const int size, bool update = true  );
  int     getMarkerSize() const { return myMarkerSize; }
  void    setBackgroundColor( const QColor& color );
  QColor  backgroundColor() const;
  void    setXGrid( bool xMajorEnabled, const int xMajorMax,
                    bool xMinorEnabled, const int xMinorMax, bool update = true );
  void    setYGrid( bool yMajorEnabled, const int yMajorMax,
                    bool yMinorEnabled, const int yMinorMax,
                    bool y2MajorEnabled, const int y2MajorMax,
                    bool y2MinorEnabled, const int y2MinorMax, bool update = true );
  void    setTitle( bool enabled, const QString& title, ObjectType type, bool update = true );
  QString getTitle( ObjectType type ) const;

  void    setFont( const QFont& font, ObjectType type, bool update = true );
  void    setHorScaleMode( const int mode, bool update = true );
  int     getHorScaleMode() const { return myXMode; }
  void    setVerScaleMode( const int mode, bool update = true );
  int     getVerScaleMode() const { return myYMode; }

  bool    isModeHorLinear();
  bool    isModeVerLinear();
  bool    isLegendShow() { return myShowLegend; };

  // Protection against QwtCurve::drawLines() bug in Qwt 0.4.x: 
  // it crashes if switched to X/Y logarithmic mode, when one or more points have
  // non-positive X/Y coordinate
  bool    isXLogEnabled() const;
  bool    isYLogEnabled() const;

  virtual bool print( const QString& file, const QString& format ) const;
  void     printPlot( QPainter* p, const QRect& rect,
                      const QwtPlotPrintFilter& = QwtPlotPrintFilter() ) const;

  QString getVisualParameters();
  void    setVisualParameters( const QString& parameters );

  void    incrementalPan ( const int incrX, const int incrY );
  void    incrementalZoom( const int incrX, const int incrY );

  QwtPlotCanvas* getPlotCanvas();
  Plot2d_Curve* getClosestCurve( QPoint p, double& distance, int& index );

protected:
  int     testOperation( const QMouseEvent& );
  void    readPreferences();
  void    writePreferences();
  QString getInfo( const QPoint& pnt );
  virtual void wheelEvent( QWheelEvent* );
  // obsolete methods on curves
  QwtPlotCurve* getPlotCurve( Plot2d_Curve* curve ); // obsolete
  bool    hasPlotCurve( Plot2d_Curve* curve ); // obsolete
  // methods on objects
  QwtPlotItem* getPlotObject( Plot2d_Object* object );
  bool    hasPlotObject( Plot2d_Object* object );

  void    setCurveType( QwtPlotCurve* curve, int curveType );

public slots:
  void    onViewPan(); 
  void    onViewZoom();
  void    onViewFitAll();
  void    onViewFitArea();
  void    onViewGlobalPan(); 
  void    onSettings();
  void    onFitData();
  void    onChangeBackground();

  void    onPanLeft();
  void    onPanRight();
  void    onPanUp();
  void    onPanDown();
  void    onZoomIn();
  void    onZoomOut();

protected:
  virtual void customEvent( QEvent* );
  void    plotMousePressed( const QMouseEvent& );
  bool    plotMouseMoved( const QMouseEvent& );
  void    plotMouseReleased( const QMouseEvent& );

signals:
  void    vpModeHorChanged();
  void    vpModeVerChanged();
  void    vpCurveChanged();
  void    contextMenuRequested( QContextMenuEvent *e );

protected:
  Plot2d_Plot2d* myPlot;
  int            myOperation;
  QPoint         myPnt;

  int            myCurveType;
  bool           myShowLegend;
  int            myLegendPos;
  int            myMarkerSize;
  QColor         myBackground;
  QString        myTitle, myXTitle, myYTitle, myY2Title;
  bool           myTitleEnabled, myXTitleEnabled, myYTitleEnabled, myY2TitleEnabled;
  bool           myXGridMajorEnabled, myYGridMajorEnabled, myY2GridMajorEnabled;
  bool           myXGridMinorEnabled, myYGridMinorEnabled, myY2GridMinorEnabled;
  int            myXGridMaxMajor, myYGridMaxMajor, myY2GridMaxMajor;
  int            myXGridMaxMinor, myYGridMaxMinor, myY2GridMaxMinor;
  int            myXMode, myYMode;
  double         myXDistance, myYDistance, myYDistance2;
  bool           mySecondY;
  ObjectDict     myObjects;
};

class Plot2d_Plot2d : public QwtPlot 
{
  Q_OBJECT
public:
  Plot2d_Plot2d( QWidget* parent );

  void       setLogScale( int axisId, bool log10 );

  void       replot();
  QwtLegend* getLegend() {
#if QWT_VERSION < 0x040200
     return d_legend;
#else  
     return legend(); /* mpv: porting to the Qwt 4.2.0 */
#endif
  }
  virtual QSize       sizeHint() const;
  virtual QSizePolicy sizePolicy() const;
  virtual QSize       minimumSizeHint() const;
  void                defaultPicker();
  void                setPickerMousePattern( int button, int state = Qt::NoButton );

  bool                polished() const { return myIsPolished; }
  QwtPlotGrid*        grid() { return myGrid; };

public slots:
  virtual void polish();

protected:
  QwtPlotGrid*       myGrid;
  QList<QColor>      myColors;
  bool               myIsPolished;
  QwtPlotZoomer*     myPlotZoomer;
};

#endif
