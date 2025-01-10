// Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File:      QtxResourceMgr.h
// Author:    Alexander SOLOVYOV, Sergey TELKOV
//
#ifndef QTXRESOURCEMGR_H
#define QTXRESOURCEMGR_H

#include "Qtx.h"

#ifndef QTX_NO_INDEXED_MAP
#include "QtxMap.h"
#endif

#include <QMap>
#include <QList>
#include <QFont>
#include <QColor>
#include <QPixmap>
#include <QByteArray>
#include <QStringList>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QConicalGradient>

class QTranslator;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxResourceMgr
{
  class IniFormat;
  class XmlFormat;
  class JsonFormat;
  class SalomexFormat;
  class Resources;

public:
  class Format;

#ifdef QTX_NO_INDEXED_MAP
  typedef QMap<QString, QString> Section;   //!< resource section
#else
  typedef IMap<QString, QString> Section;   //!< resource section
#endif

  //! Working mode; defines a way how resource manager handles user preferences
  typedef enum {
    AllowUserValues,       //!< User values are processed by the resource manager
    IgnoreUserValues       //!< User values are ignored by the resource manager
  } WorkingMode;

public:
  QtxResourceMgr();
  QtxResourceMgr( const QString&, const QString& = QString() );
  virtual ~QtxResourceMgr();

  QString          appName() const;
  QStringList      dirList() const;

  bool             checkExisting() const;
  virtual void     setCheckExisting( const bool );

  bool             isPixmapCached() const;
  void             setIsPixmapCached( const bool );

  void             clear();

  WorkingMode      workingMode() const;
  WorkingMode      setWorkingMode( WorkingMode );

  bool             value( const QString&, const QString&, int& ) const;
  bool             value( const QString&, const QString&, double& ) const;
  bool             value( const QString&, const QString&, bool& ) const;
  bool             value( const QString&, const QString&, QColor& ) const;
  bool             value( const QString&, const QString&, QFont& ) const;  
  bool             value( const QString&, const QString&, QByteArray& ) const;  
  bool             value( const QString&, const QString&, QLinearGradient& ) const;  
  bool             value( const QString&, const QString&, QRadialGradient& ) const;  
  bool             value( const QString&, const QString&, QConicalGradient& ) const;  
  bool             value( const QString&, const QString&, Qtx::BackgroundData& ) const;  
  bool             value( const QString&, const QString&, QString&, const bool = true ) const;

  int              integerValue( const QString&, const QString&, const int = 0 ) const;
  double           doubleValue( const QString&, const QString&, const double = 0 ) const;
  bool             booleanValue( const QString&, const QString&, const bool = false ) const;
  QFont            fontValue( const QString&, const QString&, const QFont& = QFont() ) const;
  QColor           colorValue( const QString&, const QString&, const QColor& = QColor() ) const;
  QString          stringValue( const QString&, const QString&, const QString& = QString(), const bool = true ) const;
  QByteArray       byteArrayValue( const QString&, const QString&, const QByteArray& = QByteArray() ) const;
  QLinearGradient  linearGradientValue( const QString&, const QString&, const QLinearGradient& = QLinearGradient() ) const;
  QRadialGradient  radialGradientValue( const QString&, const QString&, const QRadialGradient& = QRadialGradient() ) const;
  QConicalGradient conicalGradientValue( const QString&, const QString&, const QConicalGradient& = QConicalGradient() ) const;
  Qtx::BackgroundData backgroundValue( const QString&, const QString&, const Qtx::BackgroundData& = Qtx::BackgroundData() ) const;

  bool             hasSection( const QString& ) const;
  bool             hasValue( const QString&, const QString& ) const;

  void             setValue( const QString&, const QString&, const int );
  void             setValue( const QString&, const QString&, const double );
  void             setValue( const QString&, const QString&, const bool );
  void             setValue( const QString&, const QString&, const QFont& );
  void             setValue( const QString&, const QString&, const QColor& );
  void             setValue( const QString&, const QString&, const QString& );
  void             setValue( const QString&, const QString&, const QByteArray& );
  void             setValue( const QString&, const QString&, const QLinearGradient& );
  void             setValue( const QString&, const QString&, const QRadialGradient& );
  void             setValue( const QString&, const QString&, const QConicalGradient& );
  void             setValue( const QString&, const QString&, const Qtx::BackgroundData& );

