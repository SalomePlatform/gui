// File:      HTMLService_HTML.hxx
// Created:   12.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#ifndef HTMLSERVICE_HTML_H
#define HTMLSERVICE_HTML_H

#include <HTMLService.hxx>

#include <MMgt_TShared.hxx>
#include <Standard_DefineHandle.hxx>

#include <NCollection_Sequence.hxx>
#include <TCollection_ExtendedString.hxx>

class Quantity_Color;
class Handle(HTMLService_HTML);

typedef NCollection_Sequence<TCollection_ExtendedString> SequenceOfHTMLLine;

/*!
  Class HTMLService_HTML. Base class for HTML generation.
*/

class HTMLSERVICE_EXPORT HTMLService_HTML : public MMgt_TShared
{
public:
  enum { Left = 0x01, Right = 0x02, Center = 0x04,
         Top = 0x08, Bottom = 0x10, Middle = 0x20,
         HAlignment = Left | Right | Center, VAlignment = Top | Bottom | Middle } AlignmentFlags;

public:
  Standard_EXPORT HTMLService_HTML();
  Standard_EXPORT HTMLService_HTML( const Handle(HTMLService_HTML)& );
  Standard_EXPORT ~HTMLService_HTML();

  Standard_EXPORT TCollection_ExtendedString GetTitle() const;
  Standard_EXPORT void                       SetTitle( const TCollection_ExtendedString& );

  Standard_EXPORT Standard_Boolean   GenerateFile( const TCollection_ExtendedString&,
                                                   const Standard_Boolean = Standard_False ) const;
  Standard_EXPORT SequenceOfHTMLLine GenerateText( const Standard_Boolean = Standard_False ) const;

  Standard_EXPORT static TCollection_ExtendedString ColorString( const Quantity_Color& );
  Standard_EXPORT static TCollection_ExtendedString AlignString( const Standard_Integer );

  Standard_EXPORT Standard_Boolean   IsVisible() const;
  Standard_EXPORT void               SetVisible( const Standard_Boolean );

protected:
  Standard_EXPORT virtual void       Generate( SequenceOfHTMLLine& ) const = 0;
  Standard_EXPORT virtual void       SubItemsHTML( SequenceOfHTMLLine& ) const;

  Standard_EXPORT TCollection_ExtendedString EncloseToTag( const TCollection_ExtendedString&, const TCollection_ExtendedString&,
                                                           const TCollection_ExtendedString& = TCollection_ExtendedString() ) const;

  Standard_EXPORT void               ClearSubItems();
  Standard_EXPORT void               InsertSubItem( const Handle(HTMLService_HTML)& );
  Standard_EXPORT void               RemoveSubItem( const Handle(HTMLService_HTML)& );

  void                               AppendIndent( SequenceOfHTMLLine& ) const;
  void                               AppendIndent( TCollection_ExtendedString& ) const;
  void                               Concatenate( SequenceOfHTMLLine&, const SequenceOfHTMLLine& ) const;

private:
  void                               BeginDocument( SequenceOfHTMLLine& ) const;
  void                               EndDocument( SequenceOfHTMLLine& ) const;

private:
  typedef NCollection_Sequence<Handle(HTMLService_HTML)> HTMLSequence;

private:
  TCollection_ExtendedString         myTitle;
  Standard_Boolean                   myVisible;
  HTMLSequence                       mySubItems;

public:
  DEFINE_STANDARD_RTTI(HTMLService_HTML)
};

DEFINE_STANDARD_HANDLE(HTMLService_HTML, MMgt_TShared)

#endif
