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

#ifndef SUIT_SHORTCUTMGR_H
#define SUIT_SHORTCUTMGR_H

#include "SUIT.h"

#include <QObject>
#include <QString>
#include <QKeySequence>
#include <QStringList>
#include <QIcon>
#include <map>
#include <set>
#include <memory>
#include <utility>
#include <limits>

class QAction;
class QtxAction;
class QJsonObject;

#if defined WIN32
#pragma warning( disable: 4251 )
#endif

// Define SHORTCUT_MGR_DBG to enable SUIT_ShortcutMgr debug logging.
#define SHORTCUT_MGR_DBG
/*! \returns true, if SUIT_ShortcutMgr debug logging is enabled. */
SUIT_EXPORT extern inline bool ShCutDbg() {
#ifdef SHORTCUT_MGR_DBG
  return true;
#else
  return false;
#endif
}
/*! \brief Prints theString to std::wcout, if SUIT_ShortcutMgr debug logging is enabled. */
SUIT_EXPORT extern bool ShCutDbg(const QString& theString);
/*! \brief Prints theString to std::wcout, if SUIT_ShortcutMgr debug logging is enabled. */
SUIT_EXPORT extern bool ShCutDbg(const char* theString);


/*!
  \class SUIT_ShortcutContainer
  \brief Provides means to keep and edit shortcuts in compliance with the application logics.
  \ref See SUIT_ShortcutMgr for details.
*/
class SUIT_EXPORT SUIT_ShortcutContainer
{
public:
  SUIT_ShortcutContainer();

  /*! \returns IDs of modules, which interfere with the module:
  if the module is root (theModuleID is empty) - returns all module IDs, otherwise returns ["", theModuleID]. */
  std::set<QString> getIDsOfInterferingModules(const QString& theModuleID) const;

  std::set<QString> getIDsOfAllModules() const;

  /*! \brief Checks for conflicts. If theOverride, modifies incoming and disables all conflicting shortcuts.
  Redefining a key sequence for the action, if theKeySequence does not conflict with other shortcuts, is not considered as a conflict.
  \param theModuleID The method has no effect if theModuleID is invalid. \ref See SUIT_ShortcutMgr::isModuleIDValid(const QString&) for details.
  \param theInModuleActionID The method has no effect if theInModuleActionID is invalid. \ref See SUIT_ShortcutMgr::isInModuleActionIDValid(const QString&).
  If theInModuleActionID is meta-action ID, the shortcut is set to root module, and theModuleID is ignored.
  \param theKeySequence Empty theKeySequence does not cause conflicts, in this case
  a shortcut for the action is disabled: theInModuleActionID is added/retained in the container but mapped to empty key sequence.
  \param theOverride If true, conflicting shortcuts are disabled.
  \returns {moduleID, inModuleActionID}[] - Set of conflicting actions if theOverride = false,
  otherwise set of actions (without incoming one), whose shortcuts have been disabled. */
  std::set<std::pair<QString, QString>> setShortcut(
    QString theModuleID,
    const QString& theInModuleActionID,
    const QKeySequence& theKeySequence,
    bool theOverride
  );

  /*! \brief Checks for conflicts. Existence of a shortcut with another key sequence for the action,
  if theKeySequence does not conflict with other shortcuts, is not considered as a conflict.
  \param theInModuleActionID If theInModuleActionID is meta-action ID, the shortcut is looked for in root module, and theModuleID is ignored.
  \param theKeySequence Empty theKeySequence does not have conflicts.
  \returns {moduleID, inModuleActionID}[] - Set of conflicting actions. */
  std::set<std::pair<QString, QString>> getConflicts(
    QString theModuleID,
    const QString& theInModuleActionID,
    const QKeySequence& theKeySequence
  ) const;

  /*! \returns empty key sequence if shortcut for the action is not set.
  \param theInModuleActionID If theInModuleActionID is meta-action ID, seeks in root module, and theModuleID is ignored.*/
  const QKeySequence& getKeySequence(QString theModuleID, const QString& theInModuleActionID) const;

