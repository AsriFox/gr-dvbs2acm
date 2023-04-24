add_library(plsync STATIC 
    pi2_bpsk.cc
    reed_muller.cc
    util.cc
    pl_descrambler.cc 
    pl_frame_sync.cc 
    pl_freq_sync.cc 
    pl_signaling.cc
)
list(APPEND PLSYNC_LIBS plsync)

target_include_directories(plsync
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
    PUBLIC $<INSTALL_INTERFACE:include>
  )

set_property(TARGET plsync PROPERTY POSITION_INDEPENDENT_CODE ON)
set(PLSYNC_LIBS ${PLSYNC_LIBS} PARENT_SCOPE)
