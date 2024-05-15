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

#ifndef SUIT_SHORTCUTTREE_H
#define SUIT_SHORTCUTTREE_H

#include "SUIT.h"
#include <QDialog>
#include <QFrame>
#include <QTreeWidget>
#include "SUIT_ShortcutMgr.h"
#include <memory>
#include <map>
#include <set>
#include <functional>


class QLineEdit;
class QLabel;
class QPushButton;
class QTreeWidgetItem;

class SUIT_EXPORT SUIT_KeySequenceEdit : public QFrame
{
  Q_OBJECT

public:
  SUIT_KeySequenceEdit(QWidget* = nullptr);
  virtual ~SUIT_KeySequenceEdit() = default;

  void           setConfirmedKeySequence(const QKeySequence&);
  void           setEditedKeySequence(const QKeySequence&);
  QKeySequence   editedKeySequence() const;
  bool           isKeySequenceModified() const;
  void           restoreKeySequence();

  static QString parseEvent(QKeyEvent*);
  static bool    isValidKey(int);

signals:
  void           editingStarted();
  void           editingFinished();
  void           restoreFromShortcutMgrClicked();

private slots:
  void           onClear();
  void           onEditingFinished();

protected:
  virtual bool   eventFilter(QObject*, QEvent*);

private:
  void           initialize();

private:
  QLineEdit*     myKeySequenceLineEdit;
  QString        myConfirmedKeySequenceString;

  // Last valid key sequence string from myKeySequenceLineEdit.
  QString        myPrevKeySequenceString;
};


class SUIT_ShortcutTree;
class SUIT_ShortcutTreeItem;
class SUIT_ShortcutTreeFolder;
class SUIT_ShortcutTreeAction;
class QTextEdit;


class SUIT_EXPORT SUIT_EditKeySequenceDialog : public QDialog
{
  Q_OBJECT

public:
  SUIT_EditKeySequenceDialog(SUIT_ShortcutTree* theParent);
  SUIT_EditKeySequenceDialog(const SUIT_EditKeySequenceDialog&) = delete;
  SUIT_EditKeySequenceDialog& operator=(const SUIT_EditKeySequenceDialog&) = delete;
  virtual ~SUIT_EditKeySequenceDialog() = default;

  void setModuleAndActionID(const QString& theModuleID, const QString& theInModuleActionID);
  const QString& moduleID() const;
  const QString& inModuleActionID() const;

  void setModuleAndActionName(const QString& theModuleName, const QString& theActionName, const QString& theActionToolTip = "");

  void setConfirmedKeySequence(const QKeySequence& theSequence);
  QKeySequence editedKeySequence() const;

  void updateConflictsMessage();

  int exec();

private slots:
  void onEditingStarted();
  void onEditingFinished();
  void onRestoreFromShortcutMgr();
  void onConfirm();

private:
  QString myModuleID;
  QString myInModuleActionID;
  QLabel* myActionName;
  SUIT_KeySequenceEdit* myKeySequenceEdit;
  QTextEdit* myTextEdit;
};


class SUIT_EXPORT SUIT_ShortcutTree : public QTreeWidget
{
  Q_OBJECT

public:
  enum ElementIdx {
    Name = 0,
    KeySequence = 1, // Empty, if item is folder item.
  };

  enum class SortKey {
    ID,
    Name,
    ToolTip,
    KeySequence,
  };

  enum class SortOrder {
    Ascending,
    Descending
  };

  SUIT_ShortcutTree(
    std::shared_ptr<SUIT_ShortcutContainer> theContainer = std::shared_ptr<SUIT_ShortcutContainer>(),
    QWidget* theParent = nullptr
  );
  SUIT_ShortcutTree(const SUIT_ShortcutTree&) = delete;
  SUIT_ShortcutTree& operator=(const SUIT_ShortcutTree&) = delete;
  virtual ~SUIT_ShortcutTree();

  void setShortcutsFromManager();
  void setDefaultShortcuts();
  void applyChangesToShortcutMgr();

  std::shared_ptr<const SUIT_ShortcutContainer> shortcutContainer() const;

  void sort(SUIT_ShortcutTree::SortKey theKey, SUIT_ShortcutTree::SortOrder theOrder);

private:
  void updateItems(bool theHighlightModified, bool theUpdateSyncTrees);
  std::pair<SUIT_ShortcutTreeFolder*, int> findModuleFolderItem(const QString& theModuleID) const;

