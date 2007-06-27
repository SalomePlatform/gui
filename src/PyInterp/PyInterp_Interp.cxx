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
//  File   : PyInterp_Interp.cxx
//  Author : Christian CAREMOLI, Paul RASCLE, EDF
//  Module : SALOME

#include "PyInterp_Interp.h"  // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include <cStringIO.h>

#include <string>
#include <vector>

using namespace std;

#define TOP_HISTORY_PY   "--- top of history ---"
#define BEGIN_HISTORY_PY "--- begin of history ---"

PyLockWrapper::PyLockWrapper(PyThreadState* theThreadState): 
  myThreadState(theThreadState),
  mySaveThreadState(0)
{
  if (myThreadState->interp == PyInterp_Interp::_interp)
    _savestate = PyGILState_Ensure();
  else
    PyEval_AcquireThread(myThreadState);
}

PyLockWrapper::~PyLockWrapper()
{
  if (myThreadState->interp == PyInterp_Interp::_interp)
    PyGILState_Release(_savestate);
  else
    PyEval_ReleaseThread(myThreadState);
}

PyLockWrapper PyInterp_Interp::GetLockWrapper()
{
  return _tstate;
}

/*!
  \class PyInterp_Interp
  \brief Generic embedded Python interpreter.
*/

int   PyInterp_Interp::_argc   = 1;
char* PyInterp_Interp::_argv[] = {""};
PyObject*           PyInterp_Interp::builtinmodule = NULL;
PyThreadState*      PyInterp_Interp::_gtstate      = NULL;
PyInterpreterState* PyInterp_Interp::_interp       = NULL;

/*!
  \brief Basic constructor.
  
  After construction the interpreter instance successor classes 
  must call virtual method initalize().
*/
PyInterp_Interp::PyInterp_Interp(): 
  _tstate(0), _vout(0), _verr(0), _g(0)
{
}

/*!
  \brief Destructor.
*/
PyInterp_Interp::~PyInterp_Interp()
{
}

/*!
  \brief Initialize embedded interpreter.
  
  This method shoud be called after construction of the interpreter.
  The method initialize() calls virtuals methods
  - initPython()  to initialize global Python interpreter
  - initState()   to initialize embedded interpreter state
  - initContext() to initialize interpreter internal context
  - initRun()     to prepare interpreter for running commands
  which should be implemented in the successor classes, according to the
  embedded Python interpreter policy (mono or multi interpreter, etc).
*/
void PyInterp_Interp::initialize()
{
  _history.clear();       // start a new list of user's commands 
  _ith = _history.begin();

  initPython();
  // Here the global lock is released

  initState();

  PyLockWrapper aLock = GetLockWrapper();

  initContext();

  // used to interpret & compile commands
  PyObjWrapper m(PyImport_ImportModule("codeop"));
  if(!m) {
    PyErr_Print();
    return;
  }

  // Create cStringIO to capture stdout and stderr
  PycString_IMPORT;
  if (PycStringIO) { // CTH11627 : additional check
    _vout = PycStringIO->NewOutput(128);
    _verr = PycStringIO->NewOutput(128);
  }

  // All the initRun outputs are redirected to the standard output (console)
  initRun();
}

/*!
  \brief Initialize Python interpreter.

  Set program name, initialize interpreter, set program arguments,
  initiaize threads.
 */
void PyInterp_Interp::initPython()
{
  if (Py_IsInitialized())
    return;

  // Python is not initialized
  Py_SetProgramName(_argv[0]);
  Py_Initialize(); // Initialize the interpreter
  PySys_SetArgv(_argc, _argv);
  PyEval_InitThreads(); // Create (and acquire) the interpreter lock
  _interp = PyThreadState_Get()->interp;
  _gtstate = PyEval_SaveThread(); // Release global thread state
}

/*!
  \brief Get embedded Python interpreter banner.
  \return banner string
 */
string PyInterp_Interp::getbanner()
{
 // Should we take the lock ?
 // PyEval_RestoreThread(_tstate);
  string aBanner("Python ");
  aBanner = aBanner + Py_GetVersion() + " on " + Py_GetPlatform() ;
  aBanner = aBanner + "\ntype help to get general information on environment\n";
  //PyEval_SaveThread();
  return aBanner;
}

