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
// File   : Plot2d_Object.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "Plot2d_Object.h"

/*!
  Constructor
*/
Plot2d_Object::Plot2d_Object()
: myAutoAssign( true ),
  myHorTitle( "" ), myVerTitle( "" ),
  myHorUnits( "" ), myVerUnits( "" ),
  myName( "" ),
  myYAxis( QwtPlot::yLeft )
{
}

/*!
  Destructor
*/
Plot2d_Object::~Plot2d_Object()
{
}

/*!
  Copy constructor. Makes deep copy of data.
*/
Plot2d_Object::Plot2d_Object( const Plot2d_Object& object )
{
  myAutoAssign = object.isAutoAssign();
  myHorTitle   = object.getHorTitle();
  myVerTitle   = object.getVerTitle();
  myHorUnits   = object.getHorUnits();
  myVerUnits   = object.getVerUnits();
  myName       = object.getName();
  myPoints     = object.getPointList();
}

/*!
  operator=. Makes deep copy of data.
*/
Plot2d_Object& Plot2d_Object::operator=( const Plot2d_Object& object )
{
  myAutoAssign = object.isAutoAssign();
  myHorTitle   = object.getHorTitle();
  myVerTitle   = object.getVerTitle();
  myHorUnits   = object.getHorUnits();
  myVerUnits   = object.getVerUnits();
  myName       = object.getName();
  myPoints     = object.getPointList();
  return *this;
}

/*!
 * Updates object fields
 */
void Plot2d_Object::updatePlotItem( QwtPlotItem* theItem )
{
  if ( theItem->rtti() != rtti() )
    return;
}

/**
 * Auto fill parameters of object by plot view
 */
void Plot2d_Object::autoFill( const QwtPlot* )
{
}

/*!
  \return title of table
*/
QString Plot2d_Object::getTableTitle() const
{
  return QString();
}

/*!
  Sets object's horizontal title
*/
void Plot2d_Object::setHorTitle( const QString& title )
{
  myHorTitle = title;
}

/*!
  Gets object's horizontal title
*/
QString Plot2d_Object::getHorTitle() const
{
  return myHorTitle;
}

/*!
  Sets object's vertical title
*/
void Plot2d_Object::setVerTitle( const QString& title )
{
  myVerTitle = title;
}

/*!
  Gets object's vertical title
*/
QString Plot2d_Object::getVerTitle() const
{
  return myVerTitle;
}

/*!
  Sets object's horizontal units
*/
void Plot2d_Object::setHorUnits( const QString& units )
{
  myHorUnits = units;
}

/*!
  Gets object's horizontal units
*/
QString Plot2d_Object::getHorUnits() const
{
  return myHorUnits;
}

/*!
  Sets object's vertical units
*/
void Plot2d_Object::setVerUnits( const QString& units )
{
  myVerUnits = units;
}

/*!
  Gets object's vertical units
*/
QString Plot2d_Object::getVerUnits() const
{
  return myVerUnits;
}

/*!
  Sets object's name
 */
void Plot2d_Object::setName( const QString& theName )
{
  myName = theName;
}
/*!
  Gets object's name
 */
QString Plot2d_Object::getName() const
{
  return myName;
}

/*!
  Adds one point for object.
*/
void Plot2d_Object::addPoint(double theX, double theY, const QString& txt )
{
  Plot2d_Point aPoint;
  aPoint.x = theX;
  aPoint.y = theY;
  aPoint.text = txt;
  myPoints.append(aPoint);
}

/*!
  Insert one point for object on some position.
*/
void Plot2d_Object::insertPoint(int thePos, double theX, double theY,
                                const QString& txt)
{
  Plot2d_Point aPoint;
  aPoint.x = theX;
  aPoint.y = theY;
  aPoint.text = txt;

  QList<Plot2d_Point>::iterator aIt;
  int aCurrent = 0;
  for(aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt) {
    if (thePos == aCurrent) {
      myPoints.insert(aIt, aPoint);
      return;
    }
    aCurrent++;  
  }
  myPoints.append(aPoint);
}

/*!
  Delete one point for object on some position.
*/
void Plot2d_Object::deletePoint(int thePos)
{
  if ( thePos >= 0 && thePos < myPoints.count() )
    myPoints.removeAt( thePos );
}

