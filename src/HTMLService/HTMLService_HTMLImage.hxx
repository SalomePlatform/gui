// File:      HTMLService_HTMLImage.hxx
// Created:   13.10.2006
// Author:    Sergey TELKOV
// Copyright (C) AREVA NP 2006

#ifndef HTMLSERVICE_HTMLIMAGE_H
#define HTMLSERVICE_HTMLIMAGE_H

#include <HTMLService.hxx>
#include <HTMLService_HTML.hxx>

#include <Quantity_Color.hxx>

/*!
  Class HTMLService_HTMLImage. Generator of the HTML image.
*/

class HTMLSERVICE_EXPORT HTMLService_HTMLImage : public HTMLService_HTML
{
public:
  enum { SP_Unknown, SP_File, SP_HTTP, SP_FTP } SourceProtocol;

public:
  Standard_EXPORT HTMLService_HTMLImage( const TCollection_ExtendedString& = TCollection_ExtendedString() );
  Standard_EXPORT HTMLService_HTMLImage( const Handle(HTMLService_HTML)&,
                                      const TCollection_ExtendedString& = TCollection_ExtendedString() );
  Standard_EXPORT virtual ~HTMLService_HTMLImage();

  Standard_EXPORT Standard_Integer           GetWidth() const;
  Standard_EXPORT Standard_Integer           GetHeight() const;
  Standard_EXPORT TCollection_ExtendedString GetSource() const;
  Standard_EXPORT Standard_Integer           GetAlignment() const;
  Standard_EXPORT Standard_Integer           GetSourceProtocol() const;

  Standard_EXPORT void                       SetWidth( const Standard_Integer );
  Standard_EXPORT void                       SetHeight( const Standard_Integer );
  Standard_EXPORT void                       SetAlignment( const Standard_Integer );
  Standard_EXPORT void                       SetSource( const TCollection_ExtendedString& );
  Standard_EXPORT void                       SetSourceProtocol( const Standard_Integer& );

protected:
  Standard_EXPORT virtual void               Generate( SequenceOfHTMLLine& ) const;

private:
  TCollection_ExtendedString                 SourcePrefix() const;

private:
  Standard_Integer                           myAlign;
  Standard_Integer                           myWidth;
  Standard_Integer                           myHeight;
  TCollection_ExtendedString                 mySource;
  Standard_Integer                           mySrcProto;

public:
  DEFINE_STANDARD_RTTI(HTMLService_HTMLImage)
};

DEFINE_STANDARD_HANDLE(HTMLService_HTMLImage, HTMLService_HTML)

#endif
