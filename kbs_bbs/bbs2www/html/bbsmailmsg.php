<?php
	/**
	 * This file mail message log to current user.
	 * $Id$
	 */
	require("funcs.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
        if ($currentuser["userid"]=="guest")
			html_error_quit("�Ҵҹ��Ͳ��ܴ���ѶϢ�����ȵ�¼");
		$filename = bbs_sethomefile($currentuser["userid"],"msgfile");
		$title = sprintf("[%s] ����ѶϢ����", strftime("%b %e %H:%M"));
		if (bbs_mail_file($currentuser["userid"], $filename, 
				$currentuser["userid"], $title, 1) == false)
		{
			html_error_quit("ѶϢ���ݼĻ�����ʧ��");
		}
		$filename = bbs_sethomefile($currentuser["userid"], "msgcount");
		if (file_exists($filename))
			unlink($filename);
?>
<body>
ѶϢ�����Ѿ��Ļ���������<a href="javascript:history.go(-2)">����</a>
<?php
		html_normal_quit();
	}
?>
