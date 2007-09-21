// File:      HTMLService_HTMLTableRow.cxx
// Created:   12.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#include <HTMLService_HTMLTableRow.hxx>

IMPLEMENT_STANDARD_HANDLE(HTMLService_HTMLTableRow, HTMLService_HTML)
IMPLEMENT_STANDARD_RTTIEXT(HTMLService_HTMLTableRow, HTMLService_HTML)

HTMLService_HTMLTableRow::HTMLService_HTMLTableRow( const Standard_Integer cells )
: HTMLService_HTML(),
myUseBC( Standard_False )
{
  Initialize( cells );
}

HTMLService_HTMLTableRow::HTMLService_HTMLTableRow( const Handle(HTMLService_HTML)& parent, const Standard_Integer cells )
: HTMLService_HTML( parent ),
myUseBC( Standard_False )
{
  Initialize( cells );
}

HTMLService_HTMLTableRow::~HTMLService_HTMLTableRow()
{
}

Standard_Integer HTMLService_HTMLTableRow::GetSize() const
{
  if ( myCells.IsNull() )
    return 0;

  return myCells->Length();
}

Handle(HTMLService_HTMLTableCell) HTMLService_HTMLTableRow::GetCell( const Standard_Integer index ) const
{
  Handle(HTMLService_HTMLTableCell) cell;
  if ( !myCells.IsNull() )
  {
    Standard_Integer idx = index + myCells->Lower();
    if ( idx >= myCells->Lower() && idx <= myCells->Upper() )
      cell = Handle(HTMLService_HTMLTableCell)::DownCast(myCells->Value( idx ));
  }
  return cell;
}

Quantity_Color HTMLService_HTMLTableRow::GetBackgroundColor() const
{
  return myBgColor;
}

void HTMLService_HTMLTableRow::SetBackgroundColor( const Quantity_Color& c )
{
  myBgColor = c;
  myUseBC = Standard_True;
}

void HTMLService_HTMLTableRow::Initialize( const Standard_Integer num )
{
  if ( num > 0 )
    myCells = new TColStd_HArray1OfTransient( 0, num - 1 );

  if ( myCells.IsNull() )
    return;

  for ( Standard_Integer i = myCells->Lower(); i <= myCells->Upper(); i++ )
  {
    Handle(HTMLService_HTMLTableCell) cell = new HTMLService_HTMLTableCell();
    myCells->SetValue( i, cell );
    InsertSubItem( cell );
  }
}

void HTMLService_HTMLTableRow::Generate( SequenceOfHTMLLine& aHTML ) const
{
  TCollection_ExtendedString aLine( "<tr" );
  if ( myUseBC )
    aLine += TCollection_ExtendedString( " bgcolor=" ) + ColorString( GetBackgroundColor() );

  aLine += TCollection_ExtendedString( ">" );
  aHTML.Append( aLine );
  SubItemsHTML( aHTML );
  aHTML.Append( TCollection_ExtendedString( "</tr>" ) );
}
