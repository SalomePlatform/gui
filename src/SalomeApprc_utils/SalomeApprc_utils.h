#ifndef SALOMEAPPRC_UTILS_H
#define SALOMEAPPRC_UTILS_H

#include <QtCore>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#include <Qtx.h>
#include <SUIT_ResourceMgr.h>

const QString ITERACTIVE_EXTCOMPONENT_KEY("salome_interactive");

void UpdateCompInfo_with_salomeappdir(const QDir& salomeappdir, SUIT_ResourceMgr* ResMgr);
void UpdateCompInfo_with_salomemodules(const QString& salomemodules, const QDir& salomeappdir, SUIT_ResourceMgr* ResMgr);
void AddGuiComponent(const QString& comp, const QString& CompRoot, SUIT_ResourceMgr* ResMgr );

#endif