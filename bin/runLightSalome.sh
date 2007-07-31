#!/bin/bash -f
#
# Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

# this variable necessary for loading .ini or .xml file
if [ -z "$LightAppConfig" ] ; then
  export LightAppConfig=${GUI_ROOT_DIR}/share/salome/resources/gui
fi
if [ -z "$LightAppResources" ] ; then
  export LightAppResources=${GUI_ROOT_DIR}/share/salome/resources/gui
fi

modules="KERNEL GUI"

for arg in X $* ; do
    if [ "$arg" != "X" ] ; then
	case $arg in
	    --modules=* )  modules="$modules "`echo $arg | awk -F= '{ print $2 }' | sed -e "s%,% %g"` ;;
            *)             ;;
        esac
    fi
done

for mod in $modules ; do
    if [ "$arg" != "X" ] ; then
	root_dir=`printenv ${mod}_ROOT_DIR`
	export PATH=${root_dir}/bin/salome:${PATH}
	export LD_LIBRARY_PATH=${root_dir}/lib/salome:${LD_LIBRARY_PATH}
    fi
done

SUITApp LightApp -style salome $* &
