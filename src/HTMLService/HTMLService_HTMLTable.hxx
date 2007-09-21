// File:      HTMLService_HTMLTable.hxx
// Created:   12.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#ifndef HTMLSERVICE_HTMLTABLE_H
#define HTMLSERVICE_HTMLTABLE_H

#include <HTMLService.hxx>
#include <HTMLService_HTML.hxx>

#include <HTMLService_HTMLTableRow.hxx>

#include <TColStd_HArray1OfTransient.hxx>

#include <Quantity_Color.hxx>

/*!
  Class HTMLService_HTMLTable. Generator of the HTML Table text.
*/

class HTMLSERVICE_EXPORT HTMLService_HTMLTable : public HTMLService_HTML
{
public:
  Standard_EXPORT HTMLService_HTMLTable( const Standard_Integer = 0, const Standard_Integer = 0 );
  Standard_EXPORT HTMLService_HTMLTable( const Handle(HTMLService_HTML)&, const Standard_Integer = 0, const Standard_Integer = 0 );
  Standard_EXPORT virtual ~HTMLService_HTMLTable();

  Standard_EXPORT Standard_Integer   GetNumRows() const;
  Standard_EXPORT Standard_Integer   GetNumCols() const;

  Standard_EXPORT Handle(HTMLService_HTMLTableRow)  GetRow( const Standard_Integer ) const;
  Standard_EXPORT Handle(HTMLService_HTMLTableCell) GetCell( const Standard_Integer, const Standard_Integer ) const;

  Standard_EXPORT Standard_Integer   GetBorder() const;
  Standard_EXPORT Standard_Integer   GetCellSpacing() const;
  Standard_EXPORT Standard_Integer   GetCellPadding() const;
  Standard_EXPORT Quantity_Color     GetBackgroundColor() const;
  Standard_EXPORT Standard_Integer   GetWidth( Standard_Boolean* = 0 ) const;

  Standard_EXPORT void               SetBorder( const Standard_Integer );
  Standard_EXPORT void               SetCellSpacing( const Standard_Integer );
  Standard_EXPORT void               SetCellPadding( const Standard_Integer );
  Standard_EXPORT void               SetBackgroundColor( const Quantity_Color& );
  Standard_EXPORT void               SetWidth( const Standard_Integer,
                                               const Standard_Boolean = Standard_False );

  Standard_EXPORT void               SetRowSpan( const Standard_Integer, const Standard_Integer );
  Standard_EXPORT void               SetColumnSpan( const Standard_Integer, const Standard_Integer );
  Standard_EXPORT void               SetColumnWidth( const Standard_Integer, const Standard_Integer,
                                                     const Standard_Boolean = Standard_False );

protected:
  Standard_EXPORT virtual void       Generate( SequenceOfHTMLLine& ) const;

private:
  void                               PrepareCells() const;
  void                               Initialize( const Standard_Integer, const Standard_Integer );

private:
  Handle(TColStd_HArray1OfTransient) myRows;

  Standard_Boolean                   myUseBC;
  Standard_Integer                   myWidth;
  Standard_Integer                   myBorder;
  Standard_Boolean                   myPercent;
  Quantity_Color                     myBgColor;
  Standard_Integer                   mySpacing;
  Standard_Integer                   myPadding;

public:
  DEFINE_STANDARD_RTTI(HTMLService_HTMLTable)
};

DEFINE_STANDARD_HANDLE(HTMLService_HTMLTable, HTMLService_HTML)

#endif
