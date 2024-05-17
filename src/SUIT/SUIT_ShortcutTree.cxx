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

#include "SUIT_ShortcutTree.h"

#include <QWidget>
#include <QLayout>
#include <QList>
#include <QMap>

#include <QToolButton>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QBrush>
#include <QColor>
#include <QHeaderView>

#include <QKeyEvent>
#include <QKeySequence>
#include <QCollator>

#include <algorithm>


#define COLUMN_SIZE  500


SUIT_KeySequenceEdit::SUIT_KeySequenceEdit(QWidget* parent)
: QFrame(parent)
{
  initialize();
  myKeySequenceLineEdit->installEventFilter(this);
}

/*! \brief Set a key sequence to edit. */
void SUIT_KeySequenceEdit::setConfirmedKeySequence(const QKeySequence& theKeySequence)
{
  myConfirmedKeySequenceString = theKeySequence.toString();
  myKeySequenceLineEdit->setText(myConfirmedKeySequenceString);
  myPrevKeySequenceString = myConfirmedKeySequenceString;
}

void SUIT_KeySequenceEdit::setEditedKeySequence(const QKeySequence& theKeySequence)
{
  const QString keySequenceString = theKeySequence.toString();
  myKeySequenceLineEdit->setText(keySequenceString);
  myPrevKeySequenceString = keySequenceString;
}

QKeySequence SUIT_KeySequenceEdit::editedKeySequence() const
{
  return QKeySequence::fromString(myKeySequenceLineEdit->text());
}

/*! \returns true, if the edited key sequence differs from confirmed one. */
bool SUIT_KeySequenceEdit::isKeySequenceModified() const
{
  return QKeySequence(myConfirmedKeySequenceString) != editedKeySequence();
}

/*! \brief Set confirmed key sequence to line editor. */
void SUIT_KeySequenceEdit::restoreKeySequence()
{
  myKeySequenceLineEdit->setText(myConfirmedKeySequenceString);
  myPrevKeySequenceString = myConfirmedKeySequenceString;
}

/*!
  \brief Gets the key sequence from keys that were pressed
  \param e a key event
  \returns a string representation of the key sequence
*/
/*static*/ QString SUIT_KeySequenceEdit::parseEvent(QKeyEvent* e)
{
  bool isShiftPressed = e->modifiers() & Qt::ShiftModifier;
  bool isControlPressed = e->modifiers() & Qt::ControlModifier;
  bool isAltPressed = e->modifiers() & Qt::AltModifier;
  bool isMetaPressed = e->modifiers() & Qt::MetaModifier;
  bool isModifiersPressed = isControlPressed || isAltPressed || isMetaPressed; // Do not treat Shift alone as a modifier!
  int result=0;
  if(isControlPressed)
    result += Qt::CTRL;
  if(isAltPressed)
    result += Qt::ALT;
  if(isShiftPressed)
    result += Qt::SHIFT;
  if(isMetaPressed)
    result += Qt::META;

  int aKey = e->key();
  if ((isValidKey(aKey) && isModifiersPressed) || ((aKey >= Qt::Key_F1) && (aKey <= Qt::Key_F12)))
    result += aKey;

  return QKeySequence(result).toString();
}

/*!
  \brief Check if the key event contains a 'valid' key
  \param theKey the code of the key
  \returns \c true if the key is 'valid'
*/
/*static*/ bool SUIT_KeySequenceEdit::isValidKey(int theKey)
{
  if ( theKey == Qt::Key_Underscore || theKey == Qt::Key_Escape ||
     ( theKey >= Qt::Key_Backspace && theKey <= Qt::Key_Delete ) ||
     ( theKey >= Qt::Key_Home && theKey <= Qt::Key_PageDown ) ||
     ( theKey >= Qt::Key_F1 && theKey <= Qt::Key_F12 )  ||
     ( theKey >= Qt::Key_Space && theKey <= Qt::Key_Asterisk ) ||
     ( theKey >= Qt::Key_Comma && theKey <= Qt::Key_AsciiTilde ) )
    return true;
  return false;
}

/*! \brief Called when "Clear" button is clicked. */
void SUIT_KeySequenceEdit::onClear()
{
  myKeySequenceLineEdit->setText("");
  myPrevKeySequenceString = "";
  emit editingFinished();
}

/*! \brief Called when myKeySequenceLineEdit loses focus. */
void SUIT_KeySequenceEdit::onEditingFinished()
{
  if (myKeySequenceLineEdit->text().endsWith("+"))
    myKeySequenceLineEdit->setText(myPrevKeySequenceString);
  else
    myPrevKeySequenceString = myKeySequenceLineEdit->text();
    emit editingFinished();
}

