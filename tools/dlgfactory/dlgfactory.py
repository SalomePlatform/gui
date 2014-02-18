#!/usr/bin/env python
# Copyright (C) 2010-2014  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import sys, os

__descr_str = ""
__descr_str += "This script generates a set of files to initiate a dialog Qt window "
__descr_str += "(i.e. MyDialog.ui, MyDialog.h and MyDialog.cxx files). "
__descr_str += "The dialog window can be a self-consistent class (i.e. depends only "
__descr_str += "on Qt classes) or a class that inherits class GenericDialog "
__descr_str += "which implementation is provided in this package and "
__descr_str += "which design is defined by the GenericDialog.ui file (editable using "
__descr_str += "the Qt designer). "
__descr_str += "The -t option let you choose between the two possibilities (specify "
__descr_str += "\"-t qdialog\" for the first case, \"-t gdialog\" otherwise)."

__msg_str = """
#
# ---------------------------------------------------------
# Generation rules to create moc files from QObject headers
# and form source files from ui files
# ---------------------------------------------------------

%_moc.cxx: %.h
	$(MOC) $< -o $@

ui_%.h: %.ui
	$(UIC) -o $@ $<


# ---------------------------------------------------------
# Declaration of form files generated by UIC and MOC files
# as BUILT_SOURCES to be used in the building process.
# ---------------------------------------------------------
#
UIC_FILES = \
	ui___CLASSNAME__.h
#
MOC_FILES = \
	__CLASSNAME___moc.cxx

BUILT_SOURCES = $(UIC_FILES)

#
# ---------------------------------------------------------
# Declaration of sources files to the building process
# ---------------------------------------------------------
# MOC files and UIC files should be added to the list of undistributed
# source files with something like (where <MyLibrary> should be
# replaced by the name of the product declared by the directive
# lib_LTLIBRARIES):
#
nodist_<MyLibrary>_la_SOURCES += $(MOC_FILES) $(UIC_FILES)

dist_<MyLibrary>_la_SOURCES += __CLASSNAME__.cxx
salomeinclude_HEADERS       += __CLASSNAME__.h

<MyLibrary>_la_CPPFLAGS = \\
	$(QT_CXXFLAGS)

<MyLibrary>_la_LDFLAGS = \\
        $(QT_LIBS)
""" 

if __name__ == "__main__":
  from optparse import OptionParser
  import shutil, fileinput

  tool_path = os.path.dirname( os.path.abspath( sys.argv[0] ) )

  parser = OptionParser( description = __descr_str )
  parser.add_option( "-n", action="store", default="TestDialog", dest="className", metavar="className",
                     help="specify the name of the class (default is TestDialog)" )
  parser.add_option( "-t", action="store", default="qdialog", dest="classType",
                     choices=["qdialog", "gdialog"], metavar="classType",
                     help="specify the type of the class (default is qdialog)" )
  parser.add_option( "-v", "--verbose", action="store_true", default=True, dest="verbose",
                     help="verbose mode" )
  parser.add_option( "-s", "--silent", action="store_false", dest="verbose",
                     help="silent mode" )

  (options, args) = parser.parse_args()
  className = options.className
  classType = options.classType

  for ext in [".cxx", ".h", ".ui"]:
    file_dest = className + ext 
    if classType == "qdialog":
      file_src = os.path.join( tool_path, "__QDIALOG__" + ext )
      pass
    else:
      file_src = os.path.join( tool_path, "__GDIALOG__" + ext )
      pass
    shutil.copyfile( file_src, file_dest )
    finput = fileinput.FileInput( file_dest, inplace=1 )
    for line in finput:
      line = line[:-1] 
      line = line.replace( "__CLASSNAME__", className )
      print line
      pass

    if options.verbose:
      print "Note that the following directives should be present in your CMakeLists.txt (or something like that): \n"
      print __msg_str.replace( "__CLASSNAME__", className )
      pass
  pass