  void             remove( const QString& );
  void             remove( const QString&, const QString& );

  QString          currentFormat() const;
  void             setCurrentFormat( const QString& );

  Format*          format( const QString& ) const;
  void             installFormat( Format* );
  void             removeFormat( Format* );

  QStringList      options() const;
  QString          option( const QString& ) const;
  void             setOption( const QString&, const QString& );

  QStringList      constants() const;
  QString          constant( const QString& ) const;
  void             setConstant( const QString&, const QString& );

  QPixmap          defaultPixmap() const;
  virtual void     setDefaultPixmap( const QPixmap& );

  QString          resSection() const;
  QString          langSection() const;
  QString          sectionsToken() const;

  QPixmap          loadPixmap( const QString&, const QString& ) const;
  QPixmap          loadPixmap( const QString&, const QString&, const bool ) const;
  QPixmap          loadPixmap( const QString&, const QString&, const QPixmap& ) const;
  void             loadLanguage( const QString& = QString(), const QString& = QString() );

  void             raiseTranslators( const QString& );
  void             removeTranslators( const QString& );
  void             loadTranslator( const QString&, const QString& );
  void             loadTranslators( const QString&, const QStringList& );
  void             addTranslator( const QString&, QTranslator* );

  QString          path( const QString&, const QString&, const QString& ) const;

  bool             load();
  bool             import( const QString& );
  bool             save();
  bool             addResource( const QString& );

  QStringList      sections() const;
  QStringList      sections(const QRegExp&) const;
  QStringList      sections(const QStringList&) const;
  QStringList      subSections(const QString&, const bool = true) const;
  QStringList      parameters( const QString& ) const;
  QStringList      parameters( const QStringList& ) const;

  void             refresh();

  QString          language( const QString& = QString() ) const;

protected:
  virtual QString  defaultLanguage() const;

  virtual void     setDirList( const QStringList& );
  virtual void     setResource( const QString&, const QString&, const QString& );

  virtual QString  userFileName( const QString&, const bool = true ) const;
  virtual QString  globalFileName( const QString& ) const;

  virtual void     saved();

private:
  void             initialize( const bool = true ) const;
  QString          substMacro( const QString&, const QMap<QChar, QString>& ) const;

private:
  typedef QList<Resources*>        ResList;
  typedef QList<QTranslator*>      TransList;
  typedef QList<Format*>           FormatList;
  typedef QMap<QString, QString>   OptionsMap;
  typedef QMap<QString, TransList> TransListMap;

private:
  QString          myAppName;                 //!< application name
  QStringList      myDirList;                 //!< list of resources directories
  FormatList       myFormats;                 //!< list of formats
  OptionsMap       myOptions;                 //!< options map
  OptionsMap       myConstants;               //!< constants map
  ResList          myResources;               //!< resources list
  bool             myCheckExist;              //!< "check existance" flag
  TransListMap     myTranslator;              //!< map of loaded translators
  QPixmap*         myDefaultPix;              //!< default icon
  bool             myIsPixmapCached;          //!< "cached pixmaps" flag

  bool             myHasUserValues;           //!< \c true if user preferences has been read
  WorkingMode      myWorkingMode;             //!< working mode

  friend class QtxResourceMgr::Format;
};

class QTX_EXPORT QtxResourceMgr::Format
{
public:
  Format( const QString& );
  virtual ~Format();

  QString                format() const;

  QStringList            options() const;
  QString                option( const QString& ) const;
  void                   setOption( const QString&, const QString& );

  bool                   load( Resources* );
  bool                   save( Resources* );

protected:
  virtual bool           load( const QString&, QMap<QString, Section>& ) = 0;
  virtual bool           save( const QString&, const QMap<QString, Section>& ) = 0;

private:
  QString                myFmt;    //!< format name
  QMap<QString, QString> myOpt;    //!< options map
};

#endif // QTXRESOURCEMGR_H
