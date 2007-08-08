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
// File   : Style_Model.cxx
// Author : Natalia Ermolaeva, Open CASCADE S.A.S.
//

#include "Style_Model.h"
#include <QtxResourceMgr.h>

#include <QColor>
#include <QApplication>
#include <QPalette>
#include <QFont>

Style_Model::Style_Model()
{
  // grp_style group
  fillValue( is_defined_style, "is-defined-style", "Predefined Style", grp_style, Bool );
  fillValue( defined_style,    "defined-style",    "Style:", grp_style, Selector );
  myPStyles<<"Default"<<"Steel-blue"<<"Dark-green";
  // grp_colors group
  fillValue( bg_clr,         "bg-color",        "Window", grp_color );
  fillValue( pal_base_clr,   "base-color",      "Base", grp_color );
  fillValue( header_clr,     "header-color",    "Header", grp_color );

  fillValue( pal_text_clr,   "text-color",      "Text", grp_color );
  fillValue( pal_btext_clr,  "text-button-color", "Text button", grp_color );
  fillValue( pal_wtext_clr,  "text-window-color", "Text window", grp_color );

  fillValue( slider_clr,     "slider-color",    "Slider", grp_color );
  fillValue( button_clr,     "button-color",    "Button", grp_color );
  fillValue( prbar_clr,      "progress-bar-color", "Progress bar", grp_color );

  fillValue( checked_clr,    "checked-color",    "Checked", grp_color );
  fillValue( pointer_clr,    "pointer-color",   "Pointer", grp_color );
  fillValue( tbl_grline_clr, "table-grid-color", "Table grid", grp_color );

  fillValue( ttip_is_change, "ttip-is-change", "Change ToolTip", grp_color, Bool );
  fillValue( ttip_bg_clr,    "ttip-bg-color",   "Background", grp_color );
  fillValue( ttip_text_clr,  "ttip-text-color", "Text color", grp_color );

  fillValue( highlight_wdg, "is-highlight-widget", "Highlight widget", grp_color, Bool );
  fillValue( high_wdg_clr,   "highlight-widget-color", "Widget center", grp_color );
  fillValue( high_brd_wdg_clr, "highligh-brd-widget-color", "Widget border", grp_color );

  fillValue( pal_high_clr,   "highlight-color", "Highlight", grp_color );
  fillValue( pal_high_text_clr, "highlight-text-color", "Highlight text", grp_color );
  // grp_col_values group
  fillValue( fld_light_clr,  "fld-light-color", "Field light", grp_col_values );
  fillValue( fld_dark_clr,   "fld-dark-color",  "Field dark", grp_col_values );
  fillValue( pal_light_clr,  "light-color",     "Light", grp_col_values );
  fillValue( pal_dark_clr,   "dark-color",      "Dark", grp_col_values );

  fillValue( border_top_clr, "brd-top-color",   "Border top", grp_col_values );
  fillValue( border_bot_clr, "brd-bottom-color", "Border bottom", grp_col_values );
  fillValue( border_tab_top_clr, "brd-tab-top-color", "Border widget top", grp_col_values );
  fillValue( border_tab_bot_clr, "brd-tab-bottom-color", "Border widget bottom", grp_col_values );

  // lines group
  fillValue( is_lines,       "is-lines",     "Draw lines", grp_lines, Bool );
  fillValue( lines_type,     "lines-type",   "Type",       grp_lines, Selector );
  myLines<<"Horizontal"<<"Incline";
  fillValue( lines_clr,     "lines-color",   "Color",      grp_lines, Color );

  // grp_font group
  fillValue( font_value,    "app-font", "App Font", grp_font, Font );
  // grp_values group
  fillValue( all_antialized, "all-antialized", "All borders antialized", grp_value, Bool );
  fillValue( auto_raising_wdg, "is-raising-widget", "Auto raising widget", grp_value, Bool );
  fillValue( edit_rad,      "edit-rad", "Rounding of edit", grp_value, DblSpin );
  fillValue( btn_rad,       "button-rad", "Rounding of button", grp_value, DblSpin );
  fillValue( slider_rad,    "slider-rad", "Rounding of slider", grp_value, DblSpin );
  fillValue( hor_handle_delta, "hor-hadle-delta", "Hor spacinig of handle ", grp_value, DblSpin );
  fillValue( ver_handle_delta, "ver-handle-delta", "Ver spacing of handle", grp_value, DblSpin );
  fillValue( split_handle_len, "split-handle-len", "Lenght of splitter handle", grp_value, DblSpin );
  fillValue( dock_wdg_sep_extent,  "dock-widget-sep-extent", "DockWidget separator extent", grp_value, DblSpin );

  fillGroup( grp_style, "Predefined style", 2 );
  fillGroup( grp_color, "Colors", 3 );
  fillGroup( grp_col_values, "ColorValues", 2 );
  fillGroup( grp_lines, "Lines", 3 );
  fillGroup( grp_font, "Application font", 1 );
  fillGroup( grp_value, "Values", 2 );
}

