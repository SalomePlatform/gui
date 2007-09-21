// File:      HTMLService_HTMLImage.cxx
// Created:   13.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#include <HTMLService_HTMLImage.hxx>

IMPLEMENT_STANDARD_HANDLE(HTMLService_HTMLImage, HTMLService_HTML)
IMPLEMENT_STANDARD_RTTIEXT(HTMLService_HTMLImage, HTMLService_HTML)

HTMLService_HTMLImage::HTMLService_HTMLImage( const TCollection_ExtendedString& img )
: HTMLService_HTML(),
myAlign( -1 ),
myWidth( -1 ),
myHeight( -1 ),
mySource( img ),
mySrcProto( SP_File )
{
}

HTMLService_HTMLImage::HTMLService_HTMLImage( const Handle(HTMLService_HTML)& parent,
                                        const TCollection_ExtendedString& img )
: HTMLService_HTML( parent ),
myAlign( -1 ),
myWidth( -1 ),
myHeight( -1 ),
mySource( img ),
mySrcProto( SP_File )
{
}

HTMLService_HTMLImage::~HTMLService_HTMLImage()
{
}

TCollection_ExtendedString HTMLService_HTMLImage::GetSource() const
{
  return mySource;
}

Standard_Integer HTMLService_HTMLImage::GetAlignment() const
{
  return myAlign;
}

Standard_Integer HTMLService_HTMLImage::GetWidth() const
{
  return myWidth;
}

Standard_Integer HTMLService_HTMLImage::GetHeight() const
{
  return myHeight;
}

Standard_Integer HTMLService_HTMLImage::GetSourceProtocol() const
{
  return mySrcProto;
}

void HTMLService_HTMLImage::SetSource( const TCollection_ExtendedString& src )
{
  mySource = src;
}

void HTMLService_HTMLImage::SetAlignment( const Standard_Integer align )
{
  myAlign = align;
}

void HTMLService_HTMLImage::SetWidth( const Standard_Integer val )
{
  myWidth = val;
}

void HTMLService_HTMLImage::SetHeight( const Standard_Integer val )
{
  myHeight = val;
}

void HTMLService_HTMLImage::SetSourceProtocol( const Standard_Integer& sp )
{
  mySrcProto = sp;
}

void HTMLService_HTMLImage::Generate( SequenceOfHTMLLine& aHTML ) const
{
  TCollection_ExtendedString aLine( "<img" );

  aLine += TCollection_ExtendedString( " src=\"" ) + SourcePrefix() +
           GetSource() + TCollection_ExtendedString( "\"" );

  if ( GetWidth() >= 0 )
    aLine += TCollection_ExtendedString( " width=" ) + TCollection_ExtendedString( GetWidth() );
  if ( GetHeight() >= 0 )
    aLine += TCollection_ExtendedString( " height=" ) + TCollection_ExtendedString( GetHeight() );

  TCollection_ExtendedString align = AlignString( GetAlignment() );
  if ( align.Length() )
    aLine += TCollection_ExtendedString( " " ) + align;

  aLine += TCollection_ExtendedString( ">" );

  aHTML.Append( aLine );
}

TCollection_ExtendedString HTMLService_HTMLImage::SourcePrefix() const
{
  TCollection_ExtendedString prefix;
  switch ( GetSourceProtocol() )
  {
  case SP_File:
    prefix = TCollection_ExtendedString( "file://" );
    break;
  case SP_HTTP:
    prefix = TCollection_ExtendedString( "http://" );
    break;
  case SP_FTP:
    prefix = TCollection_ExtendedString( "ftp://" );
    break;
  }
  return prefix;
}
