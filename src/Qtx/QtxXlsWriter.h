// Name   : QtxXlsWriter.h
// Purpose: QtxXlsWriter is a class for storing data to *.XLS file.
//          General behaviour - creating a low-level storage format
//          for *.XLS file.
//
// History:
// 11/10/11 - Oleg Agashin - Creation of the file

#ifndef QTXXLSWRITER_HEADER
#define QTXXLSWRITER_HEADER

#include <QtxXlsBiff.h>

#include <QString>
#include <QByteArray>


#pragma pack(push,1)
/*
 * Class for exporting data to *.XLS file
 * Uses only one data stream
 *
 * *.XLS file structure:
 * +------------------------+
 * |        Header          |
 * +------------------------+
 * |         DATA           |
 * +------------------------+
 * | MSAT or Nothing if     |
 * | number of SAT sectors  |
 * | lower than 109         |
 * +------------------------+
 * | SSAT or Nothing if     |
 * | length of data stream  |
 * | greater than minimum   |
 * | value                  |
 * +------------------------+
 * |    Directory Entry     |
 * |       Structure        |
 * +------------------------+
 * | SAT or Nothing if      |
 * | length of data stream  |
 * | lower than minimum     |
 * | value                  |
 * +------------------------+
 */
class QtxXlsWriter
{
  //-------------- File structure types ------------------------
  enum SectorType { Free_SecID = -1, EndOfChain_SecID = -2,
                    SAT_SecID  = -3, MSAT_SecID       = -4,
                    Dummy      =  0};

  enum EntryType { Empty = 0, UserStorage, UserStream,
                   LockBytes, Property,    Root};

  enum NodeColour { Red = 0, Black };

  enum Constants { SectorSize = 512, MinStreamSize = 4096,
                   DirEntrySize = 128, MinSATSecInMSAT = 109,
                   MaxDirNameLen = 32,
                   SizeOfDWORD = sizeof(XlsDWORD),
                   totalSecIDinSATSector  = SectorSize/SizeOfDWORD,
                   totalSecIDinMSATSector = totalSecIDinSATSector-1 };

  enum AlignType { Byte, Dword };

  //Xls header structure
  union XlsHeader
  {
    XlsBYTE buf[SectorSize];
    struct
    {
      XlsBYTE  fileID[8];
      XlsBYTE  UID[16];
      XlsWORD  formatRev;
      XlsWORD  formatVer;
      XlsWORD  byteOrder;
      XlsWORD  sectorSize;
      XlsWORD  shortSectorSize;
      XlsBYTE  notUsed10[10];
      XlsDWORD totalSATSecs;
      XlsDWORD firstDirSecID;
      XlsDWORD notUsed;
      XlsDWORD minStreamSize;
      XlsDWORD firstSSATSecID;
      XlsDWORD totalSSATSecs;
      XlsDWORD firstMSATSecID;
      XlsDWORD totalMSATSecs;
      XlsBYTE  MSAT[436];
    };
  };

  //Directiry entity structure
  union DirEntityStructure
  {
    XlsBYTE buf[DirEntrySize];
    struct
    {
      XlsWORD  entryNameUnicode[32];
      XlsWORD  nameLenInBytes;
      XlsBYTE  entityType;
      XlsBYTE  nodeColour;
      XlsDWORD leftChildDirID;
      XlsDWORD rightChildDirID;
      XlsDWORD rootDirID;
      XlsDWORD UID[4];
      XlsDWORD userFlags;
      XlsDWORD timeStampCreation[2];
      XlsDWORD timeStampModification[2];
      XlsDWORD firstSecID;
      XlsDWORD totalStreamSize;
      XlsDWORD notUsed;
    };
  };
  //-------------- end of File structure types -----------------

public:
  QtxXlsWriter( const QString& );
  virtual ~QtxXlsWriter();

public:
  void addData( const QString& theData, const XlsWORD theRow,
                const XlsWORD theCol, const XlsWORD theStyleID );
  void addData( const double theData, const XlsWORD theRow,
                const XlsWORD theCol, const XlsWORD theStyleID );

  bool upload();
  void clear();

  void SetWorkbookName( const QString& theName );
  void SetSheetName( const QString& theName );
  void SetFileName( const QString& theName );

  void setColWidth( const XlsWORD theMaxLen, const XlsWORD theFromCol,
                    const XlsWORD theToCol, const XlsWORD theStyleID );

  void mergeCells( const XlsWORD theFirstRow, const XlsWORD theLastRow,
                   const XlsWORD theFirstCol, const XlsWORD theLastCol );

  int addFont(   const QString& theFontName,
                 const XlsWORD theFontSize,
                 const bool theIsBold = false,
                 const XlsWORD theFontColor = QtxXlsBiff::Black,
                 const XlsWORD theFontStyle = QtxXlsBiff::None,
                 const XlsWORD theSuperSubscriptType = QtxXlsBiff::None,
                 const XlsWORD theUnderlineType = QtxXlsBiff::None );

  int addCellStyle( const XlsWORD theFontID,
                    const XlsWORD theFormatID,
                    const XlsWORD theAlign,
                    const XlsWORD theVAlign,
                    const QtxXlsBiff::Border* theBorders = 0,
                    const bool    theIsWrapped = false,
                    const bool    theIsMerged = false,
                    const XlsWORD theRotation = QtxXlsBiff::None,
                    const XlsBYTE theForegrountColorID = QtxXlsBiff::DefaultForegroundColor,
                    const XlsBYTE theBackgrountColorID = QtxXlsBiff::DefaultBackgroundColor );

private:
  QByteArray formatHeader( const QByteArray& theDataBuffer );
  QByteArray formatSAT();
  QByteArray formatMSAT();
  QByteArray formatDirectoryEntry( const XlsDWORD theDataLen );

  void clearDirStructure( DirEntityStructure& theStruct );
  void alignData( QByteArray& theBuffer, XlsDWORD theFiller, AlignType theAlignType = Byte);
  void fillData( QByteArray& );
  XlsDWORD getSectorsNum( const XlsDWORD theDividend, const XlsDWORD theDivisor );
  void fillFirstSectorID( const XlsDWORD theTotalSecNum,
                          XlsDWORD& theFirstSectorID,
                          XlsDWORD& theFreeSector );

private:
  QString    myFileName;
  QString    myWorkbookName;

  QtxXlsBiff myData;

  XlsDWORD   myDataSecNum;
  XlsDWORD   myMSATSecNum;
  XlsDWORD   mySATSecNum;

  XlsDWORD   myFreeSector;
};

#pragma pack(pop)
#endif
