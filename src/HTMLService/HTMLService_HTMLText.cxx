// File:      HTMLService_HTMLText.cxx
// Created:   13.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#include <HTMLService_HTMLText.hxx>

IMPLEMENT_STANDARD_HANDLE(HTMLService_HTMLText, HTMLService_HTML)
IMPLEMENT_STANDARD_RTTIEXT(HTMLService_HTMLText, HTMLService_HTML)

HTMLService_HTMLText::HTMLService_HTMLText( const TCollection_ExtendedString& text, const Standard_Integer fontFlags )
: HTMLService_HTML(),
myText( text ),
myHeading( -1 ),
myFontFlags( fontFlags ),
myUseColor( Standard_False )
{
}

HTMLService_HTMLText::HTMLService_HTMLText( const Handle(HTMLService_HTML)& parent,
                                      const TCollection_ExtendedString& text, const Standard_Integer fontFlags )
: HTMLService_HTML( parent ),
myText( text ),
myHeading( -1 ),
myFontFlags( fontFlags ),
myUseColor( Standard_False )
{
}

HTMLService_HTMLText::~HTMLService_HTMLText()
{
}

TCollection_ExtendedString HTMLService_HTMLText::GetText() const
{
  return myText;
}

Quantity_Color HTMLService_HTMLText::GetForegroundColor() const
{
  return myColor;
}

Standard_Integer HTMLService_HTMLText::GetFontFlags() const
{
  return myFontFlags;
}

Standard_Integer HTMLService_HTMLText::GetHeadingLevel() const
{
  return myHeading;
}

void HTMLService_HTMLText::SetText( const TCollection_ExtendedString& text )
{
  myText = text;
}

void HTMLService_HTMLText::SetForegroundColor( const Quantity_Color& c )
{
  myColor = c;
  myUseColor = Standard_True;
}

void HTMLService_HTMLText::SetFontFlags( const Standard_Integer flags )
{
  myFontFlags = flags;
}

void HTMLService_HTMLText::SetHeadingLevel( const Standard_Integer level )
{
  myHeading = level;
}

void HTMLService_HTMLText::Generate( SequenceOfHTMLLine& aHTML ) const
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
  
  if ( GetFontFlags() & StrikeOut )
    txt = EncloseToTag( txt, TCollection_ExtendedString( "s" ) );

  if ( GetFontFlags() & Subscript )
    txt = EncloseToTag( txt, TCollection_ExtendedString( "sub" ) );

  if ( GetFontFlags() & Superscript )
    txt = EncloseToTag( txt, TCollection_ExtendedString( "sup" ) );

  if ( myUseColor )
    txt = EncloseToTag( txt, TCollection_ExtendedString( "font" ),
                        TCollection_ExtendedString ( "color=" ) + ColorString( GetForegroundColor() ) );

  aHTML.Append( txt );
}
