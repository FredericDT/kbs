<?php


if ( (!isset($_COOKIE['iscookies'])) || ($_COOKIE['iscookies']==''))
{
	setcookie('iscookies','0',time()+3650*24*3600);
	print '<META http-equiv=Content-Type content=text/html; charset=gb2312><meta HTTP-EQUIV=REFRESH CONTENT=3>���ڵ�½��̳����<br><br>��ϵͳҪ��ʹ��COOKIES�������������������COOKIES���������ܵ�¼��ϵͳ����';
	exit();
} 

require("inc/funcs.php");

preprocess();

show_nav();

?>
<TABLE cellSpacing=0 cellPadding=0 width=97% border=0 align=center>
<br>
<?php

showTitle();

?>
</TABLE>
<?php

if ($loginok!=1) {
	FastLogin();
}

showAllSecs();
if (isErrFounded()) {
	html_error_quit();
} else {
	showUserInfo();
	showOnlineUsers();
	showSample();
}
show_footer();

function preprocess(){
	GLOBAL $_GET;
	GLOBAL $_COOKIE;
	GLOBAL $sectionCount;

	$path='';
	if ($_GET['ShowBoards']=='Y') {
		$secNum=intval($_GET['sec']);
		if ( ($secNum>=0)  && ($secNum<$sectionCount)) {
			setcookie('ShowSecBoards'.$secNum, 'Y' ,time()+604800,''); 
			$_COOKIE['ShowSecBoards'.$secNum]='Y';
		}
	}
	if ($_GET['ShowBoards']=='N') {
		$secNum=intval($_GET['sec']);
		if ( ($secNum>=0)  && ($secNum<$sectionCount)) {
			setcookie('ShowSecBoards'.$secNum, '' ,time()+604800);
			$_COOKIE['ShowSecBoards'.$secNum]='';
		}
	}
}

function showAllSecs(){
	GLOBAL $sectionCount;
	GLOBAL $_COOKIE;
	GLOBAL $_GET;


	for ($i=0;$i<$sectionCount;$i++){
		if ($_COOKIE['ShowSecBoards'.$i]=='Y') {
			showSecs($i,true);
		} else {
			showSecs($i,false);
		}
	}
	return false;
}