Style_Model::~Style_Model()
{
}

QString Style_Model::sectionName()
{
  return "Theme"; 
}

void Style_Model::reset()
{
  ValuesMap::iterator anIt = myValues.begin(), anEnd = myValues.end();
  int anId;
  for ( ; anIt != anEnd; ++anIt ) {
    anId = anIt.key();
    myValues[anId].myValue = myValues[anId].myDefValue;
  }
}

void Style_Model::setDefaults( QApplication* app )
{
  setDefValue( is_defined_style, QVariant( true ) );
  setDefValue( defined_style,    QVariant( 0 ) );

  QPalette aPal = app->palette();
  setDefValue( bg_clr,         QVariant( aPal.color( QPalette::Window ) ) );
  setDefValue( pal_base_clr,   aPal.color ( QPalette::Base ) );
  setDefValue( header_clr,     QVariant( aPal.color( QPalette::Button ) ) );
  setDefValue( pal_text_clr,   aPal.color( QPalette::Text ) );
  setDefValue( pal_btext_clr,  aPal.color( QPalette::ButtonText ) );
  setDefValue( pal_wtext_clr,  aPal.color( QPalette::WindowText ) );

  setDefValue( slider_clr,     QVariant( aPal.color( QPalette::Button ) ) );
  setDefValue( button_clr,     QVariant( aPal.color( QPalette::Button ) ) );
  setDefValue( prbar_clr,       QVariant( aPal.color( QPalette::Highlight ) ) );

  setDefValue( checked_clr,    QVariant( aPal.color( QPalette::Base ) ) );
  setDefValue( pointer_clr,     QVariant( QColor( Qt::black ) ) );
  setDefValue( tbl_grline_clr,  QVariant( aPal.color( QPalette::Mid ) ) );

  setDefValue( ttip_is_change,  QVariant( false ) );
  setDefValue( ttip_bg_clr,     myValues[bg_clr].myValue );
  setDefValue( ttip_text_clr,   myValues[pal_text_clr].myValue );

  setDefValue( highlight_wdg,  QVariant( false ) );
  setDefValue( high_wdg_clr,   myValues[button_clr].myValue );
  setDefValue( high_brd_wdg_clr, myValues[high_wdg_clr].myValue );

  setDefValue( pal_high_clr,   aPal.color( QPalette::Highlight ) );
  setDefValue( pal_high_text_clr, aPal.color( QPalette::HighlightedText ) );

  setDefValue( fld_light_clr,   QVariant( aPal.color( QPalette::Light ) ) );
  setDefValue( fld_dark_clr,    QVariant( aPal.color( QPalette::Mid ).light( 125 ) ) );
  setDefValue( pal_light_clr,  aPal.color( QPalette::Light ) );
  setDefValue( pal_dark_clr,   aPal.color( QPalette::Dark ) );

  QColor aDark = aPal.color( QPalette::Dark );
  setDefValue( border_top_clr, QVariant( aDark.light() ) );
  setDefValue( border_bot_clr, QVariant( aDark.dark() ) );
  setDefValue( border_tab_top_clr, QVariant( aDark.light().light().light() ) );
  setDefValue( border_tab_bot_clr, QVariant( aDark.dark().dark() ) );

  setDefValue( is_lines,       QVariant( false ) );
  setDefValue( lines_type,     QVariant( 0 ) );
  setDefValue( lines_clr,      QVariant( aPal.color( QPalette::Mid ) ) );

  // not color settings
  QFont aFont = app->font();
  setDefValue( font_value,      QVariant( aFont ) );

  setDefValue( slider_rad,     QVariant( 0 ) );
  setDefValue( edit_rad,       QVariant( 0 ) );
  setDefValue( btn_rad,        QVariant( 0 ) );
  setDefValue( slider_increase,QVariant( 2 ) );
  setDefValue( all_antialized, QVariant( false ) );
  setDefValue( highlight_wdg,   QVariant( false ) );
  setDefValue( auto_raising_wdg, QVariant( false ) );
  setDefValue( hor_handle_delta, QVariant( 3 ) );
  setDefValue( ver_handle_delta, QVariant( 3 ) );
  setDefValue( split_handle_len, QVariant( 20 ) );
  setDefValue( dock_wdg_sep_extent,  QVariant( 8 ) );
}

