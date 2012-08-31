// Name   : Plot2d_HistogramItem.h
// Purpose: Plot2d_HistogramItem is a class of building histogram
//          object. Appends legend items with symbol.
//          Appends crossed bars.
//
// History:
// 11/12/07 - Natalia Ermolaeva - Creation of the file

#ifndef PLOT2D_HISTOGRAM_ITEM_H
#define PLOT2D_HISTOGRAM_ITEM_H

#include <Plot2d.h>

#include <qglobal.h>
#include <qcolor.h>
#include "qwt_plot_item.h" 
#include "qwt_series_data.h"
#include "qwt_compat.h"
#include "QColor"

#include <QMap>

class QwtIntervalSeriesData;
class QwtLegend;
class QString;

class PLOT2D_EXPORT Plot2d_HistogramQwtItem: public QwtPlotItem
{
protected:
  class PrivateData {
    public:
      int             attributes;
      QVector<QwtIntervalSample> data;
      QColor          color;
      double          reference;
  };
public:
  explicit Plot2d_HistogramQwtItem( const QString& = QString::null );
  explicit Plot2d_HistogramQwtItem( const QwtText& );
  virtual ~Plot2d_HistogramQwtItem();

  void         setData( const QwtIntervalSeriesData& );
  const QwtIntervalSeriesData& data() const;

  void         setColor( const QColor & );
  QColor       color() const;

  virtual QwtDoubleRect boundingRect() const;
  virtual int  rtti() const;
  virtual void draw( QPainter *, const QwtScaleMap &xMap, 
                     const QwtScaleMap &yMap, const QRectF & ) const;

  void         setBaseline( double );
  double       baseline() const;

  enum HistogramAttribute {
    Auto = 0,
    Xfy = 1
  };

  void         setHistogramAttribute( HistogramAttribute, bool = true );
  bool         testHistogramAttribute( HistogramAttribute ) const;

protected:
  virtual void drawBar(QPainter* , Qt::Orientation, const QRect& ) const;

protected:
  void init();

  PrivateData* m_pData;
};

class PLOT2D_EXPORT Plot2d_HistogramItem : public Plot2d_HistogramQwtItem
{
public:
  explicit Plot2d_HistogramItem( const QString& = QString::null );
  explicit Plot2d_HistogramItem( const QwtText& );
  virtual ~Plot2d_HistogramItem();

  QList<QRect> getBars() const { return m_BarItems; };

  virtual void updateLegend( QwtLegend* ) const;
  virtual void draw( QPainter *, const QwtScaleMap &xMap, 
                     const QwtScaleMap &yMap, const QRect & ) const;
  void         drawBarRect( QPainter*, const QRect& ) const;

  void         setCrossItems( const bool& theCross ) { m_bCrossed = theCross; }
  bool         isCrossItems() const { return m_bCrossed; }

protected:
  void         drawRectAndLowers( QPainter* , Qt::Orientation, const QRect& ) const;
  int          getCrossedTop( const QRect& ) const;

protected:
  QList<QRect> m_BarItems;
  bool         m_bCrossed;
};

#endif