function showSecs($secNum=0,$isFold) {
	extract($GLOBALS);
	if ( ($secNum<0)  || ($secNum>=$sectionCount)) {
		foundErr("�����������");
		return false;
	}
?>
<table cellspacing=1 cellpadding=0 align=center width="97%" class=tableBorder1>

<?php
	if ($isFold) {
?>
<TR><Th colSpan=2 height=25 align=left id=TableTitleLink>&nbsp;
<a href="<?php echo $_SERVER['PHP_SELF'] ; ?>?sec=<?php echo $secNum; ?>&ShowBoards=N" title="�ر���̳�б�"><img src="pic/nofollow.gif" border=0></a><a href="viewsec.php?sec=<?php echo $secNum ; ?>" title=���뱾������̳><?php echo $section_names[$secNum][0]; ?></a>
</th></tr>
<?php
		$boards = bbs_getboards($section_nums[$secNum], 0);
		if ($boards == FALSE) {
?>
		<TR><TD colspan="2" class=tablebody1>&nbsp;���������ް���</td></tr>
<?php
		} else {
		
			$brd_name = $boards["NAME"]; // Ӣ����
			$brd_desc = $boards["DESC"]; // ��������
			$brd_class = $boards["CLASS"]; // �������
			$brd_bm = $boards["BM"]; // ����
			$brd_artcnt = $boards["ARTCNT"]; // ������
			$brd_unread = $boards["UNREAD"]; // δ�����
			$brd_zapped = $boards["ZAPPED"]; // �Ƿ� z ��
			$brd_flag = $boards["FLAG"]; //flag
			$brd_bid = $boards["BID"]; //flag
			$rows = sizeof($brd_name);
			for ($i = 0; $i < $rows; $i++)	{
?>
		<TR><TD align=middle width="100%" class=tablebody1>
		<table width="100%" cellspacing=0 cellpadding=0><TR><TD align=middle width=46 class=tablebody1>
<?php	
				if ( $brd_unread[$i] == 1) {
					echo "<img src=pic/forum_isnews.gif alt=��������>";
				} else  {
					echo "<img src=pic/forum_nonews.gif alt=��������>";
				}
?>
		</TD>
		<TD width=1 bgcolor=#7a437a>
		<TD vAlign=top width=* class=tablebody1>
		
		<TABLE cellSpacing=0 cellPadding=2 width=100% border=0>
		<tr><td class=tablebody1 width=*><a href="list.php?boardid=4">
		<font color=#000066><?php echo $brd_name[$i] ?> </font></a>
				</td>
		<td width=40 rowspan=2 align=center class=tablebody1></td><td width=200 rowspan=2 class=tablebody1>���⣺<a href="dispbbs.php?boardid=4&id=31&replyID=67&skin=1">wawa..</a><BR>���ߣ�<a href="dispuser.php?id=1" target=_blank>Niflheim</a><BR>���ڣ�2003-06-24  19:51:48&nbsp;<a href="dispbbs.php?boardid=4&id=31&replyID=67&skin=1"><IMG border=0 src="pic/lastpost.gif" title="ת����wawa"></a></TD></TR><TR><TD width=*><FONT face=Arial><img src=pic/forum_readme.gif align=middle> <?php echo $brd_desc[$i] ?></FONT></TD></TR><TR><TD class=tablebody2 height=20 width=*>������<?php echo $brd_bm[$i]==''?'����':$brd_bm[$i] ; ?> </TD><td width=40 align=center class=tablebody2>&nbsp;</td><TD vAlign=middle class=tablebody2 width=200>
		<table width=100% border=0><tr><td width=25% vAlign=middle><img src=pic/forum_today.gif alt=������ align=absmiddle>&nbsp;<font color=#FF0000>N/A</font></td><td width=30% vAlign=middle><img src=pic/forum_topic.gif alt=���� border=0  align=absmiddle>&nbsp;N/A</td><td width=45% vAlign=middle><img src=pic/forum_post.gif alt=���� border=0 align=absmiddle>&nbsp;<?php echo $brd_artcnt[$i]; ?></td></tr>
		</table></TD></TR></TBODY></TABLE></td></tr></table></td></tr>
<?php
			}
		}
	} else {
?>
<TR><Th height=25 align=left id=TableTitleLink>&nbsp;
<a href="<?php echo $_SERVER['PHP_SELF'] ; ?>?sec=<?php echo $secNum; ?>&ShowBoards=Y" title="չ����̳�б�"><img src="pic/plus.gif" border=0></a><a href="viewsec.php?sec=<?php echo $secNum ; ?>" title=���뱾������̳><?php echo $section_names[$secNum][0]; ?></a>
</th></tr>
<?php

	}
?>
</table><br>
<?php
}

function showTitle() {
?>
<TR><TD style="line-height: 20px;">
��ӭ�¼����Ա <a href=dispuser.php?name=<?php echo $rs[4]; ?> target=_blank><b><?php echo $rs[4]; ?></b></a>&nbsp;[<a href="toplist.php?orders=2">�½�����</a>]<BR>��̳���� <B><?php echo $rs[3]; ?></B> λע���Ա , ����������<b><?php echo $rs[0]; ?></b> , ����������<b><?php echo $rs[1]; ?></b><BR>������̳��������<FONT COLOR="<?php echo $Forum_body[8]; ?>"><B><?php echo $rs[2]; ?></B></FONT> , ���շ�����<B><?php echo $rs[5]; ?></B> , ����շ�����<B><?php echo $rs[6]; ?></B></td><TD valign=bottom align=right style="line-height: 20px;"><a href=queryresult.php?stype=3>�鿴����</a> , <a href=hotlist.php?stype=1>���Ż���</a> , <a href=toplist.php?orders=1>��������</a> , <a href=toplist.php?orders=7>�û��б�</a><BR>�����һ�η������ڣ�<?php echo strftime("%Y-%m-%d %H:%M:%S"); ?>
</TD></TR>
<?php
}

