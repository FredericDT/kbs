/*
 * $Id$
 */
#define MY_CSS "/bbsold.css"
#include "bbslib.h"

void printmenubegin()
{
    printf("<tr><td nowrap>\n");
}

void printdiv(int n, char *str, char *imgurl)
{
    printmenubegin();
    printf("<img id=\"img%d\" src=\"%s\">", n, imgurl);
    printf("<A href=\"javascript:changemn('%d');\">%s</A></div>\n", n, str);
    printf("<div id=\"div%d\" style=\"display: none\">\n", n);
}

void printmenuend()
{
    printf("</td><td>&nbsp;</td></tr>\n");
}

void printmenu(char* s)
{
    printmenubegin();
    printf("%s",s);
    printmenuend();
}

int main()
{
    char buf[256], *ptr;
    int i;

    init_all();
    printf("<style type=\"text/css\">A {color: #000080}</style><br>\n");
    printf("<script src=\"/func.js\"></script>" "<body class=\"dark\" leftmargin=\"5\" topmargin=\"1\" MARGINHEIGHT=\"1\" MARGINWIDTH=\"1\">");
    printf("<table  cellSpacing=0 cellPadding=0 width=\"100%\" border=0><tr><td>");
    printf("<table width=\"100%%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
    printf("<tr><td>");
    if (currentuser == NULL)
        http_fatal("hehe:%d", loginok);
    printf("�û�: <a href=\"bbsqry?userid=%s\" target=\"f3\">%s</a><br>", getcurruserid(), getcurruserid());
    uleveltochar(buf, getcurrusr());
    printf("���: %s<br>\n", buf);
    printf("<a href=\"/bbslogout.php\" target=\"_top\">ע�����ε�¼</a><br>\n");
    printf("</td></tr><tr><td>\n");
    printf("<hr style=\"color:#2020f0; height:1px\" width=\"84px\" align=\"center\">\n");
    printf("</td></tr></table>");
    printf("</td></tr><tr><td>");
    printf("<table border=0 cellspacing=0 cellpadding=0 align=right width=95%>");
    printf("<img src=\"/images/home.gif\"><a href=\"/bbssec.php\" target=\"f3\">��������ҳ</a><br>\n");
    printf("<img src=\"/images/link0.gif\"><a target=\"f3\" href=\"bbs0an\">����������</a><br>\n");
    printf("<img src=\"/images/link0.gif\"><a target=\"f3\" href=\"/bbstop10.php\">����ʮ��</a><br>\n");
    /*printf("<img src=\"/images/link0.gif\"><a target=\"f3\" href=\"bbstopb10\">����������</a><br>\n"); */
    if (loginok) {
        int i, mybrdnum = 0;
        const struct boardheader *bptr;
        char *name;

        printf("<img src=\"/images/folder.gif\">" "<a target=\"f3\" href=\"bbsfav?select=-1\">���˶�����</a><br>\n");
/*		printdiv(1,"���˶�����","/images/folder.gif");
		load_favboard(0);
		mybrdnum = getfavnum();
 		for(i=0; i<mybrdnum; i++)
		{
			name = get_favboard(i);
			if (name == NULL)
				continue;
			if (get_favboard_type(i))
     		printf("<img src=\"/images/link.gif\">"
				" <a target=\"f3\" href=\"/bbsdoc.php?board=%s\">%s</a><br>\n",
				encode_url(buf, name, sizeof(buf)), name);
			else
     		printf("<img src=\"/images/folder.gif\">"
				" <a target=\"f3\" href=\"bbsfav?select=%d\">%s</a><br>\n",
				get_favboard_id(i), name);
		}
		printf("<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsmybrd\">"
				"Ԥ������</a><br>\n");
		printf("</div>\n");
		release_favboard();*/
    }
    printdiv(2, "����������", "/images/folder.gif");
    for (i = 0; i < SECNUM; i++)
        printf("<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsboa.php?group=%d\">%s</a><br>\n", i, secname[i][0]);
    printf("</div>\n");
    printmenuend();

    printdiv(3, "̸��˵����", "/images/folder.gif");
    if (loginok) {
        printf("<img src=\"/images/link.gif\"> <a href=\"bbsfriend\" target=\"f3\">���ߺ���</a><br>\n");
    }
    printf("<img src=\"/images/link.gif\"> <a href=\"bbsusr\" target=\"f3\">�����ķ�</a><br>\n");
    printf("<img src=\"/images/link.gif\"> <a href=\"bbsqry\" target=\"f3\">��ѯ����</a><br>\n");
    if (loginok && currentuser->userlevel & PERM_PAGE) {
        printf("<img src=\"/images/link.gif\"> <a href=\"/bbssendmsg.php\" target=\"f3\">����ѶϢ</a><br>\n");
        printf("<img src=\"/images/link.gif\"> <a href=\"/bbsmsg.php\" target=\"f3\">�鿴����ѶϢ</a><br>\n");
    }
    printf("</div>\n");
    printmenuend();
    ptr = "";
    if (loginok && currentuser->userlevel & PERM_CLOAK)
        ptr = "<img src=\"/images/link.gif\"> <a target=\"f3\" onclick=\"return confirm('ȷʵ�л�����״̬��?')\" href=\"/bbscloak.php\">�л�����</a><br>\n";
    if (loginok) {
        printdiv(4, "���˹�����", "/images/folder.gif");
        printf("			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsinfo\">��������</a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsplan.php\">��˵����<a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbssig.php\">��ǩ����<a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbspwd\">�޸�����</a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsparm\">�޸ĸ��˲���</a><br>\n"
/*
"			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsmywww\">WWW���˶���</a><br>\n"
*/
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsnick\">��ʱ���ǳ�</a><br>\n"
/*"		<img src=\"/images/link.gif\"><a target=\"f3\" href=\"bbsstat\"> ����ͳ��</a><br>"*/
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsfall\">�趨����</a><br>\n" "			%s</div>", ptr);
        printmenuend();
        printdiv(5, "�����ż���", "/images/folder.gif");
        printf("			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsnewmail\">�������ʼ�</a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsmail\">�����ʼ�</a><br>\n"
         	   "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsmailbox_system.php\">ϵͳԤ��������</a><br>\n"
		   "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsmailbox_custom.php\">�Զ�������</a><br>\n");
        if (can_send_mail())
            printf("			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbspstmail\">�����ʼ�</a><br>\n");
        printf("			</div>\n");
        printmenuend();
    }

    printdiv(6, "������", "/images/folder.gif");
    printf("<img src=\"/images/link.gif\"> <a href=\"javascript:change_style(0)\">С����</a><br>\n");
    printf("<img src=\"/images/link.gif\"> <a href=\"javascript:change_style(1)\">������</a><br>\n");
    /*printf("<img src=\"/images/link.gif\"><a target=\"f3\" href=\"bbsadl\">���ؾ�����</a><br>\n"); */
    printf("</div>\n");
    printmenuend();
    printf("<div class=\"r\">");
    /*printf("<img src=\"/images/link0.gif\"><a href=\"bbsfind\" target=\"f3\">���²�ѯ</a><br>\n"); */
    printmenu("<img src=\"/images/find.gif\"><a href=\"bbssel\" target=\"f3\">����������</a><br>\n");
    printmenu("<img src=\"/images/find.gif\"><a href=\"/games/index.html\" target=\"f3\">����������</a><br>\n");
    printmenubegin();
	printf("<table><tr><form action=/bbsdoc.php target=\"f3\"><td>&nbsp;&nbsp;"
			"<input type=text name=board maxlength=20 "
			"size=9 value=ѡ�������� onclick=\"this.select()\"></td></form></tr></table>\n");
    printf("<img src=\"/images/telnet.gif\"><a href=\"telnet:%s\">Telnet��¼</a><br>\n", NAME_BBS_ENGLISH);
    printmenuend();
#ifdef SMTH
    printmenu("<img src=\"/images/telnet.gif\"><a href=\"/data/fterm-smth.zip\">fterm����</a><br>\n");
#endif
    /*if(!loginok) 
       printf("<img src=\"/images/link0.gif\"><a href=\"javascript:void open('bbsreg', '', 'width=620,height=550')\">���û�ע��</a><br>\n"); */
    if (loginok) {
        if (HAS_PERM(currentuser, PERM_LOGINOK) && !HAS_PERM(currentuser, PERM_POST))
            printmenu("<script>alert('���������ȫվ�������µ�Ȩ��, ��ο�Announce�湫��, ��������sysop��������. ��������, ����Complain���������.')</script>\n");
        if (count_new_mails() > 0)
            printf("<script>alert('�������ż�!')</script>\n");
    }
    /*if(loginok && !(currentuser->userlevel & PERM_LOGINOK) && !has_fill_form()) 
       printf("<a target=\"f3\" href=\"bbsform\">��дע�ᵥ</a><br>\n"); */
    if (loginok && can_enter_chatroom())
        printmenu("<br><a href=\"javascript:openchat()\">[" CHAT_SERVER "]</a>");
    printf("<script>if(isNS4) arrange();if(isOP)alarrangeO();</script></p>");
    printf("</table></td></tr>");
#ifdef SMTH
    printf("<tr><td>");
    printf("<TABLE cellSpacing=0 cellPadding=0 width=\"90%\" border=0>");
    printf("<tr><td>\n");
    printf("<p>ȫ���������������ھ��У���������ߴ�20��Ԫ������һ�Ƚ�����ߴ�10��Ԫ���������</p>");
    printf("</tr></td>\n");
    printf("<tr><td>\n");
    printf("<a href=\"http://www.hpc-contest.edu.cn\"  target=\"_blank\">http://www.hpc-contest.edu.cn</a><br></p>");
    printf("</tr></td></table>\n");
    printf("</td></tr>");
#endif
    printf("</table>");
    printf("</body>");
}

int count_new_mails()
{
    struct fileheader x1;
    int n, unread = 0;
    char buf[1024];
    FILE *fp;

    if (!loginok && currentuser->userid[0] == 0)
        return 0;
    sprintf(buf, "%s/mail/%c/%s/.DIR", BBSHOME, toupper(currentuser->userid[0]), currentuser->userid);
    fp = fopen(buf, "r");
    if (fp == 0)
        return unread;
    while (fread(&x1, sizeof(x1), 1, fp) > 0)
        if (!(x1.accessed[0] & FILE_READ))
            unread++;
    fclose(fp);
    return unread;
}
