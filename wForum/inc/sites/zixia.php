<?php
require "default.php";
//define("ATTACHTMPPATH","boards/_attach");
function getattachtmppath($userid,$utmpnum)
{
  $attachdir="cache/home/" . strtoupper(substr($userid,0,1)) . "/" . $userid . "/" . $utmpnum . "/upload";
  return $attachdir;
//  return ATTACHTMPPATH;
}

define("ANNOUNCENUMBER",5);

define("ARTICLESPERPAGE",20);

$SiteName="����BBS";

$SiteURL="http://news.zixia.net:8080";

$HTMLTitle="����BBS";

$HTMLCharset="GB2312";

$DEFAULTStyle="defaultstyle";

$Banner="pic/ws.jpg";

$BannerURL="http://aka.com.cn";

define("ATTACHMAXSIZE","1048576");
define("ATTACHMAXCOUNT","20");

$section_nums = array("0", "1", "2", "3", "4", "5", "6", "7", "8", "9");
$section_names = array(
    array("�� ͷ ��", "[�ڰ�/ϵͳ]"),
    array("������", "[��/����]"),
    array("��С�ֶ�", "[����/У��]"),
    array("�Ժ�����", "[��ʳ/����]"),
    array("��Ϸ����", "[��Ϸ/����]"),
    array("����Ū��", "[����/�Ļ�]"),
    array("����֮·", "[����/�ۻ�]"),
    array("������ʩ", "[����/��ѵ]"),
    array("�¹ⱦ��", "[ת��/�б�]"),
    array("��ʥȡ��", "[רҵ/����]") 
);
$sectionCount=count($section_names);
?>
