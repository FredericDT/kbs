<?php

$needlogin=0;
require("inc/funcs.php");
setStat("��������");

do_changeStyle();

if (isErrFounded()) {
		show_nav();
		head_var("ѡ�����");
		html_error_quit();
		show_footer();
} else {
  if (!isset($_SERVER["HTTP_REFERER"]) || ( $_SERVER["HTTP_REFERER"]=="") )
  {
    header("Location: ".$SiteURL);
  }   else  {
    header("Location: ".$_SERVER["HTTP_REFERER"]);
  } 
} 


function do_changeStyle()
{
  if (!isset($_GET["style"]))
  {
	foundErr("�Ƿ��Ĳ�����");
    return ;
  } 
  setcookie("style",$_GET["style"],time()+ 604800 ); //7*24*60*60
  return ;
} 
?>