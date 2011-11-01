// Name   : QtxXlsWriter.cpp
// Purpose: Implementation of QtxXlsWriter class.
//
// History:
// 11/10/11 - Oleg Agashin - Creation of the file

#include <QtxXlsWriter.h>

#include <QFile>


/**
 *
 */
QtxXlsWriter::QtxXlsWriter( const QString& theFileName )
: myFileName(theFileName), myWorkbookName("Workbook"),
  myDataSecNum(Dummy), myMSATSecNum(Dummy),
  mySATSecNum(Dummy), myFreeSector(Dummy)
{
}

/**
 *
 */
QtxXlsWriter::~QtxXlsWriter()
{
}

/**
 * Clear all structures
 */
void QtxXlsWriter::clear()
{
  myData.clear();
  myDataSecNum = Dummy;
  myMSATSecNum = Dummy;
  mySATSecNum  = Dummy;
  myFreeSector = Dummy;
}

/**
 * Forms header of xls file
 */
QByteArray QtxXlsWriter::formatHeader( const QByteArray& theDataBuffer )
{
  //Data aligned to SectorSize boundary
  XlsDWORD aDataLen = theDataBuffer.length();

  mySATSecNum  = Dummy;
  XlsDWORD aTotalSSATSecNum = Dummy;
  myMSATSecNum = Dummy;

  //Are there some data in buffer?
  //if ( aDataLen ) 
  //
  //rem: BOF & EOF data are always there and aDataLen >= SectorSize,
  //     nothing to check

  myDataSecNum = aDataLen / SectorSize; //Total data sectors

  //If data length below a lower boundary
  if ( aDataLen < MinStreamSize )
  {
    //format SSAT
    aTotalSSATSecNum = 1;
  }
  else
  {
    //format SAT
    mySATSecNum = getSectorsNum( myDataSecNum, totalSecIDinSATSector);

    //check to add additional MSAT
    if ( mySATSecNum > MinSATSecInMSAT )
    {
      //format MSAT
      myMSATSecNum = getSectorsNum( mySATSecNum - MinSATSecInMSAT, totalSecIDinMSATSector );
    }
  }


  XlsHeader aHeader = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};

  for( XlsWORD i = 8; i < 76; i++ )
    aHeader.buf[i] = Dummy;

  aHeader.formatRev = 0x003E;
  aHeader.formatVer = 0x0003;
  aHeader.byteOrder = 0xFFFE;

  aHeader.sectorSize      = 9;
  aHeader.shortSectorSize = 6;

  aHeader.minStreamSize   = MinStreamSize;

  aHeader.totalSATSecs    = mySATSecNum;
  aHeader.totalSSATSecs   = aTotalSSATSecNum;
  aHeader.totalMSATSecs   = myMSATSecNum;

  myFreeSector = myDataSecNum;

  fillFirstSectorID( myMSATSecNum,     aHeader.firstMSATSecID, myFreeSector );
  fillFirstSectorID( aTotalSSATSecNum, aHeader.firstSSATSecID, myFreeSector );
  aHeader.firstDirSecID = myFreeSector++;

  XlsWORD anIndex = 76;

  XlsWORD aBound = mySATSecNum;
  if ( myMSATSecNum )
  {
    aBound = MinSATSecInMSAT;
  }

  for( XlsWORD i = 0; i < aBound; i++)
  {
    *(reinterpret_cast<XlsDWORD*>(&aHeader.buf[anIndex])) = myFreeSector++;

    anIndex += SizeOfDWORD;
  }
  
  //If we have a reminder
  for(; anIndex < SectorSize; anIndex++)
    aHeader.buf[anIndex] = Free_SecID;

  return BYTE_ARRAY( aHeader.buf, SectorSize );
}

/**
 * Calculates first sector ID for some structure block
 */
void QtxXlsWriter::fillFirstSectorID( const XlsDWORD theTotalSecNum,
                                      XlsDWORD& theFirstSectorID,
                                      XlsDWORD& theFreeSector )
{
  if ( theTotalSecNum )
  {
    theFirstSectorID = theFreeSector;
    theFreeSector += theTotalSecNum;
  }
  else
    theFirstSectorID = EndOfChain_SecID;
}

/**
 * Update directory structure by default values
 */
void QtxXlsWriter::clearDirStructure( DirEntityStructure& theStruct )
{
  for( XlsWORD i = 0; i < DirEntrySize; i++ )
    theStruct.buf[i] = Dummy;

  theStruct.leftChildDirID  = Free_SecID;
  theStruct.rightChildDirID = Free_SecID;
  theStruct.rootDirID       = Free_SecID;
}

/**
 * Adds user-defined font 
 */
