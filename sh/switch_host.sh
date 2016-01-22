#!/bin/bash
select host in 'kaifa' 'xian_shang' 'sim' 'che_shi';
do
#case $host  in
#'kaifa') sudo  cat ~/Desktop/work/host/kaifa.txt > /etc/hosts;;
#'xian_shang') sudo  cat ~/Desktop/work/host/xian_shang.txt > /etc/hosts;;
#'sim') sudo  cat ~/Desktop/work/host/sim.txt > /etc/hosts;;
#'che_shi') sudo  cat ~/Desktop/work/host/che_shi.txt > /etc/hosts;;
#esac
#break
sudo  cat ~/Desktop/work/host/$host.txt  > /etc/hosts
break
done
