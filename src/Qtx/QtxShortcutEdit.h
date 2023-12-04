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

#ifndef QTXSHORTCUTTREE_H
#define QTXSHORTCUTTREE_H

#include "Qtx.h"
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

class QTX_EXPORT QtxKeySequenceEdit : public QFrame
{
  Q_OBJECT

public:
  QtxKeySequenceEdit(QWidget* = nullptr);
  virtual ~QtxKeySequenceEdit() = default;

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


class QtxShortcutTree;
class QtxShortcutTreeItem;
class QtxShortcutTreeFolder;
class QtxShortcutTreeAction;
class QTextEdit;


class QTX_EXPORT QtxEditKeySequenceDialog : public QDialog
{
  Q_OBJECT

public:
  QtxEditKeySequenceDialog(QtxShortcutTree* theParent);
  QtxEditKeySequenceDialog(const QtxEditKeySequenceDialog&) = delete;
  QtxEditKeySequenceDialog& operator=(const QtxEditKeySequenceDialog&) = delete;
  virtual ~QtxEditKeySequenceDialog() = default;

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
  QtxKeySequenceEdit* myKeySequenceEdit;
  QTextEdit* myTextEdit;
};


class QTX_EXPORT QtxShortcutTree : public QTreeWidget
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

  QtxShortcutTree(
    std::shared_ptr<SUIT_ShortcutContainer> theContainer = std::shared_ptr<SUIT_ShortcutContainer>(),
    QWidget* theParent = nullptr
  );
  QtxShortcutTree(const QtxShortcutTree&) = delete;
  QtxShortcutTree& operator=(const QtxShortcutTree&) = delete;
  virtual ~QtxShortcutTree();

  void setShortcutsFromManager();
  void setDefaultShortcuts();
  void applyChangesToShortcutMgr();

  std::shared_ptr<const SUIT_ShortcutContainer> shortcutContainer() const;

  void sort(QtxShortcutTree::SortKey theKey, QtxShortcutTree::SortOrder theOrder);

private:
  void updateItems(bool theHighlightModified, bool theUpdateSyncTrees);
  std::pair<QtxShortcutTreeFolder*, int> findModuleFolderItem(const QString& theModuleID) const;

  std::set<QtxShortcutTreeItem*, std::function<bool(QtxShortcutTreeItem*, QtxShortcutTreeItem*)>> getSortedChildren(QtxShortcutTreeFolder* theParentItem);

  void insertChild(
    QtxShortcutTreeFolder* theParentItem,
    std::set<QtxShortcutTreeItem*, std::function<bool(QtxShortcutTreeItem*, QtxShortcutTreeItem*)>>& theSortedChildren,
    QtxShortcutTreeItem* theChildItem
  );

private slots:
  void onItemDoubleClicked(QTreeWidgetItem* theWidgetItem, int theColIdx);

public:
  /** Keeps IDs of modules, which will are shown on setShortcutsFromManager(). */
  std::set<QString> myModuleIDs;

  static const QList<std::pair<QtxShortcutTree::SortKey, QtxShortcutTree::SortOrder>> DEFAULT_SORT_SCHEMA;

private:
  /** Allows to modify plenty of shortcuts and then apply them to SUIT_ShortcutMgr as a batch. */
  const std::shared_ptr<SUIT_ShortcutContainer> myShortcutContainer;

  QtxEditKeySequenceDialog* myEditDialog;

  QtxShortcutTree::SortKey mySortKey;
  QtxShortcutTree::SortOrder mySortOrder;

  /**
   * Ensures that, if several QtxShortcutTree instances coexist,
   * all of them are updated when one of them applies pending changes to SUIT_ShortcutMgr.
   *
   * Sharing of SUIT_ShortcutContainer allows to keep some trees synchronized even without
   * applying changes to SUIT_ShortcutMgr. Why? See QtxPagePrefShortcutTreeItem.
   *
   * Access is not synchronized in assumption, that all instances live in the same thread.
  */
  static std::map<SUIT_ShortcutContainer*, std::set<QtxShortcutTree*>> instances;
};


class QtxShortcutTreeItem : public QTreeWidgetItem
{
public:
  enum Type {
    Folder = 0,
    Action = 1,
  };

protected:
  QtxShortcutTreeItem(const QString& theModuleID);

public:
  virtual ~QtxShortcutTreeItem() = default;
  virtual QtxShortcutTreeItem::Type type() const = 0;

  virtual void setAssets(std::shared_ptr<const SUIT_ActionAssets> theAssets, const QString& theLang) = 0;
  QString name() const;

  virtual QString getValue(QtxShortcutTree::SortKey theKey) const = 0;

public:
  const QString myModuleID;
};


class QtxShortcutTreeFolder : public QtxShortcutTreeItem
{
public:
  QtxShortcutTreeFolder(const QString& theModuleID);
  virtual ~QtxShortcutTreeFolder() = default;
  virtual QtxShortcutTreeItem::Type type() const { return QtxShortcutTreeItem::Type::Folder; };

  virtual void setAssets(std::shared_ptr<const SUIT_ActionAssets> theAssets, const QString& theLang);

  virtual QString getValue(QtxShortcutTree::SortKey theKey) const;
};


class QtxShortcutTreeAction : public QtxShortcutTreeItem
{
private:
  QtxShortcutTreeAction(const QString& theModuleID, const QString& theInModuleActionID);

public:
  static QtxShortcutTreeAction* create(const QString& theModuleID, const QString& theInModuleActionID);
  virtual ~QtxShortcutTreeAction() = default;
  virtual QtxShortcutTreeItem::Type type() const { return QtxShortcutTreeItem::Type::Action; };

  virtual void setAssets(std::shared_ptr<const SUIT_ActionAssets> theAssets, const QString& theLang);

  virtual QString getValue(QtxShortcutTree::SortKey theKey) const;

  void setKeySequence(const QString& theKeySequence);
  QString keySequence() const;
  void highlightKeySequenceAsModified(bool theHighlight);

  const QString myInModuleActionID;
};

#endif // QTXSHORTCUTTREE_H