int QtxXlsWriter::addFont( const QString& theFontName,
                           const XlsWORD theFontSize,
                           const bool theIsBold,
                           const XlsWORD theFontColor,
                           const XlsWORD theFontStyle,
                           const XlsWORD theSuperSubscriptType,
                           const XlsWORD theUnderlineType )
{
  return myData.addFont( theFontName, theFontSize, theIsBold,
                         theFontColor, theFontStyle,
                         theSuperSubscriptType,
                         theUnderlineType );
}

/**
 * Adds user-defined format 
 */
int QtxXlsWriter::addCellStyle( const XlsWORD theFontID,
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
  return myData.addCellStyle( theFontID, theFormatID, theAlign,
                           theVAlign, theBorders, theIsWrapped, theIsMerged,
                           theRotation, theForegrountColorID, theBackgrountColorID );
}

/**
 *
 */
void QtxXlsWriter::SetWorkbookName( const QString& theName )
{
  if( !theName.isEmpty() )
    myWorkbookName = theName;
}

/**
 *
 */
void QtxXlsWriter::SetSheetName( const QString& theName )
{
  myData.SetSheetName( theName );
}

/**
 *
 */
void QtxXlsWriter::SetFileName( const QString& theName )
{
  if( !theName.isEmpty() )
    myFileName = theName;
}

/**
 * Stores the file
 */
bool QtxXlsWriter::upload()
{
  QFile aFile( myFileName );
  if ( !aFile.open( QFile::WriteOnly ) )
    return false;

  QByteArray aDataBuffer( myData.getBiffData() );
  fillData( aDataBuffer );

  //counting data sectors

  QByteArray aByteArray = formatHeader( aDataBuffer );
  aFile.write( aByteArray );

  aFile.write( aDataBuffer );
  
  aByteArray = formatMSAT();
  aFile.write( aByteArray );

  aByteArray = formatDirectoryEntry( aDataBuffer.length() );
  aFile.write( aByteArray );

  aByteArray = formatSAT();
  aFile.write( aByteArray );

  aFile.close();

  return true;
}

/**
 *
 */
void QtxXlsWriter::fillData( QByteArray& theDataBuffer )
{
  alignData( theDataBuffer, Dummy );

  //Workaround of short-stream containers
  while( theDataBuffer.length() < MinStreamSize )
    theDataBuffer.append( Dummy );
}

/**
 * Aligns data to SectorSize boundary
 * by inserting Dummy data
 */
void QtxXlsWriter::alignData( QByteArray& theBuffer,
                              XlsDWORD theFiller,
                              QtxXlsWriter::AlignType theAlignType )
{
  int aPackLen = SectorSize - (theBuffer.length() % SectorSize);
  if ( aPackLen != SectorSize )
  {
    for( int i = 0; i < aPackLen; )
    {
      switch ( theAlignType )
      {
      case Byte:
        theBuffer.append( theFiller );
        i++;
        break;
      
      case Dword:
        xlsDWORDExcange anExchanger;

        anExchanger.value = theFiller;
        theBuffer.append( DWORD_BYTE_ARRAY(anExchanger.buf) );
        i += SizeOfDWORD;
        break;
      }
    }
  }
}

/**
 * Returns exact number of sectors that can storing all data
 */
XlsDWORD QtxXlsWriter::getSectorsNum( const XlsDWORD theDividend, const XlsDWORD theDivisor )
{
  XlsDWORD aSecNum = theDividend / theDivisor;
  if ( theDividend % theDivisor )
    ++aSecNum;
  return aSecNum;
}

/**
 * Fills the SAT sectors by reference data sectors IDs
 */
QByteArray QtxXlsWriter::formatSAT()
{
  QByteArray aBuffer;
  xlsDWORDExcange anExchanger;
  XlsDWORD aPenultimate = myDataSecNum - 1;

  //Fill data chain
  for( XlsDWORD anIndex = 0; anIndex < myDataSecNum; anIndex++)
  {
    if ( anIndex == aPenultimate )
      anExchanger.value = EndOfChain_SecID;
    else
      anExchanger.value = anIndex + 1;

    aBuffer.append( DWORD_BYTE_ARRAY(anExchanger.buf) );
  }

  //Fill MSAT used sectors
  for( XlsDWORD anIndex = 0; anIndex < myMSATSecNum; anIndex++)
  {
    anExchanger.value = MSAT_SecID;
    aBuffer.append( DWORD_BYTE_ARRAY(anExchanger.buf) );
  }

  //Fill SSAT

  //Fill Dir - only one sector
  anExchanger.value = EndOfChain_SecID;
  aBuffer.append( DWORD_BYTE_ARRAY(anExchanger.buf) );

  //Fill SAT used sectors
  for( XlsDWORD anIndex = 0; anIndex < mySATSecNum; anIndex++)
  {
    anExchanger.value = SAT_SecID;
    aBuffer.append( DWORD_BYTE_ARRAY(anExchanger.buf) );
  }

  alignData( aBuffer, Free_SecID, Dword );

  return aBuffer;
}

