PROJECT( tomahawk )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

IF( "${gui}" STREQUAL "no" )
    SET( QT_DONT_USE_QTGUI TRUE )
ENDIF()

IF( UNIX AND NOT APPLE )
  SET( QT_USE_QTDBUS TRUE )
ENDIF( UNIX AND NOT APPLE )
SET( QT_USE_QTSQL TRUE )
SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTWEBKIT TRUE )

INCLUDE( ${QT_USE_FILE} )
INCLUDE( ${CMAKE_MODULE_PATH}/AddAppIconMacro.cmake )

#SET( CMAKE_BUILD_TYPE "Release" )
SET( CMAKE_VERBOSE_MAKEFILE ON )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )
IF( NOT CMAKE_BUILD_TYPE STREQUAL "Release" )
    MESSAGE( "Building in debug mode, enabling all debug updates" )
    SET( DEBUG_BUILD ON )
ENDIF()

# build plugins
# use glob, but hardcoded list for now:
#FILE( GLOB plugindirs "src/plugins/*" )
#FOREACH( moddir ${plugindirs} )
#    MESSAGE( status "Building plugin: ${moddir}" )
#    ADD_SUBDIRECTORY( ${moddir} )
#ENDFOREACH( moddir )

SET( tomahawkSources ${tomahawkSources}
     web/api_v1.cpp

     resolvers/scriptresolver.cpp
     resolvers/qtscriptresolver.cpp

     musicscanner.cpp
     shortcuthandler.cpp
     scanmanager.cpp
     tomahawkapp.cpp
     main.cpp
)

IF(LIBLASTFM_FOUND)
    SET(tomahawkSources ${tomahawkSources}
        scrobbler.cpp
    )
ENDIF(LIBLASTFM_FOUND)

SET( tomahawkSourcesGui ${tomahawkSourcesGui}
     sourcetree/sourcesmodel.cpp
     sourcetree/sourcesproxymodel.cpp
     sourcetree/sourcetreeview.cpp
     sourcetree/items/sourcetreeitem.cpp
     sourcetree/items/collectionitem.cpp
     sourcetree/items/playlistitems.cpp
     sourcetree/items/categoryitems.cpp
     sourcetree/items/genericpageitems.cpp

     transferview.cpp
     PipelineStatusView.cpp
     tomahawktrayicon.cpp
     audiocontrols.cpp
     settingsdialog.cpp
     diagnosticsdialog.cpp
     configdelegatebase.cpp
     sipconfigdelegate.cpp
     resolverconfigdelegate.cpp
     resolversmodel.cpp
     tomahawkwindow.cpp
)

SET( tomahawkHeaders ${tomahawkHeaders}
     tomahawkapp.h

     web/api_v1.h

     resolvers/scriptresolver.h
     resolvers/qtscriptresolver.h

     musicscanner.h
     scanmanager.h
     shortcuthandler.h
)

IF(LIBLASTFM_FOUND)
    SET(tomahawkHeaders ${tomahawkHeaders}
        scrobbler.h
    )
ENDIF(LIBLASTFM_FOUND)


SET( tomahawkHeadersGui ${tomahawkHeadersGui}
     sourcetree/sourcesmodel.h
     sourcetree/sourcesproxymodel.h
     sourcetree/sourcetreeview.h
     sourcetree/items/sourcetreeitem.h
     sourcetree/items/collectionitem.h
     sourcetree/items/playlistitems.h
     sourcetree/items/categoryitems.h
     sourcetree/items/genericpageitems.h

     transferview.h
     PipelineStatusView.h
     tomahawktrayicon.h
     audiocontrols.h
     settingsdialog.h
     diagnosticsdialog.h
     configdelegatebase.h
     resolverconfigdelegate.h
     sipconfigdelegate.h
     resolversmodel.h
     delegateconfigwrapper.h
     tomahawkwindow.h
)

SET( tomahawkUI ${tomahawkUI}
     tomahawkwindow.ui
     diagnosticsdialog.ui
     stackedsettingsdialog.ui
     proxydialog.ui
     searchbox.ui

     audiocontrols.ui
)

INCLUDE_DIRECTORIES(
    .
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/thirdparty/liblastfm2/src

    audio
    database
    playlist
    network
    sourcetree
    topbar
    utils
    libtomahawk
    mac

    ${THIRDPARTY_DIR}/qxt/qxtweb-standalone/qxtweb
    ${THIRDPARTY_DIR}/qtweetlib/qtweetlib/src
    ${THIRDPARTY_DIR}/qtweetlib/tomahawk-custom

    ${TAGLIB_INCLUDES}
    ${PHONON_INCLUDES}
    ${QJSON_INCLUDE_DIR}
    ${LIBECHONEST_INCLUDE_DIR}
    ${LIBECHONEST_INCLUDE_DIR}/..
)

