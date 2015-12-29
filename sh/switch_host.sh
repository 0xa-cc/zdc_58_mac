#!/bin/bash
select host in 'kaifa' 'xian_shang' 'yu_shang_xian' 'che_shi';
do
case $host  in
'kaifa') sudo  cat ~/Desktop/work/host/kaifa.txt > /etc/hosts;;
'xian_shang') sudo  cat ~/Desktop/work/host/xian_shang.txt > /etc/hosts;;
'yu_shang_xian') sudo  cat ~/Desktop/work/host/yu_shang_xian.txt > /etc/hosts;;
'che_shi') sudo  cat ~/Desktop/work/host/che_shi.txt > /etc/hosts;;
esac
break
done