  std::set<SUIT_ShortcutTreeItem*, std::function<bool(SUIT_ShortcutTreeItem*, SUIT_ShortcutTreeItem*)>> getSortedChildren(SUIT_ShortcutTreeFolder* theParentItem);

  void insertChild(
    SUIT_ShortcutTreeFolder* theParentItem,
    std::set<SUIT_ShortcutTreeItem*, std::function<bool(SUIT_ShortcutTreeItem*, SUIT_ShortcutTreeItem*)>>& theSortedChildren,
    SUIT_ShortcutTreeItem* theChildItem
  );

private slots:
  void onItemDoubleClicked(QTreeWidgetItem* theWidgetItem, int theColIdx);

public:
  /** Keeps IDs of modules, which will are shown on setShortcutsFromManager(). */
  std::set<QString> myModuleIDs;

  static const QList<std::pair<SUIT_ShortcutTree::SortKey, SUIT_ShortcutTree::SortOrder>> DEFAULT_SORT_SCHEMA;

private:
  /** Allows to modify plenty of shortcuts and then apply them to SUIT_ShortcutMgr as a batch. */
  const std::shared_ptr<SUIT_ShortcutContainer> myShortcutContainer;

  SUIT_EditKeySequenceDialog* myEditDialog;

  SUIT_ShortcutTree::SortKey mySortKey;
  SUIT_ShortcutTree::SortOrder mySortOrder;

  /**
   * Ensures that, if several SUIT_ShortcutTree instances coexist,
   * all of them are updated when one of them applies pending changes to SUIT_ShortcutMgr.
   *
   * Sharing of SUIT_ShortcutContainer allows to keep some trees synchronized even without
   * applying changes to SUIT_ShortcutMgr. Why? See SUIT_PagePrefShortcutTreeItem.
   *
   * Access is not synchronized in assumption, that all instances live in the same thread.
  */
  static std::map<SUIT_ShortcutContainer*, std::set<SUIT_ShortcutTree*>> instances;
};


class SUIT_ShortcutTreeItem : public QTreeWidgetItem
{
public:
  enum Type {
    Folder = 0,
    Action = 1,
  };

protected:
  SUIT_ShortcutTreeItem(const QString& theModuleID);

public:
  virtual ~SUIT_ShortcutTreeItem() = default;
  virtual SUIT_ShortcutTreeItem::Type type() const = 0;

  virtual void setAssets(std::shared_ptr<const SUIT_ActionAssets> theAssets, const QString& theLang) = 0;
  QString name() const;

  virtual QString getValue(SUIT_ShortcutTree::SortKey theKey) const = 0;

public:
  const QString myModuleID;
};


class SUIT_ShortcutTreeFolder : public SUIT_ShortcutTreeItem
{
public:
  SUIT_ShortcutTreeFolder(const QString& theModuleID);
  virtual ~SUIT_ShortcutTreeFolder() = default;
  virtual SUIT_ShortcutTreeItem::Type type() const { return SUIT_ShortcutTreeItem::Type::Folder; };

  virtual void setAssets(std::shared_ptr<const SUIT_ActionAssets> theAssets, const QString& theLang);

  virtual QString getValue(SUIT_ShortcutTree::SortKey theKey) const;
};


class SUIT_ShortcutTreeAction : public SUIT_ShortcutTreeItem
{
private:
  SUIT_ShortcutTreeAction(const QString& theModuleID, const QString& theInModuleActionID);

public:
  static SUIT_ShortcutTreeAction* create(const QString& theModuleID, const QString& theInModuleActionID);
  virtual ~SUIT_ShortcutTreeAction() = default;
  virtual SUIT_ShortcutTreeItem::Type type() const { return SUIT_ShortcutTreeItem::Type::Action; };

  virtual void setAssets(std::shared_ptr<const SUIT_ActionAssets> theAssets, const QString& theLang);

  virtual QString getValue(SUIT_ShortcutTree::SortKey theKey) const;

  void setKeySequence(const QString& theKeySequence);
  QString keySequence() const;
  void highlightKeySequenceAsModified(bool theHighlight);

  const QString myInModuleActionID;
};

#endif // SUIT_SHORTCUTTREE_H