  /*! \returns true, if shortcut for the action is set (even if the mapped key sequence is empty).
   \param theInModuleActionID If theInModuleActionID is meta-action ID, seeks in root module, and theModuleID is ignored.*/
  bool hasShortcut(QString theModuleID, const QString& theInModuleActionID) const;

  /*! \returns {inModuleActionID, keySequence}[] - If the module was not added, the map is empty. */
  const std::map<QString, QKeySequence>& getModuleShortcutsInversed(const QString& theModuleID) const;

  /*! \brief Seeks for shortcuts in the module with in-module action IDs, which start with theInModuleActionIDPrefix.
  \returns {inModuleActionID, keySequence}[] - If the module was not added, the map is empty. */
  const std::map<QString, QKeySequence> getModuleShortcutsInversed(const QString& theModuleID, const QString& theInModuleActionIDPrefix) const;

  /*! \brief Merges shortcuts of theOther into this.
  \param theOverride if true, overrides conflicting shortcuts.
  If false, and this has no shortcut for an incoming action, and the incoming shortcut conflicts
  with an existing shortcut, disabled shortcut for the incoming action is set.
  \param theTreatAbsentIncomingAsDisabled If theOverride == false, theTreatAbsentIncomingAsDisabled is ignored.
  If theOverride and theTreatAbsentIncomingAsDisabled, and theOther has no shortcut for an action, which exists in this,
  the existing shortcut in this is set disabled.
    \returns { moduleID, { inModuleActionID, {oldKeySequence, newKeySequence} }[] }[] - Modiified shortcuts inversed. */
  std::map<QString, std::map<QString, std::pair<QKeySequence, QKeySequence>>> merge(
    const SUIT_ShortcutContainer& theOther,
    bool theOverride,
    bool theTreatAbsentIncomingAsDisabled = false
  );

  bool hasKeySequence(const QString& theModuleID, const QKeySequence& theKeySequence) const;

  /*! \brief Generates human-readable text representation of content. */
  QString toString() const;

private:
  /** { moduleID, { keySequence, inModuleActionID }[] }[]. keySequence can not be empty.
   * Can not contain entries like { <non-root module ID>, { keySequence, <meta-action ID> } }. */
  std::map<QString, std::map<QKeySequence, QString>> myShortcuts;

  /** { moduleID, { inModuleActionID, keySequence }[] }[]. keySequence can be empty.
   * Can not contain entries like { <non-root module ID>, { <meta-action ID>, keySequence } }. */
  std::map<QString, std::map<QString, QKeySequence>> myShortcutsInversed;
};


/*! \brief GUI-related assets. */
struct SUIT_EXPORT SUIT_ActionAssets
{
  struct LangDependentAssets
  {
    static const QString PROP_ID_NAME;
    static const QString PROP_ID_TOOLTIP;

    bool fromJSON(const QJsonObject& theJsonObject);
    void toJSON(QJsonObject& oJsonObject) const;

    QString myName;
    QString myToolTip;
  };

  static const QString STRUCT_ID;
  static const QString PROP_ID_LANG_DEPENDENT_ASSETS;
  static const QString PROP_ID_ICON_PATH;

  bool fromJSON(const QJsonObject& theJsonObject);
  void toJSON(QJsonObject& oJsonObject) const;
  QString toString() const;

  QStringList getLangs() const;
  void clearAllLangsExcept(const QString& theLang);

  /*! \param theOverride If true, values of theOther override conflicting values of this. */
  void merge(const SUIT_ActionAssets& theOther, bool theOverride);

  std::map<QString, LangDependentAssets> myLangDependentAssets;
  QString myIconPath;

  /*! Is not serialized. */
  QIcon myIcon;
};


