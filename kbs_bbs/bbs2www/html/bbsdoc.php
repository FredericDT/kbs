<?php
	/**
	 * This file lists articles to user.
	 * $Id$
	 */
	require("funcs.php");
	
	function display_navigation_bar($brdarr,$brdnum,$start,$total,$page,$order=FALSE)
	{
		global $section_names;
		$brd_encode = urlencode($brdarr["NAME"]);
	?>		
<table width="100%" border="0" cellspacing="0" cellpadding="0" class="b1">		
<form name="form1" action="<?php echo $_SERVER["PHP_SELF"]; ?>" method="get">
<input type="hidden" name="board" value="<?php echo $brdarr["NAME"]; ?>"/>
<tr>
<td>
    	<?php
    		if (strcmp($currentuser["userid"], "guest") != 0)
		{
    	?>
<a href="bbspst.php?board=<?php echo $brd_encode; ?>"><img src="images/postnew.gif" border="0" alt="发表话题"></a>
    	<?php
    		}
    	?>
</td>
<td align="right">
 	<?php
		      if($order)
		      {
		   	if ($start <= $total - 20)
			{
		    ?>
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>">第一页</a>]
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&page=<?php echo $page + 1; ?>">上一页</a>]
		    <?php
			}
			else
			{
		    ?>
[第一页] 
[上一页]
		    <?php
			}
			if ($page > 1)
			{
		?>
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&page=<?php echo $page - 1; ?>">下一页</a>]
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&page=1">最后一页</a>]
		    <?php
			}
			else
			{
		    ?>
[下一页] 
[最后一页]
		    <?php
			}
		     }
		     else
		     {	
		     	if ($page > 1)
			{
		?>
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&page=1">第一页</a>]
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&page=<?php echo $page - 1; ?>">上一页</a>]
		    <?php
			}
			else
			{
		    ?>
[第一页] 
[上一页]
		    <?php
			}
			if ($start <= $total - 20)
			{
		    ?>
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&page=<?php echo $page + 1; ?>">下一页</a>]
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>">最后一页</a>]
		    <?php
			}
			else
			{
		    ?>
[下一页] 
[最后一页]
		    <?php
			}
		    }
	?>
<input type="submit" class="b5" value="跳转到"/> 第 <input type="text" name="start" size="3"  onmouseover=this.focus() onfocus=this.select() class="b5"> 篇 
</td></tr></form></table>
	<?php
	}


	function display_articles($brdarr,$articles,$start,$order=FALSE)
	{
		global $dir_modes;
		global $default_dir_mode;
		$brd_encode = urlencode($brdarr["NAME"]);
?>
<table width="100%" border="0" cellspacing="0" cellpadding="3" class="t1">
<tr><td class="t2" width="50">序号</td><td class="t2" width="40">标记</td><td class="t2" width="120">作者</td><td class="t2" width="80">日期</td><td class="t2">标题</td></tr>
<?php
		$ding_cnt = 0;
		foreach ($articles as $article)
		{
			$flags = $article["FLAGS"];
			if (!strncmp($flags,"D",1)||!strncmp($flags,"d",1))
				$ding_cnt++;
		}
		$i = 0;
		if ($order) {
			$articles = array_reverse($articles);
			$i = count($articles) - $ding_cnt - 1;
		}
		foreach ($articles as $article)
		{
			$title = $article["TITLE"];
			if (strncmp($title, "Re: ", 4) != 0)
				$title = "● " . $title;

			$flags = $article["FLAGS"];
?>
<tr>
<?php
			if (!strncmp($flags,"D",1)||!strncmp($flags,"d",1)) {
?>
<td colspan="2" align="center" class="t6"><img src="images/istop.gif" alt="提示" align="absmiddle"> 提示</td>
<?php

			} else {
?>
<td class="t3"><?php echo $start+$i; ?></td>
<td class="t4">&nbsp;
<?php
			if ($flags[1] == 'y')
			{
				if ($flags[0] == ' ')
					echo "&nbsp;";
				else
					echo $flags[0];
			}
			elseif ($flags[0] == 'N' || $flags[0] == '*')
				echo $flags[0];
			else
				echo $flags[0];
			echo $flags[3];
?>
&nbsp;</td>
<?php
	}//置顶
?>
<td class="t3"><a class="ts1" href="/cgi-bin/bbs/bbsqry?userid=<?php echo $article["OWNER"]; ?>"><?php echo $article["OWNER"]; ?></a></td>
<td class="t4"><?php echo strftime("%b&nbsp;%e", $article["POSTTIME"]); ?></td>
<td class="t5">&nbsp;
<?php
	switch ($default_dir_mode)
	{
	case $dir_modes["ORIGIN"]:
		if (!strncmp($flags,"D",1)||!strncmp($flags,"d",1))
		{
?>
<a class="ts2" href="/bbscon.php?board=<?php echo $brd_encode; ?>&id=<?php echo $article["ID"]; ?>&ftype=9"><?php echo htmlspecialchars($title); ?>

</a>
<?php
		}
		else
		{
?>
<a class="ts2" href="/cgi-bin/bbs/bbstcon?board=<?php echo $brd_encode; ?>&gid=<?php echo $article["GROUPID"]; ?>"><?php echo htmlspecialchars($title); ?>

</a>
<?php
		}
		break;
	case $dir_modes["NORMAL"]:
	default:
?>
<a class="ts2" href="/bbscon.php?board=<?php echo $brd_encode; ?>&id=<?php echo $article["ID"]; ?><?php if (!strncmp($flags,"D",1)||!strncmp($flags,"d",1)) echo "&ftype=9"; ?>"><?php echo htmlspecialchars($title); ?>

</a>
<?php
	}
?>
</td>
</tr>
<?php
			if ($order)
				$i--;
			else
				$i++;
		}
?>
</table>
<?php
	}

	if ($loginok != 1)
		html_nologin();
	else
	{
		$board_list_error=FALSE;
		if (isset($_GET["board"]))
			$board = $_GET["board"];
		else{
		        html_init("gb2312","","",1);
			html_error_quit("错误的讨论区");
			$board_list_error=TRUE;
			}
		// 检查用户能否阅读该版
		$brdarr = array();
		$brdnum = bbs_getboard($board, $brdarr);
		if ($brdnum == 0){
		        html_init("gb2312","","",1);
			html_error_quit("错误的讨论区");
			$board_list_error=TRUE;
			}
		$usernum = $currentuser["index"];
		if (bbs_checkreadperm($usernum, $brdnum) == 0){
		        html_init("gb2312","","",1);
			html_error_quit("错误的讨论区");
			$board_list_error=TRUE;
			}
		if ($brdarr["FLAG"]&BBS_BOARD_GROUP) {
			for ($i=0;$i<sizeof($section_nums);$i++)
				if (!strcmp($section_nums[$i],$brdarr["SECNUM"])) {
			         Header("Location: bbsboa.php?group=" . $i . "&group2=" . $brdnum);
			         return;
                                }
		        html_init("gb2312","","",1);
			html_error_quit("错误的讨论区");
			$board_list_error=TRUE;
		}

		if (!isset($default_dir_mode))
			$default_dir_mode = $dir_modes["NORMAL"];
                $isnormalboard=bbs_normalboard($board);

        	bbs_set_onboard($brdnum,1);
		if ($isnormalboard&&($default_dir_mode == $dir_modes["NORMAL"])) {
                        $dotdirname=BBS_HOME . "/boards/" . $brdarr["NAME"] . "/.DIR";
       			if (cache_header("public, must-revalidate",filemtime($dotdirname),10))
               			return;
               	}
		html_init("gb2312","","",1);
		$total = bbs_countarticles($brdnum, $default_dir_mode);
		if ($total <= 0)
		    if (strcmp($currentuser["userid"], "guest") != 0){
			html_error_quit("本讨论区目前没有文章<br /><a href=\"bbspst.php?board=" . $board . "\">发表文章</a>");
			$board_list_error=TRUE;
			}
                    else{
			html_error_quit("本讨论区目前没有文章");
			$board_list_error=TRUE;
			}

		$artcnt = 20;
		if (isset($_GET["page"]))
			$page = $_GET["page"];
		elseif (isset($_POST["page"]))
			$page = $_POST["page"];
		else
		{
			if (isset($_GET["start"]))
			{
				$start = $_GET["start"];
				settype($start, "integer");
				$page = ($start + $artcnt - 1) / $artcnt;
			}
			else
				$page = 0;
		}
		settype($page, "integer");
		if ($page > 0)
			$start = ($page - 1) * $artcnt + 1;
		else
			$start = 0;
		/*
		 * 这里存在一个时间差的问题，可能会导致序号变乱。
		 * 原因在于两次调用 bbs_countarticles() 和 bbs_getarticles()。
		 */
		if ($start == 0 || $start > ($total - $artcnt + 1))
		{
			if ($total <= $artcnt)
			{
				$start = 1;
				$page = 1;
			}
			else
			{
				$start = ($total - $artcnt + 1);
				$page = ($start + $artcnt - 1) / $artcnt + 1;
			}
		}
		else
			$page = ($start + $artcnt - 1) / $artcnt;
		settype($page, "integer");
		$articles = bbs_getarticles($brdarr["NAME"], $start, $artcnt, $default_dir_mode);
		if ($articles == FALSE){
			html_error_quit("读取文章列表失败");
			$board_list_error=TRUE;
			}
		$bms = explode(" ", trim($brdarr["BM"]));
		$bm_url = "";
		if (strlen($bms[0]) == 0 || $bms[0][0] <= chr(32))
			$bm_url = "诚征版主中";
		else
		{
			if (!ctype_alpha($bms[0][0]))
				$bm_url = $bms[0];
			else
			{
				foreach ($bms as $bm)
				{
					$bm_url .= sprintf("<a class=\"b3\" href=\"/cgi-bin/bbs/bbsqry?userid=%s\">%s</a> ", $bm, $bm);
				}
				$bm_url = trim($bm_url);
			}
		}
		if (!isset($order_articles))
			$order_articles = FALSE;
			

?>
<body>
<?php
	if($board_list_error==FALSE)
	{
		
		$brd_encode = urlencode($brdarr["NAME"]);
		$ann_path = bbs_getannpath($brdarr["NAME"]);
?>
<a name="listtop"></a>
<table width="100%" border="0" cellspacing="0" cellpadding="3">
  <tr> 
    <td colspan="2" class="b2">
	    <a href="bbssec.php" class="b2"><?php echo BBS_FULL_NAME; ?></a>
	    -
	    <?php
	    	$sec_index = get_secname_index($brdarr["SECNUM"]);
		if ($sec_index >= 0)
		{
	    ?>
		<a href="/bbsboa.php?group=<?php echo $sec_index; ?>" class="b2"><?php echo $section_names[$sec_index][0]; ?></a>
	    <?php
		}
	    ?>
	    -
	    <?php echo $brdarr["NAME"]; ?>版(<a href="bbsnot.php?board=<?php echo $brd_encode; ?>" class="b2">进版画面</a>|<a href="/cgi-bin/bbs/bbsbrdadd?board=<?php echo $brdarr["NAME"]; ?>" class="b2">预定本版</a>)
    </td>
  </tr>
  <tr> 
    <td colspan="2" align="center" class="b4"><?php echo $brdarr["NAME"]."(".$brdarr["DESC"].")"; ?> 版</td>
  </tr>
  <tr><td class="b1">
  <img src="images/bm.gif" alt="版主" align="absmiddle">版主 <?php echo $bm_url; ?>
  </td></tr>
  <tr> 
    <td class="b1">
    <img src="images/online.gif" alt="本版在线人数" align="absmiddle">在线 <font class="b3"><?php echo $brdarr["CURRENTUSERS"]+1; ?></font> 人
    <img src="images/postno.gif" alt="本版文章数" align="absmiddle">文章 <font class="b3"><?php echo $total; ?></font> 篇
    </td>
    <td align="right" class="b1">
	    <img src="images/gmode.gif" align="absmiddle" alt="文摘区"><a class="b1" href="bbsgdoc.php?board=<?php echo $brd_encode; ?>">文摘区</a> 
	    <?php
  	    	if ($ann_path != FALSE)
		{
                    if (!strncmp($ann_path,"0Announce/",10))
			$ann_path=substr($ann_path,9);
	    ?>
	    | 
  	    <img src="images/soul.gif" align="absmiddle" alt="精华区"><a class="b1" href="/cgi-bin/bbs/bbs0an?path=<?php echo urlencode($ann_path); ?>">精华区</a>
	    <?php
		}
	    ?>
	    | 
  	    <img src="images/search.gif" align="absmiddle" alt="版内查询"><a class="b1" href="/bbsbfind.php?board=<?php echo $brd_encode; ?>">版内查询</a>
	    <?php
    		if (strcmp($currentuser["userid"], "guest") != 0)
		{
    	    ?>
	    | 
  	    <img src="images/vote.gif" align="absmiddle" alt="本版投票"><a class="b1" href="/bbsshowvote.php?board=<?php echo $brd_encode; ?>">本版投票</a>
	    | 
  	    <img src="images/model.gif" align="absmiddle" alt="发文模板"><a class="b1" href="/bbsshowtmpl.php?board=<?php echo $brd_encode; ?>">发文模板</a>
    	    <?php
    		}
    	    ?>	
    </td>
  </tr>
  <tr> 
    <td colspan="2" height="9" background="images/dashed.gif"> </td>
  </tr>
  <tr><td colspan="2" align="right" class="b1">
  <?php
  	display_navigation_bar($brdarr, $brdnum, $start, $total, $page,$order_articles );
  ?>
  </td></tr>
  <tr> 
    <td colspan="2" align="center">
    	<?php
		display_articles($brdarr, $articles, $start, $order_articles );
	?>	
    </td>
  </tr>
  <tr><td colspan="2" align="right" class="b1">
  <?php
  	display_navigation_bar($brdarr, $brdnum, $start, $total, $page,$order_articles);
  ?>
  </td></tr>
  <tr> 
    <td colspan="2" height="9" background="images/dashed.gif"> </td>
  </tr>
  <tr> 
    <td colspan="2" align="center" class="b1">
    	[<a href="#listtop">返回顶部</a>]
    	[<a href="javascript:location=location">刷新</a>]
    	[<a href="bbstdoc.php?board=<?php echo $brd_encode; ?>">同主题模式</a>]
    	<?php
    		if (strcmp($currentuser["userid"], "guest") != 0)
		{
    	?>
    	[<a href="/cgi-bin/bbs/bbsclear?board=<?php echo $brd_encode; ?>&start=<?php echo $start; ?>">清除未读</a>]
	<?php
		}
		if (bbs_is_bm($brdnum, $usernum))
		{
	?>
	[<a href="bbsmdoc.php?board=<?php echo $brd_encode; ?>">管理模式</a>]
	<?php
		}
	?>
    </td>
  </tr>
</table>
<?php
	}
		html_normal_quit();
	}
?>
