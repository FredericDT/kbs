<?php
	/**
	 * This file displays article to user.
	 * $Id$
	 */
	require("funcs.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
		if (isset($_GET["board"]))
			$board = $_GET["board"];
		else
			html_error_quit("�����������");
		// ����û��ܷ��Ķ��ð�
		$brdarr = array();
		$brdnum = bbs_getboard($board, $brdarr);
		if ($brdnum == 0)
			html_error_quit("�����������");
		$usernum = $currentuser["index"];
		if (bbs_checkreadperm($usernum, $brdnum) == 0)
			html_error_quit("�����������");
		$total = bbs_countarticles($brdnum, $dir_modes["NORMAL"]);
		if ($total <= 0)
			html_error_quit("��������Ŀǰû������");
		if (isset($_GET["num"]))
			$num = $_GET["num"];
		else
			html_error_quit("����Ĳ���");
		if (isset($_GET["id"]))
			$id = $_GET["id"];
		else
			html_error_quit("��������º�");
		$articles = bbs_get_records_from_id($brdarr["NAME"], $id, 
				$dir_modes["NORMAL"]);
		if ($articles == FALSE)
		{
			echo "bbs_get_records_from_id failed.";
		}
		else
		{
?>
<body>
<?php
			bbs_printansifile("boards/" . $brdarr["NAME"] . "/" . $articles[1]["FILENAME"]);
		}
		html_normal_quit();
	}
?>
