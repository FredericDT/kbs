/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "bbs.h"
#define         INTERNET_PRIVATE_EMAIL

/*For read.c*/
int auth_search_down();
int auth_search_up();
int do_cross();
int t_search_down();
int t_search_up();
int thread_up();
int thread_down();
int deny_user();
int show_author();
int show_authorinfo();          /*Haohmaru.98.12.19 */
int show_authorBM();            /*cityhunter 00.10.18 */
int SR_first_new();
int SR_last();
int SR_first();
int SR_read();
int SR_readX();                 /* Leeward 98.10.03 */
int SR_author();
int SR_authorX();               /* Leeward 98.10.03 */
int G_SENDMODE = false;
extern int add_author_friend();
int cmpinames();                /* added by Leeward 98.04.10 */

extern int nf;
extern int numofsig;
extern char quote_user[];
char *sysconf_str();
char currmaildir[STRLEN];
extern char mail_list[MAILBOARDNUM][40];
extern int mail_list_t;

#define maxrecp 300

static int mail_reply(int ent, struct fileheader *fileinfo, char *direct);
static int mail_del(int ent, struct fileheader *fileinfo, char *direct);
static int do_gsend(char *userid[], char *title, int num);

int chkmail()
{
    static time_t lasttime = 0;
    static int ismail = 0;
    struct fileheader fh;
    struct stat st;
    int fd;
    int i, offset;
    long numfiles;
    unsigned char ch;
    extern char currmaildir[STRLEN];

    m_init();
    if (!HAS_PERM(currentuser, PERM_BASIC)) {
        return 0;
    }
    /*
     * ylsdd 2001.4.23: ����ļ�״̬Ӧ����get_mailnum��get_sum_records֮ǰ����������
     * Ҫ���������õ�ϵͳ����. ������Ķ���Ҳ��fstat��Ϊstat�ˣ���ʡһ��open&close 
     */
    if (stat(currmaildir, &st) < 0)
        return (ismail = 0);
    if (lasttime >= st.st_mtime)
        return ismail;


    if (chkusermail(currentuser))
        return (ismail = 2);
    offset = (int) ((char *) &(fh.accessed[0]) - (char *) &(fh));
    if ((fd = open(currmaildir, O_RDONLY)) < 0)
        return (ismail = 0);
    lasttime = st.st_mtime;
    numfiles = st.st_size;
    numfiles = numfiles / sizeof(fh);
    if (numfiles <= 0) {
        close(fd);
        return (ismail = 0);
    }
    lseek(fd, (st.st_size - (sizeof(fh) - offset)), SEEK_SET);
    for (i = 0; i < numfiles; i++) {
        read(fd, &ch, 1);
        if (!(ch & FILE_READ)) {
            close(fd);
            return (ismail = 1);
        }
        lseek(fd, -sizeof(fh) - 1, SEEK_CUR);
    }
    close(fd);
    return (ismail = 0);
}

int get_mailnum()
{
    struct fileheader fh;
    struct stat st;
    int fd;
    register int numfiles;
    extern char currmaildir[STRLEN];

    if ((fd = open(currmaildir, O_RDONLY)) < 0)
        return 0;
    fstat(fd, &st);
    numfiles = st.st_size;
    numfiles = numfiles / sizeof(fh);
    close(fd);
    return numfiles;
}


static int mailto(struct userec *uentp, char *arg)
{
    char filename[STRLEN];
    int mailmode = (int) arg;

    sprintf(filename, "etc/%s.mailtoall", currentuser->userid);
    if ((uentp->userlevel == PERM_BASIC && mailmode == 1) ||
        (uentp->userlevel & PERM_POST && mailmode == 2) || (uentp->userlevel & PERM_BOARDS && mailmode == 3) || (uentp->userlevel & PERM_CHATCLOAK && mailmode == 4)) {
        mail_file(currentuser->userid, filename, uentp->userid, save_title, 0);
    }
    return 1;
}

static int mailtoall(int mode)
{

    return apply_users(mailto, (char *) mode);
}

int mailall()
{
    char ans[4], ans4[4], ans2[4], fname[STRLEN], title[STRLEN];
    char doc[4][STRLEN], buf[STRLEN];
    char buf2[STRLEN], include_mode = 'Y';
    char buf3[STRLEN], buf4[STRLEN];
    int i, replymode = 0;       /* Post New UI */

    strcpy(title, "û����");
    buf4[0] = '\0';
    modify_user_mode(SMAIL);
    clear();
    move(0, 0);
    sprintf(fname, "etc/%s.mailtoall", currentuser->userid);
    prints("��Ҫ�ĸ����еģ�\n");
    prints("(0) ����\n");
    strcpy(doc[0], "(1) δ��֤�����");
    strcpy(doc[1], "(2) ����֤�����");
    strcpy(doc[2], "(3) �а���Ȩ����");
    strcpy(doc[3], "(4) �����ų�Ա");
    for (i = 0; i < 4; i++)
        prints("%s\n", doc[i]);
    while (1) {
        getdata(8, 0, "������ģʽ (0~4)? [0]: ", ans4, 2, DOECHO, NULL, true);

        if (ans4[0] - '0' >= 1 && ans4[0] - '0' <= 4) {
            sprintf(buf, "�Ƿ�ȷ���ĸ�%s (Y/N)? [N]: ", doc[ans4[0] - '0' - 1]);
            getdata(9, 0, buf, ans2, 2, DOECHO, NULL, true);
            if (ans2[0] != 'Y' && ans2[0] != 'y') {
                return -1;
            }
            in_mail = true;
            /*
             * Leeward 98.01.17 Prompt whom you are writing to 
             */
            /*
             * strcpy(currentlookupuser->userid, doc[ans4[0]-'0'-1] + 4); 
             */

            if (currentuser->signature > numofsig)
                currentuser->signature = 1;
            while (1) {
                sprintf(buf3, "����ģʽ [[1m%c[m]", include_mode);
                move(t_lines - 4, 0);
                clrtoeol();
                prints("������: [1m%s[m\n", doc[ans4[0] - '0' - 1]);
                clrtoeol();
                prints("ʹ�ñ���: [1m%-50s[m\n", (title[0] == '\0') ? "[�����趨����]" : title);
                clrtoeol();
                if (currentuser->signature < 0)
                    prints("ʹ�����ǩ����     %s", (replymode) ? buf3 : "");
                else
                    prints("ʹ�õ� [1m%d[m ��ǩ����     %s", currentuser->signature, (replymode) ? buf3 : "");

                if (buf4[0] == '\0' || buf4[0] == '\n') {
                    move(t_lines - 1, 0);
                    clrtoeol();
                    getdata(t_lines - 1, 0, "����: ", buf4, 50, DOECHO, NULL, true);
                    if ((buf4[0] == '\0' || buf4[0] == '\n')) {
                        buf4[0] = ' ';
                        continue;
                    }
                    strcpy(title, buf4);
                    continue;
                }
                move(t_lines - 1, 0);
                clrtoeol();
                /*
                 * Leeward 98.09.24 add: viewing signature(s) while setting post head 
                 */
                sprintf(buf2, "��[1;32m0[m~[1;32m%d/V/L[mѡ/��/���ǩ����%s��[1;32mT[m�ı��⣬[1;32mEnter[m���������趨: ", numofsig,
                        (replymode) ? "��[1;32mY[m/[1;32mN[m/[1;32mR[m/[1;32mA[m������ģʽ" : "");
                getdata(t_lines - 1, 0, buf2, ans, 3, DOECHO, NULL, true);
                ans[0] = toupper(ans[0]);       /* Leeward 98.09.24 add; delete below toupper */
                if ((ans[0] - '0') >= 0 && ans[0] - '0' <= 9) {
                    if (atoi(ans) <= numofsig)
                        currentuser->signature = atoi(ans);
                } else if ((ans[0] == 'Y' || ans[0] == 'N' || ans[0] == 'A' || ans[0] == 'R') && replymode) {
                    include_mode = ans[0];
                } else if (ans[0] == 'T') {
                    buf4[0] = '\0';
                } else if (ans[0] == 'V') {     /* Leeward 98.09.24 add: viewing signature(s) while setting post head */
                    sethomefile(buf2, currentuser->userid, "signatures");
                    move(t_lines - 1, 0);
                    if (askyn("Ԥ����ʾǰ����ǩ����, Ҫ��ʾȫ����", false) == true)
                        ansimore(buf2, 0);
                    else {
                        clear();
                        ansimore2(buf2, false, 0, 18);
                    }
                } else if (ans[0] == 'L') {
                    currentuser->signature = -1;
                } else {
                    strncpy(save_title, title, STRLEN);
                    break;
                }
            }
            do_quote(fname, include_mode, "", quote_user);
            if (vedit(fname, true) == -1) {
                in_mail = false;
                unlink(fname);
                clear();
                return -2;
            }
            move(t_lines - 1, 0);
            clrtoeol();
            prints("[32m[44m���ڼ��ż��У����Ժ�.....                                                        [m");
            refresh();
            mailtoall(ans4[0] - '0');
            move(t_lines - 1, 0);
            clrtoeol();
            unlink(fname);
            in_mail = false;
            return 0;
        } else
            in_mail = false;
        return 0;
    }
    return -1;
}

void m_internet()
{
    char receiver[STRLEN], title[STRLEN];

    modify_user_mode(SMAIL);
    getdata(1, 0, "������: ", receiver, 70, DOECHO, NULL, true);
    getdata(2, 0, "����  : ", title, 70, DOECHO, NULL, true);
    if (!invalidaddr(receiver) && strchr(receiver, '@') && strlen(title) > 0) {
        clear();                /* Leeward 98.09.24fix a bug */
        switch (do_send(receiver, title, "")) { /* Leeward 98.05.11 adds "switch" */
        case -1:
            prints("�����߲���ȷ\n");
            break;
        case -2:
            prints("ȡ������\n");
            break;
        case -3:
            prints("'%s' �޷�����\n", receiver);
            break;
        case -4:
            clear();
            move(1, 0);
            prints("%s ��������,�޷�����\n", receiver);
            break;              /*Haohmaru.4.5.�������� */
        case -5:
            clear();
            move(1, 0);
            prints("%s ��ɱ�У���������\n", receiver);
            break;              /*Haohmaru.99.10.26.��ɱ�߲������� */
        case -552:
            prints("\n[1m[33m�ż���������վ�޶��ż���������Ϊ %d �ֽڣ���ȡ�����Ų���[m[m\n", MAXMAILSIZE);
            break;
        default:
            prints("�ż��Ѽĳ�\n");
        }
        pressreturn();

    } else {
        move(3, 0);
        prints("�����˻����ⲻ��ȷ, ������ѡȡָ��\n");
        pressreturn();
    }
    clear();
    refresh();
}

