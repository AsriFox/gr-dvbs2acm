if(CMAKE_SYSTEM_PROCESSOR MATCHES "(^arm)|(^aarch64)")
  add_library(ldpc_decoder_neon STATIC ldpc_decoder_neon.cc)
  if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "(^aarch64)")
    target_compile_options(ldpc_decoder_neon PRIVATE -mfpu=neon)
  endif()
  list(APPEND LDPC_LIBS ldpc_decoder_neon)
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64)|(AMD64|amd64)|(^i.86$)")
  add_library(ldpc_decoder_avx2 STATIC ldpc_decoder_avx2.cc)
  add_library(ldpc_decoder_sse41 STATIC ldpc_decoder_sse41.cc)
  target_compile_options(ldpc_decoder_avx2 PRIVATE -mavx2)
  target_compile_options(ldpc_decoder_sse41 PRIVATE -msse4.1)
  list(APPEND LDPC_LIBS ldpc_decoder_avx2)
  list(APPEND LDPC_LIBS ldpc_decoder_sse41)
endif()

add_library(ldpc_decoder_generic STATIC ldpc.cc ldpc_decoder_generic.cc)
target_include_directories(ldpc_decoder_generic
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
   PUBLIC $<INSTALL_INTERFACE:include>
)
list(APPEND LDPC_LIBS ldpc_decoder_generic)

foreach(LDPC_LIB ${LDPC_LIBS})
  set_property(TARGET ${LDPC_LIB} PROPERTY POSITION_INDEPENDENT_CODE ON)
  target_compile_options(${LDPC_LIB} PRIVATE -DCMAKE_BUILD_TYPE=Release)
endforeach()

set(LDPC_LIBS ${LDPC_LIBS} PARENT_SCOPE)
