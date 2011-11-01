// Name   : QtxXlsBorderTool.cpp
// Purpose: Implementation of QtxXlsBorderTool class.
//
// History:
// 20/10/11 - Oleg Agashin - Creation of the file

#include <QtxXlsBorderTool.h>
#include <QtxXlsWriter.h>


/**
 *
 */
int QtxXlsBorderTool::LeftTopThick( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                    const XlsBYTE theColor, const XlsWORD theFormatID,
                                    const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.leftBorderStyle = QtxXlsBiff::Thick;
  aBorder.leftBorderColor = theColor;
  aBorder.topBorderStyle  = QtxXlsBiff::Thick;
  aBorder.topBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::RightTopThick( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                     const XlsBYTE theColor, const XlsWORD theFormatID,
                                     const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.rightBorderStyle = QtxXlsBiff::Thick;
  aBorder.rightBorderColor = theColor;
  aBorder.topBorderStyle   = QtxXlsBiff::Thick;
  aBorder.topBorderColor   = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::LeftBottomThick( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                       const XlsBYTE theColor, const XlsWORD theFormatID,
                                       const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.leftBorderStyle    = QtxXlsBiff::Thick;
  aBorder.leftBorderColor    = theColor;
  aBorder.bottomBorderStyle  = QtxXlsBiff::Thick;
  aBorder.bottomBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::RightBottomThick( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                        const XlsBYTE theColor, const XlsWORD theFormatID,
                                        const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.rightBorderStyle    = QtxXlsBiff::Thick;
  aBorder.rightBorderColor    = theColor;
  aBorder.bottomBorderStyle   = QtxXlsBiff::Thick;
  aBorder.bottomBorderColor   = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::LeftTopThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.leftBorderStyle = QtxXlsBiff::Thin;
  aBorder.leftBorderColor = theColor;
  aBorder.topBorderStyle  = QtxXlsBiff::Thin;
  aBorder.topBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::RightTopThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                    const XlsBYTE theColor, const XlsWORD theFormatID,
                                    const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.rightBorderStyle = QtxXlsBiff::Thin;
  aBorder.rightBorderColor = theColor;
  aBorder.topBorderStyle   = QtxXlsBiff::Thin;
  aBorder.topBorderColor   = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::LeftBottomThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                      const XlsBYTE theColor, const XlsWORD theFormatID,
                                      const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.leftBorderStyle    = QtxXlsBiff::Thin;
  aBorder.leftBorderColor    = theColor;
  aBorder.bottomBorderStyle  = QtxXlsBiff::Thin;
  aBorder.bottomBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::RightBottomThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                       const XlsBYTE theColor, const XlsWORD theFormatID,
                                       const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.rightBorderStyle    = QtxXlsBiff::Thin;
  aBorder.rightBorderColor    = theColor;
  aBorder.bottomBorderStyle   = QtxXlsBiff::Thin;
  aBorder.bottomBorderColor   = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}



/**
 *
 */
int QtxXlsBorderTool::LeftThick( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                 const XlsBYTE theColor, const XlsWORD theFormatID,
                                 const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.leftBorderStyle = QtxXlsBiff::Thick;
  aBorder.leftBorderColor = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::RightThick( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                  const XlsBYTE theColor, const XlsWORD theFormatID,
                                  const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.rightBorderStyle = QtxXlsBiff::Thick;
  aBorder.rightBorderColor = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::TopThick( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                const XlsBYTE theColor, const XlsWORD theFormatID,
                                const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thick;
  aBorder.topBorderColor    = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::BottomThick( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.bottomBorderStyle   = QtxXlsBiff::Thick;
  aBorder.bottomBorderColor   = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::LeftThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                const XlsBYTE theColor, const XlsWORD theFormatID,
                                const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.leftBorderStyle = QtxXlsBiff::Thin;
  aBorder.leftBorderColor = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::RightThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                 const XlsBYTE theColor, const XlsWORD theFormatID,
                                 const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.rightBorderStyle = QtxXlsBiff::Thin;
  aBorder.rightBorderColor = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::TopThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                               const XlsBYTE theColor, const XlsWORD theFormatID,
                               const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thin;
  aBorder.topBorderColor    = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::BottomThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                  const XlsBYTE theColor, const XlsWORD theFormatID,
                                  const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.bottomBorderStyle   = QtxXlsBiff::Thin;
  aBorder.bottomBorderColor   = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::Thick( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                             const XlsBYTE theColor, const XlsWORD theFormatID,
                             const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thick;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thick;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thick;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thick;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::Thin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                            const XlsBYTE theColor, const XlsWORD theFormatID,
                            const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thin;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thin;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thin;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thin;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}



/**
 *
 */
int QtxXlsBorderTool::LeftTopThickRightBottomThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thick;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thick;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thin;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thin;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::RightTopThickLeftBottomThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thick;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thin;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thin;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thick;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::LeftBottomThickRightTopThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thin;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thick;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thick;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thin;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::RightBottomThickLeftTopThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thin;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thin;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thick;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thick;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::LeftTopRightThickBottomThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thick;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thick;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thin;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thick;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::LeftBottomRightThickTopThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thin;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thick;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thick;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thick;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::LeftRightThickTopBottomThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thin;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thick;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thin;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thick;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::TopThickLeftBottomRightThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thick;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thin;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thin;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thin;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::LeftThickTopBottomRightThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thin;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thick;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thin;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thin;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::BottomThickTopLeftRightThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thin;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thin;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thick;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thin;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::RightThickTopLeftBottomThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thin;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thin;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thin;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thick;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::LeftTopBottomThickRightThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thick;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thick;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thick;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thin;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::RightTopBottomThickLeftThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thick;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thin;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thick;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thick;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}

/**
 *
 */
int QtxXlsBorderTool::TopBottomThickLeftRightThin( QtxXlsWriter& theWriter, const XlsWORD theFontID,
                                                   const XlsBYTE theColor, const XlsWORD theFormatID,
                                                   const XlsWORD theAlign, const XlsWORD theVAlign )
{
  QtxXlsBiff::Border aBorder;
  QtxXlsBiff::cleanBorderStruct( aBorder );

  aBorder.topBorderStyle    = QtxXlsBiff::Thick;
  aBorder.topBorderColor    = theColor;
  aBorder.leftBorderStyle   = QtxXlsBiff::Thin;
  aBorder.leftBorderColor   = theColor;
  aBorder.bottomBorderStyle = QtxXlsBiff::Thick;
  aBorder.bottomBorderColor = theColor;
  aBorder.rightBorderStyle  = QtxXlsBiff::Thin;
  aBorder.rightBorderColor  = theColor;

  return theWriter.addCellStyle( theFontID, theFormatID,
                              theAlign, theVAlign, &aBorder );
}
