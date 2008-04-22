dnl Copyright (C) 2003  CEA/DEN, EDF R&D

AC_DEFUN([CHECK_QWT],[
AC_REQUIRE([CHECK_QT])dnl
AC_REQUIRE([AC_LINKER_OPTIONS])dnl

AC_CHECKING(for qwt)

AC_LANG_SAVE
AC_LANG_CPLUSPLUS

qwt_ok=yes

dnl where is qwt ?

AC_ARG_WITH(qwt,
    [  --with-qwt=DIR     directory path to QWT installation ],
    [QWTHOME="$withval"
      AC_MSG_RESULT("select $withval as path to QWT")
    ])

AC_ARG_WITH(qwt_inc,
    [  --with-qwt_inc=DIR   directory path to QWT includes ],
    [QWT_INCDIR="$withval"
      AC_MSG_RESULT("select $withval as path to QWT includes")
    ])

if test -z $QWTHOME; then
  AC_MSG_RESULT(QWTHOME not defined)
  exist_ok=no	
  if test "x$exist_ok" = "xno"; then
     for d in /usr/local /usr ; do
        AC_CHECK_FILE(${d}/lib${LIB_LOCATION_SUFFIX}/libqwt.so,exist_ok=yes,exist_ok=no)
        if test "x$exist_ok" = "xyes"; then
           QWTHOME=$d
           AC_MSG_RESULT(libqwt.so detected in $d/lib)
        fi
     done
  fi
  if test "x$exist_ok" = "xno"; then
     for d in `echo $LD_LIBRARY_PATH | sed -e "s/:/ /g"` ; do
        if test -f $d/libqwt.so ; then
           AC_MSG_RESULT(libqwt.so detected in $d)
           QWTHOME=$d
           QWTHOME=`echo ${QWTHOME} | sed -e "s,[[^/]]*$,,;s,/$,,;s,^$,.,"`
           exist_ok=yes
           break
        fi
     done
  fi
  if test "x$exist_ok" = "xyes"; then
     if test -z $QWT_INCDIR; then
        QWT_INCDIR=$QWTHOME"/include/qwt"
        if test ! -f $QWT_INCDIR/qwt.h ; then
          QWT_INCDIR=$QWTHOME"/include"
        fi
        if test ! -f $QWT_INCDIR/qwt.h ; then
          QWT_INCDIR=/usr/lib/qt3/include/qwt
        fi
     fi
  else
     qwt_ok=no
  fi
else
  if test -z $QWT_INCDIR; then
     QWT_INCDIR="$QWTHOME/include"
  fi   	
fi

if test "x$qwt_ok" = xno -o ! -d "$QWTHOME" ; then
  AC_MSG_RESULT(no)
  AC_MSG_WARN(qwt not found)
  qwt_ok=no
else
  CPPFLAGS_old=$CPPFLAGS
  CPPFLAGS="$CPPFLAGS $QT_INCLUDES -I$QWT_INCDIR"

  AC_CHECK_HEADER(qwt.h,qwt_ok=yes,qwt_ok=no) 
  CPPFLAGS=$CPPFLAGS_old

  AC_MSG_CHECKING(include of qwt headers)

  if test "x$qwt_ok" = xno ; then
    AC_MSG_RESULT(no)
    AC_MSG_WARN(qwt not found)
  else
    AC_MSG_RESULT(yes)
    QWT_INCLUDES=-I$QWT_INCDIR
  fi

  if test "x$qwt_ok" = "xyes" ; then
    AC_MSG_CHECKING(linking qwt library)

    LIBS_old=$LIBS
    LIBS="$LIBS $QT_LIBS"
    if test "x$QWTHOME" = "x/usr" ; then
      LIBS="$LIBS -lqwt"
    else
      LIBS="$LIBS -L$QWTHOME/lib -lqwt"
    fi

    CXXFLAGS_old=$CXXFLAGS
    CXXFLAGS="$CXXFLAGS $QT_INCLUDES $QWT_INCLUDES"

    AC_CACHE_VAL(salome_cv_lib_qwt,[
      AC_TRY_LINK(
#include <qapplication.h>
#include <qwt_plot.h>
,     int n;
      char **s;
      QApplication a(n, s);
      QwtPlot* p;
      a.setMainWidget(p);
      a.exec();,
      eval "salome_cv_lib_qwt=yes",eval "salome_cv_lib_qwt=no")
    ])
    qwt_ok="$salome_cv_lib_qwt"

    if  test "x$qwt_ok" = "xno" ; then
      AC_MSG_RESULT(unable to link with qwt library)
      AC_MSG_RESULT(QWTHOME environment variable may be wrong)
    else
      AC_MSG_RESULT(yes)
      if test "x$QWTHOME" = "x/usr" ; then
        QWT_LIBS=" -lqwt"
      else
        QWT_LIBS="-L$QWTHOME/lib -lqwt"
      fi
    fi

    LIBS=$LIBS_old
    CXXFLAGS=$CXXFLAGS_old
  fi
fi

AC_SUBST(QWT_INCLUDES)
AC_SUBST(QWT_LIBS)

AC_LANG_RESTORE

AC_MSG_RESULT(for qwt: $qwt_ok)

# Save cache
AC_CACHE_SAVE

])dnl
dnl
