<?php
	/**
	 * This file delete user's mail. --binxun
	 */
	require("funcs.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
	    html_init("gb2312");
        $filename = $_GET["file"];
		$dirname = $_GET["dir"];
		$title = $_GET["title"];

		$ret = bbs_delmail($dirname,$filename);
		switch($ret)
		{
		case 0:
		    //success
?>
�ż���ɾ��.<br><a href="/bbsreadmail.php?path=<?php echo $dirname;?>&title=<?php echo $title;?>">�����ż��б�</a>
<?php
		break;
		case -1:
		    http_error_quit("�ż�������, �޷�ɾ��");
			break;
        case -2:
		//wrong parameter
        http_error_quit("����Ĳ���!");
        break;
		}
	}
?>