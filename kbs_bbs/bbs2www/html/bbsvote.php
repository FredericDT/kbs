<?php

	require("funcs.php");
	if ($loginok != 1 || ($currentuser["userid"] == "guest") )
		html_nologin();
	else
	{
		html_init("gb2312");

		if(isset($_GET["board"]))
			$board = $_GET["board"];
		else
			html_error_quit("����������");

		if(isset($_GET["num"]))
			$num = $_GET["num"];
		else
			html_error_quit("��������1");

		$brdarr = array();
		$brdnum = bbs_getboard($board,$brdarr);
		if($brdnum == 0)
			html_error_quit("�����������");

		if(bbs_checkreadperm($currentuser["index"],$brdnum)==0)
			html_error_quit("��û��Ȩ��");

		$votearr = array();
		$retnum = bbs_get_vote_from_num($board,$votearr,$num);

		if($retnum <= 0)
			html_error_quit("��ͶƱ������");
?>
<body>
<center><p><?php echo BBS_FULL_NAME; ?> -- [ͶƱ�б�] [�û�:<?php echo $currentuser["userid"];?>] 
<?php echo $board; ?>��ͶƱ<br></p>
<hr class="default"/>
<?php
		$descdir = "vote/".$board."/desc.".$votearr[0]["DATE"] ;
		bbs_printansifile($descdir);
?>
<hr class="default"/>
<table width="613">
<tr><td>���</td><td><?php echo $num;?></tr>
<tr><td>����</td><td><?php echo $votearr[0]["TITLE"];?></tr>
<tr><td>����</td><td><?php echo $votearr[0]["TYPE"];?></tr>
<tr><td>������</td><td><?php echo $votearr[0]["USERID"];?></tr>
<tr><td>��������</td><td><?php echo date("r",$votearr[0]["DATE"]);?></tr>
<tr><td>ͶƱ����</td><td><?php echo $votearr[0]["MAXDAY"];?></tr>
</table>
<hr class="default"/>
<form action="/bbsvote.php" method="get">
<table width="613">
<?php
		if( $votearr[0]["TYPE"] == "��ѡ" ){

			for( $i=0; $i < $votearr[0]["TOTALITEMS"]; $i++){
				$itemstr = "ITEM".($i+1);
?>
<tr><td><?php echo $i+1;?></td>
<td><input type="checkbox" name="<?php echo $itemstr;?>"></td>
<td><?php echo $votearr[0][$itemstr];?></td></tr>
<?php
			}
		}
?>
</table>
<input type="hidden" name="board" value="<?php echo $board;?>">
<input type="hidden" name="num" value="<?php echo $num;?>">
<input type="submit" name="submit" value="ȷ��">
</form>
<?php
		html_normal_quit();

	}
?>
