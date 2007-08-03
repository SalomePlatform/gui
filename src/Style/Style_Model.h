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
// File:      Style_Model.h
// Author:    Natalia Ermolaeva

#ifndef STYLE_MODEL_HEADER
#define STYLE_MODEL_HEADER

#include <Style.h>

#include <QMap>
#include <QString>
#include <QVariant>
#include <QStringList>

class QtxResourceMgr;
class QApplication;
class QColor;
class QFont;

class STYLE_SALOME_EXPORT Style_Model
{
public:
  Style_Model();
  virtual ~Style_Model();

  static QString sectionName() { return "Theme"; }

  typedef enum { grp_style, grp_color, grp_col_values, grp_lines, grp_font, grp_value } Groups;

  typedef enum { // grp_style
                 is_defined_style, defined_style, 
                 // grp colors
                 bg_clr, pal_base_clr, header_clr,
                 pal_text_clr, pal_btext_clr, pal_wtext_clr,
                 slider_clr, button_clr, prbar_clr,
                 checked_clr, pointer_clr, tbl_grline_clr,
                 ttip_is_change, ttip_bg_clr, ttip_text_clr,
                 highlight_wdg, high_wdg_clr, high_brd_wdg_clr,
                 pal_high_clr, pal_high_text_clr,
                 // grp_col_values
                 fld_light_clr, fld_dark_clr, pal_light_clr, pal_dark_clr,
                 border_top_clr, border_bot_clr, border_tab_top_clr, border_tab_bot_clr,
                 // grp_lines 
                 is_lines, lines_type, lines_clr,
                 // grp_font
                 font_value,
                 // grp_values
                 all_antialized, auto_raising_wdg,
                 edit_rad, btn_rad, slider_rad, slider_increase,
                 hor_handle_delta, ver_handle_delta, split_handle_len,
                 dock_wdg_sep_extent } Properties;
  typedef enum { None, Bool, Color, String, IntSpin, DblSpin, Selector, Font } PropType;

  void           reset();
  void           setDefaults( QApplication* app );
  void           initFromResource( QtxResourceMgr* theResMgr );
  bool           updateFromResource( QtxResourceMgr* theResMgr, QString thePropName );

  void           getValueTo( QtxResourceMgr* theResMgr, int theId, bool theIsDef );
  void           setValueFrom( QtxResourceMgr* theResMgr, int theId );

  void           setPredefinedStyle( int theType );

  double         getDblValue( int theId, const bool theIsDef = false ) const;
  int            getIntValue( int theId, const bool theIsDef = false ) const;
  bool           getBoolValue( int theId, const bool theIsDef = false ) const;
  QColor         getColorValue( int theId, const bool theIsDef = false ) const;
  QString        getStringValue( int theId, const bool theIsDef = false ) const;
  QFont          getFontValue( int theId, const bool theIsDef = false ) const;

  QList<int>     getGroups();
  QString        getGroupTitle( int theId );
  int            getGroupNbColumns( int theId );
  QList<int>     getGroupProps( int theId );
  QString        getPropTitle( int theId );
  PropType       getPropType( int theId );
  QString        getPropName( int theId );
  void           getSelector( int theId, QStringList& theLst, QList<QVariant>& theIds );

private:
  void           setDefValue( Properties theId, QVariant theValue );
  void           setValue( Properties theId, QVariant theValue );

  void           fillValue( Properties theId, QString theName, QString theTitle,
                            Groups theGroupId, PropType theType = Color ); 
  void           fillGroup( Groups theId, QString theTitle, int theNbCols );

private:
  typedef struct {
    QString    myName;
    QString    myTitle;
    Groups     myGroupId;
    PropType   myType;
    QVariant   myValue;
    QVariant   myDefValue;
  } StyleValue;

  typedef struct {
    QString   myTitle;
    int       myNbColumns;
  } GroupValue;

  typedef QMap<int, StyleValue> ValuesMap;
  typedef QMap<int, GroupValue> StringMap;
  ValuesMap    myValues;
  StringMap    myGroups;

  QStringList  myPStyles;
  QStringList  myLines;
};

#endif
