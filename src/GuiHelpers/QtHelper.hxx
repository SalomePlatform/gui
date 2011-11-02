#ifndef _QT_HELPER_HXX_
#define _QT_HELPER_HXX_

//
// =============================================================
// Macro and template functions for type conversions.
// =============================================================
//
#include "Basics_Utils.hxx" // To get ToString macro
#include <QString>

// This can be used to convert QString into char *.
#define QCHARSTAR(qstring) qstring.toLatin1().data()
// This converts a string into a QString
#define S2QS(string) QString(string.c_str())
// This converts a QString into a string
#define QS2S(qstring) ToString(QCHARSTAR(qstring))
#define QSTRING(any) S2QS(ToString(any))

//
// =============================================================
// General helper macros for manipulating widgets values
// =============================================================
//
#define GETTEXT(widget) ToString(QCHARSTAR(widget->text().trimmed()))
#define SETTEXT(widget,str) widget->setText(str.c_str())

//
// =============================================================
// Qt Logger macros
// =============================================================
//

// We can use the macros defined by SALOMELocalTrace/utilities.h
// as proposed in the file KERNEL_helper.hxx. But we can use instead
// the Qt debug function:
#include <QDebug>
#define QDEBUG qWarning()
//#define QDEBUG qDebug()
#define QLOG(data) QDEBUG << "[XSALOME] " << data
#ifdef LOG
#undef LOG
#endif
#define LOG QLOG

#endif // __QT_HELPER_H_
