dnl  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
dnl  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
dnl 
dnl  This library is free software; you can redistribute it and/or 
dnl  modify it under the terms of the GNU Lesser General Public 
dnl  License as published by the Free Software Foundation; either 
dnl  version 2.1 of the License. 
dnl 
dnl  This library is distributed in the hope that it will be useful, 
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of 
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
dnl  Lesser General Public License for more details. 
dnl 
dnl  You should have received a copy of the GNU Lesser General Public 
dnl  License along with this library; if not, write to the Free Software 
dnl  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
dnl 
dnl  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
dnl
dnl
dnl

dnl
dnl  OPTIONS_VTK
dnl  ------------------------------------------------------------------------
dnl  Adds the --with-vtk=path and --with-vtk-version configure options
dnl
AC_DEFUN([OPTIONS_VTK], [
  AC_ARG_WITH([vtk],
              [AC_HELP_STRING([--with-vtk], [The prefix where VTK is installed (default is /usr/local)])],
              [with_vtk=$withval], [with_vtk=""])

  AC_ARG_WITH([vtk-version],
              [AC_HELP_STRING([--with-vtk-version], [VTK include directory name is vtk-suffix, e.g. vtk-5.0/. What is the suffix? (Default -5.0)])],
              [vtk_suffix=$withval], [vtk_suffix="yes"])
])

dnl
dnl  CHECK_VTK
dnl  ------------------------------------------------------------------------
dnl
AC_DEFUN([CHECK_VTK], [
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_CXX])dnl
AC_REQUIRE([AC_PROG_CPP])dnl
AC_REQUIRE([AC_PROG_CXXCPP])dnl
AC_REQUIRE([AC_LINKER_OPTIONS])dnl

AC_CHECKING(for VTK)

AC_LANG_SAVE
AC_LANG_CPLUSPLUS

AC_SUBST(VTK_INCLUDES)
AC_SUBST(VTK_LIBS)
AC_SUBST(VTKPY_MODULES)

VTK_INCLUDES=""
VTK_LIBS=""
VTKPY_MODULES=""

vtk_ok=no

if test "x$OpenGL_ok" != "xyes" ; then
   AC_MSG_WARN(VTK needs OpenGL correct configuration, check configure output)
fi

AC_PATH_X
if test "x$x_libraries" != "x"
then
   LXLIB="-L$x_libraries"
else
   LXLIB=""
fi

if test "x$x_libraries" = "x/usr/lib"
then
   LXLIB=""
fi

if test "x$x_libraries" = "x/usr/lib${LIB_LOCATION_SUFFIX}"
then
   LXLIB=""
fi

LOCAL_INCLUDES="$OGL_INCLUDES"
LOCAL_LIBS="-lvtkCommon -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkIO -lvtkRendering -lvtkHybrid -lvtkParallel -lvtkWidgets $OGL_LIBS $LXLIB -lX11 -lXt"
TRY_LINK_LIBS="-lvtkCommon $OGL_LIBS $LXLIB -lX11 -lXt"

dnl VTK version suffix
if test "x$vtk_suffix" == "xno" ; then
  dnl in case user wrote --with-vtk-version=no, use empty suffix
  vtk_suffix=""
fi
if test "x$vtk_suffix" != "xyes" ; then
  VTKSUFFIX="$vtk_suffix"
else
  dnl in case user wrote --with-vtk-version=yes, get the suffix from env
  if test -z $VTKSUFFIX ; then
    VTKSUFFIX="-5.0"
  fi
fi

dnl VTK install dir
if test "x$with_vtk" = "xyes" ; then
  dnl in case user wrote --with-vtk=yes
  with_vtk=""
fi
if test "x$with_vtk" = "xno" ; then
  dnl in case user wrote --with-vtk=no
  with_vtk=""
  AC_MSG_WARN(Value "no", specified for option --with-vtk, is not supported)
fi

if test "x$with_vtk" != "x" ; then
  VTKHOME="$with_vtk"
else
  if test -z $VTKHOME ; then
    AC_MSG_WARN(undefined VTKHOME variable which specify where vtk was compiled)
    for d in /usr/local /usr ; do
      if test -f ${d}/include/vtk${VTKSUFFIX}/vtkPlane.h ; then
        AC_MSG_RESULT(trying ${d})
        VTKHOME="${d}"
        break
      else
        if test -f ${d}/include/vtk-5.0/vtkPlane.h ; then
          AC_MSG_RESULT(trying ${d})
          VTKHOME="${d}"
          VTKSUFFIX="-5.0"
          break
        else
          if test -f ${d}/include/vtk/vtkPlane.h ; then
            AC_MSG_RESULT(trying ${d})
            VTKHOME="${d}"
            VTKSUFFIX=""
            break
          fi
        fi
      fi
    done
  fi
fi

LOCAL_INCLUDES="-I$VTKHOME/include/vtk${VTKSUFFIX} $LOCAL_INCLUDES"
LOCAL_LIBS="-L$VTKHOME/lib${LIB_LOCATION_SUFFIX}/vtk${VTKSUFFIX} $LOCAL_LIBS"
TRY_LINK_LIBS="-L$VTKHOME/lib${LIB_LOCATION_SUFFIX} -L$VTKHOME/lib${LIB_LOCATION_SUFFIX}/vtk${VTKSUFFIX} $TRY_LINK_LIBS"
if test "x$VTKHOME" != "x/usr" ; then
  LOCAL_LIBS="-L$VTKHOME/lib${LIB_LOCATION_SUFFIX} $LOCAL_LIBS"
fi

dnl vtk headers
CPPFLAGS_old="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $LOCAL_INCLUDES"

AC_CHECK_HEADER(vtkPlane.h,vtk_ok="yes",vtk_ok="no")

CPPFLAGS="$CPPFLAGS_old"

if test "x$vtk_ok" = "xyes"
then
   VTK_INCLUDES="$LOCAL_INCLUDES"

   dnl vtk libraries

   AC_MSG_CHECKING(linking VTK library)

   LIBS_old="$LIBS"
   LIBS="$LIBS $TRY_LINK_LIBS"
   #LIBS="$LIBS $LOCAL_LIBS"
   CPPFLAGS_old="$CPPFLAGS"
   CPPFLAGS="$CPPFLAGS $VTK_INCLUDES"

   dnl VTKPY_MODULES="$VTKHOME/python"

   AC_CACHE_VAL(salome_cv_lib_vtk,[
     AC_TRY_LINK([#include "vtkPlane.h"
                 ],
		 [vtkPlane::New()],
		 [salome_cv_lib_vtk=yes],
		 [salome_cv_lib_vtk=no])
   ])
   vtk_ok="$salome_cv_lib_vtk"
   LIBS="$LIBS_old"
   CPPFLAGS="$CPPFLAGS_old"
fi

if  test "x$vtk_ok" = "xno"
then
  AC_MSG_RESULT(no)
  AC_MSG_WARN(unable to link with vtk library)
else
  AC_MSG_RESULT(yes)
  VTK_LIBS="$LOCAL_LIBS"
  VTK_MT_LIBS="$LOCAL_LIBS"
fi

AC_MSG_RESULT(for VTK: $vtk_ok)

AC_LANG_RESTORE

# Save cache
AC_CACHE_SAVE

])dnl
