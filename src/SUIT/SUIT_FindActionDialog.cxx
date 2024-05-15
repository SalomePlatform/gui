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

#include "SUIT_FindActionDialog.h"

#include <QAction>
#include <QWidget>
#include <QLayout>
#include <QList>
#include <QMap>
#include <QApplication>

#include <QCollator>

#include <QCheckBox>
#include <QLineEdit>
#include <QBrush>
#include <QColor>
#include <QHeaderView>
#include <QKeyEvent>

#include <algorithm>
#include <limits>


SUIT_FindActionDialog::SUIT_FindActionDialog(QWidget* theParent)
: QDialog(theParent)
{
  setMinimumWidth(500);
  setWindowTitle(tr("Find action"));
  QVBoxLayout* layout = new QVBoxLayout(this);

  myQueryLineEdit = new QLineEdit(this);
  layout->addWidget(myQueryLineEdit);
  myQueryLineEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  setFocusProxy(myQueryLineEdit);

  QHBoxLayout* searchOptionsLayout = new QHBoxLayout(this);
  layout->addLayout(searchOptionsLayout);
  myIncludeUnavailableActionsCB = new QCheckBox(tr("Unavailable actions"), this);
  myIncludeUnavailableActionsCB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  myIncludeUnavailableActionsCB->setCheckState(Qt::CheckState::Checked);
  myActionSearcher.includeDisabledActions(true);
  myIncludeInactiveModulesCB = new QCheckBox(tr("Inactive modules"), this);
  myIncludeInactiveModulesCB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  myIncludeInactiveModulesCB->setCheckState(Qt::CheckState::Unchecked);
  searchOptionsLayout->addWidget(myIncludeUnavailableActionsCB);
  searchOptionsLayout->addWidget(myIncludeInactiveModulesCB);

  myFoundActionsTree = new SUIT_FoundActionTree(this);
  layout->addWidget(myFoundActionsTree);

  connect(myQueryLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(onQueryChanged(const QString&)));
  connect(myIncludeUnavailableActionsCB, SIGNAL(stateChanged(int)), this, SLOT(onSearchOptionUnavailableActionsChanged(int)));
  connect(myIncludeInactiveModulesCB, SIGNAL(stateChanged(int)), this, SLOT(onSearchOptionInactiveModulesChanged(int)));

  myQueryLineEdit->installEventFilter(myFoundActionsTree);
}

void SUIT_FindActionDialog::setActiveModuleID(const QString& theModuleID)
{
  myActiveModuleID = theModuleID;
  if(myActionSearcher.setIncludedModuleIDs(std::set<QString>({SUIT_ShortcutMgr::ROOT_MODULE_ID, myActiveModuleID})))
    updateUI();
}

void SUIT_FindActionDialog::onQueryChanged(const QString& theQuery)
{
  if (myActionSearcher.setQuery(theQuery))
    updateUI();
}

void SUIT_FindActionDialog::onSearchOptionUnavailableActionsChanged(int theState)
{
  if (myActionSearcher.includeDisabledActions(theState == Qt::CheckState::Checked))
    updateUI();
}

void SUIT_FindActionDialog::onSearchOptionInactiveModulesChanged(int theState)
{
  bool resultsChanged = false;
  if (theState == Qt::CheckState::Checked) {
    myIncludeUnavailableActionsCB->setDisabled(true);
    myIncludeUnavailableActionsCB->setCheckState(Qt::CheckState::Checked);
    resultsChanged = myActionSearcher.setIncludedModuleIDs(SUIT_ShortcutMgr::get()->getShortcutContainer().getIDsOfAllModules());
  }
  else {
    myIncludeUnavailableActionsCB->setDisabled(false);
    resultsChanged = myActionSearcher.setIncludedModuleIDs(std::set<QString>({SUIT_ShortcutMgr::ROOT_MODULE_ID, myActiveModuleID}));
  }

  if (resultsChanged)
    updateUI();
}

void SUIT_FindActionDialog::updateUI()
{
  myFoundActionsTree->updateItems(myActionSearcher.getSearchResults());
}



