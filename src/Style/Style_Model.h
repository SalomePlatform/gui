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
// File   : Style_Model.h
// Author : Natalia Ermolaeva, Open CASCADE S.A.S.
//

#ifndef STYLE_MODEL_H
#define STYLE_MODEL_H

#include "Style.h"

#include <QMap>
#include <QString>
#include <QVariant>
#include <QStringList>

class QtxResourceMgr;
class QtxPagePrefMgr;
class QApplication;
class QColor;
class QFont;

class STYLE_SALOME_EXPORT Style_Model
{
public:
  Style_Model();
  virtual ~Style_Model();

  static QString sectionName();

  typedef enum { tab_value, tab_color } Tabs;
  typedef enum { grp_style, grp_color, grp_col_values, grp_lines, grp_font, grp_value } Groups;
  typedef enum { None, Bool, Color, String, IntSpin, DblSpin, Selector, Font } PropType;
  typedef enum { // grp_style
                 defined_style, 
                 // grp colors
                 bg_clr, pal_base_clr, header_clr,
                 pal_text_clr, pal_btext_clr, pal_wtext_clr,
                 slider_clr, button_clr, prbar_clr,
                 checked_clr, pointer_clr, tbl_grline_clr,
                 ttip_is_change, ttip_bg_clr, ttip_text_clr,
                 highlight_wdg, high_wdg_clr, high_brd_wdg_clr,
                 auto_raising_wdg, pal_high_clr, pal_high_text_clr,
                 // grp_col_values
                 fld_light_clr, fld_dark_clr, pal_light_clr, pal_dark_clr,
                 border_top_clr, border_bot_clr, border_tab_top_clr, border_tab_bot_clr,
                 // grp_lines 
                 is_lines, lines_type, lines_clr, lines_transp,
                 // grp_font
                 font_value,
                 // grp_values
                 edit_rad, btn_rad, frame_rad, slider_rad,
                 hor_handle_delta, ver_handle_delta,
                 split_handle_len, slider_increase, all_antialized } Properties;

  void           reset();
  void           setDefaults( QApplication* );
  void           initFromResource( QtxResourceMgr* );
  void           updateFromResource( QtxPagePrefMgr*, const QString& );
  void           updateFromResource( QtxPagePrefMgr* );

  void           fillPreferences( QtxPagePrefMgr*, const int );

  void           getValueTo( QtxResourceMgr*, int, bool );
  void           setValueFrom( QtxResourceMgr*, int );

  void           setPredefinedStyle( int );

  double         getDblValue( int, const bool = false ) const;
  int            getIntValue( int, const bool = false ) const;
  bool           getBoolValue( int, const bool = false ) const;
  QColor         getColorValue( int, const bool = false ) const;
  QString        getStringValue( int, const bool = false ) const;
  QFont          getFontValue( int, const bool = false ) const;

  QList<int>     getTabs() const;
  QString        getTabTitle( int ) const;
  QList<int>     getGroups( int ) const;
  QString        getGroupTitle( int ) const;
  int            getGroupNbColumns( int ) const;
  QList<int>     getGroupProps( int ) const;
  QString        getPropTitle( int ) const;
  PropType       getPropType( int ) const;
  QString        getPropName( int ) const;
  void           getSelector( int, QStringList&, QList<QVariant>& ) const;

private:
  void           setDefValue( Properties, const QVariant& );
  void           setValue( Properties, const QVariant& );

  void           fillValue( Properties, const QString&, const QString&,
                            Groups, PropType = Color ); 
  void           fillGroup( Groups, Tabs, const QString&, int );

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
    Tabs      myTabType;
    QString   myTitle;
    int       myNbColumns;
  } GroupValue;

  typedef QMap<int, StyleValue> ValuesMap;
  typedef QMap<int, GroupValue> GroupMap;
  typedef QMap<int, QString> StringMap;
  ValuesMap   myValues;
  GroupMap    myGroups;
  StringMap   myTabs;

  QStringList myPStyles;
  QStringList myLines;
};

#endif // STYLE_MODEL_H
