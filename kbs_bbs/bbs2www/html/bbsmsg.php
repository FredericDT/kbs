<?php
	require("funcs.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
		$filename=bbs_sethomefile($currentuser["userid"],"msgfile");
?>
<body>
<pre>
<?php
    $fp = fopen ($filename, "r");
    if ($fp!=false) {
        while (!feof ($fp)) {
            $buffer = fgets($fp, 300);
            echo ansi_convert($buffer,"#000000","#F0F0FF");
        }
        fclose ($fp);
    } else {
?>
û���κ�ѶϢ
<?php
}
?>
<a onclick="return confirm('�����Ҫ�������ѶϢ��?')" href="/cgi-bin/bbs/bbsdelmsg">�������ѶϢ</a> <a href="/cgi-bin/bbs/bbsmailmsg">�Ļ�������Ϣ</a></pre>
</pre>
</body>
</html>
<?php
	}
?>
