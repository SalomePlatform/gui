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

#include "SUIT_ShortcutMgr.h"

#include "SUIT_Session.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_MessageBox.h"

#include <QAction>
#include <QtxAction.h>

#include <QApplication>
#include <QActionEvent>
#include <QKeySequence>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QProcessEnvironment>
#include <QRegExp>
#include <Qtx.h>

#include <list>


#include <iostream>
#include <string>
const std::wstring SHORTCUT_MGR_LOG_PREFIX = L"SHORTCUT_MGR_DBG: ";
bool ShCutDbg(const QString& theString)
{
  if (ShCutDbg()) {
    std::wcout << SHORTCUT_MGR_LOG_PREFIX << theString.toStdWString() << std::endl;
    return true;
  }
  return false;
}
bool ShCutDbg(const char* src)
{
  if (ShCutDbg()) {
    std::wcout << SHORTCUT_MGR_LOG_PREFIX << std::wstring(src, src + strlen(src)) << std::endl;
    return true;
  }
  return false;
}

void Warning(const QString& theString)
{
  std::wcout << theString.toStdWString() << std::endl;
}
void Warning(const char* src)
{
  std::wcout << std::wstring(src, src + strlen(src)) << std::endl;
}


static const QKeySequence NO_KEYSEQUENCE = QKeySequence(QString(""));
static const QString NO_ACTION = QString("");
/** Separates tokens in action ID. */
static const QString TOKEN_SEPARATOR = QString("/");
static const QString ROOT_MODULE_ID = QString("");
static const QString META_ACTION_PREFIX = QString("#");

/** Prefix of names of shortcut setting sections in preference files. */
static const QString SECTION_NAME_PREFIX = QString("shortcuts");


const QString DEFAULT_LANG = QString("en");
const QStringList LANG_PRIORITY_LIST = QStringList({DEFAULT_LANG, "fr"});
const QString LANG_SECTION = QString("language");

static const QString SECTION_NAME_ACTION_ASSET_FILE_PATHS = QString("action_assets");



/**
 * Uncomment this, to start collecting all shortcuts and action assets (1),
 * from instances of QtxActions, if a shortcut or action assets are absent in resource/asset files.
 *
 * (1) Set required language in the application settings and run features of interest.
 * For all actions from these features, their assets will be dumped to appropriate places in dump files.
 *
 * Content of dump files is appended on every run. Files are located in "<APP_DIR>/shortcut_mgr_dev/".
*/
// #define SHORTCUT_MGR_DEVTOOLS
#ifdef SHORTCUT_MGR_DEVTOOLS
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "QtxMap.h"
#include <functional>
#ifndef QT_NO_DOM
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#endif // QT_NO_DOM

/*! \brief Generates XML files with appearing at runtime shortcuts,
    using key sequences of QActions passed to the shortcut manager,
    and JSON files with assets of QtxActions passed to the shortcut manager.
    Content of these files can be easily copied to resource/asset files. */
class DevTools
{
private:
  DevTools() : myActionsWithInvalidIDsFile(nullptr) {};
  DevTools(const DevTools&) = delete;
  void operator=(const DevTools&) = delete;

public:
  ~DevTools()
  {
    for (const auto& fileNameAndPtrs : myXMLFilesAndDocs) {
      delete fileNameAndPtrs.second.second;
      delete fileNameAndPtrs.second.first;
    }

    for (const auto& fileNameAndPtrs : myJSONFilesAndDocs) {
      delete fileNameAndPtrs.second.second;
      delete fileNameAndPtrs.second.first;
    }
  }

  static DevTools* get() {
    if (!DevTools::instance)
      DevTools::instance = new DevTools();

    return DevTools::instance;
  }

  void collectShortcut(
    const QString& theModuleID,
    const QString& theInModuleActionID,
    const QKeySequence& theKeySequence
  ) {
    if (SUIT_ShortcutMgr::isInModuleMetaActionID(theInModuleActionID)) {
      auto& moduleShortcuts = myShortcutsOfMetaActions[theModuleID];
      moduleShortcuts[theInModuleActionID] = theKeySequence.toString();

      const QString fileName = theModuleID + DevTools::SHORTCUTS_OF_META_SUFFIX;
      const QString sectionName = SECTION_NAME_PREFIX + DevTools::XML_SECTION_TOKENS_SEPARATOR + ROOT_MODULE_ID;
      std::map<QString, std::map<QString, QString>> sections;
      sections[sectionName] = moduleShortcuts;
      writeToXMLFile(fileName, sections);
    }
    else {
      auto& moduleShortcuts = myShortcuts[theModuleID];
      moduleShortcuts[theInModuleActionID] = theKeySequence.toString();

      const QString fileName = theModuleID + DevTools::SHORTCUTS_SUFFIX;
      const QString sectionName = SECTION_NAME_PREFIX + DevTools::XML_SECTION_TOKENS_SEPARATOR + theModuleID;
      std::map<QString, std::map<QString, QString>> sections;
      sections[sectionName] = moduleShortcuts;
      writeToXMLFile(fileName, sections);
    }
  }

  void collectAssets(
    const QString& theModuleID,
    const QString& theInModuleActionID,
    const QString& theLang,
    const QAction* theAction
  ) {
    if (SUIT_ShortcutMgr::isInModuleMetaActionID(theInModuleActionID)) {
      QString actionID = SUIT_ShortcutMgr::makeActionID(ROOT_MODULE_ID, theInModuleActionID);
      // { actionID, assets } []
      auto& moduleAssets = myAssetsOfMetaActions[theModuleID];

      auto& actionAssets = moduleAssets[actionID];
      actionAssets.myLangDependentAssets[theLang].myName = theAction->text();
      actionAssets.myLangDependentAssets[theLang].myToolTip = theAction->statusTip();

      const QString fileName = theModuleID + DevTools::ASSETS_OF_META_SUFFIX;
      writeToJSONFile(fileName, actionID, actionAssets);
    }
    else {
      QString actionID = SUIT_ShortcutMgr::makeActionID(theModuleID, theInModuleActionID);
      // { actionID, assets } []
      auto& moduleAssets = myAssets[theModuleID];

      auto& actionAssets = moduleAssets[actionID];
      actionAssets.myLangDependentAssets[theLang].myName = theAction->text();
      actionAssets.myLangDependentAssets[theLang].myToolTip = theAction->statusTip();

      const QString fileName = theModuleID + DevTools::ASSETS_SUFFIX;
      writeToJSONFile(fileName, actionID, actionAssets);
    }
  }

  void collectShortcutAndAssets(const QtxAction* const theAction)
  {
    const auto moduleIDAndActionID = SUIT_ShortcutMgr::splitIntoModuleIDAndInModuleID(theAction->ID());
    if (moduleIDAndActionID.second.isEmpty())
      return;

    if (!SUIT_ShortcutMgr::get()->getShortcutContainer().hasShortcut(moduleIDAndActionID.first, moduleIDAndActionID.second))
      collectShortcut(moduleIDAndActionID.first, moduleIDAndActionID.second, theAction->shortcut());

    { // Collect action assets, if they are not provided in asset files.
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      if (!resMgr) {
        Warning("DevTools for SUIT_ShortcutMgr can't retrieve resource manager!");
        return;
      }

      const QString lang = resMgr->stringValue(LANG_SECTION, LANG_SECTION);
      if (lang.isEmpty())
        return;

      const auto& assetsInResources = SUIT_ShortcutMgr::getActionAssetsFromResources(theAction->ID());
      if (assetsInResources.first && assetsInResources.second.myLangDependentAssets.find(lang) != assetsInResources.second.myLangDependentAssets.end())
        return;

      collectAssets(moduleIDAndActionID.first, moduleIDAndActionID.second, lang, theAction);
    }
  }

private:
  /*! Appends new entries to content of dump files. */
  bool writeToXMLFile(const QString& theFileName, const std::map<QString, std::map<QString, QString>>& theSections)
  {
#ifdef QT_NO_DOM
  Warning("DebugTools for SUIT_ShortcutMgr can't create XML - #QT_NO_DOM is defined.");
  return false;
#else QT_NO_DOM
    static const QString DOC_TAG = "document";
    static const QString SECTION_TAG = "section";
    static const QString PARAMETER_TAG = "parameter";
    static const QString NAME_ATTR = "name";
    static const QString VAL_ATTR = "value";

    const auto itFileAndDoc = myXMLFilesAndDocs.find(theFileName);
    if (itFileAndDoc == myXMLFilesAndDocs.end()) {
      const QString fullPath = DevTools::SAVE_PATH + theFileName + ".xml";
      if (!Qtx::mkDir(QFileInfo(fullPath).absolutePath())) {
        myXMLFilesAndDocs[theFileName] = std::pair<QFile*, QDomDocument*>(nullptr, nullptr);
        return false;
      }

      QFile* file = new QFile(fullPath);
      if (!file->open(QFile::ReadWrite | QIODevice::Text)) {
        delete file;
        myXMLFilesAndDocs[theFileName] = std::pair<QFile*, QDomDocument*>(nullptr, nullptr);
        return false;
      }

      QDomDocument* dom = new QDomDocument(DOC_TAG);
      QTextStream instream(file);
      dom->setContent(instream.readAll());
      myXMLFilesAndDocs[theFileName] = std::pair<QFile*, QDomDocument*>(file, dom);
    }
    else if (itFileAndDoc->second.first == nullptr) {
      return false;
    }

    const auto fileAndDom = myXMLFilesAndDocs[theFileName];
    QFile* const file = fileAndDom.first;
    QDomDocument* const dom = fileAndDom.second;

    QDomElement doc = dom->documentElement();
    if (doc.isNull()) {
      *dom = QDomDocument(DOC_TAG);
      doc = dom->createElement(DOC_TAG);
      dom->appendChild(doc);
    }

    static const std::function<void(const std::map<QString, QString>&, QDomDocument&, QDomElement&)> mergeParamsToSection =
    [&](const std::map<QString, QString>& parameters, QDomDocument& dom, QDomElement& sectionInDom)
    {
      for (const std::pair<QString, QString>& nameAndVal : parameters) {
        const QString& paramName = nameAndVal.first;
        const QString& paramVal = nameAndVal.second;
        bool fileHasParam = false;
        for (QDomElement paramInDom = sectionInDom.firstChildElement(PARAMETER_TAG); !paramInDom.isNull(); paramInDom = paramInDom.nextSiblingElement(PARAMETER_TAG)) {
          const QString paramNameInDom = paramInDom.attribute(NAME_ATTR);
          if (paramName == paramNameInDom) {
            const QString paramValInDom = paramInDom.attribute(VAL_ATTR);
            if (paramValInDom != paramVal) {
              QDomElement replaceElement = dom.createElement(PARAMETER_TAG);
              replaceElement.setAttribute(NAME_ATTR, paramName);
              replaceElement.setAttribute(VAL_ATTR, paramVal);
              sectionInDom.replaceChild(replaceElement, paramInDom);
            }

            fileHasParam = true;
            break;
          }
        }
        if (!fileHasParam) {
          QDomElement newParam = dom.createElement(PARAMETER_TAG);
          newParam.setAttribute(NAME_ATTR, paramName);
          newParam.setAttribute(VAL_ATTR, paramVal);
          sectionInDom.insertAfter(newParam, sectionInDom.lastChildElement(PARAMETER_TAG));
        }
      }
      return;
    };

    for (const auto& sectionNameAndParams : theSections) {
      const QString& sectionName = sectionNameAndParams.first;
      const std::map<QString, QString>& parameters = sectionNameAndParams.second;

      bool fileHasSection = false;
      for (QDomElement sectionInDom = doc.firstChildElement(SECTION_TAG); !sectionInDom.isNull(); sectionInDom = sectionInDom.nextSiblingElement(SECTION_TAG)) {
        QString sectionNameInDom = sectionInDom.attribute(NAME_ATTR);
        if (sectionNameInDom == sectionName) {
          mergeParamsToSection(parameters, *dom, sectionInDom);
          fileHasSection = true;
          break;
        }
      }

      if (!fileHasSection) {
        QDomElement newSection = dom->createElement(SECTION_TAG);
        newSection.setAttribute(NAME_ATTR, sectionName);
        doc.insertAfter(newSection, doc.lastChildElement(SECTION_TAG));
        mergeParamsToSection(parameters, *dom, newSection);
      }
    }

    file->resize(0);
    QTextStream outstream(file);
    outstream << dom->toString();

    return true;
#endif // QT_NO_DOM
  }