SET( OS_SPECIFIC_LINK_LIBRARIES "" )

IF( WIN32 )
  INCLUDE( "CMakeLists.win32.txt" )
ENDIF( WIN32 )
IF( UNIX )
  INCLUDE( "CMakeLists.unix.txt" )
ENDIF( UNIX )

IF( APPLE )
    INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/thirdparty/SPMediaKeyTap )

    SET( tomahawkHeaders ${tomahawkHeaders} mac/tomahawkapp_mac.h mac/macshortcuthandler.h  )
    SET( tomahawkSources ${tomahawkSources} mac/tomahawkapp_mac.mm  mac/macshortcuthandler.cpp )

    IF(HAVE_SPARKLE)
        SET( tomahawkHeaders ${tomahawkHeaders} ${SPARKLE}/Headers )
    ENDIF(HAVE_SPARKLE)

ENDIF( APPLE )

IF(GLOOX_FOUND)
  INCLUDE_DIRECTORIES( ${GLOOX_INCLUDE_DIR} )
  SET( tomahawkHeaders ${tomahawkHeaders} xmppbot/xmppbot.h )
  SET( tomahawkSources ${tomahawkSources} xmppbot/xmppbot.cpp )
ENDIF(GLOOX_FOUND)
#ADD_SUBDIRECTORY( sip )

kde4_add_app_icon( tomahawkSources "${CMAKE_SOURCE_DIR}/data/icons/tomahawk-icon-*.png" )
qt4_add_resources( RC_SRCS "../resources.qrc" )
qt4_wrap_cpp( tomahawkMoc ${tomahawkHeaders} )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include( ${CMAKE_SOURCE_DIR}/lang/translations.cmake )

SET( final_src ${final_src} ${tomahawkMoc} ${tomahawkSources} ${tomahawkHeaders} ${trans_outfile})

IF( "${gui}" STREQUAL "no" )
ELSE()
    qt4_wrap_ui( tomahawkUI_H ${tomahawkUI} )
    qt4_wrap_cpp( tomahawkMocGui ${tomahawkHeadersGui} )
    SET( final_src ${final_src} ${tomahawkUI_H} ${tomahawkMocGui} ${tomahawkSourcesGui} ${RC_SRCS} )
ENDIF()


MESSAGE( STATUS "OS_SPECIFIC_LINK_LIBRARIES: ${OS_SPECIFIC_LINK_LIBRARIES}" )

SET(LINK_LIBRARIES  "")
IF(LIBLASTFM_FOUND)
    SET(LINK_LIBRARIES ${LINK_LIBRARIES} tomahawk_lastfm2 )
ENDIF(LIBLASTFM_FOUND)
IF(GLOOX_FOUND)
    SET(LINK_LIBRARIES ${LINK_LIBRARIES} ${GLOOX_LIBRARIES} )
ENDIF(GLOOX_FOUND)




IF( APPLE )
  IF(HAVE_SPARKLE)
    MESSAGE("Sparkle Found, installing framekwork in bundle")
    INSTALL(DIRECTORY "${SPARKLE}/Versions/Current/Resources" DESTINATION "${CMAKE_BINARY_DIR}/tomahawk.app/Contents/Frameworks/Sparkle.framework")

  ENDIF(HAVE_SPARKLE)
ENDIF( APPLE )

