#
# Check existence of Test recorder sources and libraries.
# Set ENABLE_TESTRECORDER to yes|no
#
# Author : Margarita KARPUNINA (OCN, 2008)
#

AC_DEFUN([CHECK_TESTRECORDER],[

AC_ARG_WITH(testrecorder,
	    [  --with-testrecorder=DIR       root directory path of TestRecorder installation],
	    [],
	    [with_testrecorder=no])

AS_IF([test "x$with_testrecorder" != xno || test "$with_testrecorder" != "no"],[

  echo
  echo ---------------------------------------------
  echo Testing TestRecorder
  echo ---------------------------------------------
  echo

  AC_REQUIRE([CHECK_QT])dnl
  AC_REQUIRE([AC_PROG_CXX])dnl
  AC_REQUIRE([AC_PROG_CXXCPP])dnl
  
  AC_CHECKING(for TestRecorder product)
  
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  
  TESTRECORDER_INCLUDES=""
  TESTRECORDER_LIBS=""
  ENABLE_TESTRECORDER="no"
  
  TestRecorder_ok=no

  if test "$with_testrecorder" == "yes" || test "$with_testrecorder" == "auto"; then
      TESTRECORDER_HOME=""
  else
      TESTRECORDER_HOME="$with_testrecorder"
  fi
  
  if test "$TESTRECORDER_HOME" == "" ; then
      if test "x$TESTRECORDERHOME" != "x" ; then
          TESTRECORDER_HOME=$TESTRECORDERHOME
      fi
  fi
  
  if test "x$TESTRECORDER_HOME" != "x"; then
    TESTRECORDER_INCLUDES="-I$TESTRECORDER_HOME/include"
    TESTRECORDER_LIBS="-L$TESTRECORDER_HOME/lib -lTestRecorder"
  
    CPPFLAGS_old="$CPPFLAGS"
    CXXFLAGS_old="$CXXFLAGS"
    CPPFLAGS="$TESTRECORDER_INCLUDES $QT_INLCUDES $CPPFLAGS"
    CXXFLAGS="$TESTRECORDER_INCLUDES $QT_INCLUDES $CXXFLAGS"
  
    AC_MSG_CHECKING(for TestRecorder header file)
  
    AC_CHECK_HEADER(TestApplication.h,TestRecorder_ok=yes,TestRecorder_ok=no)
  
    if test "x$TestRecorder_ok" == "xyes"; then
  
      AC_MSG_CHECKING(for TestRecorder library)
  
      LDFLAGS_old="$LDFLAGS"
      LDFLAGS="-L. -$TESTRECORDER_LIBS $QT_LIBS $LDFLAGS"
  
      AC_TRY_LINK(
	  #include "TestApplication.h",
          TESTRECORDER_init();,
	  TestRecorder_ok=yes,TestRecorder_ok=no
	  )
  
      LDFLAGS="$LDFLAGS_old"
  
      AC_MSG_RESULT($TestRecorder_ok)
    fi
  
    CPPFLAGS="$CPPFLAGS_old"
    CXXFLAGS="$CXXFLAGS_old"
  
  fi
  
  if test "x$TestRecorder_ok" == xno ; then
    AC_MSG_RESULT(for TestRecorder: no)
    AC_MSG_WARN(TestRecorder is not found or not properly installed)
  else
    AC_MSG_RESULT(for TestRecorder: yes)
    ENABLE_TESTRECORDER="yes"
  fi
  
  AC_SUBST(TESTRECORDER_INCLUDES)
  AC_SUBST(TESTRECORDER_LIBS)
  
  AC_LANG_RESTORE

  ])
  
])dnl