  /*! Appends new entries to content of dump files. */
  bool writeToJSONFile(const QString& theFileName, const QString& theActionID, const SUIT_ActionAssets& theAssets)
  {
    const auto itFileAndDoc = myJSONFilesAndDocs.find(theFileName);
    if (itFileAndDoc == myJSONFilesAndDocs.end()) {
      const QString fullPath = DevTools::SAVE_PATH + theFileName + ".json";
      if (!Qtx::mkDir(QFileInfo(fullPath).absolutePath())) {
        myJSONFilesAndDocs[theFileName] = std::pair<QFile*, QJsonDocument*>(nullptr, nullptr);
        return false;
      }

      const bool fileExisted = QFileInfo::exists(fullPath);
      QFile* file = new QFile(fullPath);
      if (!file->open(QFile::ReadWrite | QIODevice::Text)) {
        delete file;
        myJSONFilesAndDocs[theFileName] = std::pair<QFile*, QJsonDocument*>(nullptr, nullptr);
        return false;
      }

      QJsonParseError jsonError;
      QJsonDocument* document = new QJsonDocument(QJsonDocument::fromJson(file->readAll(), &jsonError));
      if (jsonError.error != QJsonParseError::NoError && fileExisted) {
        Warning("SUIT_ShortcutMgr: error during parsing of action asset dump file \"" + fullPath + "\"!");
        delete file;
        delete document;
        myJSONFilesAndDocs[theFileName] = std::pair<QFile*, QJsonDocument*>(nullptr, nullptr);
        return false;
      }

      if (!document->isObject()) {
        document->setObject(QJsonObject());
        file->resize(0);
        QTextStream outstream(file);
        outstream << document->toJson(QJsonDocument::Indented);
      }

      myJSONFilesAndDocs[theFileName] = std::pair<QFile*, QJsonDocument*>(file, document);
    }
    else if (itFileAndDoc->second.first == nullptr) {
      return false;
    }

    const auto fileAndDoc = myJSONFilesAndDocs[theFileName];
    QFile* const file = fileAndDoc.first;
    QJsonDocument* const document = fileAndDoc.second;

    QJsonObject rootJSON = document->object();
    QJsonObject actionAssetsJSON = rootJSON[theActionID].toObject();
    SUIT_ActionAssets actionAssets;
    actionAssets.fromJSON(actionAssetsJSON);
    actionAssets.merge(theAssets, true /*theOverride*/);
    actionAssets.toJSON(actionAssetsJSON);
    rootJSON[theActionID] = actionAssetsJSON;
    document->setObject(rootJSON);

    file->resize(0);
    QTextStream outstream(file);
    outstream << document->toJson(QJsonDocument::Indented);

    return true;
  }

public:
  void collectAssetsOfActionWithInvalidID(const QAction* const theAction)
  {
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    if (!resMgr) {
      Warning("DevTools for SUIT_ShortcutMgr can't retrieve resource manager!");
      return;
    }

    const QString lang = resMgr->stringValue(LANG_SECTION, LANG_SECTION);
    if (lang.isEmpty())
      return;

    if (!myActionsWithInvalidIDsFile) {
      const QString fullPath = DevTools::SAVE_PATH + lang + DevTools::INVALID_ID_ACTIONS_SUFFIX + ".csv";
      if (!Qtx::mkDir(QFileInfo(fullPath).absolutePath()))
        return;

      myActionsWithInvalidIDsFile = new QFile(fullPath);
      if (!myActionsWithInvalidIDsFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        delete myActionsWithInvalidIDsFile;
        myActionsWithInvalidIDsFile = nullptr;
        return;
      }

      QTextStream ostream(myActionsWithInvalidIDsFile);
      ostream << "text\t" << "tool tip\t" << "status tip\t" << "key sequence\t" << "QtxAction?\t" << "ID\n";
      ostream.flush();
    }

    QTextStream ostream(myActionsWithInvalidIDsFile);
    const auto aQtxAction = qobject_cast<const QtxAction*>(theAction);
    ostream << theAction->text() << "\t" << theAction->toolTip() << "\t" << theAction->statusTip() << "\t"
    << theAction->shortcut().toString() << "\t" << (aQtxAction ? "yes\t" : "no\t") << (aQtxAction ? aQtxAction->ID() + "\n" : "\n");
    ostream.flush();
  }

  static const QString SAVE_PATH;
  static const QString SHORTCUTS_SUFFIX;
  static const QString SHORTCUTS_OF_META_SUFFIX;
  static const QString ASSETS_SUFFIX;
  static const QString ASSETS_OF_META_SUFFIX;
  static const QString INVALID_ID_ACTIONS_SUFFIX;

  static DevTools* instance;
  static const QString XML_SECTION_TOKENS_SEPARATOR;

  /** { moduleID, { inModuleActionID, keySequence }[] }[]. keySequence can be empty. */
  std::map<QString, std::map<QString, QString>> myShortcuts;

  /** { moduleID, { inModuleActionID, keySequence }[] }[]. keySequence can be empty. */
  std::map<QString, std::map<QString, QString>> myShortcutsOfMetaActions;

  /** { moduleID, { actionID, assets }[] }[] */
  std::map<QString, std::map<QString, SUIT_ActionAssets>> myAssets;

  /** { moduleID, { actionID, assets }[] }[] */
  std::map<QString, std::map<QString, SUIT_ActionAssets>> myAssetsOfMetaActions;

#ifndef QT_NO_DOM
  // { filename, {file, domDoc} }[]
  std::map<QString, std::pair<QFile*, QDomDocument*>> myXMLFilesAndDocs;
#endif // QT_NO_DOM
  // { filename, {file, jsonDoc} }[]
  std::map<QString, std::pair<QFile*, QJsonDocument*>> myJSONFilesAndDocs;

  QFile* myActionsWithInvalidIDsFile;
};
/*static*/ DevTools* DevTools::instance = nullptr;
/*static*/ const QString DevTools::SAVE_PATH = "shortcut_mgr_dev/";
/*static*/ const QString DevTools::INVALID_ID_ACTIONS_SUFFIX = "_actions_with_invalid_IDs";
/*static*/ const QString DevTools::XML_SECTION_TOKENS_SEPARATOR = ":";
/*static*/ const QString DevTools::SHORTCUTS_SUFFIX = "_shortcuts";
/*static*/ const QString DevTools::SHORTCUTS_OF_META_SUFFIX = "_shortcuts_of_meta_actions";
/*static*/ const QString DevTools::ASSETS_SUFFIX = "_assets";
/*static*/ const QString DevTools::ASSETS_OF_META_SUFFIX = "_assets_of_meta_actions";
#endif // SHORTCUT_MGR_DEVTOOLS



SUIT_ShortcutContainer::SUIT_ShortcutContainer()
{
  myShortcuts.emplace(ROOT_MODULE_ID, std::map<QKeySequence, QString>());
  myShortcutsInversed.emplace(ROOT_MODULE_ID, std::map<QString, QKeySequence>());
}

std::set<QString> SUIT_ShortcutContainer::getIDsOfInterferingModules(const QString& theModuleID) const
{
  std::set<QString> IDsOfInterferingModules;
  if (theModuleID == ROOT_MODULE_ID) {
    for (const auto& moduleIDAndShortcuts : myShortcuts) {
      IDsOfInterferingModules.emplace(moduleIDAndShortcuts.first);
    }
  }
  else {
    IDsOfInterferingModules.emplace(ROOT_MODULE_ID);
    if (theModuleID != ROOT_MODULE_ID)
      IDsOfInterferingModules.emplace(theModuleID);
  }
  return IDsOfInterferingModules;
}

std::set<QString> SUIT_ShortcutContainer::getIDsOfAllModules() const
{
  std::set<QString> res;
  for (const auto& moduleIDAndShortcuts : myShortcutsInversed) {
    res.emplace(moduleIDAndShortcuts.first);
  }
  return res;
}

