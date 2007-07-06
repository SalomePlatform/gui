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
// File:      QtxPathListEdit.cxx
// Author:    Sergey TELKOV

#include "QtxPathListEdit.h"

#include "QtxPathEdit.h"

#include <QLayout>
#include <QPainter>
#include <QListView>
#include <QLineEdit>
#include <QKeyEvent>
#include <QDirModel>
#include <QCompleter>
#include <QToolButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QItemDelegate>
#include <QStringListModel>

static const char* delete_icon[] = {
"16 16 3 1",
"` c #810000",
"  c none",
"# c #ffffff",
"                ",
"                ",
" ``#        ``# ",
" ````#     ``#  ",
"  ````#   ``#   ",
"    ```# `#     ",
"     `````#     ",
"      ```#      ",
"     `````#     ",
"    ```# ``#    ",
"   ```#   ``#   ",
"  ```#     `#   ",
"  ```#      `#  ",
"   `#        `# ",
"                ",
"                "
};

static const char* insert_icon[] = {
"16 16 5 1",
"` c #000000",
". c #ffff00",
"# c #9d9da1",
"  c none",
"b c #ffffff",
"                ",
"                ",
" #  #b #.       ",
"  # #.#.` ` `   ",
"  .#.b####   `  ",
" ### ..         ",
"  . # .#     `  ",
" #` #.          ",
"    #        `  ",
"  `             ",
"             `  ",
"  `             ",
"             `  ",
"  ` ` ` ` ` `   ",
"                ",
"                "
};

static const char* movedown_icon[] = {
"16 16 2 1",
"` c #000000",
"  c none",
"                ",
"                ",
"         ```    ",
"        ```     ",
"       ```      ",
"       ```      ",
"       ```      ",
"       ```      ",
"   ```````````  ",
"    `````````   ",
"     ```````    ",
"      `````     ",
"       ```      ",
"        `       ",
"                ",
"                "
};

static const char* moveup_icon[] = {
"16 16 2 1",
"` c #000000",
"  c none",
"                ",
"                ",
"        `       ",
"       ```      ",
"      `````     ",
"     ```````    ",
"    `````````   ",
"   ```````````  ",
"       ```      ",
"       ```      ",
"       ```      ",
"       ```      ",
"      ```       ",
"     ```        ",
"                ",
"                "
};

/*!
  \class QtxPathListEdit::Editor
*/
class QtxPathListEdit::Editor : public QtxPathEdit
{
public:
  Editor( QWidget* parent = 0 ) : QtxPathEdit( parent )
  {
    layout()->setSpacing( 0 );
    lineEdit()->setFrame( false );
  }

  virtual ~Editor() {}
};

/*!
  \class QtxPathListEdit::Delegate
*/
class QtxPathListEdit::Delegate : public QItemDelegate
{
public:
  Delegate( QtxPathListEdit*, QObject* = 0 );
  virtual ~Delegate();

  virtual QWidget* createEditor( QWidget*, const QStyleOptionViewItem&, const QModelIndex& ) const;
  virtual void     setModelData( QWidget*, QAbstractItemModel*, const QModelIndex& ) const;
  virtual void     setEditorData( QWidget*, const QModelIndex& ) const;
  virtual void     paint( QPainter*, const QStyleOptionViewItem&, const QModelIndex& ) const;

protected:
  virtual void     drawFocus( QPainter*, const QStyleOptionViewItem&, const QRect& ) const;

private:
  QtxPathListEdit* myPathEdit;
};

QtxPathListEdit::Delegate::Delegate( QtxPathListEdit* pe, QObject* parent )
: QItemDelegate( parent ),
myPathEdit( pe )
{
}

QtxPathListEdit::Delegate::~Delegate()
{
}

QWidget* QtxPathListEdit::Delegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option,
                                                  const QModelIndex& index ) const
{
  return myPathEdit->createEditor( parent );
}

void QtxPathListEdit::Delegate::setModelData( QWidget* editor, QAbstractItemModel* model,
                                              const QModelIndex& index ) const
{
  myPathEdit->setModelData( editor, index );
}

void QtxPathListEdit::Delegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
  myPathEdit->setEditorData( editor, index );
}

