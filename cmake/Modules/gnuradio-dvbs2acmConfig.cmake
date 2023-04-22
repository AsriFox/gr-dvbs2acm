find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_DVBS2ACM gnuradio-dvbs2acm)

FIND_PATH(
    GR_DVBS2ACM_INCLUDE_DIRS
    NAMES gnuradio/dvbs2acm/api.h
    HINTS $ENV{DVBS2ACM_DIR}/include
        ${PC_DVBS2ACM_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_DVBS2ACM_LIBRARIES
    NAMES gnuradio-dvbs2acm
    HINTS $ENV{DVBS2ACM_DIR}/lib
        ${PC_DVBS2ACM_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-dvbs2acmTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_DVBS2ACM DEFAULT_MSG GR_DVBS2ACM_LIBRARIES GR_DVBS2ACM_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_DVBS2ACM_LIBRARIES GR_DVBS2ACM_INCLUDE_DIRS)