std::set<std::pair<QString, QString>> SUIT_ShortcutContainer::setShortcut(QString theModuleID, const QString& theInModuleActionID, const QKeySequence& theKeySequence, bool theOverride)
{
  if (!SUIT_ShortcutMgr::isModuleIDValid(theModuleID)) {
    ShCutDbg() && ShCutDbg("Attempt to define a shortcut using invalid module ID = \"" + theModuleID + "\".");
    return std::set<std::pair<QString, QString>>();
  }

  if (!SUIT_ShortcutMgr::isInModuleActionIDValid(theInModuleActionID)) {
    ShCutDbg() && ShCutDbg("Attempt to define a shortcut using invalid in-module action ID = \"" + theInModuleActionID + "\".");
    return std::set<std::pair<QString, QString>>();
  }

  if (SUIT_ShortcutMgr::isInModuleMetaActionID(theInModuleActionID))
    theModuleID = ROOT_MODULE_ID;

  auto itModuleShortcuts = myShortcuts.find(theModuleID);
  auto itModuleShortcutsInversed = myShortcutsInversed.find(theModuleID);
  if (itModuleShortcuts == myShortcuts.end()) {
    itModuleShortcuts = myShortcuts.emplace(theModuleID, std::map<QKeySequence, QString>()).first;
    itModuleShortcutsInversed = myShortcutsInversed.emplace(theModuleID, std::map<QString, QKeySequence>()).first;
  }

  std::map<QKeySequence, QString>& moduleShortcuts = itModuleShortcuts->second;
  std::map<QString, QKeySequence>& moduleShortcutsInversed = itModuleShortcutsInversed->second;

  if (theKeySequence.isEmpty()) {
    // Disable shortcut.

    auto itShortcutInversed = moduleShortcutsInversed.find(theInModuleActionID);
    if (itShortcutInversed == moduleShortcutsInversed.end()) {
      // No key sequence was mapped to the action earlier.
      // Set disabled shortcut.
      moduleShortcutsInversed.emplace(theInModuleActionID, NO_KEYSEQUENCE);
      return std::set<std::pair<QString, QString>>();
    }
    else /* if keySequence was mapped to the action earlier. */ {
      QKeySequence& keySequence = itShortcutInversed->second;

      moduleShortcuts.erase(keySequence);
      keySequence = NO_KEYSEQUENCE;

      return std::set<std::pair<QString, QString>>();
    }
  }

  { // Check if the shortcut is already set.
    const auto itShortcut = moduleShortcuts.find(theKeySequence);
    if (itShortcut != moduleShortcuts.end()) {
      if (itShortcut->second == theInModuleActionID) {
        // The shortcut was set earlier. Nothing to change.
        return std::set<std::pair<QString, QString>>();
      }
    }
  }

  auto conflictingActionIDs = std::set<std::pair<QString, QString>>();
  { // Look for conflicting shortcuts with the same key sequence from interfering modules.
    std::set<QString> IDsOfInterferingModules = getIDsOfInterferingModules(theModuleID);
    for (const QString& IDOfInterferingModule : IDsOfInterferingModules) {
      std::map<QKeySequence, QString>& shortcutsOfInterferingModule = myShortcuts.at(IDOfInterferingModule);
      auto itConflictingShortcut = shortcutsOfInterferingModule.find(theKeySequence);
      if (itConflictingShortcut != shortcutsOfInterferingModule.end()) {
        const QString& conflictingActionID = itConflictingShortcut->second;

        conflictingActionIDs.insert(std::pair<QString, QString>(IDOfInterferingModule, conflictingActionID));

        if (theOverride) {
          // Disable conflicting shortcuts.
          std::map<QString, QKeySequence>& shortcutsOfInterferingModuleInversed = myShortcutsInversed.at(IDOfInterferingModule);
          shortcutsOfInterferingModuleInversed[conflictingActionID] = NO_KEYSEQUENCE;
          shortcutsOfInterferingModule.erase(itConflictingShortcut);
        }
      }
    }

    if (!theOverride && !conflictingActionIDs.empty())
      return conflictingActionIDs;
  }

  { // Ensure, that the module has only shortcut for the action ID.
    auto itShortcutInversed = moduleShortcutsInversed.find(theInModuleActionID);
    if (itShortcutInversed != moduleShortcutsInversed.end()) {
      // Redefine key sequence for existing action.

      QKeySequence& keySequence = itShortcutInversed->second;

      moduleShortcuts.erase(keySequence);
      moduleShortcuts[theKeySequence] = theInModuleActionID;

      keySequence = theKeySequence;
    }
    else /* if the action has not been added earlier. */ {
      moduleShortcuts[theKeySequence] = theInModuleActionID;
      moduleShortcutsInversed[theInModuleActionID] = theKeySequence;
    }
  }

  return conflictingActionIDs;
}

std::set<std::pair<QString, QString>> SUIT_ShortcutContainer::getConflicts(
  QString theModuleID,
  const QString& theInModuleActionID,
  const QKeySequence& theKeySequence
) const
{
  if (theKeySequence.isEmpty())
    return std::set<std::pair<QString, QString>>();

  if (SUIT_ShortcutMgr::isInModuleMetaActionID(theInModuleActionID))
    theModuleID = ROOT_MODULE_ID;

  { // Check if the shortcut is set.
    const auto itModuleShortcuts = myShortcuts.find(theModuleID);
    if (itModuleShortcuts != myShortcuts.end()) {
      const std::map<QKeySequence, QString>& moduleShortcuts = itModuleShortcuts->second;
      const auto itShortcut = moduleShortcuts.find(theKeySequence);
      if (itShortcut != moduleShortcuts.end()) {
        if (itShortcut->second == theInModuleActionID) {
          // The shortcut is set => no conflicts.
          return std::set<std::pair<QString, QString>>();
        }
      }
    }
  }

  auto conflictingActionIDs = std::set<std::pair<QString, QString>>();
  { // Look for conflicting shortcuts with the same key sequence from interfering modules.
    std::set<QString> IDsOfInterferingModules = getIDsOfInterferingModules(theModuleID);
    for (const QString& IDOfInterferingModule : IDsOfInterferingModules) {
      const std::map<QKeySequence, QString>& shortcutsOfInterferingModule = myShortcuts.at(IDOfInterferingModule);
      const auto itConflictingShortcut = shortcutsOfInterferingModule.find(theKeySequence);
      if (itConflictingShortcut != shortcutsOfInterferingModule.end()) {
        const QString& conflictingActionID = itConflictingShortcut->second;
        conflictingActionIDs.insert(std::pair<QString, QString>(IDOfInterferingModule, conflictingActionID));
      }
    }
  }
  return conflictingActionIDs;
}

const QKeySequence& SUIT_ShortcutContainer::getKeySequence(QString theModuleID, const QString& theInModuleActionID) const
{
  if (SUIT_ShortcutMgr::isInModuleMetaActionID(theInModuleActionID))
    theModuleID = ROOT_MODULE_ID;

  const auto itModuleShortcutsInversed = myShortcutsInversed.find(theModuleID);
  if (itModuleShortcutsInversed == myShortcutsInversed.end())
    return NO_KEYSEQUENCE;

  const auto& moduleShortcutsInversed = itModuleShortcutsInversed->second;
  const auto itShortcutInversed = moduleShortcutsInversed.find(theInModuleActionID);
  if (itShortcutInversed == moduleShortcutsInversed.end())
    return NO_KEYSEQUENCE;

  return itShortcutInversed->second;
}

bool SUIT_ShortcutContainer::hasShortcut(QString theModuleID, const QString& theInModuleActionID) const
{
  if (SUIT_ShortcutMgr::isInModuleMetaActionID(theInModuleActionID))
    theModuleID = ROOT_MODULE_ID;

  const auto itModuleShortcutsInversed = myShortcutsInversed.find(theModuleID);
  if (itModuleShortcutsInversed == myShortcutsInversed.end())
    return false;

  const auto& moduleShortcutsInversed = itModuleShortcutsInversed->second;
  const auto itShortcutInversed = moduleShortcutsInversed.find(theInModuleActionID);
  if (itShortcutInversed == moduleShortcutsInversed.end())
    return false;

  return true;
}

const std::map<QString, QKeySequence>& SUIT_ShortcutContainer::getModuleShortcutsInversed(const QString& theModuleID) const
{
  static const std::map<QString, QKeySequence> EMPTY_RES;
  const auto it = myShortcutsInversed.find(theModuleID);
  if (it == myShortcutsInversed.end())
    return EMPTY_RES;

  return it->second;
}

const std::map<QString, QKeySequence> SUIT_ShortcutContainer::getModuleShortcutsInversed(const QString& theModuleID, const QString& theActionIDPrefix) const
{
  const auto it = myShortcutsInversed.find(theModuleID);
  if (it == myShortcutsInversed.end())
    return std::map<QString, QKeySequence>();

  std::map<QString, QKeySequence> shortcutsInversed;
  for (const auto& existingShortcut : it->second) {
    if (existingShortcut.first.startsWith(theActionIDPrefix))
      shortcutsInversed[existingShortcut.first] = existingShortcut.second;
  }
  return shortcutsInversed;
}

QString SUIT_ShortcutContainer::toString() const
{
  QString text;
  text += "Shortcuts inversed:\n";
  for (auto it = myShortcutsInversed.begin(); it != myShortcutsInversed.end(); it++) {
    const QString& moduleID = it->first;
    const auto& moduleShortcuts = it->second;
    text += (it == myShortcutsInversed.begin() ? "\"" : "\n\"")  + moduleID + "\"";
    for (const auto& shortcut : moduleShortcuts) {
      text += "\n\t\"" + shortcut.first + "\"\t\"" + shortcut.second.toString() + "\"";
    }
  }
  text += "\nShortcuts:\n";
  for (auto it = myShortcuts.begin(); it != myShortcuts.end(); it++) {
    const QString& moduleID = it->first;
    const auto& moduleShortcuts = it->second;
    text += (it == myShortcuts.begin() ? "\"" : "\n\"")  + moduleID + "\"";
    for (const auto& shortcut : moduleShortcuts) {
      text += "\n\t\"" + shortcut.first.toString() + "\"\t\"" + shortcut.second + "\"";
    }
  }
  return text;
}

