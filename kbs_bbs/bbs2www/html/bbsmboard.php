<?php
	/**
	 * This file manager boards.
	 * $Id$
	 */
	require("funcs.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");

		/* check perm */
		if(0){
			html_error_quit("��û��Ȩ��");
		}

		if(isset($_GET["submit"])){

			if(isset($_GET["bname"])){
				$bname = $_GET["bname"];
			}else
				html_error_quit("��������");

			if(isset($_GET["btitle"])){
				$btitle = $_GET["btitle"];
			}else
				html_error_quit("���İ�������");

			if(isset($_GET["bbm"])){
				$bbm = $_GET["bbm"];
			}else
				$bbm = "";

			if(isset($_GET["section"])){
				$section = $_GET["section"];
			}else
				$section = "";

			if(isset($_GET["desp"])){
				$desp = $_GET["desp"];
			}else
				$desp = "";

			if(isset($_GET["blevel"])){
				$blevel = $_GET["blevel"];
				settype($blevel, "integer");
			}else
				$blevel = 0;

			if(isset($_GET["banony"])){
				$banony = 1;
			}else
				$banony = 0;

			if(isset($_GET["bjunk"])){
				$bjunk = 1;
			}else
				$bjunk = 0;

			if(isset($_GET["bout"])){
				$bout = 1;
			}else
				$bout = 0;

			if(isset($_GET["battach"])){
				$battach = 1;
			}else
				$battach = 0;

			if(isset($_GET["bgroup"])){
				$bgroup = $_GET["bgroup"];
			}else
				$bgroup = 0;

			$ret = bbs_new_board($bname,$section,$desp,$btitle,$bbm,$blevel,$banony,$bjunk,$bout,$bgroup,$battach);

			if($ret < 0)
				html_error_quit("����������ʧ��".$ret);
			else
				echo "������������";

		}	//submit
		else{
			$explains = array();
			$explain_num = bbs_get_explain( $explains );
?>

<form name="form0" action="<?php echo $_SERVER["PHP_SELF"]; ?>" method="get">
����Ӣ������:<input type="text" name="bname" value=""><br>
������������:<input type="text" name="btitle" value=""><br>
�������:<select name="section" class="input" style="WIDTH: 100px">
<?php
			for($i = 0; $section_nums[$i]!="" && $section_names[$i]!=""; $i++){
?>
<option <?php if($i == 0) echo "selected";?>value=<?php echo $section_nums[$i];?>><?php echo $section_names[$i][0];?></option>
<?php
			}
?>
</select><br>
���������������(����4���ַ�����2������):<input type="text" name="desp" value=""><br>
���������:<input type="text" name="bbm" value=""><br>
����Ȩ��:<input type="text" name="blevel" value=""><br>
���澫����λ��:<select name="bgroup" class="input" style="WIDTH: 100px">
<?php
			for($i = 0; $i < $explain_num; $i ++){
?>
<option value=<?php echo $explains[$i]["GROUPS"];?>><?php echo $explains[$i]["EXPLAIN"];?></option>
<?php
			}
?>
</select><br>
<input type="checkbox" name="banony">��������<br>
<input type="checkbox" name="bjunk">���治��������<br>
<input type="checkbox" name="bout">ת�Ű���<br>
<input type="checkbox" name="battach">��ճ������<br>
<input type="checkbox" name="bclubread">�����ƾ��ֲ�<br>
<input type="checkbox" name="bclubwrite">д���ƾ��ֲ�<br>
<input type="checkbox" name="bclubhide">�������ƾ��ֲ�<br>
<input type="submit" name="submit" value="ȷ��">
</form>
<?php
		}
		html_normal_quit();
	}
?>
