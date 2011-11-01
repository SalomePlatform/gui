// Name   : QtxXlsBiff.h
// Purpose: QtxXlsBiff is a class for storing main data to high-level *.XLS structures.
//
// History:
// 17/10/11 - Oleg Agashin - Creation of the file

#ifndef QTXXLSBIFF_HEADER
#define QTXXLSBIFF_HEADER

#include <QString>
#include <QStringList>
#include <QVector>

//Own types
typedef unsigned char  XlsBYTE;
typedef unsigned short XlsWORD;
typedef unsigned int   XlsDWORD;

#define TO_PCHAR(x) reinterpret_cast<char*>(x)
#define BYTE_ARRAY(x,y) QByteArray(TO_PCHAR(x), (y))
#define DWORD_BYTE_ARRAY(x) BYTE_ARRAY((x), sizeof(XlsDWORD))

union xlsWORDExcange
{
  XlsBYTE  buf[sizeof(XlsWORD)];
  XlsWORD value;
};

union xlsDWORDExcange
{
  XlsBYTE  buf[sizeof(XlsDWORD)];
  XlsDWORD value;
};


#pragma pack(push,1)

#define SIZEOF(x) (sizeof(x) - sizeof(BIFF8_RECORD))

/**
 * Class for creating data model
 *
 * Format of *.XLS data model:
 * +----------------------------+
 * | BOF                        |
 * +----------------------------+
 * | Workbook Globals           |
 * +----------------------------+
 * | EOF                        |
 * +----------------------------+
 * +----------------------------+
 * | BOF                        |
 * +----------------------------+
 * | Sheet stream               |
 * |    getBiffBeforeData       |
 * |      #Data                 |
 * |    getBiffAfterData        |
 * +----------------------------+
 * | EOF                        |
 * +----------------------------+
 */
class QtxXlsBiff
{
  //-------------- BIFF structure types ------------------------

  union BIFF8_RECORD
  {
    XlsBYTE buf[4];
    struct
    {
      XlsWORD biffNum;
      XlsWORD length;
    };
  };

  union BIFF8_BOF
  {
    XlsBYTE buf[20];
    struct
    {
      BIFF8_RECORD  biffRecord;
      XlsWORD       verOfBIFF;
      XlsWORD       typeOfData;
      XlsWORD       buildID;
      XlsWORD       buildYear;
      XlsDWORD      historyFlags;
      XlsDWORD      lowestVer;
    };
  };

  union BIFF8_DATE1904
  {
    XlsBYTE buf[6];
    struct
    {
      BIFF8_RECORD biffRecord;
      XlsWORD      f1904;
    };
  };

  union BIFF8_XF
  {
    XlsBYTE buf[24];
    struct
    {
      BIFF8_RECORD  biffRecord;
      XlsWORD       fontID;
      XlsWORD       formatID;
      union
      {
        XlsWORD     paramFlags;
        struct
        {
          XlsWORD fLocked    : 1;
          XlsWORD fHidden    : 1;
          XlsWORD fStyle     : 1;
          XlsWORD f123Prefix : 1;
          XlsWORD ixfParent  : 12;
        };
      };
      union
      {
        XlsWORD     formatAlign;
        struct
        {
          XlsWORD align     : 3;
          XlsWORD fWrap     : 1;
          XlsWORD valign    : 3;
          XlsWORD fJustLast : 1;
          XlsWORD rotation  : 8;
        };
      };
      union
      {
        XlsWORD     formatView;
        struct
        {
          XlsWORD indent      : 4;
          XlsWORD shrinkToFit : 1;
          XlsWORD mergeCell   : 1;
          XlsWORD readOrder   : 2;
          XlsWORD reserved1   : 2;
          XlsWORD attrNum     : 1;
          XlsWORD attrFnt     : 1;
          XlsWORD attrAlign   : 1;
          XlsWORD attrBdr     : 1;
          XlsWORD attrPat     : 1;
          XlsWORD attrProt    : 1;
        };
      };
      union
      {
        XlsWORD     formatBorder;
        struct
        {
          XlsWORD left   : 4;
          XlsWORD right  : 4;
          XlsWORD top    : 4;
          XlsWORD bottom : 4;
        };
      };
      union
      {
        XlsWORD     formatLRBdrColor;
        struct
        {
          XlsWORD colorLeft    : 7;
          XlsWORD colorRight   : 7;
          XlsWORD diagonalType : 2;
        };
      };
      union
      {
        XlsDWORD     formatTBBdrColor;
        struct
        {
          XlsDWORD colorTop      : 7;
          XlsDWORD colorBottom   : 7;
          XlsDWORD colorDiagonal : 7;
          XlsDWORD diagBdrStyle  : 4;
          XlsDWORD hasXFExt      : 1;
          XlsDWORD fillPattern   : 6;
        };
      };
      union
      {
        XlsWORD     formatBGColor;
        struct
        {
          XlsWORD foregroundColor : 7;
          XlsWORD backgroundColor : 7;
          XlsWORD sxButton        : 1;
          XlsWORD reserved2       : 1;
        };
      };
    };
  };

