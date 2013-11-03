cmake_minimum_required(VERSION 2.8.11)

project(kdirchainrebuild)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Include ECM
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 5.2.0 REQUIRED COMPONENTS Core) # Not needed per se since this dependency is also dragged in through KIOCore
find_package(KF5 MODULE REQUIRED COMPONENTS KBookmarks XmlGui Solid KIO)

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Quick_DEFINITIONS} ${Qt5Qml_DEFINITIONS})

# We do allow "signals and slots" instead of "Q_SIGNALS and Q_SLOTS".
remove_definitions(-DQT_NO_KEYWORDS -DQT_NO_CAST_FROM_ASCII -DQT_NO_SIGNALS_SLOTS_KEYWORDS)

# C++11 support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(kdirchain_LIB_SRCS
  models/dirlistmodel.cpp
  models/dirtreemodel.cpp
  models/dirgroupedmodel.cpp
  kdirectory.cpp
  kdirectoryentry.cpp
  kdirectoryprivate_p.cpp
  kdirlisterv2.cpp
  kdirlisterv2_p.cpp
  staticmimetype.cpp
)

add_library(kdirchain SHARED ${kdirchain_LIB_SRCS})

target_link_libraries(kdirchain
  KF5::KIOCore
)

set_target_properties(kdirchain PROPERTIES
                                VERSION 0.0.1
)

set(kdirchainrebuild_SRCS
  main.cpp
)

add_executable(kdirchainrebuild ${kdirchainrebuild_SRCS})

target_link_libraries(kdirchainrebuild
  Qt5::Widgets
  kdirchain
)
