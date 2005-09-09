
#ifndef OB_LISTITEM_H
#define OB_LISTITEM_H

#include "OB.h"

#include <qlistview.h>

class SUIT_DataObject;

/* 
  base template class for ListViewItems and CheckListItems
*/

template<class T> class ListItemF
{
public:
	ListItemF(T&, SUIT_DataObject* );
  /*ListItem( SUIT_DataObject*, QListView* );
  ListItem( SUIT_DataObject*, QListViewItem* );
  ListItem( SUIT_DataObject*, QListView*, QListViewItem* );
  ListItem( SUIT_DataObject*, QListViewItem*, QListViewItem* );

  ListItem( SUIT_DataObject*, QListView*, int );
  ListItem( SUIT_DataObject*, QListViewItem*, int );
  ListItem( SUIT_DataObject*, QListView*, QListViewItem*, int );
  ListItem( SUIT_DataObject*, QListViewItem*, QListViewItem*, int );*/

  void            setSel( bool s );
  inline SUIT_DataObject* dataObject() const { return myObject; }
  void            paintFoc( QPainter* p, const QColorGroup& cg, const QRect& r );
  void            paintC( QPainter* p, const QColorGroup& cg, int c, int w, int align );

protected:
  void                     update();

protected:
  SUIT_DataObject* myObject;
  T& myT;
};

/* 
   ListViewItem class
*/

class OB_EXPORT OB_ListItem : public ListItemF<QListViewItem>, public QListViewItem
{
public:
	OB_ListItem( SUIT_DataObject*, QListView* );
	OB_ListItem( SUIT_DataObject*, QListViewItem* );
	OB_ListItem( SUIT_DataObject*, QListView*, QListViewItem* );
	OB_ListItem( SUIT_DataObject*, QListViewItem*, QListViewItem* );

	virtual ~OB_ListItem();

  virtual void            setSelected( bool s );
  virtual void            paintFocus( QPainter* p, const QColorGroup& cg, const QRect& r );
  virtual void            paintCell( QPainter* p, const QColorGroup& cg, int c, int w, int align );


  virtual int      rtti() const;

  static int       RTTI();
};

/* 
   CheckListItem class
*/

class OB_EXPORT OB_CheckListItem : public ListItemF<QCheckListItem>, public QCheckListItem
{
public:
  OB_CheckListItem( SUIT_DataObject*, QListView*, Type = CheckBox );
  OB_CheckListItem( SUIT_DataObject*, QListViewItem*, Type = CheckBox );
  OB_CheckListItem( SUIT_DataObject*, QListView*, QListViewItem*, Type = CheckBox );
  OB_CheckListItem( SUIT_DataObject*, QListViewItem*, QListViewItem*, Type = CheckBox );

  virtual ~OB_CheckListItem();

  virtual void            setSelected( bool s );
  virtual void            paintFocus( QPainter* p, const QColorGroup& cg, const QRect& r );
  virtual void            paintCell( QPainter* p, const QColorGroup& cg, int c, int w, int align );


  virtual int      rtti() const;

  static int       RTTI();

protected:
  void             stateChange( bool );

//private:
//  void             update();
};

#endif
