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
// File:      QtxPagePrefMgr.h
// Author:    Sergey TELKOV

#ifndef QTXPAGEPREFMGR_H
#define QTXPAGEPREFMGR_H

#include "QtxPreferenceMgr.h"

#include "QtxPathEdit.h"
#include "QtxPathListEdit.h"

#include <QFrame>
#include <QLabel>
#include <QPointer>

class QtxGridBox;
class QtxFontEdit;
class QtxGroupBox;
class QtxComboBox;
class QtxColorButton;

class QLineEdit;
class QTextEdit;
class QCheckBox;
class QTabWidget;
class QToolButton;
class QListWidget;
class QFileDialog;
class QStackedWidget;

class QTX_EXPORT QtxPagePrefMgr : public QFrame, public QtxPreferenceMgr
{
  Q_OBJECT

public:
  QtxPagePrefMgr( QtxResourceMgr*, QWidget* = 0 );
  virtual ~QtxPagePrefMgr();

  virtual QSize    sizeHint() const;
  virtual QSize    minimumSizeHint() const;

  virtual void     updateContents();

signals:
  void             resourceChanged( int );
  void             resourceChanged( QString&, QString& );
  void             resourcesChanged( const QMap<int, QString>& );

public slots:
  virtual void     setVisible( bool );

protected:
  virtual void     itemAdded( QtxPreferenceItem* );
  virtual void     itemRemoved( QtxPreferenceItem* );
  virtual void     itemChanged( QtxPreferenceItem* );

  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             initialize() const;

private:
  QtxGridBox*      myBox;
  bool             myInit;
};

class QTX_EXPORT QtxPagePrefItem : public QtxPreferenceItem
{
public:
  QtxPagePrefItem( const QString&, QtxPreferenceItem* = 0,
                   const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefItem();

  virtual int       rtti() const;

  QWidget*          widget() const;

  static int        RTTI();

protected:
  void              setWidget( QWidget* );

  virtual void      itemAdded( QtxPreferenceItem* );
  virtual void      itemRemoved( QtxPreferenceItem* );
  virtual void      itemChanged( QtxPreferenceItem* );

  void              pageChildItems( QList<QtxPagePrefItem*>&, const bool = false ) const;

  virtual void      store();
  virtual void      retrieve();

private:
  virtual void      contentChanged();

private:
  QPointer<QWidget> myWidget;
};

class QTX_EXPORT QtxPageNamedPrefItem : public QtxPagePrefItem
{
public:
  QtxPageNamedPrefItem( const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  virtual ~QtxPageNamedPrefItem();

  virtual void      setTitle( const QString& );

protected:
  QLabel*           label() const;
  QWidget*          control() const;

  void              setControl( QWidget* );

private:
  QPointer<QLabel>  myLabel;
  QPointer<QWidget> myControl;
};

class QTX_EXPORT QtxPagePrefListItem : public QObject, public QtxPagePrefItem
{
  Q_OBJECT

public:
  QtxPagePrefListItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefListItem();

  virtual void     updateContents();

  QString          emptyInfo() const;
  void             setEmptyInfo( const QString& );

  bool             isFixedSize() const;
  void             setFixedSize( const bool );

private slots:
  void             onItemSelectionChanged();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             updateInfo();
  void             updateGeom();
  void             updateState();
  void             updateVisible();

  int              selected() const;
  QtxPagePrefItem* selectedItem() const;
  void             setSelected( const int );

private:
  bool             myFix;
  QListWidget*     myList;
  QStackedWidget*  myStack;

  QString          myInfText;
  QLabel*          myInfLabel;
};

class QTX_EXPORT QtxPagePrefTabsItem : public QtxPagePrefItem
{
public:
  QtxPagePrefTabsItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefTabsItem();

  virtual void     updateContents();

  int              tabPosition() const;
  void             setTabPosition( const int );

  int              tabShape() const;
  void             setTabShape( const int );

