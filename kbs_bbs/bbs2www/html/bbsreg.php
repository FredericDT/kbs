<?php
	/**
	 * This file registry a new id, work with bbsreg.html
	 * by binxun 2003.5
	 */
	$needlogin=false;
	require("funcs.php");
	html_init("gb2312");

	@$userid=$_POST["userid"];
	@$pass1=$_POST["pass1"];
	@$pass2=$_POST["pass2"];
	@$nickname=$_POST["username"];

	@$realname=$_POST["realname"];
	@$dept=$_POST["dept"];
    @$address=$_POST["address"];
	@$year=$_POST["year"];
	@$month=$_POST["month"];
	@$day=$_POST["day"];
	@$email=$_POST["email"];
	@$phone=$_POST["phone"];
	@$gender=$_POST["gender"];


	if(strcmp($pass1,$pass2))
		html_error_quit("�����������벻һ��");
	else if(strlen($pass1) < 5 || !strcmp($pass1,$userid))
       	html_error_quit("���볤��̫�̻��ߺ��û�����ͬ!");

	$ret=bbs_createnewid($userid,$pass1,$nickname);
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
	$ret=bbs_createregform($userid,$realname,$dept,$address,$gender,$year,$month,$day,$email,$phone,TRUE);//�Զ�����ע�ᵥ
	switch($ret)
	{
	case 0:
		break;
	case 2:
		html_error_quit("���û�������!");
		break;
	case 3:
		html_error_quit("��������");
		break;
	default:
		html_error_quit("δ֪�Ĵ���!");
		break;
	}
?>
<body>
����BBSˮľ�廪ID�ɹ�,�����ڻ�û��ͨ�������֤,ֻ���������Ȩ��,���ܷ���,����,�����,�����ϵͳ���Զ�����ע�ᵥ.<br>
ע�ᵥͨ����˺�,�㽫��úϷ��û�Ȩ�ޣ�<br/><a href="http://www.smth.edu.cn">���ڵ�¼��վ</a>
</body>
</html>
