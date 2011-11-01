// Name   : QtxXlsBiff.cpp
// Purpose: Implementation of QtxXlsBiff class.
//
// History:
// 17/10/11 - Oleg Agashin - Creation of the file

#include <QtxXlsBiff.h>

/**
 *
 */
QtxXlsBiff::QtxXlsBiff()
: myDefaultFontName("Arial"),
  myDefaultFontSize(10),
  mySheetName("Sheet1"),
  myLabelsNum(0)
{
  clear();
}

/**
 * Check that high bytes of string chars
 * equal to 0 and can be skipped
 */
bool QtxXlsBiff::canBePacked( const QString& theStr )
{
  QString::const_iterator anIt = theStr.begin();
  while( (anIt++) != theStr.end() )
  {
    if ( (anIt->unicode() & 0xFF00) > 0 )
      return false;
  }
  return true;
}

/**
 *
 */
void QtxXlsBiff::SetSheetName( const QString& theName )
{
  if( !theName.isEmpty() )
    mySheetName = theName;
}

/**
 * Clear all structures
 */
void QtxXlsBiff::clear()
{
  cleanBuffers();

  myFontTable.clear();
  myFontNames.clear();
  addDefaultFonts();

  myStyleTable.clear();
  addDefaultStyles();

  mySSTTable.clear();
  myLabelsNum = 0;

  myMergedCells.clear();

  myBlankCells.clear();
}

/**
 *
 */
void QtxXlsBiff::cleanBuffers()
{
  myWbDataBuf.clear();
  myShDataBuf.clear();
}

/**
 * BOF record
 */
void QtxXlsBiff::createBOF()
{
  BIFF8_BOF aBOF;

  //BOF for workbook
  aBOF.biffRecord.biffNum = _BOF;
  aBOF.biffRecord.length  = SIZEOF(BIFF8_BOF);
  aBOF.verOfBIFF          = BIFF_ver;
  aBOF.typeOfData         = WorkbookGlobals;
  aBOF.buildID            = 0x0DBB;
  aBOF.buildYear          = 0x07CC;
  aBOF.historyFlags       = 0x41;
  aBOF.lowestVer          = 0x6;
  myWbDataBuf.append( BYTE_ARRAY(aBOF.buf, sizeof(BIFF8_BOF)) );

  //BOF for worksheet
  aBOF.typeOfData         = Sheet;
  myShDataBuf.prepend( BYTE_ARRAY(aBOF.buf, sizeof(BIFF8_BOF)) );
}

/**
 * WINDOW1 record
 */
void QtxXlsBiff::createWindow1()
{
  BIFF8_WINDOW1 aWND;

  aWND.biffRecord.biffNum = WINDOW1;
  aWND.biffRecord.length  = SIZEOF(BIFF8_WINDOW1);
  aWND.xWnd               = 0;
  aWND.yWnd               = 0;
  aWND.dxWnd              = 0x25BC;
  aWND.dyWnd              = 0x1572;
  aWND.optionFlags        = None;
    aWND.fDspHScroll      = 1;
    aWND.fDspVScroll      = 1;
    aWND.fDisplayWbTabs   = 1;
  aWND.iSelWbTab          = 0;
  aWND.iFirstDispWbTab    = 0;
  aWND.selTabNum          = 1;
  aWND.wTabRatio          = 0x0258;
  
  myWbDataBuf.append( BYTE_ARRAY(aWND.buf, sizeof(BIFF8_WINDOW1)) );
}

/**
 * Date1904 record
 */
void QtxXlsBiff::createDate1904()
{
  BIFF8_DATE1904 aDate;
  aDate.biffRecord.biffNum = DATE1904;
  aDate.biffRecord.length  = SIZEOF(BIFF8_DATE1904);
  aDate.f1904              = None;
  myWbDataBuf.append( BYTE_ARRAY(aDate.buf, sizeof(BIFF8_DATE1904)) );
}

/**
 * Adds user-defined font into the font table
 */
int QtxXlsBiff::addFont( const QString& theFontName,
                      const XlsWORD theFontSize,
                      const bool theIsBold,
                      const XlsWORD theFontColor,
                      const XlsWORD theFontStyle,
                      const XlsWORD theSuperSubscriptType,
                      const XlsWORD theUnderlineType )