/*!
  \class SUIT_ShortcutMgr
  \brief Handles action shortcut customization.

  IDENTIFIED ACTIONS/SHORTCUTS

  Register actions under action IDs. Set shortcuts, which are [action ID]<->[key sequence] mappings.
  Every time an action is registered or a shorcut is set, if there are an action and a shortcut,
  which are mapped to the same action ID, the action is bound to the key sequence of the shortcut.
  Action IDs are also used to (de)serialize shortcut settings.
  Several QActions may be registered under the same ID.

  Most of actions are intercepted on creation in SUIT_ShortcutMgr:eventFilter(QObject* theObject, QEvent* theEvent).
  If an intercepted action is instance of QtxAction, it is registered automatically.
  Since non-QtxActions have no member ID(), SUIT_ShortcutMgr is unable to register them automatically
  in SUIT_ShortcutMgr::eventFilter(). Thus, every non-QtxAction should be
  passed to SUIT_ShortcutMgr::registerAction(const QString& theActionID, QAction* theAction).

  Action ID is application-unique must be composed as <moduleID>/<inModuleActionID>.
  If an action belongs to a desktop or is available even if no module is active (e.g. 'Save As'),
  use empty string as <moduleID>. Let's call such actions as root actions.

  There is a need to keep multiple actions, which do the same from user' perspective,
  bound to the same key sequence. E.g. 'Front view'. Let's call such set of actions as meta-action.
  Actions of a meta-action may belong to different modules, and/or there may be several actions
  of the same meta-action in the same module. <inModuleActionID> of all members of a meta-action
  must be the same and start with "#".
  Meta-action is root action when it comes to checking for conflicts.
  Shortcuts of meta-actions are (de)serialized to the same section of preference files as root shortcuts.

  <inModuleActionID> can contain several "/". Go to \ref isInModuleActionIDValid(const QString&) for details.
  You can refer to multiple actions, whose <inModuleActionID> starts with the prefix.

  Only one module can be active at instance. So a key sequence must be unique within a joined temporary table of
  root and active module shortcuts. An action is allowed to be bound with only key sequence.

  ANONYMOUS ACTIONS/SHORTCUTS

  Actions without action IDs or with invalid ones are called anonymous actions.
  All anonymous actions with non-empty shortcut key sequences are registered by SUIT_ShortcutMgr.
  If a shortcut for an anonymous action clashes with a shortcut for an action with defined ID (identified action/shortcut),
  the shortcut for the anonymous action is disabled, but [the anonymous action, the hard-coded key sequence] pair
  remains within the SUIT_ShortcutMgr. If user redefines key sequences for identified actions,
  and the clash is gone, SUIT_ShortcutMgr enables back the shortcut for the anonymous action.

  WARNING!
  Avoid assigning shortcuts to instances of QAction and all its descendants directly.
  (1) Key sequence being bound directly to any registered/intercepted action with valid ID,
      if the key sequence does not conflict with shortcuts kept by SUIT_ShortcutMgr,
      is added to the manager and appears in user preference file. If it does conflict,
      it is reassigned with a key sequence from preference files or
      disabled and added to user preference files (if the files have no shortcut for the action).
  (2) It is not possible to reassign key sequences for anonymous actions using the Shortcut Editor GUI.
      It is not possible to always warn user, if a key sequence, he assigns to an identified action,
      disables an anonymous shortcut, because SUIT_ShortcutMgr has no data about anonymous actions until they appear in runtime.
      To prevent the user from relying on such warnings, they are completely disabled.
*/
class SUIT_EXPORT SUIT_ShortcutMgr: public QObject
{
  Q_OBJECT

private:
  SUIT_ShortcutMgr();
  SUIT_ShortcutMgr(const SUIT_ShortcutMgr&) = delete;
  SUIT_ShortcutMgr& operator=(const SUIT_ShortcutMgr&) = delete;

protected:
  virtual ~SUIT_ShortcutMgr();

public:
  static const QString ROOT_MODULE_ID;

  /*! \brief Create new singleton-instance of shortcut manager, if it has not been created. */
  static void Init();

  static SUIT_ShortcutMgr* get();

  /*! \brief Checks whether the theKeySequence is platform-compatible. */
  static bool isKeySequenceValid(const QKeySequence& theKeySequence);

