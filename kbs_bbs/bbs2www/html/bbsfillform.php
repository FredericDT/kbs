<?php
	/**
	 * This file Fill registry form.
	 * by binxun 2003.5
	 */
	require("funcs.php");
	html_init("gb2312");

	if ($loginok != 1)
		html_nologin();
	else
	{
		@$realname=$_POST["realname"];
		@$dept=$_POST["dept"];
		@$address=$_POST["address"];
		@$year=$_POST["year"];
		@$month=$_POST["month"];
		@$day=$_POST["day"];
		@$email=$_POST["email"];
		@$phone=$_POST["phone"];


	if(!strcmp($currentuser["userid"],"guest"))
		html_error_quit("������������ʺ���дע�ᵥ!");

	//�û��Ѿ�ͨ��ע��
	//δ���ȴ�ʱ��

	$ret=bbs_createregform($realname,$dept,$address,$year,$month,$day,$email,$phone,FALSE); //�ֹ���дע�ᵥ

	switch($ret)
	{
	case 0:
		break;	
	case 1:
		html_error_quit("����ע�ᵥ��û�д��������ĵȺ�");
		break;
	default:
		html_error_quit("δ֪�Ĵ���!");
		break;
	}
?>
<body>
ע�ᵥ�Ѿ��ύ,24Сʱ��վ�񽫻����,���ͨ��,��ͻ��úϷ��û�Ȩ�ޣ�
</body>
</html>
