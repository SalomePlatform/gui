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

#include "QtxMultiAction.h"

#include <QMenu>
#include <QLayout>
#include <QToolBar>
#include <QPainter>
#include <QToolButton>
#include <QApplication>
#include <QStyleOptionButton>

class QtxMultiAction::Button : public QToolButton
{
public:
  Button( QWidget* parent = 0 ) : QToolButton( parent ) {}
  ~Button() {};

protected:
  virtual void paintEvent( QPaintEvent* e )
  {
    QToolButton::paintEvent( e );

    int s = 4;
    int m = 2;
    int w = width();
    int h = height();

    QStyleOptionButton opt;
    opt.initFrom( this );
    if ( isDown() )
      opt.rect = QRect( w - s - m, h - s - m, s, s );
    else
      opt.rect = QRect( w - s - m - 1, h - s - m - 1, s, s );

    QPainter p( this );
    style()->drawPrimitive( QStyle::PE_IndicatorArrowDown, &opt, &p );
  }
};

/*!
  Constructor
*/
QtxMultiAction::QtxMultiAction( QObject* parent )
: QtxActionSet( parent ),
myCurrent( 0 )
{
  setVisible( true );
  setMenu( new QMenu( 0 ) );

  connect( this, SIGNAL( triggered( QAction* ) ), this, SLOT( onTriggered( QAction* ) ) );
}

QtxMultiAction::QtxMultiAction( const QString& txt, QObject* parent )
: QtxActionSet( parent ),
myCurrent( 0 )
{
  setText( txt );
  setVisible( true );
  setMenu( new QMenu( 0 ) );

  connect( this, SIGNAL( triggered( QAction* ) ), this, SLOT( onTriggered( QAction* ) ) );
}

QtxMultiAction::QtxMultiAction( const QIcon& ico, const QString& txt, QObject* parent )
: QtxActionSet( parent ),
myCurrent( 0 )
{
  setIcon( ico );
  setText( txt );
  setVisible( true );
  setMenu( new QMenu( 0 ) );

  connect( this, SIGNAL( triggered( QAction* ) ), this, SLOT( onTriggered( QAction* ) ) );
}

QtxMultiAction::~QtxMultiAction()
{
}

void QtxMultiAction::onClicked( bool )
{
  if ( myCurrent )
    myCurrent->activate( QAction::Trigger );
}

void QtxMultiAction::onTriggered( QAction* a )
{
  if ( !a )
    return;

  QList<QWidget*> lst = createdWidgets();
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QToolButton* tb = ::qobject_cast<QToolButton*>( *it );
    if ( tb && tb->menu() )
      tb->menu()->hide();
  }

  if ( myCurrent != a )
  {
    myCurrent = a;
    updateAction();
  }
}

void QtxMultiAction::updateAction()
{
  QtxActionSet::updateAction();

  QList<QWidget*> lst = createdWidgets();
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end(); ++it )
    updateButton( ::qobject_cast<QToolButton*>( *it ) );
}

void QtxMultiAction::updateAction( QWidget* w )
{
  if ( !w )
    return;

  if ( w->inherits( "QMenu" ) )
  {
    QtxActionSet::updateAction( menu() );

    QApplication::instance()->removeEventFilter( this );

    menu()->removeAction( this );

    QApplication::instance()->installEventFilter( this );
  }
}

bool QtxMultiAction::isEmptyAction() const
{
  return false;
}

QWidget* QtxMultiAction::createWidget( QWidget* parent )
{
  QToolBar* tb = ::qobject_cast<QToolBar*>( parent );
  if ( !tb )
    return 0;

  QToolButton* w = new Button( tb );
  w->setMenu( new QMenu( w ) );
  w->setFocusPolicy( Qt::NoFocus );
  w->setIconSize( tb->iconSize() );
  w->setToolButtonStyle( tb->toolButtonStyle() );

  connect( w, SIGNAL( clicked( bool ) ), this, SLOT( onClicked( bool ) ) );
  connect( tb, SIGNAL( iconSizeChanged( const QSize& ) ), w, SLOT( setIconSize( QSize ) ) );
  connect( tb, SIGNAL( toolButtonStyleChanged( Qt::ToolButtonStyle ) ),
           w, SLOT( setToolButtonStyle( Qt::ToolButtonStyle ) ) );

  updateButton( w );
  return w;
}

void QtxMultiAction::actionAdded( QAction* a )
{
  if ( !myCurrent )
    myCurrent = a;
}

void QtxMultiAction::actionRemoved( QAction* a )
{
  if ( myCurrent != a )
    return;

  myCurrent = actions().isEmpty() ? 0 : actions().first();

  updateAction();
}

void QtxMultiAction::updateButton( QToolButton* btn )
{
  if ( !btn )
    return;

  btn->setIcon( myCurrent ? myCurrent->icon() : QIcon() );
  btn->setText( myCurrent ? myCurrent->text() : QString() );
  btn->setToolTip( myCurrent ? myCurrent->toolTip() : QString() );
  btn->setStatusTip( myCurrent ? myCurrent->statusTip() : QString() );

  QMenu* pm = btn->menu();
  if ( !pm )
    return;

  pm->clear();
  for ( int i = 0; pm->layout() && i < pm->layout()->count(); i++ )
    delete pm->layout()->widget();

  delete pm->layout();

  QVBoxLayout* vbox = new QVBoxLayout( pm );
  vbox->setMargin( 1 );
  vbox->setSpacing( 0 );

  QList<QAction*> actList = actions();
  for ( QList<QAction*>::iterator itr = actList.begin(); itr != actList.end(); ++itr )
  {
    QToolButton* b = new QToolButton( pm );
    b->setDefaultAction( *itr );
    b->setToolTip( (*itr)->toolTip() );
    b->setAutoRaise( true );
    b->setIconSize( btn->iconSize() );
    b->setToolButtonStyle( btn->toolButtonStyle() );
    b->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    vbox->addWidget( b );
  }

/*
  QList<QAction*> actList = actions();
  for ( QList<QAction*>::iterator itr = actList.begin(); itr != actList.end(); ++itr )
    pm->addAction( *itr );
*/
}
