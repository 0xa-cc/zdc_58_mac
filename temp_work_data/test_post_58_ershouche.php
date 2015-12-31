<?php
//合作伙伴ID：36332768140033，密钥：smgQPQsePZldG8sPwzj6
/*
 *58 测试帐号：username: kuche08 pwd: 1q2w3e
1,浏览器访问 http://openapi.58.com/oauth2/authorize?client_id=36332768140033&redirect_uri=http://bj.ganji.com/ershouche/&response_type=code ,输入58 账号
2,http://openapi.58.com/oauth2/authorize?client_id=36332768140033&redirect_uri=http://bj.ganji.com/ershouche/&response_type=code
3,发布测试工具 http://post.58.com/service/fabu/openapi/?source=car
*/


class  OpenApi58 {
    private  $code='451e13d6eba7213ef3bed7a50aaacb9';
    private  $redirect_uri='http://bj.ganji.com/ershouche/';
    private  $client_id='36332768140033';
    private  $client_secret='smgQPQsePZldG8sPwzj6';//=md5('smgQPQsePZldG8sPwzj6'.'openapi.58.com'.$time_sign);

    public function postSend(){
        $accessToken=$this->getAccessToken();
        $time_sign=time()*1000;
        $paras=<<<EOF
<para name="zimu" value="408788"/>
		<para name="brand" value="412596"/>
		<para name="chexi" value="412660"/>
		<para name="ObjectType" value="1"/>
		<para name="carchexing" value="408947"/>
		<para name="madein" value="408852"/>
		<para name="chexibieming" value="583091"/>
		<para name="displacement" value="409110"/>
		<para name="gearbox" value="408822"/>
		<para name="kucheid" value=""/>
		<para name="shangshishijian" value="515772"/>
		<para name="cheshenyanse" value="3"/>
		<para name="erscpinpai" value="2008款 2.5V 豪华版"/>
		<para name="rundistance" value="15"/>
		<para name="rundistanceqj" value="8_12"/>
		<para name="chelingqj" value="2008_2010"/>
		<para name="MinPrice" value="13.98"/>
		<para name="MinPriceqj" value="40_999999"/>
		<para name="buytime" value="2008"/>
		<para name="shangpaiyuefen" value="515686"/>
		<para name="baoyang" value="515673"/>
		<para name="shiguqk" value="515713"/>
		<para name="shigumiaoshu" value=""/>
		<para name="syshijian" value=""/>
		<para name="yczbpic" value=""/>
		<para name="yczhibao" value="525379"/>
		<para name="qtkt" value="0"/>
		<para name="xbsx" value="0"/>
		<para name="shangpainianyue" value="200806"/>
		<para name="shifoufufeifabu" value="0"/>
		<para name="shifouyishou" value="0"/>
		<para name="paifangbiaozhun" value=""/>
		<para name="guohufeiyong" value="553663"/>
		<para name="shifougaodang" value=""/>
		<para name="cylb" value=""/>
		<para name="xinxibianhao" value="O3yxwt"/>
		<para name="escwltv2" value="0"/>
		<para name="qitadianhua" value=""/>
		<para name="goblianxiren" value="刘先生"/>
		<para name="wanglintongbieming" value="0"/>
		<para name="apixinxiid" value=""/>
		<para name="apilaiyuan" value="1"/>
		<para name="apiedit" value="1"/>
		<para name="type" value="0"/>
		<para name="gobquzhi" value="zimu=F&amp;brand=%E4%B8%B0%E7%94%B0&amp;chexi=%E9%94%90%E5%BF%97&amp;objecttype=%E8%BD%BF%E8%BD%A6&amp;displacement=2.5&amp;gearbox=%E8%87%AA%E5%8A%A8&amp;shangshishijian=2008&amp;tingshoushijian=2009&amp;zbjcfanwei=1&amp;yczb_cheling=2&amp;yczb_licheng=5&amp;madein=%E5%90%88%E8%B5%84&amp;chexibieming=%E4%B8%B0%E7%94%B0%E9%94%90%E5%BF%97&amp;rundistanceqj=8-12%E4%B8%87%E5%85%AC%E9%87%8C&amp;chelingqj=3-5%E5%B9%B4&amp;buytime=2008%E5%B9%B4&amp;shangpaiyuefen=6%E6%9C%88&amp;baoyang=%E6%98%AF&amp;shiguqk=%E6%97%A0&amp;cheshenyanse=%E9%93%B6%E8%89%B2&amp;minpriceqj=40%E4%B8%87%E4%BB%A5%E4%B8%8A&amp;cateapplyed=29&amp;localapplyed=4"/>
EOF;
        $content=<<<EOF
1、车辆介绍：2008年6月份上牌、一手车、保险和年审都到2014年6月份、车身内外都有8.5成新、车况超靓和后驱车动力十足、空间超级大和宽敞。2.5自动波、豪华版带天窗、DVD导航、胎压监测装置、车内中控锁、遥控钥匙、ABS防抱死、真皮方向盘、真皮座椅、氙气大灯、恒温空调、定速巡航等等最先进配置。原车主爱车如爱子、每走5000公里就会定时保养和检测车子、平常只用于上下班代步用途、杜绝一切不惜车的行为确保车辆：新+靓。&lt;br /&gt;&lt;br /&gt;2、检测标准：场内所有车辆都严格遵循8大类10大系统360项检测认证体系标务！确保车辆的合法性、质量的可靠性、车况透明度和安全性，所有车辆均可接受第三方检测机构检测，让您购车无忧！&lt;br /&gt;&lt;br /&gt;3、郑重承诺：本公司杜绝一切泡水车、事故车，大碰撞车和非法来源车， 均为精品二手车源！所有车源在出售前都统一保养和检查安全性能，购买的车辆可直驾驶您的爱车直接回家。&lt;br /&gt;&lt;br /&gt;4、过户手续：证件齐全+{包过户}+全国各地提档，2个小时快速办理过户手续，一站式的售后服务让您放心把车开回家，购车过程完全可以无后顾之忧！&lt;br /&gt;&lt;br /&gt;5、付款方式：支持现金支付，储蓄卡支付，信用卡支付多种付款方式供选择！&lt;br /&gt;&lt;br /&gt;6、看车地址：深圳市龙岗区！{欢迎到店试乘试驾、惊喜不断、好礼赠送}&lt;br /&gt;&lt;br /&gt;领域二手车真诚邀您来店品鉴！&lt;br /&gt;微信号：13418487913&lt;br /&gt;加盟热线：0755-28611999&lt;br /&gt;公司网址：http://shop.58.com/17665145670401/&lt;br /&gt;&lt;br /&gt;{备注：以下所有车源的图片均为实拍图片，绝对无经过电脑修饰和偷龙转凤，假一赔十}
EOF;

        $data=array(
            'cate_id'=>'29',
            'local_id'=>'158',
            'title'=>'丰田锐志 2008款 2.5V 豪华版 2008年上牌-全网最实惠价+物超所值+清仓价',
            'content'=>$content,//'58技术openapi 测试   2014年6月份上牌、一手车、保险和年审都到2018年6月份',
            'phone'=>'13021939679',
            'email'=>'xxx@58.com',
            'im'=>'xxxxxxx',
            'pic'=>'Byte[]',
            'paras'=>$paras,
            'time_sign'=>$time_sign,
            'client_id'=>$this->client_id,
            'client_secret'=>$this->getClientSecret($time_sign),
            '58user_id'=>$accessToken['uid'],
            'access_token'=>$accessToken['access_token'],
        );
        $postSendUrl='http://openapi.58.com/oauth2/gateway/postservice/send';
        $res=$this->curlRequest($postSendUrl,'post',$data);

//        print_r($data);
        var_dump($res);
    }