void Style_Model::initFromResource( QtxResourceMgr* theResMgr )
{
  ValuesMap::iterator anIt = myValues.begin(), anEnd = myValues.end();
  for ( ; anIt != anEnd; ++anIt )
    setValueFrom( theResMgr, anIt.key() );

  if ( getBoolValue( is_defined_style ) )
    setPredefinedStyle( getIntValue( defined_style ) );
}

bool Style_Model::updateFromResource( QtxResourceMgr* theResMgr, const QString& thePropName )
{
  bool retrieve = false;
  if ( !theResMgr )
    return retrieve;
  ValuesMap::iterator anIt = myValues.begin(), anEnd = myValues.end();
  int anId = -1;
  for ( ; anIt != anEnd && anId == -1; ++anIt )
    if ( anIt.value().myName == thePropName )
      anId = anIt.key();
  if ( anId == -1 )
    return retrieve;
  setValueFrom( theResMgr, anId );

  if ( ( anId == is_defined_style || anId == defined_style )
        && getBoolValue( is_defined_style ) ) {
    setPredefinedStyle( getIntValue( defined_style ) );
    retrieve = true;
    // update for resources all datas
    for ( anIt = myValues.begin(); anIt != anEnd; ++anIt ) {
      if ( anIt.key() != anId )
        getValueTo( theResMgr, anIt.key(), false );
    }
  }
  return retrieve;
}

void Style_Model::getValueTo( QtxResourceMgr* theResMgr, int theId, bool theIsDef )
{
  if ( !theResMgr )
    return;
  QString aSection = sectionName(),
          aPropName = getPropName( theId );
  switch( getPropType( theId ) ) {
    case Bool: {
      bool aValue;
      if ( !theIsDef || !theResMgr->value( aSection, aPropName, aValue ) )
        theResMgr->setValue( aSection, aPropName, getBoolValue( theId, theIsDef ) );
      break;
    }
    case Color: {
      QColor aColor;
      if ( !theIsDef || !theResMgr->value( aSection, aPropName, aColor ) )
        theResMgr->setValue( aSection, aPropName, getColorValue( theId, theIsDef ) );
      break; 
    }
    case String: {
      QString aStr;
      if ( !theIsDef || !theResMgr->value( aSection, aPropName, aStr ) )
        theResMgr->setValue( aSection, aPropName, getStringValue( theId, theIsDef ) );
      break;
    }
    case IntSpin: {
      int aValue;
      if ( !theIsDef || !theResMgr->value( aSection, aPropName, aValue ) )
        theResMgr->setValue( aSection, aPropName, getIntValue( theId, theIsDef ) );
      break;
    }
    case DblSpin: {
      double aValue;
      if ( !theIsDef || !theResMgr->value( aSection, aPropName, aValue ) )
        theResMgr->setValue( aSection, aPropName, getDblValue( theId, theIsDef ) );
      break;
    }
    case Selector: {
      int aValue;
      if ( !theIsDef || !theResMgr->value( aSection, aPropName, aValue ) )
        theResMgr->setValue( aSection, aPropName, getIntValue( theId, theIsDef ) );
      break;
    }
  case Font: {
      QFont aFont;
      if ( !theIsDef || !theResMgr->value( aSection, aPropName, aFont ) )
        theResMgr->setValue( aSection, aPropName, getFontValue( theId, theIsDef ) );
      break;
    }
    default: break;
  }
}

