/*
 * $Id$
 */
#define MY_CSS "/bbsold.css"
#include "bbslib.h"

void printdiv(int n, char *str, char *imgurl)
{
	printf("<div id=\"div%da\" class=\"r\"><img id=\"img%d\" src=\"%s\">",
		   n, n, imgurl);
	printf("<A href=\"javascript:changemn('%d');\">%s</A></div>\n", n, str);
	printf("<div id=\"div%d\" class=\"s\">\n", n);
}

int main()
{
	char buf[1024], *ptr;
	int i;

  	init_all();
	printf("<style type=\"text/css\">A {color: #000080}</style><br>\n");
	printf("<script src=\"/func.js\"></script>"
	"<body class=\"dark\" leftmargin=\"5\" topmargin=\"1\" MARGINHEIGHT=\"1\" MARGINWIDTH=\"1\">");
	printf("<table width=\"100%%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
	printf("<tr><td>");
	/*if(!loginok)
	{
		printf( "<form action=\"/bbslogin.jsp\" method=\"post\" target=\"_top\"><br>\n"
NAME_BBS_CHINESE "�û���¼<br>\n"
"�ʺ� <input class=\"default\" type=\"text\" name=\"id\" maxlength=\"12\" size=\"8\"><br>\n"
"���� <input class=\"default\" type=\"password\" name=\"passwd\" maxlength=\"39\" size=\"8\"><br>\n"
"<input class=\"button\" type=\"submit\" value=\"��¼��վ\">\n"
"</form>\n");
	}
	else*/
	{
        char buf[256]="δע���û�";
		if (currentuser==NULL)
			http_fatal("hehe:%d",loginok);
		printf("�û�: <a href=\"bbsqry?userid=%s\" target=\"f3\">%s</a><br>", 
				getcurruserid(), getcurruserid());
		uleveltochar(buf, getcurrusr());
        printf("���: %s<br>\n", buf);
        printf("<a href=\"/bbslogout.jsp\" target=\"_top\">ע�����ε�¼</a><br>\n");
	}
	printf("</td></tr><tr><td>\n");
	printf("<hr style=\"color:2020f0; height=1px\" width=\"84px\" align=\"center\">\n");
	printf("</td></tr></table>");
  	printf("<img src=\"/images/home.gif\"><a href=\"bbssec\" target=\"f3\">��������ҳ</a><br>\n");
        printf("<img src=\"/images/link0.gif\"><a target=\"f3\" href=\"bbs0an\">����������</a><br>\n");
   	printf("<img src=\"/images/link0.gif\"><a target=\"f3\" href=\"bbstop10\">����ʮ��</a><br>\n");
	/*printf("<img src=\"/images/link0.gif\"><a target=\"f3\" href=\"bbstopb10\">����������</a><br>\n");*/
	if(loginok)
	{
		int i, mybrdnum=0;
		const struct boardheader  *bptr;
		printdiv(1,"���˶�����","/images/folder.gif");
		load_favboard(0);
		mybrdnum = get_favboard_count();
 		for(i=0; i<mybrdnum; i++)
		{
			bptr = getboard(get_favboard(i+1)+1);
			if (bptr == NULL)
				continue;
     		printf("<img src=\"/images/link.gif\">"
					" <a target=\"f3\" href=\"bbsdoc?board=%s\">%s</a><br>\n",
					bptr->filename, bptr->filename);
		}
		printf("<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsmybrd\">"
				"Ԥ������</a><br>\n");
		printf("</div>\n");
	}
	printdiv(2,"����������","/images/folder.gif");
	for (i = 0; i < SECNUM; i++)
		printf("<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsboa?group=%d\">%s</a><br>\n",
				i, secname[i][0]);
	printf("</div>\n");

	printdiv(3,"̸��˵����","/images/folder.gif");
  	if(loginok) {
		printf("<img src=\"/images/link.gif\"> <a href=\"bbsfriend\" target=\"f3\">���ߺ���</a><br>\n");
	}
  	printf("<img src=\"/images/link.gif\"> <a href=\"bbsusr\" target=\"f3\">�����ķ�</a><br>\n");
  	printf("<img src=\"/images/link.gif\"> <a href=\"bbsqry\" target=\"f3\">��ѯ����</a><br>\n");
        if(loginok&&currentuser->userlevel & PERM_PAGE) {
                printf("<img src=\"/images/link.gif\"> <a href=\"bbssendmsg\" target=\"f3\">����ѶϢ</a><br>\n");
                printf("<img src=\"/images/link.gif\"> <a href=\"bbsmsg\" target=\"f3\">�鿴����ѶϢ</a><br>\n");
	}
	printf("</div>\n");
	ptr="";
	if(loginok&&currentuser->userlevel & PERM_CLOAK)
		ptr="<img src=\"/images/link.gif\"> <a target=\"f3\" onclick=\"return confirm('ȷʵ�л�����״̬��?')\" href=\"bbscloak\">�л�����</a><br>\n";
	if(loginok)
	{
		printdiv(4,"���˹�����","/images/folder.gif");
			printf(
"			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsinfo\">��������</a><br>\n"
"			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsplan\">��˵����<a><br>\n"
"			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbssig\">��ǩ����<a><br>\n"
"			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbspwd\">�޸�����</a><br>\n"
"			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsparm\">�޸ĸ��˲���</a><br>\n"
/*
"			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsmywww\">WWW���˶���</a><br>\n"
*/
"			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsnick\">��ʱ���ǳ�</a><br>\n"
/*"		<img src=\"/images/link.gif\"><a target=\"f3\" href=\"bbsstat\"> ����ͳ��</a><br>"*/
"			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsfall\">�趨����</a><br>\n"
"			%s</div>",ptr);
		printdiv(5,"�����ż���","/images/folder.gif");
		printf("			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsnewmail\">�������ʼ�</a><br>\n"
"			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsmail\">�����ʼ�</a><br>\n");
		if (can_send_mail())
			printf("			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbspstmail\">�����ʼ�</a><br>\n");
		printf("			</div>\n");
	}

	/*printdiv(6,"�ر������","/images/folder.gif");
	printf("<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsalluser\">����ʹ����</a><br>\n");*/
	/*printf("<img src=\"/images/link.gif\"><a target=\"f3\" href=\"bbsadl\">���ؾ�����</a><br>\n");*/
	/*printf("</div>\n");*/
	printf("<div class=\"r\">");
  	/*printf("<img src=\"/images/link0.gif\"><a href=\"bbsfind\" target=\"f3\">���²�ѯ</a><br>\n");*/
	printf("<img src=\"/images/find.gif\"><a href=\"bbssel\" target=\"f3\">����������</a><br>\n");
	printf("<img src=\"/images/telnet.gif\"><a href=\"telnet:%s\">Telnet��¼</a><br>\n", NAME_BBS_ENGLISH);
	/*if(!loginok) 
		printf("<img src=\"/images/link0.gif\"><a href=\"javascript:void open('bbsreg', '', 'width=620,height=550')\">���û�ע��</a><br>\n");*/
   	if(loginok) {
		if(HAS_PERM(currentuser,PERM_LOGINOK) && !HAS_PERM(currentuser,PERM_POST))
			printf("<script>alert('���������ȫվ�������µ�Ȩ��, ��ο�Announce�湫��, ��������sysop��������. ��������, ����Complain���������.')</script>\n");
		if(count_new_mails()>0) 
			printf("<script>alert('�������ż�!')</script>\n");
	}
	/*if(loginok && !(currentuser->userlevel & PERM_LOGINOK) && !has_fill_form()) 
		printf("<a target=\"f3\" href=\"bbsform\">��дע�ᵥ</a><br>\n");*/
	if(loginok && can_enter_chatroom())
		printf("<br><a href=\"javascript:openchat()\">["CHAT_SERVER"<font color=\"red\">������</font>]</a>");
	printf("</div><script>if(isNS4) arrange();if(isOP)alarrangeO();</script>");
  	printf("</body>");
}

int count_new_mails() {
        struct fileheader x1;
        int n, unread=0;
	char buf[1024];
        FILE *fp;
        if(!loginok&&currentuser->userid[0]==0) return 0;
        sprintf(buf, "%s/mail/%c/%s/.DIR", BBSHOME, toupper(currentuser->userid[0]), currentuser->userid);
        fp=fopen(buf, "r");
        if(fp==0)
			return unread;
        while(fread(&x1, sizeof(x1), 1, fp)>0)
                if(!(x1.accessed[0] & FILE_READ)) unread++;
        fclose(fp);
	return unread;
}
