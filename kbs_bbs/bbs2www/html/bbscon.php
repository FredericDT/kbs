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
		if (isset($_GET["board"]))
			$board = $_GET["board"];
		else {
			html_init("gb2312");
			html_error_quit("�����������");
		}
		// ����û��ܷ��Ķ��ð�
		$brdarr = array();
		$brdnum = bbs_getboard($board, $brdarr);
		if ($brdnum == 0) {
			html_init("gb2312");
			html_error_quit("�����������");
		}
		$usernum = $currentuser["index"];
		if (bbs_checkreadperm($usernum, $brdnum) == 0) {
			html_init("gb2312");
			html_error_quit("�����������");
		}
		$total = bbs_countarticles($brdnum, $dir_modes["NORMAL"]);
		if ($total <= 0) {
			html_init("gb2312");
			html_error_quit("��������Ŀǰû������");
		}
		if (isset($_GET["id"]))
			$id = $_GET["id"];
		else {
			html_init("gb2312");
			html_error_quit("��������º�");
		}
		$articles = bbs_get_records_from_id($brdarr["NAME"], $id, 
				$dir_modes["NORMAL"]);
		if ($articles == FALSE)
		{
			html_init("gb2312");
			html_error_quit("bbs_get_records_from_id failed.");
		}
		else
		{
			$filename=bbs_get_board_filename($brdarr["NAME"], $articles[1]["FILENAME"]);
                	if (cache_header("public",filemtime($filename),300))
                		return;
			@$attachpos=$_GET["ap"];//pointer to the size after ATTACHMENT PAD
			if ($attachpos!=0) {
				$file = fopen($filename, "r");
				fseek($file,$attachpos);
				fread($file,$attachsize,4);
				$attachname='';
				while (($char=fgetc($file,1)!='\0') {
					$attachname=$attachname . $char;
				}
				@$mimetype=mime_content_type($attachname);
				if ($mimetype!='')
					Header("Content-type: " . $mimetype);
				Header("Accept-Ranges: bytes");
				Header("Accept-Length: " . filesize($filename));
				Header("Content-Disposition: attachment; filename=" . $attachname);
				echo fread($file,$attachsize);
				fclose($file);
				exit;
			} else
			{
				html_init("gb2312");
?>
<body>
<?php
				bbs_printansifile($filename,1,$_SERVER['PHP_SELF'] . '?' . $_SERVER['QUERY_STRING']);
			}
		}
		html_normal_quit();
	}
?>
