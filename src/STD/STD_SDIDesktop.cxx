#include "STD_SDIDesktop.h"

#include <SUIT_ViewWindow.h>

#include <qvbox.h>
#include <qmenubar.h>
#include <qobjectlist.h>

STD_SDIDesktop::STD_SDIDesktop()
: SUIT_Desktop()
{
  myMainWidget = new QVBox( this );
  myMainWidget->setFrameStyle( QFrame::Panel | QFrame::Sunken );

  setCentralWidget( myMainWidget );
}

STD_SDIDesktop::~STD_SDIDesktop()
{
}

SUIT_ViewWindow* STD_SDIDesktop::activeWindow() const
{
  const QObjectList* children = myMainWidget->children();
  if ( !children )
    return 0;

  QPtrList<SUIT_ViewWindow> winList;
  for ( QObjectListIt it( *children ); it.current(); ++it )
  {
    if ( it.current()->inherits( "SUIT_ViewWindow" ) )
      winList.append( (SUIT_ViewWindow*)it.current() );
  }

  SUIT_ViewWindow* win = 0;
  for ( QPtrListIterator<SUIT_ViewWindow> itr( winList ); itr.current() && !win; ++itr )
  {
    if ( itr.current()->isActiveWindow() )
      win = itr.current();
  }

  if ( !win && !winList.isEmpty() )
    win = winList.getFirst();

  return win;
}

QPtrList<SUIT_ViewWindow> STD_SDIDesktop::windows() const
{
  QPtrList<SUIT_ViewWindow> winList;
      winList.append( activeWindow() );
  return winList;
}

QWidget* STD_SDIDesktop::parentArea() const
{
  return myMainWidget;
}