void QtxPathListEdit::Delegate::paint( QPainter* painter, const QStyleOptionViewItem& option,
                                       const QModelIndex& index ) const
{
  QPalette::ColorGroup cg = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
  if ( cg == QPalette::Normal && !( option.state & QStyle::State_Active ) )
    cg = QPalette::Inactive;

  if ( option.state & QStyle::State_Selected )
  {
    painter->fillRect( option.rect, option.palette.brush( cg, QPalette::Highlight ) );
    painter->setPen( option.palette.color( cg, QPalette::HighlightedText ) );
  }
  else
    painter->setPen( option.palette.color( cg, QPalette::Text ) );

  QItemDelegate::paint( painter, option, index );
}

void QtxPathListEdit::Delegate::drawFocus( QPainter* painter, const QStyleOptionViewItem& option,
                                           const QRect& rect ) const
{
  QItemDelegate::drawFocus( painter, option, option.rect );
}

/*!
  \class QtxPathListEdit
*/
QtxPathListEdit::QtxPathListEdit( const Qtx::PathType type, QWidget* parent )
: QFrame( parent ),
myCompleter( 0 ),
myType( type ),
myDuplicate( false )
{
  initialize();
}

QtxPathListEdit::QtxPathListEdit( QWidget* parent )
: QFrame( parent ),
myCompleter( 0 ),
myType( Qtx::PT_OpenFile ),
myDuplicate( false )
{
  initialize();
}

/*!
  Destructor
*/
QtxPathListEdit::~QtxPathListEdit()
{
}

Qtx::PathType QtxPathListEdit::pathType() const
{
  return myType;
}

void QtxPathListEdit::setPathType( const Qtx::PathType t )
{
  if ( myType == t )
    return;

  myType = t;

  delete myCompleter;
  myCompleter = 0;
}

QStringList QtxPathListEdit::pathList() const
{
  return myModel->stringList();
}

void QtxPathListEdit::setPathList( const QStringList& lst )
{
  myModel->setStringList( lst );
}

bool QtxPathListEdit::isDuplicateEnabled() const
{
  return myDuplicate;
}

void QtxPathListEdit::setDuplicateEnabled( const bool on )
{
  myDuplicate = on;
}

int QtxPathListEdit::count() const
{
  return myModel->rowCount();
}

bool QtxPathListEdit::contains( const QString& path ) const
{
  return myModel->stringList().contains( path );
}

void QtxPathListEdit::clear()
{
  myModel->removeRows( 0, myModel->rowCount() );
}

void QtxPathListEdit::remove( const int idx )
{
  if ( 0 <= idx && idx < myModel->rowCount() )
    myModel->removeRow( idx );
}

void QtxPathListEdit::remove( const QString& path )
{
  QModelIndexList indexes = myModel->match( myModel->index( 0, 0 ), Qt::DisplayRole, path,
                                            myModel->rowCount(), Qt::MatchExactly | Qt::MatchCaseSensitive );
  while ( !indexes.isEmpty() )
  {
    myModel->removeRow( indexes.last().row() );
    indexes.removeLast();
  }
}

void QtxPathListEdit::insert( const QString& path, const int idx )
{
  int index = idx < 0 ? myModel->rowCount() : qMin( idx, myModel->rowCount() );
  if ( myModel->insertRow( index ) )
    myModel->setData( myModel->index( index, 0 ), path, Qt::EditRole );
}

