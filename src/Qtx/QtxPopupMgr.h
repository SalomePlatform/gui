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

#ifndef QTXPOPUPMGR_H
#define QTXPOPUPMGR_H

#include "QtxActionMenuMgr.h"

#include <QMap>
#include <QVariant>

class QtxEvalExpr;
class QtxEvalParser;
class QtxPopupSelection;

class QTX_EXPORT QtxPopupMgr : public QtxActionMenuMgr
{
  Q_OBJECT

public:
  //! Menu item rule type
  typedef enum { 
    VisibleRule,   //!< menu item visibility state
    EnableRule,    //!< menu item enable state
    ToggleRule     //!< menu item toggle state
  } RuleType;

private:
  class PopupCreator;

public:
  QtxPopupMgr( QObject* = 0 );
  QtxPopupMgr( QMenu*, QObject* = 0 );
  virtual ~QtxPopupMgr();

  int                insert( const int, const int, const QString&, const RuleType = VisibleRule );
  int                insert( QAction*, const int, const QString&, const RuleType = VisibleRule );

  virtual int        registerAction( QAction*, const int, const QString& rule,
                                     const RuleType = VisibleRule );
  virtual void       unRegisterAction( const int );

  virtual bool       isVisible( const int actId, const int place ) const;

  QString            rule( QAction*, const RuleType = VisibleRule ) const;
  QString            rule( const int, const RuleType = VisibleRule ) const;

  void               setRule( QAction*, const QString&, const RuleType = VisibleRule );
  void               setRule( const int, const QString&, const RuleType = VisibleRule );

  QtxPopupSelection* selection() const;
  void               setSelection( QtxPopupSelection* );

  QMenu*             menu() const;
  void               setMenu( QMenu* );

  void               updateMenu();

  virtual bool       load( const QString&, QtxActionMgr::Reader& );

protected:
  virtual void       internalUpdate();
  void               setParameters( QtxEvalParser*, QStringList& ) const;
  virtual bool       isSatisfied( QAction*, const RuleType = VisibleRule ) const;
  QtxEvalExpr*       expression( QAction*, const RuleType = VisibleRule, const bool = false ) const;

private:
  bool               result( QtxEvalParser* p ) const;
  QVariant           parameter( const QString&, const int = -1 ) const;

private:
  typedef QMap<RuleType, QtxEvalExpr*> ExprMap;
  typedef QMap<QAction*, ExprMap>      RuleMap;
  typedef QMap<QString, QVariant>      CacheMap;

private:
  RuleMap            myRules;
  CacheMap           myCache;
  QtxPopupSelection* mySelection;
};

class QTX_EXPORT QtxPopupSelection : public QObject
{
  Q_OBJECT

public:
  QtxPopupSelection();
  virtual ~QtxPopupSelection();

  virtual int        count() const = 0;
  virtual QVariant   parameter( const QString& ) const;
  virtual QVariant   parameter( const int, const QString& ) const = 0;

  QString            option( const QString& ) const;
  void               setOption( const QString&, const QString& );

private:
  QString            equalityParam() const;
  QString            selCountParam() const;

private:
  typedef QMap<QString, QString> OptionsMap;

private:
  OptionsMap         myOptions;
};

#endif
