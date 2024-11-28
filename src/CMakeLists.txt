add_subdirectory(pics)

ecm_setup_version(${RELEASE_SERVICE_VERSION}
    VARIABLE_PREFIX KDIAMOND
    VERSION_HEADER kdiamond_version.h
)

add_executable(kdiamond)

target_sources(kdiamond PRIVATE
    board.cpp
    board.h
    diamond.cpp
    diamond.h
    game.cpp
    game.h
    game-state.cpp
    game-state.h
    infobar.cpp
    infobar.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    view.cpp
    view.h

    kdiamond.qrc
)

kconfig_add_kcfg_files(kdiamond settings.kcfgc)

file(GLOB ICONS_SRCS "pics/*-apps-kdiamond.png")
ecm_add_app_icon(kdiamond ICONS ${ICONS_SRCS})

target_link_libraries(kdiamond
    KDEGames6
    KF6::CoreAddons
    KF6::NotifyConfig
    KF6::I18n
    KF6::XmlGui
    KF6::Notifications
    KF6::DBusAddons
    KF6::Crash
    KF6::IconThemes
)

if(APPLE)
    set_target_properties(kdiamond PROPERTIES
        MACOSX_BUNDLE_DISPLAY_NAME "KDiamond"
        MACOSX_BUNDLE_BUNDLE_NAME "KDiamond"
        MACOSX_BUNDLE_LONG_VERSION_STRING "KDiamond ${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.kdiamond"
	MACOSX_BUNDLE_COPYRIGHT "2024 The KDiamond Developers")
endif()

install(TARGETS kdiamond  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES kdiamond.knsrc  DESTINATION ${KDE_INSTALL_DATADIR}/knsrcfiles)
install(PROGRAMS org.kde.kdiamond.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kdiamond.appdata.xml  DESTINATION  ${KDE_INSTALL_METAINFODIR})
install(FILES kdiamond.kcfg DESTINATION ${KDE_INSTALL_DATADIR}/kdiamond)
install(FILES kdiamond.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
