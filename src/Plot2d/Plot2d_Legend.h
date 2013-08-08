
#ifndef PLOT2D_LEGEND_H
#define PLOT2D_LEGEND_H

#include "Plot2d.h"
#include <qwt_legend.h>
#include <qwt_plot.h>

class QAction;

class PLOT2D_EXPORT Plot2d_Legend : public QwtLegend
{
  Q_OBJECT

public:
  /** The legend position relative to the canvas */
  enum LegendPosition { Bottom = 0, Top = 1, Right = 2, Left = 3,
                        TopLeft = 4, TopRight = 5, BottomLeft = 6, BottomRight = 7 };

  Plot2d_Legend( QwtPlot* thePlot );
  virtual ~Plot2d_Legend();

  void init();

  LegendPosition getPositionType() const;
  void setPositionType( const LegendPosition& thePosition );

  bool isEnabled() const;
  void setEnabled( bool isEnable );

  void setOnCanvas( bool );

  virtual void setPlot( QwtPlot* thePlot );

  virtual bool eventFilter( QObject* theObj, QEvent* theEvent );

protected slots:
  void onPlotDestroyed();

protected:
  virtual void layoutContents();

  virtual bool pick( const QPoint& thePoint ) const;

  QWidget* parentToAttach() const;

  void insertLegend();
  void updateLegendPosition();
  QSize correctContentSize( const QSize& theSize ) const;

private:
  QwtPlot::LegendPosition qwtLegendPosition();

private:
  bool            myIsEnabled;
  bool            myIsOnCanvas;
  LegendPosition  myPosition;
  QwtPlot*        myPlot;
};

#endif
