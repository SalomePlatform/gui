#include "SUIT_PopupClient.h"

#include <qevent.h>

/*
  Class: SUIT_PopupClient [public]
  Descr: Base class for instances which creates popup menu on QContextMenuEvent
*/

SUIT_PopupClient::SUIT_PopupClient()
: mySignal( 0 )
{
}

SUIT_PopupClient::~SUIT_PopupClient()
{
  delete mySignal;
}

bool SUIT_PopupClient::connectPopupRequest( QObject* reciever, const char* slot )
{
  if ( !reciever || !slot )
    return false;
  if ( !mySignal )
    mySignal = new Signal();
  return QObject::connect( mySignal, SIGNAL( contextMenuRequest( SUIT_PopupClient*, QContextMenuEvent* ) ),
                           reciever, slot );
}

bool SUIT_PopupClient::disconnectPopupRequest( QObject* reciever, const char* slot )
{
  if ( !reciever || !slot || !mySignal )
    return false;
  return QObject::disconnect( mySignal, SIGNAL( contextMenuRequest( SUIT_PopupClient*, QContextMenuEvent* ) ),
                              reciever, slot );
}

void SUIT_PopupClient::contextMenuRequest( QContextMenuEvent* e )
{
  if ( mySignal )
    mySignal->sendSignal( this, e );
}

/*
  Class: SUIT_PopupClient::Signal [internal]
  Descr: invoke signal which is connected to reciever in SUIT_PopupClient
*/

SUIT_PopupClient::Signal::Signal()
: QObject( 0 )
{
}

SUIT_PopupClient::Signal::~Signal()
{}

void SUIT_PopupClient::Signal::sendSignal( SUIT_PopupClient* client, QContextMenuEvent* e )
{
  emit contextMenuRequest( client, e );
}
