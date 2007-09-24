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
// TableViewer_ViewModel.cxx: implementation of the TableViewer_ViewModel class.

#include "TableViewer_ViewWindow.h"
#include "TableViewer_ViewModel.h"
#include "TableViewer_Tool.h"

#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_Application.h>
#include <SUIT_ViewManager.h>

#include <HTMLService_HTMLTable.hxx>
#include <HTMLService_HTMLTableCell.hxx>

#include <QtxToolBar.h>

#include <QtxTable.h>
#include <QImage>
#include <QPixmap>
#include <QApplication>
#include <QFont>

/*!
  Constructor
*/
TableViewer_ViewWindow::TableViewer_ViewWindow( SUIT_Desktop* theDesktop,
                                                TableViewer_Viewer* theModel )
:SUIT_ViewWindow( theDesktop )
{
  myModel = theModel;
  myTable = new QtxTable( this );

  //myTable->setReadOnly( true );
  setCentralWidget( myTable );

  myToolBar = new QtxToolBar( true, tr("LBL_TOOLBAR_LABEL"), this );

  createActions();
  createToolBar();

  // fill table
  myTable->setColumnCount(3);
  myTable->setRowCount(5);
  myTable->setCellBackground( 1, 1, Qt::red );
  myTable->setCellForeground( 1, 1, Qt::blue );
  QFont aFont = myTable->font();
  aFont.setItalic( true );
  myTable->setCellFont( 1, 1, aFont );
  myTable->setCellFont( 0, 0, aFont );

  aFont.setUnderline( true );
  myTable->setCellFont( 0, 1, aFont );

  myTable->setCellBackground( 2, 2, myTable->cellBackground( 1, 1 ) );
  myTable->setCellForeground( 2, 2, myTable->cellForeground( 1, 1 ) );
  aFont = myTable->cellFont( 1, 1 );
  myTable->setCellFont( 2, 2, aFont );
  myTable->setCellBackground( 0, 1, myTable->cellBackground( 0, 0 ) );

  myTable->setHeaderData( Qt::Horizontal, 1, "Caption");
  myTable->setHeaderBackground( Qt::Horizontal, 1, Qt::green );
  myTable->setHeaderFont(Qt::Horizontal, 0, aFont );

  myTable->setHeaderFont(Qt::Vertical, 4, myTable->headerFont( Qt::Horizontal, 0 ) );
  //backgroundColor( HorizontalHeader, 1, 1 ) );
}

/*!
  Destructor
*/
TableViewer_ViewWindow::~TableViewer_ViewWindow()
{
}

QtxTable* TableViewer_ViewWindow::table() const
{
  return myTable;
}

QImage TableViewer_ViewWindow::dumpView()
{
  return QPixmap::grabWindow( table()->winId() ).toImage();
}

void TableViewer_ViewWindow::createActions()
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  createAction( DumpId, tr( "MEN_DUMP" ),
                resMgr->loadPixmap( "TableViewer", tr( "ICON_DUMP" ) ),
                tr( "TIP_DUMP" ), tr( "PRP_DUMP" ) );

  createAction( CopyId, tr( "MEN_COPY" ),
                resMgr->loadPixmap( "STD", tr( "ICON_EDIT_COPY" ) ),
                tr( "TIP_COPY" ), tr( "PRP_COPY" ) );

  createAction( PrintId, tr( "MEN_PRINT" ),
                resMgr->loadPixmap( "STD", tr( "ICON_PRINT" ) ),
                tr( "TIP_PRINT" ), tr( "PRP_PRINT" ) );

  createAction( ExportId, tr( "MEN_EXPORT" ),
                resMgr->loadPixmap( "TableViewer", tr( "ICON_EXPORT" ) ),
                tr( "TIP_EXPORT" ), tr( "PRP_EXPORT" ) );
}

int TableViewer_ViewWindow::numRows( const ContentType type ) const
{
  int aRows = 0;
  switch ( type ) {
    case HorizontalHeader:
      aRows = 1;
      break;
    case Cells:
    case VerticalHeader:
      aRows = myTable->rowCount();
      break;
    default:
      break;
  }
  return aRows;
}