  union BIFF8_FONT
  {
    //XlsBYTE buf[19];
    XlsBYTE buf[20];
    struct
    {
      BIFF8_RECORD biffRecord;
      XlsWORD      height;
      union
      {
        XlsWORD    fontAttr;
        struct
        {
          XlsWORD reserved1  : 1;
          XlsWORD fItalic    : 1;
          XlsWORD reserved2  : 1;
          XlsWORD fStrikeout : 1;
          XlsWORD fOutline   : 1;
          XlsWORD fShadow    : 1;
          XlsWORD reserved3  : 2;
          XlsWORD unused     : 8;
        };
      };
      XlsWORD      color;
      XlsWORD      boldStyle;
      XlsWORD      superSubStyle;
      XlsBYTE      underlineStyle;
      XlsBYTE      fontFamily;
      XlsBYTE      charSet;
      XlsBYTE      reserved; //zero
      XlsBYTE      fontNameLen;
      XlsBYTE      unicodeStrFlags;
      //+ font name
    };
  };

  union BIFF8_BOUNDSHEET
  {
    XlsBYTE buf[12];
    struct
    {
      BIFF8_RECORD biffRecord;
      XlsDWORD streamStartPos;
      union
      {
        XlsWORD  sheetAttr;
        struct
        {
          XlsWORD hiddenSate : 2;
          XlsWORD reserved   : 6;
          XlsWORD sheetType  : 8;
        };
      };
      XlsBYTE  sheetNameLen;
      XlsBYTE  unicodeStrFlags;
      //+ sheet name
    };
  };

  union BIFF8_STYLE
  {
    XlsBYTE buf[8];
    struct
    {
      BIFF8_RECORD biffRecord;
      union
      {
        XlsWORD    iXF;
        struct
        {
          XlsWORD ixfe     : 12;
          XlsWORD unused   : 3;
          XlsWORD fBuiltIn : 1;
        };
      };
      XlsBYTE      builtInStyle;
      XlsBYTE      level; 
    };
  };

  union BIFF8_SST
  {
    XlsBYTE buf[12];
    struct
    {
      BIFF8_RECORD biffRecord;
      XlsDWORD     totalNum;
      XlsDWORD     uniqueNum;
    };
  };

  union BIFF8_ROW
  {
    XlsBYTE buf[20];
    struct
    {
      BIFF8_RECORD biffRecord;
      XlsWORD      rowNum;
      XlsWORD      firstCol;
      XlsWORD      lastCol;
      XlsWORD      rowHeight;
      XlsWORD      irwMac;
      XlsWORD      reserved;
      union
      {
        XlsWORD    optionFlags;
        struct
        {
          XlsWORD  fOutlineLevel: 3;
          XlsWORD  reserved1    : 1;
          XlsWORD  fCollapsed   : 1;
          XlsWORD  fDyZero      : 1;
          XlsWORD  fUnsynced    : 1;
          XlsWORD  fGhostDirty  : 1;
          XlsWORD  reserved2    : 8;
        };
      };
      XlsWORD      iXF;
    };
  };

  union BIFF8_WINDOW1
  {
    XlsBYTE buf[22];
    struct
    {
      BIFF8_RECORD biffRecord;
      XlsWORD      xWnd;
      XlsWORD      yWnd;
      XlsWORD      dxWnd;
      XlsWORD      dyWnd;
      union
      {
        XlsWORD    optionFlags;
        struct
        {
          XlsWORD  fHidden        : 1;
          XlsWORD  fIconic        : 1;
          XlsWORD  reserved1      : 1;
          XlsWORD  fDspHScroll    : 1;
          XlsWORD  fDspVScroll    : 1;
          XlsWORD  fDisplayWbTabs : 1;
          XlsWORD  fNoAFDateGroup : 1;
          XlsWORD  reserved2      : 1;
          XlsWORD  reserved3      : 8;
        };
      };
      XlsWORD      iSelWbTab;
      XlsWORD      iFirstDispWbTab;
      XlsWORD      selTabNum;
      XlsWORD      wTabRatio;
    };
  };

