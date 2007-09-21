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
#if !defined(TABLEVIEWER_VIEWWINDOW_H)
#define TABLEVIEWER_VIEWWINDOW_H

#include "TableViewer.h"
#include "SUIT_ViewWindow.h"
#include "QtxAction.h"

#include "HTMLService_HTMLTable.hxx"

class TableViewer_Viewer;
class SUIT_ViewWindow;
class SUIT_Desktop;
class Handle(HTMLService_HTMLTable);
class QImage;
class QtxTable;
class QToolBar;

class TABLEVIEWER_EXPORT TableViewer_ViewWindow: public SUIT_ViewWindow
{
  Q_OBJECT

public:
  TableViewer_ViewWindow( SUIT_Desktop* , TableViewer_Viewer* );
  ~TableViewer_ViewWindow();

  QtxTable*           table() const;
  QToolBar*           getToolBar() { return myToolBar; }

  virtual QImage      dumpView();

protected:
  typedef enum { VerticalHeader, HorizontalHeader, Cells } ContentType;
  typedef enum { DumpId, CopyId, PrintId, ExportId, Custom } ActionId;
  typedef QMap<int, QtxAction*> ActionsMap;

  virtual void        createActions();
  virtual void        createToolBar();
  virtual void        actionActivated( const int );
  virtual void        exportTableData( Handle(HTMLService_HTMLTable)&,
                                       const ContentType, const int, const int );

  void                registerAction( const int, QtxAction* );
  QtxAction*          createAction( const int, const QString&, const QPixmap&, const QString&,
                                    const QString&, const int = 0, QObject* = 0 );

  int                 numRows( const ContentType ) const;
  int                 numCols( const ContentType ) const;

  QString             text( const ContentType, const int, const int ) const;
  QString             image( const ContentType, const int, const int ) const;
  int                 fontFlags( const ContentType, const int, const int ) const;
  QColor              foregroundColor( const ContentType, const int, const int ) const;
  QColor              backgroundColor( const ContentType, const int, const int ) const;

private slots:
  void                onActivated();

private:
  void                exportData();

protected:
  TableViewer_Viewer* myModel;
  ActionsMap          myActionsMap;

private:
  QtxTable*           myTable;
  QToolBar*           myToolBar;
};

#endif // !defined(TABLEVIEWER_VIEWWINDOW_H)