SUIT_FoundActionTree::SUIT_FoundActionTree(SUIT_FindActionDialog* theParent)
: QTreeWidget(theParent)
{
  setColumnCount(2);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setSortingEnabled(false);
  header()->setSectionResizeMode(QHeaderView::Interactive);
  {
    QMap<int, QString> labelMap;
    labelMap[SUIT_FoundActionTree::ElementIdx::Name]    = SUIT_FindActionDialog::tr("Action");
    labelMap[SUIT_FoundActionTree::ElementIdx::ToolTip] = SUIT_FindActionDialog::tr("Description");
    setHeaderLabels(labelMap.values());
  }
  setExpandsOnDoubleClick(false); // Implemented manually.
  setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setColumnWidth(SUIT_FoundActionTree::ElementIdx::Name, 120);
  setColumnWidth(SUIT_FoundActionTree::ElementIdx::Name, 250);
  setMinimumHeight(300);

  setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

  mySortKey = SUIT_FoundActionTree::SortKey::MatchMetrics;
  mySortOrder = SUIT_FoundActionTree::SortOrder::Ascending;

  connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemExecuted(QTreeWidgetItem*, int)));
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

void SUIT_FoundActionTree::updateItems(const std::map<QString, std::map<QString, SUIT_ActionSearcher::AssetsAndSearchData>>& theAssets)
{
  std::set<QString> shownModuleIDs; // To sort module-items by their IDs.

  // Remove shown module items, if updated search results have no matching actions from these modules.
  for (int moduleIdx = 0; moduleIdx < topLevelItemCount(); ) {
    SUIT_FoundActionTreeModule* moduleItem = static_cast<SUIT_FoundActionTreeModule*>(topLevelItem(moduleIdx));
    myModuleItemExpansionStates[moduleItem->myModuleID] = moduleItem->isExpanded();

    const auto itUpdatedAssetsOfShownModule = theAssets.find(moduleItem->myModuleID);
    if (itUpdatedAssetsOfShownModule == theAssets.end()) {
      delete takeTopLevelItem(moduleIdx);
      continue;
    }

    if (itUpdatedAssetsOfShownModule->second.empty()) {
      delete takeTopLevelItem(moduleIdx);
      continue;
    }

    shownModuleIDs.emplace(moduleItem->myModuleID);
    moduleIdx++;
  }

  const auto shortcutMgr = SUIT_ShortcutMgr::get();
  const QString lang = SUIT_ShortcutMgr::getLang();

  SUIT_FoundActionTreeAction* preselectedActionItem = nullptr;

  for (const auto& moduleIDAndAssets : theAssets) {
    const QString& moduleID = moduleIDAndAssets.first;
    const auto& moduleAssets = moduleIDAndAssets.second;
    if (moduleAssets.empty())
      continue;

    const auto moduleItemAndIdx = findModuleItem(moduleID);
    SUIT_FoundActionTreeModule* moduleItem = moduleItemAndIdx.first;
    if (!moduleItem) {
      moduleItem = new SUIT_FoundActionTreeModule(moduleID);
      moduleItem->setAssetsAndSearchData(SUIT_ActionSearcher::AssetsAndSearchData(shortcutMgr->getModuleAssets(moduleID)), lang);

      const auto emplaceRes = shownModuleIDs.emplace(moduleID);
      insertTopLevelItem(indexOf(shownModuleIDs, emplaceRes.first), moduleItem);

      moduleItem->setFlags(Qt::ItemIsEnabled);

      const auto itExpansionState = myModuleItemExpansionStates.find(moduleID);
      if (itExpansionState == myModuleItemExpansionStates.end())
        moduleItem->setExpanded(true); // Make module item expanded at first appearance.
      else
        moduleItem->setExpanded(itExpansionState->second);
    }
    else /* if the tree has the module-item */ {
      const auto actionItems = moduleItem->takeChildren();
      for (const auto actionItem : actionItems) {
        delete actionItem;
      }
    }

    // Fill module item with action items.
    auto sortedActionItems = createActionSetWithComparator();
    for (const auto& actionIDAndAssets : moduleAssets) {
      const QString& inModuleActionID = actionIDAndAssets.first;
      const SUIT_ActionSearcher::AssetsAndSearchData& assetsAndSearchData = actionIDAndAssets.second;

      auto actionItem = SUIT_FoundActionTreeAction::create(moduleID, inModuleActionID);
      if (!actionItem) {
        ShCutDbg("SUIT_FoundActionTree can't create child item for action ID = \"" + SUIT_ShortcutMgr::makeActionID(moduleID, inModuleActionID) + "\".");
        continue;
      }

      actionItem->setAssetsAndSearchData(assetsAndSearchData, lang);
      sortedActionItems.emplace(actionItem);
    }

    SUIT_FoundActionTreeAction* preselectedActionItemCand = nullptr;
    for (const auto actionItem : sortedActionItems) {
      moduleItem->addChild(actionItem);

      // Consider first ranked available action in the module (if user did not collapsed it) as a candidate for preselected action.
      if (!preselectedActionItemCand && moduleItem->isExpanded() && actionItem->isEnabledBufferedValue())
        preselectedActionItemCand = actionItem;
    }

    if (preselectedActionItem) {
      if (preselectedActionItemCand) {
        if (preselectedActionItemCand->matchMetrics() < preselectedActionItem->matchMetrics())
          preselectedActionItem = preselectedActionItemCand;
      }
    }
    else
      preselectedActionItem = preselectedActionItemCand;
  }

  if (preselectedActionItem)
    setCurrentItem(preselectedActionItem);
}

