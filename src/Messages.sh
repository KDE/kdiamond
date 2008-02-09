#!bin/sh
#$EXTRACTRC *.ui >> rc.cpp # not needed by now, will become relevant once I add a config dialog
$XGETTEXT *.cpp -o $podir/kdiamond.pot