  /*! \returns {false, _ } if  theKeySequenceString is invalid. */
  static std::pair<bool, QKeySequence> toKeySequenceIfValid(const QString& theKeySequenceString);

  /*! \brief Valid module ID does not contain "/" and equals to result of QString(theModuleID).simplified().
  Empty module ID is valid - it is root module ID. */
  static bool isModuleIDValid(const QString& theModuleID);

  /*! \brief Valid in-module action ID may consist of several tokens, separated by "/":
  <token_0>/<token_1>...<token_N>/<token_N-1>.
  Each <token> must be non-empty and be equal to QString(<token>).simplified().
  Empty or "#" in-module action ID is not valid. */
  static bool isInModuleActionIDValid(const QString& theInModuleActionID);

  /*! \returns true, is theInModuleActionID starts with "#". */
  static bool isInModuleMetaActionID(const QString& theInModuleActionID);

  /*! \brief Extracts module ID and in-module action ID from application-unique action ID.
  The theActionID must be composed as <moduleID>/<inModuleActionID>.
  \returns { _ , "" }, if theActionID is invalid. */
  static std::pair<QString, QString> splitIntoModuleIDAndInModuleID(const QString& theActionID);

  /*! See \ref splitIntoModuleIDAndInModuleID(const QString&). */
  static bool isActionIDValid(const QString& theActionID);

  /*! \brief Creates application-unique action ID. Reverse to splitIntoModuleIDAndInModuleID.
  \returns Emppty string, if either theModuleID or theInModuleActionID is invalid*/
  static QString makeActionID(const QString& theModuleID, const QString& theInModuleActionID);

  /*! \brief Sets all shortcuts from preferences to theContainer. Incoming shortcuts override existing ones.
  If the container has shortcut for an action, which is absent in preferences, and the existing shortcut
  does not conflict with incoming ones, it is untouched.
  See \ref setShortcutsFromPreferences() for details.
  \param theDefaultOnly If true, user preferences are ignored and only default preferences are used. */
  static void fillContainerFromPreferences(SUIT_ShortcutContainer& theContainer, bool theDefaultOnly);

  /*! \brief Checks the resource manager directly.
  \returns {assetsExist, assets}. */
  static std::pair<bool, SUIT_ActionAssets> getActionAssetsFromResources(const QString& theActionID);

  /*! \returns Language, which is set in resource manager. */
  static QString getLang();


  /*! \brief Add theAction to map of managed actions. */
  void registerAction(const QString& theActionID, QAction* theAction);

  /*! \brief Add theAction to map of managed actions. QtxAction::ID() is used as action ID. */
  void registerAction(QtxAction* theAction);

  /*! \brief Get registered actions. If theInModuleActionID is meta-action ID, seeks in all modules. */
  std::set<QAction*> getActions(const QString& theModuleID, const QString& theInModuleActionID) const;

  /*! \brief Get module ID and in-module-ID of theAction.
  \returns { _ , "" } if theAction is not registered. */
  std::pair<QString, QString> getModuleIDAndInModuleID(const QAction* theAction) const;

  /*! Returns true if theAction is registered. */
  bool hasAction(const QAction* theAction) const;

  /*! \brief Get action ID of theActon.
  \returns empty string if theAction is not registered. */
  QString getActionID(const QAction* theAction) const;

  /*! \brief Enables/disable actions of the module.
  Only those actions are affected, whose parent widget is active desktop. */
  void setActionsOfModuleEnabled(const QString& theModuleID, const bool theEnable = true) const;

  [[deprecated("Use setActionsOfModuleEnabled(const QString&, bool) instead.")]]
  void setSectionEnabled(const QString& theInModuleActionIDPrefix, bool theEnable = true) const;

  /*! \brief Enables/disables all registered actions whose in-module action ID begins with theInModuleActionIDPrefix.
  Only those actions are affected, whose parent widget is active desktop. */
  void setActionsWithPrefixInIDEnabled(const QString& theInModuleActionIDPrefix, bool theEnable = true) const;

