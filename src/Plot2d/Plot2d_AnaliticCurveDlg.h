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
// File   : Plot2d_AnaliticCurveDlg.h
// Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)


#ifndef PLOT2D_ANALITIC_CURVE_DLG_H
#define PLOT2D_ANALITIC_CURVE_DLG_H

#include "Plot2d.h"
#include "Plot2d_AnaliticCurve.h"

#include <QDialog>


class QListWidget;
class QListWidgetItem;
class QGroupBox;
class QLineEdit;
class QCheckBox;
class QComboBox;

class QtxIntSpinBox;
class QtxColorButton;
class QwtPlot;

class Plot2d_AnaliticCurve;
class Plot2d_ViewFrame;

class PLOT2D_EXPORT Plot2d_AnaliticCurveDlg : public QDialog {
  Q_OBJECT
public:

  Plot2d_AnaliticCurveDlg( QWidget* parent, QwtPlot* plot );
  ~Plot2d_AnaliticCurveDlg();

  AnaliticCurveList getCurveList();
  void              setCurveList( AnaliticCurveList& );

private:  
  void setCurrentCurve( QListWidgetItem* , bool = true);
  void checkState();
  void showErrorMsg();
  bool checkItem(const QListWidgetItem* );
  void storeProps( const QListWidgetItem* );
  void connectSelectionChanged();
  void disconnectSelectionChanged();
  void updateInView(QListWidgetItem*);

private slots:
  void onAddCurve();
  void onRemoveCurve();
  void onAutoAssign(int);
  void onUpdateCurve();
  void onCurveSelectionChanged( );

private:
  QListWidget*         myCurvesList;

  QGroupBox*           myCurveParams;
  QLineEdit*           myFormula;
  QtxIntSpinBox*       myNbIntervals;
 
  QGroupBox*           myCurveProps;
  QCheckBox*           myAutoAssign;
  QComboBox*           myMarkerType;
  QComboBox*           myLineType;
  QtxIntSpinBox*       myLineWidth;
  QtxColorButton*      myColor;

  QwtPlot*             myPlot;            
  QListWidgetItem*     myCurrentItem;
};

#endif //PLOT2D_ANALITIC_CURVE_DLG_H
