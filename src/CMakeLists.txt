add_subdirectory(pics)
include_directories(${KDEGAMES_INCLUDE_DIR})

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

kde4_add_kcfg_files(kdiamond_SRCS settings.kcfgc)

kde4_add_app_icon(kdiamond_SRCS "pics/hi*-app-kdiamond.png")

kde4_add_executable(kdiamond ${kdiamond_SRCS})
target_link_libraries(kdiamond ${KDE4_KDEUI_LIBS} kdegames ${KDE4_KNOTIFYCONFIG_LIBS} ${KDE4_KNEWSTUFF2_LIBS})

install(TARGETS kdiamond  ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES kdiamond.kcfg kdiamond.notifyrc kdiamondui.rc  DESTINATION ${DATA_INSTALL_DIR}/kdiamond)
install(FILES kdiamond.knsrc  DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES kdiamond.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