void SUIT_FoundActionTree::sort(SUIT_FoundActionTree::SortKey theKey, SUIT_FoundActionTree::SortOrder theOrder)
{
  if (theKey == mySortKey && theOrder == mySortOrder)
    return;

  mySortKey == theKey;
  mySortOrder = theOrder;

  for (int moduleIdx = 0; moduleIdx < topLevelItemCount(); moduleIdx++) {
    const auto moduleItem = static_cast<SUIT_FoundActionTreeModule*>(topLevelItem(moduleIdx));

    auto sortedActionItems = createActionSetWithComparator();
    for (int childIdx = 0; childIdx < moduleItem->childCount(); childIdx++) {
      SUIT_FoundActionTreeAction* const actionItem = static_cast<SUIT_FoundActionTreeAction*>(moduleItem->child(childIdx));
      sortedActionItems.emplace(actionItem);
    }

    moduleItem->takeChildren();

    for (const auto actionItem : sortedActionItems) {
      moduleItem->addChild(actionItem);
    }
  }
}

void SUIT_FoundActionTree::keyPressEvent(QKeyEvent* theEvent)
{
  const auto key = theEvent->key();
  const auto selectedItem = currentItem();
  if ((key == Qt::Key_Enter || key == Qt::Key_Return) && selectedItem)
    onItemExecuted(selectedItem, SUIT_FoundActionTree::ElementIdx::Name);
  else
    QTreeWidget::keyPressEvent(theEvent);
}

bool SUIT_FoundActionTree::eventFilter(QObject* theQObject, QEvent* theEvent)
{
  if (theEvent->type() == QEvent::KeyPress) {
    QKeyEvent* const keyEvent = static_cast<QKeyEvent*>(theEvent);
    const auto key = keyEvent->key();

    if (key == Qt::Key_Enter || key == Qt::Key_Return || key == Qt::Key_Up || key == Qt::Key_Down) {
      keyPressEvent(keyEvent);
      return true;
    }
  }

  return false;
}

std::pair<SUIT_FoundActionTreeModule*, int> SUIT_FoundActionTree::findModuleItem(const QString& theModuleID) const
{
  for (int moduleIdx = 0; moduleIdx < topLevelItemCount(); moduleIdx++) {
    SUIT_FoundActionTreeModule* moduleItem = static_cast<SUIT_FoundActionTreeModule*>(topLevelItem(moduleIdx));
    if (moduleItem->myModuleID == theModuleID)
      return std::pair<SUIT_FoundActionTreeModule*, int>(moduleItem, moduleIdx);
  }
  return std::pair<SUIT_FoundActionTreeModule*, int>(nullptr, -1);
}

template <typename Float>
bool approximatelyEqual(Float a, Float b, Float relativeTol = std::numeric_limits<Float>::epsilon())
{
    return std::abs(a - b) <= ( (std::abs(a) < std::abs(b) ? std::abs(b) : std::abs(a)) * relativeTol);
}