  /*! \brief For all registered actions binds key sequences from myShortcutContainer. */
  void rebindActionsToKeySequences() const;

  [[deprecated("Use rebindActionsToKeySequences() instead.")]]
  void updateShortcuts() const;

  /*! \brief Checks for conflicts. If theOverride, modifies incoming and disables all conflicting shortcuts.
  Then binds key sequences with corresponding registered actions. Saves changes to preferences.

  Redefining a key sequence for the action, if the key sequence does not conflict with other shortcuts, is not considered as a conflict.
  \param theInModuleActionID The method has no effect if theInModuleActionID is empty.
  \param theKeySequence Empty theKeySequence does not cause conflicts, in this case
  a shortcut for the action is disabled: theInModuleActionID is added/retained in the container but mapped to empty key sequence.
  \param theOverride If true, conflicting shortcuts are disabled.
  \returns {moduleID, inModuleActionID}[] - Set of conflicting actions if theOverride = false,
  otherwise set of actions (without incoming one), whose shortcuts have been disabled. */
  std::set<std::pair<QString, QString>> setShortcut(const QString& theActionID, const QKeySequence& theKeySequence, bool theOverride = false);
  std::set<std::pair<QString, QString>> setShortcut(const QString& theModuleID, const QString& theInModuleActionID, const QKeySequence& theKeySequence, bool theOverride = false);

  const SUIT_ShortcutContainer& getShortcutContainer() const;

  /*! \brief Does not perform validity checks on theModuleID and theInModuleActionID. */
  void mergeShortcutContainer(const SUIT_ShortcutContainer& theContainer, bool theOverride = true, bool theTreatAbsentIncomingAsDisabled = false);

  /*! \brief Get a key sequence mapped to the action. */
  const QKeySequence& getKeySequence(const QString& theModuleID, const QString& theInModuleActionID) const;

  /*! \returns {inModuleActionID, keySequence}[] */
  const std::map<QString, QKeySequence>& getModuleShortcutsInversed(const QString& theModuleID) const;

  /*! \returns All module IDs, which were added to myShortcutContainer. */
  std::set<QString> getShortcutModuleIDs() const;

  /*! \returns IDs of modules, which interfere with the module:
  if the module is root (theModuleID is empty) - returns all module IDs, otherwise returns ["", theModuleID]. */
  std::set<QString> getIDsOfInterferingModules(const QString& theModuleID) const;

  /*! \returns assets, which describe module's header, not its content. */
  std::shared_ptr<const SUIT_ActionAssets> getModuleAssets(const QString& theModuleID) const;

  /*! \returns assets, which describe modules' headers, not their content. */
  std::map<QString, std::shared_ptr<SUIT_ActionAssets>> getModuleAssets() const { return myModuleAssets; }

  /*! \brief Retrieves module name, if the asset was loaded using \ref setAssetsFromResources(). If theLang is empty, it is effectively current language. */
  QString getModuleName(const QString& theModuleID, const QString& theLang = "") const;

  std::shared_ptr<const SUIT_ActionAssets> getActionAssets(const QString& theModuleID, const QString& theInModuleActionID) const;

  std::shared_ptr<const SUIT_ActionAssets> getActionAssets(const QString& theActionID) const;

  std::map<QString, std::map<QString, std::shared_ptr<SUIT_ActionAssets>>> getActionAssets() const { return myActionAssets; }

  /*! \brief Retrieves action name, if the asset was loaded using \ref setAssetsFromResources(). If theLang is empty, it is effectively current language. */
  QString getActionName(const QString& theModuleID, const QString& theInModuleActionID, const QString& theLang = "") const;

private slots:
  /*!
  \brief Called when the corresponding action is destroyed.
  Removes destroyed action from maps of registered actions. Preserves shortcut.
  \param theObject action being destroyed.
  */
  void onActionDestroyed(QObject* theObject);

