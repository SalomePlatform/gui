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
#include "STD_LoadStudiesDlg.h"

#include <QtGui/qlabel.h>
#include <QtGui/qlayout.h>
#include <QtGui/qlistwidget.h>
#include <QtGui/qpushbutton.h>

#define SPACING_SIZE             6
#define MARGIN_SIZE             11
#define MIN_LISTBOX_WIDTH      150
#define MIN_LISTBOX_HEIGHT     100

/*!
* \brief creates a Load study dialog box
* \param parent a parent widget
* \param modal bool argument, if true the dialog box is a modal dialog box
* \param f style flags
*/

STD_LoadStudiesDlg::STD_LoadStudiesDlg( QWidget* parent, bool modal )
: QDialog( parent )
{
  setModal( modal );
  setWindowTitle( tr("DLG_LOAD_STUDY_CAPTION") );
  setSizeGripEnabled( true );

    QGridLayout* aTopLayout = new QGridLayout(this);
    aTopLayout->setMargin(MARGIN_SIZE);
    aTopLayout->setSpacing(SPACING_SIZE);

    TextLabel1 = new QLabel( this );
    TextLabel1->setGeometry( QRect( 11, 12, 297, 16 ) ); 
    TextLabel1->setText( tr( "MEN_STUDIES_CHOICE"  ) );

    QHBoxLayout* aBtnLayout = new QHBoxLayout;
    aBtnLayout->setSpacing( SPACING_SIZE );
    aBtnLayout->setMargin( 0 );
    
    buttonOk = new QPushButton( this );
    buttonOk->setText( tr( "BUT_OK"  ) );
    buttonOk->setAutoDefault( true );
    buttonOk->setDefault( true );
    
    buttonCancel = new QPushButton( this );
    buttonCancel->setText( tr( "BUT_CANCEL"  ) );
    buttonCancel->setAutoDefault( true ); 
  
    aBtnLayout->addWidget( buttonOk );
    aBtnLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
    aBtnLayout->addWidget( buttonCancel );

    ListComponent = new QListWidget( this );
    ListComponent->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    ListComponent->setMinimumSize( MIN_LISTBOX_WIDTH, MIN_LISTBOX_HEIGHT );
    ListComponent->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    ListComponent->setSelectionMode( QListWidget::SingleSelection );

    aTopLayout->addWidget(TextLabel1,    0, 0);
    aTopLayout->addWidget(ListComponent, 1, 0);
    aTopLayout->addLayout(aBtnLayout,    2, 0);

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

