cmake_minimum_required(VERSION 3.5)

project(MLP VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS
  Widgets
  Charts
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
  Widgets
  Charts
)

include_directories(
  ${CMAKE_SOURCE_DIR}/controller
  ${CMAKE_SOURCE_DIR}/matrix
  ${CMAKE_SOURCE_DIR}/network
  ${CMAKE_SOURCE_DIR}/drowarea
  ${CMAKE_SOURCE_DIR}/primitive
  ${CMAKE_SOURCE_DIR}/normalaizeimage
  ${CMAKE_SOURCE_DIR}/timeceeper
  ${CMAKE_SOURCE_DIR}/chartarea
  ${CMAKE_SOURCE_DIR}/tablecontroller
  ${CMAKE_SOURCE_DIR}/statecontrol
)

set(CMAKE_AUTOUIC_SEARCH_PATHS forms)

set(SOURCES
  main.cc
  controller/controller.cc
  matrix/matrix.cc
  network/network.cc
  drowarea/drowarea.cc
  primitive/primitive.cc
  normalaizeimage/normalaizeimage.cc
  timeceeper/timeceeper.cpp
  chartarea/chartarea.cpp
  tablecontroller/tablecontroller.cpp
  statecontrol/statecontrol.cc
)

set(HEADERS
  controller/controller.h
  matrix/matrix.h
  network/network.h
  drowarea/drowarea.h
  primitive/primitive.h
  normalaizeimage/normalaizeimage.h
  timeceeper/timeceeper.h
  chartarea/chartarea.h
  tablecontroller/tablecontroller.h
  statecontrol/statecontrol.h
)

set(FORMS
  forms/view.ui
)

set(PROJECT_SOURCES
  ${SOURCES}
  ${HEADERS}
  ${FORMS}
)

if (APPLE)
  set(MACOSX_BUNDLE_ICON_FILE neuro_.png)
  set(app_icon_macos "${CMAKE_SOURCE_DIR}/img/neuro_.png")
  set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
  )

# Define target properties for Android with Qt 6 as:
# set_property(TARGET src APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
  if(ANDROID)
    add_library(${PROJECT_NAME} SHARED
      ${PROJECT_SOURCES}
    )

  # Define properties for Android with Qt 5 after find_package() calls as:
  # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
    add_executable(${PROJECT_NAME}
      ${PROJECT_SOURCES}
    )
  endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Charts
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  MACOSX_BUNDLE TRUE
  WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(${PROJECT_NAME})
endif()

