<?php
	/**
	 * This file lists articles to user.
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
		// TODO: Added bbs_check_post_perm();
		if (isset($_GET["reid"]))
			$reid = $_GET["reid"];
		else {
			$reid = 0;
		}
		settype($reid, "integer");
		if ($reid > 0)
		{
			$articles = bbs_get_records_from_id($brdarr["NAME"], $reid, 
					$dir_modes["NORMAL"]);
			if ($articles == FALSE)
			{
				html_error_quit("����� Re �ı��");
			}
		}
		$brd_encode = urlencode($brdarr["NAME"]);
	}
?>
<body>
<center>
<?php echo $BBS_FULL_NAME; ?> -- �������� [ʹ����: <?php echo $currentuser["userid"]; ?>]
<hr class="default" />
<form name="postform" method="post" action="/cgi-bin/bbs/bbssnd?board=<?php echo $brd_encode; ?>&reid=<?php echo $reid; ?>">
<table border="1">
<tr>
<td>
<?php
		$top_file= bbs_get_vote_filename($brdarr["NAME"], "notes");
		$fp = fopen($top_file, "r");
		if ($fp == FALSE)
		{
			//html_error_quit("����û�б���¼");
		}
		fclose($fp);
?>
</td>
</tr>
<tr><td>
����: <script language="JavaScript">document.write(getCookie("UTMPUSERID"));</script><br />
ʹ�ñ���: <input type="text" name="title" size="40" maxlength="100" value="">
������: [<script language="JavaScript">document.write(queryString("board"));</script>]<br />
<script language="JavaScript">
<!--
    if (queryString("attach",0)==1)
    	document.write("������<input type=\"text\" name=\"attachname\" size=\"50\" value=\"\" disabled > ");
-->
</script>
ʹ��ǩ���� <select name="signature">
<script language="JavaScript">
<!--
var num=queryString("totalsig",5);
var sel=queryString("sig",1);
if (sel>99) sel=1;
if (sel>num) num=sel;
if (sel==0)
    document.write("<option value=\"0\" selected>��ʹ��ǩ����</option>");
else
    document.write("<option value=\"0\">��ʹ��ǩ����</option>");
for (i=1;i<=num;i++) {
  if (i==sel)
    document.write("<option value=\""+i+"\" selected>�� "+i+" ��</option>");
  else
    document.write("<option value=\""+i+"\">�� "+i+" ��</option>");
}
-->
</script>
</select>
 [<a target="_balnk" href="bbssig.php">�鿴ǩ����</a>] 
<input type="checkbox" name="outgo" value="1">ת��
<br>
<textarea name="text" rows="20" cols="80" wrap="physical">

</textarea></td></tr>
<tr><td class="post" align="center">
<input type="submit" value="����"> 
<input type="reset" value="���">
<script language="JavaScript">
<!--
   function GoAttachWindow(){     
	
   	var hWnd = window.open("bbsupload.php","_blank","width=600,height=300,scrollbars=yes");  

	if ((document.window != null) && (!hWnd.opener))  

		   hWnd.opener = document.window;  

	hWnd.focus();  

   	return false;  

   }  

    if (queryString("attach",0)!=0)
    	document.write("<input type=\"button\" name=\"attach22\" value=\"����\" onclick=\"return GoAttachWindow()\">");
-->
</script>
</td></tr>
</table></form>

</html>
<?php
?>
