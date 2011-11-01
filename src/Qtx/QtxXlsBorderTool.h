// Name   : QtxXlsBorderTool.h
// Purpose: QtxXlsBorderTool is a class for easy creation of cell border styles.
//
// History:
// 20/10/11 - Oleg Agashin - Creation of the file

#ifndef QTXXLSBORDERTOOL_H_
#define QTXXLSBORDERTOOL_H_

#include <QtxXlsBiff.h>

class QtxXlsWriter;

class QtxXlsBorderTool
{
public:
// Corner styles
  //Thick
  static int LeftTopThick( QtxXlsWriter& theWriter,
                           const XlsWORD theFontID,
                           const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                           const XlsWORD theFormatID = QtxXlsBiff::General,
                           const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                           const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int RightTopThick( QtxXlsWriter& theWriter,
                            const XlsWORD theFontID,
                            const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                            const XlsWORD theFormatID = QtxXlsBiff::General,
                            const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                            const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int LeftBottomThick( QtxXlsWriter& theWriter,
                              const XlsWORD theFontID,
                              const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                              const XlsWORD theFormatID = QtxXlsBiff::General,
                              const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                              const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int RightBottomThick( QtxXlsWriter& theWriter,
                               const XlsWORD theFontID,
                               const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                               const XlsWORD theFormatID = QtxXlsBiff::General,
                               const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                               const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  // Thin
  static int LeftTopThin( QtxXlsWriter& theWriter,
                          const XlsWORD theFontID,
                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                          const XlsWORD theFormatID = QtxXlsBiff::General,
                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int RightTopThin( QtxXlsWriter& theWriter,
                           const XlsWORD theFontID,
                           const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                           const XlsWORD theFormatID = QtxXlsBiff::General,
                           const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                           const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int LeftBottomThin( QtxXlsWriter& theWriter,
                             const XlsWORD theFontID,
                             const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                             const XlsWORD theFormatID = QtxXlsBiff::General,
                             const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                             const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int RightBottomThin( QtxXlsWriter& theWriter,
                              const XlsWORD theFontID,
                              const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                              const XlsWORD theFormatID = QtxXlsBiff::General,
                              const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                              const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );


// Side styles
  //Thick
  static int LeftThick( QtxXlsWriter& theWriter,
                        const XlsWORD theFontID,
                        const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                        const XlsWORD theFormatID = QtxXlsBiff::General,
                        const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                        const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int RightThick( QtxXlsWriter& theWriter,
                         const XlsWORD theFontID,
                         const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                         const XlsWORD theFormatID = QtxXlsBiff::General,
                         const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                         const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int TopThick( QtxXlsWriter& theWriter,
                       const XlsWORD theFontID,
                       const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                       const XlsWORD theFormatID = QtxXlsBiff::General,
                       const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                       const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int BottomThick( QtxXlsWriter& theWriter,
                          const XlsWORD theFontID,
                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                          const XlsWORD theFormatID = QtxXlsBiff::General,
                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  //For whole cell border
  static int Thick( QtxXlsWriter& theWriter,
                    const XlsWORD theFontID,
                    const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                    const XlsWORD theFormatID = QtxXlsBiff::General,
                    const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                    const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );


  //Thin
  static int LeftThin( QtxXlsWriter& theWriter,
                       const XlsWORD theFontID,
                       const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                       const XlsWORD theFormatID = QtxXlsBiff::General,
                       const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                       const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int RightThin( QtxXlsWriter& theWriter,
                        const XlsWORD theFontID,
                        const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                        const XlsWORD theFormatID = QtxXlsBiff::General,
                        const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                        const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int TopThin( QtxXlsWriter& theWriter,
                      const XlsWORD theFontID,
                      const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                      const XlsWORD theFormatID = QtxXlsBiff::General,
                      const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                      const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int BottomThin( QtxXlsWriter& theWriter,
                         const XlsWORD theFontID,
                         const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                         const XlsWORD theFormatID = QtxXlsBiff::General,
                         const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                         const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  //For whole cell border
  static int Thin( QtxXlsWriter& theWriter,
                   const XlsWORD theFontID,
                   const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                   const XlsWORD theFormatID = QtxXlsBiff::General,
                   const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                   const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

// Combine styles
  static int LeftTopThickRightBottomThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int RightTopThickLeftBottomThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int LeftBottomThickRightTopThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int RightBottomThickLeftTopThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int LeftTopRightThickBottomThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int LeftBottomRightThickTopThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int LeftRightThickTopBottomThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int TopThickLeftBottomRightThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int LeftThickTopBottomRightThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int BottomThickTopLeftRightThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int RightThickTopLeftBottomThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int LeftTopBottomThickRightThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int RightTopBottomThickLeftThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );

  static int TopBottomThickLeftRightThin( QtxXlsWriter& theWriter,
                                          const XlsWORD theFontID,
                                          const XlsBYTE theColor = QtxXlsBiff::DefaultForegroundColor,
                                          const XlsWORD theFormatID = QtxXlsBiff::General,
                                          const XlsWORD theAlign = QtxXlsBiff::AlignGeneral,
                                          const XlsWORD theVAlign = QtxXlsBiff::VAlignCenter );
};

#endif /* QTXXLSBORDERTOOL_H_ */
