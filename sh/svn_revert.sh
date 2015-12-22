#!/bin/bash
if [ "$1" != "true" ]; then
	ls /server/www/ganji/ganji_online |xargs -I {} svn st '/server/www/ganji/ganji_online/{}'|grep 'M\s'|grep -v 'code_base2/app/housing/conf/platform/global.ini'|grep -v 'msc/wap/app/misc/ganji/Christmasday/WapChristmasdayPage.class.php'|cut -c 5-
else
	ls /server/www/ganji/ganji_online |xargs -I {} svn st '/server/www/ganji/ganji_online/{}'|grep 'M\s'|grep -v 'code_base2/app/housing/conf/platform/global.ini'|grep -v 'msc/wap/app/misc/ganji/Christmasday/WapChristmasdayPage.class.php'|cut -c 5-|xargs svn revert
fi
exit 0
#ls /server/www/ganji/ganji_online |xargs -I {} svn st '/server/www/ganji/ganji_online/{}'|grep 'M\s'|grep -v 'code_base2/app/housing/conf/platform/global.ini'|grep -v 'msc/wap/app/misc/ganji/Christmasday/WapChristmasdayPage.class.php'|cut -c 5-|xargs svn revert
