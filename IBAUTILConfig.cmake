# - Try to find libibautil
# Once done this will define
#  
#  IBAUTIL_FOUND        - system has ibautil
#  IBAUTIL_INCLUDE_DIR  - the ibautil include directory
#  IBAUTIL_LIBRARY      - Link these to use ibautil

FIND_PATH(IBAUTIL_INCLUDE_DIR NAMES ibautils/ib_parser.h ibautils/ib_port.h ibautils/ib_fabric.h ibautils/regex.h )
FIND_LIBRARY(IBAUTIL_LIBRARY ibautils NAMES libibautils.so )

if(IBAUTIL_INCLUDE_DIR AND IBAUTIL_LIBRARY)
  MESSAGE(STATUS "Found ibautil: ${IBAUTIL_LIBRARY}")
  SET(IBAUTIL_FOUND TRUE)

    ADD_LIBRARY(ibautils SHARED IMPORTED)
    set_target_properties(ibautils PROPERTIES IMPORTED_LOCATION ${IBAUTIL_LIBRARY})

  else(IBAUTIL_INCLUDE_DIR AND IBAUTIL_LIBRARY)
    MESSAGE(FATAL_ERROR "Unable to find ibautil library. Please install: https://github.com/nateucar/libibautils" )
    SET(IBAUTIL_FOUND FALSE)
endif(IBAUTIL_INCLUDE_DIR AND IBAUTIL_LIBRARY)