function FastLogin()
{
extract($GLOBALS);
?>
<table cellspacing=1 cellpadding=3 align=center class=tableBorder1>
<form action="logon.php" method=post>
<input type="hidden" name="action" value="doLogon">
<tr>
<th align=left id=tabletitlelink height=25 style="font-weight:normal">
<b>-=> ���ٵ�¼���</b>
[<a href=reg.php>ע���û�</a>]��[<a href=lostpass.php style="CURSOR: help">��������</a>]
</th>
</tr>
<tr>
<td class=tablebody1 height=40 width="100%">
&nbsp;�û�����<input maxLength=16 name=id size=12>�������룺<input maxLength=20 name=password size=12 type=password>����<select name=CookieDate><option selected value=0>������</option><option value=1>����һ��</option><option value=2>����һ��</option><option value=3>����һ��</option></select><input type=hidden name=comeurl value="<?php echo $_SERVER['PHP_SELF']; ?>"><input type=submit name=submit value="�� ½">
</td>
</tr>
</form>
</table><br>
<?php 
} 

function usersysinfo($info){
   if (strpos($info,';')!==false)  {
	  $usersys=explode(';',$info);
	  if (count($usersys)>=2)  {
		  $usersys[1]=str_replace("MSIE","Internet Explorer",$usersys[1]);
		  $usersys[2]=str_replace(")","",$usersys[2]);
		  $usersys[2]=str_replace("NT 5.1","XP",$usersys[2]);
		  $usersys[2]=str_replace("NT 5.0","2000",$usersys[2]);
		  $usersys[2]=str_replace("9x","Me",$usersys[2]);
		  $usersys[1]="� �� ����".trim($usersys[1]);
		  $usersys[2]="����ϵͳ��".trim($usersys[2]);
		  $function_ret=$usersys[1].'��'.$usersys[2];
	  }  else  {
		  	  $function_ret='� �� ����δ֪������ϵͳ��δ֪';
	  }
  }  else {
    if ($getinfo==1)  {
      $function_ret="δ֪��δ֪";
    } 
  } 
  return $function_ret;
} 

function showUserInfo(){
?>
<table cellpadding=5 cellspacing=1 class=tableborder1 align=center style="word-break:break-all;">
<TR><Th align=left colSpan=2 height=25>-=> �û�������Ϣ</Th></TR>
<TR><TD vAlign=top class=tablebody1 height=25 width=100% >
<?php
$userip = $_SERVER["HTTP_X_FORWARDED_FOR"];
$userip2 = $_SERVER["REMOTE_ADDR"];
if  ($userip=='') {
	echo '������ʵ�ɣ� �ǣ�'. $userip2. '��';
} else {
	echo '������ʵ�ɣ� �ǣ�'. $userip .'��';
}
echo usersysinfo($_SERVER["HTTP_USER_AGENT"]);

?>
</TD></TR></table><br>
<?php

}

function showOnlineUsers(){
	$Forum_OnlineNum=bbs_getonlinenumber();
?>
<table cellpadding=5 cellspacing=1 class=tableborder1 align=center style="word-break:break-all;">
<TR><Th colSpan=2 align=left id=tabletitlelink height=25 style="font-weight:normal"><b>-=> ��̳����ͳ��</b>&nbsp;[<a href=showOnlineUsers.php?action=show>��ʾ��ϸ�б�</a>] [<a href=boardstat.php?reaction=online>�鿴�����û�λ��</a>]</Th></TR>
<TR><TD width=100% vAlign=top class=tablebody1>  Ŀǰ��̳���ܹ��� <b><?php echo $Forum_OnlineNum ; ?></b> �����ߣ�����ע���Ա <b><?php echo  $OnlineNum ?></b> �ˣ��ÿ� <b><?php echo  $GuestNum ?></b> �ˡ�<br>
��ʷ������߼�¼�� <b><?php echo  $Maxonline ?></b> ��ͬʱ����
</td></tr>
</table><br>
<?php
}

function showSample(){
?>
<table cellspacing=1 cellpadding=3 width="97%" border=0 align=center>
<tr><td align=center><img src="pic/forum_nonews.gif" align="absmiddle">&nbsp;û���µ�����&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="pic/forum_isnews.gif" align="absmiddle">&nbsp;���µ�����&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="pic/forum_lock.gif" align="absmiddle">&nbsp;����������̳</td></tr>
</table><br>
<?php
}

?>