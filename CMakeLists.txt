project(kdiamond)

#Temporary CMake code for stand-alone compilation. Remove when merging back to trunk.
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)
find_package(LibKDEGames REQUIRED)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES}
${KDEGAMES_INCLUDE_DIR}/highscore/ ${KDEGAMES_INCLUDE_DIR}/kgame/ )
#End of temporary CMake code.

add_subdirectory(src)
add_subdirectory(themes)
