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

#include "QtxShortcutEdit.h"

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


QtxKeySequenceEdit::QtxKeySequenceEdit(QWidget* parent)
: QFrame(parent)
{
  initialize();
  myKeySequenceLineEdit->installEventFilter(this);
}

/*! \brief Set a key sequence to edit. */
void QtxKeySequenceEdit::setConfirmedKeySequence(const QKeySequence& theKeySequence)
{
  myConfirmedKeySequenceString = theKeySequence.toString();
  myKeySequenceLineEdit->setText(myConfirmedKeySequenceString);
  myPrevKeySequenceString = myConfirmedKeySequenceString;
}

void QtxKeySequenceEdit::setEditedKeySequence(const QKeySequence& theKeySequence)
{
  const QString keySequenceString = theKeySequence.toString();
  myKeySequenceLineEdit->setText(keySequenceString);
  myPrevKeySequenceString = keySequenceString;
}

QKeySequence QtxKeySequenceEdit::editedKeySequence() const
{
  return QKeySequence::fromString(myKeySequenceLineEdit->text());
}

/*! \returns true, if the edited key sequence differs from confirmed one. */
bool QtxKeySequenceEdit::isKeySequenceModified() const
{
  return QKeySequence(myConfirmedKeySequenceString) != editedKeySequence();
}

/*! \brief Set confirmed key sequence to line editor. */
void QtxKeySequenceEdit::restoreKeySequence()
{
  myKeySequenceLineEdit->setText(myConfirmedKeySequenceString);
  myPrevKeySequenceString = myConfirmedKeySequenceString;
}

/*!
  \brief Gets the key sequence from keys that were pressed
  \param e a key event
  \returns a string representation of the key sequence
*/
/*static*/ QString QtxKeySequenceEdit::parseEvent(QKeyEvent* e)
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
/*static*/ bool QtxKeySequenceEdit::isValidKey(int theKey)
{
  if ( theKey == Qt::Key_Underscore || theKey == Qt::Key_Escape ||
     ( theKey >= Qt::Key_Backspace && theKey <= Qt::Key_Delete ) ||
     ( theKey >= Qt::Key_Home && theKey <= Qt::Key_PageDown ) ||
     ( theKey >= Qt::Key_F1 && theKey <= Qt::Key_F12 )  ||
     ( theKey >= Qt::Key_Space && theKey <= Qt::Key_Asterisk ) ||
     ( theKey >= Qt::Key_Comma && theKey <= Qt::Key_Question ) ||
     ( theKey >= Qt::Key_A && theKey <= Qt::Key_AsciiTilde ) )
    return true;
  return false;
}

/*! \brief Called when "Clear" button is clicked. */
void QtxKeySequenceEdit::onClear()
{
  myKeySequenceLineEdit->setText("");
  myPrevKeySequenceString = "";
  emit editingFinished();
}

