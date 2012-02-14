// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File:      QtxBackgroundTool.cxx
// Author:    Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "QtxBackgroundTool.h"
#include "QtxColorButton.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>


/*!
  \class QtxBackgroundTool
  \brief Implementation of the widget managing background data

  The background data can be specified as:
  - image (by assigning the file name to be used as background texture)
  - single color (by assigning any color)
  - simple two-color gradient (with the gradient type id and two colors)
  - complex gradient (by assigning arbitrary gradient data) (NOT IMPLEMENTED YET)

  To enable / disable any background type,  setModeAllowed() function can be used.
  Widget's orientation can be specified via the constructor parameter of changed with 
  setOrientation() function.

  To specify two-color gradient modes, use setGradient() function. By default, no gradient modes
  are provided by the widget.
  
  Arbitrary gradient mode is not implemented yet, it is remaining for future improvement.

  Typical usage can be as follows:
  \code
  // create tool widget
  QtxBackgroundTool* tool = new QtxBackgroundTool( Qt::Vertical, this );
  // assign gradients types
  QStringList sl;
  sl << "Horizontal" << "Vertical" << "First diagonal" << "Second diagonal";
  tool->setGradients(sl);
  // disable image texture and custom gradient modes
  tool->setModeAllowed(Qtx::ImageBackground, false);
  tool->setModeAllowed(Qtx::CustomGradientBackground, false);
  // initialize widget with the some background data
  tool->setData( bgData );
  \endcode

  \todo Complete support of custom gradient (QLinearGradient, QRadialGradient, QConicalGradient).
  \sa Qtx::BackgroundData, QtxBackgroundDialog, Qtx::stringToBackground(), Qtx::backgroundToString()
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxBackgroundTool::QtxBackgroundTool( QWidget* parent )
  : QWidget( parent ), myLastGradient( -1 )
{
  init( Qt::Horizontal );
}

/*!
  \brief Constructor.
  \param o widget orientation
  \param parent parent widget
*/
QtxBackgroundTool::QtxBackgroundTool( Qt::Orientation o, QWidget* parent )
  : QWidget( parent ), myLastGradient( -1 )
{
  init( o );
}

/*!
  \brief Perform internal initialization
*/
void QtxBackgroundTool::init( Qt::Orientation o )
{
  // mode combo box
  myModeCombo = new QComboBox( this );
  // sub-widgets container
  myContainer = new QStackedWidget( this );

  QWidget* wrap;
  QHBoxLayout* wrapLayout;

  // add image controls
  wrap = new QWidget( this );
  wrapLayout = new QHBoxLayout( wrap );
  wrapLayout->setMargin( 0 );
  wrapLayout->setSpacing( 5 );
  myFileName    = new QLineEdit( wrap );
  myBrowseBtn   = new QPushButton( tr( "Browse..." ), wrap );
  myTextureMode = new QComboBox( wrap );
  wrapLayout->addWidget( myFileName );
  wrapLayout->addWidget( myBrowseBtn );
  wrapLayout->addWidget( myTextureMode );
  myContainer->addWidget( wrap ); // Image
  // add color controls
  wrap = new QWidget( this );
  wrapLayout = new QHBoxLayout( wrap );
  wrapLayout->setMargin( 0 );
  wrapLayout->setSpacing( 5 );
  myFirstColor  = new QtxColorButton( wrap );
  mySecondColor = new QtxColorButton( wrap );
  wrapLayout->addWidget( myFirstColor );
  wrapLayout->addWidget( mySecondColor );
  myContainer->addWidget( wrap ); // Color
  // add gradient controls ... NOT IMPLEMENTED YET
  wrap = new QWidget( this );
  wrapLayout = new QHBoxLayout( wrap );
  wrapLayout->setMargin( 0 );
  wrapLayout->setSpacing( 5 );
  QLabel* foo = new QLabel( tr( "Not implemented yet" ), wrap );
  foo->setAlignment( Qt::AlignCenter );
  wrapLayout->addWidget( foo );
  myContainer->addWidget( wrap ); // Gradient

  // initialize widget
  myFirstColor->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  mySecondColor->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  myTextureMode->addItem( tr( "Center" ),  Qtx::CenterTexture );
  myTextureMode->addItem( tr( "Tile" ),    Qtx::TileTexture );
  myTextureMode->addItem( tr( "Stretch" ), Qtx::StretchTexture );

  connect( myModeCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( updateState() ) );
  connect( myBrowseBtn, SIGNAL( clicked() ), this, SLOT( browse() ) );

  setModeAllowed( Qtx::ImageBackground );
  setModeAllowed( Qtx::ColorBackground );
  setModeAllowed( Qtx::SimpleGradientBackground );
  setModeAllowed( Qtx::CustomGradientBackground );
  setImageFormats( QString() );
  setOrientation( o );
}

