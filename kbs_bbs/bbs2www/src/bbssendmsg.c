/*
 * $Id$
 */
#include "bbslib.h"

int msg_can_sendmsg(char *userid, int utmpnum)
{
    struct userec *x;
    uinfo_t *uin;

    if (getuser(userid, &x) == 0)
        return 0;
    /*http_fatal("���޴���"); */
    if (strcmp(x->userid, "guest") && !HAS_PERM(currentuser, PERM_PAGE))
        return 0;
    if (utmpnum == 0)
        uin = t_search(userid, utmpnum);
    else
        uin = get_utmpent(utmpnum);
    if (uin == NULL)
        return 0;
    if (strcasecmp(uin->userid, userid))
        return 0;
    if (!canmsg(currentuser, uin))
        return 0;

    return 1;
}

extern char msgerr[255];
int main()
{
    int i;
    int mode, destutmp = 0;
    char destid[20], msg[256];

    init_all();
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ��ܷ�ѶϢ, ���ȵ�¼��");
    strsncpy(destid, getparm("destid"), 13);
    strsncpy(msg, getparm("msg"), 51);
    destutmp = atoi(getparm("destutmp"));
    if (destid[0] == 0 || msg[0] == 0) {
        char buf3[256];

        strcpy(buf3, "<body onload=\"document.form0.msg.focus()\">");
        if (destid[0] == 0)
            strcpy(buf3, "<body onload=\"document.form0.destid.focus()\">");
        printf("%s\n", buf3);
        printf("	<form name=\"form0\" action=\"bbssendmsg\" method=\"post\">"
               "		  <input type=\"hidden\" name=\"destutmp\" value=\"%d\">"
               "	��ѶϢ��: <input name=\"destid\" maxlength=\"12\" value=\"%s\" size=\"12\"><br>"
               "	ѶϢ����: <input name=\"msg\" maxlength=\"50\" size=\"50\" value=\"%s\"><br>"
               "		  <input type=\"submit\" value=\"ȷ��\" width=\"6\">" "	</form> ", destutmp, destid, msg);
        http_quit();
    }
    if (!msg_can_sendmsg(destid, destutmp))
        http_fatal("�޷�������Ϣ");
    /*if (destpid > 100)
       destpid -= 100; */
    printf("<body onload=\"document.form1.b1.focus()\">\n");
    if (!strcasecmp(destid, currentuser->userid))
        printf("�㲻�ܸ��Լ���ѶϢ��");
    else {
        int result;

        /*struct userec *x;

           if(getuser(destid, &x) > 0)
           strsncpy(destid, x->userid, sizeof(destid)); */
        for (i = 0; i < strlen(msg); i++) {
            if (msg[i] <= 27 && msg[i] >= -1)
                msg[i] = ' ';
        }
        if ((result = send_msg(getcurruserid(), get_utmpent_num(u_info), destid, destutmp, msg)) == 1)
            printf("�Ѿ������ͳ���Ϣ");
        else if (result == -1)
            printf("������Ϣʧ�ܣ�%s", msgerr);
        else
            printf("������Ϣʧ��, ����Ŀǰ�����߻����޷�������Ϣ");
    }
    printf("<script>top.fmsg.location=\"bbsgetmsg\"</script>\n");
    printf("<br><form name=\"form1\"><input name=\"b1\" type=\"button\" onclick=\"history.go(-2)\" value=\"[����]\">");
    printf("</form>");
    http_quit();

    return 0;
}
