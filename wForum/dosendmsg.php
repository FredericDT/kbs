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


function main(){
	global $_POST;
	if (isset($_POST["destid"]))
		$destid = $_POST["destid"];
	else
		$destid = "";
	if (isset($_POST["msg"]))
		$msg = $_POST["msg"];
	else
		$msg = "";
	if (isset($_POST["destutmp"]))
		$destutmp = $_POST["destutmp"];
	else
		$destutmp = 0;
	settype($destutmp, "integer");
	if (strlen($destid) == 0 || strlen($msg) == 0)	{
		foundErr("���Ų�������");
		return false;
	}
	if (bbs_sendwebmsg($destid, $msg, $destutmp, $errmsg)==FALSE){
		foundErr($errmsg);
		return false;
	}
	setSucMsg("��Ϣ�ѳɹ����ͣ�");
	return html_success_quit('���������¼', 'showmsgs.php');
}


?>