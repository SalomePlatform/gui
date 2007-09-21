// File:      HTMLService_HTMLTable.cxx
// Created:   12.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#include <HTMLService_HTMLTable.hxx>

IMPLEMENT_STANDARD_HANDLE(HTMLService_HTMLTable, HTMLService_HTML)
IMPLEMENT_STANDARD_RTTIEXT(HTMLService_HTMLTable, HTMLService_HTML)

HTMLService_HTMLTable::HTMLService_HTMLTable( const Standard_Integer rows, const Standard_Integer cols )
: HTMLService_HTML(),
myWidth( -1 ),
myBorder( -1 ),
mySpacing( -1 ),
myPadding( -1 ),
myUseBC( Standard_False ),
myPercent( Standard_False )
{
  Initialize( rows, cols );
}

HTMLService_HTMLTable::HTMLService_HTMLTable( const Handle(HTMLService_HTML)& parent,
                                        const Standard_Integer rows, const Standard_Integer cols )
: HTMLService_HTML( parent ),
myWidth( -1 ),
myBorder( -1 ),
mySpacing( -1 ),
myPadding( -1 ),
myUseBC( Standard_False ),
myPercent( Standard_False )
{
  Initialize( rows, cols );
}

HTMLService_HTMLTable::~HTMLService_HTMLTable()
{
}

Standard_Integer HTMLService_HTMLTable::GetNumRows() const
{
  if ( myRows.IsNull() )
    return 0;

  return myRows->Length();
}

Standard_Integer HTMLService_HTMLTable::GetNumCols() const
{
  if ( myRows.IsNull() )
    return 0;

  Handle(HTMLService_HTMLTableRow) row = Handle(HTMLService_HTMLTableRow)::DownCast(myRows->Value( myRows->Lower() ));
  if ( row.IsNull() )
    return 0;

  return row->GetSize();
}

Handle(HTMLService_HTMLTableRow) HTMLService_HTMLTable::GetRow( const Standard_Integer index ) const
{
  Handle(HTMLService_HTMLTableRow) row;
  if ( !myRows.IsNull() )
  {
    Standard_Integer idx = index + myRows->Lower();
    if ( idx >= myRows->Lower() && idx <= myRows->Upper() )
      row = Handle(HTMLService_HTMLTableRow)::DownCast(myRows->Value( idx ));
  }
  return row;
}

Handle(HTMLService_HTMLTableCell) HTMLService_HTMLTable::GetCell( const Standard_Integer r, const Standard_Integer c ) const
{
  Handle(HTMLService_HTMLTableCell) cell;
  Handle(HTMLService_HTMLTableRow) row = GetRow( r );
  if ( !row.IsNull() )
    cell = row->GetCell( c );
  return cell;
}

Standard_Integer HTMLService_HTMLTable::GetBorder() const
{
  return myBorder;
}

Standard_Integer HTMLService_HTMLTable::GetCellSpacing() const
{
  return mySpacing;
}

Standard_Integer HTMLService_HTMLTable::GetCellPadding() const
{
  return myPadding;
}

Quantity_Color HTMLService_HTMLTable::GetBackgroundColor() const
{
  return myBgColor;
}

Standard_Integer HTMLService_HTMLTable::GetWidth( Standard_Boolean* percent ) const
{
  if ( percent )
    *percent = myPercent;
  return myWidth;
}

void HTMLService_HTMLTable::SetBorder( const Standard_Integer val )
{
  myBorder = val;
}

void HTMLService_HTMLTable::SetCellSpacing( const Standard_Integer val )
{
  mySpacing = val;
}

void HTMLService_HTMLTable::SetCellPadding( const Standard_Integer val )
{
  myPadding = val;
}

void HTMLService_HTMLTable::SetBackgroundColor( const Quantity_Color& c )
{
  myBgColor = c;
  myUseBC = Standard_True;
}

void HTMLService_HTMLTable::SetWidth( const Standard_Integer width, const Standard_Boolean percent )
{
  myWidth = width;
  myPercent = percent;
}

void HTMLService_HTMLTable::SetColumnWidth( const Standard_Integer col,
                                         const Standard_Integer w, const Standard_Boolean percent )
{
  for ( Standard_Integer r = myRows->Lower(); r <= myRows->Upper(); r++ )
  {
    Handle(HTMLService_HTMLTableRow) row = Handle(HTMLService_HTMLTableRow)::DownCast(myRows->Value( r ));
    if ( !row.IsNull() )
    {
      Handle(HTMLService_HTMLTableCell) cell = row->GetCell( col );
      if ( !cell.IsNull() )
        cell->SetWidth( w, percent );
    }
  }
}

