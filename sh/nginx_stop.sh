#!/bin/bash
sudo kill $(ps -ef|grep nginx|grep master|cut -c 7-11);
