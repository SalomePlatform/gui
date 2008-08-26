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
// File:      QtxDoubleSpinBox.cxx
// Author:    Sergey TELKOV

#include "QtxDoubleSpinBox.h"

#include <QLineEdit>
#include <QDoubleValidator>

/*!
  \class QtxDoubleSpinBox
  \brief Enhanced version of the Qt's double spin box.

  The QtxDoubleSpinBox class represents the widget for entering the
  floating point values. In addition to the functionality provided by
  QDoubleSpinBox, this class supports "cleared" state - this is the
  state corresponding to "None" (or empty) entered value.

  To set "cleared" state use setCleared() method. To check if the spin
  box stores "cleared" state, use isCleared() method.
  For example:
  \code
  if (myDblSpinBox->isCleared()) {
    ... // process "None" state
  }
  else {
    double value = myDblSpinBox->value();
    ... // process entered value
  }
  \endcode
*/

/*!
  \brief Constructor.

  Constructs a spin box with 0.0 as minimum value and 99.99 as maximum value,
  a step value of 1.0 and a precision of 2 decimal places. 
  The value is initially set to 0.00.

  \param parent parent object
*/
QtxDoubleSpinBox::QtxDoubleSpinBox( QWidget* parent )
: QDoubleSpinBox( parent ),
  myCleared( false ),
  myPrecision(0)
{
  connect( lineEdit(), SIGNAL( textChanged( const QString& ) ), 
	   this, SLOT( onTextChanged( const QString& ) ) );
}

QValidator::State QtxDoubleSpinBox::validate( QString& str, int& pos ) const
{
  if (myPrecision >= 0)
    return QDoubleSpinBox::validate(str, pos);

  QString pref = this->prefix();
  QString suff = this->suffix();
  uint overhead = pref.length() + suff.length();
  QValidator::State state = QValidator::Invalid;

  QDoubleValidator v (NULL);

  if ( overhead == 0 )
    state = v.validate( str, pos );
  else
    {
      if ( str.length() >= overhead && str.startsWith( pref ) &&
	   str.right( suff.length() ) == suff )
	{
	  QString core = str.mid( pref.length(), str.length() - overhead );
	  int corePos = pos - pref.length();
	  state = v.validate( core, corePos );
	  pos = corePos + pref.length();
	  str.replace( pref.length(), str.length() - overhead, core );
	}
      else
	{
	  state = v.validate( str, pos );
	  if ( state == QValidator::Invalid )
	    {
	      QString special = this->specialValueText().trimmed();
	      QString candidate = str.trimmed();
	      if ( special.startsWith( candidate ) )
		{
		  if ( candidate.length() == special.length() )
		    state = QValidator::Acceptable;
		  else
		    state = QValidator::Intermediate;
		}
	    }
	}
    }
  return state;
}

/*!
  \brief Constructor.

  Constructs a spin box with specified minimum, maximum and step value.
  The precision is set to 2 decimal places. 
  The value is initially set to the minimum value.

  \param min spin box minimum possible value
  \param max spin box maximum possible value
  \param step spin box increment/decrement value
  \param parent parent object
*/
QtxDoubleSpinBox::QtxDoubleSpinBox( double min, double max, double step, QWidget* parent )
: QDoubleSpinBox( parent ),
  myCleared( false ),
  myPrecision( 0 )
{
  setMinimum( min );
  setMaximum( max );
  setSingleStep( step );

  connect( lineEdit(), SIGNAL( textChanged( const QString& ) ), 
	   this, SLOT( onTextChanged( const QString& ) ) );
}

/*!
  \brief Destructor.
*/
QtxDoubleSpinBox::~QtxDoubleSpinBox()
{
}

/*!
  \brief Check if spin box is in the "cleared" state.
  \return \c true if spin box is cleared
*/
bool QtxDoubleSpinBox::isCleared() const
{
  return myCleared;
}

/*!
  \brief Change "cleared" status of the spin box.
  \param on new "cleared" status
*/
void QtxDoubleSpinBox::setCleared( const bool on )
{
  if ( myCleared == on )
    return;
  
  myCleared = on;
  setSpecialValueText( specialValueText() );
}

void QtxDoubleSpinBox::setPrecision( const int prec )
{
  int newPrec = qMax( prec, 0 );
  int oldPrec = qMax( myPrecision, 0 );
  myPrecision = prec;
  if ( newPrec != oldPrec )
    update();
}

int QtxDoubleSpinBox::getPrecision() const
{
  return myPrecision;
}

double QtxDoubleSpinBox::valueFromText(const QString &text) const
{
  if (myPrecision < 0)
    return text.toDouble();

  return QDoubleSpinBox::valueFromText(text);
}

QString QtxDoubleSpinBox::textFromValue( double val ) const
{
  QString s;
  s.setNum( val, myPrecision >= 0 ? 'f' : 'g', myPrecision == 0 ? 6 : qAbs( myPrecision ) );
  return removeTrailingZeroes( s );
}

/*!
  \brief Return string without excess zeros in start and in end
*/
QString QtxDoubleSpinBox::removeTrailingZeroes( const QString& src ) const
{
  QString delim( QLocale().decimalPoint() );

  int idx = src.lastIndexOf( delim );
  if ( idx == -1 )
    return src;

  QString iPart = src.left( idx );
  QString fPart = src.mid( idx + 1 );

  while ( !fPart.isEmpty() && fPart.at( fPart.length() - 1 ) == '0' )
    fPart.remove( fPart.length() - 1, 1 );

  QString res = iPart;
  if ( !fPart.isEmpty() )
    res += delim + fPart;

  return res;
}

/*!
  \brief Perform \a steps increment/decrement steps.
  
  The \a steps value can be any integer number. If it is > 0,
  the value incrementing is done, otherwise value is decremented
  \a steps times.  

  \param steps number of increment/decrement steps
*/
void QtxDoubleSpinBox::stepBy( int steps )
{
  myCleared = false;

  QDoubleSpinBox::stepBy( steps );
}

/*!
  \brief Called when user enters the text in the spin box.
  \param txt current spin box text (not used)
*/
void QtxDoubleSpinBox::onTextChanged( const QString& /*txt*/ )
{
  myCleared = false;
}
