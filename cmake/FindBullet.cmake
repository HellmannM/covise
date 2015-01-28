# - Find BULLET
# Find the BULLET includes and library
#
#  BULLET_INCLUDE_DIR - Where to find BULLET includes
#  BULLET_LIBRARIES   - List of libraries when using BULLET
#  BULLET_FOUND       - True if BULLET was found

IF(BULLET_INCLUDE_DIR)
  SET(BULLET_FIND_QUIETLY TRUE)
ENDIF(BULLET_INCLUDE_DIR)

FIND_PATH(BULLET_INCLUDE_DIR "bullet/btBulletDynamicsCommon.h"
  PATHS
  $ENV{BULLET_HOME}/include
  $ENV{EXTERNLIBS}/bullet/include
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
  DOC "BULLET - Headers"
)

SET(BULLET_NAMES BulletDynamics)
SET(BULLET_DBG_NAMES BulletDynamics_Debug)

FIND_LIBRARY(BULLET_LIBRARY NAMES ${BULLET_NAMES}
  PATHS
  $ENV{BULLET_HOME}
  $ENV{EXTERNLIBS}/bullet
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  PATH_SUFFIXES lib lib64
  DOC "BULLET - Library"
)

SET(BULLET_COLLISION_NAMES BulletCollision)
SET(BULLET_COLLISION_DBG_NAMES BulletCollision_Debug)

FIND_LIBRARY(BULLET_COLLISION_LIBRARY NAMES ${BULLET_COLLISION_NAMES}
  PATHS
  $ENV{BULLET_HOME}
  $ENV{EXTERNLIBS}/bullet
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  PATH_SUFFIXES lib lib64
  DOC "BULLET_COLLISION - Library"
)

SET(BULLET_MATH_NAMES LinearMath)
SET(BULLET_MATH_DBG_NAMES LinearMath_Debug)

FIND_LIBRARY(BULLET_MATH_LIBRARY NAMES ${BULLET_MATH_NAMES}
  PATHS
  $ENV{BULLET_HOME}
  $ENV{EXTERNLIBS}/bullet
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  PATH_SUFFIXES lib lib64
  DOC "BULLET_MATH - Library"
)

INCLUDE(FindPackageHandleStandardArgs)

IF(MSVC)
  # VisualStudio needs a debug version
  FIND_LIBRARY(BULLET_LIBRARY_DEBUG NAMES ${BULLET_DBG_NAMES}
    PATHS
    $ENV{BULLET_HOME}
    $ENV{EXTERNLIBS}/BULLET
    PATH_SUFFIXES lib lib64
    DOC "BULLET - Library (Debug)"
  )
  FIND_LIBRARY(BULLET_COLLISION_LIBRARY_DEBUG NAMES ${BULLET_COLLISION_DBG_NAMES}
    PATHS
    $ENV{BULLET_HOME}
    $ENV{EXTERNLIBS}/BULLET
    PATH_SUFFIXES lib lib64
    DOC "BULLET_COLLISION - Library (Debug)"
  )
  FIND_LIBRARY(BULLET_MATH_LIBRARY_DEBUG NAMES ${BULLET_MATH_DBG_NAMES}
    PATHS
    $ENV{BULLET_HOME}
    $ENV{EXTERNLIBS}/BULLET
    PATH_SUFFIXES lib lib64
    DOC "BULLET_MATH - Library (Debug)"
  )
  
  IF(BULLET_LIBRARY_DEBUG AND BULLET_LIBRARY AND BULLET_COLLISION_LIBRARY_DEBUG AND BULLET_COLLISION_LIBRARY AND BULLET_MATH_LIBRARY_DEBUG AND BULLET_MATH_LIBRARY)
    SET(BULLET_LIBRARIES optimized ${BULLET_LIBRARY} debug ${BULLET_LIBRARY_DEBUG} optimized ${BULLET_COLLISION_LIBRARY} debug ${BULLET_COLLISION_LIBRARY_DEBUG} optimized ${BULLET_MATH_LIBRARY} debug ${BULLET_MATH_LIBRARY_DEBUG})
  ENDIF(BULLET_LIBRARY_DEBUG AND BULLET_LIBRARY AND BULLET_COLLISION_LIBRARY_DEBUG AND BULLET_COLLISION_LIBRARY AND BULLET_MATH_LIBRARY_DEBUG AND BULLET_MATH_LIBRARY)

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(BULLET DEFAULT_MSG BULLET_LIBRARY BULLET_LIBRARY_DEBUG BULLET_COLLISION_LIBRARY BULLET_COLLISION_LIBRARY_DEBUG BULLET_MATH_LIBRARY BULLET_MATH_LIBRARY_DEBUG BULLET_INCLUDE_DIR)

  MARK_AS_ADVANCED(BULLET_LIBRARY BULLET_LIBRARY_DEBUG BULLET_COLLISION_LIBRARY BULLET_COLLISION_LIBRARY_DEBUG BULLET_MATH_LIBRARY BULLET_MATH_LIBRARY_DEBUG BULLET_INCLUDE_DIR)
  
ELSE(MSVC)
  # rest of the world
  SET(BULLET_LIBRARIES ${BULLET_LIBRARY} ${BULLET_COLLISION_LIBRARY} ${BULLET_MATH_LIBRARY})

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(BULLET DEFAULT_MSG BULLET_LIBRARY BULLET_COLLISION_LIBRARY BULLET_MATH_LIBRARY BULLET_INCLUDE_DIR)
  
  MARK_AS_ADVANCED(BULLET_LIBRARY BULLET_COLLISION_LIBRARY BULLET_MATH_LIBRARY BULLET_INCLUDE_DIR)
  
ENDIF(MSVC)

IF(BULLET_FOUND)
  SET(BULLET_INCLUDE_DIRS ${BULLET_INCLUDE_DIR} ${BULLET_INCLUDE_DIR}/bullet)
ENDIF(BULLET_FOUND)