{
  BIFF8_FONT aFont = formatFontRecord( theFontName,
                                       theFontSize,
                                       theIsBold,
                                       theFontColor,
                                       theFontStyle,
                                       theSuperSubscriptType,
                                       theUnderlineType );

  myFontTable.append( aFont );
  myFontNames.append( theFontName );
  
  return myFontTable.size() - 1;
}

/**
 *
 */
QtxXlsBiff::BIFF8_FONT QtxXlsBiff::formatFontRecord( const QString& theFontName,
                                                     const XlsWORD theFontSize,
                                                     const bool theIsBold,
                                                     const XlsWORD theFontColor,
                                                     const XlsWORD theFontStyle,
                                                     const XlsWORD theSuperSubscriptType,
                                                     const XlsWORD theUnderlineType )

{
  BIFF8_FONT aFont;
  aFont.biffRecord.biffNum = FONT;
  //Beacause unicode
  aFont.biffRecord.length  = SIZEOF(BIFF8_FONT) + theFontName.length();
  aFont.height             = theFontSize * 20;
  aFont.fontAttr           = theFontStyle;
  aFont.color              = theFontColor;
  aFont.boldStyle          = theIsBold ? BoldText : NormalText;
  aFont.superSubStyle      = theSuperSubscriptType;
  aFont.underlineStyle     = theUnderlineType;
  aFont.fontFamily         = None;
  aFont.charSet            = ANSI_Latin;
  aFont.reserved           = None;
  aFont.fontNameLen        = theFontName.length();
  aFont.unicodeStrFlags    = Compressed;

  if ( !canBePacked( theFontName ) )
  {
    aFont.unicodeStrFlags    = Uncompressed;
    aFont.biffRecord.length += theFontName.length();
  }

  return aFont;
}

/**
 *
 */
void QtxXlsBiff::addDefaultFonts()
{
  BIFF8_FONT aFont = formatFontRecord( myDefaultFontName, myDefaultFontSize );
  //MS Excel requires five font records at least
  for( int i = 0; i < 5; i++ )
  {
    myFontTable.append( aFont );
    myFontNames.append( myDefaultFontName );

  }
}

/**
 * Packs all font records into font table
 */
void QtxXlsBiff::createFontTable()
{
  QByteArray aUnicodeByteName;
  int aCount = myFontTable.size();

  for( int i = 0; i < aCount; i++ )
  {
    if ( myFontTable[i].unicodeStrFlags == Compressed )
    {
      aUnicodeByteName.append( myFontNames[i] );
    }
    else
    {
      aUnicodeByteName = unicodeToByteArray( myFontNames[i] );
    }

    myWbDataBuf.append( BYTE_ARRAY(myFontTable[i].buf, sizeof(BIFF8_FONT)) );
    myWbDataBuf.append( aUnicodeByteName );

    aUnicodeByteName.clear();
  }
}

/**
 * Adds user-defined cell format into the format table
 */
int QtxXlsBiff::addCellStyle( const XlsWORD theFontID,
                              const XlsWORD theFormatID,
                              const XlsWORD theAlign,
                              const XlsWORD theVAlign,
                              const QtxXlsBiff::Border* theBorders,
                              const bool    theIsWrapped,
                              const bool    theIsMerged,
                              const XlsWORD theRotation,
                              const XlsBYTE theForegrountColorID,
                              const XlsBYTE theBackgrountColorID/*,
                              const XlsWORD theParentStyleID*/ )
{
  BIFF8_XF aXF;
  for( int i = 2; i < 24; i++ )
    aXF.buf[i] = 0;

  //Create default style formats
  aXF.biffRecord.biffNum = XF;
  aXF.biffRecord.length  = SIZEOF(BIFF8_XF);
  aXF.fontID             = theFontID > 3 ? theFontID + 1 : theFontID;
  aXF.formatID           = theFormatID;

  aXF.fStyle             = Cell;
  aXF.ixfParent          = NoParentStyle/*theParentStyleID*/;

  aXF.align              = theAlign;
  aXF.valign             = theVAlign;
  aXF.fWrap              = theIsWrapped;
  aXF.rotation           = theRotation;
  aXF.mergeCell          = theIsMerged;


  aXF.foregroundColor    = theForegrountColorID;
  aXF.backgroundColor    = theBackgrountColorID;

  if( theBorders )
  {
    aXF.left               = theBorders->leftBorderStyle;
    aXF.right              = theBorders->rightBorderStyle;
    aXF.top                = theBorders->topBorderStyle;
    aXF.bottom             = theBorders->bottomBorderStyle;

    aXF.colorLeft          = theBorders->leftBorderColor;
    aXF.colorRight         = theBorders->rightBorderColor;
    aXF.colorTop           = theBorders->topBorderColor;
    aXF.colorBottom        = theBorders->bottomBorderColor;

    aXF.diagBdrStyle       = theBorders->diagonalBorderStyle;
    aXF.diagonalType       = theBorders->diagonalType;
    aXF.colorDiagonal      = theBorders->diagonalColor;
  }

  myStyleTable.append( aXF );

  return myStyleTable.size() - 1;
}

