<?php

//http://www.bthand.com/search/%E8%8B%8D%E4%BA%95%E7%A9%BA.html
//http://www.bthand.com/search/%E8%8B%8D%E4%BA%95%E7%A9%BA.html
$k='苍井空';
$searchHost='http://www.bthand.com';
$searchPath='/search/%s/1-1.html';
$searchUrl=sprintf($searchHost.$searchPath,urlencode($k));


$searchHtml=file_get_contents($searchUrl);
preg_match('/\<div class="bottom-pager"\>.*?\<\/div\>/s',$searchHtml,$matches);
preg_match_all('/href="(.*?)"/s',$matches[0],$matchNew);
$matchNew[1]=array_merge(array(sprintf($searchPath,urlencode($k))),$matchNew[1]);

foreach($matchNew[1] as $val){
    $url=$searchHost.$val;
    down_load_baidu_yun($url);
//    die();
}


function down_load_baidu_yun($searchUrl){
    global $searchHost;
    $patten='\<div class="item-title"\>.*?href="(.*?)".*?\<\/div\>';
    $searchHtml=file_get_contents($searchUrl);
    preg_match_all(sprintf('/%s/s',$patten),$searchHtml,$matches);

    $curl="curl 'http://pan.baidu.com/rest/2.0/services/cloud_dl?bdstoken=3c21ca62f44df7e4fe806e9e6e07fb8f&channel=chunlei&clienttype=0&web=1&app_id=250528' -H 'Cookie: BDUSS=lVUC1uNmRMZW5nQm9VQWk4OTd4NUlkMFVqM28zY1h2Vkpaa3FWSEczcExSb2RXQVFBQUFBJCQAAAAAAAAAAAEAAABq0gQJwMvA78zUybMxOTg5AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEu5X1ZLuV9WMV; BIDUPSID=9CCEADF27025CCF25A60501C056ED243; PSTM=1449130969; PANWEB=1; bdshare_firstime=1449231684309; Hm_lvt_773fea2ac036979ebb5fcc768d8beb67=1449231673; Hm_lvt_adf736c22cd6bcc36a1d27e5af30949e=1449231673; MCITY=-210%3A; BAIDUID=9CCEADF27025CCF25A60501C056ED243:SL=0:NR=10:FG=1; BDRCVFR[feWj1Vr5u3D]=I67x6TjHwwYf0; Hm_lvt_7a3960b6f067eb0085b7f96ff5e660b0=1452597174,1453519061,1453544348,1453794085; Hm_lpvt_7a3960b6f067eb0085b7f96ff5e660b0=1453794085; cflag=65151%3A3; H_PS_PSSID=18881_1421_18240_18879_17946_17565_18964_18778_17001_17072_15896_12319_18980_19000; PANPSC=15751285120778212858%3aV4xYP%2bguMp4FXw7LJwitWH%2bDBiInasN4cN9WOUyshdd9c6xaO4oLgTDCjjLS5Fu9td6cTYiXvmSPazt5F8xENn22f22FudOmK4e5sEWLgBa8hwwJ4T4TM11t%2b2uE4py1waIsCoRbv1cwA1Wz9pryQqIPtONpP5mFUbdDfbz2uykEt8j%2fGEmnepUCaRAzLlKqA8yxsQhfco2wcc33C7HckA%3d%3d' -H 'Origin: http://pan.baidu.com' -H 'Accept-Encoding: gzip, deflate' -H 'Accept-Language: zh-CN,zh;q=0.8' -H 'User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.152 Safari/537.36' -H 'Content-Type: application/x-www-form-urlencoded; charset=UTF-8' -H 'Accept: application/json, text/javascript, */*; q=0.01' -H 'Referer: http://pan.baidu.com/disk/home' -H 'X-Requested-With: XMLHttpRequest' -H 'Connection: keep-alive' --data '#data#'  --compressed";

    $para=array(
        'method'=>'query_magnetinfo',
        'app_id'=>'250528',
//    'source_url'=>'magnet:?xt=urn:btih:0A88F4357D4BA6B56C369A36E7595A540E93AA16&dn=大屏幕午夜剧场流川枫与苍井空[大屏幕电影网].rmvb',
        'save_path'=>'/我的视频/苍井空/',
        'type'=>'4',
    );

    foreach($matches[1] as $val){
        $itemHtml=file_get_contents($searchHost.$val);
        preg_match('/href="(magnet\:.*?)"/s',$itemHtml,$matchItem);

        $para['source_url']=$matchItem[1];
        $curl=str_replace('#data#',http_build_query($para),$curl);
        $shRes=shell_exec($curl);
        echo $para['source_url']."\n";
        echo $shRes."\n";
    }
}