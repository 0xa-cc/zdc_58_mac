<?php
SetServer('127.0.0.1', 9312); //注意这里的主机
#$cl->SetMatchMode(SPH_MATCH_EXTENDED); //使用多字段模式
//dump($cl);
$index=”article”;
$res = $cl->Query($keyword, $index);
$err = $cl->GetLastError();
dump($res);
