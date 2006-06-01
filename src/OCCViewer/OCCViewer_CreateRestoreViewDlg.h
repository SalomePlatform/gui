// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef OCCVIEWER_CREATERESTOREVIEWDLG_H
#define OCCVIEWER_CREATERESTOREVIEWDLG_H

#include "OCCViewer.h"

#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewWindow.h"

#include <QtxDialog.h>
#include <QtxListBox.h> 
#include <SUIT_Application.h>

#include <qmap.h>
#include <qvbox.h>
#include <qdialog.h>
#include <qlistbox.h>
#include <qstringlist.h>

class OCCViewer_ViewPort3d;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class OCCVIEWER_EXPORT OCCViewer_CreateRestoreViewDlg : public QDialog
{
	Q_OBJECT

public:
	OCCViewer_CreateRestoreViewDlg( QWidget*, OCCViewer_Viewer* );
	virtual ~OCCViewer_CreateRestoreViewDlg();

	const viewAspectList&       parameters() const;
	viewAspect                  currentItem() const;
	virtual bool						    eventFilter( QObject*, QEvent* );

public slots:
	void	                      OKpressed();
	void	                      clearList();
	void                        editItemText( QListBoxItem* );
	void	                      changeImage( QListBoxItem* );
	void	                      deleteSelectedItems();

signals:
	void	                      dlgOk();

private:
	int								          myKeyFlag;
	QtxListBox*						      myListBox;
	OCCViewer_ViewPort3d*	      myCurViewPort;
	viewAspect                  myCurrentItem;
	viewAspectList              myParametersMap;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
