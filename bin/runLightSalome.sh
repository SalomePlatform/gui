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
# File   : runLightSalome.sh
# Author : Vadim SANDLER, Open CASCADE S.A.S, vadim.sandler@opencascade.com
#

###
# set default value for the LightAppConfig variable, which 
# is necessary for loading of .ini or .xml resources file
###

if [ -z "$LightAppConfig" ] ; then
  export LightAppConfig=${GUI_ROOT_DIR}/share/salome/resources/gui
fi
if [ -z "$LightAppResources" ] ; then
  export LightAppResources=${GUI_ROOT_DIR}/share/salome/resources/gui
fi

###
# process --modules=... command line option (get list of modules)
###

modules=""

for arg in X $* ; do
    if [ "$arg" != "X" ] ; then
	case $arg in
	    --modules=* )  modules=`echo $arg | awk -F= '{ print $2 }' | sed -e "s%,% %g"` ;;
            *)             ;;
        esac
    fi
done

modules="KERNEL GUI $modules"

###
# exclude modules duplication
###

mods=""

for mod in $modules ; do
    echo $mods | grep -E "\<$mod\>" >/dev/null 2>&1
    if [ "$?" == "1" ] ; then
	mods="$mods $mod"
    fi
done

###
# set additional environment
###

for mod in $mods ; do
    if [ "$arg" != "X" ] ; then
	root_dir=`printenv ${mod}_ROOT_DIR`
	if [ "$root_dir" != "" ] ; then
	    mod_lower=`echo $mod | tr "A-Z" "a-z"`
	    export PATH=${root_dir}/bin/salome:${PATH}
	    export LD_LIBRARY_PATH=${root_dir}/lib/salome:${LD_LIBRARY_PATH}
	    if [ "$mod" != "KERNEL" ] && [ "$mod" != "GUI" ] ; then
		export LightAppConfig=${LightAppConfig}:${root_dir}/share/salome/resources/${mod_lower}
	    fi
	fi
    fi
done

###
# start application
###

SUITApp LightApp -style salome $* &