/**
 *
 */
void QtxXlsBiff::addDefaultStyles()
{
  BIFF8_XF aXF;
  for( int i = 2; i < 24; i++ )
    aXF.buf[i] = 0;

  //Create default style formats
  aXF.biffRecord.biffNum = XF;
  aXF.biffRecord.length  = SIZEOF(BIFF8_XF);
  aXF.fStyle             = Style;
  aXF.ixfParent          = NoParentStyle;
  aXF.align              = AlignGeneral;
  aXF.valign             = VAlignCenter;
  aXF.foregroundColor    = DefaultForegroundColor;
  aXF.backgroundColor    = DefaultBackgroundColor;

  //Table contains 15 style format records at least
  for( int i = 0; i < 15; i++ )
    myStyleTable.append( aXF );

  //Create default cell formats, 
  aXF.paramFlags         = Cell;
  myStyleTable.append( aXF );
}

/**
 * Packs all cell format records into cell format table
 */
void QtxXlsBiff::createFormatTable()
{
  // Write additional format records here
  QVector<BIFF8_XF>::iterator anIt = myStyleTable.begin();
  while( anIt != myStyleTable.end() )
  {
    myWbDataBuf.append( BYTE_ARRAY((*anIt).buf, sizeof(BIFF8_XF)) );
    ++anIt;
  }
}

/**
 * STYLE record
 */
void QtxXlsBiff::createStyleInfoTable()
{
  //Each style in an Excel workbook, whether built-in
  //or user-defined, requires a style record
  BIFF8_STYLE aStyle;
  aStyle.biffRecord.biffNum = STYLE;
  aStyle.biffRecord.length  = SIZEOF(BIFF8_STYLE);
  aStyle.iXF                = None;
    aStyle.ixfe             = 0;
    aStyle.fBuiltIn         = 1;
  aStyle.builtInStyle       = Normal;
  aStyle.level              = 0xFF;
  myWbDataBuf.append( BYTE_ARRAY(aStyle.buf, sizeof(BIFF8_STYLE)) );
}

/**
 * Stores unicode value of string into byte array
 * ( QString converts unicode char into ASCII char )
 */
QByteArray QtxXlsBiff::unicodeToByteArray( const QString& theStr )
{
  QByteArray aUnicodeByteName;
  xlsWORDExcange anExchanger;

  for( int i = 0; i < theStr.length(); i++ )
  {
    anExchanger.value = theStr[i].unicode();
    aUnicodeByteName.append( anExchanger.buf[0] );
    aUnicodeByteName.append( anExchanger.buf[1] );
  }

  return aUnicodeByteName;
}

/**
 * BOUNDSHEET record contains information about worksheet
 */