/*static*/ const QString SUIT_ActionAssets::LangDependentAssets::PROP_ID_NAME = "name";
/*static*/ const QString SUIT_ActionAssets::LangDependentAssets::PROP_ID_TOOLTIP = "tooltip";

bool SUIT_ActionAssets::LangDependentAssets::fromJSON(const QJsonObject& theJsonObject)
{
  myName    = theJsonObject[SUIT_ActionAssets::LangDependentAssets::PROP_ID_NAME].toString();
  myToolTip = theJsonObject[SUIT_ActionAssets::LangDependentAssets::PROP_ID_TOOLTIP].toString();

  if (myName.isEmpty())
    myName = myToolTip;

  return !myName.isEmpty();
}

void SUIT_ActionAssets::LangDependentAssets::toJSON(QJsonObject& oJsonObject) const
{
  oJsonObject[SUIT_ActionAssets::LangDependentAssets::PROP_ID_NAME] = myName;
  oJsonObject[SUIT_ActionAssets::LangDependentAssets::PROP_ID_TOOLTIP] = myToolTip;
}

/*static*/ const QString SUIT_ActionAssets::STRUCT_ID = "SUIT_ActionAssets";
/*static*/ const QString SUIT_ActionAssets::PROP_ID_LANG_DEPENDENT_ASSETS = "langDependentAssets";
/*static*/ const QString SUIT_ActionAssets::PROP_ID_ICON_PATH = "iconPath";

bool SUIT_ActionAssets::fromJSON(const QJsonObject& theJsonObject)
{
  myLangDependentAssets.clear();

  auto lda = SUIT_ActionAssets::LangDependentAssets();
  const auto& langToLdaJson = theJsonObject[SUIT_ActionAssets::PROP_ID_LANG_DEPENDENT_ASSETS].toObject();
  for (const QString& lang : langToLdaJson.keys()) {
    if (!lda.fromJSON(langToLdaJson[lang].toObject()))
      continue;

    myLangDependentAssets[lang] = lda;
  }

  myIconPath = theJsonObject[SUIT_ActionAssets::PROP_ID_ICON_PATH].toString();

  return !myLangDependentAssets.empty();
}

void SUIT_ActionAssets::toJSON(QJsonObject& oJsonObject) const
{
  auto langDependentAssetsJSON = QJsonObject();

  auto langDependentAssetsItemJSON = QJsonObject();
  for (const auto& langAndLDA : myLangDependentAssets) {
    langAndLDA.second.toJSON(langDependentAssetsItemJSON);
    langDependentAssetsJSON[langAndLDA.first] = langDependentAssetsItemJSON;
  }
  oJsonObject[SUIT_ActionAssets::PROP_ID_LANG_DEPENDENT_ASSETS] = langDependentAssetsJSON;

  oJsonObject[SUIT_ActionAssets::PROP_ID_ICON_PATH] = myIconPath;
}

QString SUIT_ActionAssets::toString() const
{
  QJsonObject jsonObject;
  toJSON(jsonObject);
  return QString::fromStdString(QJsonDocument(jsonObject).toJson(QJsonDocument::Indented).toStdString());
}

QStringList SUIT_ActionAssets::getLangs() const
{
  QStringList langs;

  for (const auto& langAndAssets : myLangDependentAssets) {
    langs.push_back(langAndAssets.first);
  }

  return langs;
}

void SUIT_ActionAssets::clearAllLangsExcept(const QString& theLang)
{
  for (auto it = myLangDependentAssets.begin(); it != myLangDependentAssets.end();) {
    if (it->first == theLang)
      it++;
    else
      it = myLangDependentAssets.erase(it);
  }
}

void SUIT_ActionAssets::merge(const SUIT_ActionAssets& theOther, bool theOverride)
{
  for (const auto& otherLangAndLDA : theOther.myLangDependentAssets) {
    const QString& lang = otherLangAndLDA.first;
    const auto& otherLDA = otherLangAndLDA.second;
    auto& thisLDA = myLangDependentAssets[lang];

    if (thisLDA.myName.isEmpty() || theOverride && !otherLDA.myName.isEmpty())
      thisLDA.myName = otherLDA.myName;

    if (thisLDA.myToolTip.isEmpty() || theOverride && !otherLDA.myToolTip.isEmpty())
      thisLDA.myToolTip = otherLDA.myToolTip;
  }

  if (theOverride)
    myIconPath = theOther.myIconPath;
}

std::map<QString, std::map<QString, QKeySequence>> SUIT_ShortcutContainer::merge(
  const SUIT_ShortcutContainer& theOther,
  bool theOverride,
  bool theTreatAbsentIncomingAsDisabled
) {
  std::map<QString, std::map<QString, QKeySequence>> changesOfThis;

  for (const auto& shortcutsInversedOfOtherPair : theOther.myShortcutsInversed) {
    const QString& moduleIDOther = shortcutsInversedOfOtherPair.first;
    const auto& shortcutsInversedOther = shortcutsInversedOfOtherPair.second;
    for (const auto& shortcutInversedOther : shortcutsInversedOther) {
      const QString& inModuleActionIDOther = shortcutInversedOther.first;
      const QKeySequence& keySequenceOther = shortcutInversedOther.second;
      if (theOverride) {
        if (hasShortcut(moduleIDOther, inModuleActionIDOther) && getKeySequence(moduleIDOther, inModuleActionIDOther) == keySequenceOther) {
          continue;
        }
        else /* if this has no shortcut for the action  or  if this has a shortcut for the action, but the key sequence differs. */ {
          const auto disabledActionsOfThis = setShortcut(moduleIDOther, inModuleActionIDOther, keySequenceOther, true);
          changesOfThis[moduleIDOther][inModuleActionIDOther] = keySequenceOther;
          for (const auto& disabledActionOfThis : disabledActionsOfThis) {
            changesOfThis[disabledActionOfThis.first][disabledActionOfThis.second] = NO_KEYSEQUENCE;
          }
        }
      }
      else /* if (!theOverride) */ {
        if (hasShortcut(moduleIDOther, inModuleActionIDOther))
          continue;
        else {
          const auto conflictingActionsOfThis = setShortcut(moduleIDOther, inModuleActionIDOther, keySequenceOther, false);
          if (conflictingActionsOfThis.empty()) {
            changesOfThis[moduleIDOther][inModuleActionIDOther] = keySequenceOther;
          }
          else /* if this has no shortcut for the action, but the incoming key sequence conflicts with others shortcuts. */ {
            changesOfThis[moduleIDOther][inModuleActionIDOther] = NO_KEYSEQUENCE;
          }
        }
      }
    }
  }

  if (theOverride && theTreatAbsentIncomingAsDisabled) {
    // Disable existing shortcuts, if they are absent in theOther.
    for (auto& shortcutsInversedPair : myShortcutsInversed) {
      const QString& moduleID = shortcutsInversedPair.first;
      auto& moduleShortcutsInversed = shortcutsInversedPair.second;
      for (auto& inversedShortcut : moduleShortcutsInversed) {
        if (theOther.hasShortcut(moduleID, inversedShortcut.first))
          continue;

        if (inversedShortcut.second.isEmpty())
          continue; // Existing shortcut is already disabled.

        auto itShortcutsPair = myShortcuts.find(moduleID);
        if (itShortcutsPair == myShortcuts.end())
          continue; // The check is an overhead in an error-free designed class, but let be just in case.

        auto& moduleShortcuts = itShortcutsPair->second;
        moduleShortcuts.erase(inversedShortcut.second);
        inversedShortcut.second = NO_KEYSEQUENCE;
        changesOfThis[moduleID][inversedShortcut.first] = NO_KEYSEQUENCE;
      }
    }
  }

  return changesOfThis;
}


SUIT_ShortcutMgr* SUIT_ShortcutMgr::myShortcutMgr = nullptr;

SUIT_ShortcutMgr::SUIT_ShortcutMgr()
: QObject()
{
  qApp->installEventFilter( this );
}

SUIT_ShortcutMgr::~SUIT_ShortcutMgr()
{
  qApp->removeEventFilter( this );
}

/*static*/ void SUIT_ShortcutMgr::Init()
{
  if( myShortcutMgr == nullptr) {
    myShortcutMgr = new SUIT_ShortcutMgr();
    myShortcutMgr->setShortcutsFromPreferences();
  }
}

/*static*/ SUIT_ShortcutMgr* SUIT_ShortcutMgr::get()
{
  Init();
  return myShortcutMgr;
}

/*static*/ bool SUIT_ShortcutMgr::isKeySequenceValid(const QKeySequence& theKeySequence)
{
  // TODO Perform check whether a key sequence is platform-compatible.
  return true;
}

/*static*/ std::pair<bool, QKeySequence> SUIT_ShortcutMgr::toKeySequenceIfValid(const QString& theKeySequenceString)
{
  auto res = std::pair<bool, QKeySequence>(false, QKeySequence());

  try {
    res.second = QKeySequence::fromString(theKeySequenceString);
    if (res.second.toString() != theKeySequenceString)
      return std::pair<bool, QKeySequence>(false, QKeySequence());

    if (!SUIT_ShortcutMgr::isKeySequenceValid(res.second))
      return std::pair<bool, QKeySequence>(false, QKeySequence());
  }
  catch (...) {
    return std::pair<bool, QKeySequence>(false, QKeySequence());
  }

  res.first = true;
  return res;
}

/*static*/ bool SUIT_ShortcutMgr::isModuleIDValid(const QString& theModuleID)
{
  if (theModuleID.contains(TOKEN_SEPARATOR))
    return false;

  if (theModuleID.simplified() != theModuleID)
    return false;

  return true;
}