  void onAnonymousActionDestroyed(QObject* theObject);

private:
  /*! \brief Overrides QObject::eventFilter().
  If theEvent is QEvent::ActionAdded and the action is instance of QtxAction, registers it. */
  virtual bool eventFilter(QObject* theObject, QEvent* theEvent);

  /*! \brief Does not perform validity checks on theModuleID and theInModuleActionID. */
  std::set<std::pair<QString, QString>> setShortcutNoIDChecks(const QString& theModuleID, const QString& theInModuleActionID, const QKeySequence& theKeySequence, bool theOverride);

  /*! \brief Set shortcuts from preference files. The method is intended to be called before any calls to setShortcut() or mergeShortcutContainer().
  Definition of this method assumes, that shortcut settings are serialized as prerefence entries {name=<inModuleActionID>, val=<keySequence>}
  in dedicated section for each module, with names of sections being composed as "shortcuts:<moduleID>".

  E.g. in case of XML file it may look like this:
  <!--
  <section name="<module ID>">
    ...
    <parameter name="<in-module action ID>" value="key sequence">
    ...
  </section>
  -->
  <section name="shortcuts:">
    <parameter name="TOT_DESK_FILE_NEW" value="Ctrl+N"/>
    <parameter name="TOT_DESK_FILE_OPEN" value="Ctrl+O"/>
    <parameter name="#General/Show object(s)" value="Ctrl+Alt+S"/>
    <parameter name="#General/Hide object(s)" value="Ctrl+Alt+H"/>
    <parameter name="#Viewers/Back view" value="Ctrl+Alt+B"/>
    <parameter name="#Viewers/Front view" value="Ctrl+Alt+F"/>
  </section>
   <section name="shortcuts:GEOM">
    <parameter name="Isolines/Increase number" value="Meta+I"/>
    <parameter name="Isolines/Decrease number" value="Meta+D"/>
    <parameter name="Transparency/Increase" value="Meta+Y"/>
    <parameter name="Transparency/Decrease" value="Meta+T"/>
  </section>

  Empty inModuleActionIDs are ignored.

  nb! For any theQtxAction you wish user be able to assign it to a shortcut,
  add theQtxAction.ID() to default resource files (you can map it to no key sequence).*/
  void setShortcutsFromPreferences();

  /*! \brief Writes shortcuts to preference files.
  \param theShortcuts { moduleID, { inModuleActionID, keySequence }[] }[]. Empty inModuleActionIDs are ignored. */
  static void saveShortcutsToPreferences(const std::map<QString, std::map<QString, QKeySequence>>& theShortcutsInversed);

  /*! \brief Writes shortcuts to preference files.
  \param theShortcuts { moduleID, { inModuleActionID, {oldKeySequence, newKeySequence} }[] }[]. Empty inModuleActionIDs are ignored.
  OldKeySequences are ignored. */
  static void saveShortcutsToPreferences(const std::map<QString, std::map<QString, std::pair<QKeySequence, QKeySequence>>>& theShortcutsInversed);

  /*! Fills myActionAssets from asset files in theLanguage.
  \param theLanguage If default, fills assets in current language.
  If an asset in requested language is not found, seeks for the asset EN in and then in FR.

  Asset files must be structured like this:
  {
    ...
    actionID : {
      "langDependentAssets": {
        ...
        lang: {
          "name": name,
          "tooltip": tooltip
        },
        ...
      },
      "iconPath": iconPath
    },
    ...
  }
  */
  void setAssetsFromResources(QString theLanguage = QString());

  void registerAnonymousShortcut(QAction* const theAction);
  void enableAnonymousShortcutsClashingWith(const QString& theModuleID, const bool theEnable) const;
  void enableAnonymousShortcutsClashingWith(const QKeySequence& theKeySequence, bool theEnable) const;
  QString anonymousShortcutsToString() const;

private:
  static SUIT_ShortcutMgr* myShortcutMgr;

