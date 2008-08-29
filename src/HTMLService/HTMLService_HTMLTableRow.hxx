// File:      HTMLService_HTMLTableRow.hxx
// Created:   12.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#ifndef HTMLService_HTMLTABLEROW_H
#define HTMLService_HTMLTABLEROW_H

#include <HTMLService.hxx>
#include <HTMLService_HTML.hxx>

#include <HTMLService_HTMLTableCell.hxx>

#include <TColStd_HArray1OfTransient.hxx>

/*!
  Class HTMLService_HTMLTableRow. Generator of the HTML Table row text.
*/

class HTMLService_HTMLTableRow : public HTMLService_HTML
{
public:
  Standard_EXPORT HTMLService_HTMLTableRow( const Standard_Integer = 0 );
  Standard_EXPORT HTMLService_HTMLTableRow( const Handle(HTMLService_HTML)&, const Standard_Integer = 0 );
  Standard_EXPORT virtual ~HTMLService_HTMLTableRow();

  Standard_EXPORT Standard_Integer          GetSize() const;
  Standard_EXPORT Handle(HTMLService_HTMLTableCell) GetCell( const Standard_Integer ) const;

  Standard_EXPORT Quantity_Color            GetBackgroundColor() const;
  Standard_EXPORT void                      SetBackgroundColor( const Quantity_Color& );

protected:
  Standard_EXPORT virtual void              Generate( SequenceOfHTMLLine& ) const;

private:
  void                                      Initialize( const Standard_Integer );

private:
  Handle(TColStd_HArray1OfTransient)        myCells;
  Standard_Boolean                          myUseBC;
  Quantity_Color                            myBgColor;

public:
  DEFINE_STANDARD_RTTI(HTMLService_HTMLTableRow)
};

DEFINE_STANDARD_HANDLE(HTMLService_HTMLTableRow, HTMLService_HTML)

#endif
