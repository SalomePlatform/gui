isEmpty( RES_FILES ):RES_FILES = resources/*.png

win32:SOURCES+=$$RES_FILES
win32:Resource=$$RES_FILES
win32:copy_res.name = Install resources ${QMAKE_FILE_IN}
win32:copy_res.commands = type ${QMAKE_FILE_IN} > ${QMAKE_FILE_OUT}
# Take into account extra "resources\" prefix in the file name
win32:copy_res.output = $${GUIResources}\\..\\${QMAKE_FILE_NAME}
win32:copy_res.input = Resource
win32:QMAKE_EXTRA_COMPILERS += copy_res

resources.files = $$RES_FILES
resources.path = $${GUIResources}/resources

INSTALLS += resources