void m_init()
{
    setmailfile(currmaildir, currentuser->userid, DOT_DIR);
}

int do_send(char *userid, char *title, char *q_file)
{
    struct fileheader newmessage;
    struct stat st;
    char filepath[STRLEN], fname[STRLEN], *ip;
    int fp, sum, sumlimit;
    char buf2[256], buf3[STRLEN], buf4[STRLEN];
    int replymode = 1;          /* Post New UI */
    char ans[4], include_mode = 'Y';

    int internet_mail = 0;
    char tmp_fname[256];
    int noansi;
    struct userec *user;
    extern char quote_title[120];

    int now;                    /* added by Bigman: for SYSOP mail */
    int ret;

    if (!strchr(userid, '@')) {
        if (getuser(userid, &user) == 0)
            return -4;
        ret = chkreceiver(currentuser, user);
        if (ret == 1)
            return -3;
        /*
         * SYSOPҲ�ܸ���ɱ���˷��� 
         */


        if (ret == 2) {
            move(1, 0);
            prints("�������������������, �޷������ż���", sum, sumlimit);
            pressreturn();
            return -2;
        }
    }
#ifdef INTERNET_PRIVATE_EMAIL
    /*
     * I hate go to , but I use it again for the noodle code :-) 
     */
    else {
        /*
         * if(!strstr(userid,"edu.tw")){
         * if(strstr(userid,"@bbs.ee.nthu."))
         * strcat(userid,"edu.tw");
         * else
         * strcat(userid,".edu.tw");} 
         */
        if (chkusermail(currentuser)) {
            move(1, 0);
            prints("�������������������, �޷������ż���", sum, sumlimit);
            pressreturn();
            return -2;
        }
        internet_mail = 1;
        modify_user_mode(IMAIL);
        buf4[0] = ' ';
        sprintf(tmp_fname, "tmp/bbs-internet-gw/%05d", getpid());
        strcpy(filepath, tmp_fname);
        goto edit_mail_file;
    }
    /*
     * end of kludge for internet mail 
     */
#endif

    if (ret == 3)
        return -3;

    setmailpath(filepath, userid);
    if (stat(filepath, &st) == -1) {
        if (mkdir(filepath, 0755) == -1)
            return -1;
    } else {
        if (!(st.st_mode & S_IFDIR))
            return -1;
    }

    memset(&newmessage, 0, sizeof(newmessage));
    now = time(NULL);
    sprintf(fname, "M.%d.A", now);

    setmailfile(filepath, userid, fname);
    ip = strrchr(fname, 'A');
    while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1) {
        if (*ip == 'Z')
            ip++, *ip = 'A', *(ip + 1) = '\0';
        else
            (*ip)++;
        setmailfile(filepath, userid, fname);
    }

    close(fp);
    strcpy(newmessage.filename, fname);


    /*
     * strncpy(newmessage.title,title,STRLEN) ; 
     */
    in_mail = true;
#if defined(MAIL_REALNAMES)
    sprintf(genbuf, "%s (%s)", currentuser->userid, currentuser->realname);
#else
/*sprintf(genbuf,"%s (%s)",currentuser->userid,currentuser->username) ;*/
    strcpy(genbuf, currentuser->userid);        /* Leeward 98.04.14 */
#endif
    strncpy(newmessage.owner, genbuf, OWNER_LEN);
    newmessage.owner[OWNER_LEN - 1] = 0;

    setmailfile(filepath, userid, fname);

#ifdef INTERNET_PRIVATE_EMAIL
  edit_mail_file:
#endif
    if (!title) {
        replymode = 0;
        title = "û����";
        buf4[0] = '\0';
    } else
        buf4[0] = ' ';

    if (currentuser->signature > numofsig)
        currentuser->signature = 1;
    while (1) {
        sprintf(buf3, "����ģʽ [[1m%c[m]", include_mode);
        move(t_lines - 4, 0);
        clrtoeol();
        prints("������: [1m%s[m\n", userid);
        clrtoeol();
        prints("ʹ�ñ���: [1m%-50s[m\n", (title[0] == '\0') ? "[�����趨����]" : title);
        clrtoeol();
        if (currentuser->signature < 0)
            prints("ʹ�����ǩ����     %s", (replymode) ? buf3 : "");
        else
            prints("ʹ�õ� [1m%d[m ��ǩ����     %s", currentuser->signature, (replymode) ? buf3 : "");

        if (buf4[0] == '\0' || buf4[0] == '\n') {
            move(t_lines - 1, 0);
            clrtoeol();
            getdata(t_lines - 1, 0, "����: ", buf4, 50, DOECHO, NULL, true);
            if ((buf4[0] == '\0' || buf4[0] == '\n')) {
                buf4[0] = ' ';
                continue;
            }
            title = buf4;
            continue;
        }
        move(t_lines - 1, 0);
        clrtoeol();
        /*
         * Leeward 98.09.24 add: viewing signature(s) while setting post head 
         */
        sprintf(buf2, "�� [1;32m0[m~[1;32m%d/V/L[mѡ/��/���ǩ����%s��[1;32mT[m�ı��⣬[1;32mEnter[m���������趨: ", numofsig,
                (replymode) ? "��[1;32mY[m/[1;32mN[m/[1;32mR[m/[1;32mA[m������ģʽ" : "");
        getdata(t_lines - 1, 0, buf2, ans, 3, DOECHO, NULL, true);
        ans[0] = toupper(ans[0]);       /* Leeward 98.09.24 add; delete below toupper */
        if ((ans[0] - '0') >= 0 && ans[0] - '0' <= 9) {
            if (atoi(ans) <= numofsig)
                currentuser->signature = atoi(ans);
        } else if ((ans[0] == 'Y' || ans[0] == 'N' || ans[0] == 'A' || ans[0] == 'R') && replymode) {
            include_mode = ans[0];
        } else if (ans[0] == 'T') {
            buf4[0] = '\0';
        } else if (ans[0] == 'L') {
            currentuser->signature = -1;
        } else if (ans[0] == 'V') {     /* Leeward 98.09.24 add: viewing signature(s) while setting post head */
            sethomefile(buf2, currentuser->userid, "signatures");
            move(t_lines - 1, 0);
            if (askyn("Ԥ����ʾǰ����ǩ����, Ҫ��ʾȫ����", false) == true)
                ansimore(buf2, 0);
            else {
                clear();
                ansimore2(buf2, false, 0, 18);
            }
        } else {
            strcpy(newmessage.title, title);
            strncpy(save_title, newmessage.title, STRLEN);
            break;
        }
    }

    do_quote(filepath, include_mode, q_file, quote_user);
    strcpy(quote_title, newmessage.title);

#ifdef INTERNET_PRIVATE_EMAIL
    if (internet_mail) {
        int res, ch;

        if (vedit(filepath, false) == -1) {
            unlink(filepath);
            clear();
            return -2;
        }
        clear();
      redo:
        prints("�ż������ĸ� %s \n", userid);
        prints("����Ϊ�� %s \n", title);
        prints("ȷ��Ҫ�ĳ���? (Y/N) [Y]");
        refresh();
        ch = igetkey();
        switch (ch) {
        case KEY_REFRESH:
            move(3, 0);
            goto redo;
        case 'N':
        case 'n':
            prints("%c\n", 'N');
            prints("\n�ż���ȡ��...\n");
            res = -2;
            break;
        default:
            {
                /*
                 * uuencode or convert to big5 option -- Add by ming, 96.10.9 
                 */
                char data[3];
                int isuu, isbig5;

                prints("%c\n", 'Y');
                if (askyn("�Ƿ񱸷ݸ��Լ�", false) == true)
                    mail_file(currentuser->userid, tmp_fname, currentuser->userid, save_title, 0);

                prints("����Ҫת�ĵĵ�ַ�޷��������������� Y �� y\n");
                getdata(5, 0, "Uuencode? [N]: ", data, 2, DOECHO, 0, 0);
                if (data[0] == 'y' || data[0] == 'Y')
                    isuu = 1;
                else
                    isuu = 0;

                prints("����Ҫ���ż�ת�ĵ�̨�������� Y �� y\n");
                getdata(7, 0, "ת��BIG5��? [N]: ", data, 2, DOECHO, 0, 0);
                if (data[0] == 'y' || data[0] == 'Y')
                    isbig5 = 1;
                else
                    isbig5 = 0;

                getdata(8, 0, "����ANSI���Ʒ��? [Y]: ", data, 2, DOECHO, 0, 0);
                if (data[0] == 'n' || data[0] == 'N')
                    noansi = 0;
                else
                    noansi = 1;

                prints("���Ժ�, �ż�������...\n");
                refresh();
                /*
                 * res = bbs_sendmail( tmp_fname, title, userid );  
                 */
                res = bbs_sendmail(tmp_fname, title, userid, isuu, isbig5, noansi);

                newbbslog(BBSLOG_USER, "mailed %s", userid);
                break;
            }
        }
        unlink(tmp_fname);
        return res;
    } else
#endif
    {
        if (vedit(filepath, true) == -1) {
            unlink(filepath);
            clear();
            return -2;
        }
        clear();
//        if (askyn("�Ƿ񱸷ݸ��Լ�", false) == true)
//            mail_file(currentuser->userid, filepath, currentuser->userid, save_title, 0);
        /*
         * if(!chkreceiver(userid))
         * {
         * prints("%s ��������,�޷�����",userid);
         * return -4;
         * }
         */

        if (false == canIsend2(userid)) {       /* Leeward 98.04.10 */
            prints("[1m[33m�ܱ�Ǹ��ϵͳ�޷��������ţ���Ϊ %s �ܾ����������ż���[m[m\n\n", userid);
            sprintf(save_title, "���š� %s �ܾ����������ż���", userid);
            mail_file(currentuser->userid, filepath, currentuser->userid, save_title, 1);
            return -2;
        }
        if (askyn("ȷ���ĳ���", true) == false)
            return -2;

        mail_file_sent(userid, filepath, currentuser->userid, save_title, 0);
        setmailfile(genbuf, userid, DOT_DIR);
        if (append_record(genbuf, &newmessage, sizeof(newmessage)) == -1)
            return -1;

        if (stat(filepath, &st) != -1) {
            user->usedspace += st.st_size;
            currentuser->usedspace += st.st_size;
        }

        newbbslog(BBSLOG_USER, "mailed %s", userid);
        if (!strcasecmp(userid, "SYSOP"))
            updatelastpost("sysmail");
        return 0;
    }
}

