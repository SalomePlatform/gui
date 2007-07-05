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
// File:      QtxFontEdit.h
// Author:    Sergey TELKOV

#ifndef QTXFONTEDIT_H
#define QTXFONTEDIT_H

#include "Qtx.h"

#include <QFrame>

class QtxComboBox;
class QToolButton;
class QFontComboBox;

class QTX_EXPORT QtxFontEdit : public QFrame
{
  Q_OBJECT

public:
  typedef enum { Family    = 0x01,
                 Size      = 0x02,
                 UserSize  = 0x04,
                 Bold      = 0x08,
                 Italic    = 0x10,
                 Underline = 0x20,
                 Preview   = 0x40,
                 Scripting = Bold | Italic | Underline,
                 All = Family | Size | UserSize | Scripting | Preview } Features;

public:
  QtxFontEdit( const int, QWidget* = 0 );
  QtxFontEdit( QWidget* = 0 );
  virtual ~QtxFontEdit();

  QFont          currentFont() const;
  void           setCurrentFont( const QFont& );

  int            fontSize() const;
  QString        fontFamily() const;
  int            fontScripting() const;

  void           setFontSize( const int );
  void           setFontFamily( const QString& );
  void           setFontScripting( const int );

  int            features() const;
  void           setFeatures( const int );

private slots:
  void           onPreview( bool );
  void           onFontChanged( const QFont& );
 
private:
  void           initialize();
  void           updateState();

private:
  QtxComboBox*   mySize;
  QFontComboBox* myFamily;
  QToolButton*   myPreview;
  int            myFeatures;
  QToolButton    *myB, *myI, *myU;
};

#endif // QTXFONTEDIT_H
