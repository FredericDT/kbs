<?php
require("inc/funcs.php");

require("inc/usermanage.inc.php");

setStat("�û��ʼ�����");

show_nav();

head_var($userid."�Ŀ������","usermanagemenu.php",0);

if ($loginok==1) {
	main();
}else {
	foundErr("��ҳ��Ҫ������ʽ�û���ݵ�½֮����ܷ��ʣ�");
}


if (isErrFounded()) {
		html_error_quit();
} 
show_footer();

function main() {
	global $_GET;

	$boxName=$_GET['boxname'];
	if (!isset($_GET['start'])) {
		$startNum=99999;
	} else {
		$startNum=intval($_GET['start']);
	}

	if ($boxName=='') {
		$boxName='inbox';
	}
	if ($boxName=='inbox') {
		showUserManageMenu();
		showmailBoxes();
		showmailBox('.DIR','�ռ���', $startNum);
		return true;
	}
	if ($boxName=='sendbox') {
		showUserManageMenu();
		showmailBoxes();
		showmailBox('.SENT','������',$startNum );
		return true;
	}
	if ($boxName=='deleted') {
		showUserManageMenu();
		showmailBoxes();
		showmailBox('.DELETED','������',$startNum);
		return true;
	}
	foundErr("��ָ���˴�����������ƣ�");
	return false;
}

function showmailBoxes() {
?>
<TABLE cellpadding=6 cellspacing=1 align=center class=tableborder1><TBODY><TR>
<TD align=center class=tablebody1><a href="usermailbox.php?boxname=inbox"><img src=pic/m_inbox.gif border=0 alt=�ռ���></a> &nbsp; <a href="usermailbox.php?boxname=sendbox"><img src=pic/m_outbox.gif border=0 alt=������></a> &nbsp; <a href="usermailbox.php?boxname=deleted"><img src=pic/m_recycle.gif border=0 alt=�ϼ���></a>&nbsp; <a href="friendlist.php"><img src=pic/m_address.gif border=0 alt=��ַ��></a>&nbsp;<a href=JavaScript:openScript('messanger.php?action=new',500,400)><img src=pic/m_write.gif border=0 alt=������Ϣ></a></td></tr></TBODY></TABLE>
<?php
}

function showmailBox($path, $desc, $startNum){
	global $currentuser;
?>
<br>
<table cellpadding=3 cellspacing=1 align=center class=tableborder1>
<tr>
<th valign=middle width=30 height=25>�Ѷ�</th>
<th valign=middle width=100>
<?php   if ($Desc=="������")
    echo "������";  
  else
    echo "�ռ���";?>
</th>
<th valign=middle width=300>����</th>
<th valign=middle width=150>����</th>
<th valign=middle width=50>��С</th>
<th valign=middle width=30>����</th>
</tr>
<?php
	$mail_fullpath = bbs_setmailfile($currentuser["userid"],$path);
	$mail_num = bbs_getmailnum2($mail_fullpath);
	if($mail_num < 0 || $mail_num > 30000) {
		foundErr('����'.$desc.'���ż�̫�࣡');
		return false;
	}
	if($mail_num == 0) {
?>
<tr>
<td class=tablebody1 align=center valign=middle colspan=6>����<?php echo $desc; ?>��û���ż���</td>
</tr>
</table>
<?php
	return false;
	}
	$num=ARTICLESPERPAGE;
	if ($startNum > $mail_num - $num + 1) $startNum = $mail_num - $num + 1;
	if ($startNum < 0)
	{
		$startNum = 0;
		$num = $mail_num;
	}
	$maildata = bbs_getmails($mail_fullpath,$startNum,$num);
	if ($maildata == FALSE) {
			foundErr("��ȡ�ʼ�����ʧ��!");
			return false;
	}
	for ($i = 0; $i < $num; $i++){
?>
<tr>
<td class=tablebody1 align=center valign=middle>
<?php 
         if ($$maildata[$i]["FLAGS"]=='Y')
		print "<img src=\"pic/m_news.gif\">";
          else
         	 print "<img src=\"".$Forum_info[7]."m_olds.gif\">";
          break;
        case "outbox":
          print "<img src=\"".$Forum_info[7]."m_issend_2.gif\">";
          break;
        case "issend":
          print "<img src=\"".$Forum_info[7]."m_issend_1.gif\">";
          break;
        case "recycle":
          if ($rs["flag"]==0)
		print "<img src=\"".$Forum_info[7]."m_news.gif\">";
            else
          	print "<img src=\"".$Forum_info[7]."m_olds.gif\">";
          break;
      } 
?>
</td>
<td class=<?php       echo $tablebody; ?> align=center valign=middle style="<?php       echo $newstyle; ?>">
<?php       if ($smstype=="inbox" || $smstype=="recycle")
      {
?>
<a href="dispuser.php?name=<?php         echo HTMLEncode($rs["sender"]); ?>" target=_blank><?php         echo HTMLEncode($rs["sender"]); ?></a>
<?php       }
        else
      {
?>
<a href="dispuser.php?name=<?php         echo HTMLEncode($rs["incept"]); ?>" target=_blank><?php         echo HTMLEncode($rs["incept"]); ?></a>
<?php       } ?>
</td>
<td class=<?php       echo $tablebody; ?> align=left style="<?php       echo $newstyle; ?>"><a href="JavaScript:openScript('messanger.php?action=<?php       echo $readaction; ?>&id=<?php       echo $rs["id"]; ?>&sender=<?php       echo $rs["sender"]; ?>',500,400)"><?php       echo HTMLEncode($rs["title"]); ?></a>	</td>
<td class=<?php       echo $tablebody; ?> style="<?php       echo $newstyle; ?>"><?php       echo $rs["sendtime"]; ?></td>
<td class=<?php       echo $tablebody; ?> style="<?php       echo $newstyle; ?>"><?php       echo mb_strlen($rs["content"],"EUC-JP"); ?>Byte</td>
<td align=center valign=middle width=30 class=<?php       echo $tablebody; ?>><input type=checkbox name=id value=<?php       echo $rs["id"]; ?>></td>
</tr>
<?php
	}
?>
</table>
<?php
}


?>