int m_send(char userid[])
{
    char uident[STRLEN];

    /*
     * ���Mail Bigman:2000.8.22 
     */
    if (HAS_PERM(currentuser, PERM_DENYMAIL))
        return DONOTHING;

    if (uinfo.mode != LUSERS && uinfo.mode != LAUSERS && uinfo.mode != FRIEND && uinfo.mode != GMENU) {
        move(1, 0);
        clrtoeol();
        modify_user_mode(SMAIL);
        usercomplete("�����ˣ� ", uident);
        if (uident[0] == '\0') {
            clear();
            return 0;
        }
    } else
        strcpy(uident, userid);
    clear();
    switch (do_send(uident, NULL, "")) {
    case -1:
        prints("�����߲���ȷ\n");
        break;
    case -2:
        prints("ȡ������\n");
        break;
    case -3:
        prints("'%s' �޷�����\n", uident);
        break;
    case -4:
        clear();
        move(1, 0);
        prints("%s ��������,�޷�����\n", uident);
        break;                  /*Haohmaru.4.5.�������� */
    case -5:
        clear();
        move(1, 0);
        prints("%s ��ɱ�У���������\n", uident);
        break;                  /*Haohmaru.99.10.26.��ɱ�߲������� */
    case -552:
        prints("\n[1m[33m�ż���������վ�޶��ż���������Ϊ %d �ֽڣ���ȡ�����Ų���[m[m\n", MAXMAILSIZE);
        break;
    default:
        prints("�ż��Ѽĳ�\n");
    }
    pressreturn();
    return 0;
}

int read_mail(fptr)
struct fileheader *fptr;
{
    setmailfile(genbuf, currentuser->userid, fptr->filename);
    ansimore(genbuf, false);
    fptr->accessed[0] |= FILE_READ;
    return 0;
}

