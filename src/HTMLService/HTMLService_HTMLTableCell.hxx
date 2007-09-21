// File:      HTMLService_HTMLTableCell.hxx
// Created:   12.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#ifndef HTMLService_HTMLTABLECELL_H
#define HTMLService_HTMLTABLECELL_H

#include <HTMLService.hxx>
#include <HTMLService_HTMLParagraph.hxx>

#include <Quantity_Color.hxx>

/*!
  Class HTMLService_HTMLTableCell. Generator of the HTML Table cell text.
*/

class HTMLSERVICE_EXPORT HTMLService_HTMLTableCell : public HTMLService_HTMLParagraph
{
public:
  Standard_EXPORT HTMLService_HTMLTableCell();
  Standard_EXPORT HTMLService_HTMLTableCell( const Handle(HTMLService_HTML)& );
  Standard_EXPORT virtual ~HTMLService_HTMLTableCell();

  Standard_EXPORT Standard_Integer           GetColSpan() const;
  Standard_EXPORT Standard_Integer           GetRowSpan() const;
  Standard_EXPORT Standard_Integer           GetAlignment() const;
  Standard_EXPORT Standard_Integer           GetWidth( Standard_Boolean* = 0 ) const;

  Standard_EXPORT Quantity_Color             GetBackgroundColor() const;
  Standard_EXPORT Standard_Boolean           IsHeaderCell() const;

  Standard_EXPORT void                       SetColSpan( const Standard_Integer );
  Standard_EXPORT void                       SetRowSpan( const Standard_Integer );
  Standard_EXPORT void                       SetAlignment( const Standard_Integer );
  Standard_EXPORT void                       SetWidth( const Standard_Integer,
                                                       const Standard_Boolean = Standard_False );
  Standard_EXPORT void                       SetBackgroundColor( const Quantity_Color& );
  Standard_EXPORT void                       SetHeaderCell( const Standard_Boolean );

protected:
  Standard_EXPORT virtual void               Generate( SequenceOfHTMLLine& ) const;

private:
  TCollection_ExtendedString                 CellString() const;

private:
  Standard_Integer           myWidth;
  Standard_Integer           myAlign;
  Standard_Boolean           myUseBC;
  Standard_Boolean           myPercent;
  Standard_Integer           myColSpan;
  Standard_Integer           myRowSpan;
  Quantity_Color             myBgColor;
  Standard_Boolean           myIsHeader;

public:
  DEFINE_STANDARD_RTTI(HTMLService_HTMLTableCell)
};

DEFINE_STANDARD_HANDLE(HTMLService_HTMLTableCell, HTMLService_HTMLParagraph)

#endif
