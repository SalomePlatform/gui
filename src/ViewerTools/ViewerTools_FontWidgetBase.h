// Copyright (C) 2007-2023  CEA, EDF, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#ifndef VIEWERTOOLS_FONTWIDGETBASE_H
#define VIEWERTOOLS_FONTWIDGETBASE_H

#include "ViewerTools.h"

#include <QWidget>

class QToolButton;
class QComboBox;
class QCheckBox;
class QColor;

/*!
 * Class       : ViewerTools_FontWidgetBase
 * Description : Dialog for specifying font
 */
class VIEWERTOOLS_EXPORT ViewerTools_FontWidgetBase : public QWidget
{
  Q_OBJECT

public:
                ViewerTools_FontWidgetBase( QWidget* );
  virtual       ~ViewerTools_FontWidgetBase();

  void          SetColor( const QColor& );
  QColor        GetColor() const;

  virtual void  SetData( const QColor&, const int, const bool, const bool, const bool );

  virtual void  GetData( QColor&, int&, bool&, bool&, bool& ) const;

public:
  virtual void  Initialize();

protected:
  virtual void  InitializeFamilies() = 0;

protected slots:
  void          onColor();

protected:
  QToolButton*  myColorBtn;
  QComboBox*    myFamily;
  QCheckBox*    myBold;
  QCheckBox*    myItalic;
  QCheckBox*    myShadow;
};

#endif
