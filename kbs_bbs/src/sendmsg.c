#include "bbs.h"
#ifdef lint
#include <sys/uio.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "screen.h"
#define MAXMESSAGE 5

int RMSG = false;
extern int RUNSH;
extern struct screenline *big_picture;
extern char MsgDesUid[14];      /* ��������msg��Ŀ��uid 1998.7.5 by dong */
long f_offset = 0;
static int RMSGCount = 0;       /* Leeward 98.07.30 supporting msgX */

int get_msg(uid, msg, line)
char *msg, *uid;
int line;
{
    char genbuf[3];

    move(line, 0);
    clrtoeol();
    prints("�����Ÿ���%s", uid);
    memset(msg, 0, sizeof(msg));
    while (1) {
        getdata(line + 1, 0, "���� : ", msg, 59, DOECHO, NULL, false);
        if (msg[0] == '\0')
            return false;
        getdata(line + 2, 0, "ȷ��Ҫ�ͳ���(Y)�ǵ� (N)��Ҫ (E)�ٱ༭? [Y]: ", genbuf, 2, DOECHO, NULL, 1);
        if (genbuf[0] == 'e' || genbuf[0] == 'E')
            continue;
        if (genbuf[0] == 'n' || genbuf[0] == 'N')
            return false;
        if (genbuf[0] == 'G') {
            if (HAS_PERM(currentuser, PERM_SYSOP))
                return 2;
            else
                return true;
        } else
            return true;
    }
}

int s_msg()
{
    return do_sendmsg(NULL, NULL, 0);
}
extern char msgerr[255];

int do_sendmsg(uentp, msgstr, mode)
struct user_info *uentp;
const char msgstr[256];
int mode;
{
    char uident[STRLEN];
    struct user_info *uin;
    char buf[80], msgbak[256];
    int Gmode = 0;
    int result;

    *msgbak = 0;                /* period 2000-11-20 may be used without init */
    if ((mode == 0) || (mode == 3)) {
        modify_user_mode(MSG);
        move(2, 0);
        clrtobot();
    }
    if (uentp == NULL) {
        prints("<����ʹ���ߴ���>\n");
        move(1, 0);
        clrtoeol();
        prints("��ѶϢ��: ");
        creat_list();
        namecomplete(NULL, uident);
        if (uident[0] == '\0') {
            clear();
            return 0;
        }
        uin = t_search(uident, false);
        if (uin == NULL) {
            move(2, 0);
            prints("�Է�Ŀǰ�������ϣ�����ʹ���ߴ����������...\n");
            pressreturn();
            move(2, 0);
            clrtoeol();
            return -1;
        }
        if (!canmsg(currentuser, uin)) {
            move(2, 0);
            prints("�Է��Ѿ��رս���ѶϢ�ĺ�����...\n");
            pressreturn();
            move(2, 0);
            clrtoeol();
            return -1;
        }
        /*
         * ��������msg��Ŀ��uid 1998.7.5 by dong 
         */
        strcpy(MsgDesUid, uident);
        /*
         * uentp = uin; 
         */

    } else {
        /*
         * if(!strcasecmp(uentp->userid,currentuser->userid))  rem by Haohmaru,�����ſ����Լ����Լ���msg
         * return 0;    
         */ uin = uentp;
        strcpy(uident, uin->userid);
        /*
         * strcpy(MsgDesUid, uin->userid); change by KCN,is wrong 
         */
    }

    /*
     * try to send the msg 
     */
    result = sendmsgfunc(uin, msgstr, mode);

    switch (result) {
    case 1:                    /* success */
        if (mode == 0) {
            prints("\n���ͳ�ѶϢ....\n");
            pressreturn();
            clear();
        }
        return 1;
        break;
    case -1:                   /* failed, reason in msgerr */
        if (mode == 0) {
            move(2, 0);
            prints(msgerr);
            pressreturn();
            move(2, 0);
            clrtoeol();
        }
        return -1;
        break;
    case 0:                    /* message presending test ok, get the message and resend */
        if (mode == 4)
            return 0;
        Gmode = get_msg(uident, buf, 1);
        if (!Gmode) {
            move(1, 0);
            clrtoeol();
            move(2, 0);
            clrtoeol();
            return 0;
        }
        break;
    default:                   /* unknown reason */
        return result;
        break;
    }
    /*
     * resend the message 
     */
    result = sendmsgfunc(uin, buf, mode);

    switch (result) {
    case 1:                    /* success */
        if (mode == 0) {
            prints("\n���ͳ�ѶϢ....\n");
            pressreturn();
            clear();
        }
        return 1;
        break;
    case -1:                   /* failed, reason in msgerr */
        if (mode == 0) {
            move(2, 0);
            prints(msgerr);
            pressreturn();
            move(2, 0);
            clrtoeol();
        }
        return -1;
        break;
    default:                   /* unknown reason */
        return result;
        break;
    }
    return 1;
}



int show_allmsgs()
{
    char fname[STRLEN];
    int oldmode;

    sethomefile(fname, currentuser->userid, "msgfile");
    clear();
    oldmode = uinfo.mode;
    modify_user_mode(LOOKMSGS);
    if (dashf(fname)) {
        ansimore(fname, true);
        clear();
    } else {
        move(5, 30);
        prints("û���κε�ѶϢ���ڣ���");
        pressanykey();
        clear();
    }
    uinfo.mode = oldmode;
    return 0;
}


