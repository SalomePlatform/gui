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

#include "SUIT_ToolButton.h"

#include <QtGui/qmenu.h>
#include <QtGui/qstyle.h>

/*!Constructor.*/
SUIT_ToolButton::SUIT_ToolButton( QWidget *parent, const char *name,
                                  bool changeItemAfterClick )
: QToolButton( parent ),
myChangeItemAfterClick( changeItemAfterClick )
{
  initialize();
}

/*!Initialize tool buttons.*/
void SUIT_ToolButton::initialize()
{
//  mySignal = NULL;
  myPopup = new QMenu( this );
  setMenu( myPopup );
  connect( myPopup, SIGNAL( activated( int ) ), SLOT( OnSelectAction( int ) ) );
}

/*!drawButton is redefined to draw DownArrow*/
void SUIT_ToolButton::drawButton( QPainter * p )
{
/*
  QToolButton::drawButton( p );

//draw DownArrow
  int x, y, w, h;
  QStyle::visualRect(QRect(0, 0, width(), height()), this).rect( &x, &y, &w, &h );
  style().drawPrimitive( QStyle::PE_ArrowDown,
    p, QRect(x+w/2+3, y+h/2+3, w/2, h/2),   //QRect(x+(w-x)/2, y+(h-y)/2, w, h)
    colorGroup(), isEnabled() );
*/
}

/*! Add action into popup*/
void SUIT_ToolButton::AddAction( QAction* theAction )
{
  bool aIsFirst = false;
  if ( myPopup->actions().isEmpty() ) 
  {
    aIsFirst = true;
    setIcon( theAction->icon() );
    setText( theAction->text() );
  }
    myPopup->addAction( theAction );
}

/*! Sets myPopup item with theIndex as current*/
void SUIT_ToolButton::SetItem( int theIndex )
{
  QAction* a = myPopup->actions()[theIndex];
  if ( a )
  {
    setIcon( a->icon() );
    setText( a->text() );
  }
}

/*!Public SLOT.
 * On select action (icon and text set with id = \a theItemID)
 */
void SUIT_ToolButton::OnSelectAction( int theItemID )
{
/*
  if ( myChangeItemAfterClick )
  {
    // Protection against unexpected null pointers returned
    if ( myPopup->iconSet(theItemID) )
      setPixmap(myPopup->iconSet(theItemID)->pixmap());
    setTextLabel(myPopup->text(theItemID));
    QMenuItem* aItem = myPopup->findItem(theItemID);
    if (aItem != NULL) 
    {
      mySignal = aItem->signal();
    }
  }
*/
}

/*!On mouse release event.*/
void SUIT_ToolButton::mouseReleaseEvent( QMouseEvent* theEvent )
{
  QToolButton::mouseReleaseEvent( theEvent );
/*
  if ( mySignal ) 
    mySignal->activate();
*/
}