void Style_Model::setValueFrom( QtxResourceMgr* theResMgr, int theId )
{
  if ( !theResMgr )
    return;
  QString aSection = sectionName(),
          aPropName = getPropName( theId );
  QVariant aValue = myValues[theId].myDefValue;
  switch( getPropType( theId ) ) {
    case Bool:
      aValue = theResMgr->booleanValue( aSection, aPropName, getBoolValue( theId, true ) );
    break;
    case Color:
      aValue = theResMgr->colorValue( aSection, aPropName, getColorValue( theId , true ) );
    break; 
    case String:
      aValue = theResMgr->stringValue( aSection, aPropName, getStringValue( theId, true ) );
    break;
    case IntSpin:
      aValue = theResMgr->integerValue( aSection, aPropName, getIntValue( theId, true ) );
    break;
    case DblSpin:
      aValue = theResMgr->doubleValue( aSection, aPropName, getDblValue( theId, true ) );
    break;
    case Selector:
      aValue = theResMgr->integerValue( aSection, aPropName, getIntValue( theId, true ) );
    break;
    case Font:
      aValue = theResMgr->fontValue( aSection, aPropName, getFontValue( theId, true ) );
    break;
    default: break;
  }
  myValues[theId].myValue = aValue;
}

void Style_Model::setPredefinedStyle( int theType )
{
  reset();
  switch ( theType ) {
    case 0: {
      setValue( slider_rad, QVariant( 3 ) );
      setValue( slider_increase,  QVariant( 5 ) );
      setValue( auto_raising_wdg, QVariant( true ) );
      break;
    }
    case 1: {
      //light blue variant
      setValue( bg_clr,        QVariant( QColor( "lightsteelblue" ) ) );
      setValue( lines_type,  QVariant( 0 ) );
      setValue( lines_clr, QVariant( QColor( "blue" ) ) );
      setValue( checked_clr,   QVariant( QColor( "cornflowerblue" ) ) );
      setValue( header_clr,    QVariant( QColor( "silver" ) ) );
      setValue( slider_clr,    QVariant( QColor( "cornflowerblue" ) ) );
      setValue( button_clr,    QVariant( QColor( "silver" ) ) );
      setValue( pointer_clr,   QVariant( QColor( "midnightblue" ) ) );
      setValue( fld_light_clr, QVariant( QColor( "whitesmoke" ) ) );
      setValue( fld_dark_clr,  QVariant( QColor( "grey" ).light( 145 ) ) );
      setValue( prbar_clr,     QVariant( QColor ("cornflowerblue" ) ) );
      setValue( slider_rad,    QVariant( 3 ) );
      setValue( edit_rad,      QVariant( 6 ) );
      setValue( btn_rad,       QVariant( 10 ) );
      setValue( slider_increase, QVariant( 5 ) );
      setValue( all_antialized, QVariant( false ) ); // true

      setValue( highlight_wdg, QVariant( true ) );
      setValue( high_wdg_clr,  QVariant("skyblue") );
      setValue( high_brd_wdg_clr, QVariant("cornflowerblue") );
      setValue( pal_base_clr,  QVariant( QColor( "white" ) ) );
      //setValue( auto_raising_wdg, QVariant( true ) );
      break;
    }
    case 2: {
      QColor aDarkGreen  = QColor( "darkgreen" ).dark(195);
      setValue( bg_clr,        QVariant( QColor( "black" ) ) );
      setValue( lines_type,  QVariant( 0 ) );
      setValue( checked_clr,   QVariant( QColor( "black" ) ) );
      setValue( header_clr,    QVariant( aDarkGreen ) );
      setValue( slider_clr,    QVariant( QColor( "goldenrod" ) ) );
      setValue( button_clr,    QVariant( aDarkGreen ) );

      setValue( border_top_clr, QVariant( "goldenrod" ) );
      setValue( border_bot_clr, QVariant( "saddlebrown" ) );
      setValue( border_tab_top_clr, QVariant( "goldenrod" ) );
      setValue( border_tab_bot_clr, QVariant( "saddlebrown" ) );

      setValue( pointer_clr,   QVariant( QColor( "gold" ) ) );
      setValue( fld_light_clr, QVariant( QColor( "green" ) ) );
      setValue( fld_dark_clr,  QVariant( aDarkGreen ) );
      setValue( prbar_clr,     QVariant( QColor( "goldenrod" ) ) );
      setValue( tbl_grline_clr,QVariant( "saddlebrown" ) );
      setValue( ttip_is_change,QVariant( true ) );
      setValue( ttip_bg_clr,   QVariant( "green" ) );

      setValue( pal_text_clr,  QVariant( QColor( "white" ) ) );
      setValue( pal_btext_clr, QVariant( QColor( "gold" ) ) );
      setValue( pal_wtext_clr, QVariant( QColor( "gold" ) ) );
      setValue( pal_base_clr,  QVariant( QColor( "black" ) ) );
      setValue( pal_high_clr,  QVariant( QColor( "gold" ) ) );
      setValue( pal_high_text_clr, QVariant( QColor( "goldenrod" ) ) );
      setValue( pal_light_clr, QVariant( QColor( "goldenrod" ) ) );
      setValue( pal_dark_clr,  QVariant( QColor( "saddlebrown" ) ) );

      setValue( slider_rad,    QVariant( 3 ) );
      setValue( edit_rad,      QVariant( 6 ) );
      setValue( btn_rad,       QVariant( 10 ) );
      setValue( slider_increase, QVariant( 6 ) );
      setValue( all_antialized, QVariant( false ) );

      setValue( highlight_wdg, QVariant( true ) );
      setValue( high_wdg_clr,  QVariant("green") );
      setValue( high_brd_wdg_clr, QVariant( aDarkGreen ) );
      break;
    }
    default:
      return;
  }
}

