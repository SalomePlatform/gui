#  Copyright (C) 2010  CEA/DEN, EDF R&D, OPEN CASCADE
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
# -* Makefile *- 
#
# Author : Guillaume Boulant (EDF/R&D) 

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# moc-files generation
%_moc.cxx: %.hxx
	$(MOC) $< -o $@

# Qt form files generation (using UIC)
ui_%.hxx: %.ui
	$(UIC) -o $@ $<

UIC_FILES_QDIALOG = \
	ui_QDialogTest.hxx

# MOC pre-processing
MOC_FILES_QDIALOG = \
	QDialogTest_moc.cxx

BUILT_SOURCES = $(UIC_FILES_QDIALOG) $(UIC_FILES_GDIALOG)

mostlyclean-local:
	rm -f @builddir@/*_moc.cxx @builddir@/ui_*.hxx
	rm @builddir@/QDialogTest*
	rm @builddir@/GDialogTest*

QDialogTest.ui: __QDIALOG__.ui __QDIALOG__.hxx __QDIALOG__.cxx dlgfactory.sh
	$(srcdir)/dlgfactory.sh -n QDialogTest -t qdialog

GDialogTest.ui: __GDIALOG__.ui __GDIALOG__.hxx __GDIALOG__.cxx dlgfactory.sh
	$(srcdir)/dlgfactory.sh -n GDialogTest -t gdialog


QT_CXXFLAGS=@QT_INCLUDES@ @QT_MT_INCLUDES@
QT_LIBS=@QT_LIBS@

# Program targets
bin_PROGRAMS = qtester gtester

# QDialog tester
qtester_SOURCES = \
        qtester.cxx \
	QDialogTest.cxx \
	$(MOC_FILES_QDIALOG) $(UIC_FILES_QDIALOG)

qtester_CPPFLAGS = \
	$(QT_CXXFLAGS) \
	-I. \
	-I$(srcdir)

qtester_LDFLAGS = \
        $(QT_LIBS)


# GDialog tester
MOC_FILES_GDIALOG= \
	GenericDialog_moc.cxx \
	GDialogTest_moc.cxx

UIC_FILES_GDIALOG= \
	ui_GenericDialog.hxx \
	ui_GDialogTest.hxx

gtester_SOURCES = \
	gtester.cxx \
	GenericDialog.cxx \
	GDialogTest.cxx \
	$(MOC_FILES_GDIALOG) $(UIC_FILES_GDIALOG)

gtester_CPPFLAGS = \
	$(QT_CXXFLAGS) \
	-I. \
	-I$(srcdir)

gtester_LDFLAGS = \
        $(QT_LIBS)