/*!
  \brief Destructor.
*/
QtxBackgroundTool::~QtxBackgroundTool()
{
}

/*!
  \brief Get background data from the widget
  \return background data
  \sa setData()
*/
Qtx::BackgroundData QtxBackgroundTool::data() const
{
  Qtx::BackgroundData bgData;
  int idx = myModeCombo->currentIndex();
  if ( idx != -1 ) {
    // get currently selected mode
    int id = myModeCombo->itemData( idx, TypeRole ).toInt();
    // texture data
    if ( isModeAllowed( Qtx::ImageBackground ) ) {
      bgData.setTexture( myFileName->text().trimmed(),
			 Qtx::TextureMode( myTextureMode->itemData( myTextureMode->currentIndex() ).toInt() ) );
    }
    // single-color data
    if ( isModeAllowed( Qtx::ColorBackground ) ) {
      bgData.setColor( myFirstColor->color() );
    }
    // simple two-color gradient data
    if ( isModeAllowed( Qtx::SimpleGradientBackground ) ) {
      bgData.setGradient( myLastGradient, myFirstColor->color(), mySecondColor->color() );
    }
    // custom gradient data
    if ( isModeAllowed( Qtx::CustomGradientBackground ) ) {
      // bgData.setGradient( ... );
      // NOT IMPLEMENTED YET
    }

    // set current mode
    bgData.setMode( Qtx::BackgroundMode( id ) );
  }
  return bgData;
}

/*!
  \brief Set background data from the widget
  \param bgData background data being set to the widget
  \sa data()
*/
void QtxBackgroundTool::setData( const Qtx::BackgroundData& bgData )
{
  Qtx::BackgroundMode m = bgData.mode();
  QColor c1, c2;
  int     gtype = bgData.gradient( c1, c2 );
  QString fileName;
  int tmode = bgData.texture( fileName );
  // texture data
  myFileName->setText( fileName );
  myTextureMode->setCurrentIndex( tmode );
  // color / simple gradient data
  myFirstColor->setColor( c1 );
  mySecondColor->setColor( c2 );
  // gradient data
  // ... NOT IMPLEMENTED YET

  // set current index
  int idx = -1;
  for ( int i = 0; i < myModeCombo->count() && idx == -1; i++ ) {
    int im = myModeCombo->itemData( i, TypeRole ).toInt();
    // for simple gradient mode we also check gradient type
    if ( m == Qtx::SimpleGradientBackground && im == Qtx::SimpleGradientBackground ) {
      int it = myModeCombo->itemData( i, IdRole ).toInt();
      if ( it == gtype ) idx = i;
    }
    // for other modes we just check mode itself
    else if ( im == m ) idx = i;
  }
  myModeCombo->setCurrentIndex( idx );
}

/*!
  \brief Get allowed two-color gradients to the widget
  \param gradients gradients names are returned via this parameter
  \param ids gradients identifiers are returned via this parameter (empty list can be returned)
*/
void QtxBackgroundTool::gradients( QStringList& gradList, QIntList& idList ) const
{
  gradList = myGradients;
  idList   = myGradientsIds;
}

/*!
  \brief Set allowed two-color gradients to the widget
  \param gradients gradients names
  \param ids optional gradients identifiers; if not specified, gradients are automatically numbered starting from 0
*/
void QtxBackgroundTool::setGradients( const QStringList& gradList, const QIntList& idList )
{
  myGradients    = gradList;
  myGradientsIds = idList;
  myLastGradient = -1;
  Qtx::BackgroundData d = data(); // store current state
  internalUpdate();               // re-initialize
  setData( d );                   // restore current state (if possible)
}

/*!
  \brief Check if specific background mode is allowed
  \param mode background mode
  \return \c true if specified background mode is enabled or \c false otherwise
  \sa setModeAllowed()
*/
bool QtxBackgroundTool::isModeAllowed( Qtx::BackgroundMode mode ) const
{
  return myTypesAllowed.contains( mode ) ? myTypesAllowed[ mode ] : false;
}

/*!
  \brief Enable / disable specific background mode
  \param mode background mode
  \param on enable / disable flag
  \sa isModeAllowed()
*/
void QtxBackgroundTool::setModeAllowed( Qtx::BackgroundMode mode, bool on )
{
  if ( mode == Qtx::CustomGradientBackground )
    return; // NOT IMPLEMENTED YET //

  myTypesAllowed[ mode ] = on;
  Qtx::BackgroundData d = data(); // store current state
  internalUpdate();               // re-initialize
  setData( d );                   // restore current state (if possible)
}

