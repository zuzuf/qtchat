PROJECT(qtchat)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)

Message(STATUS "")
Message(STATUS "--- QtChat, serverless LAN chat ---")
Message(STATUS "")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

SET(QTCHAT_MAIL_BUGREPORT         "zuzuf86@gmail.com")
SET(QTCHAT_AUTHOR                 "Roland Brochard")

SET(QTCHAT_VERSION_HI     "1")     # 1.0.0
SET(QTCHAT_VERSION_LO     "0")
SET(QTCHAT_VERSION_PATCH  "0")
SET(QTCHAT_VERSION ${QTCHAT_VERSION_HI}.${QTCHAT_VERSION_LO}.${QTCHAT_VERSION_PATCH})
SET(PACKAGE_STRING "QtChat ${QTCHAT_VERSION_HI}.${QTCHAT_VERSION_LO}.${QTCHAT_VERSION_PATCH}")

SET(CPACK_PACKAGE_VERSION_MAJOR "${QTCHAT_VERSION_HI}")
SET(CPACK_PACKAGE_VERSION_MINOR "${QTCHAT_VERSION_LO}")
SET(CPACK_PACKAGE_VERSION_PATCH "${QTCHAT_VERSION_PATCH}")

SET(CPACK_PACKAGE_VERSION "${QTCHAT_VERSION_HI}.${QTCHAT_VERSION_LO}.${QTCHAT_VERSION_PATCH}")
SET(CPACK_PACKAGE_NAME "qtchat")

MESSAGE(STATUS "Version : v${QTCHAT_VERSION_HI}.${QTCHAT_VERSION_LO}.${QTCHAT_VERSION_PATCH}")

# Information about the current Operating System
Message(STATUS " System : ${CMAKE_SYSTEM} (${CMAKE_SYSTEM_PROCESSOR})")

