
#include <OB_FindDlg.h>
#include <OB_Browser.h>

#include <QtxTreeView.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>

OB_FindDlg::OB_FindDlg( OB_Browser* b )
: QGroupBox( tr( "FIND" ), b ),
  myBrowser( b )
{
  QVBoxLayout *vbox = new QVBoxLayout();
  
  QFrame *btns = new QFrame( this ), *checks = new QFrame( this );
  vbox->addWidget( btns );
  vbox->addWidget( checks );
  setLayout( vbox );
  
  myData = new QLineEdit( btns );
  myToFirst = new QPushButton( "|<<", btns );
  myToLast = new QPushButton( ">>|", btns );
  myNext = new QPushButton( ">>", btns );
  myPrev = new QPushButton( "<<", btns );
  myClose = new QPushButton( tr( "CLOSE" ), checks );
  myIsCaseSens = new QCheckBox( tr( "CASE_SENSITIVE" ), checks );
  myIsRegExp = new QCheckBox( tr( "IS_REG_EXP" ), checks );
  int w = 30, h = myToFirst->height();
  myToFirst->setMaximumSize( w, h );
  myToLast->setMaximumSize( w, h );
  myNext->setMaximumSize( w, h );
  myPrev->setMaximumSize( w, h );

  QHBoxLayout* l = new QHBoxLayout( btns );
  l->addWidget( myToFirst, 0 );
  l->addWidget( myPrev, 0 );
  l->addWidget( myData, 1 );
  l->addWidget( myNext, 0 );
  l->addWidget( myToLast, 0 );

  QHBoxLayout* c = new QHBoxLayout( checks );
  c->addWidget( myIsCaseSens, 0 );
  //c->addWidget( myIsRegExp, 0 );
  myIsRegExp->hide();//temporary hidden
  
  c->addWidget( myClose, 0 );

  connect( myToFirst, SIGNAL( clicked() ), this, SLOT( onFind() ) );
  connect( myToLast, SIGNAL( clicked() ), this, SLOT( onFind() ) );
  connect( myNext, SIGNAL( clicked() ), this, SLOT( onFind() ) );
  connect( myPrev, SIGNAL( clicked() ), this, SLOT( onFind() ) );
  connect( myClose, SIGNAL( clicked() ), this, SLOT( onClose() ) );
}

OB_FindDlg::~OB_FindDlg()
{
}

void OB_FindDlg::onClose()
{
  hide();
}

void OB_FindDlg::onFind()
{
  const QModelIndexList& l = myBrowser->selectedIndexes();
  QModelIndex current;
  if( l.count()>0 )
    current = l.first();

  Qt::MatchFlags fl = Qt::MatchContains | Qt::MatchRecursive;
  if( myIsCaseSens->isChecked() )
    fl = fl | Qt::MatchCaseSensitive;
  if( myIsRegExp->isChecked() )
    fl = fl | Qt::MatchRegExp;

  QModelIndexList found = myBrowser->model()->match( myBrowser->model()->index( 0, 0 ),
                                                     0, myData->text(), -1, fl );
  QModelIndex f;
  if( found.count()>0 )
    if( sender()==myToFirst )
      f = found.first();
    else if( sender()==myToLast )
      f = found.last();
    else
    {
      int ind = found.indexOf( current );
      if( ind<0 )
        ind = 0;
      if( sender()==myNext )
      {
        ind++;
        if( ind>=found.count() )
          ind = 0;
        f = found[ ind ];
      }
      else if( sender()==myPrev )
      {
        ind--;
        if( ind<0 )
          ind = found.count()-1;
        f = found[ ind ];
      }
    }
  myBrowser->select( f, true, false );
  myBrowser->treeView()->scrollTo( f );
}