int TableViewer_ViewWindow::numCols( const ContentType type ) const
{
  int aCols = 0;
  switch ( type ) {
    case VerticalHeader:
      aCols = 1;
      break;
    case Cells:
    case HorizontalHeader:
      aCols = myTable->columnCount();
      break;
    default:
      break;
  }
  return aCols;
}

QString TableViewer_ViewWindow::text( const ContentType type, const int row, const int col ) const
{
  QString aTxt = "";
  switch ( type ) {
    case VerticalHeader:
      aTxt = myTable->headerData( Qt::Vertical, row ).toString();
      break;
    case HorizontalHeader:
      aTxt = myTable->headerData( Qt::Horizontal, col ).toString();
      break;
    case Cells:
      if ( myTable->item( row, col ) )
        aTxt = myTable->item( row, col )->text();
      break;
    default:
      break;
  }
  return aTxt;
}

QString TableViewer_ViewWindow::image( const ContentType type, const int row, const int col ) const
{
  return "";
}

int TableViewer_ViewWindow::fontFlags( const ContentType type, const int row, const int col ) const
{
  int aFlags = 0;
  QFont aFont = myTable->font();
  switch ( type ) {
    case VerticalHeader:
      aFont = myTable->headerFont( Qt::Vertical, row );
      break;
    case HorizontalHeader:
      aFont = myTable->headerFont( Qt::Horizontal, col );
      break;
    case Cells:
      aFont = myTable->cellFont( row, col );
      break;
    default:
      break;
  }
  if ( aFont.bold() )
    aFlags |= HTMLService_HTMLText::Bold;
  if ( aFont.italic() )
    aFlags |= HTMLService_HTMLText::Bold;
  if ( aFont.underline() )
    aFlags |= HTMLService_HTMLText::Underline;
  if ( aFont.strikeOut() )
    aFlags |= HTMLService_HTMLText::StrikeOut;
  //Subscript

  return aFlags;
}

QColor TableViewer_ViewWindow::foregroundColor( const ContentType type, const int row,
                                                const int col ) const
{
  QColor aColor;
  switch ( type ) {
    case VerticalHeader:
    case HorizontalHeader: {
      bool aHor = type == HorizontalHeader;
      aColor = myTable->headerForeground( aHor ? Qt::Horizontal : Qt::Vertical,
                                          aHor ? col : row );
    }
      break;
    case Cells:
      aColor = myTable->cellForeground( row, col );
      break;
    default:
      break;
  }
  return aColor;
}

QColor TableViewer_ViewWindow::backgroundColor( const ContentType type, const int row,
                                                const int col ) const
{
  QColor aColor;
  switch ( type ) {
    case VerticalHeader:
    case HorizontalHeader: {
      bool aHor = type == HorizontalHeader;
      aColor = myTable->headerBackground( aHor ? Qt::Horizontal : Qt::Vertical,
                                          aHor ? col : row );
      }
      break;
    case Cells:
      aColor = myTable->cellBackground( row, col );
      break;
    default:
      break;
  }
  return aColor;
}

void TableViewer_ViewWindow::createToolBar()
{
  myToolBar->addAction( myActionsMap[DumpId] );
  myToolBar->addAction( myActionsMap[CopyId] );
  myToolBar->addAction( myActionsMap[PrintId] );
  myToolBar->addAction( myActionsMap[ExportId] );
}

void TableViewer_ViewWindow::actionActivated( const int id )
{
  switch ( id ) {
    case DumpId:
       onDumpView();
      break;
    case ExportId:
      exportData();
      break;
  }
}

void TableViewer_ViewWindow::registerAction( const int id, QtxAction* a )
{
  if ( !a )
    return;

  myActionsMap.insert( id, a );
  connect( a, SIGNAL( activated() ), this, SLOT( onActivated() ) );
}

QtxAction* TableViewer_ViewWindow::createAction( const int id, const QString& menu, const QPixmap& ico,
                                         const QString& tip, const QString& status,
                                         const int accel, QObject* parent )
{
  QtxAction* a = new QtxAction( menu, ico, menu, accel, parent ? parent : myDesktop );
  a->setToolTip( tip );
  a->setStatusTip( status );
  registerAction( id, a );
  return a;
}

