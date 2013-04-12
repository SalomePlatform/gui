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
// File:      QtxTableInterface.h
// Author:    Natalia Ermolaeva

#ifndef QTXTABLEINTERFACE_H
#define QTXTABLEINTERFACE_H

#include "Qtx.h"

#include <QModelIndexList>

class QVariant;
class QItemSelectionModel;

#ifdef WIN32
#pragma warning( disable : 4251 )
#endif

class QTX_EXPORT QtxTableInterface
{
public:
  QtxTableInterface() {};
  virtual ~QtxTableInterface() {};

  /*!
   Returns a table row count
  */
  virtual int rowCount() const = 0;

  /**
   * Returns a table column count
   */
  virtual int columnCount() const = 0;

  /* Returns a table header value
   * @param theSection
   *   a header section
   * @param theOrientation
   *   a horizontal or vertical orientation
   * @param theRole
   *   a type of modification, that should be applyed to table
   */
  virtual QVariant headerData( const int theSection, const Qt::Orientation theOrientation,
                               const int theRole = Qt::DisplayRole ) const = 0;

  /* Sets a table header value
   * @param theSection
   *   a header section
   * @param theOrientation
   *   a horizontal or vertical orientation
   * @param theValue
   *   a value
   * @param theRole
   *   a type of modification, that should be applyed to table
   */
  virtual void setHeaderData( const int theSection,
                              const Qt::Orientation theOrientation,
                              const QVariant& theValue,
                              const int theRole = Qt::DisplayRole ) = 0;

  /*!
   * Returns a table cell value
   * @param theRow
   *   a row position
   * @param theColumn
   *   a column position
   * @param theRole
   *   a type of modification, that should be applyed to table
   */
  virtual QVariant data( const int theRow, const int theColumn,
                         const int theRole = Qt::DisplayRole ) const = 0;

  /*!
   * Set the value into a table cell
   * @param theRow
   *   a row position
   * @param theColumn
   *   a column position
   * @param theValue
   *   a value
   * @param theRole
   *   a type of modification, that should be applyed to table
   */
  virtual void setData( const int theRow, const int theColumn,
                        const QVariant& theValue,
                        const int theRole = Qt::DisplayRole ) = 0;

  /*!
   * Returns the table selection model
   */
  virtual QItemSelectionModel* selectionModel() const = 0;

  /**
   * Returns a list of selected indexes
   */
  virtual QModelIndexList getSelectedIndexes() const = 0;

};

#ifdef WIN32
#pragma warning( default: 4251 )
#endif

#endif // QTXTABLE_H
