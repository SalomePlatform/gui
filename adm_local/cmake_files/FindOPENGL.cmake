
IF(${WINDOWS})
  FIND_LIBRARY(OpenGL_LIB OpenGL32 "C:/Program Files/Microsoft SDKs/Windows/v6.0A/lib")
  FIND_LIBRARY(GlU_LIB GlU32 "C:/Program Files/Microsoft SDKs/Windows/v6.0A/lib")
  SET(OGL_LIBS ${OpenGL_LIB} ${GlU_LIB})
ENDIF(${WINDOWS})