double Style_Model::getDblValue( int theId, const bool theIsDef ) const
{
  double aRes = 0;
  QVariant aValue;
  if ( myValues.contains(theId) && myValues[theId].myType == DblSpin ) {
    if ( theIsDef )
      aValue =  myValues[theId].myDefValue;
    else
      aValue =  myValues[theId].myValue;
    if ( aValue.canConvert( QVariant::Double ) )
      aRes = myValues[theId].myValue.toDouble();
  }
  return aRes;
}

int Style_Model::getIntValue( int theId, const bool theIsDef ) const
{
  int aRes = 0;
  QVariant aValue;
  if ( myValues.contains(theId) && ( myValues[theId].myType == IntSpin
                                     || myValues[theId].myType == Selector ) ) {
    if ( theIsDef )
      aValue =  myValues[theId].myDefValue;
    else
      aValue =  myValues[theId].myValue;
    if ( aValue.canConvert( QVariant::Int ) )
      aRes = myValues[theId].myValue.toInt();
  }
  return aRes;
}

bool Style_Model::getBoolValue( int theId, const bool theIsDef ) const
{
  bool aRes = false;
  QVariant aValue;
  if ( myValues.contains(theId) && myValues[theId].myType == Bool ) {
    if ( theIsDef )
      aValue =  myValues[theId].myDefValue;
    else
      aValue =  myValues[theId].myValue;
    if ( aValue.canConvert( QVariant::Bool ) )
      aRes = myValues[theId].myValue.toBool();
  }
  return aRes;
}