# Qt5
set(QT_MIN_VERSION "5.2.0")
set(QT_USE_GUI 1)
set(QT_USE_QTNETWORK 1)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
set(QT_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

add_definitions(-DAPP_NAME=\"${CMAKE_PROJECT_NAME}\"
                -DORG_NAME=\"${QTCHAT_AUTHOR}\"
                -DAPP_VERSION=\"${QTCHAT_VERSION}\")

set(CMAKE_CXX_FLAGS "-std=gnu++11")

set(QTCHAT_SOURCES src/main.cpp
    src/mainwindow.cpp
    src/user.cpp
    src/messaginglayer.cpp
    src/chatroom.cpp
    src/settings.cpp
    src/usermanager.cpp
    src/textedit.cpp
    src/userlist.cpp
    src/userwidget.cpp
    src/sendfiledialog.cpp
    src/receivefiledialog.cpp
    src/userdetailsdialog.cpp)

set(QTCHAT_HEADERS src/mainwindow.h
    src/user.h
    src/messaginglayer.h
    src/chatroom.h
    src/settings.h
    src/usermanager.h
    src/textedit.h
    src/userlist.h
    src/userwidget.h
    src/sendfiledialog.h
    src/receivefiledialog.h
    src/userdetailsdialog.h)

set(QTCHAT_FORMS src/mainwindow.ui
    src/chatroom.ui
    src/settings.ui
    src/sendfiledialog.ui
    src/receivefiledialog.ui
    src/userdetailsdialog.ui)

set(QTCHAT_RESOURCES icons.qrc
    i18n.qrc)

file(GLOB QTCHAT_TRANSLATIONS RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/i18n/qtchat_*.ts)
file(GLOB QTCHAT_TRANSLATIONS_QM RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/i18n/*.qm)

file(WRITE i18n.qrc "<RCC>
    <qresource prefix=\"/\">
")

foreach(f IN LISTS QTCHAT_TRANSLATIONS_QM)
    file(APPEND i18n.qrc "        <file>${f}</file>
")
endforeach(f)

file(APPEND i18n.qrc "    </qresource>
</RCC>")

# build
include_directories(${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_BINARY_DIR})
qt5_add_translation(QTCHAT_QM ${QTCHAT_TRANSLATIONS})
qt5_wrap_ui(QTCHAT_UI ${QTCHAT_FORMS})
qt5_add_resources(QTCHAT_RCC ${QTCHAT_RESOURCES})

add_executable(${CMAKE_PROJECT_NAME} WIN32 MACOSX_BUNDLE
               ${QTCHAT_SOURCES} ${QTCHAT_UI}
               ${QTCHAT_RCC} ${QTCHAT_RC} ${QTCHAT_QM}
               ${QTCHAT_TRANSLATIONS}
              )
target_link_libraries(${CMAKE_PROJECT_NAME} ${QT_LIBRARIES})


# installation
IF(WIN32) # copy qt dlls and change settings for different projects
    SET(QTLIBLIST Qt5Core Qt5Gui Qt5Widgets Qt5Network Qt5Xml Qt5XmlPatterns Qt5OpenGL Qt5Svg phonon Qt5Script Qt5Sql Qt5Multimedia Qt5Declarative)
    FOREACH(qtlib ${QTLIBLIST})
        GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
        install(FILES ${QT_DLL_PATH_tmp}/${qtlib}.dll DESTINATION bin)
    ENDFOREACH(qtlib)
	
    FOREACH(extradll ${EXTRADLLS})
        install(FILES ${extradll} DESTINATION bin)
    ENDFOREACH(extradll)
	
    # installation
    #  binary
    install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin)
ENDIF(WIN32)

IF(UNIX)

    # installation
    #  binary
    install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin LIBRARY DESTINATION lib${LIB_SUFFIX})
    #  desktop file
    install(FILES distrib/linux/qtchat.desktop DESTINATION share/applications)
    # icon
    install(FILES icons/qtchat.png DESTINATION share/pixmaps)

ENDIF(UNIX)




#
# --- CPACK configuration ---
#
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "QtChat, simple serverless chat program over LAN networks")
SET(CPACK_PACKAGE_VENDOR "Roland Brochard")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${PACKAGE_STRING}")

IF(UNIX)
    SET(CPACK_GENERATOR "DEB")
    SET(CPACK_PACKAGE_CONTACT "zuzuf86@gmail.com")
    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Roland Brochard")
    SET(CPACK_DEBIAN_PACKAGE_SECTION "network")
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5-core, libqt5-widgets, libqt5-gui, libqt5-network")
    SET(CPACK_PACKAGE_EXECUTABLES "qtchat")

ELSEIF(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_PACKAGE_NAME "QtChat")
    set(CPACK_NSIS_DISPLAY_NAME "QtChat")
    set(CPACK_NSIS_CONTACT "zuzuf86@gmail.com")
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY "QtChat")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")

    SET(CPACK_PACKAGE_EXECUTABLES "qtchat" "QtChat")

    set(CPACK_NSIS_MODIFY_PATH "ON")

    # this works
    set(CPACK_NSIS_MENU_LINKS "http://www.zuzuf.net/qtchat" "Homepage for QtChat")

    # this doesn't
    set(CPACK_CREATE_DESKTOP_LINKS "qtchat")

    # Icon in the add/remove control panel. Must be an .exe file
    set(CPACK_NSIS_INSTALLED_ICON_NAME bin\\\\qtchat.exe)

    set(CPACK_NSIS_URL_INFO_ABOUT "http://www.zuzuf.net/qtchat")
    set(CPACK_NSIS_HELP_LINK "http://www.zuzuf.net/qtchat")
ENDIF(UNIX)

SET(CPACK_STRIP_FILES "qtchat")
SET(CPACK_SOURCE_STRIP_FILES "")

SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
SET(CPACK_IGNORE_FILES "/CVS/;/\\\\.svn/;/\\\\.git/;\\\\.swp$;\\\\.#;/#;\\\\.tar.gz$;/CMakeFiles/;CMakeCache.txt;refresh-copyright-and-license.pl;build;release;")
SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_IGNORE_FILES})
# simulate autotools' "make dist"
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

INCLUDE(CPack)