/*!
  \brief Initialize run command.
 
  This method is used to prepare interpreter for running 
  Python commands.
  
  \return \c true on success and \c false on error
*/
bool PyInterp_Interp::initRun()
{
  PySys_SetObject("stderr",_verr);
  PySys_SetObject("stdout",_vout);

  PyObjWrapper verr(PyObject_CallMethod(_verr,"reset",""));
  PyObjWrapper vout(PyObject_CallMethod(_vout,"reset",""));

  //PyObject *m = PyImport_GetModuleDict();
  
  PySys_SetObject("stdout",PySys_GetObject("__stdout__"));
  PySys_SetObject("stderr",PySys_GetObject("__stderr__"));

  return true;
}

/*!
  \brief Compile Python command and evaluate it in the 
         python dictionary context if possible.
  \internal
  \param command Python command string
  \param context Python context (dictionary)
  \return -1 on fatal error, 1 if command is incomplete and 0
         if command is executed successfully
 */
static int compile_command(const char *command,PyObject *context)
{
  PyObject *m = PyImport_AddModule("codeop");
  if(!m) { // Fatal error. No way to go on.
    PyErr_Print();
    return -1;
  }
  PyObjWrapper v(PyObject_CallMethod(m,"compile_command","s",command));
  if(!v) {
    // Error encountered. It should be SyntaxError,
    //so we don't write out traceback
    PyObjWrapper exception, value, tb;
    PyErr_Fetch(&exception, &value, &tb);
    PyErr_NormalizeException(&exception, &value, &tb);
    PyErr_Display(exception, value, NULL);
    return -1;
  }
  else if (v == Py_None) {
    // Incomplete text we return 1 : we need a complete text to execute
    return 1;
  }
  else {
    // Complete and correct text. We evaluate it.
    //#if PY_VERSION_HEX < 0x02040000 // python version earlier than 2.4.0
    //    PyObjWrapper r(PyEval_EvalCode(v,context,context));
    //#else
    PyObjWrapper r(PyEval_EvalCode((PyCodeObject *)(void *)v,context,context));
    //#endif
    if(!r) {
      // Execution error. We return -1
      PyErr_Print();
      return -1;
    }
    // The command has been successfully executed. Return 0
    return 0;
  }
}

/*!
  \brief Run Python command.
  \param command Python command
  \return command status
*/
int PyInterp_Interp::run(const char *command)
{
  beforeRun();
  return simpleRun(command);
}

/*!
  \brief Run Python command (used internally).
  \param command Python command
  \param addToHistory if \c true (default), the command is added to the commands history
  \return command status
*/
int PyInterp_Interp::simpleRun(const char *command, const bool addToHistory)
{
  if( addToHistory && strcmp(command,"") != 0 ) {
    _history.push_back(command);
    _ith = _history.end();
  }

  // We come from C++ to enter Python world
  // We need to acquire the Python global lock
  //PyLockWrapper aLock(_tstate); // san - lock is centralized now

  // Reset redirected outputs before treatment
  PySys_SetObject("stderr",_verr);
  PySys_SetObject("stdout",_vout);

  PyObjWrapper verr(PyObject_CallMethod(_verr,"reset",""));
  PyObjWrapper vout(PyObject_CallMethod(_vout,"reset",""));

  int ier = compile_command(command,_g);

  // Outputs are redirected on standards outputs (console)
  PySys_SetObject("stdout",PySys_GetObject("__stdout__"));
  PySys_SetObject("stderr",PySys_GetObject("__stderr__"));

  return ier;
}

/*!
  \brief Get previous command in the commands history.
  \return previous command
*/
const char * PyInterp_Interp::getPrevious()
{
  if(_ith != _history.begin()){
    _ith--;
    return (*_ith).c_str();
  }
  else
    return BEGIN_HISTORY_PY;
}

/*!
  \brief Get next command in the commands history.
  \return next command
*/
const char * PyInterp_Interp::getNext()
{
  if(_ith != _history.end()){
    _ith++;
  }
  if (_ith == _history.end())
    return TOP_HISTORY_PY;
  else
    return (*_ith).c_str();
}

/*!
  \brief Get standard error output.
  \return standard error output
*/
string PyInterp_Interp::getverr(){ 
  //PyLockWrapper aLock(_tstate);
  PyObjWrapper v(PycStringIO->cgetvalue(_verr));
  string aRet(PyString_AsString(v));
  return aRet;
}

/*!
  \brief Get standard output.
  \return standard output
*/
string PyInterp_Interp::getvout(){  
  //PyLockWrapper aLock(_tstate);
  PyObjWrapper v(PycStringIO->cgetvalue(_vout));
  string aRet(PyString_AsString(v));
  return aRet;
}