IF( UNIX AND NOT APPLE AND KDE4_INSTALLED ) #install protocol file
  FILE( READ ${CMAKE_SOURCE_DIR}/admin/unix/tomahawk.protocol protocol )
  STRING( REPLACE "/path/to/binary" # match this
              "${CMAKE_INSTALL_PREFIX}/bin/tomahawk" # this is linux (kde) so pretty safe I think
              edited_protocol # save in this variable
              "${protocol}" # from the contents of this var
          )
  FILE( WRITE ${CMAKE_BINARY_DIR}/tomahawk.protocol "${edited_protocol}" )
  IF( ${SERVICES_INSTALL_DIR} )
    SET( PROTOCOL_INSTALL_DIR ${SERVICES_INSTALL_DIR} )
  ELSE()
    SET( PROTOCOL_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/kde4/services" )
  ENDIF()
  INSTALL( FILES ${CMAKE_BINARY_DIR}/tomahawk.protocol DESTINATION ${PROTOCOL_INSTALL_DIR} )
ENDIF()









SET( PROJECT_NAME  "Tomahawk")
STRING( TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE )

if(UNIX AND NOT APPLE)
        set(BIN_INSTALL_DIR "bin")
        set(DOC_INSTALL_DIR "share/doc/${PROJECT_NAME_LOWERCASE}/")
else()
        set(BIN_INSTALL_DIR ".")
        set(DOC_INSTALL_DIR ".")
endif()

IF(APPLE)
    SET( PROJECT_TARGET  ${PROJECT_NAME} )
ELSE()
    SET( PROJECT_TARGET  ${PROJECT_NAME_LOWERCASE} )
ENDIF()

if(APPLE)
    set(CMAKE_INSTALL_PREFIX "/Applications")
    set(EXECUTABLE "${PROJECT_TARGET}.app")
elseif(WIN32)
    set(EXECUTABLE "${PROJECT_TARGET}${CMAKE_EXECUTABLE_SUFFIX}")
else()
    set(EXECUTABLE "${BIN_INSTALL_DIR}/${PROJECT_TARGET}${CMAKE_EXECUTABLE_SUFFIX}")
endif()

ADD_EXECUTABLE( ${PROJECT_TARGET} WIN32 MACOSX_BUNDLE ${final_src} )
TARGET_LINK_LIBRARIES( ${PROJECT_TARGET}
    ${LINK_LIBRARIES}
    ${TOMAHAWK_LIBRARIES}
    ${PHONON_LIBS}
    ${OS_SPECIFIC_LINK_LIBRARIES}
    ${QT_LIBRARIES}
    ${MAC_EXTRA_LIBS}
    ${LIBECHONEST_LIBRARY}
    ${QXTWEB_LIBRARIES}
    ${QJSON_LIBRARIES}
    ${TAGLIB_LIBRARIES}
    ${CLUCENE_LIBRARIES}
)

#SET_TARGET_PROPERTIES(tomahawk PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_BINARY_DIR}/Info.plist"


SET(PROJECT_BUNDLE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}")
SET(PROJECT_PLUGIN_DIR "${EXECUTABLE}/Contents/Plugins")


if(APPLE)
        set(MACOSX_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
        set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
        set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
        set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
        set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
        set(MACOSX_BUNDLE_ICON_FILE "tomahawkSources.icns")
        set(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
        set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")

        set(MACOSX_BUNDLE_RESOURCES "${PROJECT_BUNDLE_DIR}/Contents/Resources")
        #set(MACOSX_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/data/icons/${MACOSX_BUNDLE_ICON_FILE}")
        execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${MACOSX_BUNDLE_RESOURCES})
        #execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MACOSX_BUNDLE_ICON} ${MACOSX_BUNDLE_RESOURCES})
endif()


message(STATUS "${PROJECT_NAME} will be installed to ${CMAKE_INSTALL_PREFIX}")
install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_DIR})

# Only support CPack packaging on newer versions of CMake.
if(NOT "${CMAKE_VERSION}" VERSION_LESS "2.8.4")
    set(CPACK_GENERATOR "TBZ2")
    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_VENDOR "${PROJECT_VENDOR}")
    #set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/${README_FILE}")
    if(WIN32)
        set(CPACK_GENERATOR "NSIS")
        set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "${PROJECT_NAME}")
        set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
        set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME} ${PROJECT_VERSION}")
        set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/${LICENSE_FILE}")
        set(CPACK_NSIS_EXECUTABLES_DIRECTORY "${BIN_INSTALL_DIR}")
        #set(CPACK_NSIS_MUI_ICON "${PROJECT_ICONS_DIRECTORY}/NSIS.ico")
        #set(CPACK_PACKAGE_ICON "${PROJECT_ICONS_DIRECTORY}\\\\NSISHeader.bmp")
        set(CPACK_NSIS_URL_INFO_ABOUT "http://${PROJECT_DOMAIN}")
        set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
        set(CPACK_NSIS_MENU_LINKS "${LICENSE_FILE}" "License" "${README_FILE}" "Readme")
        set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${CPACK_NSIS_INSTALLED_ICON_NAME}")
    endif()
    if(APPLE)
        set(CMAKE_INSTALL_PREFIX "/Applications")
        set(CPACK_GENERATOR "DragNDrop")
        set(CPACK_DMG_FORMAT "UDBZ")
        set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
        set(CPACK_SYSTEM_NAME "OSX")
        set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
        set(CPACK_PACKAGE_ICON ${MACOSX_BUNDLE_ICON})
        set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/admin/mac/DS_Store.in")
        set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/admin/mac/dmg_background.png")
    endif()

    include(CPack)

    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${BIN_INSTALL_DIR}")
    include(InstallRequiredSystemLibraries)

endif()



IF(APPLE OR WIN32)
    include(DeployQt4)
    install_qt4_app( ${EXECUTABLE} "" "" )
ENDIF()





#INCLUDE( "CPack.txt" )
