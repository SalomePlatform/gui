// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "SUIT_FileValidator.h"

#include "SUIT_MessageBox.h"

#include <QFile>
#include <QFileInfo>

/*! constructor */
SUIT_FileValidator::SUIT_FileValidator(QWidget* parent) :
myParent(parent) 
{
}
  
/*! returns false if can't open file */
bool SUIT_FileValidator::canOpen( const QString& file ) 
{
  if ( !QFile::exists( file ) )
  {
    SUIT_MessageBox::critical( myParent, QObject::tr( "ERR_ERROR" ),
                               QObject::tr( "ERR_FILE_NOT_EXIST" ).arg( file ) );
      return false;
  }
  if ( !QFileInfo( file ).isReadable() )
  {
    SUIT_MessageBox::critical( myParent, QObject::tr( "ERR_ERROR" ),
                               QObject::tr( "ERR_PERMISSION_DENIED" ).arg( file ) );
    return false; 
  }
  return true;
}

/*! returns false if can't save file */
bool SUIT_FileValidator::canSave( const QString& file ) 
{
  if ( QFile::exists( file ) )
  {
    // if file exists - raise warning...
    if ( SUIT_MessageBox::question( myParent, QObject::tr( "WRN_WARNING" ),
                                    QObject::tr( "QUE_DOC_FILEEXISTS" ).arg( file ),
                                    SUIT_MessageBox::Yes | SUIT_MessageBox::No,
                                    SUIT_MessageBox::No ) != SUIT_MessageBox::Yes )
    {
      return false;
    }
    // ... and if user wants to overwrite file, check it for writeability
    if ( !QFileInfo( file ).isWritable() )
    {
      SUIT_MessageBox::critical( myParent, QObject::tr( "ERR_ERROR" ),
                                 QObject::tr( "ERR_PERMISSION_DENIED" ).arg( file ) );
      return false; 
    }
  }
  else
  {
    // if file doesn't exist - try to create it
    QFile qf( file );
    if ( !qf.open( QFile::WriteOnly ) )
    {
      SUIT_MessageBox::critical( myParent, QObject::tr( "ERR_ERROR" ),
                                 QObject::tr( "ERR_PERMISSION_DENIED" ).arg( file ) );
      return false;
    }
    else
    {
      // remove just created file
      qf.close();
      qf.remove();
    }
  }
  return true;
}
