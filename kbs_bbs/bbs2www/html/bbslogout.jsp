<?
  require("funcs.jsp");
  if ($loginok!=1) {
?>
<html>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" type="text/css" href="/bbs.css">
û�е�½<br><br>
<a href="/index.html">���ٷ���</a>
</html>
<? } else {
	bbs_wwwlogoff();
    setcookie("UTMPKEY","");
    setcookie("UTMPNUM","");
    setcookie("UTMPUSERID","");
    setcookie("LOGINTIME","");

	header("Location: /index.html");
   }
?>