    public function getAccessToken(){
        return json_decode('{"uid":"25941852915718","expires_in":"365","refresh_token":"9ab947e2bbec88878c2242db9e899a1_v1","access_token":"439eef837aaf478b4f4161fff9910_v1"}',true);
        $access_token_url='http://openapi.58.com/oauth2/access_token';

        $time_sign=time()*1000;
        $data=array(
            'code'=>$this->code,
            'grant_type'=>'authorization_code',
            'redirect_uri'=>$this->redirect_uri,
            'time_sign'=>$time_sign,
            'client_id'=>$this->client_id,
            'client_secret'=>$this->getClientSecret($time_sign),
        );

        $getRes=self::curlRequest($access_token_url,'post',$data);
        return json_decode($getRes,true);
    }

    private function getClientSecret($timeSign){
        return md5($this->client_secret.'openapi.58.com'.$timeSign);
    }

    public function getRefreshToken($access_token,$refresh_token){
        $refresh_token_url='http://openapi.58.com/oauth2/refresh_token';

        $time_sign=time()*1000;
        $data=array(
            'access_token'=>$access_token,
            'refresh_token'=>$refresh_token,
            'grant_type'=>'refresh_token',
            'redirect_uri'=>$this->redirect_uri,
            'time_sign'=>$time_sign,
            'client_id'=>$this->client_id,
            'client_secret'=>$this->getClientSecret($time_sign),
            '58user_id'=>'35473652971031',
        );
        $getRes=self::curlRequest($refresh_token_url,'post',$data);
        return json_decode($getRes,true);
    }

    public function getCode($mobile,$password){
        $login_url='https://passport.58.com/douimobilelogin';
        $data=array(
            'domain'=>'58.com',
            'callback'=>'handleMobileLoginResult',
            'sysIndex'=>'1',
            'pptmobile'=>$mobile,
            'pptmobilepassword'=>$password,
        );
        $getRes=self::curlRequest($login_url,'post',$data);
        preg_match('/location\.replace\(\'http\:\/\/\/(.*?)\'\);/U',$getRes,$matches);
        $get_code_url=isset($matches[1])?'http://openapi.58.com/'.$matches[1]:'';
        $get_code_url=str_replace('domain=','',$get_code_url).'&domain=58.com';

        $data=array(
            'domain'=>'58.com',
            'type'=>'mobilepassworderror',
            'callback'=>'handleMobileLoginResult',
            'isweak'=>'0',
            'uid'=>'${uid}',
        );
        return file_get_contents($get_code_url);
    }

    public static function curlRequest($url,$method='post',$data=array(),$header=array()){
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
}

$openApi58=new OpenApi58();
//$accessToken=json_decode($openApi58->getAccessToken(),true);
//if(!isset($accessToken['error'])){
//    $refreshToken=$openApi58->getRefreshToken($accessToken['access_token'],$accessToken['refresh_token']);
//    var_dump($refreshToken);
//}
$openApi58->postSend();