#!/bin/bash
baseDir='/server/www/ganji/ganji_online/';
dirList=$(ls $baseDir);
for item in $dirList
do
	svn up  ${baseDir}${item} --username xiashiyu_58 --password 123456
done
