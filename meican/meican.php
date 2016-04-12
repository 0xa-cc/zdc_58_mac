<?php
$loginUrl='https://meican.com/account/directlogin';
$userName='zhudongchang@58ganji.com';
$password='Zdc0428';

$data=array(
	'username'=>$userName,
	'loginType'=>'username',
	'password'=>$password,
	'remember'=>'true',
	'redirectUrl'=>'',
);


$cookiePath=tempnam(sys_get_temp_dir(), 'cookie');;

login_post($loginUrl,$cookiePath,$data);//模拟登陆，保存cookie
$getOrderUrl='https://meican.com/orders/list';
$getRes=get_content($getOrderUrl,$cookiePath);//获取订单
unlink($cookiePath);

echo $getRes;

//模拟登录 
function login_post($url, $cookie, $post) { 
    $curl = curl_init();//初始化curl模块 
    curl_setopt($curl, CURLOPT_URL, $url);//登录提交的地址 
    curl_setopt($curl, CURLOPT_RETURNTRANSFER,true);//是否自动显示返回的信息 
    curl_setopt($curl, CURLOPT_COOKIEJAR, $cookie); //设置Cookie信息保存在指定的文件中 
    curl_setopt($curl, CURLOPT_POST,true);//post方式提交 
    curl_setopt($curl, CURLOPT_POSTFIELDS, http_build_query($post));//要提交的信息 
    $res=curl_exec($curl);//执行cURL 
    curl_close($curl);//关闭cURL资源，并且释放系统资源 
    return $res;
} 


//登录成功后获取数据 
function get_content($url, $cookie) { 
    $ch = curl_init(); 
    curl_setopt($ch, CURLOPT_URL, $url); 
    curl_setopt($ch, CURLOPT_HEADER, 0); 
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); 
    curl_setopt($ch, CURLOPT_COOKIEFILE, $cookie); //读取cookie 
    $rs = curl_exec($ch); //执行cURL抓取页面内容 
    curl_close($ch); 
    return $rs; 
} 

function curl_request($url,$method='post',$data=array(),$header=array()){
    $ch = curl_init(); //初始化CURL句柄
    curl_setopt($ch, CURLOPT_URL, $url); //设置请求的URL
    curl_setopt($ch, CURLOPT_RETURNTRANSFER,true); //设为TRUE把curl_exec()结果转化为字串，而不是直接输出
    if('post'!=strtolower($method)){
        curl_setopt($ch, CURLOPT_CUSTOMREQUEST, $method); //设置请求方式
    }else{
        curl_setopt($ch,CURLOPT_POST,true);
    }
    empty($data) or curl_setopt($ch, CURLOPT_POSTFIELDS, http_build_query($data));//设置提交的字符串
    empty($header) or curl_setopt($ch, CURLOPT_HTTPHEADER, $header);//设置提交header
    0===strpos($url,'https:') and curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);//https请求加这个代码
    $content=curl_exec($ch);
    curl_close($ch);
    return $content;
}