/*!
  Remove all points for object.
*/
void Plot2d_Object::clearAllPoints()
{
  myPoints.clear();
}

/*!
  Gets object's data : abscissas of points
*/
pointList Plot2d_Object::getPointList() const
{
  return myPoints;
}

/*!
  Gets object's data : abscissas of points
*/
void Plot2d_Object::setPointList( const pointList& points )
{
  clearAllPoints();
  myPoints = points;
}

/*!
  Sets object's data. 
*/
void Plot2d_Object::setData( const double* hData, const double* vData, long size, const QStringList& lst )
{
  clearAllPoints();
  QStringList::const_iterator anIt = lst.begin(), aLast = lst.end(); 
  for( long i = 0; i < size; i++, anIt++ )
    addPoint( hData[i], vData[i], anIt==aLast ? QString() : *anIt );
}

/*!
  Gets object's data : abscissas of points
*/
double* Plot2d_Object::horData() const
{
  int aNPoints = nbPoints();
  double* aX = new double[aNPoints];
  for (int i = 0; i < aNPoints; i++) {
    aX[i] = myPoints[i].x;
  }
  return aX;
}

/*!
  Gets object's data : ordinates of points
*/
double* Plot2d_Object::verData() const
{
  int aNPoints = nbPoints();
  double* aY = new double[aNPoints];
  for (int i = 0; i < aNPoints; i++) {
    aY[i] = myPoints[i].y;
  }
  return aY;
}

/*!
  Gets object's data : number of points
*/
int Plot2d_Object::nbPoints() const
{
  return myPoints.count();
}

/*!
  Returns true if object has no data
*/
bool Plot2d_Object::isEmpty() const
{
  return myPoints.isEmpty();
}

/*!
  Sets object's AutoAssign flag - in this case attributes will be set automatically
*/
void Plot2d_Object::setAutoAssign( bool on )
{
  myAutoAssign = on;
}

/*!
  Gets object's AutoAssign flag state
*/
bool Plot2d_Object::isAutoAssign() const
{
  return myAutoAssign;
}

/*!
  Sets object's y axis
*/
void Plot2d_Object::setYAxis(QwtPlot::Axis theYAxis)
{
  if(theYAxis == QwtPlot::yLeft || theYAxis == QwtPlot::yRight)
    myYAxis = theYAxis;
}

/*!
  Gets object's y axis
*/
QwtPlot::Axis Plot2d_Object::getYAxis() const
{
  return myYAxis;
}

/*!
  Gets object's minimal abscissa
*/
double Plot2d_Object::getMinX() const
{
  QList<Plot2d_Point>::const_iterator aIt;
  double aMinX = 1e150;
  //int aCurrent = 0;
  for(aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt) {
    if ( (*aIt).x < aMinX )
      aMinX = (*aIt).x;
  }
  return aMinX;
}

/*!
  Gets object's minimal ordinate
*/
double Plot2d_Object::getMinY() const
{
  QList<Plot2d_Point>::const_iterator aIt;
  double aMinY = 1e150;
  //int aCurrent = 0;
  for(aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt) {
    if ( (*aIt).y < aMinY )
      aMinY = (*aIt).y;
  }
  return aMinY;
}

/*!
  Checks if two colors are close to each other [ static ]
  uses COLOR_DISTANCE variable as max tolerance for comparing of colors
*/

const long COLOR_DISTANCE = 100;
bool Plot2d_Object::closeColors( const QColor& color1,
                                        const QColor& color2 )
{
  long tol = abs( color2.red()   - color1.red() ) + 
             abs( color2.green() - color1.green() ) +
       abs( color2.blue()  - color1.blue() );

  return ( tol <= COLOR_DISTANCE );
}

/*!
  Changes text assigned to point of object
  \param ind -- index of point
  \param txt -- new text
*/
void Plot2d_Object::setText( const int ind, const QString& txt )
{
  if( ind<0 || ind>=myPoints.count() )
    return;

  myPoints[ind].text = txt;
}

/*!
  \return text assigned to point
  \param ind -- index of point
*/
QString Plot2d_Object::text( const int ind ) const
{
  if( ind<0 || ind>=myPoints.count() )
    return QString();
  else
    return myPoints[ind].text;
}