  /** { moduleID, { inModuleActionID, action[] }[] }[]. May contain entries like { <non-root module ID>, { <meta-action ID>, actions[] } }. */
  std::map<QString, std::map<QString, std::set<QAction*>>> myActions;

  /** { action, { moduleID, inModuleActionID } }[]. May contain entries like { <non-root module ID>, { <meta-action ID>, actions[] } }. */
  std::map<QAction*, std::pair<QString, QString>> myActionIDs; // To maintain uniqueness of actions and effectively retrieve IDs of registered actions.

  /** Can not contain entries like { <non-root module ID>, { <meta-action ID>, actions[] } }. */
  SUIT_ShortcutContainer myShortcutContainer;

  /* nb!
  Sets of moduleIDs and inModuleActionIDs are equal for myActions and myActionIDs.
  Sets of moduleIDs and inModuleActionIDs may NOT be equal for myActions and myShortcutContainer.
  */

  /** { moduleID, {inModuleActionID, assets}[] }[] */
  std::map<QString, std::map<QString, std::shared_ptr<SUIT_ActionAssets>>> myActionAssets;

  /** {moduleID, assets}[] */
  mutable std::map<QString, std::shared_ptr<SUIT_ActionAssets>> myModuleAssets;

  mutable std::set<QString> myActiveModuleIDs;

  /** Actions without IDs, but with hard-coded non-empty key sequences.
  * Shortcuts, defined in preferences, override shortcuts of anonymous actions - if an active module has a preference shortcut,
  * anonymous shortcuts with the same key sequence are disabled. If the root module has a preference shortcut, which
  * is in clash with anonymous shortcuts, clashing anonymous actions are always disabled. */
  std::map<QAction*, QKeySequence> myAnonymousShortcuts;

  std::map<QKeySequence, std::set<QAction*>> myAnonymousShortcutsInverse;
};


/*!
  \class SUIT_SentenceMatcher
  \brief Approximate string matcher, treats strings as sentences composed of words.
*/
class SUIT_EXPORT SUIT_SentenceMatcher
{
public:
  /*! Default config:
    Exact word order = false;
    Fuzzy words = true;
    Case sensitive = false;
    Query = ""; // matches nothing.
  */
  SUIT_SentenceMatcher();

  void setUseExactWordOrder(bool theOn);
  void setUseFuzzyWords(bool theOn);
  void setCaseSensitive(bool theOn);
  inline bool isCaseSensitive() const { return myIsCaseSensitive; };

  /*! \param theQuery should not be regex. */
  void setQuery(QString theQuery);

  inline const QString& getQuery() const { return myQuery; };

  /*! \returns match metrics. The metrics >= 0. INF means mismatch.
  The class is unable to differentiate exact match with some approximate matches! */
  double match(const QString& theInputString) const;

  /** \brief For debug. */
  QString toString() const;

private:
  static bool makePermutatedSentences(const QStringList& theWords, QList<QStringList>& theSentences);
  static void makeFuzzyWords(const QStringList& theWords, QStringList& theFuzzyWords);

  /*! \returns number of characters in matched words. The number >= 0. */
  static int matchWithSentenceIgnoreEndings(const QString& theInputString, const QStringList& theSentence, bool theCaseSensitive);
  /*! \returns number of characters in matched words. The number >= 0. */
  static int matchWithSentencesIgnoreEndings(const QString& theInputString, const QList<QStringList>& theSentences, bool theCaseSensitive);

  /*! \returns number of characters in matched words. The number >= 0. */
  static int matchAtLeastOneWord(const QString& theInputString, const QStringList& theWords, bool theCaseSensitive);

  /*! \returns number of characters in matched words. The number >= 0. */
  static int match(
    const QString& theInputString,
    const QStringList& theSentence,
    bool theCaseSensitive
  );

  /*! \returns number of characters in matched words. The number >= 0. */
  static int match(
    const QString& theInputString,
    const QList<QStringList>& theSentences,
    bool theCaseSensitive
  );

