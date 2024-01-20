// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
//  Date   : 22/06/2007
//
#include "SUITApp_init_python.hxx"
#include <QString>

#if PY_VERSION_HEX < 0x03050000
static char*
Py_EncodeLocale(const wchar_t *arg, size_t *size)
{
	return _Py_wchar2char(arg, size);
}
static wchar_t*
Py_DecodeLocale(const char *arg, size_t *size)
{
	return _Py_char2wchar(arg, size);
}
#endif

bool SUIT_PYTHON::initialized                       = false;

void SUIT_PYTHON::init_python(int argc, char **argv)
{
  if (Py_IsInitialized())
  {
    return;
  }

  wchar_t **changed_argv = new wchar_t*[argc]; // Setting arguments
  for (int i = 0; i < argc; i++)
  {
   changed_argv[i] = Py_DecodeLocale(argv[i], NULL);    
  }

  Py_SetProgramName(changed_argv[0]);
  Py_Initialize(); // Initialize the interpreter

  PySys_SetArgv(argc, changed_argv);
  PyRun_SimpleString("import threading\n");
  // VSR (22/09/2016): This is a workaround to prevent invoking qFatal() from PyQt5
  // causing application aborting
  QString script;
  script += "def _custom_except_hook(exc_type, exc_value, exc_traceback):\n";
  script += "  import sys\n";
  script += "  sys.__excepthook__(exc_type, exc_value, exc_traceback)\n";
  script += "  pass\n";
  script += "\n";
  script += "import sys\n";
  script += "sys.excepthook = _custom_except_hook\n";
  script += "del _custom_except_hook, sys\n";
  PyRun_SimpleString(qUtf8Printable(script));
  // VSR (22/09/2016): end of workaround
#if PY_VERSION_HEX < 0x03070000
  PyEval_InitThreads(); // Create (and acquire) the interpreter lock - can be called many times
#endif
  // Py_InitThreads acquires the GIL
  PyThreadState *pts = PyGILState_GetThisThreadState(); 
  PyEval_ReleaseThread(pts);
  SUIT_PYTHON::initialized = true;
}
