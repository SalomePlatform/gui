//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:      QtxComboBox.h
// Author:    Sergey TELKOV
//
#ifndef QTXCOMBOBOX_H
#define QTXCOMBOBOX_H

#include "Qtx.h"

#include <qmap.h>
#include <qcombobox.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxComboBox : public QComboBox
{
    Q_OBJECT

    typedef QMap<int, int> IndexIdMap;

public:
    QtxComboBox( QWidget* = 0, const char* = 0 );
    QtxComboBox( bool, QWidget* = 0, const char* = 0 );
    virtual ~QtxComboBox();

    bool         isCleared() const;
    void         setCleared( const bool );

    virtual void setCurrentItem( int );
    virtual void setCurrentText( const QString& );

    int          currentId() const;
    void         setCurrentId( int );

signals:
    void         activatedId( int );
    void         highlightedId( int );

private slots:
    void         onActivated( int );
    void         onActivated( const QString& );

protected:
    virtual void paintEvent( QPaintEvent* );

private:
    int          id( const int ) const;
    int          index( const int ) const;

    void         resetClear();
    void         paintClear( QPaintEvent* );

private:
    bool         myCleared;
    IndexIdMap   myIndexId;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
