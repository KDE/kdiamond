add_subdirectory(pics)
include_directories(${KDEGAMES_INCLUDE_DIR})

set(kdiamond_SRCS
    board.cpp
    container.cpp
    diamond.cpp
    game.cpp
    main.cpp
    mainwindow.cpp
    renderer.cpp
)

kde4_add_kcfg_files(kdiamond_SRCS settings.kcfgc)

kde4_add_executable(kdiamond ${kdiamond_SRCS})
target_link_libraries(kdiamond ${KDE4_KDEUI_LIBS} ${KDEGAMES_LIBRARY} ${KDE4_KNOTIFYCONFIG_LIBS})

install(TARGETS kdiamond  DESTINATION ${BIN_INSTALL_DIR})
install(FILES kdiamond.kcfg kdiamond.notifyrc kdiamondui.rc  DESTINATION ${DATA_INSTALL_DIR}/kdiamond)
install(FILES kdiamond.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