  QSize            tabIconSize() const;
  void             setTabIconSize( const QSize& );

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             updateTabs();

private:
  QTabWidget*      myTabs;
};

class QTX_EXPORT QtxPagePrefFrameItem : public QtxPagePrefItem
{
public:
  QtxPagePrefFrameItem( const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefFrameItem();

  virtual void     updateContents();

  int              margin() const;
  void             setMargin( const int );

  int              spacing() const;
  void             setSpacing( const int );

  int              columns() const;
  void             setColumns( const int );

  Qt::Orientation  orientation() const;
  void             setOrientation( const Qt::Orientation );

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             updateFrame();

private:
  QtxGridBox*      myBox;
};

class QTX_EXPORT QtxPagePrefGroupItem : public QtxPagePrefItem
{
public:
  QtxPagePrefGroupItem( const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  QtxPagePrefGroupItem( const int, const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefGroupItem();

  virtual void     updateContents();

  int              margin() const;
  void             setMargin( const int );

  int              spacing() const;
  void             setSpacing( const int );

  int              columns() const;
  void             setColumns( const int );

  Qt::Orientation  orientation() const;
  void             setOrientation( const Qt::Orientation );

  bool             isFlat() const;
  void             setFlat( const bool );

  virtual void     setResource( const QString&, const QString& );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             updateState();
  void             updateGroup();

private:
  QtxGridBox*      myBox;
  QtxGroupBox*     myGroup;
};

class QTX_EXPORT QtxPagePrefSpaceItem : public QtxPagePrefItem
{
public:
  QtxPagePrefSpaceItem( QtxPreferenceItem* = 0 );
  QtxPagePrefSpaceItem( Qt::Orientation, QtxPreferenceItem* = 0 );
  QtxPagePrefSpaceItem( const int, const int, QtxPreferenceItem* = 0 );
  virtual ~QtxPagePrefSpaceItem();

  int              size( Qt::Orientation ) const;
  void             setSize( Qt::Orientation, const int );

  int              stretch( Qt::Orientation ) const;
  void             setStretch( Qt::Orientation, const int );

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             initialize( const int, const int, const int, const int );
};

class QTX_EXPORT QtxPagePrefCheckItem : public QtxPagePrefItem
{
public:
  QtxPagePrefCheckItem( const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefCheckItem();

  virtual void     setTitle( const QString& );

  virtual void     store();
  virtual void     retrieve();

private:
  QCheckBox*       myCheck;
};

class QTX_EXPORT QtxPagePrefEditItem : public QtxPageNamedPrefItem
{
public:
  typedef enum { String, Integer, Double } InputType;

public:
  QtxPagePrefEditItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  QtxPagePrefEditItem( const int, const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefEditItem();

  int              inputType() const;
  void             setInputType( const int );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             updateEditor();

private:
  int              myType;
  QLineEdit*       myEditor;
};

class QTX_EXPORT QtxPagePrefSelectItem : public QtxPageNamedPrefItem
{
public:
  typedef enum { NoInput, String, Integer, Double } InputType;

public:
  QtxPagePrefSelectItem( const QString&, QtxPreferenceItem* = 0,
                         const QString& = QString(), const QString& = QString() );
  QtxPagePrefSelectItem( const int, const QString&, QtxPreferenceItem* = 0,
                         const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefSelectItem();

  int              inputType() const;
  void             setInputType( const int );

  QStringList      strings() const;
  QList<int>       numbers() const;

  void             setStrings( const QStringList& );
  void             setNumbers( const QList<int>& );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             updateSelector();
  void             setStrings( const QVariant& );
  void             setNumbers( const QVariant& );

private:
  int              myType;
  QtxComboBox*     mySelector;
};

class QTX_EXPORT QtxPagePrefSpinItem : public QtxPageNamedPrefItem
{
public:
  typedef enum { Integer, Double } InputType;

public:
  QtxPagePrefSpinItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  QtxPagePrefSpinItem( const int, const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefSpinItem();

  QVariant         step() const;
  QVariant         minimum() const;
  QVariant         maximum() const;

  QString          prefix() const;
  QString          suffix() const;
  QString          specialValueText() const;

  void             setStep( const QVariant& );
  void             setMinimum( const QVariant& );
  void             setMaximum( const QVariant& );

  void             setPrefix( const QString& );
  void             setSuffix( const QString& );
  void             setSpecialValueText( const QString& );

  int              inputType() const;
  void             setInputType( const int );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             updateSpinBox();

private:
  int              myType;
};

class QTX_EXPORT QtxPagePrefTextItem : public QtxPageNamedPrefItem
{
public:
  QtxPagePrefTextItem( QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  QtxPagePrefTextItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefTextItem();

  virtual void     store();
  virtual void     retrieve();

private:
  QTextEdit*       myEditor;
};

class QTX_EXPORT QtxPagePrefColorItem : public QtxPageNamedPrefItem
{
public:
  QtxPagePrefColorItem( const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefColorItem();

  virtual void     store();
  virtual void     retrieve();

private:
  QtxColorButton*  myColor;
};

class QTX_EXPORT QtxPagePrefFontItem : public QObject, public QtxPageNamedPrefItem
{
  Q_OBJECT

public:
  QtxPagePrefFontItem( const int, const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  QtxPagePrefFontItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefFontItem();

  int              features() const;
  void             setFeatures( const int );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  QtxFontEdit*     myFont;
};

class QTX_EXPORT QtxPagePrefPathItem : public QtxPageNamedPrefItem
{
public:
  QtxPagePrefPathItem( const Qtx::PathType, const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  QtxPagePrefPathItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefPathItem();

  Qtx::PathType    pathType() const;
  void             setPathType( const Qtx::PathType );

  QString          pathFilter() const;
  void             setPathFilter( const QString& );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );
  
private:
  QtxPathEdit*     myPath;
};

class QTX_EXPORT QtxPagePrefPathsItem : public QtxPageNamedPrefItem
{
public:
  QtxPagePrefPathsItem( QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  QtxPagePrefPathsItem( const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  QtxPagePrefPathsItem( const Qtx::PathType, const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefPathsItem();

  Qtx::PathType    pathType() const;
  void             setPathType( const Qtx::PathType );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  QtxPathListEdit* myPaths;
};

#endif