/*!
  Validates entered path, returns true if OK
*/
/*
bool QtxPathListEdit::validate( const bool quietMode )
{
  if ( myEdited )
  {
    QString dirPath = QFileInfo( myEdit->text().stripWhiteSpace() ).filePath();
    QDir dir(dirPath);
    QListBoxItem* found = 0;
    for (unsigned i = 0; i < myList->count()-1; i++) {
      QDir aDir(myList->text(i));
      if ( aDir.canonicalPath().isNull() && myList->text(i) == dir.absPath() ||
          !aDir.canonicalPath().isNull() && aDir.exists() && aDir.canonicalPath() == dir.canonicalPath()) {
          found = myList->item(i);
        break;
      }
    }
    if (dirPath.isEmpty()) {
      if (found) {
        // it should be last (empty) item in the list - nothing to do
        return true;
      }
      else {
        // delete directory from the list
        removeDir(myLastSelected);
        return true;
      }
    }
    else {
      if (found) {
        if (found != myLastSelected) {
          // it is forbidden to add directory more then once
	  if ( !quietMode )
	    QMessageBox::critical(this, 
				  tr("Error"),
				  tr("Directory already specified."), 
				  tr("Ok"));
	  myEdit->setFocus();
          return false;
        }
      }
      else {
        if (!dir.exists()) {
	  if ( !quietMode && QMessageBox::information(this, 
						      tr("Warning"),
						      tr("%1\n\nThe directory doesn't exist.\nAdd directory anyway?").arg(dir.absPath()),
						      tr("Yes"), tr("No"), QString::null, 1, 1) == 1) {
	    myEdit->setFocus();
            return false;
	  }
        }
        // append
        appendDir(myLastSelected, dir.absPath());
      }
    }
  }
  return true;
}
*/
/*!
  Event filter
*/
bool QtxPathListEdit::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == QEvent::KeyPress )
  {
    QKeyEvent* ke = (QKeyEvent*)e;
    if ( ke->key() == Qt::Key_Delete )
      onDelete();
    else if ( ke->key() == Qt::Key_Insert )
      onInsert();
    else if ( ke->key() == Qt::Key_Up && ke->modifiers() == Qt::CTRL )
    {
      onUp();
      return true;
    }
    else if ( ke->key() == Qt::Key_Down && ke->modifiers() == Qt::CTRL )
    {
      onDown();
      return true;
    }
  }

  return QFrame::eventFilter( o, e );
}

/*!
  <Insert> button slot
*/

void QtxPathListEdit::onInsert( bool )
{
  int empty = -1;
  QStringList lst = myModel->stringList();
  for ( int r = 0; r < lst.count() && empty == -1; r++ )
  {
    if ( lst.at( r ).isEmpty() )
      empty = r;
  }

  if ( empty == -1 )
    myModel->insertRows( empty = myModel->rowCount(), 1 );

  QModelIndex idx = myModel->index( empty, 0 );
  myList->setCurrentIndex( idx );
  myList->edit( idx );
}

/*!
  <Delete> button slot
*/
void QtxPathListEdit::onDelete( bool )
{
  QModelIndex idx = myList->currentIndex();
  if ( !idx.isValid() )
    return;

  myModel->removeRow( idx.row() );
}

/*!
  <Move up> button slot
*/
void QtxPathListEdit::onUp( bool )
{
  QModelIndex idx = myList->currentIndex();
  if ( !idx.isValid() || idx.row() < 1 )
    return;

  QModelIndex toIdx = myModel->index( idx.row() - 1, 0 );

  QVariant val = myModel->data( toIdx, Qt::DisplayRole );
  myModel->setData( toIdx, myModel->data( idx, Qt::DisplayRole ), Qt::DisplayRole );
  myModel->setData( idx, val, Qt::DisplayRole );

  myList->setCurrentIndex( toIdx );
}

/*!
  <Move down> button slot
*/
void QtxPathListEdit::onDown( bool )
{
  QModelIndex idx = myList->currentIndex();
  if ( !idx.isValid() || idx.row() >= myModel->rowCount() - 1 )
    return;

  QModelIndex toIdx = myModel->index( idx.row() + 1, 0 );

  QVariant val = myModel->data( toIdx, Qt::DisplayRole );
  myModel->setData( toIdx, myModel->data( idx, Qt::DisplayRole ), Qt::DisplayRole );
  myModel->setData( idx, val, Qt::DisplayRole );

  myList->setCurrentIndex( toIdx );
}

