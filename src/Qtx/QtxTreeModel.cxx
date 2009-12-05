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
// File:   QtxTreeModel.cxx
// Author: Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)
//

#include "QtxTreeModel.h"


/*!
  \class QtxTreeModel
  \brief Base tree model class with custom sorting support.
  \sa QtxProxyModel
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
QtxTreeModel::QtxTreeModel( QObject* parent )
: QAbstractItemModel( parent )
{
}

/*!
  \brief Destructor
*/
QtxTreeModel::~QtxTreeModel()
{
}

/*!
  \brief Check if the specified column supports custom sorting.
  \param column column index on which data is being sorted
  \return \c true if column requires custom sorting
  \sa lessThan()
*/
bool QtxTreeModel::customSorting( const int /*column*/ ) const
{
  return false;
}

/*!
  \brief Compares two model indexes for the sorting purposes.

  This method is called only for those columns for which customSorting()
  method returns \c true. The default implementation is useless, derived classes
  should re-implement this method in order to use custom sorting rules.

  \param left first index to compare
  \param right second index to compare
  \return result of the comparison
  \sa customSorting()
*/
bool QtxTreeModel::lessThan( const QModelIndex& left, const QModelIndex& right ) const
{
  return left.row() < right.row();
}


/*!
  \class QtxProxyModel
  \brief Proxy model which can be used above the QtxTreeMovel class
  to enable custom sorting/filtering of the data.
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
QtxProxyModel::QtxProxyModel( QObject* parent )
: QSortFilterProxyModel( parent ),
  mySortingEnabled( true )
{
}

/*!
  \brief Destructor.
*/
QtxProxyModel::~QtxProxyModel()
{
}

/*!
  \brief Check if sorting is enabled.
  \return \c true if sorting is enabled
  \sa setSortingEnabled()
*/
bool QtxProxyModel::isSortingEnabled() const
{
  return mySortingEnabled;
}

/*!
  \brief Enable/disable sorting.
  \param enabled new flag state
  \sa isSortingEnabled()
*/
void QtxProxyModel::setSortingEnabled( bool enabled )
{
  mySortingEnabled = enabled;
  clear();
}

/*!
  \brief Compares two model indexes for the sorting purposes.
  \param left first index to compare
  \param right second index to compare
  \return True if item for left is less than item for right
*/
bool QtxProxyModel::lessThan( const QModelIndex& left, const QModelIndex& right ) const
{
  if ( !isSortingEnabled() && left.isValid() && right.isValid() ) {
    return left.row() < right.row();
  }
  if ( treeModel() && treeModel()->customSorting( left.column() ) ) {
    return treeModel()->lessThan( left, right );
  }
  return QSortFilterProxyModel::lessThan( left, right );
}

/*
  \brief Get tree model.
  \return tree model
*/
QtxTreeModel* QtxProxyModel::treeModel() const
{
  return dynamic_cast<QtxTreeModel*>( sourceModel() );
}

/*!
  \class QtxMultiSortModel
  \brief Proxy model for multi-column sorting.

  This class can be used when it is necessary to sort the source model
  by several columns. It provides overloaded sort( columns, order ) method and re-implements
  lessThan() method to consider several items from the same row.
  Note that multi-column sorting is less efficient with the current Qt version (4.4.3)
  than single-column sorting, as it requires significantly more comparisons, so use it with care!
*/

/*!
   \brief Constructor.
   \param parent parent object
 */
QtxMultiSortModel::QtxMultiSortModel( QObject* parent )
: QtxProxyModel( parent )
{
}

/*!
   \brief Destructor.
 */
QtxMultiSortModel::~QtxMultiSortModel()
{
}

/*!
   \brief Performs multi-column sorting.

   This is a front-end for multi-column sorting. Indices of columns should be ordered with descending priority
   (that is, the first element in the array has maximum priority during sorting, 
   and the last one has the minimum priority). 
   Comparison is performed by re-implementation of lessThan().
   Note that "multiSort" method name is used instead of "sort" to avoid overloading 
   problems between derived classes and the bases.

   \param columns array of column indices
   \param orders array of individual sort orders for each column in columns
   \sa lessThan()
 */
void QtxMultiSortModel::multiSort( const QVector<int>& columns, 
                                   const QVector<Qt::SortOrder>& orders )
{
  // Non-empty column list means we're already sorting
  if ( mySortColumns.size() )
    return;

  // mySortColumns will be used by lessThan()
  mySortColumns = columns;
  mySortOrders  = orders;
  // Remember this parameter to use it in lessThan()
  myMajorOrder  = mySortOrders.empty() ? Qt::AscendingOrder : mySortOrders.first();
  int aDummySortColumn = mySortColumns.empty() ? 0 : mySortColumns.first();

  // Virtual mechanism still works, so sort() can be re-implemented in derived classes
  invalidate();
  sort( aDummySortColumn, myMajorOrder );

  mySortColumns.clear();
  mySortOrders.clear();
}

/*!
  \brief Comparison method based on a list of columns
  \param idx1 Specifies first row for comparison
  \param idx2 Specifies second row for comparison
  \return true if items for idx1.row() are less that items for idx2.row()
  \sa sort()
 */
bool QtxMultiSortModel::lessThan( const QModelIndex& idx1, const QModelIndex& idx2 ) const
{
  if ( mySortColumns.empty() )
    return QtxProxyModel::lessThan( idx1, idx2 );

  int aSrcRow1 = idx1.row();
  int aSrcRow2 = idx2.row();
  bool aResult = false;

  size_t anIt = 0, anEnd = mySortColumns.size();
  for ( ; !aResult && anIt != anEnd; anIt++ ){
    // Do we have to swap the rows to reverse the comparison result?
    int aRow1 = aSrcRow1, aRow2 = aSrcRow2;
    QModelIndex aParent1 = idx1.parent(), aParent2 = idx2.parent();
    if ( mySortOrders[anIt] != myMajorOrder ){
      std::swap( aRow1, aRow2 );
      std::swap( aParent1, aParent2 );
    }

    QModelIndex aSortIdx1 = this->sourceModel()->index( aRow1, mySortColumns[anIt], aParent1 );
    QModelIndex aSortIdx2 = this->sourceModel()->index( aRow2, mySortColumns[anIt], aParent2 );

    bool localLess = QtxProxyModel::lessThan( aSortIdx1, aSortIdx2 );

    // aSortIdx2 < aSortIdx1 --> it means we should stop here and return false
    if ( !localLess && QtxProxyModel::lessThan( aSortIdx2, aSortIdx1 ) )
      break;

    aResult = aResult || localLess;
  }

  return aResult;
}