QColor Style_Model::getColorValue( int theId, const bool theIsDef ) const
{
  QColor aColor = Qt::gray;
  QVariant aValue;
  if ( myValues.contains(theId) && myValues[theId].myType == Color ) {
    if ( theIsDef )
      aValue =  myValues[theId].myDefValue;
    else
      aValue =  myValues[theId].myValue;

   if ( aValue.canConvert( QVariant::Color ) )
     aColor = myValues[theId].myValue.value<QColor>();
  }
  return aColor;
}

QString Style_Model::getStringValue( int theId, const bool theIsDef ) const
{
  QString aRes = false;
  QVariant aValue;
  if ( myValues.contains(theId) && myValues[theId].myType == String ) {
    if ( theIsDef )
      aValue =  myValues[theId].myDefValue;
    else
      aValue =  myValues[theId].myValue;
    if ( aValue.canConvert( QVariant::String ) )
      aRes = myValues[theId].myValue.toString();
  }
  return aRes;
}

QFont Style_Model::getFontValue( int theId, const bool theIsDef ) const
{
  QFont aFont;
  QVariant aValue;
  if ( myValues.contains(theId) && myValues[theId].myType == Font ) {
    if ( theIsDef )
      aValue =  myValues[theId].myDefValue;
    else
      aValue =  myValues[theId].myValue;
    if ( aValue.canConvert( QVariant::Font ) )
      aFont = myValues[theId].myValue.value<QFont>();
  }
  return aFont;
}

QList<int> Style_Model::getGroups() const
{
  return myGroups.keys();
}

QString Style_Model::getGroupTitle( int theId ) const
{
  return myGroups.contains( theId ) ? myGroups[theId].myTitle : "";
}

int Style_Model::getGroupNbColumns( int theId ) const
{
  return myGroups.contains( theId ) ? myGroups[theId].myNbColumns : 1;
}

QList<int> Style_Model::getGroupProps( int theId ) const
{
  QList<int> values;
  ValuesMap::const_iterator anIt = myValues.begin(), anEnd = myValues.end();
  for ( ; anIt != anEnd; ++anIt )
    if ( anIt.value().myGroupId == theId )
      values.append( anIt.key() );
  return values;

}

QString Style_Model::getPropTitle( int theId ) const
{
  return myValues.contains( theId ) ? myValues[theId].myTitle : ""; 
}

Style_Model::PropType Style_Model::getPropType( int theId ) const
{
  return myValues.contains( theId ) ? myValues[theId].myType : None; 
}

QString Style_Model::getPropName( int theId ) const
{
  return myValues.contains( theId ) ? myValues[theId].myName : ""; 
}

void Style_Model::getSelector( int theId, QStringList& theLst, QList<QVariant>& theIds ) const
{
  theLst.clear();
  theIds.clear();
  switch ( theId ) {
    case defined_style: theLst = myPStyles; break;
    case lines_type:    theLst = myLines; break;
    default: break;
  }
  QStringList::iterator anIt = theLst.begin(), anEnd = theLst.end();
  int i = 0;
  for ( ; anIt != anEnd; ++anIt )
    theIds.append( QVariant( i++ ) );
}

void Style_Model::setDefValue( Properties theId, const QVariant& theValue )
{
  myValues[theId].myDefValue = theValue;
  setValue( theId, theValue );
}

void Style_Model::setValue( Properties theId, const QVariant& theValue )
{
  myValues[theId].myValue = theValue;
}

void Style_Model::fillValue( Properties theId, const QString& theName,
			     const QString& theTitle, Groups theGroupId, PropType theType )
{
  StyleValue aValue;
  aValue.myName = theName;
  aValue.myTitle = theTitle;
  aValue.myGroupId = theGroupId;
  aValue.myType = theType;
  aValue.myValue = QVariant();

  myValues[theId] = aValue;
}

void Style_Model::fillGroup( Groups theId, const QString& theTitle, int theNbCols )
{
  GroupValue aValue;
  aValue.myTitle = theTitle;
  aValue.myNbColumns = theNbCols;

  myGroups[theId] = aValue;
}

