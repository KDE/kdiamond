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
    KF5KDEGames
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::NotifyConfig
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::Notifications
    KF${KF_MAJOR_VERSION}::DBusAddons
    KF${KF_MAJOR_VERSION}::Crash
)

install(TARGETS kdiamond  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES kdiamond.knsrc  DESTINATION ${KDE_INSTALL_DATADIR}/knsrcfiles)
install(PROGRAMS org.kde.kdiamond.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kdiamond.appdata.xml  DESTINATION  ${KDE_INSTALL_METAINFODIR})
install(FILES kdiamond.kcfg DESTINATION ${KDE_INSTALL_DATADIR}/kdiamond)
install(FILES kdiamond.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