/*!
  \brief Get allowed image formats
  \return image formats
  \sa setImageFormats()
*/
QString QtxBackgroundTool::imageFormats() const
{
  return myImageFormats;
}

/*!
  \brief Set allowed image formats
  \param formats image formats
  \sa imageFormats()
*/
void QtxBackgroundTool::setImageFormats( const QString& formats )
{
  myImageFormats = formats.isEmpty() ? tr( "Images Files (*.bmp *.png *.jpg *.jpeg *.gif *.tiff)" ) : formats;
}

/*!
  \brief Get widget editor orientation
  \return orientation
  \sa setOrientation()
*/
Qt::Orientation QtxBackgroundTool::orientation() const
{
  return qobject_cast<QVBoxLayout*>( layout() ) ? Qt::Vertical : Qt::Horizontal;
}

/*!
  \brief Change widget orientation
  \param orientation new widget orientation
  \sa orientation()
*/
void QtxBackgroundTool::setOrientation( Qt::Orientation orientation )
{
  QBoxLayout* l = (orientation == Qt::Horizontal) ? (QBoxLayout*)(new QHBoxLayout) : (QBoxLayout*)(new QVBoxLayout);
  l->setMargin( 0 );
  l->setSpacing( 5 );
  l->addWidget( myModeCombo );
  l->addWidget( myContainer );
  delete layout();
  setLayout( l );
}

/*!
  \brief Initialization: fill in the widget with items according to the 
  available modes
*/
void QtxBackgroundTool::internalUpdate()
{
  myModeCombo->clear();
  if ( isModeAllowed( Qtx::ImageBackground ) ) {
    myModeCombo->addItem( tr( "Image" ) );
    int idx = myModeCombo->count()-1;
    myModeCombo->setItemData( idx, (int)Qtx::ImageBackground, TypeRole );
  }
  if ( isModeAllowed( Qtx::ColorBackground ) ) {
    if ( myModeCombo->count() > 0 ) 
      myModeCombo->insertSeparator( myModeCombo->count() );
    myModeCombo->addItem( tr( "Single Color" ) );
    int idx = myModeCombo->count()-1;
    myModeCombo->setItemData( idx, (int)Qtx::ColorBackground, TypeRole );
  }
  if ( isModeAllowed( Qtx::SimpleGradientBackground ) ) {
    if ( myGradients.count() > 0 && myModeCombo->count() > 0 ) 
      myModeCombo->insertSeparator( myModeCombo->count() );
    for ( int i = 0; i < myGradients.count(); i++ ) {
      myModeCombo->addItem( myGradients[i] );
      int idx = myModeCombo->count()-1;
      myModeCombo->setItemData( idx, (int)Qtx::SimpleGradientBackground, TypeRole );
      myModeCombo->setItemData( idx, myGradientsIds.count() > i ? myGradientsIds[i] : i, IdRole );
    }
  }
  if ( isModeAllowed( Qtx::CustomGradientBackground ) ) {
    if ( myModeCombo->count() > 0 ) 
      myModeCombo->insertSeparator( myModeCombo->count() );
    myModeCombo->addItem( tr( "Custom" ) );
    int idx = myModeCombo->count()-1;
    myModeCombo->setItemData( idx, (int)Qtx::CustomGradientBackground, TypeRole );
  }
  updateState();
}

/*!
  \brief Update widget state
*/
void QtxBackgroundTool::updateState()
{
  int idx = myModeCombo->currentIndex();
  int id = -1;
  if ( idx >= 0 ) {
    id = myModeCombo->itemData( idx, TypeRole ).toInt();
    switch( id ) {
    case Qtx::ImageBackground:
      myContainer->setCurrentIndex( Image );
      break;
    case Qtx::ColorBackground:
    case Qtx::SimpleGradientBackground:
      myContainer->setCurrentIndex( Color );
      myLastGradient = myModeCombo->itemData( idx, IdRole ).toInt();
      break;
    case Qtx::CustomGradientBackground:
      myContainer->setCurrentIndex( Gradient );
      break;
    }
  }
  myModeCombo->setEnabled( idx >= 0 );
  myContainer->setEnabled( idx >= 0 );
  mySecondColor->setEnabled( id == Qtx::SimpleGradientBackground );
}

/*!
  \brief Called when "Browse..." button is pressed
*/
void QtxBackgroundTool::browse()
{
  QString fileName = QFileDialog::getOpenFileName( this, tr( "Select Picture" ), myFileName->text().trimmed(), myImageFormats );
  if ( !fileName.isEmpty() )
    myFileName->setText( fileName );
}