/*static*/ bool SUIT_ShortcutMgr::isInModuleActionIDValid(const QString& theInModuleActionID)
{
  QStringList tokens = theInModuleActionID.split(TOKEN_SEPARATOR);
   for (QStringList::size_type i = 0; i < tokens.length(); i++) {
    const QString simplifiedToken = tokens[i].simplified();
    if (
      simplifiedToken.isEmpty() ||
      simplifiedToken != tokens[i] ||
      i == 0 && simplifiedToken == META_ACTION_PREFIX ||
      i != 0 && simplifiedToken.startsWith(META_ACTION_PREFIX)
    )
      return false;
  }
  return true;
}

/*static*/ bool SUIT_ShortcutMgr::isInModuleMetaActionID(const QString& theInModuleActionID)
{
  return theInModuleActionID.startsWith(META_ACTION_PREFIX);
}

/*static*/ std::pair<QString, QString> SUIT_ShortcutMgr::splitIntoModuleIDAndInModuleID(const QString& theActionID)
{
  QStringList tokens = theActionID.split(TOKEN_SEPARATOR);
  if (tokens.length() < 2)
    return std::pair<QString, QString>();

  auto res = std::pair<QString, QString>();

  if (tokens[0].simplified() != tokens[0])
    return std::pair<QString, QString>();

  res.first = tokens[0];
  tokens.pop_front();

  for (QStringList::size_type i = 0; i < tokens.length(); i++) {
    const QString simplifiedToken = tokens[i].simplified();
    if (
      simplifiedToken.isEmpty() ||
      simplifiedToken != tokens[i] ||
      i == 0 && simplifiedToken == META_ACTION_PREFIX ||
      i != 0 && simplifiedToken.startsWith(META_ACTION_PREFIX)
    )
      return std::pair<QString, QString>();
  }
  res.second = tokens.join(TOKEN_SEPARATOR);

  return res;
}

/*static*/ bool SUIT_ShortcutMgr::isActionIDValid(const QString& theActionID)
{
  return !SUIT_ShortcutMgr::splitIntoModuleIDAndInModuleID(theActionID).second.isEmpty();
}

/*static*/ QString SUIT_ShortcutMgr::makeActionID(const QString& theModuleID, const QString& theInModuleActionID)
{
  if (!SUIT_ShortcutMgr::isModuleIDValid(theModuleID))
    return QString();

  if (!isInModuleActionIDValid(theInModuleActionID))
    return QString();

  return theModuleID + TOKEN_SEPARATOR + theInModuleActionID;
}

/*static*/ void SUIT_ShortcutMgr::fillContainerFromPreferences(SUIT_ShortcutContainer& theContainer, bool theDefaultOnly)
{
  ShCutDbg() && ShCutDbg("Retrieving preferences from resources.");

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if (!resMgr) {
    Warning("SUIT_ShortcutMgr can't retrieve resource manager!");
    return;
  }

  const auto resMgrWorkingModeBefore = resMgr->workingMode();
  if (theDefaultOnly)
    resMgr->setWorkingMode(QtxResourceMgr::IgnoreUserValues);

  /** List of modules with invalid IDs. */
  QStringList invalidModuleIDs;

  /** { moduleID, {inModuleActionID, keySequence}[] }[] */
  std::map<QString, std::list<std::pair<QString, QString>>> invalidShortcuts;

  /**
   * Shortcuts, which have not been set, because they are in conflict with previously parsed shortcuts.
   * { moduleID, {inModuleActionID, keySequence}[] }[] */
  std::map<QString, std::list<std::pair<QString, QKeySequence>>> conflicts;

  // Resource manager strips leading and trailing whitespaces from subsections' names.
  // And then it is not able to retrieve parametes from that subsections,
  // because parsed subsection names differ from the ones in resource file.
  // Anyway, it does not affect operability of ShortcutMgr.
  QStringList moduleIDs = resMgr->subSections(SECTION_NAME_PREFIX, true);
  if (ShCutDbg()) {
    if (moduleIDs.isEmpty())
      ShCutDbg("No discovered shortcut modules.");
    else
      ShCutDbg("Discovered shortcut modules: \"" + moduleIDs.join("\", \"") + ".");
  }
  moduleIDs.push_front(ROOT_MODULE_ID); // Resource manager filters out empty section suffices.
  moduleIDs.removeDuplicates();

  for (size_t i = 0; i < moduleIDs.size(); i++) {
    const auto& moduleID = moduleIDs[i];
    if (!SUIT_ShortcutMgr::isModuleIDValid(moduleID)) {
      invalidModuleIDs.push_back(moduleID);
      continue;
    }

    const QString sectionName = SECTION_NAME_PREFIX + resMgr->sectionsToken() + moduleID;
    QStringList moduleActionIDs = resMgr->parameters(sectionName);

    for(const QString& inModuleActionID : moduleActionIDs) {
      QString keySequenceString = QString("");
      resMgr->value(sectionName, inModuleActionID, keySequenceString);
      const auto keySequence = SUIT_ShortcutMgr::toKeySequenceIfValid(keySequenceString);

      ShCutDbg() && ShCutDbg("Shortcut discovered: \"" + moduleID + "\"\t\"" + inModuleActionID + "\"\t\"" + keySequenceString + "\".");

      if (
        !SUIT_ShortcutMgr::isInModuleActionIDValid(inModuleActionID) ||
        !keySequence.first ||
        SUIT_ShortcutMgr::isInModuleMetaActionID(inModuleActionID) && moduleID != ROOT_MODULE_ID
      ) {
        std::list<std::pair<QString, QString>>& moduleInvalidShortcuts = invalidShortcuts[moduleID];
        moduleInvalidShortcuts.push_back(std::pair<QString, QString>(inModuleActionID, keySequenceString));
        continue;
      }

      const auto shortcutConflicts = theContainer.setShortcut(moduleID, inModuleActionID, keySequence.second, false /*override*/);
      if (!shortcutConflicts.empty()) {
        auto& moduleConflicts = conflicts[moduleID];
        moduleConflicts.push_back(std::pair<QString, QKeySequence>(inModuleActionID, keySequence.second));
      }
    }
  }

  if (!invalidModuleIDs.isEmpty() || !invalidShortcuts.empty() || !conflicts.empty())
  { // Prepare report and show warning.
    QString report;
    if (!invalidModuleIDs.isEmpty()) {
      report += tr("Invalid module IDs") + ":";
      for (const QString& invalidModuleID : invalidModuleIDs) {
        report += "\n\t\"" + invalidModuleID + "\"" ;
      }
    }

    if (!invalidShortcuts.empty()) {
      if (!report.isEmpty())
        report += "\n\n";

      report += tr("Invalid shortcuts") + ":";
      for (const auto& moduleAndShortcuts : invalidShortcuts) {
        report += "\n\t\"" + moduleAndShortcuts.first + "\"";
        const std::list<std::pair<QString, QString>>& moduleShortcuts = moduleAndShortcuts.second;
        for (const auto& shortcut : moduleShortcuts) {
          report += "\n\t\t\"" + shortcut.first + "\"\t\"" + shortcut.second + "\"";
        }
      }
    }

    if (!conflicts.empty()) {
      if (!report.isEmpty())
        report += "\n\n";

      report += tr("These shortcuts have not been set to theContainer, because they conflict with previously parsed ones") + ":";
      for (const auto& moduleAndShortcuts : conflicts) {
        report += "\n\t\"" + moduleAndShortcuts.first + "\"";

        const std::list<std::pair<QString, QKeySequence>>& moduleShortcuts = moduleAndShortcuts.second;
        for (const auto& shortcut : moduleShortcuts) {
          report += "\n\t\t\"" + shortcut.first + "\"\t\"" + shortcut.second.toString() + "\"";
        }
      }
    }

    report += "\n.";

    const auto text = tr("Invalid shortcuts in preferences");
    const auto informativeText = tr("Fix the following entries in the preference files manually");
    if (!theDefaultOnly) {
      // If user preferences are accounted, show warning in UI.
      SUIT_Application* app = SUIT_Session::session()->activeApplication();
      if (app && app->desktop()) {
        // Is not compiled without cast or with static_cast<QWidget*>.
        QMessageBox msgBox((QWidget*)app->desktop());
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText("<b>" + text + "</b>");
        msgBox.setInformativeText(informativeText + ":");
        msgBox.setWindowFlags(Qt::WindowType::Popup);
        msgBox.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        msgBox.setDetailedText(report);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setMinimumWidth(600);
        msgBox.exec();
      }
    }
    Warning(text + ". " + informativeText + ":\n" + report);
  }

  if (theDefaultOnly)
    resMgr->setWorkingMode(resMgrWorkingModeBefore);

  ShCutDbg() && ShCutDbg("theContainer holds following shortcuts:\n" + theContainer.toString());
}

QString substituteBashVars(const QString& theString)
{
  QString res = theString;
  const auto env = QProcessEnvironment::systemEnvironment();
  int pos = 0;
  QRegExp rx("\\$\\{([^\\}]+)\\}"); // Match substrings enclosed by "${" and "}".
  rx.setMinimal(true); // Set search to non-greedy.
  while((pos = rx.indexIn(res, pos)) != -1) {
    QString capture = rx.cap(1);
    QString subst = env.value(capture);
    ShCutDbg("capture = " + capture);
    ShCutDbg("subst   = " + subst);
    res.replace("${" + capture + "}", subst);
    pos += rx.matchedLength();
  }
  return res;
}

QString substitutePowerShellVars(const QString& theString)
{
  QString res = theString;
  int pos = 0;
  QRegExp rx("%([^%]+)%"); // Match substrings enclosed by "%".
  rx.setMinimal(true); // Set search to non-greedy.
  while((pos = rx.indexIn(res, pos)) != -1) {
    QString capture = rx.cap(1);
    QString subst = Qtx::getenv(capture.toUtf8().constData());
    ShCutDbg("capture = " + capture);
    ShCutDbg("subst   = " + subst);
    res.replace("%" + capture + "%", subst);
    pos += rx.matchedLength();
  }
  return res;
}

QString substituteVars(const QString& theString)
{
  QString str = substituteBashVars(theString);
  return substitutePowerShellVars(str);
}

