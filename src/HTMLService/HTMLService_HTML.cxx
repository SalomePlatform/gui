// File:      HTMLService_HTML.cxx
// Created:   12.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#include <HTMLService_HTML.hxx>

#include <Quantity_Color.hxx>

#include <OSD_File.hxx>
#include <OSD_Protection.hxx>
#include <Standard_TypeDef.hxx>
#include <Standard_ExtCharacter.hxx>

IMPLEMENT_STANDARD_HANDLE(HTMLService_HTML, MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(HTMLService_HTML, MMgt_TShared)

HTMLService_HTML::HTMLService_HTML()
: myVisible( Standard_True )
{
}

HTMLService_HTML::HTMLService_HTML( const Handle(HTMLService_HTML)& parent )
: myVisible( Standard_True )
{
  if ( !parent.IsNull() )
    parent->InsertSubItem( this );
}

HTMLService_HTML::~HTMLService_HTML()
{
}

TCollection_ExtendedString HTMLService_HTML::GetTitle() const
{
  return myTitle;
}

void HTMLService_HTML::SetTitle( const TCollection_ExtendedString& title )
{
  myTitle = title;
}

Standard_Boolean HTMLService_HTML::GenerateFile( const TCollection_ExtendedString& aFileName,
                                              const Standard_Boolean fullDoc ) const
{
  if ( !aFileName.Length() )
    return Standard_False;

  TCollection_AsciiString aFName( aFileName );
  OSD_File aFile( aFName );
  aFile.Build( OSD_WriteOnly, OSD_Protection() );
  if ( !aFile.IsOpen() )
    return Standard_False;

  SequenceOfHTMLLine aHTML = GenerateText( fullDoc );

  TCollection_AsciiString endL( "\n" );
  for ( Standard_Integer i = 1; i <= aHTML.Length(); i++ )
  {
    TCollection_ExtendedString aLine = aHTML.Value( i );
    TCollection_AsciiString aAsciiLine;
    if ( aLine.IsAscii() )
      aAsciiLine = TCollection_AsciiString( aLine );
    else
    {
      for ( Standard_Integer j = 1; j <= aLine.Length(); j++ )
      {
        Standard_ExtCharacter curExt = aLine.Value(j);
        if ( IsAnAscii( curExt ) )
          aAsciiLine += TCollection_AsciiString( ToCharacter( curExt ) );
        else
        {
          char buf[256];
          sprintf( buf, "&#%d;", curExt );
          aAsciiLine+= TCollection_AsciiString( buf );
        }
      }
    }
    aFile.Write( aAsciiLine, aAsciiLine.Length() );
    aFile.Write( endL, 1 );
  }
  aFile.Close();

  return Standard_True;
}

SequenceOfHTMLLine HTMLService_HTML::GenerateText( const Standard_Boolean fullDoc ) const
{
  SequenceOfHTMLLine aHTML;

  if ( fullDoc )
    BeginDocument( aHTML );

  SequenceOfHTMLLine aBody;
  Generate( aBody );
  if ( fullDoc )
    AppendIndent( aBody );

  for ( Standard_Integer i = 1; i <= aBody.Length(); i++ )
    aHTML.Append( aBody.Value( i ) );

  if ( fullDoc )
    EndDocument( aHTML );

  return aHTML;
}

Standard_Boolean HTMLService_HTML::IsVisible() const
{
  return myVisible;
}

void HTMLService_HTML::SetVisible( const Standard_Boolean vis )
{
  myVisible = vis;
}

void HTMLService_HTML::SubItemsHTML( SequenceOfHTMLLine& theHTML ) const
{
  for ( Standard_Integer i = 1; i <= mySubItems.Length(); i++ )
  {
    Handle(HTMLService_HTML) item = mySubItems.Value( i );
    if ( !item->IsVisible() )
      continue;

    SequenceOfHTMLLine aLines;
    item->Generate( aLines );
    AppendIndent( aLines );
    Concatenate( theHTML, aLines );
  }
}

void HTMLService_HTML::AppendIndent( SequenceOfHTMLLine& aHTML ) const
{
  for ( Standard_Integer i = 1; i <= aHTML.Length(); i++ )
    AppendIndent( aHTML.ChangeValue( i ) );
}

void HTMLService_HTML::AppendIndent( TCollection_ExtendedString& aLine ) const
{
  Standard_Integer ident = 2;
  TCollection_ExtendedString identString( ident, ' ' );
  aLine.Insert( 1, identString );
}

void HTMLService_HTML::Concatenate( SequenceOfHTMLLine& aTrg, const SequenceOfHTMLLine& aSrc ) const
{
  for ( Standard_Integer i = 1; i <= aSrc.Length(); i++ )
    aTrg.Append( aSrc.Value( i ) );
}

void HTMLService_HTML::BeginDocument( SequenceOfHTMLLine& theHTML ) const
{
  theHTML.Append( "<html>" );
  theHTML.Append( "<head>" );
  theHTML.Append( TCollection_ExtendedString( "<title>" ) + GetTitle() + TCollection_ExtendedString( "</title>" ) );
  theHTML.Append( "</head>" );
  theHTML.Append( "<body>" );
}

void HTMLService_HTML::EndDocument( SequenceOfHTMLLine& theHTML ) const
{
  theHTML.Append( "</body>" );
  theHTML.Append( "</html>" );
}

void HTMLService_HTML::InsertSubItem( const Handle(HTMLService_HTML)& item )
{
  if ( item.IsNull() )
    return;

  Standard_Boolean found = Standard_False;
  for ( Standard_Integer i = 1; i <= mySubItems.Length() && !found; i++ )
    found = mySubItems.Value( i ) == item;

  if ( !found )
    mySubItems.Append( item );
}

void HTMLService_HTML::RemoveSubItem( const Handle(HTMLService_HTML)& item )
{
  if ( item.IsNull() )
    return;

  Standard_Integer idx = -1;
  for ( Standard_Integer i = 1; i <= mySubItems.Length() && idx == -1; i++ )
  {
    if ( mySubItems.Value( i ) == item )
      idx = i;
  }

  if ( idx != -1 )
    mySubItems.Remove( idx );
}

void HTMLService_HTML::ClearSubItems()
{
  mySubItems.Clear();
}

TCollection_ExtendedString HTMLService_HTML::ColorString( const Quantity_Color& c )
{
  Standard_Integer r = (Standard_Integer)( c.Red() * 255 );
  Standard_Integer g = (Standard_Integer)( c.Green() * 255 );
  Standard_Integer b = (Standard_Integer)( c.Blue() * 255 );

  char buf[255];
  sprintf( buf, "#%02X%02X%02X", r, g, b );
  return TCollection_ExtendedString( buf );
}

TCollection_ExtendedString HTMLService_HTML::AlignString( const Standard_Integer flags )
{
  TCollection_ExtendedString hRes;
  switch ( flags & HAlignment )
  {
  case Left:
    hRes = TCollection_ExtendedString( "left" );
    break;
  case Right:
    hRes = TCollection_ExtendedString( "right" );
    break;
  case Center:
    hRes = TCollection_ExtendedString( "center" );
    break;
  }

  if ( hRes.Length() )
    hRes = TCollection_ExtendedString( "align=" ) + hRes;

  TCollection_ExtendedString vRes;
  switch ( flags & VAlignment )
  {
  case Top:
    vRes = TCollection_ExtendedString( "top" );
    break;
  case Bottom:
    vRes = TCollection_ExtendedString( "bottom" );
    break;
  case Middle:
    vRes = TCollection_ExtendedString( "middle" );
    break;
  }

  if ( vRes.Length() )
    vRes = TCollection_ExtendedString( "valign=" ) + hRes;

  TCollection_ExtendedString res;
  if ( hRes.Length() )
    res += hRes;
  if ( vRes.Length() )
  {
    if ( res.Length() )
      res += TCollection_ExtendedString( " " );
    res += vRes;
  }

  return res;
}

TCollection_ExtendedString HTMLService_HTML::EncloseToTag( const TCollection_ExtendedString& txt,
                                                        const TCollection_ExtendedString& tag,
                                                        const TCollection_ExtendedString& attribs ) const
{
  TCollection_ExtendedString res;
  if ( tag.Length() )
  {
    res = TCollection_ExtendedString ( "<" ) + tag;
    if ( attribs.Length() )
      res += TCollection_ExtendedString( " " ) + attribs;
    res += TCollection_ExtendedString( ">" ) + txt + TCollection_ExtendedString( "</" ) +
           tag + TCollection_ExtendedString( ">" );
  }
  else
    res = txt;

  return res;
}
