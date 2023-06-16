// Copyright (C) 2007-2023  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  File   : CAF_Tools.cxx
//  Author : UI team
//
#include "CAF_Tools.h"

/*!
  \class CAF_Tools
  \brief Prodives a set of helpful static methods.
*/

/*!
  \brief Convert TCollection_ExtendedString \a src to QString.
  \param src string to be converted
  \return resulting QString object
*/
QString CAF_Tools::toQString ( const TCollection_ExtendedString& src )
{
  return QString( (const QChar*)src.ToExtString(), src.Length() );
}

/*!
  \brief Convert TCollection_AsciiString \a src to QString.
  \param src string to be converted
  \return resulting QString object
*/
QString CAF_Tools::toQString( const TCollection_AsciiString& src )
{
  return QString( src.ToCString() );
}

/*!
  \brief Convert QString \a src to TCollection_AsciiString.
  \param src string to be converted
  \return resulting TCollection_AsciiString object
*/
TCollection_AsciiString CAF_Tools::toAsciiString( const QString& src )
{
  TCollection_AsciiString res;
  if ( !src.isEmpty() )
    res = TCollection_AsciiString( src.toLatin1().data() );
  return res;
}

/*!
  \brief Convert QString \a src to TCollection_ExtendedString.
  \param src string to be converted
  \return resulting TCollection_ExtendedString object
*/
TCollection_ExtendedString CAF_Tools::toExtString ( const QString& src )
{
  TCollection_ExtendedString result;
  for ( int i = 0; i < (int)src.length(); i++ )
    result.Insert( i + 1, src[ i ].unicode() );
  return result;
}

/*!
  \brief Convert QColor object to Quantity_Color object.
  \param c color object in Qt format
  \return color object in OCC format
*/
Quantity_Color CAF_Tools::color( const QColor& c )
{
  Quantity_Color aColor;
  if ( c.isValid() )
    aColor = Quantity_Color( c.red()   / 255., c.green() / 255.,
                             c.blue()  / 255., Quantity_TOC_RGB );
  return aColor;
}

/*!
  \brief Convert Quantity_Color object to QColor object.
  \param c color object in OCC format
  \return color object in Qt format
*/
QColor CAF_Tools::color( const Quantity_Color& c )
{
  return QColor ( int( c.Red()   * 255 ),
                  int( c.Green() * 255 ),
                  int( c.Blue()  * 255 ) );
}
