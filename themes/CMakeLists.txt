set(kdiamond_THEME_SRCS
	diamonds.desktop
	diamonds.svgz
	diamonds.png
	funny_zoo.desktop
	funny_zoo.svgz
	funny_zoo.png
        default.desktop
        egyptian.svgz
        egyptian_preview.png
)
install(FILES ${kdiamond_THEME_SRCS} DESTINATION ${KDE_INSTALL_DATADIR}/kdiamond/themes)
