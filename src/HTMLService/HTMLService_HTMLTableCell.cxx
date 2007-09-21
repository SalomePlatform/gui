// File:      HTMLService_HTMLTableCell.cxx
// Created:   12.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#include <HTMLService_HTMLTableCell.hxx>

IMPLEMENT_STANDARD_HANDLE(HTMLService_HTMLTableCell, HTMLService_HTMLParagraph)
IMPLEMENT_STANDARD_RTTIEXT(HTMLService_HTMLTableCell, HTMLService_HTMLParagraph)

HTMLService_HTMLTableCell::HTMLService_HTMLTableCell()
: HTMLService_HTMLParagraph(),
myRowSpan( 1 ),
myColSpan( 1 ),
myWidth( -1 ),
myAlign( -1 ),
myUseBC( Standard_False ),
myPercent( Standard_False ),
myIsHeader( Standard_False )
{
}

HTMLService_HTMLTableCell::HTMLService_HTMLTableCell( const Handle(HTMLService_HTML)& parent )
: HTMLService_HTMLParagraph( parent ),
myRowSpan( 1 ),
myColSpan( 1 ),
myWidth( -1 ),
myAlign( -1 ),
myUseBC( Standard_False ),
myPercent( Standard_False ),
myIsHeader( Standard_False )
{
}

HTMLService_HTMLTableCell::~HTMLService_HTMLTableCell()
{
}

Standard_Integer HTMLService_HTMLTableCell::GetColSpan() const
{
  return myColSpan;
}

Standard_Integer HTMLService_HTMLTableCell::GetRowSpan() const
{
  return myRowSpan;
}

Standard_Integer HTMLService_HTMLTableCell::GetAlignment() const
{
  return myAlign;
}

Standard_Integer HTMLService_HTMLTableCell::GetWidth( Standard_Boolean* percent ) const
{
  if ( percent )
    *percent = myPercent;
  return myWidth;
}

Quantity_Color HTMLService_HTMLTableCell::GetBackgroundColor() const
{
  return myBgColor;
}

Standard_Boolean HTMLService_HTMLTableCell::IsHeaderCell() const
{
  return myIsHeader;
}

void HTMLService_HTMLTableCell::SetColSpan( const Standard_Integer span )
{
  myColSpan = span;
}

void HTMLService_HTMLTableCell::SetRowSpan( const Standard_Integer span )
{
  myRowSpan = span;
}

void HTMLService_HTMLTableCell::SetAlignment( const Standard_Integer align )
{
  myAlign = align;
}

void HTMLService_HTMLTableCell::SetWidth( const Standard_Integer width, const Standard_Boolean percent )
{
  myWidth = width;
  myPercent = percent;
}

void HTMLService_HTMLTableCell::SetBackgroundColor( const Quantity_Color& c )
{
  myBgColor = c;
  myUseBC = Standard_True;
}

void HTMLService_HTMLTableCell::SetHeaderCell( const Standard_Boolean on )
{
  myIsHeader = on;
}

void HTMLService_HTMLTableCell::Generate( SequenceOfHTMLLine& aHTML ) const
{
  TCollection_ExtendedString aLine;
  if ( IsHeaderCell() )
    aLine = TCollection_ExtendedString( "<th" );
  else
    aLine = TCollection_ExtendedString( "<td" );
  Standard_Boolean percent = Standard_False;
  if ( GetWidth( &percent ) >= 0 )
  {
    aLine += TCollection_ExtendedString( " width=" ) + TCollection_ExtendedString( GetWidth() );
    if ( percent )
      aLine += TCollection_ExtendedString( "%" );
  }
  if ( GetRowSpan() > 1 )
    aLine += TCollection_ExtendedString( " rowspan=" ) + TCollection_ExtendedString( GetRowSpan() );
  if ( GetColSpan() > 1 )
    aLine += TCollection_ExtendedString( " colspan=" ) + TCollection_ExtendedString( GetColSpan() );

  TCollection_ExtendedString align = AlignString( GetAlignment() );
  if ( align.Length() )
    aLine += align;

  if ( myUseBC )
    aLine += TCollection_ExtendedString( " bgcolor=" ) + ColorString( GetBackgroundColor() );

  aLine += TCollection_ExtendedString( ">" );
  aHTML.Append( aLine );

  HTMLService_HTMLParagraph::Generate( aHTML );
  if ( IsHeaderCell() )
    aHTML.Append( "</th>" );
  else
    aHTML.Append( "</td>" );
}

TCollection_ExtendedString HTMLService_HTMLTableCell::CellString() const
{
  TCollection_ExtendedString txt = GetText();

  if ( GetHeadingLevel() >= 0 )
    txt = EncloseToTag( txt, TCollection_ExtendedString( "h" ) + TCollection_ExtendedString( GetHeadingLevel() ) );

  if ( GetFontFlags() & Bold )
    txt = EncloseToTag( txt, TCollection_ExtendedString( "b" ) );
    
  if ( GetFontFlags() & Italic )
    txt = EncloseToTag( txt, TCollection_ExtendedString( "i" ) );

  if ( GetFontFlags() & Underline )
    txt = EncloseToTag( txt, TCollection_ExtendedString( "u" ) );
  
  if ( GetFontFlags() & Subscript )
    txt = EncloseToTag( txt, TCollection_ExtendedString( "sub" ) );

  if ( GetFontFlags() & Superscript )
    txt = EncloseToTag( txt, TCollection_ExtendedString( "sup" ) );

  return txt;
}
