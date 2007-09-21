// File:      HTMLService_HTMLParagraph.cxx
// Created:   13.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#include <HTMLService_HTMLParagraph.hxx>

#include <HTMLService_HTMLImage.hxx>

IMPLEMENT_STANDARD_HANDLE(HTMLService_HTMLParagraph, HTMLService_HTMLText)
IMPLEMENT_STANDARD_RTTIEXT(HTMLService_HTMLParagraph, HTMLService_HTMLText)

HTMLService_HTMLParagraph::HTMLService_HTMLParagraph()
: HTMLService_HTMLText(),
myNoBr( Standard_False )
{
}

HTMLService_HTMLParagraph::HTMLService_HTMLParagraph( const Handle(HTMLService_HTML)& parent )
: HTMLService_HTMLText( parent ),
myNoBr( Standard_False )
{
}

HTMLService_HTMLParagraph::~HTMLService_HTMLParagraph()
{
}

Standard_Boolean HTMLService_HTMLParagraph::GetNoBreak() const
{
  return myNoBr;
}

void HTMLService_HTMLParagraph::SetNoBreak( const Standard_Boolean& on )
{
  myNoBr = on;
}

void HTMLService_HTMLParagraph::Clear()
{
  ClearSubItems();
}

void HTMLService_HTMLParagraph::Insert( const Handle(HTMLService_HTML)& item )
{
  InsertSubItem( item );
}

void HTMLService_HTMLParagraph::InsertText( const TCollection_ExtendedString& text )
{
  Insert( new HTMLService_HTMLText( this, text ) );
}

void HTMLService_HTMLParagraph::InsertImage( const TCollection_ExtendedString& img,
                                          const Standard_Integer w, const Standard_Integer h )
{
  Handle(HTMLService_HTMLImage) image = new HTMLService_HTMLImage( this, img );
  if ( w > 0 )
    image->SetWidth( w );
  if ( h > 0 )
    image->SetHeight( h );

  Insert( image );
}

void HTMLService_HTMLParagraph::Generate( SequenceOfHTMLLine& aHTML ) const
{
  SequenceOfHTMLLine aLines;
  HTMLService_HTMLText::Generate( aLines );

  TCollection_ExtendedString aText;
  for ( Standard_Integer i = 1; i <= aLines.Length(); i++ )
    aText += aLines.Value( i );

  if ( GetNoBreak() )
    aText = EncloseToTag( aText, "nobr" );

  aText = EncloseToTag( aText, "p" );

  TCollection_ExtendedString aHead( aText ), aTail;
  Standard_Integer idx = aHead.Search( "</" );
  if ( idx != -1 )
    aTail = aHead.Split( idx - 1 );

  if ( aHead.Length() )
    aHTML.Append( aHead );

  SubItemsHTML( aHTML );

  if ( aTail.Length() )
    aHTML.Append( aTail );
}
