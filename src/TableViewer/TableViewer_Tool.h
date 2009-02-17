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
#if !defined(TABLEVIEWER_TOLL_H)
#define TABLEVIEWER_TOOL_H

#include "TableViewer.h"

class TCollection_ExtendedString;
class QString;
class Quantity_Color;
class QColor;

class TABLEVIEWER_EXPORT TableViewer_Tool
{
public:
  static TCollection_ExtendedString  ToExtString( const QString& );
  static double                      ToDouble( const QString& );
  static double                      ToDouble( const QString&, bool& );
  static QString                     DoubleToQString( const double, const int, const bool = false, const bool = false );

  static Quantity_Color                           Color( const QColor& );
  static QColor                                   Color( const Quantity_Color& );
};

#endif // !defined(TABLEVIEWER_TOOL_H)

