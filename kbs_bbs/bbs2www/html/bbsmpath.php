<?php
	/**
	 *  This file control import_path .
     *  action:  todo: add, del, ...
     *  author:  stiger
	 */
	require("funcs.php");
	if ($loginok !=1 )
		html_nologin();
	else
	{
		html_init("gb2312");

		if(isset($_GET["action"])){
			$action = $_GET["action"];
			
		}

		$pathret = bbs_get_import_path();
		if( $pathret == FALSE )
			html_error_quit("û������˼·");
?>
<body>
<center><p><?php echo BBS_FULL_NAME; ?> -- [˿·����] [�û�: <?php echo $currentuser["userid"];?>]</p>
<hr class="default"/>
<table width="613">
<tr><td>���</td><td>˿·��</td><td>·��</td></tr>
<?php
		for($i = 0; $i < 40; $i ++){
?>
<tr><td>
<?php echo $i;?>
</td><td>
<?php echo $pathret[$i]["TITLE"];?>
</td><td>
<?php if($pathret[$i]["TITLE"][0]) echo $pathret[$i]["PATH"];?>
</td></tr>
<?php
		}
?>
</table>
<?php
		html_normal_quit();
	}
?>
