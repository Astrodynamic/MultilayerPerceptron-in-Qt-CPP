cmake_minimum_required(VERSION 3.15)

project(MLP VERSION 1.0.0 LANGUAGES CXX)

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
  ${CMAKE_SOURCE_DIR}/normalaizeimage
  ${CMAKE_SOURCE_DIR}/tablecontroller
  ${CMAKE_SOURCE_DIR}/statecontrol
  ${CMAKE_SOURCE_DIR}/timeceeper
  ${CMAKE_SOURCE_DIR}/controller
  ${CMAKE_SOURCE_DIR}/primitive
  ${CMAKE_SOURCE_DIR}/chartarea
  ${CMAKE_SOURCE_DIR}/drowarea
  ${CMAKE_SOURCE_DIR}/network
  ${CMAKE_SOURCE_DIR}/matrix
)

set(CMAKE_AUTOUIC_SEARCH_PATHS forms)

set(SOURCES
  ${CMAKE_SOURCE_DIR}/controller/controller.cc
  ${CMAKE_SOURCE_DIR}/matrix/matrix.cc
  ${CMAKE_SOURCE_DIR}/network/network.cc
  ${CMAKE_SOURCE_DIR}/drowarea/drowarea.cc
  ${CMAKE_SOURCE_DIR}/primitive/primitive.cc
  ${CMAKE_SOURCE_DIR}/normalaizeimage/normalaizeimage.cc
  ${CMAKE_SOURCE_DIR}/timeceeper/timeceeper.cpp
  ${CMAKE_SOURCE_DIR}/chartarea/chartarea.cpp
  ${CMAKE_SOURCE_DIR}/tablecontroller/tablecontroller.cpp
  ${CMAKE_SOURCE_DIR}/statecontrol/statecontrol.cc
)

set(HEADERS
  ${CMAKE_SOURCE_DIR}/controller/controller.h
  ${CMAKE_SOURCE_DIR}/matrix/matrix.h
  ${CMAKE_SOURCE_DIR}/network/network.h
  ${CMAKE_SOURCE_DIR}/drowarea/drowarea.h
  ${CMAKE_SOURCE_DIR}/primitive/primitive.h
  ${CMAKE_SOURCE_DIR}/normalaizeimage/normalaizeimage.h
  ${CMAKE_SOURCE_DIR}/timeceeper/timeceeper.h
  ${CMAKE_SOURCE_DIR}/chartarea/chartarea.h
  ${CMAKE_SOURCE_DIR}/tablecontroller/tablecontroller.h
  ${CMAKE_SOURCE_DIR}/statecontrol/statecontrol.h
)

set(FORMS
  ${CMAKE_SOURCE_DIR}/forms/view.ui
)

set(PROJECT_SOURCES
  ${SOURCES}
  ${HEADERS}
  ${FORMS}
  ${CMAKE_SOURCE_DIR}/main.cc
)

if(APPLE)
  set(MACOSX_BUNDLE_ICON_FILE appicon.png)
  set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/data/icon/appicon.png")
  set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${app_icon_macos}
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

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
  -Wall
  -Werror
  -Wextra
  -Wpedantic
  -fsanitize=address
  -Wno-error=deprecated-declarations
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Charts
  -fsanitize=address
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

# Add cppcheck
find_program(CPPCHECK cppcheck)

if(CPPCHECK)
  message(STATUS "cppcheck found: ${CPPCHECK}")
  list(
    APPEND CPPCHECK_ARGS
    "--enable=all"
    "--inconclusive"
    "--quiet"
    "--language=c++"
    "--std=c++17"
    "--suppress=unusedFunction"
    "--suppress=missingInclude"
    "--suppress=unusedStructMember"
    "--suppress=variableScope"
    "--suppress=*:${PROJECT_SOURCE_DIR}/lib/*"
  )
  add_custom_target(
    cppcheck
    COMMAND ${CPPCHECK} ${CPPCHECK_ARGS} ${HEADERS} ${SOURCES}
  )
else()
  message(STATUS "cppcheck not found")
endif()

# Add clang-format
find_program(CLANG_FORMAT clang-format)

if(CLANG_FORMAT)
  message(STATUS "clang-format found: ${CLANG_FORMAT}")
  list(
    APPEND CLANG_ARGS
    "-i"
    "-style=Google"
    "--verbose"
  )
  add_custom_target(
    clang-format
    COMMAND ${CLANG_FORMAT} ${CLANG_ARGS} ${HEADERS} ${SOURCES}
  )
else()
  message(STATUS "clang-format not found")
endif()
