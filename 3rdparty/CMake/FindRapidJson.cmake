# Looks for environment variable:
# RAPIDJSON_PATH 

# Sets the variables:
# RAPIDJSON_INCLUDE_DIRS
# RAPIDJSON_LIBRARIES
# RAPIDJSON_FOUND

set(RAPIDJSON_PATH $ENV{RAPIDJSON_PATH})

# If there was no environment variable override for the RAPIDJSON_PATH
# try finding it inside the local 3rdparty path.
if ("${RAPIDJSON_PATH}" STREQUAL "")
  set(RAPIDJSON_PATH "${LOCAL_3RDPARTY}/rapidjson")
endif()

# message("RAPIDJSON_PATH = " "${RAPIDJSON_PATH}")

find_path( RAPIDJSON_INCLUDE_DIRS "rapidjson/rapidjson.h"
  PATHS /usr/include ${RAPIDJSON_PATH}/include )

message("RAPIDJSON_INCLUDE_DIRS = " "${RAPIDJSON_INCLUDE_DIRS}")

# set(RAPIDJSON_LIBRARY_DIR ${RAPIDJSON_PATH}/lib/x64)

# message("RAPIDJSON_LIBRARY_DIR = " "${RAPIDJSON_LIBRARY_DIR}")

#find_library(RAPIDJSON_LIBRARIES
#  NAMES RAPIDJSON_world455d RAPIDJSON
#  PATHS ${RAPIDJSON_LIBRARY_DIR} )

# message("RAPIDJSON_LIBRARY_DIR = " "${RAPIDJSON_LIBRARY_DIR}")

# DAR Not using the static RAPIDJSON libraries. What's the name under Linux?
#find_library(RAPIDJSON_STATIC_LIBRARIES
# NAMES RAPIDJSON_world455s
#  PATHS ${RAPIDJSON_LIBRARY_DIR} )

#message("RAPIDJSON_STATIC_LIBRARIES = " "${RAPIDJSON_STATIC_LIBRARIES}")

include(FindPackageHandleStandardArgs)

#find_package_handle_standard_args(RAPIDJSON DEFAULT_MSG RAPIDJSON_INCLUDE_DIRS RAPIDJSON_LIBRARIES RAPIDJSON_STATIC_LIBRARIES)
#find_package_handle_standard_args(RAPIDJSON DEFAULT_MSG RAPIDJSON_INCLUDE_DIRS RAPIDJSON_LIBRARIES)
find_package_handle_standard_args(RAPIDJSON DEFAULT_MSG RAPIDJSON_INCLUDE_DIRS)

#mark_as_advanced(RAPIDJSON_INCLUDE_DIRS RAPIDJSON_LIBRARIES RAPIDJSON_STATIC_LIBRARIES)
#mark_as_advanced(RAPIDJSON_INCLUDE_DIRS RAPIDJSON_LIBRARIES)
mark_as_advanced(RAPIDJSON_INCLUDE_DIRS)

# message("RAPIDJSON_FOUND = " "${RAPIDJSON_FOUND}")
