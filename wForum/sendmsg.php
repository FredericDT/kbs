<?php


require("inc/funcs.php");

require("inc/usermanage.inc.php");

require("inc/user.inc.php");

html_init();

if ($loginok==1) {
	main();
}else {
	foundErr("��ҳ��Ҫ������ʽ�û���ݵ�½֮����ܷ��ʣ�");
}

if (isErrFounded()) {
	echo "<body>";
	html_error_quit();
}

show_footer();

function main() {
	global $currentuser;
	global $_GET;
	if (isset($_GET["destid"]))
		$destid = $_GET["destid"];
	else
		$destid = "";
	if (isset($_GET["destutmp"]))
		$destutmp = $_GET["destutmp"];
	else
		$destutmp = 0;
	settype($destutmp, "integer");
?>
<body  topmargin=0 leftmargin=0" onkeydown="if(event.keyCode==13 && event.ctrlKey)messager.submit()">

<form action="dosendmsg.php" method=post name=messager>
<input type="hidden" name="destutmp" value="<?php echo $destutmp; ?>"/>
<table cellpadding=3 cellspacing=1 align=center class=tableborder1>
          <tr> 
            <th colspan=3>���Ͷ���Ϣ��������������Ϣ��</td>
          </tr>
          <tr> 
            <td class=tablebody1 valign=middle><b>��ѶϢ��:</b></td>
            <td class=tablebody1 valign=middle>
              <input name="destid" maxlength="12" value="<?php echo $destid; ?>" size="12"/>
              <SELECT name=font onchange=DoTitle(this.options[this.selectedIndex].value)>
              <OPTION selected value="">ѡ��</OPTION>

			  </SELECT>
            </td>
          </tr>
           <tr> 
            <td class=tablebody1 valign=top width=15%><b>���ݣ�</b></td>
            <td  class=tablebody1 valign=middle>
              <input name="msg" maxlength="50" size="50" />
            </td>
          </tr>
          <tr> 
            <td  class=tablebody1 colspan=2>
<b>˵��</b>��<br>
�� ������ʹ��<b>Ctrl+Enter</b>����ݷ��Ͷ���<br>
<!--
�� ������Ӣ��״̬�µĶ��Ž��û�������ʵ��Ⱥ�������<b>5</b>���û�<br>
�� �������<b>50</b>���ַ����������<b>300</b>���ַ�<br>
-->
            </td>
          </tr>
          <tr> 
            <td  class=tablebody2 valign=middle colspan=2 align=center> 
              <input type=Submit value="����" name=Submit>
              &nbsp; 
              <input type=button value="�鿴�����¼" name="chatlog" onclick="location.href='showmsgs.php'">
              &nbsp; 
              <input type="button" name="close" value="�ر�" onclick="window.close()">
            </td>
          </tr>

        </table>
</form>
<?php

}
?>