<?php
$domainUrl='http://www.laishushu.com';
$getRes=file_get_contents($domainUrl.'/v/455/');
preg_match('/\<div class="dir_main_section"\>.*?\<\/div\>/s',$getRes,$matches);
$getRes=$matches[0];
preg_match_all('/\<a href="(.*?)"\>.*?\<\/a\>/s',$getRes,$matches);

//echo count($matches[1]);die();
foreach($matches[1] as $val){
	$articleHtml=file_get_contents($domainUrl.$val);
	$articleHtml=mb_convert_encoding($articleHtml,'utf8','gbk');
	preg_match('/\<div class="inner mt" id="inner"\>(.*?)\<\/div\>/s',$articleHtml,$articleMatches);
	$content=$articleMatches[1];
	$content=str_replace(array('<br />','&nbsp;',"\r"),array("\n",' ',''),$content);
	$content=str_replace('(来书书网 <a href="http://www.laishushu.com" target="_blank">www.laishushu.com</a>)','',$content);
	$content=str_replace('来/书/书/网 www.laīshushu.cōm来/书/书/网 www.laīshushu.cōm','',$content);
	$content=preg_replace('/\[第\d+章.*?\]/','',$content);
	file_put_contents('小月阿姨.txt',$content,FILE_APPEND);
//	echo $content;
//	die();
	echo "$val\n";
}