void QtxPathListEdit::initialize()
{
  QVBoxLayout* base = new QVBoxLayout( this );
  base->setMargin( 0 );
  base->setSpacing( 5 );

  QWidget* cBox = new QWidget( this );
  base->addWidget( cBox );
  
  QHBoxLayout* cLayout = new QHBoxLayout( cBox );
  cLayout->setMargin( 0 );
  cLayout->setSpacing( 0 );

  cLayout->addStretch( 1 );

  QToolButton* insertBtn = new QToolButton( cBox );
  insertBtn->setIcon( QPixmap( insert_icon ) );
  cLayout->addWidget( insertBtn );

  QToolButton* deleteBtn = new QToolButton( cBox );
  deleteBtn->setIcon( QPixmap( delete_icon ) );
  cLayout->addWidget( deleteBtn );

  QToolButton* upBtn = new QToolButton( cBox );
  upBtn->setIcon( QPixmap( moveup_icon ) );
  cLayout->addWidget( upBtn );

  QToolButton* downBtn = new QToolButton( cBox );
  downBtn->setIcon( QPixmap( movedown_icon ) );
  cLayout->addWidget( downBtn );


  myList = new QListView( this );
  myList->setAlternatingRowColors( true );
  myList->setItemDelegate( new Delegate( this, myList ) );
  myList->setModel( myModel = new QStringListModel( myList ) );
  myList->setSelectionMode( QListView::SingleSelection );
  myList->setSelectionBehavior( QListView::SelectRows );
  myList->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  myList->setEditTriggers( QListView::DoubleClicked );
  myList->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
  myList->installEventFilter( this );

  base->addWidget( myList );

  connect( insertBtn, SIGNAL( clicked( bool ) ), this, SLOT( onInsert( bool ) ) );
  connect( deleteBtn, SIGNAL( clicked( bool ) ), this, SLOT( onDelete( bool ) ) );
  connect( upBtn,     SIGNAL( clicked( bool ) ), this, SLOT( onUp( bool ) ) );
  connect( downBtn,   SIGNAL( clicked( bool ) ), this, SLOT( onDown( bool ) ) );
}

QWidget* QtxPathListEdit::createEditor( QWidget* parent )
{
  QtxPathEdit* edit = new Editor( parent );
  edit->setPathType( pathType() );
  return edit;
}

void QtxPathListEdit::setModelData( QWidget* editor, const QModelIndex& index )
{
  QtxPathEdit* edit = ::qobject_cast<QtxPathEdit*>( editor );
  if ( !edit )
    return;

  QString path = edit->path().trimmed();

  if ( !isDuplicateEnabled() && !checkDuplicate( path, index.row() ) )
    return;

  if ( !checkExistance( path ) )
    return;

  myModel->setData( index, path, Qt::EditRole );
}

void QtxPathListEdit::setEditorData( QWidget* editor, const QModelIndex& index )
{
  QtxPathEdit* edit = ::qobject_cast<QtxPathEdit*>( editor );
  if ( !edit )
    return;

  QVariant v = myModel->data( index, Qt::EditRole );
  edit->setPath( v.toString() );
}

bool QtxPathListEdit::checkExistance( const QString& str, const bool msg )
{
  if ( pathType() == Qtx::PT_SaveFile )
    return true;

  bool ok = QFileInfo( str ).exists();
  if ( !ok && msg )
    ok = QMessageBox::question( this, tr( "Warning" ), tr( "Path \"%1\" doesn't exist. Add it to list anyway?" ).arg( str ),
                                QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes;

  if ( ok && QFileInfo( str ).exists() )
  {
    switch ( pathType() )
    {
    case Qtx::PT_OpenFile:
      ok = QFileInfo( str ).isFile();
      if ( !ok && msg )
        QMessageBox::warning( this, tr( "Error" ), tr( "Location \"%1\" doesn't point to file" ).arg( str ) );
      break;
    case Qtx::PT_Directory:
      ok = QFileInfo( str ).isDir();
      if ( !ok && msg )
        QMessageBox::warning( this, tr( "Error" ), tr( "Location \"%1\" doesn't point to directory" ).arg( str ) );
      break;
    }
  }

  return ok;
}

bool QtxPathListEdit::checkDuplicate( const QString& str, const int row, const bool msg )
{
  int cur = -1;
  QStringList lst = myModel->stringList();
  for ( int r = 0; r < lst.count() && cur == -1; r++ )
  {
    if ( r != row && lst.at( r ) == str )
      cur = r;
  }

  if ( cur != -1 && msg )
    QMessageBox::warning( this, tr( "Error" ), tr( "Path \"%1\" already exist in the list" ).arg( str ) );
   
  return cur == -1;
}
