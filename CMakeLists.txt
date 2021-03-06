CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET ( TOOLKIT_VERSION_MAJOR 1 )
SET ( TOOLKIT_VERSION_MINOR 1 )
SET ( TOOLKIT_VERSION_PATCH 3 )
SET ( TOOLKIT_VERSION
	${TOOLKIT_VERSION_MAJOR}.${TOOLKIT_VERSION_MINOR}.${TOOLKIT_VERSION_PATCH} )

PROJECT ( RBDL-Toolkit VERSION ${TOOLKIT_VERSION} )

CMAKE_MINIMUM_REQUIRED (VERSION 3.0)
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

OPTION(USE_QT6 "if on build toolkit using qt6 else qt5")

IF(APPLE)
OPTION(BUILD_OSX_BUNDLE "Build Application as osx app bundle" OFF)
ENDIF(APPLE)

IF(MSVC)
	ADD_DEFINITIONS (-DWINDOWS_BUILD)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /permissive- /std:c++17")
ENDIF(MSVC)

SET(CMAKE_CXX_STANDARD 17)

# The QT Interface
SET( QT_USE_QTXML TRUE )
SET( QT_WRAP_CPP TRUE )
SET(CMAKE_AUTOMOC ON )
SET(CMAKE_AUTORCC ON )
SET(CMAKE_AUTOUIC ON )

# add flag for relocatable code -- required to work with shared Qt libraries
SET(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} "-fPIC" )

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTOOLKIT_DEBUG")
SET(CUSTOM_RBDL_PATH "" CACHE PATH "Path to specific RBDL Installation")

SET(CUSTOM_QT_PATH "" CACHE PATH "Path to Qt Installation")

IF (CUSTOM_QT_PATH)
	SET(CMAKE_CXX_STANDARD_REQUIRED ON)
	SET(CMAKE_SKIP_BUILD_RPATH FALSE)
	SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
	SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
	SET(CMAKE_PREFIX_PATH "${CUSTOM_QT_PATH}/lib/cmake")
ENDIF(CUSTOM_QT_PATH)

FIND_PACKAGE (RBDL COMPONENTS LUAMODEL URDFREADER REQUIRED)
FIND_PACKAGE (Eigen3 REQUIRED)
FIND_PACKAGE (Boost REQUIRED)

IF(USE_QT6)
  FIND_PACKAGE (Qt6 REQUIRED COMPONENTS Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput Concurrent )
  ADD_DEFINITIONS (${QT_DEFINITIONS})
  ADD_DEFINITIONS ( -DUSE_QT6 )
  SET( QT_INCLUDE_DIR
	${Qt6Core_INCLUDE_DIRS}
	${Qt6Gui_INCLUDE_DIRS}
	${Qt6Widgets_INCLUDE_DIRS}
	${Qt63DCore_INCLUDE_DIRS}
	${Qt63DExtras_INCLUDE_DIRS}
	${Qt63DRender_INCLUDE_DIRS}
	${Qt63DInput_INCLUDE_DIRS}
	${Qt6Concurrent_INCLUDE_DIRS}
	)

  SET( QT_LIBRARIES
    ${Qt6Core_LIBRARIES}
	${Qt6Gui_LIBRARIES}
	${Qt6Widgets_LIBRARIES}
	${Qt63DCore_LIBRARIES}
    ${Qt63DExtras_LIBRARIES}
	${Qt63DRender_LIBRARIES}
	${Qt63DInput_LIBRARIES}
	${Qt6Concurrent_LIBRARIES}
	)
  QT6_WRAP_UI ( UI_SRC
	ui/Timeline.ui
	ui/ModelSelector.ui
	ui/SceneGroupList.ui
	ui/SettingsEditor.ui
	ui/VectorEditor.ui
	)

  #Sources that contain q objects
  QT6_WRAP_CPP ( MOC_SRCS
	include/ToolkitApp.h
	include/SceneWidget.h
	include/ModelSelectorDialog.h
	include/Timeline.h
	include/VectorEditor.h
	include/rbdl_wrapper.h
	include/SceneGroupsWidget.h
	include/variantdelegate.h
	include/ToolkitSettings.h
	)
