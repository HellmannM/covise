MACRO(USE_FFMPEG)
  IF(NOT FFMPEG_USED)
    SET(FFMPEG_USED TRUE)
    covise_find_package(FFMPEG)
    if (FFMPEG_FOUND)
       ADD_DEFINITIONS(-DHAVE_FFMPEG)
       INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIRS})
       SET(EXTRA_LIBS ${EXTRA_LIBS} ${FFMPEG_LIBRARIES})
       if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
          SET(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations")
       elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
          SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
       endif()
     endif()
  ENDIF(NOT FFMPEG_USED)
ENDMACRO(USE_FFMPEG)