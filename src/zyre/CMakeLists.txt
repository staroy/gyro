set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)

add_definitions(
  -DBOOST_ERROR_CODE_HEADER_ONLY
  -DMSGPACK_USE_BOOST
  -DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}
  -DZYRE_BUILD_DRAFT_API
)

include_directories(
  ${CMAKE_INSTALL_PREFIX}/include
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/external/easylogging++
  /usr/include/lua5.3
)

link_directories(
  ${CMAKE_INSTALL_PREFIX}/lib
)

set(DCC_NAME "lvdb-cc")
set(ZCC_NAME "zyre-cc")
set(ZLL_NAME "zyre-ll")
set(ZL_NAME "zyre-lua")

set(zcc_src
  zyre.cpp
  http.cpp
  packjson.cpp
  auth.c
  certs.c
)

set(zcc_inc
  zyre.hpp
  http.h
  wspp.hpp
  packjson.h
  certs.h
)

set(dcc_src
  db.cpp
)

set(dcc_inc
  db.h
  db.hpp
  iterator_tpl.h
)

set(zcc_lib
  boost_system
  boost_program_options
  zmq
  czmq
  zyre
  pthread)

set(zll_src
  ldb.cpp
  timeout.cpp
  lzyre.cpp
  lthread.cpp
  lsrv.cpp
)

set(zll_inc
  ldb.h
  timeout.h
  lzyre.hpp
  lthread.h
  lsrv.h
)

set(zll_lib
  ${ZCC_NAME}
  lua5.3
  leveldb
  cmsgpack
  boost_system
  boost_program_options
  pthread)

set(zl_src
  main.cpp
  ${CMAKE_SOURCE_DIR}/external/easylogging++/easylogging++.cc
  ${CMAKE_SOURCE_DIR}/contrib/epee/src/mlog.cpp
  ${CMAKE_SOURCE_DIR}/src/common/command_line.cpp
  ${CMAKE_SOURCE_DIR}/src/common/i18n.cpp
)

set(zl_inc
  ${CMAKE_SOURCE_DIR}/contrib/epee/include/misc_log_ex.h
  ${CMAKE_SOURCE_DIR}/src/common/command_line.h
  ${CMAKE_SOURCE_DIR}/src/common/i18n.h
)

set(zl_lib
  ${DCC_NAME}
  ${ZCC_NAME}
  ${ZLL_NAME}
  lua5.3
  leveldb
  cmsgpack
  boost_system
  boost_filesystem
  boost_program_options
  ssl
  crypto
  zmq
  czmq
  zyre
  pthread)

if("Release" STREQUAL ${CMAKE_BUILD_TYPE})
  add_library(${DCC_NAME} STATIC ${dcc_src} ${dcc_inc})
  target_link_libraries(${DCC_NAME} ${dcc_lib})
  add_library(${ZCC_NAME} STATIC ${zcc_src} ${zcc_inc})
  target_link_libraries(${ZCC_NAME} ${zcc_lib})
  add_library(${ZLL_NAME} STATIC  ${zll_src} ${zll_inc})
  target_link_libraries(${ZLL_NAME} ${zll_lib})
  add_executable(${ZL_NAME} ${zl_src} ${zl_inc})
  target_link_libraries(${ZL_NAME} ${zl_lib})
else()
  add_library(${DCC_NAME} SHARED ${dcc_src} ${dcc_inc})
  target_link_libraries(${DCC_NAME} ${dcc_lib})
  add_library(${ZCC_NAME} SHARED ${zcc_src} ${zcc_inc})
  target_link_libraries(${ZCC_NAME} ${zcc_lib})
  add_library(${ZLL_NAME} SHARED  ${zll_src} ${zll_inc})
  target_link_libraries(${ZLL_NAME} ${zll_lib})
  add_executable(${ZL_NAME} ${zl_src} ${zl_inc})
  target_link_libraries(${ZL_NAME} ${zl_lib})
endif()

add_dependencies(${ZLL_NAME} ${ZCC_NAME})
add_dependencies(${ZL_NAME} ${ZLL_NAME})

install(TARGETS ${ZL_NAME} ${DCC_NAME} ${ZCC_NAME} ${ZLL_NAME}
  RUNTIME DESTINATION   bin
  PUBLIC_HEADER DESTINATION include
  LIBRARY DESTINATION   lib
  ARCHIVE DESTINATION lib
)
