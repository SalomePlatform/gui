#!/bin/csh -f
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
# File   : runLightSalome.csh
# Author : Vadim SANDLER, Open CASCADE S.A.S, vadim.sandler@opencascade.com
#

###
# set default value for the LightAppConfig variable, which 
# is necessary for loading of .ini or .xml resources file
###

if ( ! $?LightAppConfig )    setenv LightAppConfig ${GUI_ROOT_DIR}/share/salome/resources/gui
if ( ! $?LightAppResources ) setenv LightAppResources ${GUI_ROOT_DIR}/share/salome/resources/gui

###
# process --modules=... command line option (get list of modules)
###

set modules=""

foreach arg ( X $* )
    if ( "$arg" != "X" ) then
	switch ( $arg )
	    case --modules=*:
		set modules=`echo $arg | awk -F= '{ print $2 }' | sed -e "s%,% %g"`
		breaksw
	    default:
		breaksw
	endsw
    endif
end

set modules="KERNEL GUI $modules"

###
# exclude modules duplication
###

set mods=""

foreach mod ( $modules )
    echo $mods | grep -E "\<$mod\>" >/dev/null
    if ( "$?" == "1" ) then
	set mods="$mods $mod"
    endif
end

###
# set additional environment
###

foreach mod ( $mods )
    if ( "$arg" != "X" ) then
	set root_dir=`printenv ${mod}_ROOT_DIR`
	if ( "$root_dir" != "" ) then
	    set mod_lower=`echo $mod | tr "A-Z" "a-z"`
	    setenv PATH ${root_dir}/bin/salome:${PATH}
	    setenv LD_LIBRARY_PATH ${root_dir}/lib/salome:${LD_LIBRARY_PATH}
	    if ( "$mod" != "KERNEL" && "$mod" != "GUI" ) then
		setenv LightAppConfig ${LightAppConfig}:${root_dir}/share/salome/resources/${mod_lower}
	    endif
	endif
    endif
end

###
# start application
###

SUITApp LightApp -style salome $* &