/*static*/ std::pair<bool, SUIT_ActionAssets> SUIT_ShortcutMgr::getActionAssetsFromResources(const QString& theActionID)
{
  auto res = std::pair<bool, SUIT_ActionAssets>(false, SUIT_ActionAssets());

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if (!resMgr) {
    Warning("SUIT_ShortcutMgr can't retrieve resource manager!");
    return res;
  }

  QStringList actionAssetFilePaths = resMgr->parameters(SECTION_NAME_ACTION_ASSET_FILE_PATHS);
  for (const QString& actionAssetFilePath : actionAssetFilePaths) {
    const QString path = substituteVars(actionAssetFilePath);
    QFile actionAssetFile(path);
    if (!actionAssetFile.open(QIODevice::ReadOnly)) {
      Warning("SUIT_ShortcutMgr can't open action asset file \"" + path + "\"!");
      continue;
    }

    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(actionAssetFile.readAll(), &jsonError);
    actionAssetFile.close();
    if(jsonError.error != QJsonParseError::NoError) {
      Warning("SUIT_ShortcutMgr: error during parsing of action asset file \"" + path + "\"!");
      continue;
    }

    if(!document.isObject()) {
      Warning("SUIT_ShortcutMgr: empty action asset file \"" + path + "\"!");
      continue;
    }

    QJsonObject object = document.object();
    if (object.keys().indexOf(theActionID) == -1)
      continue;

    SUIT_ActionAssets actionAssets;
    if (!actionAssets.fromJSON(object[theActionID].toObject())) {
      ShCutDbg("Action asset file \"" + path + "\" contains invalid action assets with ID \"" + theActionID + "\".");
      continue;
    }

    res.second.merge(actionAssets, true);
  }

  res.first = true;
  return res;
}


/*static*/ QString SUIT_ShortcutMgr::getLang()
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if (!resMgr) {
    Warning("SUIT_ShortcutMgr can't retrieve resource manager!");
    return DEFAULT_LANG;
  }

  return resMgr->stringValue(LANG_SECTION, LANG_SECTION, DEFAULT_LANG);
}


void SUIT_ShortcutMgr::registerAction(const QString& theActionID, QAction* theAction)
{
  const auto moduleIDAndActionID = splitIntoModuleIDAndInModuleID(theActionID);
  const QString& moduleID = moduleIDAndActionID.first;
  const QString& inModuleActionID = moduleIDAndActionID.second;

  if (inModuleActionID.isEmpty()) {
    ShCutDbg() && ShCutDbg("Attempt to register an action \"" + theAction->toolTip() + "\" with invalid ID \"" + theActionID + "\".");
    if (theAction->shortcut() != NO_KEYSEQUENCE)
      theAction->setShortcut(NO_KEYSEQUENCE);

    return;
  }

  { // If an action with the same memory address was registered earlier,
    // clear all data about it to start registering procedure from scratch.
    auto itPreviousModuleAndActionID = myActionIDs.find(theAction);
    if (itPreviousModuleAndActionID != myActionIDs.end()) {
      // Clear the data from myActions.
      const auto& previousModuleAndActionID = itPreviousModuleAndActionID->second;
      auto itActions = myActions.find(previousModuleAndActionID.first);
      if (itActions != myActions.end()) {
        std::map<QString, std::set<QAction*>>& moduleActions = itActions->second;
        auto itModuleActions = moduleActions.find(previousModuleAndActionID.second);
        if (itModuleActions != moduleActions.end()) {
          std::set<QAction*>& registeredActions = itModuleActions->second;
          registeredActions.erase(theAction);
        }
      }

      myActionIDs.erase(itPreviousModuleAndActionID);
    }
  }

  auto itActions = myActions.find(moduleID);
  if (itActions == myActions.end()) {
    itActions = myActions.emplace(moduleID, std::map<QString, std::set<QAction*>>()).first;
  }

  std::map<QString, std::set<QAction*>>& moduleActions = itActions->second;
  auto itModuleActions = moduleActions.find(inModuleActionID);
  if (itModuleActions != moduleActions.end()) {
    std::set<QAction*>& registeredActions = itModuleActions->second;
    const bool actionIsNew = registeredActions.emplace(theAction).second;
    if (actionIsNew)
      myActionIDs[theAction] = moduleIDAndActionID;
  }
  else {
    std::set<QAction*>& registeredActions = moduleActions[inModuleActionID];
    registeredActions.emplace(theAction);
    myActionIDs[theAction] = moduleIDAndActionID;
  }

  connect(theAction, SIGNAL(destroyed(QObject*)), this, SLOT (onActionDestroyed(QObject*)));

  if (myShortcutContainer.hasShortcut(moduleID, inModuleActionID)) {
    const QKeySequence& keySequence = getKeySequence(moduleID, inModuleActionID);
    theAction->setShortcut(keySequence);
  }
  else {
    ShCutDbg(
      "Action with ID \"" +
      (SUIT_ShortcutMgr::isInModuleMetaActionID(inModuleActionID) ? ROOT_MODULE_ID + TOKEN_SEPARATOR + inModuleActionID : theActionID) +
      "\" is not added to default resource files."
    );
    auto conflicts = myShortcutContainer.setShortcut(moduleID, inModuleActionID, theAction->shortcut(), false);
    if (!conflicts.empty())
      theAction->setShortcut(NO_KEYSEQUENCE); // Unbind any key sequence, if it was bound outside of the class and interferes with other shortcuts.
  }
}

void SUIT_ShortcutMgr::registerAction(QtxAction* theAction)
{
  registerAction(theAction->ID(), theAction);
}

std::set<QAction*> SUIT_ShortcutMgr::getActions(const QString& theModuleID, const QString& theInModuleActionID) const
{
  if (SUIT_ShortcutMgr::isInModuleMetaActionID(theInModuleActionID)) {
    std::set<QAction*> actions;
    for (const auto& actionAndID : myActionIDs) {
      if (actionAndID.second.second == theInModuleActionID)
        actions.emplace(actionAndID.first);
    }
    return actions;
  }
  else {
    const auto itActions = myActions.find(theModuleID);
    if (itActions == myActions.end())
      return std::set<QAction*>();

    const std::map<QString, std::set<QAction*>>& moduleActions = itActions->second;
    const auto itModuleActions = moduleActions.find(theInModuleActionID);
    if (itModuleActions == moduleActions.end())
      return std::set<QAction*>();

    return itModuleActions->second;
  }
}

std::pair<QString, QString> SUIT_ShortcutMgr::getModuleIDAndInModuleID(const QAction* theAction) const {
  const auto it = myActionIDs.find(const_cast<QAction*>(theAction));
  if (it == myActionIDs.end())
    return std::pair<QString, QString>();

  return it->second;
}

bool SUIT_ShortcutMgr::hasAction(const QAction* theAction) const
{
  return myActionIDs.find(const_cast<QAction*>(theAction)) != myActionIDs.end();
}

QString SUIT_ShortcutMgr::getActionID(const QAction* theAction) const
{
  const auto it = myActionIDs.find(const_cast<QAction*>(theAction));
  if (it == myActionIDs.end())
    return QString();

  return SUIT_ShortcutMgr::makeActionID(it->second.first, it->second.second);
}

void SUIT_ShortcutMgr::setActionsOfModuleEnabled(const QString& theModuleID, const bool theEnable) const
{
  const auto itModuleActions = myActions.find(theModuleID);
  if (itModuleActions == myActions.end())
    return;

  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if (!app)
    return;

  const std::map<QString, std::set<QAction*>>& moduleActions = itModuleActions->second;
  for (const auto& idAndActions : moduleActions) {
    const std::set<QAction*>& actions = idAndActions.second;
    for (QAction* const action : actions) {
      if (action->parentWidget() == (QWidget*)app->desktop()) // Is not compiled without cast or with static_cast<QWidget*>.
        action->setEnabled(theEnable);
    }
  }
}

void SUIT_ShortcutMgr::setActionsWithPrefixInIDEnabled(const QString& theInModuleActionIDPrefix, bool theEnable) const
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if (!app)
    return;

  for (const std::pair<QAction*, std::pair<QString, QString>>& actionAndID : myActionIDs) {
    QAction* const action = actionAndID.first;
    // Is not compiled without cast or with static_cast<QWidget*>.
    if (action->parentWidget() == (QWidget*)app->desktop()) {
      const QString& inModuleActionID = actionAndID.second.second;
      if (inModuleActionID.startsWith(theInModuleActionIDPrefix))
        action->setEnabled(theEnable);
    }
  }
}

void SUIT_ShortcutMgr::setSectionEnabled(const QString& theInModuleActionIDPrefix, bool theEnable) const
{
  setActionsWithPrefixInIDEnabled(theInModuleActionIDPrefix, theEnable);
}

void SUIT_ShortcutMgr::rebindActionsToKeySequences() const
{
  ShCutDbg() && ShCutDbg("SUIT_ShortcutMgr::rebindActionsToKeySequences()");
  for (const std::pair<QAction*, std::pair<QString, QString>>& actionAndID : myActionIDs) {
    actionAndID.first->setShortcut(getKeySequence(actionAndID.second.first, actionAndID.second.second));
  }
}

void SUIT_ShortcutMgr::updateShortcuts() const
{
  rebindActionsToKeySequences();
}

std::set<std::pair<QString, QString>> SUIT_ShortcutMgr::setShortcut(const QString& theActionID, const QKeySequence& theKeySequence, bool theOverride)
{
  const auto moduleIDAndActionID = splitIntoModuleIDAndInModuleID(theActionID);
  const QString& moduleID = moduleIDAndActionID.first;
  const QString& inModuleActionID = moduleIDAndActionID.second;

  if (inModuleActionID.isEmpty()) {
    ShCutDbg() && ShCutDbg("Attempt to set shortcut with invalid action ID \"" + theActionID + "\".");
    return std::set<std::pair<QString, QString>>();
  }

  return setShortcutNoIDChecks(moduleID, inModuleActionID, theKeySequence, theOverride);
}

