# Looks for environment variable:
# OPENCV_PATH 

# Sets the variables:
# OPENCV_INCLUDE_DIRS
# OPENCV_LIBRARIES
# OPENCV_FOUND

set(OPENCV_PATH $ENV{OPENCV_PATH})

# If there was no environment variable override for the OPENCV_PATH
# try finding it inside the local 3rdparty path.
if ("${OPENCV_PATH}" STREQUAL "")
  set(OPENCV_PATH "${LOCAL_3RDPARTY}/opencv_4_5")
endif()

# message("OPENCV_PATH = " "${OPENCV_PATH}")

find_path( OPENCV_INCLUDE_DIRS "opencv2/opencv.hpp"
  PATHS /usr/include ${OPENCV_PATH}/include )

message("OPENCV_INCLUDE_DIRS = " "${OPENCV_INCLUDE_DIRS}")

set(OPENCV_LIBRARY_DIR ${OPENCV_PATH}/lib/x64)

# message("OPENCV_LIBRARY_DIR = " "${OPENCV_LIBRARY_DIR}")

find_library(OPENCV_LIBRARIES
  NAMES opencv_world455d OPENCV
  PATHS ${OPENCV_LIBRARY_DIR} )

message("OPENCV_LIBRARY_DIR = " "${OPENCV_LIBRARY_DIR}")

# DAR Not using the static OPENCV libraries. What's the name under Linux?
#find_library(OPENCV_STATIC_LIBRARIES
# NAMES opencv_world455s
#  PATHS ${OPENCV_LIBRARY_DIR} )

#message("OPENCV_STATIC_LIBRARIES = " "${OPENCV_STATIC_LIBRARIES}")

include(FindPackageHandleStandardArgs)

#find_package_handle_standard_args(OPENCV DEFAULT_MSG OPENCV_INCLUDE_DIRS OPENCV_LIBRARIES OPENCV_STATIC_LIBRARIES)
find_package_handle_standard_args(OPENCV DEFAULT_MSG OPENCV_INCLUDE_DIRS OPENCV_LIBRARIES)

#mark_as_advanced(OPENCV_INCLUDE_DIRS OPENCV_LIBRARIES OPENCV_STATIC_LIBRARIES)
mark_as_advanced(OPENCV_INCLUDE_DIRS OPENCV_LIBRARIES)

# message("OPENCV_FOUND = " "${OPENCV_FOUND}")
