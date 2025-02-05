// Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
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

//  KERNEL SALOME_Event : Define event posting mechanism
//  File   : SALOME_Event.h
//  Author : Sergey ANIKIN
//
#ifndef SALOME_EVENT_H
#define SALOME_EVENT_H

#include "Event.h"

#include <QEvent>

//! SALOME custom event type
#define SALOME_EVENT QEvent::Type( QEvent::User + 10000 )

class EVENT_EXPORT SALOME_CustomEvent : public QEvent
{
public:
  SALOME_CustomEvent( int type );
  SALOME_CustomEvent( QEvent::Type type, void* data );

  void* data() const;
  void  setData( void* data );

private:
  void *d;   //!< internal data
};

class QSemaphore;

class EVENT_EXPORT SALOME_Event
{
public:
  SALOME_Event();
  virtual ~SALOME_Event();

  void            ExecutePostedEvent();
  virtual void    Execute() = 0;

  static bool     IsSessionThread();
  void            process();

protected:
  void            processed();
  static void     GetSessionThread();

private:
  QSemaphore*     mySemaphore;     //!< internal semaphore
};

template<class TObject, typename TRes> class TMemFunEvent : public SALOME_Event
{
public:
  typedef TRes TResult;
  TResult myResult;
  typedef TResult (TObject::* TAction)();
  TMemFunEvent(TObject* theObject, TAction theAction, 
               TResult theResult = TResult()):
    myObject(theObject),
    myAction(theAction),
    myResult(theResult)
  {}
  virtual void Execute()
  {
    myResult = (myObject->*myAction)();
  }
private:
  TObject* myObject;
  TAction  myAction;
};

template<class TObject> class TVoidMemFunEvent : public SALOME_Event
{
public:
  typedef void (TObject::* TAction)();
  TVoidMemFunEvent(TObject* theObject, TAction theAction):
    myObject(theObject),
    myAction(theAction)
  {}
  virtual void Execute()
  {
    (myObject->*myAction)();
  }
private:
  TObject* myObject;
  TAction myAction;
};

template<class TObject, typename TRes, typename TArg, typename TStoreArg = TArg> 
class TMemFun1ArgEvent : public SALOME_Event
{
public:
  typedef TRes TResult;
  TResult myResult;
  typedef TResult (TObject::* TAction)(TArg);
  TMemFun1ArgEvent(TObject* theObject, TAction theAction, TArg theArg, 
                   TResult theResult = TResult()):
    myObject(theObject),
    myAction(theAction),
    myResult(theResult),
    myArg(theArg)
  {}
  virtual void Execute()
  {
    myResult = (myObject->*myAction)(myArg);
  }
private:
  TObject* myObject;
  TAction myAction;
  TStoreArg myArg;
};

template<class TObject, typename TArg, typename TStoreArg = TArg> 
class TVoidMemFun1ArgEvent : public SALOME_Event
{
public:
  typedef void (TObject::* TAction)(TArg);
  TVoidMemFun1ArgEvent(TObject* theObject, TAction theAction, TArg theArg):
    myObject(theObject),
    myAction(theAction),
    myArg(theArg)
  {}
  virtual void Execute()
  {
    (myObject->*myAction)(myArg);
  }
private:
  TObject* myObject;
  TAction myAction;
  TStoreArg myArg;
};

template<class TObject, typename TRes, typename TArg, typename TArg1, typename TStoreArg = TArg, typename TStoreArg1 = TArg1>
class TMemFun2ArgEvent: public SALOME_Event
{
public:
  typedef TRes TResult;
  TResult myResult;
  typedef TResult (TObject::* TAction)(TArg,TArg1);
  TMemFun2ArgEvent(TObject* theObject, TAction theAction, 
                   TArg theArg, TArg1 theArg1,
                   TResult theResult = TResult()):
    myObject(theObject),
    myAction(theAction),
    myResult(theResult),
    myArg(theArg),
    myArg1(theArg1)
  {}
  virtual void Execute()
  {
    myResult = (myObject->*myAction)(myArg,myArg1);
  }
private:
  TObject* myObject;
  TAction myAction;
  TStoreArg myArg;
  TStoreArg1 myArg1;
};

template<class TObject, typename TArg, typename TArg1, typename TStoreArg = TArg, typename TStoreArg1 = TArg1>
class TVoidMemFun2ArgEvent : public SALOME_Event
{
public:
  typedef void (TObject::* TAction)(TArg,TArg1);
  TVoidMemFun2ArgEvent(TObject* theObject, TAction theAction, TArg theArg, TArg1 theArg1):
    myObject(theObject),
    myAction(theAction),
    myArg(theArg),
    myArg1(theArg1)
  {}
  virtual void Execute()
  {
    (myObject->*myAction)(myArg,myArg1);
  }
private:
  TObject* myObject;
  TAction myAction;
  TStoreArg myArg;
  TStoreArg1 myArg1;
};

template<class TEvent> inline typename TEvent::TResult ProcessEvent(TEvent* theEvent)
{
  typename TEvent::TResult aResult;
  if(SALOME_Event::IsSessionThread()) {
    theEvent->Execute();
    aResult = theEvent->myResult;
  }
  else {
    theEvent->process();
    aResult = theEvent->myResult;
  }
  delete theEvent;
  return aResult;
}

inline void ProcessVoidEvent(SALOME_Event* theEvent)
{
  if(SALOME_Event::IsSessionThread()) {
    theEvent->Execute();
  }
  else {
    theEvent->process();
  }
  delete theEvent;
}

#endif // SALOME_EVENT_H