/**
 * Fills the MSAT sectors by reference SAT sectors IDs
 */
QByteArray QtxXlsWriter::formatMSAT()
{
  QByteArray aBuffer;

  if ( !myMSATSecNum )
  {
    return aBuffer;
  }

  xlsDWORDExcange anExchanger;
  XlsDWORD aRemSATSectors = mySATSecNum - MinSATSecInMSAT;
  XlsDWORD aNextMSATSec   = myDataSecNum + 1;

  for( XlsDWORD anIndex = 1; anIndex <= aRemSATSectors; anIndex++)
  {
    if ( !( anIndex % totalSecIDinSATSector ) )
    {
      anExchanger.value = aNextMSATSec++;
      aBuffer.append( DWORD_BYTE_ARRAY(anExchanger.buf) );
    }

    anExchanger.value = myFreeSector++;
    aBuffer.append( DWORD_BYTE_ARRAY(anExchanger.buf) );
  }

  alignData( aBuffer, Free_SecID, Dword );

  return aBuffer;
}

/**
 * Fills entry point for workbook
 */
QByteArray QtxXlsWriter::formatDirectoryEntry( const XlsDWORD theDataLen )
{
  QString aName;
  QByteArray aBuffer;
  DirEntityStructure anEntity;

  clearDirStructure( anEntity );

  aName = "Root Entry";
  
  XlsDWORD aStrLen = aName.length();
  for ( XlsDWORD i = 0; i < aStrLen ; i++ )
    anEntity.entryNameUnicode[i] = aName[i].unicode();

  anEntity.nameLenInBytes  = (aStrLen + 1) << 1;
  anEntity.entityType      = Root;
  anEntity.nodeColour      = Black; //Red;
  anEntity.rootDirID       = 1; //only one stream, this points to next dir entry
  anEntity.firstSecID      = EndOfChain_SecID;
  anEntity.totalStreamSize = Dummy;

  anEntity.UID[0] = 0x00020820;
  anEntity.UID[1] = 0x00000000;
  anEntity.UID[2] = 0x000000C0;
  anEntity.UID[3] = 0x46000000;

  //Add root entry to buffer
  aBuffer.append( BYTE_ARRAY(anEntity.buf, DirEntrySize) );


  clearDirStructure( anEntity );
  
  aStrLen = myWorkbookName.length();
  if ( aStrLen > MaxDirNameLen )
  {
    myWorkbookName.truncate( MaxDirNameLen );
    aStrLen = MaxDirNameLen;
  }

  for ( XlsDWORD i = 0; i < aStrLen ; i++ )
    anEntity.entryNameUnicode[i] = myWorkbookName[i].unicode();

  anEntity.nameLenInBytes  = (aStrLen + 1) << 1;
  anEntity.entityType      = UserStream;
  anEntity.nodeColour      = Black;
  anEntity.firstSecID      = 0;
  anEntity.totalStreamSize = theDataLen;

  //Add root entry to buffer
  aBuffer.append( BYTE_ARRAY(anEntity.buf, DirEntrySize) );

  alignData( aBuffer, Dummy );

  return aBuffer;
}

/**
 * Adds string value
 */
void QtxXlsWriter::addData( const QString& theData, const XlsWORD theRow,
                            const XlsWORD theCol, const XlsWORD theStyleID )
{
	bool aIsDouble = false;
	double aDouble = theData.toDouble( &aIsDouble );
	if ( aIsDouble )
		myData.addData( aDouble, theRow, theCol, theStyleID );
	else
		myData.addData( theData, theRow, theCol, theStyleID );

}

/**
 * Adds double value
 */
void QtxXlsWriter::addData( const double theData, const XlsWORD theRow,
                            const XlsWORD theCol, const XlsWORD theStyleID )
{
  myData.addData( theData, theRow, theCol, theStyleID );
}

/**
 *
 */
void QtxXlsWriter::setColWidth( const XlsWORD theMaxLen, const XlsWORD theFromCol,
                                const XlsWORD theToCol, const XlsWORD theStyleID )
{
  myData.setColWidth( theMaxLen + 1, theFromCol,
                      theToCol, theStyleID );
}

/**
 *
 */
void QtxXlsWriter::mergeCells( const XlsWORD theFirstRow, const XlsWORD theLastRow,
                               const XlsWORD theFirstCol, const XlsWORD theLastCol )
{
  myData.mergeCells( theFirstRow, theLastRow, theFirstCol, theLastCol );
}
