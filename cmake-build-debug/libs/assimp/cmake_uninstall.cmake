IF(NOT EXISTS "C:/Users/caleb/Graphics/gpr300-sokol/cmake-build-debug/libs/assimp/install_manifest.txt")
  MESSAGE(FATAL_ERROR "Cannot find install manifest: \"C:/Users/caleb/Graphics/gpr300-sokol/cmake-build-debug/libs/assimp/install_manifest.txt\"")
ENDIF(NOT EXISTS "C:/Users/caleb/Graphics/gpr300-sokol/cmake-build-debug/libs/assimp/install_manifest.txt")

FILE(READ "C:/Users/caleb/Graphics/gpr300-sokol/cmake-build-debug/libs/assimp/install_manifest.txt" files)
STRING(REGEX REPLACE "\n" ";" files "${files}")
FOREACH(file ${files})
  MESSAGE(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
  EXEC_PROGRAM(
    "C:/Users/caleb/AppData/Local/Programs/CLion/bin/cmake/win/x64/bin/cmake.exe" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
    OUTPUT_VARIABLE rm_out
    RETURN_VALUE rm_retval
    )
  IF(NOT "${rm_retval}" STREQUAL 0)
    MESSAGE(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
  ENDIF(NOT "${rm_retval}" STREQUAL 0)
ENDFOREACH(file)