/*!
  \brief Custom event filter.
  \param obj event receiver object
  \param event event
  \returns \c true if further event processing should be stopped
*/
bool SUIT_KeySequenceEdit::eventFilter(QObject* theObject, QEvent* theEvent)
{
  if (theObject == myKeySequenceLineEdit) {
    if (theEvent->type() == QEvent::KeyPress) {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(theEvent);
      QString text = parseEvent(keyEvent);
      if (keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace)
        myKeySequenceLineEdit->setText("");
      if (!text.isEmpty())
        myKeySequenceLineEdit->setText(text);

      emit editingStarted();
      return true;
    }
    if (theEvent->type() == QEvent::KeyRelease) {
      onEditingFinished();
      return true;
    }
  }
  return false;
}

/*
  \brief Perform internal intialization.
*/
void SUIT_KeySequenceEdit::initialize()
{
  static const int PIXMAP_SIZE = 30;

  QHBoxLayout* base = new QHBoxLayout( this );
  base->setMargin(0);
  base->setSpacing(5);

  base->addWidget(myKeySequenceLineEdit = new QLineEdit(this));
  setFocusProxy(myKeySequenceLineEdit);

  QToolButton* clearBtn = new QToolButton();
  auto clearPixmap = QPixmap(":/images/shortcut_disable.svg");
  clearPixmap.scaled(QSize(PIXMAP_SIZE, PIXMAP_SIZE), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  clearBtn->setIcon(clearPixmap);
  clearBtn->setToolTip(tr("Disable shortcut."));
  base->addWidget(clearBtn);

  QToolButton* restoreBtn = new QToolButton();
  auto restorePixmap = QPixmap(":/images/shortcut_restore.svg");
  restorePixmap.scaled(QSize(PIXMAP_SIZE, PIXMAP_SIZE), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  restoreBtn->setIcon(restorePixmap);
  restoreBtn->setToolTip(tr("Restore the currently applied key sequence."));
  base->addWidget(restoreBtn);

  myKeySequenceLineEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  clearBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  restoreBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  connect(clearBtn, SIGNAL(clicked()), this, SLOT(onClear()));
  connect(restoreBtn, SIGNAL(clicked()), this, SIGNAL(restoreFromShortcutMgrClicked()));
  connect(myKeySequenceLineEdit, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
}


/*! \param theParent must not be nullptr. */
SUIT_EditKeySequenceDialog::SUIT_EditKeySequenceDialog(SUIT_ShortcutTree* theParent)
: QDialog(theParent)
{
  setMinimumWidth(500);
  setWindowTitle(tr("Change key sequence"));
  QVBoxLayout* layout = new QVBoxLayout(this);
  myActionName = new QLabel(this);
  myActionName->setTextFormat(Qt::RichText);
  myKeySequenceEdit = new SUIT_KeySequenceEdit(this);
  myTextEdit = new QTextEdit(this);
  layout->addWidget(myActionName);
  layout->addWidget(myKeySequenceEdit);
  layout->addWidget(myTextEdit);
  myActionName->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  myKeySequenceEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  myTextEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  myTextEdit->setReadOnly(true);
  myTextEdit->setAcceptRichText(true);
  myTextEdit->setPlaceholderText(tr("No conflicts."));
  setFocusProxy(myKeySequenceEdit);

  QHBoxLayout* buttonLayout = new QHBoxLayout(this);
  layout->addLayout(buttonLayout);
  QPushButton* confirmButton = new QPushButton(tr("Confirm"), this);
  confirmButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
  cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  buttonLayout->addStretch();
  buttonLayout->addWidget(confirmButton);
  buttonLayout->addWidget(cancelButton);

  connect(myKeySequenceEdit, SIGNAL(editingStarted()), this, SLOT(onEditingStarted()));
  connect(myKeySequenceEdit, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
  connect(myKeySequenceEdit, SIGNAL(restoreFromShortcutMgrClicked()), this, SLOT(onRestoreFromShortcutMgr()));
  connect(confirmButton, SIGNAL(clicked()), this, SLOT(onConfirm()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void SUIT_EditKeySequenceDialog::setModuleAndActionID(const QString& theModuleID, const QString& theInModuleActionID)
{
  myModuleID = theModuleID;
  myInModuleActionID = theInModuleActionID;
}

const QString& SUIT_EditKeySequenceDialog::moduleID() const { return myModuleID; }
const QString& SUIT_EditKeySequenceDialog::inModuleActionID() const { return myInModuleActionID; }

void SUIT_EditKeySequenceDialog::setModuleAndActionName(const QString& theModuleName, const QString& theActionName, const QString& theActionToolTip)
{
  myActionName->setText("<b>" + theModuleName + "</b>&nbsp;&nbsp;" + theActionName);
  myActionName->setToolTip(theActionToolTip);
}

void SUIT_EditKeySequenceDialog::setConfirmedKeySequence(const QKeySequence& theSequence)
{
  myKeySequenceEdit->setConfirmedKeySequence(theSequence);
}

QKeySequence SUIT_EditKeySequenceDialog::editedKeySequence() const
{
  return myKeySequenceEdit->editedKeySequence();
}

int SUIT_EditKeySequenceDialog::exec()
{
  myKeySequenceEdit->setFocus(Qt::ActiveWindowFocusReason);
  return QDialog::exec();
}

void SUIT_EditKeySequenceDialog::onEditingStarted()
{
  myTextEdit->setEnabled(false);
}

void SUIT_EditKeySequenceDialog::onEditingFinished()
{
  updateConflictsMessage();
}

void SUIT_EditKeySequenceDialog::onRestoreFromShortcutMgr()
{
  const auto shortcutMgr = SUIT_ShortcutMgr::get();
  myKeySequenceEdit->setEditedKeySequence(shortcutMgr->getKeySequence(myModuleID, myInModuleActionID));
  updateConflictsMessage();
}

/*! Updates message with list of actions, whose shortcuts will be disabled on Confirm. */
void SUIT_EditKeySequenceDialog::updateConflictsMessage()
{
  myTextEdit->setEnabled(true);
  QTextDocument* doc = myTextEdit->document();
  if (!doc) {
    doc = new QTextDocument(myTextEdit);
    myTextEdit->setDocument(doc);
  }

  if (!myKeySequenceEdit->isKeySequenceModified()) {
    doc->clear();
    return;
  }

  const QKeySequence newKeySequence = editedKeySequence();

  const auto shortcutTree = static_cast<SUIT_ShortcutTree*>(parentWidget());
  /** {moduleID, inModuleActionID}[] */
  std::set<std::pair<QString, QString>> conflicts = shortcutTree->shortcutContainer()->getConflicts(myModuleID, myInModuleActionID, newKeySequence);
  if (!conflicts.empty()) {
    const auto shortcutMgr = SUIT_ShortcutMgr::get();

    QString report = "<b>" + tr("These shortcuts will be disabled on confirm:") + "</b>";
    {
      report += "<ul>";
      for (const auto& conflict : conflicts) {
        const QString conflictingModuleName = shortcutMgr->getModuleName(conflict.first);
        const QString conflictingActionName = shortcutMgr->getActionName(conflict.first, conflict.second);
        report += "<li><b>" + conflictingModuleName + "</b>&nbsp;&nbsp;" + conflictingActionName + "</li>";
      }
      report += "</ul>";
    }
    doc->setHtml(report);
  }
  else /* if no conflicts */ {
    doc->clear();
  }
}

void SUIT_EditKeySequenceDialog::onConfirm()
{
  if (myKeySequenceEdit->isKeySequenceModified())
    accept();
  else
    reject();
}


/*! \brief Compensates lack of std::distance(), which is introduced in C++17.
\returns -1, if theIt does not belong to the  */
template <class Container>
size_t indexOf(
  const Container& theContainer,
  const typename Container::iterator& theIt
) {
  auto it = theContainer.begin();
  size_t distance = 0;
  while (it != theContainer.end()) {
    if (it == theIt)
      return distance;

    it++;
    distance++;
  }
  return -1;
}


/*! \param theContainer Share the same container between several trees,
to edit them synchronously even without exchange of changes with SUIT_ShortcutMgr.
Pass nullptr to create non-synchronized tree. */
SUIT_ShortcutTree::SUIT_ShortcutTree(
  std::shared_ptr<SUIT_ShortcutContainer> theContainer,
  QWidget* theParent
) : QTreeWidget(theParent),
myShortcutContainer(theContainer ? theContainer : std::shared_ptr<SUIT_ShortcutContainer>(new SUIT_ShortcutContainer())),
mySortKey(SUIT_ShortcutTree::SortKey::Name), mySortOrder(SUIT_ShortcutTree::SortOrder::Ascending)
{
  setColumnCount(2);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setColumnWidth(0, COLUMN_SIZE);
  setSortingEnabled(false); // Items are sorted in the same way, as in ShortcutContainer.
  header()->setSectionResizeMode(QHeaderView::Interactive);
  {
    QMap<int, QString> labelMap;
    labelMap[SUIT_ShortcutTree::ElementIdx::Name]        = tr("Action");
    labelMap[SUIT_ShortcutTree::ElementIdx::KeySequence] = tr("Key sequence");
    setHeaderLabels(labelMap.values());
  }
  setExpandsOnDoubleClick(false); // Open shortcut editor on double click instead.
  setSortingEnabled(false);
  setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  myEditDialog = new SUIT_EditKeySequenceDialog(this);

  this->installEventFilter(this);
  connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

  SUIT_ShortcutTree::instances[myShortcutContainer.get()].emplace(this);
}

SUIT_ShortcutTree::~SUIT_ShortcutTree()
{
  SUIT_ShortcutTree::instances[myShortcutContainer.get()].erase(this);
  if (SUIT_ShortcutTree::instances[myShortcutContainer.get()].empty())
    SUIT_ShortcutTree::instances.erase(myShortcutContainer.get());
}

/*! \brief Copies shortcuts from ShortcutMgr. (Re)displays shortcuts of myModuleIDs. */
void SUIT_ShortcutTree::setShortcutsFromManager()
{
  const auto shortcutMgr = SUIT_ShortcutMgr::get();
  *myShortcutContainer = shortcutMgr->getShortcutContainer();
  // nb! ShortcutMgr never removes shortcuts from its container, only disables.

  updateItems(false /*theHighlightModified*/, true /*theUpdateSyncTrees*/);
}

/*! \brief Copies shortcuts from resources, user files are not accounted. (Re)displays shortcuts of myModuleIDs. */
void SUIT_ShortcutTree::setDefaultShortcuts()
{
  SUIT_ShortcutContainer defaultShortcuts;
  SUIT_ShortcutMgr::fillContainerFromPreferences(defaultShortcuts, true /*theDefaultOnly*/);

  myShortcutContainer->merge(defaultShortcuts, true /*theOverride*/, true /*theTreatAbsentIncomingAsDisabled*/);
  // nb! SUIT_ShortcutContainer never erases shortcuts, only disables.

  updateItems(true /*theHighlightModified*/, true /*theUpdateSyncTrees*/);
}

/*! \brief Applies pending changes to ShortcutMgr. Updates other instances of SUIT_ShortcutTree. */
void SUIT_ShortcutTree::applyChangesToShortcutMgr()
{
  const auto mgr = SUIT_ShortcutMgr::get();
  mgr->mergeShortcutContainer(*myShortcutContainer);

  // Update non-synchronized with this instances.
  for (const auto& containerAndSyncTrees : SUIT_ShortcutTree::instances) {
    if (containerAndSyncTrees.first == myShortcutContainer.get())
      continue;

    const std::set<SUIT_ShortcutTree*>& syncTrees = containerAndSyncTrees.second;
    const auto itFirstSyncTree = syncTrees.begin();
    if (itFirstSyncTree == syncTrees.end())
      continue;

    (*itFirstSyncTree)->setShortcutsFromManager();
    const auto editDialog = (*itFirstSyncTree)->myEditDialog;
    editDialog->setConfirmedKeySequence(mgr->getShortcutContainer().getKeySequence(editDialog->moduleID(), editDialog->inModuleActionID()));
    editDialog->updateConflictsMessage();
  }
}

std::shared_ptr<const SUIT_ShortcutContainer> SUIT_ShortcutTree::shortcutContainer() const
{
  return myShortcutContainer;
}

/*! \brief Does not sort modules. */
void SUIT_ShortcutTree::sort(SUIT_ShortcutTree::SortKey theKey, SUIT_ShortcutTree::SortOrder theOrder)
{
  if (theKey == mySortKey && theOrder == mySortOrder)
    return;

  mySortKey = theKey;
  mySortOrder = theOrder;

  for (int moduleIdx = 0; moduleIdx < topLevelItemCount(); moduleIdx++) {
    const auto moduleItem = static_cast<SUIT_ShortcutTreeFolder*>(topLevelItem(moduleIdx));
    const auto sortedChildren = getSortedChildren(moduleItem);
    moduleItem->takeChildren();

    for (const auto childItem : sortedChildren) {
      moduleItem->addChild(childItem);
    }
  }
}

/*! \param If theUpdateSyncTrees, trees sharing the same shortcut container are updated. */
void SUIT_ShortcutTree::updateItems(bool theHighlightModified, bool theUpdateSyncTrees)
{
  const auto shortcutMgr = SUIT_ShortcutMgr::get();
  const QString lang = SUIT_ShortcutMgr::getLang();

  for (const QString& moduleID : myModuleIDs) {
    const auto& moduleShortcuts = myShortcutContainer->getModuleShortcutsInversed(moduleID);
    if (moduleShortcuts.empty()) {
      // Do not display empty module.
      const auto moduleItemAndIdx = findModuleFolderItem(moduleID);
      if (moduleItemAndIdx.second >= 0)
        delete takeTopLevelItem(moduleItemAndIdx.second);

      continue;
    }

    const auto moduleItemAndIdx = findModuleFolderItem(moduleID);
    SUIT_ShortcutTreeFolder* moduleItem = moduleItemAndIdx.first;
    if (!moduleItem) {
      moduleItem = new SUIT_ShortcutTreeFolder(moduleID);
      moduleItem->setAssets(shortcutMgr->getModuleAssets(moduleID), lang);
      addTopLevelItem(moduleItem);
      moduleItem->setFlags(Qt::ItemIsEnabled);

      auto sortedChildren = getSortedChildren(moduleItem);
      for (const auto& shortcut : moduleShortcuts) {
        const QString& inModuleActionID = shortcut.first;
        const QKeySequence& keySequence = shortcut.second;
        const QString keySequenceString = keySequence.toString();

        auto actionItem = SUIT_ShortcutTreeAction::create(moduleID, inModuleActionID);
        if (!actionItem) {
          ShCutDbg("SUIT_ShortcutTree can't create child item for action ID = \"" + SUIT_ShortcutMgr::makeActionID(moduleID, inModuleActionID) + "\".");
          continue;
        }

        actionItem->setAssets(shortcutMgr->getActionAssets(moduleID, inModuleActionID), lang);
        actionItem->setKeySequence(keySequenceString);

        if (theHighlightModified) {
          const QKeySequence& appliedKeySequence = SUIT_ShortcutMgr::get()->getKeySequence(moduleID, inModuleActionID);
          actionItem->highlightKeySequenceAsModified(keySequence != appliedKeySequence);
        }

        insertChild(moduleItem, sortedChildren, actionItem);
      }

      moduleItem->setExpanded(true); // Make tree expanded on first show.
    }
    else /* if the tree has the module-item */ {
      for (int childIdx = 0; childIdx < moduleItem->childCount(); childIdx++) {
        // Update exisiting items of a module.
        SUIT_ShortcutTreeAction* const childItem = static_cast<SUIT_ShortcutTreeAction*>(moduleItem->child(childIdx));
        const auto itShortcut = moduleShortcuts.find(childItem->myInModuleActionID);
        if (itShortcut == moduleShortcuts.end()) {
          // Shortcut of the item has been removed from myShortcutContainer - impossible.
          continue;
        }
        const QKeySequence& newKeySequence = itShortcut->second;
        const QString newKeySequenceString = newKeySequence.toString();
        if (childItem->keySequence() != newKeySequenceString)
          childItem->setKeySequence(newKeySequenceString);

        if (theHighlightModified) {
          const QKeySequence& appliedKeySequence = SUIT_ShortcutMgr::get()->getKeySequence(moduleID, childItem->myInModuleActionID);
          childItem->highlightKeySequenceAsModified(newKeySequence != appliedKeySequence);
        }
        else
          childItem->highlightKeySequenceAsModified(false);
      }

      // Add new items if myShortcutContainer acquired new shortcuts, which may happen if a developer forgot
      // to add shortcuts for registered actions to resource files.
      if (moduleItem->childCount() < moduleShortcuts.size()) {
        auto sortedChildren = getSortedChildren(moduleItem);
        for (const auto& shortcut : moduleShortcuts) {
          const QString& inModuleActionID = shortcut.first;
          const auto predicate = [&inModuleActionID](const SUIT_ShortcutTreeItem* const theItem) -> bool {
            return static_cast<const SUIT_ShortcutTreeAction* const>(theItem)->myInModuleActionID == inModuleActionID;
          };

          if (std::find_if(sortedChildren.begin(), sortedChildren.end(), predicate) == sortedChildren.end()) {
            const auto actionItem = SUIT_ShortcutTreeAction::create(moduleID, inModuleActionID);
            if (!actionItem) {
              ShCutDbg("SUIT_ShortcutTree can't create child item for action ID = \"" + SUIT_ShortcutMgr::makeActionID(moduleID, inModuleActionID) + "\".");
              continue;
            }

            const QKeySequence& keySequence = shortcut.second;
            actionItem->setAssets(shortcutMgr->getActionAssets(moduleID, inModuleActionID), lang);
            actionItem->setKeySequence(keySequence.toString());

            if (theHighlightModified) {
              const QKeySequence& appliedKeySequence = SUIT_ShortcutMgr::get()->getKeySequence(moduleID, inModuleActionID);
              actionItem->highlightKeySequenceAsModified(keySequence != appliedKeySequence);
            }

            insertChild(moduleItem, sortedChildren, actionItem);
          }
        }
      }
    }
  }

  if (theUpdateSyncTrees) {
    const std::set<SUIT_ShortcutTree*>& syncTrees = SUIT_ShortcutTree::instances[myShortcutContainer.get()];
    for (const auto syncTree: syncTrees) {
      if (syncTree == this)
        continue;

      syncTree->updateItems(theHighlightModified, false /*theUpdateSyncTrees*/);
      const auto editDialog = syncTree->myEditDialog;
      editDialog->setConfirmedKeySequence(myShortcutContainer->getKeySequence(editDialog->moduleID(), editDialog->inModuleActionID()));
      editDialog->updateConflictsMessage();
    }
  }
}

/*! \returns Pointer and index of top-level item.
If the tree does not contain an item with theModuleID, returns {nullptr, -1}. */
std::pair<SUIT_ShortcutTreeFolder*, int> SUIT_ShortcutTree::findModuleFolderItem(const QString& theModuleID) const
{
  for (int moduleIdx = 0; moduleIdx < topLevelItemCount(); moduleIdx++) {
    SUIT_ShortcutTreeFolder* moduleItem = static_cast<SUIT_ShortcutTreeFolder*>(topLevelItem(moduleIdx));
    if (moduleItem->myModuleID == theModuleID)
      return std::pair<SUIT_ShortcutTreeFolder*, int>(moduleItem, moduleIdx);
  }
  return std::pair<SUIT_ShortcutTreeFolder*, int>(nullptr, -1);
}

/*! \returns Children of theParentItem being sorted according to current sort mode and order. */
std::set<SUIT_ShortcutTreeItem*, std::function<bool(SUIT_ShortcutTreeItem*, SUIT_ShortcutTreeItem*)>> SUIT_ShortcutTree::getSortedChildren(SUIT_ShortcutTreeFolder* theParentItem)
{
  QList<std::pair<SUIT_ShortcutTree::SortKey, SUIT_ShortcutTree::SortOrder>> sortSchema = SUIT_ShortcutTree::DEFAULT_SORT_SCHEMA;
  {
    for (auto itSameKey = sortSchema.begin(); itSameKey != sortSchema.end(); itSameKey++) {
      if (itSameKey->first == mySortKey) {
        sortSchema.erase(itSameKey);
        break;
      }
    }
    sortSchema.push_front(std::pair<SUIT_ShortcutTree::SortKey, SUIT_ShortcutTree::SortOrder>(mySortKey, mySortOrder));
  }

  const std::function<bool(SUIT_ShortcutTreeItem*, SUIT_ShortcutTreeItem*)> comparator =
  [this, sortSchema](const SUIT_ShortcutTreeItem* theItemA, const SUIT_ShortcutTreeItem* theItemB) {
    const auto collator = QCollator();
    for (const auto& keyAndOrder : sortSchema) {
      const int res = collator.compare(theItemA->getValue(keyAndOrder.first), theItemB->getValue(keyAndOrder.first));
      if (res != 0)
        return keyAndOrder.second == SUIT_ShortcutTree::SortOrder::Ascending ? res < 0 : res > 0;
    }
    return false;
  };

  std::set<SUIT_ShortcutTreeItem*, std::function<bool(SUIT_ShortcutTreeItem*, SUIT_ShortcutTreeItem*)>> sortedChildren(comparator);
  for (int childIdx = 0; childIdx < theParentItem->childCount(); childIdx++) {
    SUIT_ShortcutTreeAction* const childItem = static_cast<SUIT_ShortcutTreeAction*>(theParentItem->child(childIdx));
    sortedChildren.emplace(childItem);
  }
  return sortedChildren;
}

/*! \brief Inserts theChildItem to theParentItem and theSortedChildren.
Does not check whether theSortedChildren are actually child items of theParentItem.
Does not check whether current item sort schema is same as one of theSortedChildren. */
void SUIT_ShortcutTree::insertChild(
  SUIT_ShortcutTreeFolder* theParentItem,
  std::set<SUIT_ShortcutTreeItem*, std::function<bool(SUIT_ShortcutTreeItem*, SUIT_ShortcutTreeItem*)>>& theSortedChildren,
  SUIT_ShortcutTreeItem* theChildItem
) {
  auto emplaceRes = theSortedChildren.emplace(theChildItem);
  theParentItem->insertChild(indexOf(theSortedChildren, emplaceRes.first), theChildItem);
}

void SUIT_ShortcutTree::onItemDoubleClicked(QTreeWidgetItem* theItem, int theColIdx)
{
  {
    SUIT_ShortcutTreeItem* const item = static_cast<SUIT_ShortcutTreeItem*>(theItem);
    // Do not react if folder-item is clicked.
    if (item->type() != SUIT_ShortcutTreeItem::Type::Action)
      return;
  }

  SUIT_ShortcutTreeAction* const actionItem = static_cast<SUIT_ShortcutTreeAction*>(theItem);

  myEditDialog->setModuleAndActionID(actionItem->myModuleID, actionItem->myInModuleActionID);
  QString actionToolTip = actionItem->toolTip(SUIT_ShortcutTree::ElementIdx::Name);
  actionToolTip.truncate(actionToolTip.lastIndexOf('\n') + 1);
  myEditDialog->setModuleAndActionName(
    static_cast<SUIT_ShortcutTreeItem*>(actionItem->parent())->name(),
    actionItem->name(),
    actionToolTip
  );
  myEditDialog->setConfirmedKeySequence(QKeySequence::fromString(actionItem->keySequence()));
  myEditDialog->updateConflictsMessage();
  const bool somethingChanged = myEditDialog->exec() == QDialog::Accepted;

  if (!somethingChanged)
    return;

  const QKeySequence newKeySequence = myEditDialog->editedKeySequence();

  /** { moduleID, inModuleActionID }[] */
  std::set<std::pair<QString, QString>> disabledActionIDs = myShortcutContainer->setShortcut(actionItem->myModuleID, actionItem->myInModuleActionID, newKeySequence, true /*override*/);

  /** { moduleID, {inModuleActionID, keySequence}[] }[] */
  std::map<QString, std::map<QString, QString>> changes;
  changes[actionItem->myModuleID][actionItem->myInModuleActionID] = newKeySequence.toString();
  for (const auto moduleAndActionID : disabledActionIDs) {
    changes[moduleAndActionID.first][moduleAndActionID.second] = QString();
  }

  // Set new key sequences to shortcut items.
  for (const auto& moduleIDAndChanges : changes) {
    const QString& moduleID = moduleIDAndChanges.first;

    const auto moduleItemAndIdx = findModuleFolderItem(moduleID);
    const auto moduleItem = moduleItemAndIdx.first;
    if (!moduleItem)
      continue;

    /** {inModuleActionID, newKeySequence}[] */
    const std::map<QString, QString>& moduleChanges = moduleIDAndChanges.second;

    // Go through module' shortcut items, and highlight those, whose key sequences differ from applied key sequences.
    for (int childIdx = 0; childIdx < moduleItem->childCount(); childIdx++) {
      SUIT_ShortcutTreeAction* const childItem = static_cast<SUIT_ShortcutTreeAction*>(moduleItem->child(childIdx));
      const auto itChange = moduleChanges.find(childItem->myInModuleActionID);
      if (itChange == moduleChanges.end()) {
        // The shortcut has not been changed.
        continue;
      }

      childItem->setKeySequence(itChange->second);

      const QKeySequence& appliedKeySequence = SUIT_ShortcutMgr::get()->getKeySequence(moduleID, childItem->myInModuleActionID);
      childItem->highlightKeySequenceAsModified(QKeySequence::fromString(itChange->second) != appliedKeySequence);
    }
  }
}

/*static*/ const QList<std::pair<SUIT_ShortcutTree::SortKey, SUIT_ShortcutTree::SortOrder>> SUIT_ShortcutTree::DEFAULT_SORT_SCHEMA =
{
  {SUIT_ShortcutTree::SortKey::Name, SUIT_ShortcutTree::SortOrder::Ascending},
  {SUIT_ShortcutTree::SortKey::ToolTip, SUIT_ShortcutTree::SortOrder::Ascending},
  {SUIT_ShortcutTree::SortKey::KeySequence, SUIT_ShortcutTree::SortOrder::Ascending},
  {SUIT_ShortcutTree::SortKey::ID, SUIT_ShortcutTree::SortOrder::Ascending}
};

/*static*/ std::map<SUIT_ShortcutContainer*, std::set<SUIT_ShortcutTree*>> SUIT_ShortcutTree::instances =
std::map<SUIT_ShortcutContainer*, std::set<SUIT_ShortcutTree*>>();



SUIT_ShortcutTreeItem::SUIT_ShortcutTreeItem(const QString& theModuleID)
: QTreeWidgetItem(), myModuleID(theModuleID)
{ }

QString SUIT_ShortcutTreeItem::name() const
{
  return text(SUIT_ShortcutTree::ElementIdx::Name);
}


SUIT_ShortcutTreeFolder::SUIT_ShortcutTreeFolder(const QString& theModuleID)
: SUIT_ShortcutTreeItem(theModuleID)
{
  QFont f = font(SUIT_ShortcutTree::ElementIdx::Name);
  f.setBold(true);
  setFont(SUIT_ShortcutTree::ElementIdx::Name, f);
  setText(SUIT_ShortcutTree::ElementIdx::Name, theModuleID);
}

void SUIT_ShortcutTreeFolder::setAssets(std::shared_ptr<const SUIT_ActionAssets> theAssets, const QString& theLang)
{
  if (!theAssets)
    return;

  setIcon(SUIT_ShortcutTree::ElementIdx::Name, theAssets->myIcon);

  const auto& ldaMap = theAssets->myLangDependentAssets;
  if (ldaMap.empty()) {
    setText(SUIT_ShortcutTree::ElementIdx::Name, myModuleID);
    return;
  }

  auto itLDA = ldaMap.find(theLang);
  if (itLDA == ldaMap.end())
    itLDA = ldaMap.begin();

  const SUIT_ActionAssets::LangDependentAssets& lda = itLDA->second;
  const QString& name = lda.myName.isEmpty() ? myModuleID : lda.myName;
  setText(SUIT_ShortcutTree::ElementIdx::Name, name);
}

QString SUIT_ShortcutTreeFolder::getValue(SUIT_ShortcutTree::SortKey theKey) const
{
  switch (theKey) {
    case SUIT_ShortcutTree::SortKey::ID:
      return myModuleID;
    case SUIT_ShortcutTree::SortKey::Name:
      return name();
    case SUIT_ShortcutTree::SortKey::ToolTip:
      return name();
    default:
      return QString();
  }
}


SUIT_ShortcutTreeAction::SUIT_ShortcutTreeAction(const QString& theModuleID, const QString& theInModuleActionID)
: SUIT_ShortcutTreeItem(theModuleID), myInModuleActionID(theInModuleActionID)
{
  setText(SUIT_ShortcutTree::ElementIdx::Name, theInModuleActionID);
  setToolTip(
    SUIT_ShortcutTree::ElementIdx::Name,
    theInModuleActionID + (theInModuleActionID.at(theInModuleActionID.length()-1) == "." ? "\n" : ".\n") + SUIT_ShortcutTree::tr("Double click to edit key sequence.")
  );
  setToolTip(SUIT_ShortcutTree::ElementIdx::KeySequence, SUIT_ShortcutTree::tr("Double click to edit key sequence."));
}

/*static*/ SUIT_ShortcutTreeAction* SUIT_ShortcutTreeAction::create(const QString& theModuleID, const QString& theInModuleActionID)
{
  if (theInModuleActionID.isEmpty()) {
    ShCutDbg("SUIT_ShortcutTreeItem: attempt to create item with empty action ID.");
    return nullptr;
  }

  return new SUIT_ShortcutTreeAction(theModuleID, theInModuleActionID);
}

void SUIT_ShortcutTreeAction::setAssets(std::shared_ptr<const SUIT_ActionAssets> theAssets, const QString& theLang)
{
  if (!theAssets)
    return;

  setIcon(SUIT_ShortcutTree::ElementIdx::Name, theAssets->myIcon);

  const auto& ldaMap = theAssets->myLangDependentAssets;
  if (ldaMap.empty()) {
    setText(SUIT_ShortcutTree::ElementIdx::Name, myInModuleActionID);
    return;
  }

  auto itLDA = ldaMap.find(theLang);
  if (itLDA == ldaMap.end())
    itLDA = ldaMap.begin();

  const SUIT_ActionAssets::LangDependentAssets& lda = itLDA->second;
  const QString& name = lda.myName.isEmpty() ? myInModuleActionID : lda.myName;
  setText(SUIT_ShortcutTree::ElementIdx::Name, name);

  const QString& actionToolTip = lda.myToolTip.isEmpty() ? name : lda.myToolTip;
  setToolTip(
    SUIT_ShortcutTree::ElementIdx::Name,
    actionToolTip + (actionToolTip.at(actionToolTip.length()-1) == "." ? "\n" : ".\n") + SUIT_ShortcutTree::tr("Double click to edit key sequence.")
  );
}

QString SUIT_ShortcutTreeAction::getValue(SUIT_ShortcutTree::SortKey theKey) const
{
  switch (theKey) {
    case SUIT_ShortcutTree::SortKey::ID:
      return myInModuleActionID;
    case SUIT_ShortcutTree::SortKey::Name:
      return name();
    case SUIT_ShortcutTree::SortKey::ToolTip:
      return toolTip(SUIT_ShortcutTree::ElementIdx::Name);
    case SUIT_ShortcutTree::SortKey::KeySequence:
      return keySequence();
    default:
      return QString();
  }
}

void SUIT_ShortcutTreeAction::setKeySequence(const QString& theKeySequence)
{
  setText(SUIT_ShortcutTree::ElementIdx::KeySequence, theKeySequence);
}

QString SUIT_ShortcutTreeAction::keySequence() const
{
  return text(SUIT_ShortcutTree::ElementIdx::KeySequence);
}

/*! \brief Highlights text at ElementIdx::KeySequence. */
void SUIT_ShortcutTreeAction::highlightKeySequenceAsModified(bool theHighlight)
{
  static const QBrush bgHighlitingBrush = QBrush(Qt::darkGreen);
  static const QBrush fgHighlitingBrush = QBrush(Qt::white);
  static const QBrush noBrush = QBrush();

  setBackground(SUIT_ShortcutTree::ElementIdx::KeySequence, theHighlight ? bgHighlitingBrush : noBrush);
  setForeground(SUIT_ShortcutTree::ElementIdx::KeySequence, theHighlight ? fgHighlitingBrush : noBrush);
}