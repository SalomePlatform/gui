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
// File   : Session_ServerCheck.hxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#if !defined(SESSION_SERVERCHECK_HXX)
#define SESSION_SERVERCHECK_HXX

#include "SALOME_Session.hxx"

#include <QThread> 
#include <QMutex>

class QWaitCondition;

class SESSION_EXPORT Session_ServerCheck : public QThread
{
  class Locker;

public:
  Session_ServerCheck( QMutex*, QWaitCondition* );
  virtual ~Session_ServerCheck();

  QString         currentMessage();
  QString         error();

  int             currentStep();
  int             totalSteps();

protected:
  virtual void    run();

private:
  void            setStep( const int );
  void            setError( const QString& msg );

private:
  QMutex          myDataMutex;         //!< data mutex
  QMutex*         myMutex;             //!< splash mutex
  QWaitCondition* myWC;                //!< splash wait condition

  bool            myCheckCppContainer; //!< flag : check C++ container
  bool            myCheckPyContainer;  //!< flag : check Python container
  bool            myCheckSVContainer;  //!< flag : check supervision container
  int             myAttempts;          //!< number of attemtps to get response from server
  int             myDelay;             //!< delay between attempts in microseconds
  int             myCurrentStep;       //!< current step
  QString         myMessage;           //!< current information message
  QString         myError;             //!< error message

  friend class Locker;
};

#endif  // SESSION_SERVERCHECK_HXX