std::set<std::pair<QString, QString>> SUIT_ShortcutMgr::setShortcut(const QString& theModuleID, const QString& theInModuleActionID, const QKeySequence& theKeySequence, bool theOverride)
{
  if (!SUIT_ShortcutMgr::isModuleIDValid(theModuleID)) {
    ShCutDbg() && ShCutDbg("Attempt to set shortcut with invalid module ID \"" + theModuleID + "\".");
    return std::set<std::pair<QString, QString>>();
  }

  if (!SUIT_ShortcutMgr::isInModuleActionIDValid(theInModuleActionID)) {
    ShCutDbg() && ShCutDbg("Attempt to set shortcut with invalid in-module action ID \"" + theInModuleActionID + "\".");
    return std::set<std::pair<QString, QString>>();
  }

  return setShortcutNoIDChecks(theModuleID, theInModuleActionID, theKeySequence, theOverride);
}

const SUIT_ShortcutContainer& SUIT_ShortcutMgr::getShortcutContainer() const
{
  return myShortcutContainer;
}

void SUIT_ShortcutMgr::mergeShortcutContainer(const SUIT_ShortcutContainer& theContainer, bool theOverride, bool theTreatAbsentIncomingAsDisabled)
{
  ShCutDbg() && ShCutDbg("ShortcutMgr merges shortcut container...");
  const auto changes = myShortcutContainer.merge(theContainer, theOverride, theTreatAbsentIncomingAsDisabled);
  ShCutDbg() && ShCutDbg("ShortcutMgr keeps following shortcuts:\n" + myShortcutContainer.toString());

  // Turn off hotkeys for disabled shortcuts.
  for (const auto& moduleIDAndChanges : changes) {
    const QString& moduleID = moduleIDAndChanges.first;
    const auto& moduleChanges = moduleIDAndChanges.second;
    for (const std::pair<QString, QKeySequence>& modifiedShortcut : moduleChanges) {
      if (modifiedShortcut.second == NO_KEYSEQUENCE) {
        const std::set<QAction*> actions = getActions(moduleID, modifiedShortcut.first);
        for (QAction* const action : actions) {
          action->setShortcut(NO_KEYSEQUENCE);
        }
      }
    }
  }

  // Turn on hotkeys for enabled shortcuts.
  for (const auto& moduleIDAndChanges : changes) {
    const QString& moduleID = moduleIDAndChanges.first;
    const auto& moduleChanges = moduleIDAndChanges.second;
    for (const std::pair<QString, QKeySequence>& modifiedShortcut : moduleChanges) {
      if (modifiedShortcut.second != NO_KEYSEQUENCE) {
        const std::set<QAction*> actions = getActions(moduleID, modifiedShortcut.first);
        for (QAction* const action : actions) {
          action->setShortcut(modifiedShortcut.second);
        }
      }
    }
  }

  SUIT_ShortcutMgr::saveShortcutsToPreferences(changes);
}

QKeySequence SUIT_ShortcutMgr::getKeySequence(const QString& theModuleID, const QString& theInModuleActionID) const
{
  return myShortcutContainer.getKeySequence(theModuleID, theInModuleActionID);
}

const std::map<QString, QKeySequence>& SUIT_ShortcutMgr::getModuleShortcutsInversed(const QString& theModuleID) const
{
  return myShortcutContainer.getModuleShortcutsInversed(theModuleID);
}

std::set<QString> SUIT_ShortcutMgr::getShortcutModuleIDs() const
{
  return myShortcutContainer.getIDsOfAllModules();
}

std::set<QString> SUIT_ShortcutMgr::getIDsOfInterferingModules(const QString& theModuleID) const
{
  return myShortcutContainer.getIDsOfInterferingModules(theModuleID);
}

std::shared_ptr<const SUIT_ActionAssets> SUIT_ShortcutMgr::getModuleAssets(const QString& theModuleID) const
{
  const auto itModuleAssets = myModuleAssets.find(theModuleID);
  if (itModuleAssets == myModuleAssets.end()) {
    auto assets = std::shared_ptr<SUIT_ActionAssets>(new SUIT_ActionAssets());
    auto lda = SUIT_ActionAssets::LangDependentAssets();
    lda.myName = theModuleID; // At least something meaningful.

    assets->myLangDependentAssets.emplace(SUIT_ShortcutMgr::getLang(), lda);
    return assets;
  }
  return itModuleAssets->second;
}

QString SUIT_ShortcutMgr::getModuleName(const QString& theModuleID, const QString& theLang) const
{
  const auto assets = getModuleAssets(theModuleID);
  const auto& ldaMap = assets->myLangDependentAssets;
  if (ldaMap.empty())
    return theModuleID;

  auto itLang = ldaMap.find(theLang.isEmpty() ? SUIT_ShortcutMgr::getLang() : theLang);
  if (itLang == ldaMap.end())
    itLang = ldaMap.begin(); // Get name in any language.

  const auto& name = itLang->second.myName;
  return name.isEmpty() ? theModuleID : name;
}

std::shared_ptr<const SUIT_ActionAssets> SUIT_ShortcutMgr::getActionAssets(const QString& theModuleID, const QString& theInModuleActionID) const
{
  const QString actionID = SUIT_ShortcutMgr::makeActionID(theModuleID, theInModuleActionID);
  if (actionID.isEmpty()) {
    ShCutDbg() && ShCutDbg("Can't get action assets: either/both module ID \"" + theModuleID + "\" or/and in-module action ID \"" + theInModuleActionID + "\" is/are invalid.");
    return std::shared_ptr<const SUIT_ActionAssets>(nullptr);
  }
  return getActionAssets(actionID);
}

std::shared_ptr<const SUIT_ActionAssets> SUIT_ShortcutMgr::getActionAssets(const QString& theActionID) const
{
  const auto it = myActionAssets.find(theActionID);
  if (it == myActionAssets.end())
    return std::shared_ptr<const SUIT_ActionAssets>(nullptr);
  else
    return it->second;
}

QString SUIT_ShortcutMgr::getActionName(const QString& theModuleID, const QString& theInModuleActionID, const QString& theLang) const
{
  const QString actionID = SUIT_ShortcutMgr::makeActionID(theModuleID, theInModuleActionID);
  if (actionID.isEmpty()) {
    ShCutDbg() && ShCutDbg("Can't get action name: either/both module ID \"" + theModuleID + "\" or/and in-module action ID \"" + theInModuleActionID + "\" is/are invalid.");
    return actionID;
  }

  const auto itActionAssets = myActionAssets.find(actionID);
  if (itActionAssets != myActionAssets.end() && !itActionAssets->second->myLangDependentAssets.empty()) {
    const auto& ldaMap = itActionAssets->second->myLangDependentAssets;
    if (ldaMap.empty())
      return theInModuleActionID;

    auto itLang = ldaMap.find(theLang.isEmpty() ? SUIT_ShortcutMgr::getLang() : theLang);
    if (itLang == ldaMap.end())
      itLang = ldaMap.begin(); // Get name in any language.

    const auto& name = itLang->second.myName;
    return name.isEmpty() ? theInModuleActionID : name;
  }
  else /* if action assets have not been loaded. */ {
    // Try to get action->text() and use it as a name.

    // Pitfall of the approach: at the time this code block is called, the action may not exist.
    // Moreover, an action with such an ID may not even have been created at the time of calling this method.
    // Thus, even buffering of assets of every action ever created at runtime does not guarantee,
    // that the assets will be available at any point in the life of the application,
    // unless the assets are added to dedicated section in an asset file.

    const auto actions = getActions(theModuleID, theInModuleActionID);
    for (const auto& action : actions) {
      if (!action->text().isEmpty())
        return action->text();
    }
    return theInModuleActionID;
  }
}

void SUIT_ShortcutMgr::onActionDestroyed(QObject* theObject)
{
  QAction* action = static_cast<QAction*>(theObject);

  auto itID = myActionIDs.find(action);
  if (itID == myActionIDs.end())
    return;

  const QString& moduleID = itID->second.first;
  const QString& inModuleActionID = itID->second.second;

  auto itModuleActions = myActions.find(moduleID);
  if (itModuleActions != myActions.end()) {
    std::map<QString, std::set<QAction*>>& moduleActions = itModuleActions->second;
    auto itActions = moduleActions.find(inModuleActionID);
    if (itActions != moduleActions.end()) {
      std::set<QAction*>& actions = itActions->second;
      actions.erase(action);
    }
  }

  myActionIDs.erase(itID);
}

bool SUIT_ShortcutMgr::eventFilter(QObject* theObject, QEvent* theEvent)
{
  if (theEvent) {
    if (theEvent->type() == QEvent::ActionAdded) {
      auto anActionEvent = static_cast<QActionEvent*>(theEvent);

      QtxAction* aQtxAction = qobject_cast<QtxAction*>(anActionEvent->action());
      if (aQtxAction) {
#ifdef SHORTCUT_MGR_DBG
        {
          const auto moduleIDAndActionID = splitIntoModuleIDAndInModuleID(aQtxAction->ID());
          if (moduleIDAndActionID.second.isEmpty())
            ShCutDbg("ActionAdded event, but ID of the action is invalid. Action name = \"" + aQtxAction->toolTip() + "\", ID = \"" + aQtxAction->ID() + "\".");
          else if (!myShortcutContainer.hasShortcut(moduleIDAndActionID.first, moduleIDAndActionID.second))
            ShCutDbg("ActionAdded event, but shortcut container has no shortcut for the action. It is ok, if preference files has not been parsed yet. Action ID = \"" + moduleIDAndActionID.second + "\".");
        }
#endif//SHORTCUT_MGR_DBG
#ifdef SHORTCUT_MGR_DEVTOOLS
        {
          DevTools::get()->collectShortcutAndAssets(aQtxAction);
          const auto moduleIDAndActionID = splitIntoModuleIDAndInModuleID(aQtxAction->ID());
          if (moduleIDAndActionID.second.isEmpty())
            DevTools::get()->collectAssetsOfActionWithInvalidID(aQtxAction);
        }
#endif//SHORTCUT_MGR_DEVTOOLS
	      registerAction(aQtxAction);
      }
      else {
        QAction* aQAction = qobject_cast<QAction*>(anActionEvent->action());
#ifdef SHORTCUT_MGR_DEVTOOLS
        if (aQAction)
          DevTools::get()->collectAssetsOfActionWithInvalidID(aQAction);
#endif//SHORTCUT_MGR_DEVTOOLS
        if (aQAction && aQAction->shortcut() != NO_KEYSEQUENCE) {
#ifdef SHORTCUT_MGR_DBG
          ShCutDbg("ActionAdded event, but the added action is not QtxAction and bound to non-empty key sequence. name: \"" + aQAction->toolTip() + "\".");
#endif//SHORTCUT_MGR_DBG
          // Since non-QtxAction has no ID, it is impossible to properly manage its shortcut.
          // And the shortcut may interfere with managed ones.
          aQAction->setShortcut(NO_KEYSEQUENCE);
        }
      }
    }
  }

  return QObject::eventFilter(theObject, theEvent);
}

