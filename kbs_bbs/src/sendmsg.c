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
        multi_getdata(line + 1, 0, 78, "���� : ", msg, MAX_MSG_SIZE, false);
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
const char msgstr[MAX_MSG_SIZE];
int mode;
{
    char uident[STRLEN];
    struct user_info *uin;
    char buf[MAX_MSG_SIZE];
    int Gmode = 0;
    int result;

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
    char buf[MAX_MSG_SIZE], showmsg[MAX_MSG_SIZE*2], chk[STRLEN];
    int oldmode, count, i, j, page, ch, y, all=0, reload=0;
    char title[STRLEN];
    struct msghead head;
    time_t now;
    char fname[STRLEN];
    FILE* fn;


    oldmode = uinfo.mode;
    modify_user_mode(LOOKMSGS);

    page = 0;
    count = get_msgcount(0, currentuser->userid);
    while(1) {
        if(reload) {
            reload = 0;
            page = 0;
            count = get_msgcount(all?2:0, currentuser->userid);
        }
        clear();
        if(count==0) {
            good_move(5,30);
            prints("û���κε�ѶϢ���ڣ���");
        }
        else {
            y = 0;
            i = page;
            load_msghead(all?2:0, currentuser->userid, i, &head);
            load_msgtext(currentuser->userid, &head, buf);
            j = translate_msg(buf, &head, showmsg);
            while(y+j<=23) {
                y+=j; i++;
                prints("%s", showmsg);
                if(i>=count) break;
                load_msghead(all?2:0, currentuser->userid, i, &head);
                load_msgtext(currentuser->userid, &head, buf);
                j = translate_msg(buf, &head, showmsg);
            }
        }
        good_move(23,0);
        if(!all)
            prints("[1;44;32mѶϢ�����   ����<[37mr[32m> ���<[37mc[32m> �Ļ�����<[37mm[32m> ��Ѷ��<[37mi[32m> ѶϢ����<[37ms[32m>                 ");
        else
            prints("[1;44;32mѶϢ�����   ����<[37mr[32m> ���<[37mc[32m> �Ļ�����<[37mm[32m> ��Ѷ��<[37mi[32m> ѶϢ����<[37ms[32m> ȫ��<[37ma[32m>         ");
        refresh();
        oflush();
reenter:
        ch = igetkey();
        switch(ch) {
            case 'r':
            case 'R':
            case 'q':
            case 'Q':
            case KEY_LEFT:
                goto outofhere;
            case KEY_UP:
                if(page>0) page--;
                break;
            case KEY_DOWN:
                if(page<count-1) page++;
                break;
            case KEY_PGDN:
            case ' ':
                if(page<count-11) page+=10;
                else page=count-1;
                break;
            case KEY_PGUP:
                if(page>10) page-=10;
                else page=0;
                break;
            case KEY_HOME:
                page=0;
                break;
            case KEY_END:
                page=count-1;
                break;
            case 'i':
            case 'I':
            case 's':
            case 'S':
                reload = 1;
                count = get_msgcount(0, currentuser->userid);
                if(count==0) break;
                good_move(23, 0);
                clrtoeol();
                getdata(23, 0, "������ؼ���:", chk, 50, true, NULL, true);
                if(chk[0]) {
                    int fd, fd2;
                    char fname[STRLEN], fname2[STRLEN];
                    size_t bm_search[256];
                    struct msghead head;
                    int i, j;
                    bool init=false;
                    sethomefile(fname, currentuser->userid, "msgindex");
                    sethomefile(fname2, currentuser->userid, "msgindex3");
                    fd = open(fname, O_RDONLY, 0644);
                    fd2 = open(fname2, O_WRONLY|O_CREAT, 0644);
                    write(fd2, &i, 4);
                    lseek(fd, 4, SEEK_SET);
                    for(i=0;i<count;i++) {
                        read(fd, &head, sizeof(struct msghead));
                        if(toupper(ch)=='S') load_msgtext(currentuser->userid, &head, buf);
                        if(toupper(ch)=='I'&&!strncasecmp(chk, head.id, IDLEN) ||
                            toupper(ch)=='S'&&bm_strcasestr_rp(buf, chk, bm_search, &init) != NULL)
                            write(fd2, &head, sizeof(struct msghead));
                    }
                    close(fd2);
                    close(fd);
                    all = 1;
                }
                break;
            case 'c':
            case 'C':
                clear_msg(currentuser->userid);
                goto outofhere;
            case 'a':
            case 'A':
                if(all) {
                    sethomefile(buf, currentuser->userid, "msgindex3");
                    unlink(buf);
                    all = 0;
                    reload = 1;
                }
                break;
            case 'm':
            case 'M':
                sprintf(fname, "tmp/%s.msg", currentuser->userid);
                fn = fopen(fname, "w");
                count = get_msgcount(0, currentuser->userid);
                for(i=0;i<count;i++) {
                    load_msgtext(0, currentuser->userid, i, buf);
                    translate_msg(buf, showmsg);
                    fprintf(fn, "%s", showmsg);
                }
                fclose(fn);

                now = time(0);
                sprintf(title, "[%12.12s] ����ѶϢ����", ctime(&now) + 4);
                mail_file(currentuser->userid, fname, currentuser->userid, title, BBSPOST_MOVE);
                unlink(fname);
                clear_msg(currentuser->userid);
                goto outofhere;
            default:
                goto reenter;
        }
    }
outofhere:
    
    if(all) {
        sethomefile(buf, currentuser->userid, "msgindex3");
        unlink(buf);
    }
    clear();
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
    int y, x, ch, i, ox, oy, tmpansi, pid;
    char savebuffer[25][256];
    char buf[MAX_MSG_SIZE+100], outmsg[MAX_MSG_SIZE*2], buf2[STRLEN], uid[14];
    struct user_info * uin;
    struct msghead head;
    int now, count, canreply, first=1;

    good_getyx(&y, &x);
    tmpansi = showansi;
    showansi = 1;
    RMSG = true;
    RMSGCount++;
    for(i=0;i<=24;i++)
        saveline(i, 0, savebuffer[i]);

    if ((uinfo.mode == POSTING || uinfo.mode == SMAIL) && !DEFINE(currentuser, DEF_LOGININFORM)) {      /*Haohmaru.99.12.16.������ʱ����msg */
        good_move(0, 0);
        clrtoeol();
        refresh();
        if (get_unreadcount(currentuser->userid)) {
            prints("[1m[33m�����µ�ѶϢ���뷢�������º� Ctrl+Z ��ѶϢ[m");
            good_move(y, x);
            refresh();
            sleep(1);
        } else {
            prints("[1mû���κ��µ�ѶϢ����![m");
            good_move(y, x);
            refresh();
            sleep(1);
        }
        saveline(0, 1, savebuffer[0]);
        goto outhere;
    }
    count = get_msgcount(1, currentuser->userid);
    if (!count) {
        good_move(0, 0);
        clrtoeol();
        refresh();
        prints("[1mû���κε�ѶϢ���ڣ���[m");
        good_move(y, x);
        refresh();
        sleep(1);
        saveline(0, 1, savebuffer[0]);  /* restore line */
        goto outhere;
    }

    now = get_unreadmsg(currentuser->userid);
    if(now==-1) now = count-1;
    else {
        load_msghead(1, currentuser->userid, now, &head);
        while(head.topid!=getuinfopid()&&now<count-1){
            now = get_unreadmsg(currentuser->userid);
            load_msghead(1, currentuser->userid, now, &head);
        };
    }
    while(1){
        load_msghead(1, currentuser->userid, now, &head);
        load_msgtext(currentuser->userid, &head, buf);
        translate_msg(buf, &head, outmsg);
        strncpy(uid, head.id, IDLEN);
        pid = head.frompid;
        uin = t_search(uid, pid);
        if(head.mode==3||uin==NULL) canreply = 0;
        else canreply = 1;
        
        if (DEFINE(currentuser, DEF_SOUNDMSG))
            bell();
        good_move(0,0);
        if (DEFINE(currentuser, DEF_HIGHCOLOR))
            prints("\x1b[1m%s", outmsg);
        else
            prints("%s", outmsg);

        if(first) {
            refresh();
            oflush();
            ch = igetkey();
            first = 0;
        }
        

        clrtoeol();
        if(canreply)
            prints("[m %3d/%-3d, �ظ�%s:", now+1, count, uid);
        else
            if(uin)
                prints("[m %3d/%-3d,�����л�,Enter����, ����Ϣ�޷��ظ�", now+1, count);
            else
                prints("[m %3d/%-3d,�����л�,Enter����, �û�%s����վ,�޷��ظ�", now+1, count, uid);
        good_getyx(&oy, &ox);
        
        if(canreply)
            ch = -multi_getdata(oy, ox, 78, NULL, buf, 1024, true);
        else {
            refresh();
            oflush();
            do {
                ch = igetkey();
            } while(ch!=KEY_UP&&ch!=KEY_DOWN&&ch!='\r'&&ch!='\n');
        }
        for(i=0;i<=oy;i++)
            norefresh_saveline(i, 1, savebuffer[i]);
        switch(ch) {
            case KEY_UP:
                now--;
                if(now<0) now=count-1;
                break;
            case KEY_DOWN:
                now++;
                if(now>=count) now=0;
                break;
            default:
                if(canreply) {
                    if(buf[0]) {
                        strcpy(MsgDesUid, uid);
                        i = do_sendmsg(uin, buf, 4);
                        buf[0]=0;
                        if(i==1) strcpy(buf, "[1m�����ͳ�ѶϢ��[m");
                        else if(i==-2) strcpy(buf, "[1m�Է��Ѿ�������...[m");
                        if(buf[0]) {
                            good_move(0,0);
                            clrtoeol();
                            prints("%s", buf);
                            refresh();
#ifdef NINE_BUILD
                            if(i!=1)
#endif
                            sleep(1);
                        }
                    }
                    ch = '\n';
                }
                break;
        }
        if (ch=='\r'||ch=='\n') break;
    }


outhere:
    for(i=0;i<=24;i++)
        norefresh_saveline(i, 1, savebuffer[i]);
    showansi = tmpansi;
    good_move(y,x);
    refresh();
    RMSGCount--;
    if (0 == RMSGCount)
        RMSG = false;
    return;
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