ELSE (USE_QT6)
  FIND_PACKAGE (Qt5 REQUIRED COMPONENTS Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput Concurrent )
  ADD_DEFINITIONS (${QT_DEFINITIONS})

  SET( QT_INCLUDE_DIR
	${Qt5Core_INCLUDE_DIRS}
	${Qt5Gui_INCLUDE_DIRS}
	${Qt5Widgets_INCLUDE_DIRS}
	${Qt53DCore_INCLUDE_DIRS}
	${Qt53DExtras_INCLUDE_DIRS}
	${Qt53DRender_INCLUDE_DIRS}
	${Qt53DInput_INCLUDE_DIRS}
	${Qt5Concurrent_INCLUDE_DIRS}
	)

  SET( QT_LIBRARIES
    ${Qt5Core_LIBRARIES}
	${Qt5Gui_LIBRARIES}
	${Qt5Widgets_LIBRARIES}
	${Qt53DCore_LIBRARIES}
    ${Qt53DExtras_LIBRARIES}
	${Qt53DRender_LIBRARIES}
	${Qt53DInput_LIBRARIES}
	${Qt5Concurrent_LIBRARIES}
	)
  QT5_WRAP_UI ( UI_SRC
	ui/Timeline.ui
	ui/ModelSelector.ui
	ui/SceneGroupList.ui
	ui/SettingsEditor.ui
	ui/VectorEditor.ui
	)

  #Sources that contain q objects
  QT5_WRAP_CPP ( MOC_SRCS
	include/ToolkitApp.h
	include/SceneWidget.h
	include/ModelSelectorDialog.h
	include/Timeline.h
	include/VectorEditor.h
	include/rbdl_wrapper.h
	include/SceneGroupsWidget.h
	include/variantdelegate.h
	include/ToolkitSettings.h
	)
ENDIF (USE_QT6)

CONFIGURE_FILE (
	"${CMAKE_CURRENT_SOURCE_DIR}/include/toolkit_config.h.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/include/toolkit_config.h"
	)

