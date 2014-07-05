cmake_minimum_required(VERSION 2.8.11)

project(kdirchainrebuild)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Include ECM
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(OpenMP)
if(OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    add_definitions(-D_GLIBCXX_PARALLEL)
endif()

add_definitions(-D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS)

# required for QML_INSTALL_DIR (among others)
include(KDEInstallDirs)
include(KDECMakeSettings)

find_package(Qt5 5.2.0 REQUIRED COMPONENTS Core Quick Qml Gui Concurrent) # Needed for the kdirchainmodelplugin library
find_package(KF5 CONFIG REQUIRED COMPONENTS Bookmarks XmlGui Solid KIO)

# Please KDE people. QUIT BREAKING THE INCLUDE PATHS! Manually add path to include/KDE.
include_directories(${CMAKE_INSTALL_PREFIX}/include/KDE)

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
  models/dirgroupedproxymodel.cpp
  models/flatdirgroupedsortmodel.cpp
  utils/breadcrumburlmodel.cpp
  utils/shortcut.cpp
  utils/urlundoredo.cpp
  utils/mimeimageprovider.h
  utils/splitview.cpp
  kdirectory.cpp
  kdirectoryentry.cpp
  kdirectoryprivate_p.cpp
  kdirlisterv2.cpp
  kdirlisterv2_p.cpp
  staticmimetype.cpp
  ThreadPool.h
#  kstringunicode.cpp
#  kradix.cpp
#  kradix2.cpp
)

set(kdirchainrebuild_SRCS
  main.cpp
)

set(kdirchainmodelplugin_LIB_SRCS
  qmlplugin/dirchainmodelplugin.cpp
)

add_library(kdirchain SHARED ${kdirchain_LIB_SRCS})
add_library(kdirchainmodelplugin SHARED ${kdirchainmodelplugin_LIB_SRCS})

install(FILES
  ${kdirchain_LIB_INCLUDES}
  DESTINATION ${INCLUDE_INSTALL_DIR}/kdirchain COMPONENT Devel
)

target_link_libraries(kdirchain
  Qt5::Gui # For QKeySequence
  Qt5::Quick # For QQuickImageProvider
  Qt5::Concurrent # For sorting offloading out of the main thread.
  KF5::KIOCore
  KF5::KIOWidgets
)

target_link_libraries(kdirchainmodelplugin
  Qt5::Qml
  KF5::IconThemes # For KIconLoader
  kdirchain
)

set_target_properties(kdirchain PROPERTIES
                                VERSION 0.0.1
)

add_executable(kdirchainrebuild ${kdirchainrebuild_SRCS})

target_link_libraries(kdirchainrebuild
  Qt5::Widgets
  kdirchain
)

install(TARGETS kdirchainmodelplugin DESTINATION ${QML_INSTALL_DIR}/kdirchainmodel)
install(FILES qmlplugin/qmldir DESTINATION ${QML_INSTALL_DIR}/kdirchainmodel)
install(TARGETS kdirchain EXPORT kdirchainTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
