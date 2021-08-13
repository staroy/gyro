find_package(Qt6 COMPONENTS Qml Quick)

include_directories(
  ${CMAKE_INSTALL_PREFIX}/include
  ${Qt6Widgets_INCLUDE_DIRS}
  ${QtQml_INCLUDE_DIRS}
  )

link_directories(
  ${CMAKE_INSTALL_PREFIX}/lib
  )

add_definitions(
  ${QtQml_DEFINITIONS}
  ${Qt5Quick_DEFINITIONS}
  -DBOOST_ERROR_CODE_HEADER_ONLY
  -DQT_DEPRECATED_WARNINGS
  -DQT_DISABLE_DEPRECATED_BEFORE=0x060000
  #-DQT_NO_DEBUG_OUTPUT
  #-DQT_NO_INFO_OUTPUT
  #-DQT_NO_WARNING_OUTPUT
  )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")

set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++11 -fstrict-aliasing -pedantic-errors -pedantic -Wno-deprecated-declarations -Wno-unused-variable")

set(sms_HEADERS
  contact_model.h
  message_model.h
  main.h
  )

set(sms_SOURCES
  contact_model.cpp
  message_model.cpp
  main.cpp
  )

qt6_add_resources(QT_RESOURCES main.qrc)

monero_add_executable(sms ${sms_HEADERS} ${sms_SOURCES} ${QT_RESOURCES})

target_link_libraries(sms
  easylogging
  wallet
  rpc_base
  cryptonote_core
  cncrypto
  common
  mnemonics
  msgpackc
  zyre-cc
  zyre-ll
  lua5.3
  leveldb
  epee
  version
  zmq
  czmq
  zyre
  uuid
  ${EPEE_READLINE}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_CHRONO_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_LOCALE_LIBRARY}
  ${ICU_LIBRARIES}
  ${Boost_THREAD_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
  ${GNU_READLINE_LIBRARY}
  ${EXTRA_LIBRARIES}
  Qt6::Qml
  Qt6::Quick)

set_property(TARGET sms
  PROPERTY
    OUTPUT_NAME "gyro-sms")

install(TARGETS sms
  RUNTIME DESTINATION   bin
  PUBLIC_HEADER DESTINATION include
  LIBRARY DESTINATION   lib
  ARCHIVE DESTINATION lib
)
