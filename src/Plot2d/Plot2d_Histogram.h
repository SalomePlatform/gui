// Name   : Plot2d_Histogram.h
// Purpose: Plot2d_Histogram is a class which defines histogram
//          plot item.
//
// History:
// 11/12/07 - Natalia Ermolaeva - Creation of the file

#ifndef PLOT2D_HISTOGRAM_H
#define PLOT2D_HISTOGRAM_H

#include "Plot2d.h"
#include "Plot2d_HistogramItem.h"
#include "Plot2d_Object.h"

class QString;
class QColor;
class QwtIntervalSeriesData;

class PLOT2D_EXPORT Plot2d_Histogram : public Plot2d_Object
{
public:
  Plot2d_Histogram();
  virtual ~Plot2d_Histogram();

  virtual int       rtti();
  virtual QwtPlotItem* createPlotItem();
  virtual void      autoFill( const QwtPlot* );
  virtual void      updatePlotItem( QwtPlotItem* );

  void              setData( const QList<double>&, const QList<double>& );
  QwtIntervalSeriesData   getData() const;

  QColor            getColor() const;
  void              setColor( const QColor& );
  double            getWidth( const bool& ) const;
  void              setWidth( const double );

  static double     getMinInterval( const QList<double>& );

protected:
  QColor            getNextColor( const QwtPlot* );
  bool              existColor( const QwtPlot*, const QColor& );

private:
  QColor            m_Color;
  double            m_dWidth;
  double            m_dDefWidth;
};


#if defined WIN32
#pragma warning( default: 4251 )
#endif

#endif