std::set<std::pair<QString, QString>> SUIT_ShortcutMgr::setShortcutNoIDChecks(const QString& theModuleID, const QString& theInModuleActionID, const QKeySequence& theKeySequence, bool theOverride)
{
  std::set<std::pair<QString, QString>> disabledShortcutsIDs = myShortcutContainer.setShortcut(theModuleID, theInModuleActionID, theKeySequence, theOverride);

  if (theOverride || disabledShortcutsIDs.empty()) {
    // Bind actions to corresponding modified key sequences. Save changes to preferences.

    /** { moduleID, {inModuleActionID, keySequence}[] }[] */
    std::map<QString, std::map<QString, QKeySequence>> modifiedShortcuts;

    for (const auto& moduleIDAndActionID : disabledShortcutsIDs) {
      // Unbind actions of disabled shortcuts.

      const QString& moduleID = moduleIDAndActionID.first;
      const QString& inModuleActionID = moduleIDAndActionID.second;

      std::map<QString, QKeySequence>& modifiedModuleShortcuts = modifiedShortcuts[moduleID];
      modifiedModuleShortcuts[inModuleActionID] = NO_KEYSEQUENCE;

      const std::set<QAction*> actions = getActions(moduleID, inModuleActionID);
      for (QAction* const action : actions) {
        action->setShortcut(NO_KEYSEQUENCE);
      }
    }

    { // Bind actions to theKeySequence.
      std::map<QString, QKeySequence>& modifiedModuleShortcuts = modifiedShortcuts[theModuleID];
      modifiedModuleShortcuts[theInModuleActionID] = theKeySequence;

      const std::set<QAction*> actions = getActions(theModuleID, theInModuleActionID);
      for (QAction* const action : actions) {
        action->setShortcut(theKeySequence);
      }
    }

    SUIT_ShortcutMgr::saveShortcutsToPreferences(modifiedShortcuts);
  }

  return disabledShortcutsIDs;
}

void SUIT_ShortcutMgr::setShortcutsFromPreferences()
{
  ShCutDbg() && ShCutDbg("ShortcutMgr is initializing...");

  SUIT_ShortcutContainer container;
  SUIT_ShortcutMgr::fillContainerFromPreferences(container, false /*theDefaultOnly*/);
  mergeShortcutContainer(container, true /*theOverrde*/, false /*theTreatAbsentIncomingAsDisabled*/);
  setAssetsFromResources();

  ShCutDbg() && ShCutDbg("ShortcutMgr has been initialized.");
}

/*static*/ void SUIT_ShortcutMgr::saveShortcutsToPreferences(const std::map<QString, std::map<QString, QKeySequence>>& theShortcutsInversed)
{
  ShCutDbg() && ShCutDbg("Saving preferences to resources.");

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if (!resMgr) {
    Warning("SUIT_ShortcutMgr can't retrieve resource manager!");
    return;
  }

  for (const auto& moduleIDAndShortcutsInversed : theShortcutsInversed) {
    const auto& moduleID = moduleIDAndShortcutsInversed.first;
    const auto& moduleShortcutsInversed = moduleIDAndShortcutsInversed.second;
    for (const auto& shortcutInversed : moduleShortcutsInversed) {
      if (shortcutInversed.first.isEmpty()) {
        ShCutDbg("Attempt to serialize a shortcut with empty action ID.");
        continue;
      }

      const QString sectionName = SECTION_NAME_PREFIX + resMgr->sectionsToken() + moduleID;
      resMgr->setValue(sectionName, shortcutInversed.first, shortcutInversed.second.toString());

      ShCutDbg() && ShCutDbg("Saving shortcut: \"" + moduleID + "\"\t\"" + shortcutInversed.first + "\"\t\"" + shortcutInversed.second.toString() + "\"");
    }
  }
}

void SUIT_ShortcutMgr::setAssetsFromResources(QString theLanguage)
{
  ShCutDbg() && ShCutDbg("Retrieving action assets.");

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if (!resMgr) {
    Warning("SUIT_ShortcutMgr can't retrieve resource manager!");
    return;
  }

  if (theLanguage.isEmpty())
    theLanguage = resMgr->stringValue(LANG_SECTION, LANG_SECTION, DEFAULT_LANG);

  QStringList langPriorityList = LANG_PRIORITY_LIST;
  langPriorityList.push_front(theLanguage);
  langPriorityList.removeDuplicates();

  QStringList actionAssetFilePaths = resMgr->parameters(SECTION_NAME_ACTION_ASSET_FILE_PATHS);
#ifdef SHORTCUT_MGR_DBG
  ShCutDbg("Asset files: " + actionAssetFilePaths.join(", ") + ".");
#endif
  for (const QString& actionAssetFilePath : actionAssetFilePaths) {
    const QString path = substituteVars(actionAssetFilePath);
    QFile actionAssetFile(path);
    if (!actionAssetFile.open(QIODevice::ReadOnly)) {
      Warning("SUIT_ShortcutMgr can't open action asset file \"" + path + "\"!");
      continue;
    }

    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(actionAssetFile.readAll(), &jsonError);
    actionAssetFile.close();
    if(jsonError.error != QJsonParseError::NoError) {
        Warning("SUIT_ShortcutMgr: error during parsing of action asset file \"" + path + "\"!");
        continue;
    }

    if(document.isObject()) {
      QJsonObject object = document.object();
      SUIT_ActionAssets actionAssets;
      for (const QString& actionID : object.keys()) {
        if (!SUIT_ShortcutMgr::isActionIDValid(actionID)) {
          ShCutDbg("Action asset file \"" + path + "\" contains invalid action ID \"" + actionID + "\".");
          continue;
        }

        if (!actionAssets.fromJSON(object[actionID].toObject())) {
          ShCutDbg("Action asset file \"" + path + "\" contains invalid action assets with ID \"" + actionID + "\".");
          continue;
        }

        const bool nameInCurLangExists = actionAssets.myLangDependentAssets.find(theLanguage) != actionAssets.myLangDependentAssets.end();
        if (nameInCurLangExists) {
          actionAssets.clearAllLangsExcept(theLanguage);
        }
        else {
          bool nameInLinguaFrancaExists = false;
          QString usedLanguage = QString();
          for (int i = 1; i < langPriorityList.length(); i++) {
            nameInLinguaFrancaExists = actionAssets.myLangDependentAssets.find(langPriorityList[i]) != actionAssets.myLangDependentAssets.end();
            if (nameInLinguaFrancaExists) {
              usedLanguage = langPriorityList[i];
              actionAssets.clearAllLangsExcept(usedLanguage);
              break;
            }
          }

          #ifdef SHORTCUT_MGR_DBG
          if (nameInLinguaFrancaExists)
            ShCutDbg("Can't find assets for action with ID \"" + actionID + "\" at current (" + theLanguage + ") language. Assets in " + usedLanguage + " is used for the action." );
          else {
            ShCutDbg("Can't find assets for action with ID \"" + actionID + "\". Tried " + langPriorityList.join(", ") + " languages." );
            continue;
          }
          #endif
        }

        auto itAssets = myActionAssets.find(actionID);
        if (itAssets == myActionAssets.end()) {
          auto pAssets = std::shared_ptr<SUIT_ActionAssets>(new SUIT_ActionAssets(actionAssets));
          itAssets = myActionAssets.emplace(actionID, pAssets).first;
        }
        else
          itAssets->second->merge(actionAssets, true);

        const auto& assets = itAssets->second;
        if (!assets->myIconPath.isEmpty() && assets->myIcon.isNull())
          assets->myIcon = QIcon(substituteVars(assets->myIconPath));
      }
    }
  }

  #ifdef SHORTCUT_MGR_DBG
  ShCutDbg("Parsed assets: ");
  QJsonObject object;
  for (const auto& actionIDAndAssets : myActionAssets) {
    actionIDAndAssets.second->toJSON(object);
    QJsonDocument doc(object);
    QString strJson = doc.toJson(QJsonDocument::Indented);
    ShCutDbg(actionIDAndAssets.first + " : " +  strJson);
  }
  #endif

  // Fill myModuleAssets.
  for (const auto& moduleID : myShortcutContainer.getIDsOfAllModules()) {
    const auto assets = std::shared_ptr<SUIT_ActionAssets>(new SUIT_ActionAssets());
    auto& lda = assets->myLangDependentAssets[DEFAULT_LANG];

    if (moduleID == ROOT_MODULE_ID) {
      lda.myName = tr("General");

      { // Load icon.
        QString dirPath;
        if (resMgr->value("resources", "LightApp", dirPath)) {
          assets->myIconPath = dirPath + (!dirPath.isEmpty() && dirPath[dirPath.length() - 1] == "/" ? "" : "/") + "icon_default.png";
          assets->myIcon = QIcon(substituteVars(assets->myIconPath));
        }
      }
    }
    else {
      QString moduleName = moduleID;
      resMgr->value(moduleID, "name", moduleName);
      lda.myName = moduleName;

      resMgr->value(moduleID, "description", lda.myToolTip);

      { // Load icon.
        QString dirPath;
        QString fileName;
        if (resMgr->value("resources", moduleID, dirPath) && resMgr->value(moduleID, "icon", fileName)) {
          assets->myIconPath = dirPath + (!dirPath.isEmpty() && dirPath[dirPath.length() - 1] == "/" ? "" : "/") + fileName;
          assets->myIcon = QIcon(substituteVars(assets->myIconPath));
        }
      }
    }

    myModuleAssets.emplace(moduleID, std::move(assets));
  }
}