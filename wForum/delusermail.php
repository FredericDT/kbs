<?php
require("inc/funcs.php");

require("inc/usermanage.inc.php");

require("inc/user.inc.php");

setStat("ɾ���ʼ�");

show_nav();

echo "<br>";

$boxDesc=getMailBoxName($_POST['boxname']);

if (!isErrFounded()) {
	head_var($userid."��".$boxDesc,"usermailbox.php?boxname=".$_POST['boxname'],0);
}

if ($loginok==1) {
	main();
}else {
	foundErr("��ҳ��Ҫ������ʽ�û���ݵ�½֮����ܷ��ʣ�");
}


if (isErrFounded()) {
		html_error_quit();
} 
show_footer();

function main(){
	global $_POST;
	global $boxDesc;
	$boxName=$_POST['boxname'];
	if (!isset($_POST['nums'])) {
		foundErr("����ָ�����ż�������!");
		return false;
	}
	$action=$_POST['action'];
	if ($action=='deleteAll') {
		return deleteAll($boxName,$boxDesc);
	}
	if ($action!='delete') {
		foundErr("��������");
		return false;
	}
	$nums=split(',',$_POST['nums']);
	if ($boxName=='') {
		$boxName='inbox';
	}
	if ($boxName=='inbox') {

		deleteMails('inbox','.DIR','�ռ���', $nums);
		return true;
	}
	if ($boxName=='sendbox') {

		deleteMails('sendbox','.SENT','������',$nums );
		return true;
	}
	if ($boxName=='deleted') {
		deleteMails('deleted','.DELETED','������',$nums);
		return true;
	}
	foundErr("��ָ���˴�����������ƣ�");
	return false;
}

function deleteMails($boxName, $boxPath, $boxDesc, $nums){
	global $currentuser;
	$dir = bbs_setmailfile($currentuser["userid"],$boxPath);

	$total = filesize( $dir ) / 256 ;
	if( $total <= 0 ){
		foundErr("����ָ�����ż������ڡ�");
		return false;
	}
	$articles = array ();
	$mailnum=count($nums);
	$filenames=array();
	for ($i=0;$i<$mailnum;$i++) {
		if( bbs_get_records_from_num($dir, intval($nums[$i]), $articles) ) {
			$filenames[] = $articles[0]["FILENAME"];
		}else{
			foundErr("����ָ�����ż������ڡ�");
			return false;
		}
	}
	for ($i=0;$i<$mailnum;$i++){
		$ret = bbs_delmail($boxPath,$filenames[$i]);
		switch($ret){
		case 0:
			break;
		case -1:
			foundErr("����ָ���ż�������, �޷�ɾ����");
			return false;
			break;
		case -2:
			foundErr("����Ĳ���!");
			return false;
			break;
		}
	}
	setSucMsg("�ʼ��ѳɹ�ɾ����");
	return html_success_quit('����'.$boxDesc, 'usermailbox.php?boxname='.$boxName);
}
?>