std::set<SUIT_FoundActionTreeAction*, std::function<bool(SUIT_FoundActionTreeAction*, SUIT_FoundActionTreeAction*)>> SUIT_FoundActionTree::createActionSetWithComparator() const
{
  QList<std::pair<SUIT_FoundActionTree::SortKey, SUIT_FoundActionTree::SortOrder>> sortSchema = SUIT_FoundActionTree::DEFAULT_SORT_SCHEMA;
  {
    for (auto itSameKey = sortSchema.begin(); itSameKey != sortSchema.end(); itSameKey++) {
      if (itSameKey->first == mySortKey) {
        sortSchema.erase(itSameKey);
        break;
      }
    }
    sortSchema.push_front(std::pair<SUIT_FoundActionTree::SortKey, SUIT_FoundActionTree::SortOrder>(mySortKey, mySortOrder));
  }

  static const QCollator collator;
  const std::function<bool(SUIT_FoundActionTreeAction*, SUIT_FoundActionTreeAction*)> comparator =
  [sortSchema, &collator](const SUIT_FoundActionTreeAction* theItemA, const SUIT_FoundActionTreeAction* theItemB) {
    for (const auto& keyAndOrder : sortSchema) {
      const QVariant fieldOfA = theItemA->getValue(keyAndOrder.first);
      const QVariant fieldOfB = theItemB->getValue(keyAndOrder.first);

      bool* const fieldOfAIsDouble = new bool(false);
      bool* const fieldOfBIsDouble = new bool(false);
      const double matchMetricsA = fieldOfA.toDouble(fieldOfAIsDouble);
      const double matchMetricsB = fieldOfB.toDouble(fieldOfBIsDouble);
      if (*fieldOfAIsDouble && *fieldOfBIsDouble) {
        if (!approximatelyEqual(matchMetricsA, matchMetricsB)) {
          const double res = matchMetricsA - matchMetricsB;
          return keyAndOrder.second == SUIT_FoundActionTree::SortOrder::Ascending ? res < 0 : res > 0;
        }
      }
      else {
        const int res = collator.compare(fieldOfA.toString(), fieldOfB.toString());
        if (res != 0)
          return keyAndOrder.second == SUIT_FoundActionTree::SortOrder::Ascending ? res < 0 : res > 0;
      }
    }
    return false;
  };

  return std::set<SUIT_FoundActionTreeAction*, std::function<bool(SUIT_FoundActionTreeAction*, SUIT_FoundActionTreeAction*)>>(comparator);
}

void SUIT_FoundActionTree::onItemExecuted(QTreeWidgetItem* theItem, int theColIdx)
{
  SUIT_FoundActionTreeItem* const item = static_cast<SUIT_FoundActionTreeItem*>(theItem);
  if (item->type() == SUIT_FoundActionTreeItem::Type::Action) {
    SUIT_FoundActionTreeAction* const actionItem = static_cast<SUIT_FoundActionTreeAction*>(theItem);
    if (actionItem->trigger())
      static_cast<SUIT_FindActionDialog*>(parentWidget())->accept();
  }
  else /* if (item->type() == SUIT_FoundActionTreeItem::Type::Module) */ {
    item->setExpanded(!item->isExpanded());
  }
}

/*static*/ const QList<std::pair<SUIT_FoundActionTree::SortKey, SUIT_FoundActionTree::SortOrder>> SUIT_FoundActionTree::DEFAULT_SORT_SCHEMA =
{
  {SUIT_FoundActionTree::SortKey::MatchMetrics, SUIT_FoundActionTree::SortOrder::Ascending},
  {SUIT_FoundActionTree::SortKey::Name, SUIT_FoundActionTree::SortOrder::Ascending},
  {SUIT_FoundActionTree::SortKey::ToolTip, SUIT_FoundActionTree::SortOrder::Ascending},
  {SUIT_FoundActionTree::SortKey::ID, SUIT_FoundActionTree::SortOrder::Ascending}
};


SUIT_FoundActionTreeItem::SUIT_FoundActionTreeItem(const QString& theModuleID)
: QTreeWidgetItem(), myModuleID(theModuleID)
{ }

QString SUIT_FoundActionTreeItem::name() const
{
  return text(SUIT_FoundActionTree::ElementIdx::Name);
}

QString SUIT_FoundActionTreeItem::toolTip() const
{
  return text(SUIT_FoundActionTree::ElementIdx::ToolTip);
}


SUIT_FoundActionTreeModule::SUIT_FoundActionTreeModule(const QString& theModuleID)
: SUIT_FoundActionTreeItem(theModuleID)
{
  QFont f = font(SUIT_FoundActionTree::ElementIdx::Name);
  f.setBold(true);
  setFont(SUIT_FoundActionTree::ElementIdx::Name, f);
  setText(SUIT_FoundActionTree::ElementIdx::Name, theModuleID);
}

void SUIT_FoundActionTreeModule::setAssetsAndSearchData(const SUIT_ActionSearcher::AssetsAndSearchData& theAssetsAndSD, const QString& theLang)
{
  if (!theAssetsAndSD.myAssets)
    return;

  setIcon(SUIT_FoundActionTree::ElementIdx::Name, theAssetsAndSD.myAssets->myIcon);

  const auto& ldaMap = theAssetsAndSD.myAssets->myLangDependentAssets;
  if (ldaMap.empty()) {
    setText(SUIT_FoundActionTree::ElementIdx::Name, myModuleID);
    return;
  }

  auto itLDA = ldaMap.find(theLang);
  if (itLDA == ldaMap.end())
    itLDA = ldaMap.begin();

  const SUIT_ActionAssets::LangDependentAssets& lda = itLDA->second;
  const QString& name = lda.myName.isEmpty() ? myModuleID : lda.myName;
  setText(SUIT_FoundActionTree::ElementIdx::Name, name);
}

