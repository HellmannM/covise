
SET(HEADERS
  ARTracePlugin.h
)

SET(SOURCES
  ARTracePlugin.cpp  
)

cover_add_plugin(ARInteractors ${COVISE_APPL_LIBRARY} ${COVISE_SHM_LIBRARY})
