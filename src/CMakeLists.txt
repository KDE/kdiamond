add_subdirectory(pics)
include_directories(${KDEGAMES_INCLUDE_DIR})

set(kdiamond_SRCS
    board.cpp
    container.cpp
    diamond.cpp
    game.cpp
    greeter.cpp
    main.cpp
    mainwindow.cpp
    renderer.cpp
)

kde4_add_kcfg_files(kdiamond_SRCS settings.kcfgc)

kde4_add_executable(kdiamond ${kdiamond_SRCS})
target_link_libraries(kdiamond  ${KDE4_KDEUI_LIBS} kdegames)

install(TARGETS kdiamond  DESTINATION ${BIN_INSTALL_DIR})
install(FILES kdiamond.kcfg  DESTINATION  ${DATA_INSTALL_DIR}/kdiamond)
install(FILES kdiamondui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kdiamond)
install(FILES kdiamond.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR})
