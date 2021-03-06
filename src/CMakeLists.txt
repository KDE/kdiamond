add_subdirectory(pics)

set(kdiamond_SRCS
	board.cpp
	diamond.cpp
	game.cpp
	game-state.cpp
	infobar.cpp
	main.cpp
	mainwindow.cpp
	view.cpp
	kdiamond.qrc
)

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KDIAMOND VERSION_HEADER kdiamond_version.h)

kconfig_add_kcfg_files(kdiamond_SRCS settings.kcfgc)

file(GLOB ICONS_SRCS "pics/*-apps-kdiamond.png")
ecm_add_app_icon(kdiamond_SRCS ICONS ${ICONS_SRCS})

add_executable(kdiamond ${kdiamond_SRCS})
target_link_libraries(kdiamond
    KF5KDEGames
    KF5::NotifyConfig
    KF5::I18n
    KF5::XmlGui
    KF5::Notifications
    KF5::DBusAddons
    KF5::Crash
)

install(TARGETS kdiamond  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES kdiamond.knsrc  DESTINATION ${KDE_INSTALL_DATADIR}/knsrcfiles)
install(PROGRAMS org.kde.kdiamond.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kdiamond.appdata.xml  DESTINATION  ${KDE_INSTALL_METAINFODIR})
install(FILES kdiamond.kcfg DESTINATION ${KDE_INSTALL_DATADIR}/kdiamond)
install(FILES kdiamond.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR})
