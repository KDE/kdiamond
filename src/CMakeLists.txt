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
)

kconfig_add_kcfg_files(kdiamond_SRCS settings.kcfgc)

#kde4_add_app_icon(kdiamond_SRCS "pics/hi*-apps-kdiamond.png")

add_executable(kdiamond ${kdiamond_SRCS})
target_link_libraries(kdiamond  KF5KDEGames KF5::NotifyConfig KF5::I18n KF5::XmlGui KF5::Notifications)

install(TARGETS kdiamond  ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES kdiamond.knsrc  DESTINATION ${CONFIG_INSTALL_DIR})
install(PROGRAMS org.kde.kdiamond.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES kdiamond.kcfg DESTINATION ${DATA_INSTALL_DIR}/kdiamond)
install(FILES kdiamondui.rc  DESTINATION ${KXMLGUI_INSTALL_DIR}/kdiamond)
install(FILES kdiamond.notifyrc DESTINATION ${KNOTIFYRC_INSTALL_DIR})