/*!
  \class QtxBackgroundDialog
  \brief Dialog box that can be used to set-up the background data

  Usage example:
  \code
  // create dialog box
  QtxBackgroundDialog dlg( this );
  // assign gradients types
  QStringList sl;
  sl << "Horizontal" << "Vertical" << "First diagonal" << "Second diagonal";
  dlg.setGradients(sl);
  // disable image texture and custom gradient modes
  dlg.setModeAllowed(Qtx::ImageBackground, false);
  dlg.setModeAllowed(Qtx::CustomGradientBackground, false);
  // initialize dialog box with the some background data
  dlg.setData(backgroundData);
  // execute dialog box and obtain result background data
  if ( dlg.exec() ) {
    Qtx::BackgroundData bgData = dlg.getBackgroundData();
  }
  \endcode

  Also it is possible to use static function:
  \code
  Qtx::BackgroundData bgData = QtxBackgroundDialog::getBackground( this );
  if ( bgData.isValid() ) 
    doSomething( bgData );
  \endcode
*/

/*!
  \brief Constructor
  \param parent parent widget
*/
QtxBackgroundDialog::QtxBackgroundDialog( QWidget* parent )
  : QtxDialog( parent, true, true, OK | Cancel )
{
  init();
}

/*!
  \brief Constructor
  \param bgData initial background data
  \param parent parent widget
*/
QtxBackgroundDialog::QtxBackgroundDialog( const Qtx::BackgroundData& bgData, QWidget* parent )
  : QtxDialog( parent, true, true, OK | Cancel )
{
  init();
}

/*!
  \brief Destructor
*/
QtxBackgroundDialog::~QtxBackgroundDialog()
{
}

/*!
  \brief Perform internal initialization
*/
void QtxBackgroundDialog::init()
{
  // title
  setWindowTitle( tr( "Change background" ) );
  // flags
  setDialogFlags( SetFocus );
  // move "Cancel" button to the right
  setButtonPosition( Right, Cancel );

  // main layout
  QVBoxLayout* main = new QVBoxLayout( mainFrame() );
  main->setMargin( 0 );
  main->setSpacing( 5 );

  // background widget
  myTool = new QtxBackgroundTool( Qt::Vertical, mainFrame() );
  main->addWidget( myTool );
}

/*!
  \brief Set background data
  \param bgData background data
*/
void QtxBackgroundDialog::setData( const Qtx::BackgroundData& bgData )
{
  myTool->setData( bgData );
}

/*!
  \brief Get background data
  \return background data
*/
Qtx::BackgroundData QtxBackgroundDialog::data() const
{
  return myTool->data();
}

/*!
  \brief Set allowed two-color gradients to the widget
  \param gradients gradients names
  \param ids optional gradients identifiers; if not specified, gradients are automatically numbered starting from 0
*/
void QtxBackgroundDialog::setGradients( const QStringList& gradList, const QIntList& idList )
{
  myTool->setGradients( gradList, idList );
}

/*!
  \brief Enable / disable specific background mode
  \param mode background mode
  \param on enable / disable flag
*/
void QtxBackgroundDialog::setModeAllowed( Qtx::BackgroundMode mode, bool on )
{
  myTool->setModeAllowed( mode, on );
}

/*!
  \brief Set allowed image formats
  \param formats image formats
*/
void QtxBackgroundDialog::setImageFormats( const QString& formats )
{
  myTool->setImageFormats( formats );
}

/*!
  \brief This is a convenience static function that returns an background data selected by the user.
  If the user presses Cancel, it returns an invalid background data.
*/
Qtx::BackgroundData QtxBackgroundDialog::getBackground( QWidget* parent,
							const Qtx::BackgroundData& bgData,
							bool enableSolidColor,
							bool enableTexture,
							bool enableGradient,
							bool enableCustom,
							const QStringList& gradList,
							const QIntList& idList,
							const QString& formats )
{
  QtxBackgroundDialog dlg( parent );
  dlg.setModeAllowed( Qtx::ImageBackground,          enableTexture );
  dlg.setModeAllowed( Qtx::ColorBackground,          enableSolidColor );
  dlg.setModeAllowed( Qtx::SimpleGradientBackground, enableGradient );
  dlg.setModeAllowed( Qtx::CustomGradientBackground, enableCustom );
  dlg.setGradients( gradList, idList );
  dlg.setImageFormats( formats );
  dlg.setData( bgData );
  Qtx::BackgroundData res;
  int rc = dlg.exec();
  if ( rc ) res = dlg.data();
  return res;
}
