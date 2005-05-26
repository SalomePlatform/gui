// File:      SalomeApp_Module.h
// Created:   10/25/2004 11:33:06 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#ifndef SALOMEAPP_MODULE_H
#define SALOMEAPP_MODULE_H

#include "SalomeApp.h"
#include "SalomeApp_Selection.h"

#include <CAM_Module.h>

#include <string>

class QDockWindow;

class CAM_Study;

class QtxPopupMgr;

class SUIT_Operation;
class SUIT_Convertor;
class SUIT_ViewModel;
class SUIT_DataObject;

class SalomeApp_DataModel;
class SalomeApp_Application;
class SalomeApp_SelectionManager;

/*
  Class : SalomeApp_Module
  Description : Base class for all salome modules
*/

class SALOMEAPP_EXPORT SalomeApp_Module : public CAM_Module
{
  Q_OBJECT

public:
  SalomeApp_Module( const QString& );
  virtual ~SalomeApp_Module();

  virtual void                        initialize( CAM_Application* );
  virtual void                        windows( QMap<int, int>& ) const;
  virtual void                        viewManagers( QStringList& ) const;
  virtual QString                     engineIOR() const;

  virtual void                        contextMenuPopup( const QString&, QPopupMenu* );

  // Convenient shortcuts
  SalomeApp_Application*              getApp() const;
  
  void                                updateObjBrowser( bool = true, SUIT_DataObject* = 0 );

  virtual void                        selectionChanged();

public slots:
  virtual void                        activateModule( SUIT_Study* );
  virtual void                        deactivateModule( SUIT_Study* );
  void                                MenuItem();

protected slots:
  virtual void                        onModelSaved();
  virtual void                        onModelOpened();
  virtual void                        onModelClosed();

protected:
  QtxPopupMgr*                        popupMgr();
  virtual CAM_DataModel*              createDataModel();
  virtual SalomeApp_Selection*        createSelection() const;

private:
  QtxPopupMgr*                        myPopupMgr;
};

#endif