INCLUDE_DIRECTORIES (${QT_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR}/include)
INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/vendor/urdfparser/include)
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR}/ui)
INCLUDE_DIRECTORIES (${RBDL_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${EIGEN3_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIR})

IF ( ${CMAKE_VERSION} VERSION_LESS 3.12.0 )
	ADD_DEFINITIONS (-DTOOKIT_VERSION="${TOOLKIT_VERSION}" -DINSTALL_DIR="${CMAKE_INSTALL_PREFIX}/share/ORB/rbdl-toolkit")
ELSE( ${CMAKE_VERSION} VERSION_LESS 3.12.0 )
	ADD_COMPILE_DEFINITIONS (TOOKIT_VERSION="${TOOLKIT_VERSION}" INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/share/ORB/rbdl-toolkit")
ENDIF( ${CMAKE_VERSION} VERSION_LESS 3.12.0 )

IF(NOT APPLE)
IF(NOT MSVC)
CONFIGURE_FILE(extra/mk_appimage.sh mk_appimage.sh)
CONFIGURE_FILE(extra/mk_pluginbuilder.sh mk_pluginbuilder.sh)
ENDIF(NOT MSVC)
ENDIF(NOT APPLE)

ADD_SUBDIRECTORY (
  vendor
)

SET ( ToolkitLib_SRCS
	src/ToolkitApp.cc
	src/Timeline.cc
	src/ToolkitSettings.cc
	src/SceneWidget.cc
	src/ModelSelectorDialog.cc
	src/util.cc
	src/render_util.cc
	src/math_util.cc
	src/rbdl_wrapper.cc
	src/variantdelegate.cc
	src/toolkit_errors.cc
	src/luamodel_wrapper.cc
	src/urdfmodel_wrapper.cc
)

SET ( Toolkit_SRCS
	src/main.cc
)

ADD_LIBRARY ( toolkitlib SHARED
	${ToolkitLib_SRCS}
	${MOC_SRCS}
	${UI_SRC}
)

TARGET_COMPILE_DEFINITIONS (toolkitlib PUBLIC TOOLKIT_COMPILE_LIB)

TARGET_LINK_LIBRARIES ( toolkitlib
	${QT_LIBRARIES}
	${RBDL_LIBRARY}
	${RBDL_LUAMODEL_LIBRARY}
	${RBDL_URDFREADER_LIBRARY}
	urdfparser
)

ADD_SUBDIRECTORY (
  plugins
)


IF(BUILD_OSX_BUNDLE)
	CONFIGURE_FILE(extra/mk_dmg.sh mk_dmg.sh)
	SET( MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
	SET( MACOSX_BUNDLE_GUI_IDENTIFIER orb.rbdl-toolkit)
	SET( MACOSX_BUNDLE_INFO_STRING "RBDL-Toolkit Application for rigid body modeling")
	SET( MACOSX_BUNDLE_ICON_FILE "rbdl-toolkit.icns")
	SET( MACOSX_BUNDLE_LONG_VERISO ${TOOLKIT_VERSION})
	SET( MACOSX_BUNDLE_SHORT_VERISO ${TOOLKIT_VERSION})
	SET( MACOSX_BUNDLE_BUNDLE_VERISO ${TOOLKIT_VERSION})
	ADD_EXECUTABLE ( rbdl-toolkit
		MACOSX_BUNDLE
		${Toolkit_SRCS}
		extra/rbdl-toolkit.icns
	)
	SET_TARGET_PROPERTIES(
		rbdl-toolkit
		PROPERTIES
		RESOURCE "extra/rbdl-toolkit.icns"
	)
	ADD_CUSTOM_TARGET(dmg
		BYPRODUCTS rbdl-toolkit.dmg rbdl-toolkit.app
		COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mk_dmg.sh
	)
ELSE(BUILD_OSX_BUNDLE)
	ADD_EXECUTABLE ( rbdl-toolkit
		${Toolkit_SRCS}
	)
ENDIF(BUILD_OSX_BUNDLE)

TARGET_LINK_LIBRARIES ( rbdl-toolkit
	toolkitlib
)

IF (CUSTOM_QT_PATH)
ADD_CUSTOM_TARGET(appimage
	BYPRODUCTS rbdl-toolkit-x86_64.AppImage rbdl-toolkit.AppDir
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mk_appimage.sh -customqt
)
ELSE(CUSTOM_QT_PATH)
ADD_CUSTOM_TARGET(appimage
	BYPRODUCTS rbdl-toolkit-x86_64.AppImage rbdl-toolkit.AppDir
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mk_appimage.sh
)
ENDIF(CUSTOM_QT_PATH)

IF(NOT BUILD_OSX_BUNDLE)
INSTALL ( TARGETS rbdl-toolkit RUNTIME DESTINATION bin )
IF(MVSC)
INSTALL ( TARGETS toolkitlib ARCHIVE DESTINATION lib )
INSTALL ( TARGETS toolkitlib LIBRARY DESTINATION lib )
INSTALL ( TARGETS toolkitlib RUNTIME DESTINATION bin )
ELSE()
INSTALL ( TARGETS toolkitlib LIBRARY DESTINATION lib )
ENDIF()
#IF(WIN32)
#CONFIGURE_FILE(extra/mk_win_installer.bat mk_win_installer.bat)
## the windeployqt executable
#GET_TARGET_PROPERTY(QMAKE_EXE Qt5::qmake IMPORTED_LOCATION)
#GET_FILENAME_COMPONENT(QT_BIN_DIR "${QMAKE_EXE}" DIRECTORY)
#
#FIND_PROGRAM(WINDEPLOYQT_ENV_SETUP qtenv2.bat HINTS "${QT_BIN_DIR}")
#FIND_PROGRAM(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")
#
## Run windeployqt immediately after build
#ADD_CUSTOM_TARGET(package
	#BYPRODUCTS RBDLToolkit_Deploy
    #COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mk_win_installer.bat)
#
#ENDIF()
ENDIF()

FILE ( GLOB MESH_FILES meshes/*.obj )
IF(BUILD_OSX_BUNDLE)
INSTALL ( FILES ${MESH_FILES} DESTINATION ${CMAKE_BINARY_DIR}/rbdl-toolkit.app/Contents/Resources/meshes )
ELSE(BUILD_OSX_BUNDLE)
INSTALL ( FILES ${MESH_FILES} DESTINATION share/ORB/rbdl-toolkit/meshes )
ENDIF(BUILD_OSX_BUNDLE)