void HTMLService_HTMLTable::SetRowSpan( const Standard_Integer row, const Standard_Integer span )
{
  Handle(HTMLService_HTMLTableRow) aRow = GetRow( row );
  if ( !aRow.IsNull() )
  {
    for ( Standard_Integer i = 0; i < aRow->GetSize(); i++ )
      aRow->GetCell( i )->SetRowSpan( span );
  }
}

void HTMLService_HTMLTable::SetColumnSpan( const Standard_Integer col, const Standard_Integer span )
{
  for ( Standard_Integer r = myRows->Lower(); r <= myRows->Upper(); r++ )
  {
    Handle(HTMLService_HTMLTableRow) row = Handle(HTMLService_HTMLTableRow)::DownCast(myRows->Value( r ));
    if ( !row.IsNull() )
    {
      Handle(HTMLService_HTMLTableCell) cell = row->GetCell( col );
      if ( !cell.IsNull() )
        cell->SetColSpan( span );
    }
  }
}

void HTMLService_HTMLTable::Initialize( const Standard_Integer rows, const Standard_Integer cols )
{
  if ( rows > 0 )
    myRows = new TColStd_HArray1OfTransient( 1, rows );

  if ( myRows.IsNull() )
    return;

  for ( Standard_Integer i = myRows->Lower(); i <= myRows->Upper(); i++ )
  {
    Handle(HTMLService_HTMLTableRow) row = new HTMLService_HTMLTableRow( cols );
    myRows->SetValue( i, row );
    InsertSubItem( row );
  }
}

void HTMLService_HTMLTable::Generate( SequenceOfHTMLLine& aHTML ) const
{
  PrepareCells();

  TCollection_ExtendedString aLine( "<table" );
  Standard_Boolean percent = Standard_False;
  if ( GetWidth( &percent ) >= 0 )
  {
    aLine += TCollection_ExtendedString( " width=" ) + TCollection_ExtendedString( GetWidth() );
    if ( percent )
      aLine += TCollection_ExtendedString( "%" );
  }
  if ( GetBorder() >= 0 )
    aLine += TCollection_ExtendedString( " border=" ) + TCollection_ExtendedString( GetBorder() );
  if ( GetCellSpacing() > 1 )
    aLine += TCollection_ExtendedString( " cellspacing=" ) + TCollection_ExtendedString( GetCellSpacing() );
  if ( GetCellPadding() > 1 )
    aLine += TCollection_ExtendedString( " cellpadding=" ) + TCollection_ExtendedString( GetCellPadding() );

  if ( myUseBC )
    aLine += TCollection_ExtendedString( " bgcolor=" ) + ColorString( GetBackgroundColor() );

  aLine += TCollection_ExtendedString( ">" );
  aHTML.Append( aLine );
  SubItemsHTML( aHTML );

  aHTML.Append( TCollection_ExtendedString( "</table>" ) );
}

void HTMLService_HTMLTable::PrepareCells() const
{
  for ( Standard_Integer row = 0; row < GetNumRows(); row++ )
  {
    for ( Standard_Integer col = 0; col < GetNumCols(); col++ )
    {
      Handle(HTMLService_HTMLTableCell) cell = GetCell( row, col );
      if ( !cell.IsNull() )
        cell->SetVisible( Standard_True );
    }
  }

  for ( Standard_Integer r = 0; r < GetNumRows(); r++ )
  {
    for ( Standard_Integer c = 0; c < GetNumCols(); c++ )
    {
      Handle(HTMLService_HTMLTableCell) cell = GetCell( r, c );
      if ( !cell->IsVisible() )
        continue;

      Standard_Integer rowSpan = cell->GetRowSpan();
      Standard_Integer colSpan = cell->GetColSpan();
      if ( rowSpan < 2 && colSpan < 2 )
        continue;

      for ( Standard_Integer sr = r; sr < r + rowSpan; sr++ )
      {
        for ( Standard_Integer sc = c; sc < c + colSpan; sc++ )
        {
          Handle(HTMLService_HTMLTableCell) spanedCell = GetCell( sr, sc );
          if ( !spanedCell.IsNull() && spanedCell != cell )
            spanedCell->SetVisible( Standard_False );
        }
      }
    }
  }
}