int del_mail(int ent, struct fileheader *fh, char *direct)
{
    char buf[PATHLEN];
    char *t;
    struct stat st;

    if (strstr(direct, ".DELETED")) {
        strcpy(buf, direct);
        t = strrchr(buf, '/') + 1;
        strcpy(t, fh->filename);
        if (stat(buf, &st) != -1)
            currentuser->usedspace -= st.st_size;
    }

    strcpy(buf, direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
    if (!delete_record(direct, sizeof(*fh), ent, (RECORD_FUNC_ARG) cmpname, fh->filename)) {
        sprintf(genbuf, "%s/%s", buf, fh->filename);
        if (strstr(direct, ".DELETED"))
            unlink(genbuf);
        else {
            strcpy(buf, direct);
            t = strrchr(buf, '/') + 1;
            strcpy(t, ".DELETED");
            append_record(buf, fh, sizeof(*fh));
        }
        return 0;
    }
    return 1;
}

int mrd;
int delete_new_mail(struct fileheader *fptr, int idc, void *arg)
{
    if (fptr->accessed[1] & FILE_DEL) {
        del_mail(idc, fptr, currmaildir);
        return 1;
    }
    return 0;
}

int read_new_mail(struct fileheader *fptr, int idc, void *arg)
{
    char done = false, delete_it;
    char fname[256];

    if (fptr->accessed[0])
        return 0;
    prints("��ȡ %s ������ '%s' ?\n", fptr->owner, fptr->title);
    prints("(Yes, or No): ");
    getdata(1, 0, "(Y)��ȡ (N)���� (Q)�뿪 [Y]: ", genbuf, 3, DOECHO, NULL, true);
    if (genbuf[0] == 'q' || genbuf[0] == 'Q') {
        clear();
        return QUIT;
    }
    if (genbuf[0] != 'y' && genbuf[0] != 'Y' && genbuf[0] != '\0') {
        clear();
        return 0;
    }
    read_mail(fptr);
    strcpy(fname, genbuf);
    mrd = 1;
    delete_it = false;
    while (!done) {
        move(t_lines - 1, 0);
        prints("(R)����, (D)ɾ��, (G)���� ? [G]: ");
        switch (igetkey()) {
        case 'R':
        case 'r':

            /*
             * ���Mail Bigman:2000.8.22 
             */
            if (HAS_PERM(currentuser, PERM_DENYMAIL)) {
                clear();
                move(3, 10);
                prints("�ܱ�Ǹ,��Ŀǰû��MailȨ��!");
                pressreturn();
                break;
            }
            mail_reply(idc, fptr, currmaildir);
            /*
             * substitute_record(currmaildir, fptr, sizeof(*fptr), dc);
             */
            break;
        case 'D':
        case 'd':
            delete_it = true;
        default:
            done = true;
        }
        if (!done)
            ansimore(fname, false);     /* re-read */
    }
    if (delete_it) {
        clear();
        prints("Delete Message '%s' ", fptr->title);
        getdata(1, 0, "(Yes, or No) [N]: ", genbuf, 3, DOECHO, NULL, true);
        if (genbuf[0] == 'Y' || genbuf[0] == 'y') {     /* if not yes quit */
            fptr->accessed[1] |= FILE_DEL;
        }
    }
    if (substitute_record(currmaildir, fptr, sizeof(*fptr), idc))
        return -1;
    clear();
    return 0;
}

int m_new()
{
    clear();
    mrd = 0;
    modify_user_mode(RMAIL);
    setmailfile(currmaildir, currentuser->userid, ".DIR");
    if (apply_record(currmaildir, (APPLY_FUNC_ARG) read_new_mail, sizeof(struct fileheader), NULL, 1, false) == -1) {
        clear();
        move(0, 0);
        prints("No new messages\n\n\n");
        return -1;
    }
    apply_record(currmaildir, (APPLY_FUNC_ARG) delete_new_mail, sizeof(struct fileheader), NULL, 1, true);
/*    	
    if (delcnt) {
        while (delcnt--)
            delete_record(currmaildir, sizeof(struct fileheader), delmsgs[delcnt], NULL, NULL);
    }
*/
    clear();
    move(0, 0);
    if (mrd)
        prints("No more messages.\n\n\n");
    else
        prints("No new messages.\n\n\n");
    return -1;
}

void mailtitle()
{
    /*
     * Leeward 98.01.19 adds below codes for statistics 
     */
#ifdef NINE_BUILD
    int MailSpace = 10000;
#else
    int MailSpace = ((HAS_PERM(currentuser, PERM_SYSOP)
                      || !strcmp(currentuser->userid, "Arbitrator")) ? 9999 : (HAS_PERM(currentuser,
                                                                                        PERM_CHATCLOAK) ? 4000 : (HAS_PERM(currentuser, PERM_MANAGER) ? 600
                                                                                                                  : (HAS_PERM(currentuser, PERM_LOGINOK) ? 240 : 15))));
#endif
    int UsedSpace = get_mailusedspace(currentuser, 0) / 1024;

    showtitle("�ʼ�ѡ��    ", BBS_FULL_NAME);
    prints("�뿪[��,e]  ѡ��[��,��]  �Ķ��ż�[��,r]  ����[R]  ���ţ��������[d,D]  ����[h][m\n");
    /*
     * prints("[44m���    %-20s %-49s[m\n","������","��  ��") ; 
     */
    if (0 != get_mailnum() && 0 == UsedSpace)
        UsedSpace = 1;
    else if (UsedSpace < 0)
        UsedSpace = 0;
    prints("[44m���    %-12s %6s  %-13s����������������%4dK����ǰ����%4dK [m\n", (strstr(currmaildir, ".SENT")) ? "������" : "������", "��  ��", "��  ��", MailSpace, UsedSpace);    /* modified by dong , 1998.9.19 */
//    clrtobot();
}

char *maildoent(char *buf, int num, struct fileheader *ent)
{
    time_t filetime;
    char *date;
    char b2[512];
    char status, reply_status;
    char *t;
    extern char ReadPost[];
    extern char ReplyPost[];
    char c1[8];
    char c2[8];
    int same = false;

    filetime = get_posttime(ent);       /* ���ļ���ȡ��ʱ�� */
    if (filetime > 740000000)
        date = ctime(&filetime) + 4;    /* ʱ�� -> Ӣ�� */
    else
        /*
         * date = ""; char *���ͱ���, ���ܴ���, modified by dong, 1998.9.19 
         */
    {
        date = ctime(&filetime) + 4;
        date = "";
    }

    if (DEFINE(currentuser, DEF_HIGHCOLOR)) {
        strcpy(c1, "[1;33m");
        strcpy(c2, "[1;36m");
    } else {
        strcpy(c1, "[33m");
        strcpy(c2, "[36m");
    }
    if (!strcmp(ReadPost, ent->title) || !strcmp(ReplyPost, ent->title))
        same = true;
    strncpy(b2, ent->owner, OWNER_LEN);
    ent->owner[OWNER_LEN - 1] = 0;
    if ((t = strchr(b2, ' ')) != NULL)
        *t = '\0';
    if (ent->accessed[0] & FILE_READ) {
        if (ent->accessed[0] & FILE_MARKED)
            status = 'm';
        else
            status = ' ';
    } else {
        if (ent->accessed[0] & FILE_MARKED)
            status = 'M';
        else
            status = 'N';
    }
    if (ent->accessed[0] & FILE_REPLIED) {
        if (ent->accessed[0] & FILE_FORWARDED)
            reply_status = 'A';
        else
            reply_status = 'R';
    } else {
        if (ent->accessed[0] & FILE_FORWARDED)
            reply_status = 'F';
        else
            reply_status = ' ';
    }
    /*
     * if (ent->accessed[0] & FILE_REPLIED)
     * reply_status = 'R';
     * else
     * reply_status = ' '; 
 *//*
 * * * * * added by alex, 96.9.7 
 */
    if (!strncmp("Re:", ent->title, 3)) {
        sprintf(buf, " %s%3d[m %c%c %-12.12s %6.6s  %s%.50s[m", same ? c1 : "", num, reply_status, status, b2, date, same ? c1 : "", ent->title);
    } /* modified by dong, 1998.9.19 */
    else {
        sprintf(buf, " %s%3d[m %c%c %-12.12s %6.6s  �� %s%.49s[m", same ? c2 : "", num, reply_status, status, b2, date, same ? c2 : "", ent->title);
    }                           /* modified by dong, 1998.9.19 */
    return buf;
}

#ifdef POSTBUG
extern int bug_possible;
#endif

int mail_read(ent, fileinfo, direct)
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char buf[512], notgenbuf[128];
    char *t;
    int readnext;
    char done = false, delete_it, replied;

    clear();
    readnext = false;
    setqtitle(fileinfo->title);
    strcpy(buf, direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
    sprintf(notgenbuf, "%s/%s", buf, fileinfo->filename);
    delete_it = replied = false;
    while (!done) {
        ansimore(notgenbuf, false);
        move(t_lines - 1, 0);
        prints("(R)����, (D)ɾ��, (G)����? [G]: ");
        switch (igetkey()) {
        case 'R':
        case 'r':

            /*
             * ���Mail Bigman:2000.8.22 
             */
            if (HAS_PERM(currentuser, PERM_DENYMAIL)) {
                clear();
                move(3, 10);
                prints("�ܱ�Ǹ,��Ŀǰû��MailȨ��!");
                pressreturn();
                break;
            }
            replied = true;
            mail_reply(ent, fileinfo, direct);
            break;
        case ' ':
        case 'j':
        case KEY_RIGHT:
        case KEY_DOWN:
        case KEY_PGDN:
            done = true;
            readnext = true;
            break;
        case 'D':
        case 'd':
            delete_it = true;
        default:
            done = true;
        }
    }
    if (delete_it)
        return mail_del(ent, fileinfo, direct);
    else {
        fileinfo->accessed[0] |= FILE_READ;
#ifdef POSTBUG
        if (replied)
            bug_possible = true;
#endif
        substitute_record(currmaildir, fileinfo, sizeof(*fileinfo), ent);
#ifdef POSTBUG
        bug_possible = false;
#endif
    }
    if (readnext == true)
        return READ_NEXT;
    return FULLUPDATE;
}

 /*ARGSUSED*/ static int mail_reply(int ent, struct fileheader *fileinfo, char *direct)
{
    char uid[STRLEN];
    char title[STRLEN];
    char q_file[STRLEN];
    char *t;

    clear();
    modify_user_mode(SMAIL);
    strncpy(uid, fileinfo->owner, OWNER_LEN);
    uid[OWNER_LEN - 1] = 0;
    if ((t = strchr(uid, ' ')) != NULL)
        *t = '\0';
    if (toupper(fileinfo->title[0]) != 'R' || fileinfo->title[1] != 'e' || fileinfo->title[2] != ':')
        strcpy(title, "Re: ");
    else
        title[0] = '\0';
    strncat(title, fileinfo->title, STRLEN - 5);

    setmailfile(q_file, currentuser->userid, fileinfo->filename);
    strncpy(quote_user, fileinfo->owner, IDLEN);
    quote_user[IDLEN] = 0;
    switch (do_send(uid, title, q_file)) {
    case -1:
        prints("�޷�Ͷ��\n");
        break;
    case -2:
        prints("ȡ������\n");
        break;
    case -3:
        prints("'%s' �޷�����\n", uid);
        break;
    case -4:
        clear();
        move(1, 0);
        prints("%s ��������,�޷�����\n", uid);
        break;                  /*Haohmaru.4.5.�������� */
    case -5:
        clear();
        move(1, 0);
        prints("%s ��ɱ�У���������\n", uid);
        break;                  /*Haohmaru.99.10.26.��ɱ�߲������� */
    default:
        prints("�ż��Ѽĳ�\n");
        fileinfo->accessed[0] |= FILE_REPLIED;  /*added by alex, 96.9.7 */
    }
    pressreturn();
    return FULLUPDATE;
}

static int mail_del(int ent, struct fileheader *fileinfo, char *direct)
{
    clear();
    prints("ɾ�����ż� '%s' ", fileinfo->title);
    getdata(1, 0, "(Yes, or No) [N]: ", genbuf, 2, DOECHO, NULL, true);
    if (genbuf[0] != 'Y' && genbuf[0] != 'y') { /* if not yes quit */
        move(2, 0);
        prints("ȡ��ɾ��\n");
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    if (del_mail(ent, fileinfo, direct) == 0) {
        return DIRCHANGED;
    }
    move(2, 0);
    prints("ɾ��ʧ��\n");
    pressreturn();
    clear();
    return FULLUPDATE;
}

/** Added by netty to handle mail to 0Announce */
int mail_to_tmp(ent, fileinfo, direct)
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char buf[STRLEN];
    char *p;
    char fname[STRLEN];
    char board[STRLEN];
    char ans[STRLEN];

    if (!HAS_PERM(currentuser, PERM_BOARDS)) {
        return DONOTHING;
    }
    strncpy(buf, direct, sizeof(buf));
    if ((p = strrchr(buf, '/')) != NULL)
        *p = '\0';
    clear();
    sprintf(fname, "%s/%s", buf, fileinfo->filename);
    sprintf(genbuf, "��--%s--�����ݴ浵,ȷ����?(Y/N) [N]: ", fileinfo->title);
    a_prompt(-1, genbuf, ans);
    if (ans[0] == 'Y' || ans[0] == 'y') {
        sprintf(board, "tmp/bm.%s", currentuser->userid);
        if (dashf(board)) {
            sprintf(genbuf, "Ҫ�����ھ��ݴ浵֮����?(Y/N) [N]: ");
            a_prompt(-1, genbuf, ans);
            if (ans[0] == 'Y' || ans[0] == 'y') {
                sprintf(genbuf, "/bin/cat %s >> tmp/bm.%s", fname, currentuser->userid);
                system(genbuf);
            } else {
                /*
                 * sprintf( genbuf, "/bin/cp -r %s  tmp/bm.%s", fname , currentuser->userid );
                 */
                sprintf(genbuf, "tmp/bm.%s", currentuser->userid);
                f_cp(fname, genbuf, 0);
            }
        } else {
            sprintf(genbuf, "tmp/bm.%s", currentuser->userid);
            f_cp(fname, genbuf, 0);
        }
        sprintf(genbuf, " �ѽ������´����ݴ浵, �밴�κμ��Լ��� << ");
        a_prompt(-1, genbuf, ans);
    }
    clear();
    return FULLUPDATE;
}


#ifdef INTERNET_EMAIL
int mail_forward_internal(int ent, struct fileheader *fileinfo, char *direct, int isuu)
{
    char buf[STRLEN];
    char *p;

    if (strcmp("guest", currentuser->userid) == 0) {
        clear();
        move(3, 10);
        prints("�ܱ�Ǹ,��ת��������������ʽID!");
        pressreturn();
        return FULLUPDATE;
    }

    /*
     * ���Mail Bigman:2000.8.22 
     */
    if (HAS_PERM(currentuser, PERM_DENYMAIL)) {
        clear();
        move(3, 10);
        prints("�ܱ�Ǹ,��Ŀǰû��MailȨ��!");
        pressreturn();
        return FULLUPDATE;
    }

    if (!HAS_PERM(currentuser, PERM_FORWARD)) {
        return DONOTHING;
    }
    strncpy(buf, direct, sizeof(buf));
    if ((p = strrchr(buf, '/')) != NULL)
        *p = '\0';
    clear();
    switch (doforward(buf, fileinfo, isuu)) {
    case 0:
        prints("����ת�����!\n");
        fileinfo->accessed[0] |= FILE_FORWARDED;        /*added by alex, 96.9.7 */
        /*
         * comment out by jjyang for direct mail delivery 
         */
        newbbslog(BBSLOG_USER, "forwarded file to %s", currentuser->email);
        /*
         * comment out by jjyang for direct mail delivery 
         */

        break;
    case -1:
        prints("Forward failed: system error.\n");
        break;
    case -2:
        prints("Forward failed: missing or invalid address.\n");
        break;
    case -552:
        prints
            ("\n[1m[33m�ż���������վ�޶��ż���������Ϊ %d �ֽڣ���ȡ��ת�Ĳ���[m[m\n\n���֪�����ˣ�Ҳ��������Լ���:PP����\n\n*1* ʹ�� [1m[33mWWW[m[m ��ʽ���ʱ�վ����ʱ���Ա������ⳤ�ȵ����µ��Լ��ļ������\n*2* ʹ�� [1m[33mpop3[m[m ��ʽ�ӱ�վ�û�������ȡ�ţ�û���κγ������ơ�\n*3* �������Ϥ��վ�� WWW �� pop3 �������Ķ� [1m[33mAnnounce[m[m ���йع��档\n",
             MAXMAILSIZE);
        break;
    default:
        prints("ȡ��ת��...\n");
    }
    pressreturn();
    clear();
    return FULLUPDATE;
}

int mail_uforward(int ent, struct fileheader *fileinfo, char *direct)
{
    return mail_forward_internal(ent, fileinfo, direct, 1);
}

int mail_forward(int ent, struct fileheader *fileinfo, char *direct)
{
    return mail_forward_internal(ent, fileinfo, direct, 0);
}

#endif

int mail_del_range(ent, fileinfo, direct)
int ent;
struct fileheader *fileinfo;
char *direct;
{
    int ret;

    ret = (del_range(ent, fileinfo, direct, 0));        /*Haohmaru.99.5.14.�޸�һ��bug,
                                                         * * ������ܻ���Ϊɾ�ż���.tmpfile����ɾ�����.tmpfile */
    if (!strcmp(direct, ".DELETED"))
        get_mailusedspace(currentuser, 1);
    return ret;
}

int mail_mark(ent, fileinfo, direct)
int ent;
struct fileheader *fileinfo;
char *direct;
{
    if (fileinfo->accessed[0] & FILE_MARKED)
        fileinfo->accessed[0] &= ~FILE_MARKED;
    else
        fileinfo->accessed[0] |= FILE_MARKED;
    substitute_record(currmaildir, fileinfo, sizeof(*fileinfo), ent);
    return (PARTUPDATE);
}


int mail_move(int ent, struct fileheader *fileinfo, char *direct)
{
    struct _select_item *sel;
    int i, j;
    char buf[PATHLEN];
    char *t;
    char menu_char[3][10] = { "I) �ռ���", "J) ������", "Q) �˳�" };

    clear();
    move(5, 3);
    prints("��ѡ���ƶ����ĸ�����");
    sel = (struct _select_item *) malloc(sizeof(struct _select_item) * (mail_list_t + 4));
    sel[0].x = 3;
    sel[0].y = 6;
    sel[0].hotkey = 'I';
    sel[0].type = SIT_SELECT;
    sel[0].data = menu_char[0];
    sel[1].x = 3;
    sel[1].y = 7;
    sel[1].hotkey = 'J';
    sel[1].type = SIT_SELECT;
    sel[1].data = menu_char[1];
    for (i = 0; i < mail_list_t; i++) {
        sel[i + 2].x = 3;
        sel[i + 2].y = i + 8;
        sel[i + 2].hotkey = mail_list[i][0];
        sel[i + 2].type = SIT_SELECT;
        sel[i + 2].data = (void *) mail_list[i];
    }
    sel[mail_list_t + 2].x = 3;
    sel[mail_list_t + 2].y = mail_list_t + 8;
    sel[mail_list_t + 2].hotkey = 'Q';
    sel[mail_list_t + 2].type = SIT_SELECT;
    sel[mail_list_t + 2].data = menu_char[2];
    sel[mail_list_t + 3].x = -1;
    sel[mail_list_t + 3].y = -1;
    sel[mail_list_t + 3].hotkey = -1;
    sel[mail_list_t + 3].type = 0;
    sel[mail_list_t + 3].data = NULL;
    i = simple_select_loop(sel, SIF_NUMBERKEY | SIF_SINGLE | SIF_ESCQUIT, 0, 6, NULL) - 1;
    if (i >= 0 && i < mail_list_t + 2) {
        strcpy(buf, direct);
        t = strrchr(buf, '/') + 1;
        *t = '.';
        t++;
        if (i >= 2)
            strcpy(t, mail_list[i - 2] + 30);
        else if (i == 0)
            strcpy(t, "DIR");
        else if (i == 1)
            strcpy(t, "DELETED");
        if (strcmp(buf, direct))
            if (!delete_record(direct, sizeof(*fileinfo), ent, (RECORD_FUNC_ARG) cmpname, fileinfo->filename)) {
                append_record(buf, fileinfo, sizeof(*fileinfo));
            }
    }
    free(sel);
    return (FULLUPDATE);
}

extern int mailreadhelp();

struct one_key mail_comms[] = {
    {'d', mail_del},
    {'D', mail_del_range},
    {'r', mail_read},
    {'R', mail_reply},
    {'m', mail_mark},
    {'M', mail_move},
    {'i', mail_to_tmp},
#ifdef INTERNET_EMAIL
    {'F', mail_forward},
    {'U', mail_uforward},
#endif
    /*
     * Added by ming, 96.10.9 
     */
    {'a', auth_search_down},
    {'A', auth_search_up},
    {'/', t_search_down},
    {'?', t_search_up},
    {']', thread_down},
    {'[', thread_up},
    {Ctrl('A'), show_author},
    {Ctrl('Q'), show_authorinfo},       /*Haohmaru.98.12.19 */
    {Ctrl('W'), show_authorBM}, /*cityhunter 00.10.18 */
    {Ctrl('N'), SR_first_new},
    {'\\', SR_last},
    {'=', SR_first},
    {Ctrl('C'), do_cross},
    {Ctrl('S'), SR_read},
    {'n', SR_first_new},
    {'p', SR_read},
    {Ctrl('X'), SR_readX},      /* Leeward 98.10.03 */
    {Ctrl('U'), SR_author},
    {Ctrl('H'), SR_authorX},    /* Leeward 98.10.03 */
    {'h', mailreadhelp},
    {Ctrl('J'), mailreadhelp},
    {Ctrl('O'), add_author_friend},
    {'\0', NULL},
};

int m_read()
{
    m_init();
    in_mail = true;
    i_read(RMAIL, currmaildir, mailtitle, (READ_FUNC) maildoent, &mail_comms[0], sizeof(struct fileheader));
    in_mail = false;
    return FULLUPDATE /* 0 */ ;
}

#ifdef INTERNET_EMAIL

#include <netdb.h>
#include <pwd.h>
#include <time.h>
#define BBSMAILDIR "/usr/spool/mqueue"
int invalidaddr(char *addr)
{
    if (*addr == '\0')
        return 1;               /* blank */
    while (*addr) {
        if (!isalnum(*addr) && strchr("[].%!@:-_", *addr) == NULL)
            return 1;
        addr++;
    }
    return 0;
}

void spacestozeros(s)
char *s;
{
    while (*s) {
        if (*s == ' ')
            *s = '0';
        s++;
    }
}

int getqsuffix(s)
char *s;
{
    struct stat stbuf;
    char qbuf[STRLEN], dbuf[STRLEN];
    char c1 = 'A', c2 = 'A';
    int pos = strlen(BBSMAILDIR) + 3;

    sprintf(dbuf, "%s/dfAA%5d", BBSMAILDIR, getpid());
    sprintf(qbuf, "%s/qfAA%5d", BBSMAILDIR, getpid());
    spacestozeros(dbuf);
    spacestozeros(qbuf);
    while (1) {
        if (stat(dbuf, &stbuf) && stat(qbuf, &stbuf))
            break;
        if (c2 == 'Z') {
            c2 = 'A';
            if (c1 == 'Z')
                return -1;
            else
                c1++;
            dbuf[pos] = c1;
            qbuf[pos] = c1;
        } else
            c2++;
        dbuf[pos + 1] = c2;
        qbuf[pos + 1] = c2;
    }
    strcpy(s, &(qbuf[pos]));
    return 0;
}

int g_send()
{
    char uident[13], tmp[3];
    int cnt, i, n, fmode = false;
    char maillists[STRLEN];
    struct userec *lookupuser;

    /*
     * ���Mail Bigman:2000.8.22 
     */
    if (HAS_PERM(currentuser, PERM_DENYMAIL))
        return DONOTHING;

    modify_user_mode(SMAIL);
    clear();
    sethomefile(maillists, currentuser->userid, "maillist");
    cnt = listfilecontent(maillists);
    while (1) {
        if (cnt > maxrecp - 10) {
            move(2, 0);
            prints("Ŀǰ���Ƽ��Ÿ� [1m%d[m ��", maxrecp);
        }
        getdata(0, 0, "(A)���� (D)ɾ�� (I)������� (C)���Ŀǰ���� (E)���� (S)�ĳ�? [S]�� ", tmp, 2, DOECHO, NULL, true);
        if (tmp[0] == '\n' || tmp[0] == '\0' || tmp[0] == 's' || tmp[0] == 'S') {
            break;
        }
        if (tmp[0] == 'a' || tmp[0] == 'd' || tmp[0] == 'A' || tmp[0] == 'D') {
            move(1, 0);
            if (tmp[0] == 'a' || tmp[0] == 'A')
                usercomplete("����������ʹ���ߴ���(ֻ�� ENTER ��������): ", uident);
            else
                namecomplete("����������ʹ���ߴ���(ֻ�� ENTER ��������): ", uident);
            move(1, 0);
            clrtoeol();
            if (uident[0] == '\0')
                continue;
            if (!getuser(uident, &lookupuser)) {
                move(2, 0);
                prints("���ʹ���ߴ����Ǵ����.\n");
                continue;
            } else
                strcpy(uident, lookupuser->userid);
        }
        switch (tmp[0]) {
        case 'A':
        case 'a':
            if (!(lookupuser->userlevel & PERM_READMAIL)) {
                move(2, 0);
                prints("�ż��޷����ĸ�: [1m%s[m\n", lookupuser->userid);
                break;
            } else if (seek_in_file(maillists, uident)) {
                move(2, 0);
                prints("�Ѿ���Ϊ�ռ���֮һ \n");
                break;
            }
            addtofile(maillists, uident);
            cnt++;
            break;
        case 'E':
        case 'e':
            cnt = 0;
            break;
        case 'D':
        case 'd':
            {
                if (seek_in_file(maillists, uident)) {
                    del_from_file(maillists, uident);
                    cnt--;
                }
                break;
            }
        case 'I':
        case 'i':
            n = 0;
            clear();
            for (i = cnt; i < maxrecp && n < nf; i++) {
                int key;

                move(2, 0);
                prints("%s\n", getuserid2(topfriend[n].uid));
                move(4, 0);
                clrtoeol();
                move(3, 0);
                n++;
                if (!fmode) {
                    prints("(A)ʣ�µ�ȫ������ (Y)���� (N)������ (Q)����? [Y]:");
                    //TODO: add KEY_REFRESH support
                    key = igetkey();
                } else
                    key = 'Y';
                if (key == 'q' || key == 'Q')
                    break;
                if (key == 'A' || key == 'a') {
                    fmode = true;
                    key = 'Y';
                }
                if (key == '\0' || key == '\n' || key == 'y' || key == 'Y' || '\r' == key) {
                    struct userec *lookupuser;
                    char *errstr;
                    char *touserid = getuserid2(topfriend[n - 1].uid);

                    errstr = NULL;
                    if (!touserid) {
                        errstr = "���ʹ���ߴ����Ǵ����.\n";
                    } else {
                        strcpy(uident, getuserid2(topfriend[n - 1].uid));
                        if (!getuser(uident, &lookupuser)) {
                            errstr = "���ʹ���ߴ����Ǵ����.\n";
                        } else if (!(lookupuser->userlevel & PERM_READMAIL)) {
                            errstr = "�ż��޷����ĸ���\n";
                        } else if (seek_in_file(maillists, uident)) {
                            i--;
                            continue;
                        }
                    }
                    if (errstr) {
                        if (fmode != true) {
                            move(4, 0);
                            prints(errstr);
                            pressreturn();
                        }
                        i--;
                        continue;
                    }
                    addtofile(maillists, uident);
                    cnt++;
                }
            }
            fmode = false;
            clear();
            break;
        case 'C':
        case 'c':
            unlink(maillists);
            cnt = 0;
            break;
        }
        if (tmp[0] == 'e' || tmp[0] == 'E')
            break;
        move(5, 0);
        clrtobot();
        if (cnt > maxrecp)
            cnt = maxrecp;
        move(3, 0);
        clrtobot();
        listfilecontent(maillists);
    }
    if (cnt > 0) {
        G_SENDMODE = 2;
        switch (do_gsend(NULL, NULL, cnt)) {
        case -1:
            prints("�ż�Ŀ¼����\n");
            break;
        case -2:
            prints("ȡ������\n");
            break;
        case -4:
            prints("�����Ѿ������޶�\n");
            break;
        default:
            prints("�ż��Ѽĳ�\n");
        }
        G_SENDMODE = 0;
        pressreturn();
    }
    return 0;
}

/*Add by SmallPig*/

static int do_gsend(char *userid[], char *title, int num)
{
    struct stat st;
    char buf2[256], buf3[STRLEN], buf4[STRLEN];
    int replymode = 1;          /* Post New UI */
    char ans[4], include_mode = 'Y';
    char filepath[STRLEN], tmpfile[STRLEN];
    int cnt;
    FILE *mp;
    extern char quote_title[120];

    /*
     * ����ں��Ѽ���ʱ�ķ����������� Bigman 2000.12.11 
     */
    if (chkusermail(currentuser)) {
        move(1, 0);
        prints("��������Ѿ������޶�޷�ת���ż���\n");
        pressreturn();
        return -4;
    }

    in_mail = true;
#if defined(MAIL_REALNAMES)
    sprintf(genbuf, "%s (%s)", currentuser->userid, currentuser->realname);
#else
    /*
     * sprintf(genbuf,"%s (%s)",currentuser->userid,currentuser->username) ; 
     */
    strcpy(genbuf, currentuser->userid);        /* Leeward 98.04.14 */
#endif
    move(1, 0);
    clrtoeol();
    if (!title) {
        replymode = 0;
        title = "û����";
        buf4[0] = '\0';
    } else
        buf4[0] = ' ';

    sprintf(tmpfile, "tmp/bbs-gsend/%05d", getpid());
    /*
     * Leeward 98.01.17 Prompt whom you are writing to 
     * if (1 == G_SENDMODE)
     * strcpy(lookupuser->userid, "��������");
     * else if (2 == G_SENDMODE)
     * strcpy(lookupuser->userid, "��������");
     * else
     * strcpy(lookupuser->userid, "��λ����");
     */

    if (currentuser->signature > numofsig)
        currentuser->signature = 1;
    while (1) {
        sprintf(buf3, "����ģʽ [[1m%c[m]", include_mode);
        move(t_lines - 3, 0);
        clrtoeol();
        prints("ʹ�ñ���: [1m%-50s[m\n", (title[0] == '\0') ? "[�����趨����]" : title);
        clrtoeol();
        if (currentuser->signature < 0)
            prints("ʹ�����ǩ����     %s", (replymode) ? buf3 : "");
        else
            prints("ʹ�õ� [1m%d[m ��ǩ����     %s", currentuser->signature, (replymode) ? buf3 : "");

        if (buf4[0] == '\0' || buf4[0] == '\n') {
            move(t_lines - 1, 0);
            clrtoeol();
            getdata(t_lines - 1, 0, "����: ", buf4, 50, DOECHO, NULL, true);
            if ((buf4[0] == '\0' || buf4[0] == '\n')) {
                buf4[0] = ' ';
                continue;
            }
            title = buf4;
            continue;
        }
        move(t_lines - 1, 0);
        clrtoeol();
        /*
         * Leeward 98.09.24 add: viewing signature(s) while setting post head 
         */
        sprintf(buf2, "��[1;32m0[m~[1;32m%d/V/L[mѡ/��/���ǩ����%s��[1;32mT[m�ı��⣬[1;32mEnter[m���������趨: ", numofsig,
                (replymode) ? "��[1;32mY[m/[1;32mN[m/[1;32mR[m/[1;32mA[m������ģʽ" : "");
        getdata(t_lines - 1, 0, buf2, ans, 3, DOECHO, NULL, true);
        ans[0] = toupper(ans[0]);       /* Leeward 98.09.24 add; delete below toupper */
        if ((ans[0] - '0') >= 0 && ans[0] - '0' <= 9) {
            if (atoi(ans) <= numofsig)
                currentuser->signature = atoi(ans);
        } else if ((ans[0] == 'Y' || ans[0] == 'N' || ans[0] == 'A' || ans[0] == 'R') && replymode) {
            include_mode = ans[0];
        } else if (ans[0] == 'T') {
            buf4[0] = '\0';
        } else if (ans[0] == 'V') {     /* Leeward 98.09.24 add: viewing signature(s) while setting post head */
            sethomefile(buf2, currentuser->userid, "signatures");
            move(t_lines - 1, 0);
            if (askyn("Ԥ����ʾǰ����ǩ����, Ҫ��ʾȫ����", false) == true)
                ansimore(buf2, 0);
            else {
                clear();
                ansimore2(buf2, false, 0, 18);
            }
        } else if (ans[0] == 'L') {
            currentuser->signature = -1;
        } else {
            strncpy(save_title, title, STRLEN);
            break;
        }
    }

    /*
     * Bigman:2000.8.13 Ⱥ�巢��ΪʲôҪ���������� 
     */
    /*
     * do_quote( tmpfile,include_mode ); 
     */

    strcpy(quote_title, save_title);
    if (vedit(tmpfile, true) == -1) {
        unlink(tmpfile);
        clear();
        return -2;
    }
    clear();
    if (G_SENDMODE == 2) {
        char maillists[STRLEN];

        sethomefile(maillists, currentuser->userid, "maillist");
        if ((mp = fopen(maillists, "r")) == NULL) {
            return -3;
        }
    }
    for (cnt = 0; cnt < num; cnt++) {
        char uid[13];
        char buf[STRLEN];
        struct userec *user;

        if (G_SENDMODE == 1)
            getuserid(uid, topfriend[cnt].uid);
        else if (G_SENDMODE == 2) {
            if (fgets(buf, STRLEN, mp) != NULL) {
                if (strtok(buf, " \n\r\t") != NULL)
                    strcpy(uid, buf);
                else
                    continue;
            } else {
                cnt = num;
                continue;
            }
        } else
            strcpy(uid, userid[cnt]);
        setmailpath(filepath, uid);
        if (stat(filepath, &st) == -1) {
            if (mkdir(filepath, 0755) == -1) {
                if (G_SENDMODE == 2)
                    fclose(mp);
                return -1;
            }
        } else {
            if (!(st.st_mode & S_IFDIR)) {
                if (G_SENDMODE == 2)
                    fclose(mp);
                return -1;
            }
        }

        if (getuser(uid, &user) == 0) {
            prints("�Ҳ����û�%s,�밴 Enter �������������˷���...", uid);
            pressreturn();
            clear();
        } else if (user->userlevel & PERM_SUICIDE) {
            prints("%s ��ɱ�У��������ţ��밴 Enter �������������˷���...", uid);
            pressreturn();
            clear();
        } else if (!(user->userlevel & PERM_READMAIL)) {
            prints("%s û�����ŵ�Ȩ�����������ţ��밴 Enter �������������˷���...", uid);
            pressreturn();
            clear();
        } else if (chkusermail(user)) { /*Haohamru.99.4.05 */
            prints("%s ��������,�޷�����,�밴 Enter �������������˷���...", uid);
            pressreturn();
            clear();
        } else /* �������ѷ��ŵĴ��� Bigman 2000.9.8 */ if (false == canIsend2(uid)) {  /* Leeward 98.04.10 */
            char tmp_title[STRLEN], save_title_bak[STRLEN];

            prints("[1m[33m�ܱ�Ǹ��ϵͳ�޷��� %s �������ţ���Ϊ %s �ܾ����������ż���\n\n�밴 Enter �������������˷���...[m[m\n\n", uid, uid);
            pressreturn();
            clear();
            strcpy(save_title_bak, save_title);
            sprintf(tmp_title, "���š� %s �ܾ����������ż���", uid);
            mail_file(currentuser->userid, tmpfile, currentuser->userid, tmp_title, 0);
            strcpy(save_title, save_title_bak);
        } else {
            mail_file(currentuser->userid, tmpfile, uid, save_title, 0);
        }
    }
    mail_file_sent(".group", tmpfile, currentuser->userid, save_title, 0);
    unlink(tmpfile);
    if (G_SENDMODE == 2)
        fclose(mp);
    return 0;
}

/*Add by SmallPig*/
int ov_send()
{
    int all, i;

    /*
     * ���Mail Bigman:2000.8.22 
     */
    if (HAS_PERM(currentuser, PERM_DENYMAIL))
        return DONOTHING;

    modify_user_mode(SMAIL);
    move(1, 0);
    clrtobot();
    move(2, 0);
    prints("���Ÿ����������е��ˣ�Ŀǰ��վ���ƽ����Լĸ� [1m%d[m λ��\n", maxrecp);
    if (nf <= 0) {
        prints("�㲢û���趨���ѡ�\n");
        pressanykey();
        clear();
        return 0;
    } else {
        prints("�������£�\n");
    }
    G_SENDMODE = 1;
    all = (nf >= maxrecp) ? maxrecp : nf;
    for (i = 0; i < all; i++) {
        char *userid;

        userid = getuserid2(topfriend[i].uid);
        if (!userid)
            prints("\x1b[1;32m%-12s\x1b[m ", topfriend[i].uid);
        else
            prints("%-12s ", userid);
        if ((i + 1) % 6 == 0)
            prints("\n");
    }
    pressanykey();
    switch (do_gsend(NULL, NULL, all)) {
    case -1:
        prints("�ż�Ŀ¼����\n");
        break;
    case -2:
        prints("�ż�ȡ��\n");
        break;
    case -4:
        prints("�����Ѿ������޶�\n");
        break;
    default:
        prints("�ż��Ѽĳ�\n");
    }
    pressreturn();
    G_SENDMODE = 0;
    return 0;
}

int in_group(uident, cnt)
char uident[maxrecp][STRLEN];
int cnt;
{
    int i;

    for (i = 0; i < cnt; i++)
        if (!strcmp(uident[i], uident[cnt])) {
            return i + 1;
        }
    return 0;
}

int doforward(char *direct, struct fileheader *fh, int isuu)
{
    static char address[STRLEN];
    char fname[STRLEN];
    char receiver[STRLEN];
    char title[STRLEN];
    int return_no;
    char tmp_buf[200];
    int y = 5;
    int noansi;

    clear();
    if (address[0] == '\0') {
        strncpy(address, currentuser->email, STRLEN);
        if (strstr(currentuser->email, "@bbs.zixia.net") || strstr(currentuser->email, "bbs@smth.org") || strlen(currentuser->email) == 0) {
            strcpy(address, currentuser->userid);
        }
    }

    if (chkusermail(currentuser)) {
        move(1, 0);
        prints("��������Ѿ������޶�޷�ת���ż���\n");
        pressreturn();
        return -4;
    }

    prints("��ֱ�Ӱ� Enter ������������ʾ�ĵ�ַ, ��������������ַ\n");
    prints("(��Ҫת�ŵ��Լ���BBS����,��ֱ���������ID��Ϊ��ַ����)\n");
    prints("�� %s �ġ�%s��ת�ĸ�:", fh->owner, fh->title);
    sprintf(genbuf, "[%s]: ", address);
    getdata(3, 0, genbuf, receiver, 70, DOECHO, NULL, true);
    if (receiver[0] == '\0') {
        sprintf(genbuf, "ȷ�������¼ĸ� %s ��? (Y/N) [Y]: ", address);
        getdata(3, 0, genbuf, receiver, 3, DOECHO, NULL, true);
        if (receiver[0] == 'n' || receiver[0] == 'N')
            return 1;
        strncpy(receiver, address, STRLEN);
    } else {
        strncpy(address, receiver, STRLEN);
        /*
         * ȷ�ϵ�ַ�Ƿ���ȷ added by dong, 1998.10.1 
         */
        sprintf(genbuf, "ȷ�������¼ĸ� %s ��? (Y/N) [Y]: ", address);
        getdata(3, 0, genbuf, receiver, 3, DOECHO, NULL, true);
        if (receiver[0] == 'n' || receiver[0] == 'N')
            return 1;
        strncpy(receiver, address, STRLEN);
    }
    if (invalidaddr(receiver))
        return -2;
    if (!HAS_PERM(currentuser, PERM_POST))
        if (!strstr(receiver, "@") && !strstr(receiver, ".")) {
            prints("������Ȩ��ת���ż���վ�������û���");
            pressreturn();
            return -22;
        }

    sprintf(fname, "tmp/forward/%s.%05d", currentuser->userid, getpid());
    /*
     * sprintf( tmp_buf, "cp %s/%s %s",
     * direct, fh->filename, fname);
     */
    sprintf(tmp_buf, "%s/%s", direct, fh->filename);
    f_cp(tmp_buf, fname, 0);
    sprintf(title, "%.50s(ת��)", fh->title);   /*Haohmaru.00.05.01,moved here */
    if (askyn("�Ƿ��޸���������", 0) == 1) {
        vedit(fname, false);
        y = 2;
        newbbslog(BBSLOG_USER, "�޸ı�ת�������»��ż�: %s", title);    /*Haohmaru.00.05.01 */
        /*
         * clear(); 
         */
    }


    {                           /* Leeward 98.04.27: better:-) */

        char *ptrX;

        /*
         * ptrX = strstr(receiver, ".bbs@smth.org");
         * @smth.org @zixia.net ȡ��ǰ����û����� 
         */
        ptrX = strstr(receiver, (const char *) email_domain());

        /*
         * disable by KCN      if (!ptrX) ptrX = strstr(receiver, ".bbs@"); 
         */
        if (ptrX && '@' == *(ptrX - 1))
            *(ptrX - 1) = 0;
    }

    if (!strstr(receiver, "@") && !strstr(receiver, ".")) {     /* sending local file need not uuencode or convert to big5... */
        struct userec *lookupuser;

        prints("ת���ż��� %s, ���Ժ�....\n", receiver);
        refresh();

        return_no = getuser(receiver, &lookupuser);
        if (return_no == 0) {
            return_no = 1;
            prints("ʹ�����Ҳ���...\n");
        } else {                /* �����Ӧ��ʹ��lookupuser�е�����,��֤��Сд��ȷ  period 2000-12-13 */
            strncpy(receiver, lookupuser->userid, IDLEN + 1);
            receiver[IDLEN] = 0;

            /*
             * if(!chkreceiver(receiver,NULL))Haohamru.99.4.05
             * FIXME NULL -> lookupuser���� zixia.net ������ô�ĵ�... ��û�����⣿ 
             */
            if (!HAS_PERM(currentuser, PERM_SYSOP) && lookupuser->userlevel & PERM_SUICIDE) {
                prints("%s ��ɱ�У���������\n", receiver);
                return -5;
            }
            if (!HAS_PERM(currentuser, PERM_SYSOP) && !(lookupuser->userlevel & PERM_READMAIL)) {
                prints("%s û�����ŵ�Ȩ������������\n", receiver);
                return -5;
            }


            if (!HAS_PERM(currentuser, PERM_SYSOP) && chkusermail(lookupuser)) {        /*Haohamru.99.4.05 */
                prints("%s ��������,�޷�����\n", receiver);
                return -4;
            }

            if (false == canIsend2(receiver)) { /* Leeward 98.04.10 */
                prints("[1m[33m�ܱ�Ǹ��ϵͳ�޷�ת�Ĵ��ţ���Ϊ %s �ܾ����������ż���[m[m\n\n", receiver);
                sprintf(title, "���š� %s �ܾ����������ż���", receiver);
                mail_file(currentuser->userid, fname, currentuser->userid, title, 0);
                return -4;
            }
            return_no = mail_file(currentuser->userid, fname, lookupuser->userid, title, 0);
        }
    } else {
        /*
         * Add by ming, 96.10.9 
         */
        char data[3];
        int isbig5;

        data[0] = 0;
        prints("����Ҫ���ż�ת�ĵ�̨�������� Y �� y\n");
        getdata(7, 0, "ת��BIG5��? [N]: ", data, 2, DOECHO, 0, 0);
        if (data[0] == 'y' || data[0] == 'Y')
            isbig5 = 1;
        else
            isbig5 = 0;

        getdata(8, 0, "����ANSI���Ʒ��? [Y]: ", data, 2, DOECHO, 0, 0);
        if (data[0] == 'n' || data[0] == 'N')
            noansi = 0;
        else
            noansi = 1;

        prints("ת���ż��� %s, ���Ժ�....\n", receiver);
        refresh();

        /*
         * return_no = bbs_sendmail(fname, title, receiver); 
         */

        return_no = bbs_sendmail(fname, title, receiver, isuu, isbig5, noansi);
    }

    unlink(fname);
    return (return_no);
}

#endif
struct command_def {
    char *prompt;
    int permission;
    int (*func) ();
    void *arg;
};

void t_override();

const static struct command_def mail_cmds[] = {
    {"N) �������ż�", 0, m_new, NULL},
    {"R) ����ȫ���ż�", 0, m_read, NULL},
    {"S) ����", 0, m_send, NULL},
    {"G) �ĸ� / �趨��������", 0, g_send, NULL},
    {"O)���趨��������", 0, t_override, NULL},
    {"F)�����Ÿ���������", 0, ov_send, NULL},
    {"M) ���Ÿ�������", PERM_SYSOP, mailall, NULL},
};

const static char *mail_sysbox[] = {
    ".DIR",
    ".SENT",
    ".DELETED"
};

const static char *mail_sysboxtitle[] = {
    "I)�ռ���",
    "T)������",
    "J)������",
};

struct mail_proc_arg {
    int leftpos;
    int rightpos;
    int cmdnum;
    int sysboxnum;
    int numbers;
    int tmpnum;
};

static void maillist_refresh(struct _select_def *conf)
{
    int i;

    clear();
//    docmdtitle("[�����ż�ѡ��]",
 //              "  ��ѡ��[\x1b[1;32m��\x1b[0;37m,\x1b[1;32me\x1b[0;37m] ����[\x1b[1;32m��\x1b[0;37m,\x1b[1;32mr\x1b[0;37m] ѡ��[\x1b[1;32m��\x1b[0;37m,\x1b[1;32m��\x1b[0;37m] ���[\x1b[1;32ma\x1b[0;37m] ����[\x1b[1;32mT\x1b[0;37m] ɾ��[\x1b[1;32md\x1b[0;37m]                   \x1b[m");
    docmdtitle("[�����ż�ѡ��]",
               "  ��ѡ��[\x1b[1;32m��\x1b[0;37m,\x1b[1;32me\x1b[0;37m] ����[\x1b[1;32mr\x1b[0;37m] ѡ��[\x1b[1;32m��\x1b[0;37m,\x1b[1;32m��\x1b[0;37m] �����л�[\x1b[1;32m��\x1b[m,\x1b[1;32mTab\x1b[m] ���[\x1b[1;32ma\x1b[0;37m] ����[\x1b[1;32mT\x1b[0;37m] ɾ��[\x1b[1;32md\x1b[0;37m]                   \x1b[m");

    move(2, 0);
    prints("%s", "\x1b[1;44;37m��������ѡ�����������������������������Щ��������Զ������䩤������������\x1b[m");
    for (i = 3; i < scr_lns - 1; i++) {
        good_move(i, 38);
        prints("%s", "\x1b[1;44;37m�O\x1b[m");
    }
    move(17, 0);
    prints("%s", "\x1b[1;44;37m����ϵͳԤ�������䩤��������������������\x1b[m");

    if (mail_list_t == 0) {
        good_move(14, 46);
        prints("%s", "���Զ�������");
    }
}
static void maillist_show(struct _select_def *conf, int pos)
{
    struct mail_proc_arg *arg = (struct mail_proc_arg *) conf->arg;
    char buf[80];

    if (pos <= arg->cmdnum) {
        /*
         * ��������
         */
        outs(mail_cmds[pos - 1].prompt);
    } else if (pos <= arg->sysboxnum + arg->cmdnum) {
        int sel;

        sel = pos - arg->cmdnum - 1;
        outs(mail_sysboxtitle[sel]);

        setmailfile(buf, currentuser->userid, mail_sysbox[sel]);
        prints("(%d)", getmailnum(buf));
    } else {
        /*
         * �Զ�������
         */
        int sel;
        char dirbstr[60];

        sel = pos - arg->cmdnum - arg->sysboxnum;
        if (sel < 10)
            outc(' ');
        prints("%d) %s", sel, mail_list[sel - 1]);
        sprintf(dirbstr, ".%s", mail_list[sel - 1] + 30);
        setmailfile(buf, currentuser->userid, dirbstr);
        prints("(%d)", getmailnum(buf));
    }
}

static int maillist_onselect(struct _select_def *conf)
{
    struct mail_proc_arg *arg = (struct mail_proc_arg *) conf->arg;
    char buf[20];

    if (conf->pos <= arg->cmdnum) {
        /*
         * ��������
         */
        (*mail_cmds[conf->pos - 1].func) ();
    } else if (conf->pos <= arg->sysboxnum + arg->cmdnum) {
        int sel;

        sel = conf->pos - arg->cmdnum - 1;
        setmailfile(currmaildir, currentuser->userid, mail_sysbox[sel]);
        in_mail = true;
        i_read(RMAIL, currmaildir, mailtitle, (READ_FUNC) maildoent, &mail_comms[0], sizeof(struct fileheader));
        in_mail = false;
        /*
         * ϵͳ����
         */
    } else {
        /*
         * �Զ�������
         */
        int sel;

        sel = conf->pos - arg->sysboxnum - arg->cmdnum - 1;
        sprintf(buf, ".%s", mail_list[sel]);
        setmailfile(currmaildir, currentuser->userid, buf);
        in_mail = true;
        i_read(RMAIL, currmaildir, mailtitle, (READ_FUNC) maildoent, &mail_comms[0], sizeof(struct fileheader));
        in_mail = false;
    }
    return SHOW_REFRESH;
}

static int maillist_prekey(struct _select_def *conf, int *command)
{
    struct mail_proc_arg *arg = (struct mail_proc_arg *) conf->arg;

    /*
     * �����������ҵ������
     */
    if (*command == KEY_RIGHT) {
	if (mail_list_t==0)
            *command='\n';
	else
            *command='\t';
    }
    if (*command == KEY_LEFT)
	if ((conf->pos <= arg->cmdnum + arg->sysboxnum))
            return SHOW_QUIT;
        else {
	    *command='\t';
            return SHOW_CONTINUE;
	}
    
    if (*command == 'e')
        return SHOW_QUIT;
    update_endline();
    if (!isdigit(*command))
        arg->tmpnum = -1;
    return SHOW_CONTINUE;
}

static int maillist_key(struct _select_def *conf, int command)
{
    struct mail_proc_arg *arg = (struct mail_proc_arg *) conf->arg;
    int i;

    if (command=='\t') {
        if (conf->pos<=arg->cmdnum+arg->sysboxnum) {
	/* ���*/
	    if (!mail_list_t)
                return SHOW_CONTINUE;
	    arg->leftpos=conf->pos;
	    conf->new_pos=arg->rightpos;
	} else {
	    arg->rightpos=conf->pos;
	    conf->new_pos=arg->leftpos;
	}
        return SHOW_SELCHANGE;
    }
    if (toupper(command) == 'A') {
        char bname[STRLEN], buf[PATHLEN];
        int i = 0, y, x;
        struct stat st;

        if (mail_list_t >= MAILBOARDNUM) {
            move(2, 0);
            clrtoeol();
            prints("�������Ѿ�������(%d)��", MAILBOARDNUM);
            pressreturn();
            return SHOW_REFRESH;
        }
        move(0, 0);
        clrtoeol();
        getdata(0, 0, "�����Զ���������ʾ������: ", buf, 30, DOECHO, NULL, true);
        if (buf[0] == 0) {
            return SHOW_REFRESH;
        }
        strncpy(mail_list[mail_list_t], buf, 29);
        move(0, 0);
        clrtoeol();
        while (1) {
            i++;
            sprintf(bname, ".MAILBOX%d", i);
            setmailfile(buf, currentuser->userid, bname);
            if (stat(buf, &st) == -1)
                break;
        }
        sprintf(bname, "MAILBOX%d", i);
	f_touch(buf);
        strncpy(mail_list[mail_list_t] + 30, bname, 9);
        mail_list_t++;
        save_mail_list();
        x = 0;

        y = 3 + (20 - mail_list_t) / 2;
        arg->numbers++;
        conf->item_count = arg->numbers;
        conf->item_per_page = arg->numbers;
        for (i = arg->cmdnum + arg->sysboxnum; i < arg->cmdnum + arg->sysboxnum + mail_list_t; i++) {
            conf->item_pos[i].x = 44;
            conf->item_pos[i].y = y + i - arg->cmdnum - arg->sysboxnum;
        }
        return SHOW_REFRESH;
    }
    if (toupper(command) == 'D') {
        int p = 1, i, j;
        char ans[2];
	int num,y;

	if (conf->pos<=arg->cmdnum+arg->sysboxnum)
            return SHOW_CONTINUE;
        move(0, 0);
        clrtoeol();
        getdata(0, 0, "ȷ��ɾ������Ŀ¼��(y/N)", ans, 2, DOECHO, NULL, true);
        p = ans[0] == 'Y' || ans[0] == 'y';
        if (p) {
            p = conf->pos-arg->cmdnum-arg->sysboxnum-1;
            for (j = p; j < mail_list_t - 1; j++)
                memcpy(mail_list[j], mail_list[j + 1], sizeof(mail_list[j]));
            mail_list_t--;
            save_mail_list();
            y = 3 + (20 - mail_list_t) / 2;
            arg->numbers--;
            conf->item_count = arg->numbers;
            conf->item_per_page = arg->numbers;
	    if (conf->pos>arg->numbers) conf->pos=arg->numbers;
            for (i = arg->cmdnum + arg->sysboxnum; i < arg->cmdnum + arg->sysboxnum + mail_list_t; i++) {
                conf->item_pos[i].x = 44;
                conf->item_pos[i].y = y + i - arg->cmdnum - arg->sysboxnum;
            }
            return SHOW_REFRESH;
        }
        return SHOW_REFRESH;
    }
    if (toupper(command) == 'T') {
        int p = 1, i, j;
        char bname[STRLEN];
	int num;
        char ans[2];

	if (conf->pos<=arg->cmdnum+arg->sysboxnum)
            return SHOW_CONTINUE;
        move(0, 0);
        clrtoeol();
        i = conf->pos-arg->cmdnum-arg->sysboxnum-1;
        strcpy(bname, mail_list[i]);
        getdata(0, 0, "��������������: ", bname, 30, DOECHO, NULL, false);
        if (bname[0]) {
            strcpy(mail_list[i], bname);
            save_mail_list();
            return SHOW_REFRESH;
        }
        return SHOW_REFRESH;
    }

    for (i = 0; i < arg->cmdnum; i++)
        if (toupper(command) == mail_cmds[i].prompt[0]) {
            conf->new_pos = i + 1;
            return SHOW_SELCHANGE;
        }
    for (i = 0; i < arg->sysboxnum; i++)
        if (toupper(command) == mail_sysboxtitle[i][0]) {
            conf->new_pos = i + arg->cmdnum + 1;
            return SHOW_SELCHANGE;
        }
    if (isdigit(command)) {
        int num;

        num = command - '0';
        if ((arg->tmpnum == -1) || (num + arg->tmpnum * 10 > mail_list_t)) {
            arg->tmpnum = command - '0';
            if (arg->tmpnum <= mail_list_t) {
                conf->new_pos = arg->tmpnum + arg->cmdnum + arg->sysboxnum;
                return SHOW_SELCHANGE;
            }
        } else {
            conf->new_pos = arg->tmpnum * 10 + arg->cmdnum + arg->sysboxnum + num;
            arg->tmpnum = command - '0';
            return SHOW_SELCHANGE;
        }
    }
    return SHOW_CONTINUE;
}

int MailProc()
{
    struct _select_def maillist_conf;
    struct mail_proc_arg arg;
    POINT *pts;
    int i;
    int y;

    clear();
    arg.tmpnum = -1;
    arg.cmdnum = sizeof(mail_cmds) / sizeof(struct command_def);
    if (!HAS_PERM(currentuser,PERM_SYSOP))
	    arg.cmdnum--;
    arg.sysboxnum = sizeof(mail_sysbox) / sizeof(char *);
    arg.numbers = mail_list_t + arg.cmdnum + arg.sysboxnum;
    arg.leftpos=2;
    arg.rightpos=arg.cmdnum + arg.sysboxnum+1;
    pts = (POINT *) malloc(sizeof(POINT) * (arg.numbers+20));

    /*
     * �������������λ��
     */
    y = 2 + (17 - arg.cmdnum) / 2;
    for (i = 0; i < arg.cmdnum; i++) {
        pts[i].x = 6;
        pts[i].y = y + i;
    }

    y = 18 + (5 - arg.sysboxnum) / 2;
    /*
     * ����ϵͳ�����λ��
     */
    for (; i < arg.cmdnum + arg.sysboxnum; i++) {
        pts[i].x = 6;
        pts[i].y = y + i - arg.cmdnum;
    }
    /*
     * ����ϵͳ�����λ��
     */

    y = 3 + (20 - mail_list_t) / 2;
    for (; i < arg.cmdnum + arg.sysboxnum + mail_list_t; i++) {
        pts[i].x = 44;
        pts[i].y = y + i - arg.cmdnum - arg.sysboxnum;
    }
    bzero((char *) &maillist_conf, sizeof(struct _select_def));
    maillist_conf.item_count = arg.numbers;
    maillist_conf.item_per_page = arg.numbers;
    maillist_conf.flag = LF_BELL | LF_LOOP;     //|LF_HILIGHTSEL;
    maillist_conf.prompt = "��";
    maillist_conf.item_pos = pts;
    maillist_conf.arg = &arg;
    maillist_conf.title_pos.x = 1;
    maillist_conf.title_pos.y = 6;
    maillist_conf.pos = 2;

    maillist_conf.on_select = maillist_onselect;
    maillist_conf.show_data = maillist_show;
    maillist_conf.pre_key_command = maillist_prekey;
    maillist_conf.key_command = maillist_key;
    maillist_conf.show_title = maillist_refresh;

    list_select_loop(&maillist_conf);
    free(pts);
}
