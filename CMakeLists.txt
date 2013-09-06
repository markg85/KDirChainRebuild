cmake_minimum_required(VERSION 2.8.11)

project(kdirchainrebuild)


set(CMAKE_AUTOMOC ON)


find_package(ECM 0.0.8 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})


find_package(Qt5Transitional REQUIRED Core)
find_package(Qt5Network REQUIRED)

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_DEFINITIONS} ${Qt5Quick_DEFINITIONS} ${QT_INCLUDES} ${KDE4_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Quick_DEFINITIONS} ${Qt5Qml_DEFINITIONS})


set (QT_MIN_VERSION "5.2.0")

find_package(kdeqt5staging REQUIRED NO_MODULE)

# WHY do i need so many dependencies? Crazy, huh?
find_package(KF5 MODULE REQUIRED COMPONENTS CMake Compiler InstallDirs
                                            ItemModels KWidgetsAddons KWindowSystem KCodecs KCoreAddons
                                            KConfig KAuth KDBusAddons KI18n KGuiAddons KConfigWidgets
                                            KService ItemViews KNotifications KIconThemes KCompletion KJobWidgets
                                            KIO)

#message("KIO lib:::: ${kio_LIBRARY}")
#message("KCoreAddons_LIBRARIES:::: ${KCoreAddons_LIBRARIES}")

# We do allow "signals and slots" instead of "Q_SIGNALS and Q_SLOTS".
remove_definitions(-DQT_NO_KEYWORDS -DQT_NO_CAST_FROM_ASCII -DQT_NO_SIGNALS_SLOTS_KEYWORDS)


################# setup the include directories #################
include_directories( ${kdeqt5staging_INCLUDE_DIRS}
                     ${kdeqt5staging_INCLUDE_DIR}
                     ${KDE4_INCLUDES}
                     ${KF5_INCLUDE_DIRS}  # since e-c-m 0.0.7
                     ${KF5_INCLUDE_DIRS}/KDE
                   )


# C++11 support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(kdirchainrebuild_SRCS
  kdirectory.cpp
  kdirectoryentry.cpp
  kdirectoryprivate_p.cpp
  kdirlisterv2.cpp
  kdirlisterv2_p.cpp
  main.cpp
)

add_executable(kdirchainrebuild ${kdirchainrebuild_SRCS})

target_link_libraries(kdirchainrebuild
  kdeqt5staging
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  kio
  KF5::KIOCore
  KF5::KIOWidgets
  kdecore
  kdeui
  kdeqt5staging
  KF5::KI18n
  ${KCoreAddons_LIBRARIES}

)
