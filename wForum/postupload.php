<?php
$needlogin=1;
require("inc/funcs.php");
html_init();
?>
<body topmargin=0 leftmargin=0>
<form name="form" method="post" action="saveannounce_upfile.php?boardid=<?php echo $_REQUEST['boardid']; ?>" enctype="multipart/form-data">
<table width="100%" border=0 cellspacing=0 cellpadding=0>
<tr><td class=tablebody2 valign=top height=30>
<input type="hidden" name="MAX_FILE_SIZE" value="<?php echo ATTACHMAXSIZE; ?>">
<input type="file" name="upfile">
<input type="submit" name="Submit" value="�ϴ�" onclick="fname.value=file1.value,parent.document.forms[0].Submit.disabled=true,
parent.document.forms[0].Submit2.disabled=true;">
<font color=#FF0000 >���컹���ϴ�<?php   echo 'N/A'; ?>��</font>��
  ��̳���ƣ�һƪ����<?php   echo ATTACHMAXCOUNT; ?>����һ��<?php   echo $GroupSetting[50]; ?>��,ÿ��<?php   echo intval(ATTACHMAXSIZE/1024); ?>K
</td></tr>
</table>
</form>
</body>
</html>
<?php 
$conn=null;
?>