/*! \brief Called when myKeySequenceLineEdit loses focus. */
void QtxKeySequenceEdit::onEditingFinished()
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
bool QtxKeySequenceEdit::eventFilter(QObject* theObject, QEvent* theEvent)
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
void QtxKeySequenceEdit::initialize()
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
QtxEditKeySequenceDialog::QtxEditKeySequenceDialog(QtxShortcutTree* theParent)
: QDialog(theParent)
{
  setMinimumWidth(500);
  setWindowTitle(tr("Change key sequence"));
  QVBoxLayout* layout = new QVBoxLayout(this);
  myActionName = new QLabel(this);
  myActionName->setTextFormat(Qt::RichText);
  myKeySequenceEdit = new QtxKeySequenceEdit(this);
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

void QtxEditKeySequenceDialog::setModuleAndActionID(const QString& theModuleID, const QString& theInModuleActionID)
{
  myModuleID = theModuleID;
  myInModuleActionID = theInModuleActionID;
}

const QString& QtxEditKeySequenceDialog::moduleID() const { return myModuleID; }
const QString& QtxEditKeySequenceDialog::inModuleActionID() const { return myInModuleActionID; }

void QtxEditKeySequenceDialog::setModuleAndActionName(const QString& theModuleName, const QString& theActionName, const QString& theActionToolTip)
{
  myActionName->setText("<b>" + theModuleName + "</b>&nbsp;&nbsp;" + theActionName);
  myActionName->setToolTip(theActionToolTip);
}

void QtxEditKeySequenceDialog::setConfirmedKeySequence(const QKeySequence& theSequence)
{
  myKeySequenceEdit->setConfirmedKeySequence(theSequence);
}

QKeySequence QtxEditKeySequenceDialog::editedKeySequence() const
{
  return myKeySequenceEdit->editedKeySequence();
}

int QtxEditKeySequenceDialog::exec()
{
  myKeySequenceEdit->setFocus(Qt::ActiveWindowFocusReason);
  return QDialog::exec();
}

void QtxEditKeySequenceDialog::onEditingStarted()
{
  myTextEdit->setEnabled(false);
}

void QtxEditKeySequenceDialog::onEditingFinished()
{
  updateConflictsMessage();
}

void QtxEditKeySequenceDialog::onRestoreFromShortcutMgr()
{
  const auto shortcutMgr = SUIT_ShortcutMgr::get();
  myKeySequenceEdit->setEditedKeySequence(shortcutMgr->getKeySequence(myModuleID, myInModuleActionID));
  updateConflictsMessage();
}

/*! Updates message with list of actions, whose shortcuts will be disabled on Confirm. */
void QtxEditKeySequenceDialog::updateConflictsMessage()
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

  const auto shortcutTree = static_cast<QtxShortcutTree*>(parentWidget());
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

void QtxEditKeySequenceDialog::onConfirm()
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
QtxShortcutTree::QtxShortcutTree(
  std::shared_ptr<SUIT_ShortcutContainer> theContainer,
  QWidget* theParent
) : QTreeWidget(theParent),
myShortcutContainer(theContainer ? theContainer : std::shared_ptr<SUIT_ShortcutContainer>(new SUIT_ShortcutContainer())),
mySortKey(QtxShortcutTree::SortKey::Name), mySortOrder(QtxShortcutTree::SortOrder::Ascending)
{
  setColumnCount(2);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setColumnWidth(0, COLUMN_SIZE);
  setSortingEnabled(false); // Items are sorted in the same way, as in ShortcutContainer.
  header()->setSectionResizeMode(QHeaderView::Interactive);
  {
    QMap<int, QString> labelMap;
    labelMap[QtxShortcutTree::ElementIdx::Name]        = tr("Action");
    labelMap[QtxShortcutTree::ElementIdx::KeySequence] = tr("Key sequence");
    setHeaderLabels(labelMap.values());
  }
  setExpandsOnDoubleClick(false); // Open shortcut editor on double click instead.
  setSortingEnabled(false);
  setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  myEditDialog = new QtxEditKeySequenceDialog(this);

  this->installEventFilter(this);
  connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

  QtxShortcutTree::instances[myShortcutContainer.get()].emplace(this);
}

QtxShortcutTree::~QtxShortcutTree()
{
  QtxShortcutTree::instances[myShortcutContainer.get()].erase(this);
  if (QtxShortcutTree::instances[myShortcutContainer.get()].empty())
    QtxShortcutTree::instances.erase(myShortcutContainer.get());
}

/*! \brief Copies shortcuts from ShortcutMgr. (Re)displays shortcuts of myModuleIDs. */
void QtxShortcutTree::setShortcutsFromManager()
{
  const auto shortcutMgr = SUIT_ShortcutMgr::get();
  *myShortcutContainer = shortcutMgr->getShortcutContainer();
  // nb! ShortcutMgr never removes shortcuts from its container, only disables.

  updateItems(false /*theHighlightModified*/, true /*theUpdateSyncTrees*/);
}

/*! \brief Copies shortcuts from resources, user files are not accounted. (Re)displays shortcuts of myModuleIDs. */
void QtxShortcutTree::setDefaultShortcuts()
{
  SUIT_ShortcutContainer defaultShortcuts;
  SUIT_ShortcutMgr::fillContainerFromPreferences(defaultShortcuts, true /*theDefaultOnly*/);

  myShortcutContainer->merge(defaultShortcuts, true /*theOverride*/, true /*theTreatAbsentIncomingAsDisabled*/);
  // nb! SUIT_ShortcutContainer never erases shortcuts, only disables.

  updateItems(true /*theHighlightModified*/, true /*theUpdateSyncTrees*/);
}

/*! \brief Applies pending changes to ShortcutMgr. Updates other instances of QtxShortcutTree. */
void QtxShortcutTree::applyChangesToShortcutMgr()
{
  const auto mgr = SUIT_ShortcutMgr::get();
  mgr->mergeShortcutContainer(*myShortcutContainer);

  // Update non-synchronized with this instances.
  for (const auto& containerAndSyncTrees : QtxShortcutTree::instances) {
    if (containerAndSyncTrees.first == myShortcutContainer.get())
      continue;

    const std::set<QtxShortcutTree*>& syncTrees = containerAndSyncTrees.second;
    const auto itFirstSyncTree = syncTrees.begin();
    if (itFirstSyncTree == syncTrees.end())
      continue;

    (*itFirstSyncTree)->setShortcutsFromManager();
    const auto editDialog = (*itFirstSyncTree)->myEditDialog;
    editDialog->setConfirmedKeySequence(mgr->getShortcutContainer().getKeySequence(editDialog->moduleID(), editDialog->inModuleActionID()));
    editDialog->updateConflictsMessage();
  }
}

std::shared_ptr<const SUIT_ShortcutContainer> QtxShortcutTree::shortcutContainer() const
{
  return myShortcutContainer;
}

/*! \brief Does not sort modules. */
void QtxShortcutTree::sort(QtxShortcutTree::SortKey theKey, QtxShortcutTree::SortOrder theOrder)
{
  if (theKey == mySortKey && theOrder == mySortOrder)
    return;

  mySortKey == theKey;
  mySortOrder = theOrder;

  for (int moduleIdx = 0; moduleIdx < topLevelItemCount(); moduleIdx++) {
    const auto moduleItem = static_cast<QtxShortcutTreeFolder*>(topLevelItem(moduleIdx));
    const auto sortedChildren = getSortedChildren(moduleItem);
    moduleItem->takeChildren();

    for (const auto childItem : sortedChildren) {
      moduleItem->addChild(childItem);
    }
  }
}

/*! \param If theUpdateSyncTrees, trees sharing the same shortcut container are updated. */
void QtxShortcutTree::updateItems(bool theHighlightModified, bool theUpdateSyncTrees)
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
    QtxShortcutTreeFolder* moduleItem = moduleItemAndIdx.first;
    if (!moduleItem) {
      moduleItem = new QtxShortcutTreeFolder(moduleID);
      moduleItem->setAssets(shortcutMgr->getModuleAssets(moduleID), lang);
      addTopLevelItem(moduleItem);
      moduleItem->setFlags(Qt::ItemIsEnabled);

      auto sortedChildren = getSortedChildren(moduleItem);
      for (const auto& shortcut : moduleShortcuts) {
        const QString& inModuleActionID = shortcut.first;
        const QKeySequence& keySequence = shortcut.second;
        const QString keySequenceString = keySequence.toString();

        auto actionItem = QtxShortcutTreeAction::create(moduleID, inModuleActionID);
        if (!actionItem) {
          ShCutDbg("QtxShortcutTree can't create child item for action ID = \"" + SUIT_ShortcutMgr::makeActionID(moduleID, inModuleActionID) + "\".");
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
        QtxShortcutTreeAction* const childItem = static_cast<QtxShortcutTreeAction*>(moduleItem->child(childIdx));
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
          const auto predicate = [&inModuleActionID](const QtxShortcutTreeItem* const theItem) -> bool {
            return static_cast<const QtxShortcutTreeAction* const>(theItem)->myInModuleActionID == inModuleActionID;
          };

          if (std::find_if(sortedChildren.begin(), sortedChildren.end(), predicate) == sortedChildren.end()) {
            const auto actionItem = QtxShortcutTreeAction::create(moduleID, inModuleActionID);
            if (!actionItem) {
              ShCutDbg("QtxShortcutTree can't create child item for action ID = \"" + SUIT_ShortcutMgr::makeActionID(moduleID, inModuleActionID) + "\".");
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
    const std::set<QtxShortcutTree*>& syncTrees = QtxShortcutTree::instances[myShortcutContainer.get()];
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
std::pair<QtxShortcutTreeFolder*, int> QtxShortcutTree::findModuleFolderItem(const QString& theModuleID) const
{
  for (int moduleIdx = 0; moduleIdx < topLevelItemCount(); moduleIdx++) {
    QtxShortcutTreeFolder* moduleItem = static_cast<QtxShortcutTreeFolder*>(topLevelItem(moduleIdx));
    if (moduleItem->myModuleID == theModuleID)
      return std::pair<QtxShortcutTreeFolder*, int>(moduleItem, moduleIdx);
  }
  return std::pair<QtxShortcutTreeFolder*, int>(nullptr, -1);
}

/*! \returns Children of theParentItem being sorted according to current sort mode and order. */
std::set<QtxShortcutTreeItem*, std::function<bool(QtxShortcutTreeItem*, QtxShortcutTreeItem*)>> QtxShortcutTree::getSortedChildren(QtxShortcutTreeFolder* theParentItem)
{
  QList<std::pair<QtxShortcutTree::SortKey, QtxShortcutTree::SortOrder>> sortSchema = QtxShortcutTree::DEFAULT_SORT_SCHEMA;
  {
    for (auto itSameKey = sortSchema.begin(); itSameKey != sortSchema.end(); itSameKey++) {
      if (itSameKey->first == mySortKey) {
        sortSchema.erase(itSameKey);
        break;
      }
    }
    sortSchema.push_front(std::pair<QtxShortcutTree::SortKey, QtxShortcutTree::SortOrder>(mySortKey, mySortOrder));
  }

  static const QCollator collator;
  const std::function<bool(QtxShortcutTreeItem*, QtxShortcutTreeItem*)> comparator =
  [this, sortSchema, &collator](const QtxShortcutTreeItem* theItemA, const QtxShortcutTreeItem* theItemB) {
    int res = 0;
    for (const auto& keyAndOrder : sortSchema) {
      int res = 0;
      res = collator.compare(theItemA->getValue(keyAndOrder.first), theItemB->getValue(keyAndOrder.first));
      if (res != 0)
        return keyAndOrder.second == QtxShortcutTree::SortOrder::Ascending ? res < 0 : res > 0;
    }
    return false;
  };

  std::set<QtxShortcutTreeItem*, std::function<bool(QtxShortcutTreeItem*, QtxShortcutTreeItem*)>> sortedChildren(comparator);
  for (int childIdx = 0; childIdx < theParentItem->childCount(); childIdx++) {
    QtxShortcutTreeAction* const childItem = static_cast<QtxShortcutTreeAction*>(theParentItem->child(childIdx));
    sortedChildren.emplace(childItem);
  }
  return sortedChildren;
}

/*! \brief Inserts theChildItem to theParentItem and theSortedChildren.
Does not check whether theSortedChildren are actually child items of theParentItem.
Does not check whether current item sort schema is same as one of theSortedChildren. */
void QtxShortcutTree::insertChild(
  QtxShortcutTreeFolder* theParentItem,
  std::set<QtxShortcutTreeItem*, std::function<bool(QtxShortcutTreeItem*, QtxShortcutTreeItem*)>>& theSortedChildren,
  QtxShortcutTreeItem* theChildItem
) {
  auto emplaceRes = theSortedChildren.emplace(theChildItem);
  theParentItem->insertChild(indexOf(theSortedChildren, emplaceRes.first), theChildItem);
}

void QtxShortcutTree::onItemDoubleClicked(QTreeWidgetItem* theItem, int theColIdx)
{
  {
    QtxShortcutTreeItem* const item = static_cast<QtxShortcutTreeItem*>(theItem);
    // Do not react if folder-item is clicked.
    if (item->type() != QtxShortcutTreeItem::Type::Action)
      return;
  }

  QtxShortcutTreeAction* const actionItem = static_cast<QtxShortcutTreeAction*>(theItem);

  myEditDialog->setModuleAndActionID(actionItem->myModuleID, actionItem->myInModuleActionID);
  QString actionToolTip = actionItem->toolTip(QtxShortcutTree::ElementIdx::Name);
  actionToolTip.truncate(actionToolTip.lastIndexOf('\n') + 1);
  myEditDialog->setModuleAndActionName(
    static_cast<QtxShortcutTreeItem*>(actionItem->parent())->name(),
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
      QtxShortcutTreeAction* const childItem = static_cast<QtxShortcutTreeAction*>(moduleItem->child(childIdx));
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

/*static*/ const QList<std::pair<QtxShortcutTree::SortKey, QtxShortcutTree::SortOrder>> QtxShortcutTree::DEFAULT_SORT_SCHEMA =
{
  {QtxShortcutTree::SortKey::Name, QtxShortcutTree::SortOrder::Ascending},
  {QtxShortcutTree::SortKey::ToolTip, QtxShortcutTree::SortOrder::Ascending},
  {QtxShortcutTree::SortKey::KeySequence, QtxShortcutTree::SortOrder::Ascending},
  {QtxShortcutTree::SortKey::ID, QtxShortcutTree::SortOrder::Ascending}
};

/*static*/ std::map<SUIT_ShortcutContainer*, std::set<QtxShortcutTree*>> QtxShortcutTree::instances =
std::map<SUIT_ShortcutContainer*, std::set<QtxShortcutTree*>>();



QtxShortcutTreeItem::QtxShortcutTreeItem(const QString& theModuleID)
: QTreeWidgetItem(), myModuleID(theModuleID)
{ }

QString QtxShortcutTreeItem::name() const
{
  return text(QtxShortcutTree::ElementIdx::Name);
}


QtxShortcutTreeFolder::QtxShortcutTreeFolder(const QString& theModuleID)
: QtxShortcutTreeItem(theModuleID)
{
  QFont f = font(QtxShortcutTree::ElementIdx::Name);
  f.setBold(true);
  setFont(QtxShortcutTree::ElementIdx::Name, f);
  setText(QtxShortcutTree::ElementIdx::Name, theModuleID);
}

void QtxShortcutTreeFolder::setAssets(std::shared_ptr<const SUIT_ActionAssets> theAssets, const QString& theLang)
{
  if (!theAssets)
    return;

  setIcon(QtxShortcutTree::ElementIdx::Name, theAssets->myIcon);

  const auto& ldaMap = theAssets->myLangDependentAssets;
  if (ldaMap.empty()) {
    setText(QtxShortcutTree::ElementIdx::Name, myModuleID);
    return;
  }

  auto itLDA = ldaMap.find(theLang);
  if (itLDA == ldaMap.end())
    itLDA = ldaMap.begin();

  const SUIT_ActionAssets::LangDependentAssets& lda = itLDA->second;
  const QString& name = lda.myName.isEmpty() ? myModuleID : lda.myName;
  setText(QtxShortcutTree::ElementIdx::Name, name);
}

QString QtxShortcutTreeFolder::getValue(QtxShortcutTree::SortKey theKey) const
{
  switch (theKey) {
    case QtxShortcutTree::SortKey::ID:
      return myModuleID;
    case QtxShortcutTree::SortKey::Name:
      return name();
    case QtxShortcutTree::SortKey::ToolTip:
      return name();
    default:
      return QString();
  }
}


QtxShortcutTreeAction::QtxShortcutTreeAction(const QString& theModuleID, const QString& theInModuleActionID)
: QtxShortcutTreeItem(theModuleID), myInModuleActionID(theInModuleActionID)
{
  setText(QtxShortcutTree::ElementIdx::Name, theInModuleActionID);
  setToolTip(
    QtxShortcutTree::ElementIdx::Name,
    theInModuleActionID + (theInModuleActionID.at(theInModuleActionID.length()-1) == "." ? "\n" : ".\n") + QtxShortcutTree::tr("Double click to edit key sequence.")
  );
  setToolTip(QtxShortcutTree::ElementIdx::KeySequence, QtxShortcutTree::tr("Double click to edit key sequence."));
}

/*static*/ QtxShortcutTreeAction* QtxShortcutTreeAction::create(const QString& theModuleID, const QString& theInModuleActionID)
{
  if (theInModuleActionID.isEmpty()) {
    ShCutDbg("QtxShortcutTreeItem: attempt to create item with empty action ID.");
    return nullptr;
  }

  return new QtxShortcutTreeAction(theModuleID, theInModuleActionID);
}

void QtxShortcutTreeAction::setAssets(std::shared_ptr<const SUIT_ActionAssets> theAssets, const QString& theLang)
{
  if (!theAssets)
    return;

  setIcon(QtxShortcutTree::ElementIdx::Name, theAssets->myIcon);

  const auto& ldaMap = theAssets->myLangDependentAssets;
  if (ldaMap.empty()) {
    setText(QtxShortcutTree::ElementIdx::Name, myInModuleActionID);
    return;
  }

  auto itLDA = ldaMap.find(theLang);
  if (itLDA == ldaMap.end())
    itLDA = ldaMap.begin();

  const SUIT_ActionAssets::LangDependentAssets& lda = itLDA->second;
  const QString& name = lda.myName.isEmpty() ? myInModuleActionID : lda.myName;
  setText(QtxShortcutTree::ElementIdx::Name, name);

  const QString& actionToolTip = lda.myToolTip.isEmpty() ? name : lda.myToolTip;
  setToolTip(
    QtxShortcutTree::ElementIdx::Name,
    actionToolTip + (actionToolTip.at(actionToolTip.length()-1) == "." ? "\n" : ".\n") + QtxShortcutTree::tr("Double click to edit key sequence.")
  );
}

QString QtxShortcutTreeAction::getValue(QtxShortcutTree::SortKey theKey) const
{
  switch (theKey) {
    case QtxShortcutTree::SortKey::ID:
      return myInModuleActionID;
    case QtxShortcutTree::SortKey::Name:
      return name();
    case QtxShortcutTree::SortKey::ToolTip:
      return toolTip(QtxShortcutTree::ElementIdx::Name);
    case QtxShortcutTree::SortKey::KeySequence:
      return keySequence();
    default:
      return QString();
  }
}

void QtxShortcutTreeAction::setKeySequence(const QString& theKeySequence)
{
  setText(QtxShortcutTree::ElementIdx::KeySequence, theKeySequence);
}

QString QtxShortcutTreeAction::keySequence() const
{
  return text(QtxShortcutTree::ElementIdx::KeySequence);
}

/*! \brief Highlights text at ElementIdx::KeySequence. */
void QtxShortcutTreeAction::highlightKeySequenceAsModified(bool theHighlight)
{
  static const QBrush bgHighlitingBrush = QBrush(Qt::darkGreen);
  static const QBrush fgHighlitingBrush = QBrush(Qt::white);
  static const QBrush noBrush = QBrush();

  setBackground(QtxShortcutTree::ElementIdx::KeySequence, theHighlight ? bgHighlitingBrush : noBrush);
  setForeground(QtxShortcutTree::ElementIdx::KeySequence, theHighlight ? fgHighlitingBrush : noBrush);
}