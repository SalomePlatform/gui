// File:      HTMLService_HTMLText.hxx
// Created:   13.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#ifndef HTMLSERVICE_HTMLTEXT_H
#define HTMLSERVICE_HTMLTEXT_H

#include <HTMLService.hxx>
#include <HTMLService_HTML.hxx>

#include <Quantity_Color.hxx>

/*!
  Class HTMLService_HTMLText. Generator of the simple HTML text.
*/

class HTMLSERVICE_EXPORT HTMLService_HTMLText : public HTMLService_HTML
{
public:
  enum { Normal = 0x00, Bold = 0x01, Italic = 0x02, Underline = 0x04,
         StrikeOut = 0x08, Subscript = 0x10, Superscript = 0x20 } FontFlags;

public:
  Standard_EXPORT HTMLService_HTMLText( const TCollection_ExtendedString& = TCollection_ExtendedString(),
                                     const Standard_Integer = Normal );
  Standard_EXPORT HTMLService_HTMLText( const Handle(HTMLService_HTML)&,
                                     const TCollection_ExtendedString& = TCollection_ExtendedString(),
                                     const Standard_Integer = Normal );
  Standard_EXPORT virtual ~HTMLService_HTMLText();

  Standard_EXPORT TCollection_ExtendedString GetText() const;
  Standard_EXPORT Standard_Integer           GetFontFlags() const;
  Standard_EXPORT Standard_Integer           GetHeadingLevel() const;
  Standard_EXPORT Quantity_Color             GetForegroundColor() const;

  Standard_EXPORT void                       SetFontFlags( const Standard_Integer );
  Standard_EXPORT void                       SetHeadingLevel( const Standard_Integer );
  Standard_EXPORT void                       SetForegroundColor( const Quantity_Color& );
  Standard_EXPORT void                       SetText( const TCollection_ExtendedString& );

protected:
  Standard_EXPORT virtual void               Generate( SequenceOfHTMLLine& ) const;

private:
  TCollection_ExtendedString myText;
  Quantity_Color             myColor;
  Standard_Integer           myHeading;
  Standard_Boolean           myUseColor;
  Standard_Integer           myFontFlags;

public:
  DEFINE_STANDARD_RTTI(HTMLService_HTMLText)
};

DEFINE_STANDARD_HANDLE(HTMLService_HTMLText, HTMLService_HTML)

#endif
