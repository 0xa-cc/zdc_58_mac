#!/bin/bash
ps -ef|grep phpstorm|cut -c 8-12|head -n 1|xargs kill