void QtxXlsBiff::createBoundsheet( const int theSSTLength )
{
  BIFF8_BOUNDSHEET aBoundsheet;
  aBoundsheet.biffRecord.biffNum = BOUNDSHEET;
  aBoundsheet.biffRecord.length  = SIZEOF(BIFF8_BOUNDSHEET) + mySheetName.length();

  aBoundsheet.hiddenSate         = Visible;
  aBoundsheet.reserved           = None;
  aBoundsheet.sheetType          = WorksheetType;

  aBoundsheet.sheetNameLen       = mySheetName.length();
  aBoundsheet.unicodeStrFlags    = Compressed;

  QByteArray aUnicodeByteName;
  if ( canBePacked( mySheetName ) )
  {
    aUnicodeByteName.append( mySheetName );
  }
  else
  {
    aBoundsheet.unicodeStrFlags    = Uncompressed;
    aBoundsheet.biffRecord.length += mySheetName.length();

    aUnicodeByteName = unicodeToByteArray(mySheetName);
  }

  aBoundsheet.streamStartPos = myWbDataBuf.length() +
                               sizeof(BIFF8_BOUNDSHEET)  + 
                               aUnicodeByteName.length() +
                               sizeof(BIFF8_SST) +
                               theSSTLength +
                               sizeof(BIFF8_RECORD); //EOF

  myWbDataBuf.append( BYTE_ARRAY(aBoundsheet.buf, sizeof(BIFF8_BOUNDSHEET)) );
  myWbDataBuf.append( aUnicodeByteName );
}

/**
 * SST record contains Shared String Table
 */
void QtxXlsBiff::createSSTTable( const QByteArray& theStringTable )
{
  BIFF8_SST aSST;
  aSST.biffRecord.biffNum = SST;
  aSST.biffRecord.length  = SIZEOF(BIFF8_SST) + theStringTable.size();
  aSST.totalNum           = myLabelsNum;
  aSST.uniqueNum          = mySSTTable.size();

  myWbDataBuf.append( BYTE_ARRAY(aSST.buf, sizeof(BIFF8_SST)) );
  myWbDataBuf.append( theStringTable );
}

/**
 * 
 */
void QtxXlsBiff::createMergeCellsRecord()
{
  if ( myMergedCells.isEmpty() )
    return;

  QVector<BIFF8_REF>::iterator anIt = myMergedCells.begin();

  QByteArray aMergedCellsBuf;
  BIFF8_RECORD aRec;
  aRec.biffNum = MERGECELLS;
  aRec.length  = sizeof(XlsWORD) + sizeof(BIFF8_REF) * myMergedCells.size();
  aMergedCellsBuf.append( DWORD_BYTE_ARRAY(aRec.buf) );

  xlsWORDExcange aCount;
  aCount.value = myMergedCells.size();
  aMergedCellsBuf.append( BYTE_ARRAY(aCount.buf, sizeof(XlsWORD)) );

  while( anIt != myMergedCells.end() )
  {
    aMergedCellsBuf.append( BYTE_ARRAY(anIt->buf, sizeof(BIFF8_REF)) );
    ++anIt;
  }

  myShDataBuf.append( aMergedCellsBuf );
}

/**
 * Returns byte array with workbook and worksheet data
 * prepared for writing
 */
QByteArray QtxXlsBiff::getBiffData()
{
  createBOF();
  createWindow1();
  createDate1904();

  createFontTable();
  createFormatTable();
  createStyleInfoTable();

  //SST len determination
  QByteArray aUnicodeByteName;
  QStringList::const_iterator anIt = mySSTTable.begin();
  while( anIt != mySSTTable.end() )
  {
    xlsWORDExcange anExchanger;
    anExchanger.value = anIt->length();
    aUnicodeByteName.append( anExchanger.buf[0] );
    aUnicodeByteName.append( anExchanger.buf[1] );

    if ( canBePacked( *anIt ) )
    {
      aUnicodeByteName.append( Compressed );
      aUnicodeByteName.append( *anIt );
    }
    else
    {
      aUnicodeByteName.append( Uncompressed );
      aUnicodeByteName.append( unicodeToByteArray( *anIt ) );
    }

    ++anIt;
  }

  createBoundsheet( aUnicodeByteName.size() );
  createSSTTable( aUnicodeByteName );

  xlsDWORDExcange anExchanger;
  anExchanger.value = _EOF;

  QByteArray aEOF( TO_PCHAR(anExchanger.buf), sizeof(XlsDWORD) );

  QByteArray aDataBuffer( myWbDataBuf );
  aDataBuffer.append( aEOF );

  createMergeCellsRecord();

  aDataBuffer.append( myShDataBuf );
  aDataBuffer.append( aEOF );

  cleanBuffers();

  return aDataBuffer;
}

/**
 * Adds number value
 */