  bool myUseExactWordOrder; // If false, try to match with sentences, composed of query's words in different orders.
  bool myUseFuzzyWords; // Try to match with sentences, composed of query's truncated words.
  bool myIsCaseSensitive;
  QString myQuery;

  QStringList myWords; // It is also original search sentence.
  QList<QStringList> myPermutatedSentences;

  QStringList myFuzzyWords; // Regexes.
  QList<QStringList> myFuzzyPermutatedSentences;
};


/*!
  \class SUIT_ActionSearcher
  \brief Searches in data, provided in action asset files and shortcut preferences.
*/
class SUIT_EXPORT SUIT_ActionSearcher
{
public:
  enum MatchField {
    ID,
    Name,
    ToolTip,
    KeySequence
  };

  class AssetsAndSearchData
  {
  public:
    AssetsAndSearchData(std::shared_ptr<const SUIT_ActionAssets> theAssets = nullptr, double theMatchMetrics = std::numeric_limits<double>::infinity());

    void setMatchMetrics(double theMatchMetrics);
    double matchMetrics() const { return myMatchMetrics; };

    std::shared_ptr<const SUIT_ActionAssets> myAssets;

    void toJSON(QJsonObject& oJsonObject) const;
    QString toString() const;

  private:
    /*! \brief Ideally it should be number of weighted character permutations. Now it is just a number of characters in unmatched words. */
    double myMatchMetrics;
  };

  /*! Default config:
      Included modules' IDs = { ROOT_MODULE_ID };
      Include disabled actions = false;
      Fields to match = { Name, Tooltip };
      Case sensitive = false;
      Fuzzy matching = true;
      Query = ""; // matches everything.
  */
  SUIT_ActionSearcher();
  SUIT_ActionSearcher(const SUIT_ActionSearcher&) = delete;
  SUIT_ActionSearcher& operator=(const SUIT_ActionSearcher&) = delete;
  virtual ~SUIT_ActionSearcher() = default;

  /*! \returns true, if set of results is changed. */
  bool setIncludedModuleIDs(std::set<QString> theIncludedModuleIDs);

  /*! \returns true, if set of results is changed. */
  bool includeDisabledActions(bool theOn);
  inline bool areDisabledActionsIncluded() const {return myIncludeDisabledActions;};

  /*! \returns true, if set of results is changed. */
  bool setFieldsToMatch(const std::set<SUIT_ActionSearcher::MatchField>& theFields);

  /*! \returns true, if set of results is changed. */
  bool setCaseSensitive(bool theOn);

  /*! \returns true, if set of results is changed. */
  bool setQuery(const QString& theQuery);
  inline const QString& getQuery() const {return myMatcher.getQuery();};

  const std::map<QString, std::map<QString, SUIT_ActionSearcher::AssetsAndSearchData>>& getSearchResults() const;


private:
  /*! \brief Applies filter to all actions, provided in asset files for SUIT_ShortcutMgr.
  \returns { true, _ } if set of results is changed; { _ , true } if matching metrics is changed for at least one result. */
  std::pair<bool, bool> filter();

  /*! \brief Applies filter to search results only.
  \returns { true, _ } if set of results is shrunk; { _ , true } if matching metrics is changed for at least one result. */
  std::pair<bool, bool> filterResults();

  /*! \brief Applies filter only to actions, which are not in search results.
  \returns True, if set of results is extended. */
  bool extendResults();

  double matchAction(const QString& theModuleID, const QString& theInModuleActionID, std::shared_ptr<const SUIT_ActionAssets> theAssets);

  QString toString() const;


  std::set<QString> myIncludedModuleIDs;
  bool myIncludeDisabledActions;

  std::set<SUIT_ActionSearcher::MatchField> myFieldsToMatch;
  SUIT_SentenceMatcher myMatcher;

  /* { moduleID, {inModuleActionID, assetsAndSearchData}[] }[]. */
  std::map<QString, std::map<QString, SUIT_ActionSearcher::AssetsAndSearchData>> mySearchResults;
};


#if defined WIN32
#pragma warning( default: 4251 )
#endif

#endif
