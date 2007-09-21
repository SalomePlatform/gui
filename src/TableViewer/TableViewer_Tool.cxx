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
// TableViewer_ViewModel.cxx: implementation of the TableViewer_ViewModel class.

#include "TableViewer_Tool.h"

#include <TCollection_ExtendedString.hxx>
#include <Quantity_Color.hxx>

#include <QString>
#include <QColor>

//================================================================
// Function : ToExtString
// Purpose  :
//================================================================

TCollection_ExtendedString TableViewer_Tool::ToExtString( const QString& src )
{
  if ( src.isEmpty() )
    return TCollection_ExtendedString();

  Standard_Integer len = src.length();
  Standard_ExtString extStr = new Standard_ExtCharacter [( len + 1 ) * 2];
  memcpy( extStr, src.unicode(), len * 2 );
  extStr[len] = 0;

  TCollection_ExtendedString trg( extStr );

  delete [] extStr;

  return trg;
}

//================================================================
// Function : ToDouble
// Purpose  : convert string to double
//================================================================
double TableViewer_Tool::ToDouble( const QString& txt )
{
  bool isDone = true;
  return TableViewer_Tool::ToDouble( txt, isDone );
}

//================================================================
// Function : ToDouble
// Purpose  : convert string to double
//================================================================
double TableViewer_Tool::ToDouble( const QString& txt, bool& isDone )
{ 
  return txt.toDouble( &isDone );
}

//================================================================
// Function : DoubleToQString
// Purpose  : convert string to double
//================================================================
QString TableViewer_Tool::DoubleToQString( const double val, const bool replaceDot )
{
  QString txt = QString::number( val );
  if ( replaceDot )
    txt.replace( '.', ',' );
  return txt;
}

//================================================================
// Function : DoubleToQString
// Purpose  : convert Qt color to OpenCascade color
//================================================================
Quantity_Color TableViewer_Tool::Color( const QColor& c )
{
  return Quantity_Color( c.red() / 255., c.green() / 255., c.blue() / 255., Quantity_TOC_RGB );
}

//================================================================
// Function : DoubleToQString
// Purpose  : convert OpenCascade color to Qt color
//================================================================
QColor TableViewer_Tool::Color( const Quantity_Color& c )
{
  return QColor( (int)( 255 * c.Red() ), (int)( 255 * c.Green() ), (int)( 255 * c.Blue() ) );
}
