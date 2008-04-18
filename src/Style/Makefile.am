#  Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#  File   : Makefile.in
#  Author : Vladimir Klyachin (OCN)
#  Module : SalomeApp

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libSalomeStyle.la

salomeinclude_HEADERS= \
	Style.h \
	Style_Tools.h \
	Style_Model.h \
	Style_Salome.h

dist_libSalomeStyle_la_SOURCES= \
	Style_Tools.cxx \
	Style_Model.cxx \
	Style_Salome.cxx

MOC_FILES= \
	Style_Salome_moc.cxx

QRC_FILES= \
	qrc_Style.cxx

nodist_libSalomeStyle_la_SOURCES= $(MOC_FILES) $(QRC_FILES)

libSalomeStyle_la_CPPFLAGS=$(QT_INCLUDES) \
	-I$(srcdir)/../Qtx

libSalomeStyle_la_LDFLAGS=$(QT_MT_LIBS) \
	../Qtx/libqtx.la
