#!/bin/sh
basepath=$(cd `dirname $0`; pwd;);
basepath=$(dirname $basepath);
files=`cat $basepath/svn.log` 
for file in $files
do
        echo "check file=$file";

        svn co $file --username xiashiyu_58 --password 123456;

done