int dowall(struct user_info *uin, char *buf2)
{
    if (!uin->active || !uin->pid)
        return -1;
    /*---	������ǰ���ڷ���Ϣ��	period	2000-11-13	---*/
    if (getpid() == uin->pid)
        return -1;

    move(1, 0);
    clrtoeol();
    prints("[32m���� %s �㲥.... Ctrl-D ֹͣ�Դ�λ User �㲥��[m", uin->userid);
    refresh();
    if (strcmp(uin->userid, "guest")) { /* Leeward 98.06.19 */
        /*
         * ��������msg��Ŀ��uid 1998.7.5 by dong 
         */
        strcpy(MsgDesUid, uin->userid);

        do_sendmsg(uin, buf2, 3);       /* �㲥ʱ���ⱻ����� guest ��� */
    }
    return 0;
}


int wall()
{
    char buf2[STRLEN];

    if (check_systempasswd() == false)
        return 0;
    modify_user_mode(MSG);
    move(2, 0);
    clrtobot();
    if (!get_msg("����ʹ����", buf2, 1)) {
        move(1, 0);
        clrtoeol();
        move(2, 0);
        clrtoeol();
        return 0;
    }
    if (apply_ulist((APPLY_UTMP_FUNC) dowall, buf2) == -1) {
        move(2, 0);
        prints("û���κ�ʹ��������\n");
        pressanykey();
    }
    sprintf(genbuf, "%s �㲥:%s", currentuser->userid, buf2);
    securityreport(genbuf, NULL, NULL);
    prints("\n�Ѿ��㲥���....\n");
    pressanykey();
    return 0;
}

int msg_count;

void r_msg_sig(int signo)
{
    msg_count++;
    signal(SIGUSR2, r_msg_sig);
}

void r_msg()
{
    int y, x, ch, i, ox, oy, tmpansi;
    char savebuffer[24][256];
    char buf[MAX_MSG_SIZE+100], outmsg[MAX_MSG_SIZE*2], buf2[STRLEN];
    int now, count;

    getyx(&y, &x);
    tmpansi = showansi;
    showansi = 1;
    for(i=0;i<=24;i++)
        saveline(i, 0, savebuffer[i]);

    if ((uinfo.mode == POSTING || uinfo.mode == SMAIL) && !DEFINE(currentuser, DEF_LOGININFORM)) {      /*Haohmaru.99.12.16.������ʱ����msg */
        move(0, 0);
        clrtoeol();
        refresh();
        if (get_unreadcount(currentuser->userid)) {
            prints("[1m[33m�����µ�ѶϢ���뷢�������º� Ctrl+Z ��ѶϢ[m");
            move(y, x);
            refresh();
            sleep(1);
        } else {
            prints("[1mû���κ��µ�ѶϢ����![m");
            move(y, x);
            refresh();
            sleep(1);
        }
        saveline(0, 1, savebuffer[0]);
        return;
    }
    count = get_msgcount(currentuser->userid);
    if (!count) {        /* Leeward 98.07.30 */
        move(0, 0);
        clrtoeol();
        refresh();
        prints("[1mû���κε�ѶϢ���ڣ���[m");
        move(y, x);
        refresh();
        sleep(1);
        saveline(0, 1, savebuffer[0]);  /* restore line */
        return;
    }

    now = get_unreadmsg(currentuser->userid);
    if(now==-1) now = get_msgcount(currentuser->userid)-1;
    do {
        load_msgtext(currentuser->userid, now, buf);
        translate_msg(buf, outmsg);
        move(0,0);
        if (DEFINE(currentuser, DEF_SOUNDMSG))
            bell();
        if (DEFINE(currentuser, DEF_HIGHCOLOR))
            prints("\x1b[1m%s", outmsg);
        else
            prints("%s", outmsg);
        getyx(&oy, &ox);

        prints("  ��%-3.3d/%3.3d����Ϣ, R�ظ�", count, now);
        clrtoeol();
        
        refresh();
        oflush();
        ch = igetkey();
        for(i=0;i<oy;i++)
            saveline(i, 1, savebuffer[i]);

        now = get_unreadmsg(currentuser->userid);
    } while(now!=-1);

    showansi = tmpansi;
}

void r_lastmsg()
{
    f_offset = 0;
    r_msg();
}

int myfriend_wall(struct user_info *uin, char *buf, int i)
{
    if ((uin->pid - uinfo.pid == 0) || !uin->active || !uin->pid || !canmsg(currentuser, uin))
        return -1;
    if (myfriend(uin->uid, NULL)) {
        move(1, 0);
        clrtoeol();
        prints("\x1b[1;32m������ѶϢ�� %s...  \x1b[m", uin->userid);
        refresh();
        strcpy(MsgDesUid, uin->userid);
        do_sendmsg(uin, buf, 5);
    }
    return 0;
}

int friend_wall()
{
    char buf[80];

    if (uinfo.invisible) {
        move(2, 0);
        prints("��Ǹ, �˹���������״̬�²���ִ��...\n");
        pressreturn();
        return 0;
    }
    modify_user_mode(MSG);
    move(2, 0);
    clrtobot();
    if (!get_msg("�ҵĺ�����", buf, 1))
        return 0;
    if (apply_ulist(myfriend_wall, buf) == -1) {
        move(2, 0);
        prints("���Ͽ���һ��\n");
        pressanykey();
    }
    move(6, 0);
    prints("ѶϢ�������...");
    pressanykey();
    return 1;
}