  union BIFF8_COLINFO
  {
    XlsBYTE buf[16];
    struct
    {
      BIFF8_RECORD biffRecord;
      XlsWORD      colFirst;
      XlsWORD      colLast;
      XlsWORD      colDx;
      XlsWORD      iXF;
      XlsWORD      options;
      XlsWORD      reserved;
    };
  };

  union BIFF8_REF
  {
    XlsBYTE buf[8];
    struct  
    {
      XlsWORD rowFirst;
      XlsWORD rowLast;
      XlsWORD colFirst;
      XlsWORD colLast;
    };
  };

  //------------------- Excel data types -------------------
  union BIFF8_NUMBER
  {
    XlsBYTE buf[18];
    struct
    {
      BIFF8_RECORD  biffRecord;
      XlsWORD       row;
      XlsWORD       col;
      XlsWORD       iXF;
      double        num;
    };
  };

  union BIFF8_BLANK
  {
    XlsBYTE buf[10];
    struct
    {
      BIFF8_RECORD  biffRecord;
      XlsWORD       row;
      XlsWORD       col;
      XlsWORD       iXF;
    };
  };

  union BIFF8_LABELSST
  {
    XlsBYTE buf[14];
    struct
    {
      BIFF8_RECORD  biffRecord;
      XlsWORD       row;
      XlsWORD       col;
      XlsWORD       iXF;
      XlsDWORD      iSST;
    };
  };
  //-------------- end of Excel data types -----------------

  enum TypeOfData { WorkbookGlobals = 0x5, VB = 0x6,
                    Sheet = 0x10, Chart = 0x20,
                    MacroSheet = 0x40,
                    Workspace = 0x100 };

  enum TypeOfXF { Cell = 0, Style };

  enum ReadingOrder { Context = 0, LeftToRight, RightToLeft }; 

  enum OtherFormatConstants { NoTransKeys = 0, DefaultParentStyle = 0, NoParentStyle = 0xFFF };

  enum SheetHiddenState { Visible = 0, Hidden, VeryHidden };
  
  enum SheetType { WorksheetType = 0, MacroType, ChartType, VBType = 6};

  enum BuiltInSyles { Normal = 0, RowLevel_n, ColLevel_n, Comma,
                      Currency, Percent, Comma_0, Currency_0 };

  enum Constants { MinFontNum = 5, MinXFNum = 15 };

  enum StringFlags { Compressed = 0, Uncompressed };

  enum FormatNumbers { _BOF   = 0x0809,
                       _EOF   = 0x000A,
                       WINDOW1  = 0x003d,
                       DATE1904 = 0x0022,
                       FONT = 0x0031,
                       BOUNDSHEET = 0x0085,
                       NUMBER = 0x0203,
                       BLANK  = 0x0201,
                       LABELSST = 0x00FD,
                       XF     = 0x00E0,
                       SST    = 0x00FC,
                       ROW    = 0x0208,
                       MERGECELLS = 0x00E5,
                       COLINFO = 0x007D,
                       STYLE  = 0x0293,
                       BIFF_ver = 0x0600 };
  //-------------- end of BIFF structure types -----------------
public:
  enum Align { AlignGeneral = 0, AlignLeft, AlignCenter, AlignRight,
               AlignFill, AlignJustify, AlignCenterAcrossSelection };

  enum VAlign { VAlignTop = 0, VAlignCenter, VAlignBottom, VAlignJustify };

  enum LineStyles { None = 0, Thin, Medium, Dashed,
                    Dotted, Thick, Double, Hair,
                    MediumDashed, DashDot, MediumDashDot,
                    DashDotDot, MediumDashDotDot,
                    SlantedDashDot,
                    DiagDown = 1, DiagUp = 2, DiagBoth = 3};

  enum FontStyle { Italic = 0x2, Strikeout = 0x8, Outline = 0x10,
                   Shadow = 0x20, Superscript = 1, Subscript = 2 };

