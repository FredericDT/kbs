<?php

function showBoardStaticsTop($boardArr){
?>
<TABLE cellpadding=3 cellspacing=1 class=tableborder1 align=center><TR><Th height=25 width=100% align=left id=tabletitlelink style="font-weight:normal">���浱ǰ����<b><?php echo $boardArr['CURRENTUSERS'];?></b>������ </Th></TR></td></tr></TABLE>
<BR>
<table cellpadding=0 cellspacing=0 border=0 width=97% align=center valign=middle><tr><td align=center width=2> </td><td align=left><a href=announce.php?boardid=2><span class="buttonclass1" border=0 alt=������></span></a>&nbsp;&nbsp;<a href=vote.php?boardid=2><span class="buttonclass2" border=0 alt=������ͶƱ></span>&nbsp;&nbsp;<a href=smallpaper.php?boardid=2><span class="buttonclass3" border=0 alt=����С�ֱ�></span></a></td><td align=right><img src=pic/team2.gif align=absmiddle>
<?php 
	$bms=split(' ',$boardArr['BM']);
	foreach($bms as $bm) {
?>
<a href="dispuser.php?name=<?php echo $bm; ?>" target=_blank title=����鿴�ð�������><?php echo $bm; ?></a>
<?php
	}
?>
</td></tr></table>
<?php
}

function showBoardContents($boardID,$boardName){
	global $dir_modes;
	$dir_mode = $dir_modes["ORIGIN"];
?>
<form action=admin_batch.asp method=post name=batch><TR align=middle><Th height=25 width=32 id=tabletitlelink><a href=list.asp?name=<?php echo $boardName; ?>&page=&action=batch>״̬</a></th><Th width=* id=tabletitlelink>�� ��  (��<img src=pic/plus.gif align=absmiddle>����չ�������б�)</Th><Th width=80 id=tabletitlelink>�� ��</Th><Th width=64 id=tabletitlelink>�ظ�</Th><Th width=195 id=tabletitlelink>������ | �ظ���</Th></TR>
<?php
	$total = bbs_countarticles($boardID, $dir_mode);
	if ($total<=0)
	$totalPages=$total;
	$articles = bbs_getarticles($boardName, 0, 100, $dir_modes["ORIGIN"]);
	$articleNum=count($articles);
	for($i=0;$i<$articleNum;$i++){
?>
<TR align=middle><TD class=tablebody2 width=32 height=27><img src="pic/blue/folder.gif" alt=��������></TD><TD align=left class=tablebody1 width=* ><img src="pic/nofollow.gif" id="followImg1"><a href="dispbbs.asp?boardName=<?php echo $boardName ;?>&ID=<?php echo $articles[$i]['ID'] ;?>" title="<?php echo $articles[$i]['TITLE'] ;?><br>���ߣ�<?php echo $articles[$i]['OWNER'] ;?><br>������2003-6-2 14:04:17<br>��������asdfsadf..."><?php echo $articles[$i]['TITLE'] ;?></a></TD><TD class=tablebody2 width=80><a href="dispuser.asp?id=<?php echo $articles[$i]['OWNER'] ;?>" target=_blank><?php echo $articles[$i]['OWNER'] ;?></a></TD><TD class=tablebody1 width=64>0/2</TD><TD align=left class=tablebody2 width=195>&nbsp;&nbsp;<a href="dispbbs.asp?boardid=1&id=1&star=1#1">2003-6-2&nbsp;14:04</a>&nbsp;<font color=#FF0000>|</font>&nbsp;<a href=dispuser.asp?id=4 target=_blank>Roy</a></TD></TR>
<tr style="display:none" id="follow<?php echo $i;?>"><td colspan=5 id="<?php echo $i;?>" style="padding:0px"><div style="width:240px;margin-left:18px;border:1px solid black;background-color:lightyellow;color:black;padding:2px" onclick="loadThreadFollow(<?php echo $i;?>,1)">���ڶ�ȡ���ڱ�����ĸ��������Ժ��</div></td></tr>
<?php
	}

}

function showBroadcast($boardID,$boardName){
?>
<tr><td class=tablebody1 colspan=5 height=20>
	<table width=100% ><tr><td valign=middle height=20 width=50> <a href=AllPaper.asp?boardid=1 title=����鿴����̳����С�ֱ�><b>�㲥��</b></a> </td><td width=*> <marquee scrolldelay=150 scrollamount=4 onmouseout="if (document.all!=null){this.start()}" onmouseover="if (document.all!=null){this.stop()}"></marquee><td align=right width=240><a href=elist.asp?boardid=<?php echo boardID; ?> title=�鿴���澫��><font color=#FF0000><B>����</B></font></a> | <a href=boardstat.asp?reaction=online&boardid=<?php echo boardID; ?> title=�鿴����������ϸ���>����</a> | <a href=bbseven.asp?boardid=1 title=�鿴�����¼�>�¼�</a> | <a href=BoardPermission.asp?boardid=<?php echo boardID; ?> title=�鿴�����û���Ȩ��>Ȩ��</a> | <a href=admin_boardset.asp?boardid=<?php echo boardID; ?>>����</a></td></tr></table>
</td></tr>
<?php
}

?>