void TableViewer_ViewWindow::onActivated()
{
  const QObject* obj = sender();
  const QtxAction* a = ::qobject_cast<const QtxAction*>( obj );
  if ( !a )
    return;

  int id = -1;
  for ( ActionsMap::const_iterator it = myActionsMap.begin();
        it != myActionsMap.end() && id == -1; ++it ) {
    if ( it.value() == a )
      id = it.key();
  }

  if ( id == -1 )
    return;

  actionActivated( id );
}

void TableViewer_ViewWindow::exportData()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  QString fileName = app->getFileName( false, QString::null, tr( "EXPORT_FILTER" ),
                                       tr( "EXPORT_TABLE" ), 0 );
  if ( fileName.isEmpty() )
    return;

  QApplication::setOverrideCursor( Qt::WaitCursor );

  int rows = numRows( Cells );
  int cols = numCols( Cells );
  int horOffset = numRows( HorizontalHeader );
  int verOffset = numCols( VerticalHeader );

  Handle(HTMLService_HTMLTable) table = new HTMLService_HTMLTable( rows + horOffset,
                                                                   cols + verOffset );
  QString title = getViewManager()->getTitle();
  SUIT_ViewWindow* anActiveWnd = getViewManager()->getActiveView();
  if ( anActiveWnd )
    title = anActiveWnd->windowTitle();

  table->SetTitle( TableViewer_Tool::ToExtString( title ) );
  table->SetWidth( 100, true );
  table->SetBorder( 2 );

  exportTableData( table, HorizontalHeader, 0, verOffset );
  exportTableData( table, VerticalHeader, horOffset, 0 );
  exportTableData( table, Cells, horOffset, verOffset );

  Handle(HTMLService_HTMLTableCell) ltCell = table->GetCell( 0, 0 );
  if ( !ltCell.IsNull() ) {
    ltCell->SetHeaderCell( true );
    ltCell->SetBackgroundColor( Quantity_NOC_GRAY );
  }
  table->GenerateFile( TableViewer_Tool::ToExtString( fileName ), true );

  QApplication::restoreOverrideCursor();
}

void TableViewer_ViewWindow::exportTableData( Handle(HTMLService_HTMLTable)& table,
                                              const ContentType type,
                                              const int rowOffset, const int colOffset )
{
  if ( table.IsNull() )
    return;

  bool isExpImg = true;
  bool dotToComma = false;
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if ( resMgr ) {
    isExpImg = resMgr->booleanValue( "TableViewer", "export_table_images", true );
    dotToComma = resMgr->booleanValue( "TableViewer", "replace_dot_by_comma", true );
  }

  for ( int c = 0, nbC = numCols( type ); c < nbC; c++ ) {
    for ( int r = 0, nbR = numRows( type ); r < nbR; r++ ) {
      Handle(HTMLService_HTMLTableCell) cell = 
         table->GetCell( r + rowOffset, c + colOffset );
      if ( cell.IsNull() )
        continue;
      cell->SetHeaderCell( type != Cells );
      if ( cell->IsHeaderCell() )
        cell->SetBackgroundColor( Quantity_NOC_GRAY );
      if ( isExpImg ) {
        QString img = image( type, r, c );
        if ( !img.isEmpty() )
          cell->InsertImage( TableViewer_Tool::ToExtString( img ), 32, 32 );
      }
      QString txt = text( type, r, c );
      if ( !txt.isEmpty() ) {
        bool aDbl = false;
        double aVal = TableViewer_Tool::ToDouble( txt, aDbl );
        if ( aDbl )
          cell->InsertText( TableViewer_Tool::ToExtString( 
                            TableViewer_Tool::DoubleToQString( aVal, dotToComma ) ) );
        else
          cell->InsertText( TableViewer_Tool::ToExtString( txt ) );
      }
      if ( foregroundColor( type, r, c ).isValid() )
        cell->SetForegroundColor( TableViewer_Tool::Color( foregroundColor( type, r, c ) ) );
      if ( backgroundColor( type, r, c ).isValid() )
        cell->SetBackgroundColor( TableViewer_Tool::Color( backgroundColor( type, r, c ) ) );
      cell->SetFontFlags( fontFlags( type, r, c ) );
    }
  }
}
