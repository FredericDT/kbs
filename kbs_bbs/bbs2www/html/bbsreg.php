<?php
	/**
	 * This file registry a new id, work with bbsreg.html
	 * by binxun 2003.5
	 */
	$needlogin=false;
	require("funcs.php");
	html_init("gb2312");

	@$userid=$_POST["userid"];
	@$nickname=$_POST["username"];

	@$realname=$_POST["realname"];
	@$dept=$_POST["dept"];
    @$address=$_POST["address"];
	@$year=$_POST["year"];
	@$month=$_POST["month"];
	@$day=$_POST["day"];
	@$reg_email=$_POST["reg_email"];
	@$phone=$_POST["phone"];
	@$gender=$_POST["gender"];
	@$m_register=$_POST["m_register"];
	@$mobile_phone=$_POST["mobile_phone"];

    if(!strchr($reg_email,'@'))
	    html_error_quit("�����ע�� email ��ַ!");

	//generate passwd
	$password=bbs_findpwd_check("","","");

	//�������뵽reg_email
/*	bool mail ( string to, string subject, string message [, string
additional_headers [, string additional_parameters]])*/
    if(!mail($reg_email,"BBSˮľ�廪վ�û�ע������",$userid . "��������" . $password))
	    html_error_quit("�������뵽����ע��Emailʧ��!��ȷ�����ĸ�Email��ַ��ȷ");

	//create new id
	$ret=bbs_createnewid($userid,$password,$nickname);

	switch($ret)
	{
	case 0:
			break;
	case 1:
			html_error_quit("�û����з�������ĸ�ַ��������ַ�������ĸ!");
			break;
	case 2:
			html_error_quit("�û�������Ϊ������ĸ!");
			break;
	case 3:
			html_error_quit("ϵͳ���ֻ�������!");
			break;
	case 4:
			html_error_quit("���û����Ѿ���ʹ��!");
			break;
	case 5:
			html_error_quit("�û���̫��,�12���ַ�!");
			break;
	case 6:
			html_error_quit("����̫��,�39���ַ�!");
			break;
	case 10:
			html_error_quit("ϵͳ����,����ϵͳ����ԱSYSOP��ϵ.");
			break;
	default:
			html_error_quit("ע��IDʱ����δ֪�Ĵ���!");
			break;
	}

	if(!strcmp($gender,"��"))$gender=1;
    else
        $gender=2;
    settype($year,"integer");
	settype($month,"integer");
	settype($day,"integer");
	settype($m_register,"bool");

    if(!$m_register)$mobile_phone="";
	$ret=bbs_createregform($userid,$realname,$dept,$address,$gender,$year,$month,$day,$reg_email,$phone,$mobile_phone,TRUE);//�Զ�����ע�ᵥ
	switch($ret)
	{
	case 0:
		break;
	case 2:
		html_error_quit("���û�������!");
		break;
	case 3:
		html_error_quit("����ע�ᵥ���� ��������! ��������ֹ���дע�ᵥ");
		break;
	default:
		html_error_quit("����ע�ᵥ���� δ֪�Ĵ���! ��������ֹ���дע�ᵥ");
		break;
	}
?>
<body>
����BBSˮľ�廪ID�ɹ�,�����ڻ�û��ͨ�������֤,ֻ���������Ȩ��,���ܷ���,����,�����,�����ϵͳ���Զ�����ע�ᵥ.<br>
ע�ᵥͨ����˺�,�㽫��úϷ��û�Ȩ�ޣ�<br/><a href="https://www.smth.edu.cn">���ڵ�¼��վ</a>
</body>
</html>