void QtxXlsBiff::addData( const double theNumber,
                          const XlsWORD theRow,
                          const XlsWORD theCol,
                          const XlsWORD theStyleID )
{
  BIFF8_NUMBER aNumber;
  aNumber.biffRecord.biffNum = NUMBER;
  aNumber.biffRecord.length  = SIZEOF(BIFF8_NUMBER);

  aNumber.row = theRow;
  aNumber.col = theCol;
  aNumber.iXF = theStyleID; //first cell format record having id = 15
  aNumber.num = theNumber;

  myShDataBuf.append( BYTE_ARRAY(aNumber.buf, sizeof(BIFF8_NUMBER)) );
}

/**
 * Adds string or blank value
 */
void QtxXlsBiff::addData( const QString& theString,
                          const XlsWORD  theRow,
                          const XlsWORD  theCol,
                          const XlsWORD  theStyleID )
{
  if ( theString.isEmpty() )
  {
    BIFF8_BLANK aBlank;
    aBlank.biffRecord.biffNum = BLANK;
    aBlank.biffRecord.length  = SIZEOF(BIFF8_BLANK);
    aBlank.row                = theRow;
    aBlank.col                = theCol;
    aBlank.iXF                = theStyleID;

    myShDataBuf.append( BYTE_ARRAY(aBlank.buf, sizeof(BIFF8_BLANK)) );
    return;
  }

  int anIndex = mySSTTable.indexOf( theString );
  
  BIFF8_LABELSST aLabel;
  aLabel.biffRecord.biffNum = LABELSST;
  aLabel.biffRecord.length  = SIZEOF(BIFF8_LABELSST);
  aLabel.row                = theRow;
  aLabel.col                = theCol;
  aLabel.iXF                = theStyleID; //first cell format record having id = 15

  if ( anIndex < 0 )
  {
    mySSTTable.append( theString );
    aLabel.iSST             = mySSTTable.size() - 1;
  }
  else
  {
    aLabel.iSST             = anIndex;
  }

  ++myLabelsNum;
  myShDataBuf.append( BYTE_ARRAY(aLabel.buf, sizeof(BIFF8_LABELSST)) );
}

/**
 *
 */
void QtxXlsBiff::mergeCells( const XlsWORD theFirstRow, const XlsWORD theLastRow,
                             const XlsWORD theFirstCol, const XlsWORD theLastCol )
{
  BIFF8_REF aRef;
  aRef.rowFirst = theFirstRow;
  aRef.rowLast  = theLastRow;
  aRef.colFirst = theFirstCol;
  aRef.colLast  = theLastCol;

  myMergedCells.append( aRef );
}

/**
 *
 */
void QtxXlsBiff::setColWidth( const XlsWORD theWidth, const XlsWORD theFrom,
                              const XlsWORD theTo, const XlsWORD theStyleID )
{
  BIFF8_COLINFO aColinfo;
  aColinfo.biffRecord.biffNum = COLINFO;
  aColinfo.biffRecord.length  = SIZEOF(BIFF8_COLINFO);
  aColinfo.colFirst           = theFrom;
  aColinfo.colLast            = theTo;
  aColinfo.colDx              = 256 * theWidth;
  aColinfo.iXF                = theStyleID;
  aColinfo.options            = None;
  aColinfo.reserved           = None;

  myShDataBuf.append( BYTE_ARRAY(aColinfo.buf, sizeof(BIFF8_COLINFO)) );
}

/**
 *
 */
void QtxXlsBiff::getFontParams( const XlsWORD theStyleID, QString& theFontName,
                                int& theFontSize, bool& theIsBold )
{
  if ( myStyleTable.size() < theStyleID )
  {
    theFontName = "";
    theFontSize = -1;
    theIsBold   = false;
    return;
  }

  int aFontId = myStyleTable[theStyleID].fontID;
  if ( aFontId < MinFontNum + 1 )
  {
    theFontName = myDefaultFontName;
    theFontSize = myDefaultFontSize;
    theIsBold   = false;
    return;
  }

  --aFontId;

  theFontName = myFontNames[aFontId];
  theFontSize = myFontTable[aFontId].height / 20;
  theIsBold   = myFontTable[aFontId].boldStyle == BoldText;
}

/**
 *
 */
void QtxXlsBiff::cleanBorderStruct( QtxXlsBiff::Border& theBorder )
{
  Border aBorder = { None, None, None,
                     None, None, None,
                     None, None, None,
                     None, None };

  theBorder = aBorder;
}