  enum FontWeight { NormalText = 0x190, BoldText = 0x2BC };

  enum FontUnderlineType { SingleUnderline = 1, DoubleUnderline, 
                           SingleAccountingUnderline = 0x21, DoubleAccountingUnderline = 0x22 };

  enum InternalFormat { General = 0, Integer }; //0x31 total - see XF reference

  enum DefaultGroundColors { DefaultForegroundColor = 0x40, DefaultBackgroundColor };

  enum DefaultFontColor { Black = 0x7FFF };

  enum Font { DefaultFont = 0 };
  
  enum Style { DefaultStyle = 15 };

  enum CharSet { ANSI_Latin = 0, SystemDefault, Symbol };

  struct Border
  {
    LineStyles leftBorderStyle;
    LineStyles rightBorderStyle;
    LineStyles topBorderStyle;
    LineStyles bottomBorderStyle;

    LineStyles diagonalType;
    LineStyles diagonalBorderStyle;

    XlsBYTE    leftBorderColor;
    XlsBYTE    rightBorderColor;
    XlsBYTE    topBorderColor;
    XlsBYTE    bottomBorderColor;
    XlsBYTE    diagonalColor;
  };

public:
  QtxXlsBiff();

public:
  QByteArray getBiffData();
  void clear();

  static void cleanBorderStruct( Border& theBorder );

  void SetSheetName( const QString& theName );

  void setColWidth( const XlsWORD theWidth, const XlsWORD theFrom,
                    const XlsWORD theTo,const XlsWORD theStyleID );

  void getFontParams( const XlsWORD theStyleID, QString& theFontName,
                      int& theFontSize, bool& theIsBold );

  void addData(  const double  theNumber,
                 const XlsWORD theRow,
                 const XlsWORD theCol,
                 const XlsWORD theStyleID = 0 );

  void addData(  const QString& theString,
                 const XlsWORD  theRow,
                 const XlsWORD  theCol,
                 const XlsWORD  theStyleID = 0 );

  int addFont(   const QString& theFontName,
                 const XlsWORD theFontSize,
                 const bool theIsBold = false,
                 const XlsWORD theFontColor = Black,
                 const XlsWORD theFontStyle = None,
                 const XlsWORD theSuperSubscriptType = None,
                 const XlsWORD theUnderlineType = None );

  int addCellStyle( const XlsWORD theFontID,
                    const XlsWORD theFormatID,
                    const XlsWORD theAlign,
                    const XlsWORD theVAlign,
                    const Border* theBorders = 0,
                    const bool    theIsWrapped = false,
                    const bool    theIsMerged = false,
                    const XlsWORD theRotation = None,
                    const XlsBYTE theForegrountColorID = DefaultForegroundColor,
                    const XlsBYTE theBackgrountColorID = DefaultBackgroundColor );

  void mergeCells( const XlsWORD theFirstRow, const XlsWORD theLastRow,
                   const XlsWORD theFirstCol, const XlsWORD theLastCol );

private:
  bool canBePacked( const QString& theStr );
  QByteArray unicodeToByteArray( const QString& theStr );
  void cleanBuffers();
  void addDefaultFonts();
  void addDefaultStyles();

  void createBOF();
  void createWindow1();
  void createDate1904();
  void createFontTable();
  void createFormatTable();
  void createStyleInfoTable();
  void createBoundsheet( const int theSSTLength );
  void createSSTTable( const QByteArray& theStringTable );
  void createMergeCellsRecord();

  BIFF8_FONT formatFontRecord( const QString& theFontName,
                               const XlsWORD theFontSize,
                               const bool theIsBold = false,
                               const XlsWORD theFontColor = Black,
                               const XlsWORD theFontStyle = None,
                               const XlsWORD theSuperSubscriptType = None,
                               const XlsWORD theUnderlineType = None );
private:
  QString                 myDefaultFontName;
  int                     myDefaultFontSize;

  QString                 mySheetName;

  QByteArray              myWbDataBuf;
  QByteArray              myShDataBuf;

  QVector<BIFF8_FONT>     myFontTable;
  QStringList             myFontNames;

  QVector<BIFF8_XF>       myStyleTable;
  QVector<BIFF8_REF>      myMergedCells;

  QStringList             mySSTTable;
  XlsDWORD                myLabelsNum;

  QVector<BIFF8_BLANK>    myBlankCells;
};

#pragma pack(pop)
#endif
