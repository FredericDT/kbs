<?php
require "default.php";
//define("ATTACHTMPPATH","boards/_attach");
function getattachtmppath($userid,$utmpnum)
{
  $attachdir="cache/home/" . strtoupper(substr($userid,0,1)) . "/" . $userid . "/" . $utmpnum . "/upload";
  return $attachdir;
//  return ATTACHTMPPATH;
}

$SiteName="����BBS";

$HTMLTitle="����BBS";

$HTMLCharset="GB2312";

$DEFAULTStyle="style1.css";

$Banner="pic/ws.jpg";

$BannerURL="http://aka.com.cn";

$SiteURL=$_SERVER['SERVER_NAME'];
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
?>
