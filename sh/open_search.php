<?php
$url='http://www.itechzero.com/google-mirror-sites-collect.html';
$path=dirname(__FILE__).'/temp_data';
$content='';
if(file_exists($path)){
	$content=file_get_contents($path);
}
if(empty($content)){
	$content=file_get_contents($url);
	file_put_contents($path,$content);
}
$matchUrl=preg_match('/\<h3\>.*?原版网页.*?\<\/h3\>.*?\<a.*?href="(.*?)".*?\>.*?\<\/a\>/s',$content,$matches);
if(isset($matches[1])){
	$shell='open '.$matches[1];
	shell_exec($shell);
}
