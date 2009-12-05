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
// File:   QtxTreeModel.h
// Author: Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)
//

#ifndef QTXTREEMODEL_H
#define QTXTREEMODEL_H

#include <Qtx.h>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QModelIndex;

class QTX_EXPORT QtxTreeModel : public QAbstractItemModel
{
  Q_OBJECT
 
public:
  QtxTreeModel( QObject* = 0 );
  ~QtxTreeModel();

  virtual bool           customSorting( const int ) const;
  virtual bool           lessThan( const QModelIndex& left, const QModelIndex& right ) const;
};

class QTX_EXPORT QtxProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  QtxProxyModel( QObject* = 0 );
  ~QtxProxyModel();

  bool                   isSortingEnabled() const;

public slots:
  void                   setSortingEnabled( bool );

protected:
  virtual bool           lessThan( const QModelIndex&, const QModelIndex& ) const;

private:
  QtxTreeModel*          treeModel() const;

private:
  bool                   mySortingEnabled;
};

class QTX_EXPORT QtxMultiSortModel : public QtxProxyModel
{
  Q_OBJECT

public:
  QtxMultiSortModel( QObject* = 0 );
  ~QtxMultiSortModel();

  virtual void           multiSort( const QVector<int>&, const QVector<Qt::SortOrder>& );

protected:
  virtual bool           lessThan( const QModelIndex&, const QModelIndex& ) const;

private:
  QVector<int>             mySortColumns;
  QVector<Qt::SortOrder>   mySortOrders;
  Qt::SortOrder            myMajorOrder;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif // QTXTREEMODEL_H
