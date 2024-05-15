// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef SUIT_FINDACTIONDIALOG_H
#define SUIT_FINDACTIONDIALOG_H

#include "SUIT.h"
#include "SUIT_ShortcutMgr.h"
#include <QDialog>
#include <QFrame>
#include <QTreeWidget>
#include <QList>
#include <QVariant>
#include <memory>
#include <map>
#include <set>
#include <functional>
#include <utility>


class QCheckBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QKeyEvent;
class SUIT_FoundActionTree;


class SUIT_EXPORT SUIT_FindActionDialog : public QDialog
{
  Q_OBJECT

public:
  SUIT_FindActionDialog(QWidget* theParent);
  SUIT_FindActionDialog(const SUIT_FindActionDialog&) = delete;
  SUIT_FindActionDialog& operator=(const SUIT_FindActionDialog&) = delete;
  virtual ~SUIT_FindActionDialog() = default;

  void setActiveModuleID(const QString& theModuleID = SUIT_ShortcutMgr::ROOT_MODULE_ID);

private slots:
  void onQueryChanged(const QString& theKeyword);
  void onSearchOptionUnavailableActionsChanged(int);
  void onSearchOptionInactiveModulesChanged(int);

private:
  void updateUI();

  QLineEdit* myQueryLineEdit;
  QCheckBox* myIncludeUnavailableActionsCB;
  QCheckBox* myIncludeInactiveModulesCB;
  SUIT_FoundActionTree* myFoundActionsTree;

  QString myActiveModuleID;
  SUIT_ActionSearcher myActionSearcher;
};


class SUIT_FoundActionTreeItem;
class SUIT_FoundActionTreeModule;
class SUIT_FoundActionTreeAction;


class SUIT_EXPORT SUIT_FoundActionTree : public QTreeWidget
{
  Q_OBJECT

public:
  enum ElementIdx {
    Name = 0,
    ToolTip = 1
  };

  enum class SortKey {
    MatchMetrics,
    ID,
    Name,
    ToolTip
  };

  enum class SortOrder {
    Ascending,
    Descending
  };

  SUIT_FoundActionTree(SUIT_FindActionDialog* theParent);
  SUIT_FoundActionTree(const SUIT_FoundActionTree&) = delete;
  SUIT_FoundActionTree& operator=(const SUIT_FoundActionTree&) = delete;
  virtual ~SUIT_FoundActionTree() = default;

  void updateItems(const std::map<QString, std::map<QString, SUIT_ActionSearcher::AssetsAndSearchData>>& theAssets);

  void sort(SUIT_FoundActionTree::SortKey theKey, SUIT_FoundActionTree::SortOrder theOrder);

  void keyPressEvent(QKeyEvent* theEvent);

protected:
  bool eventFilter(QObject* theQObject, QEvent* theEvent);

private:
  std::pair<SUIT_FoundActionTreeModule*, int> findModuleItem(const QString& theModuleID) const;
  std::set<SUIT_FoundActionTreeAction*, std::function<bool(SUIT_FoundActionTreeAction*, SUIT_FoundActionTreeAction*)>> createActionSetWithComparator() const;

private slots:
  void onItemExecuted(QTreeWidgetItem* theWidgetItem, int theColIdx);

public:
  static const QList<std::pair<SUIT_FoundActionTree::SortKey, SUIT_FoundActionTree::SortOrder>> DEFAULT_SORT_SCHEMA;

private:
  SUIT_FoundActionTree::SortKey mySortKey;
  SUIT_FoundActionTree::SortOrder mySortOrder;

  /** {moduleID, isExpanded}[] */
  std::map<QString, bool> myModuleItemExpansionStates;
};


class SUIT_FoundActionTreeItem : public QTreeWidgetItem
{
public:
  enum Type {
    Module = 0,
    Action = 1,
  };

protected:
  SUIT_FoundActionTreeItem(const QString& theModuleID);

public:
  virtual ~SUIT_FoundActionTreeItem() = default;
  virtual SUIT_FoundActionTreeItem::Type type() const = 0;

  virtual void setAssetsAndSearchData(const SUIT_ActionSearcher::AssetsAndSearchData& theAssetsAndSD, const QString& theLang) = 0;
  QString name() const;
  QString toolTip() const;

  virtual QVariant getValue(SUIT_FoundActionTree::SortKey theKey) const = 0;

  virtual bool isEnabled() const = 0;

public:
  const QString myModuleID;
};


class SUIT_FoundActionTreeModule : public SUIT_FoundActionTreeItem
{
public:
  SUIT_FoundActionTreeModule(const QString& theModuleID);
  virtual ~SUIT_FoundActionTreeModule() = default;
  virtual SUIT_FoundActionTreeItem::Type type() const { return SUIT_FoundActionTreeItem::Type::Module; };

  /*! \brief Search data is unused. */
  virtual void setAssetsAndSearchData(const SUIT_ActionSearcher::AssetsAndSearchData& theAssetsAndSD, const QString& theLang);

  virtual QVariant getValue(SUIT_FoundActionTree::SortKey theKey) const;

  virtual bool isEnabled() const;
};


class SUIT_FoundActionTreeAction : public SUIT_FoundActionTreeItem
{
private:
  SUIT_FoundActionTreeAction(const QString& theModuleID, const QString& theInModuleActionID);

public:
  static SUIT_FoundActionTreeAction* create(const QString& theModuleID, const QString& theInModuleActionID);
  virtual ~SUIT_FoundActionTreeAction() = default;
  virtual SUIT_FoundActionTreeItem::Type type() const { return SUIT_FoundActionTreeItem::Type::Action; };

  virtual void setAssetsAndSearchData(const SUIT_ActionSearcher::AssetsAndSearchData& theAssetsAndSD, const QString& theLang);

  virtual QVariant getValue(SUIT_FoundActionTree::SortKey theKey) const;
  double matchMetrics() const { return myMatchMetrics; };

  virtual bool isEnabled() const;
  bool isEnabledBufferedValue() const { return myIsEnabledBufferedValue; };

  bool trigger() const;

  const QString myInModuleActionID;

private:
  double myMatchMetrics;
  mutable bool myIsEnabledBufferedValue;
};

#endif // SUIT_FINDACTIONDIALOG_H