QVariant SUIT_FoundActionTreeModule::getValue(SUIT_FoundActionTree::SortKey theKey) const
{
  switch (theKey) {
    case SUIT_FoundActionTree::SortKey::MatchMetrics:
      return double(0);
    case SUIT_FoundActionTree::SortKey::ID:
      return myModuleID;
    case SUIT_FoundActionTree::SortKey::Name:
      return name();
    case SUIT_FoundActionTree::SortKey::ToolTip:
      return toolTip();
    default:
      return QString();
  }
}

bool SUIT_FoundActionTreeModule::isEnabled() const
{
  return true;
}


SUIT_FoundActionTreeAction::SUIT_FoundActionTreeAction(const QString& theModuleID, const QString& theInModuleActionID)
: SUIT_FoundActionTreeItem(theModuleID), myInModuleActionID(theInModuleActionID),
  myMatchMetrics(std::numeric_limits<double>::infinity()), myIsEnabledBufferedValue(false)
{
  setText(SUIT_FoundActionTree::ElementIdx::Name, theInModuleActionID);
}

/*static*/ SUIT_FoundActionTreeAction* SUIT_FoundActionTreeAction::create(const QString& theModuleID, const QString& theInModuleActionID)
{
  if (theInModuleActionID.isEmpty()) {
    ShCutDbg("SUIT_FoundActionTreeItem: attempt to create item with empty action ID.");
    return nullptr;
  }

  return new SUIT_FoundActionTreeAction(theModuleID, theInModuleActionID);
}

void SUIT_FoundActionTreeAction::setAssetsAndSearchData(const SUIT_ActionSearcher::AssetsAndSearchData& theAssetsAndSD, const QString& theLang)
{
  if (!theAssetsAndSD.myAssets)
    return;

  setIcon(SUIT_FoundActionTree::ElementIdx::Name, theAssetsAndSD.myAssets->myIcon);

  const auto& ldaMap = theAssetsAndSD.myAssets->myLangDependentAssets;
  if (ldaMap.empty()) {
    setText(SUIT_FoundActionTree::ElementIdx::Name, myInModuleActionID);
    return;
  }

  auto itLDA = ldaMap.find(theLang);
  if (itLDA == ldaMap.end())
    itLDA = ldaMap.begin();

  const SUIT_ActionAssets::LangDependentAssets& lda = itLDA->second;
  const QString& name = lda.myName.isEmpty() ? myInModuleActionID : lda.myName;
  setText(SUIT_FoundActionTree::ElementIdx::Name, name);

  setText(SUIT_FoundActionTree::ElementIdx::ToolTip, lda.myToolTip);

  if (isEnabled()) {
    setToolTip(
      SUIT_FoundActionTree::ElementIdx::Name,
      SUIT_FoundActionTree::tr("Double click to start")
    );

    setToolTip(
      SUIT_FoundActionTree::ElementIdx::ToolTip,
      SUIT_FoundActionTree::tr("Double click to start")
    );
  }
  else {
    static const QBrush greyedOutBrush = QBrush(Qt::gray);
    setForeground(SUIT_FoundActionTree::ElementIdx::Name,    greyedOutBrush);
    setForeground(SUIT_FoundActionTree::ElementIdx::ToolTip, greyedOutBrush);
  }

  myMatchMetrics = theAssetsAndSD.matchMetrics();
}

QVariant SUIT_FoundActionTreeAction::getValue(SUIT_FoundActionTree::SortKey theKey) const
{
  switch (theKey) {
    case SUIT_FoundActionTree::SortKey::MatchMetrics:
      return myMatchMetrics;
    case SUIT_FoundActionTree::SortKey::ID:
      return myInModuleActionID;
    case SUIT_FoundActionTree::SortKey::Name:
      return name();
    case SUIT_FoundActionTree::SortKey::ToolTip:
      return toolTip();
    default:
      return QString();
  }
}

bool SUIT_FoundActionTreeAction::isEnabled() const
{
  const auto& actions = SUIT_ShortcutMgr::get()->getActions(myModuleID, myInModuleActionID);
  myIsEnabledBufferedValue = std::find_if(actions.begin(), actions.end(), [](const QAction* const theAction){ return theAction->isEnabled(); }) != actions.end();
  return myIsEnabledBufferedValue;
}

bool SUIT_FoundActionTreeAction::trigger() const
{
  bool res = false;
  const auto& actions = SUIT_ShortcutMgr::get()->getActions(myModuleID, myInModuleActionID);
  for (const auto& action : actions) {
    if (action->isEnabled()) {
      action->trigger();
      res = true;
    }
  }
  return res;
}