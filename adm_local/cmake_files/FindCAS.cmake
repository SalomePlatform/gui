#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

SET(CASROOT $ENV{CASROOT})

SET(OCC_VERSION_MAJOR 6)
SET(OCC_VERSION_MINOR 3)
SET(OCC_VERSION_MAINTENANCE 0)

SET(CAS_CPPFLAGS)
SET(CAS_CPPFLAGS ${CAS_CPPFLAGS} -DOCC_VERSION_MAJOR=${OCC_VERSION_MAJOR})
SET(CAS_CPPFLAGS ${CAS_CPPFLAGS} -DOCC_VERSION_MINOR=${OCC_VERSION_MINOR})
SET(CAS_CPPFLAGS ${CAS_CPPFLAGS} -DOCC_VERSION_MAINTENANCE=${OCC_VERSION_MAINTENANCE})
SET(CAS_CPPFLAGS ${CAS_CPPFLAGS} -DLIN -DLINTEL -DCSFDB)
SET(CAS_CPPFLAGS ${CAS_CPPFLAGS} -DNo_exception)
SET(CAS_CPPFLAGS ${CAS_CPPFLAGS} -DHAVE_CONFIG_H)
SET(CAS_CPPFLAGS ${CAS_CPPFLAGS} -DHAVE_LIMITS_H)
SET(CAS_CPPFLAGS ${CAS_CPPFLAGS} -I${CASROOT}/inc)

IF(WINDOWS)
  IF(CMAKE_BUILD_TYPE STREQUAL Debug)
    SET(CASROOT_LIBDIR ${CASROOT}/win32/libd)
  ELSE(CMAKE_BUILD_TYPE STREQUAL Debug)
    SET(CASROOT_LIBDIR ${CASROOT}/win32/lib)
  ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)
ELSE(WINDOWS)
  SET(CASROOT_LIBDIR ${CASROOT}/lib)
ENDIF(WINDOWS)

FIND_LIBRARY(BinLPlugin BinLPlugin PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(BinPlugin BinPlugin PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(BinTObjPlugin BinTObjPlugin PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(BinXCAFPlugin BinXCAFPlugin PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(FWOSPlugin FWOSPlugin PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(mscmd mscmd PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(PTKernel PTKernel PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(StdLPlugin StdLPlugin PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(StdPlugin StdPlugin PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKAdvTools TKAdvTools PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKBin TKBin PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKBinL TKBinL PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKBinTObj TKBinTObj PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKBinXCAF TKBinXCAF PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKBO TKBO PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKBool TKBool PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKBRep TKBRep PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKCAF TKCAF PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKCDF TKCDF PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKCDLFront TKCDLFront PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKCPPClient TKCPPClient PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKCPPExt TKCPPExt PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKCPPIntExt TKCPPIntExt PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKCPPJini TKCPPJini PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKCSFDBSchema TKCSFDBSchema PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKDCAF TKDCAF PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKDraw TKDraw PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKernel TKernel PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKFeat TKFeat PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKFillet TKFillet PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKG2d TKG2d PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKG3d TKG3d PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKGeomAlgo TKGeomAlgo PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKGeomBase TKGeomBase PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKHLR TKHLR PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKIDLFront TKIDLFront PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKIGES TKIGES PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKLCAF TKLCAF PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKMath TKMath PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKMesh TKMesh PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKMeshVS TKMeshVS PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKNIS TKNIS PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKOffset TKOffset PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKOpenGl TKOpenGl PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKPCAF TKPCAF PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKPLCAF TKPLCAF PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKPrim TKPrim PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKPShape TKPShape PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKService TKService PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKShapeSchema TKShapeSchema PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKShHealing TKShHealing PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKStdLSchema TKStdLSchema PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKStdSchema TKStdSchema PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKSTEP TKSTEP PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKSTEP209 TKSTEP209 PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKSTEPAttr TKSTEPAttr PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKSTEPBase TKSTEPBase PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKSTL TKSTL PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKTCPPExt TKTCPPExt PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKTObj TKTObj PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKTObjDRAW TKTObjDRAW PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKTopAlgo TKTopAlgo PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKTopTest TKTopTest PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKV2d TKV2d PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKV3d TKV3d PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKViewerTest TKViewerTest PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKVRML TKVRML PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKWOK TKWOK PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKWOKTcl TKWOKTcl PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKXCAF TKXCAF PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKXCAFSchema TKXCAFSchema PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKXDEDRAW TKXDEDRAW PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKXDEIGES TKXDEIGES PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKXDESTEP TKXDESTEP PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKXMesh TKXMesh PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKXml TKXml PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKXmlL TKXmlL PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKXmlTObj TKXmlTObj PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKXmlXCAF TKXmlXCAF PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(TKXSBase TKXSBase PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(TKXSDRAW TKXSDRAW PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(wokcmd wokcmd PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(wokdeliverysteps wokdeliverysteps PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(wokdfltsteps wokdfltsteps PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(wokobjssteps wokobjssteps PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(wokorbixsteps wokorbixsteps PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(woksteps woksteps PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(woktoolscmd woktoolscmd PATHS ${CASROOT_LIBDIR})
# FIND_LIBRARY(wokutilscmd wokutilscmd PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(XCAFPlugin XCAFPlugin PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(XmlLPlugin XmlLPlugin PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(XmlPlugin XmlPlugin PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(XmlTObjPlugin XmlTObjPlugin PATHS ${CASROOT_LIBDIR})
FIND_LIBRARY(XmlXCAFPlugin XmlXCAFPlugin PATHS ${CASROOT_LIBDIR})

SET(CAS_KERNEL ${TKernel} ${TKMath})
SET(CAS_OCAF ${TKernel} ${TKMath} ${TKCDF} ${TKLCAF})
SET(CAS_VIEWER ${TKService} ${TKV3d} ${TKG3d} ${TKGeomBase} ${TKBRep})
SET(CAS_OCAFVIS ${TKCAF} ${TKBRep} ${TKG2d})
SET(CAS_MODELER ${TKG3d} ${TKGeomBase} ${TKGeomAlgo} ${TKBRep} ${TKTopAlgo} ${TKG2d})

SET(TKV3d_EA ${TKernel} ${TKMath} ${TKV3d})
SET(TKBool_EA ${TKernel} ${TKMath} ${TKBRep} ${TKG2d} ${TKG3d} ${TKGeomBase} ${TKGeomAlgo} ${TKTopAlgo} ${TKBool})
SET(TKBRep_EA ${TKernel} ${TKMath} ${TKBRep})
SET(TKIGES_EA ${TKernel} ${TKMath} ${TKXSBase} ${TKBRep} ${TKIGES})
SET(TKSTEP_EA ${TKernel} ${TKMath} ${TKXSBase} ${TKBRep} ${TKSTEP})
SET(TKSTL_EA ${TKernel} ${TKMath} ${TKSTL})
SET(TKCAF_EA ${TKPrim} ${TKCAF})

SET(TKV3d ${TKV3d_EA})
SET(TKBool ${TKBool_EA})
SET(TKBRep ${TKBRep_EA})
SET(TKIGES ${TKIGES_EA})
SET(TKSTEP ${TKSTEP_EA})
SET(TKSTL ${TKSTL_EA})
SET(TKCAF ${TKCAF_EA})