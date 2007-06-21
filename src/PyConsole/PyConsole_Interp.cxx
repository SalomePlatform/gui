//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : PyConsole_Interp.cxx
//  Author : Nicolas REJNERI
//  Module : SALOME

#include "PyConsole_Interp.h"

/*!
  \class PyConsole_Interp
  \brief Python interpreter to be embedded to the SALOME study's GUI.

  Python interpreter is created one per SALOME study.
  Calls initialize method defined in the base class, which calls redefined 
  virtual methods initState() & initContext().

  /EDF-CCAR/
  When SALOME uses multi Python interpreter feature, 
  every study has its own interpreter and thread state (_tstate = Py_NewInterpreter()).
  This is fine because every study has its own modules (sys.modules) stdout and stderr.<br>
  <b>But</b> some Python modules must be imported only once. In multi interpreter 
  context Python modules (*.py) are imported several times.
  For example, the pyqt module must be imported only once because 
  it registers classes in a C module.
  It's quite the same with omniorb modules (internals and generated with omniidl).
  This problem is handled with "shared modules" defined in salome_shared_modules.py.
  These "shared modules" are imported only once and only copied in all 
  the other interpreters.<br>
  <b>But</b> it's not the only problem. Every interpreter has its own 
  __builtin__ module. That's fine but if we have copied some modules 
  and imported others problems may arise with operations that are not allowed
  in restricted execution environment. So we must impose that all interpreters
  have identical __builtin__ module.
*/

using namespace std;

/*!
  \brief Constructor.

  Creates new python interpreter.
*/
PyConsole_Interp::PyConsole_Interp(): PyInterp_base()
{
}

/*!
  \brief Destructor.

  Does nothing for the moment.
*/
PyConsole_Interp::~PyConsole_Interp()
{
}
 
/*!
  \brief Initialize internal Python interpreter state.
*/
bool PyConsole_Interp::initState()
{
  // The GIL is acquired and will be held on initState output
  // It is the caller responsability to release the lock if needed

/* LLS
  PyEval_AcquireLock();
  _tstate = Py_NewInterpreter(); // create an interpreter and save current state
  PySys_SetArgv(PyInterp_base::_argc,PyInterp_base::_argv); // initialize sys.argv
*/

  PyEval_AcquireLock();
#ifdef WIN32 
  _tstate = PyGILState_GetThisThreadState();
  // if no thread state defined
  if ( _tstate )
    PyThreadState_Swap(_tstate);
  else
#endif
  {
    _tstate = Py_NewInterpreter(); // create an interpreter and save current state
    PySys_SetArgv(PyInterp_base::_argc,PyInterp_base::_argv); // initialize sys.argv
    //if(MYDEBUG) MESSAGE("PythonConsole_PyInterp::initState - this = "<<this<<"; _tstate = "<<_tstate);
  }

  
  //If builtinmodule has been initialized all the sub interpreters
  // will have the same __builtin__ module
  if(builtinmodule){ 
    PyObject *m = PyImport_GetModuleDict();
    PyDict_SetItemString(m, "__builtin__", builtinmodule);
    _tstate->interp->builtins = PyModule_GetDict(builtinmodule);
    Py_INCREF(_tstate->interp->builtins);
  }
  PyEval_ReleaseThread(_tstate);
  return true;
}

/*!
  \brief Initialize python interpeter context.

  The GIL is assumed to be held.
  It is the caller responsability caller to acquire the GIL.
  It will still be held on initContext() exit.
*/
bool PyConsole_Interp::initContext()
{
  PyObject *m = PyImport_AddModule("__main__");  // interpreter main module (module context)
  if(!m){
    PyErr_Print();
    return false;
  }  
  _g = PyModule_GetDict(m);          // get interpreter dictionnary context

  if(builtinmodule){
    PyDict_SetItemString(_g, "__builtins__", builtinmodule); // assign singleton __builtin__ module
  }
  return true;
}
