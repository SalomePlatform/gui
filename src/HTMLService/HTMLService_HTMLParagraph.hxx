// File:      HTMLService_HTMLParagraph.hxx
// Created:   13.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#ifndef HTMLSERVICE_HTMLPARAGRAPH_H
#define HTMLSERVICE_HTMLPARAGRAPH_H

#include <HTMLService.hxx>
#include <HTMLService_HTMLText.hxx>

/*!
  Class HTMLService_HTMLParagraph. Generator of the HTML paragraph text.
*/

class HTMLService_HTMLParagraph : public HTMLService_HTMLText
{
public:
  Standard_EXPORT HTMLService_HTMLParagraph();
  Standard_EXPORT HTMLService_HTMLParagraph( const Handle(HTMLService_HTML)& );
  Standard_EXPORT virtual ~HTMLService_HTMLParagraph();

  Standard_EXPORT Standard_Boolean GetNoBreak() const;
  Standard_EXPORT void             SetNoBreak( const Standard_Boolean& );

  Standard_EXPORT void             Clear();
  Standard_EXPORT void             Insert( const Handle(HTMLService_HTML)& );
  Standard_EXPORT void             InsertText( const TCollection_ExtendedString& );
  Standard_EXPORT void             InsertImage( const TCollection_ExtendedString&,
                                                const Standard_Integer = -1, const Standard_Integer = -1 );

protected:
  Standard_EXPORT virtual void     Generate( SequenceOfHTMLLine& ) const;

private:
  Standard_Boolean                 myNoBr;

public:
  DEFINE_STANDARD_RTTI(HTMLService_HTMLParagraph)
};

DEFINE_STANDARD_HANDLE(HTMLService_HTMLParagraph, HTMLService_HTMLText)

#endif
