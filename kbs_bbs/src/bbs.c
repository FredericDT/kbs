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
/* ���� ��ע�� �� Alex&Sissi ��� �� alex@mars.net.edu.cn */

#include "bbs.h"
#include <time.h>

/*#include "../SMTH2000/cache/cache.h"*/

extern int numofsig;
int scrint = 0;
int local_article;
int readpost;
int digestmode;
int usernum;
char currboard[STRLEN - BM_LEN];
char currBM[BM_LEN - 1];
int selboard = 0;

char ReadPost[STRLEN] = "";
char ReplyPost[STRLEN] = "";
struct fileheader ReadPostHeader;
int FFLL = 0;
int Anony;
char genbuf[1024];
char quote_title[120], quote_board[120];
char quote_user[120];
struct friends_info *topfriend;
extern char currdirect[255];

#ifndef NOREPLY
char replytitle[STRLEN];
#endif

char *filemargin();

/*For read.c*/
int change_post_flag(int ent, struct fileheader *fileinfo, char *direct, int flag, int prompt);

/* bad 2002.8.1 */

int auth_search_down();
int auth_search_up();
int t_search_down();
int t_search_up();
int post_search_down();
int post_search_up();
int thread_up();
int thread_down();
int deny_user();

/*int     b_jury_edit();  stephen 2001.11.1*/
int add_author_friend();
int m_read();                   /*Haohmaru.2000.2.25 */
int SR_first_new();
int SR_last();
int SR_first();
int SR_read();
int SR_readX();                 /* Leeward 98.10.03 */
int SR_author();
int SR_authorX();               /* Leeward 98.10.03 */
int SR_BMfunc();
int SR_BMfuncX();               /* Leeward 98.04.16 */
int Goodbye();
int i_read_mail();              /* period 2000.11.12 */

void RemoveAppendedSpace();     /* Leeward 98.02.13 */
int set_delete_mark(int ent, struct fileheader *fileinfo, char *direct);        /* KCN */

extern time_t login_start_time;
extern int cmpbnames();
extern int B_to_b;

extern struct screenline *big_picture;
extern struct userec *user_data;

int check_readonly(char *checked)
{                               /* Leeward 98.03.28 */
    if (checkreadonly(checked)) {       /* Checking if DIR access mode is "555" */
        if (currboard == checked) {
            move(0, 0);
            clrtobot();
            move(8, 0);
            prints("                                        "); /* 40 spaces */
            move(8, (80 - (24 + strlen(checked))) / 2); /* Set text in center */
            prints("[1m[33m�ܱ�Ǹ��[31m%s ��Ŀǰ��ֻ��ģʽ[33m\n\n                          �������ڸð淢������޸�����[0m\n", checked);
            pressreturn();
            clear();
        }
        return true;
    } else
        return false;
}

/* undelete һƪ���� Leeward 98.05.18 */
/* modified by ylsdd */
int UndeleteArticle(int ent, struct fileheader *fileinfo, char *direct)
{
    char *p, buf[1024];
    char UTitle[128];
    struct fileheader UFile;
    int i;
    FILE *fp;

    if (digestmode != 4 && digestmode != 5)
        return DONOTHING;
    if (!chk_currBM(currBM, currentuser))
        return DONOTHING;

    sprintf(buf, "boards/%s/%s", currboard, fileinfo->filename);
    if (!dashf(buf)) {
        clear();
        move(2, 0);
        prints("�����²����ڣ��ѱ��ָ�, ɾ�����б����");
        pressreturn();
        return FULLUPDATE;
    }
    fp = fopen(buf, "r");
    if (!fp)
        return DONOTHING;


    strcpy(UTitle, fileinfo->title);
    if ((p = strrchr(UTitle, '-')) != NULL) {   /* create default article title */
        *p = 0;
        for (i = strlen(UTitle) - 1; i >= 0; i--) {
            if (UTitle[i] != ' ')
                break;
            else
                UTitle[i] = 0;
        }
    }

    i = 0;
    while (!feof(fp) && i < 2) {
        fgets(buf, 1024, fp);
        if (feof(fp))
            break;
        if (strstr(buf, "������: ") && strstr(buf, "), ����: ")) {
            i++;
        } else if (strstr(buf, "��  ��: ")) {
            i++;
            strcpy(UTitle, buf + 8);
            if ((p = strchr(UTitle, '\n')) != NULL)
                *p = 0;
        }
    }
    fclose(fp);

    bzero(&UFile, sizeof(UFile));
    strcpy(UFile.owner, fileinfo->owner);
    strcpy(UFile.title, UTitle);
    strcpy(UFile.filename, fileinfo->filename);

    sprintf(buf, "boards/%s/.DIR", currboard);
    append_record(buf, &UFile, sizeof(UFile));
    updatelastpost(currboard);
    fileinfo->filename[0] = '\0';
    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    sprintf(buf, "undeleted %s's ��%s�� on %s", UFile.owner, UFile.title, currboard);
    report(buf);

    clear();
    move(2, 0);
    prints("'%s' �ѻָ������� \n", UFile.title);
    pressreturn();

    return FULLUPDATE;
}

int check_stuffmode()
{
    if (uinfo.mode == RMAIL)
        return true;
    else
        return false;
}

/*Add by SmallPig*/
void setqtitle(char *stitle)
{                               /* ȡ Reply ���º��µ� ����title */
    FFLL = 1;
    if (strncmp(stitle, "Re: ", 4) != 0 && strncmp(stitle, "RE: ", 4) != 0) {
        sprintf(ReplyPost, "Re: %s", stitle);
        strncpy(ReadPost, stitle, STRLEN - 1);
        ReadPost[STRLEN - 1] = 0;
    } else {
        strncpy(ReplyPost, stitle, STRLEN - 1);
        strncpy(ReadPost, ReplyPost + 4, STRLEN - 1);
        ReplyPost[STRLEN - 1] = 0;
        ReadPost[STRLEN - 1] = 0;
    }
}

/*Add by SmallPig*/
int shownotepad()
{                               /* ��ʾ notepad */
    modify_user_mode(NOTEPAD);
    ansimore("etc/notepad", true);
    clear();
    return 1;
}

void printutitle()
{                               /* ��Ļ��ʾ �û��б� title */
    /*---	modified by period	2000-11-02	hide posts/logins	---*/
#ifndef _DETAIL_UINFO_
    int isadm;
    const char *fmtadm = "#��վ #����", *fmtcom = "           ";

    isadm = HAS_PERM(currentuser, PERM_ADMINMENU);
#endif

    move(2, 0);
    prints(
#ifdef _DETAIL_UINFO_
              "[44m �� ��  ʹ���ߴ���     %-19s  #��վ #���� %4s    �����������   [m\n",
#else
              "[44m �� ��  ʹ���ߴ���     %-19s  %11s %4s    �����������   [m\n",
#endif
#if defined(ACTS_REALNAMES)
              "��ʵ����",
#else
              "ʹ�����ǳ�",
#endif
#ifndef _DETAIL_UINFO_
              isadm ? fmtadm : fmtcom,
#endif
              "���");
}


int g_board_names(struct boardheader *fhdrp)
{
    if ((fhdrp->level & PERM_POSTMASK) || HAS_PERM(currentuser, fhdrp->level)
        || (fhdrp->level & PERM_NOZAP)) {
        AddNameList(fhdrp->filename);
    }
    return 0;
}

void make_blist()
{                               /* ���а� ���� �б� */
    CreateNameList();
    apply_boards((int (*)()) g_board_names);
}

int Select()
{
    modify_user_mode(SELECT);
    do_select(0, NULL, genbuf);
    return 0;
}

int Post()
{                               /* ���˵��ڵ� �ڵ�ǰ�� POST ���� */
    if (!selboard) {
        prints("\n\n���� (S)elect ȥѡ��һ����������\n");
        pressreturn();          /* �ȴ���return�� */
        clear();
        return 0;
    }
#ifndef NOREPLY
    *replytitle = '\0';
#endif
    do_post();
    return 0;
}

int get_a_boardname(char *bname, char *prompt)
{                               /* ����һ������ */
    /*
     * struct boardheader fh; 
     */

    make_blist();
    namecomplete(prompt, bname);        /* �����Զ����� */
    if (*bname == '\0') {
        return 0;
    }
    /*---	Modified by period	2000-10-29	---*/
    if (getbnum(bname) <= 0)
        /*---	---*/
    {
        move(1, 0);
        prints("���������������\n");
        pressreturn();
        move(1, 0);
        return 0;
    }
    return 1;
}

/* Add by SmallPig */
int do_cross(int ent, struct fileheader *fileinfo, char *direct)
{                               /* ת�� һƪ ���� */
    char bname[STRLEN];
    char dbname[STRLEN];
    char ispost[10];
    char q_file[STRLEN];

    if (!HAS_PERM(currentuser, PERM_POST)) {    /* �ж��Ƿ���POSTȨ */
        return DONOTHING;
    }

    if ((fileinfo->accessed[0] & FILE_FORWARDED) && !HAS_PERM(currentuser, PERM_SYSOP)) {
        clear();
        move(1, 0);
        prints("�������Ѿ�ת����һ�Σ��޷��ٴ�ת��");
        move(2, 0);
        pressreturn();
        return FULLUPDATE;
    }

    if (uinfo.mode != RMAIL)
        sprintf(q_file, "boards/%s/%s", currboard, fileinfo->filename);
    else
        setmailfile(q_file, currentuser->userid, fileinfo->filename);
    strcpy(quote_title, fileinfo->title);

    clear();
    move(4, 0);                 /* Leeward 98.02.25 */
    prints
        ("[1m[33m��ע�⣺[31m��վվ��涨��ͬ�����ݵ������Ͻ��� 5 (��) ���������������ظ�������\n\nΥ����[33m���������»ᱻɾ��֮�⣬������[31m��������������µ�Ȩ����[33m��ϸ�涨����գ�\n\n    Announce ���վ�棺������ת�����������µĹ涨����\n\n���ҹ�ͬά�� BBS �Ļ�������ʡϵͳ��Դ��лл������\n\n[0m");
    move(1, 0);
    if (!get_a_boardname(bname, "������Ҫת��������������: ")) {
        return FULLUPDATE;
    }
    /*
     * if (!strcmp(bname,currboard))Haohmaru,98.10.03 
     */
    if (!strcmp(bname, currboard) && (uinfo.mode != RMAIL)) {
        move(3, 0);
        clrtobot();
        prints("\n\n                          ��������²���Ҫת��������!");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    {                           /* Leeward 98.01.13 ���ת����������ת���İ����Ƿ񱻽�ֹ�� POST Ȩ */
        char szTemp[STRLEN];

        strcpy(szTemp, currboard);      /* ���浱ǰ���� */
        strcpy(currboard, bname);       /* ���õ�ǰ����ΪҪת�����İ��� */
        if (deny_me(currentuser->userid, currboard) && !HAS_PERM(currentuser, PERM_SYSOP)) {    /* ������ֹPOST ��� */
            move(3, 0);
            clrtobot();
            prints("\n\n                �ܱ�Ǹ�����ڸð汻�����ֹͣ�� POST ��Ȩ��...\n");
            pressreturn();
            clear();
            strcpy(currboard, szTemp);  /* �ָ���ǰ���� */
            return FULLUPDATE;
        } else if (true == check_readonly(currboard)) { /* Leeward 98.03.28 */
            strcpy(currboard, szTemp);  /* �ָ���ǰ���� */
            return FULLUPDATE;
        } else
            strcpy(currboard, szTemp);  /* �ָ���ǰ���� */
    }

    move(0, 0);
    clrtoeol();
    prints("ת�� ' %s ' �� %s �� ", quote_title, bname);
    move(1, 0);
    getdata(1, 0, "(S)ת�� (L)��վ (A)ȡ��? [A]: ", ispost, 9, DOECHO, NULL, true);
    if (ispost[0] == 's' || ispost[0] == 'S' || ispost[0] == 'L' || ispost[0] == 'l') {
        strcpy(quote_board, currboard);
        strcpy(dbname, currboard);
        strcpy(currboard, bname);
        if (post_cross(currentuser, currboard, quote_board, quote_title, q_file, Anony, in_mail, ispost[0], 0) == -1) { /* ת�� */
            pressreturn();
            move(2, 0);
            strcpy(currboard, dbname);
            return FULLUPDATE;
        }
        strcpy(currboard, dbname);
        move(2, 0);
        prints("' %s ' ��ת���� %s �� \n", quote_title, bname);
        fileinfo->accessed[0] |= FILE_FORWARDED;        /*added by alex, 96.10.3 */
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    } else {
        prints("ȡ��");
    }
    move(2, 0);
    pressreturn();
    return FULLUPDATE;
}


void readtitle()
{                               /* ���� ��ʾ�����б� �� title */
    struct boardheader *bp;
    char header[STRLEN], title[STRLEN];
    char readmode[10];
    int chkmailflag = 0;

    bp = getbcache(currboard);
    memcpy(currBM, bp->BM, BM_LEN - 1);
    if (currBM[0] == '\0' || currBM[0] == ' ') {
        strcpy(header, "����������");
    } else {
        if (HAS_PERM(currentuser, PERM_OBOARDS)) {
            char *p1, *p2;

            strcpy(header, "����: ");
            p1 = currBM;
            p2 = p1;
            while (1) {
                if ((*p2 == ' ') || (*p2 == 0)) {
                    int end;

                    end = 0;
                    if (p1 == p2) {
                        if (*p2 == 0)
                            break;
                        p1++;
                        p2++;
                        continue;
                    }
                    if (*p2 == 0)
                        end = 1;
                    *p2 = 0;
                    if (apply_utmp(NULL, 1, p1, NULL)) {
                        sprintf(genbuf, "\x1b[32m%s\x1b[33m ", p1);
                        strcat(header, genbuf);
                    } else {
                        strcat(header, p1);
                        strcat(header, " ");
                    }
                    if (end)
                        break;
                    p1 = p2 + 1;
                    *p2 = ' ';
                }
                p2++;
            }
        } else {
            sprintf(header, "����: %s", currBM);
        }
    }
    chkmailflag = chkmail();
    if (chkmailflag == 2)       /*Haohmaru.99.4.4.������Ҳ������ */
        strcpy(title, "[�������䳬������,����������!]");
    else if (chkmailflag)       /* �ż���� */
        strcpy(title, "[�����ż�]");
    else if ((bp->flag & BOARD_VOTEFLAG))       /* ͶƱ��� */
        sprintf(title, "ͶƱ�У��� V ����ͶƱ");
    else
        strcpy(title, bp->title + 13);

    showtitle(header, title);   /* ��ʾ ��һ�� */
    prints("�뿪[��,e] ѡ��[��,��] �Ķ�[��,r] ��������[Ctrl-P] ����[d] ����¼[TAB] ����[h][m\n");
    if (digestmode == 0)        /* �Ķ�ģʽ */
        strcpy(readmode, "һ��");
    else if (digestmode == 1)
        strcpy(readmode, "��ժ");
    else if (digestmode == 2)
        strcpy(readmode, "����");
    else if (digestmode == 3)
        strcpy(readmode, "����");
    else if (digestmode == 4)
        strcpy(readmode, "����");
    else if (digestmode == 5)
        strcpy(readmode, "ֽ¦");
    else if (digestmode == 6)
        strcpy(readmode, "ԭ��");
    else if (digestmode == 7)
        strcpy(readmode, "����");
    else if (digestmode == 8)
        strcpy(readmode, "����");

    prints("[37m[44m ���   %-12s %6s %-40s[%4sģʽ] [m\n", "�� �� ��", "��  ��", " ���±���", readmode);
    clrtobot();
}

char *readdoent(char *buf, int num, struct fileheader *ent)
{                               /* �������б��� ��ʾ һƪ���±��� */
    time_t filetime;
    char date[20];
    char *TITLE;
    int type;
    int manager;
    char cUnreadMark = (DEFINE(currentuser, DEF_UNREADMARK) ? '*' : 'N');
    char *typeprefix;
    char *typesufix;

    typesufix = typeprefix = "";

    manager = (HAS_PERM(currentuser, PERM_OBOARDS) || (chk_currBM(currBM, currentuser)));

    type = brc_unread(FILENAME2POSTTIME(ent->filename)) ? cUnreadMark : ' ';
    if ((ent->accessed[0] & FILE_DIGEST) /*&& HAS_PERM(currentuser,PERM_MARKPOST) */ ) {        /* ��ժģʽ �ж� */
        if (type == ' ')
            type = 'g';
        else
            type = 'G';
    }
    if (ent->accessed[0] & FILE_MARKED) {       /* ����ļ���markס�ˣ��ı��ʶ */
        switch (type) {
        case ' ':
            type = 'm';
            break;
        case '*':
        case 'N':
            type = 'M';
            break;
        case 'g':
            type = 'b';
            break;
        case 'G':
            type = 'B';
            break;
        }
    }
/*    if(HAS_PERM(currentuser,PERM_OBOARDS) && ent->accessed[1] & FILE_READ) *//*
 * * �����ܹ����ϵ��ܿ�����re��־,Haohmaru.99.6.7 
 */
    if (manager & ent->accessed[1] & FILE_READ) {       /* ���������ܿ�����re��־, Bigman.2001.2.27 */
        switch (type) {
        case 'g':
#ifdef _DEBUG_
            type = 'o';
            break;
#endif                          /* _DEBUG_ */
        case 'G':
            type = 'O';
            break;
        case 'm':
#ifdef _DEBUG_
            type = 'u';
            break;
#endif                          /* _DEBUG_ */
        case 'M':
            type = 'U';
            break;
        case 'b':
#ifdef _DEBUG_
            type = 'd';
            break;
#endif                          /* _DEBUG_ */
        case 'B':
            type = '8';
            break;
        case ' ':
#ifdef _DEBUG_
            type = ',';
            break;
#endif                          /* _DEBUG_ */
        case '*':
        case 'N':
        default:
            type = ';';
            break;
        }
    } else if (HAS_PERM(currentuser, PERM_OBOARDS) && ent->accessed[0] & FILE_SIGN)
        /*
         * �����ܹ����ϵ��ܿ�Sign��־, Bigman: 2000.8.12 
         */
    {
        type = '#';
    }

    if (manager && ent->accessed[1] & FILE_DEL) {       /* ����ļ���mark deleteס�ˣ���ʾX */
        type = 'X';
    }

    if (manager && (ent->accessed[0] & FILE_IMPORTED)) {        /* �ļ��Ѿ������뾫���� */
        if (type == ' ') {
            typeprefix = "\x1b[42m";
            typesufix = "\x1b[m";
        } else {
            typeprefix = "\x1b[32m";
            typesufix = "\x1b[m";
        }
    }
    filetime = atoi(ent->filename + 2); /* ���ļ���ȡ��ʱ�� */
    if (filetime > 740000000) {
        /*
         * add by KCN
         * char* datestr = ctime( &filetime ) + 4;
         */
        strncpy(date, ctime(&filetime) + 4, 6);
        /*
         * strcpy(date,"[0m[m      [0m");
         * strncpy(date+7,datestr,6);
         */
        /*
         * date[5]='1'+(atoi(datestr+4)%7);
         * if (date[5]=='2') date[5]='7';
         */
    }
    /*
     * date = ctime( &filetime ) + 4;   ʱ�� -> Ӣ�� 
     */
    else
        /*
         * date = ""; char *���ͱ���, ���ܴ���, modified by dong, 1998.9.19 
         */
        /*
         * { date = ctime( &filetime ) + 4; date = ""; } 
         */
        date[0] = 0;

    /*
     * Re-Write By Excellent 
     */

    TITLE = ent->title;         /*���±���TITLE */

    if (uinfo.mode != RMAIL&&digestmode!=1) { // �·����Ƚ�
	if (FFLL == 0) {
        if (ent->groupid!=ent->id)       /*Re������ */
            sprintf(buf, " %4d %s%c%s %-12.12s %6.6s  %-47.47s ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
        else                    /* ��Re������ */
            sprintf(buf, " %4d %s%c%s %-12.12s %6.6s  �� %-44.44s ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
    } else {                    /* ���� ��ͬ�����ʶ */
        if (ent->groupid!=ent->id) { /*Re������ */
            if (ReadPostHeader.groupid==ent->groupid) /* ��ǰ�Ķ����� ��ʶ */
                sprintf(buf, " [36m%4d[m %s%c%s %-12.12s %6.6s[36m��%-47.47s[m ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
            else
                sprintf(buf, " %4d %s%c%s %-12.12s %6.6s  %-47.47s", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
        } else {
            if (ReadPostHeader.groupid==ent->groupid)      /* ��ǰ�Ķ����� ��ʶ */
                sprintf(buf, " [33m%4d[m %s%c%s %-12.12s %6.6s[33m���� %-44.44s[m ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
            else
                sprintf(buf, " %4d %s%c%s %-12.12s %6.6s  �� %-44.44s ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
        }
    	
    }
    }
    else if (FFLL == 0) {   // �ɷ����Ƚ�
        if (!strncmp("Re:", ent->title, 3))       /*Re������ */
            sprintf(buf, " %4d %s%c%s %-12.12s %6.6s  %-47.47s ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
        else                    /* ��Re������ */
            sprintf(buf, " %4d %s%c%s %-12.12s %6.6s  �� %-44.44s ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
    } else {                    /* ���� ��ͬ�����ʶ */
        if (!strncmp("Re:", ent->title, 3)) { /*Re������ */
            if (!strcmp(ReplyPost + 3, ent->title + 3)) /* ��ǰ�Ķ����� ��ʶ */
                sprintf(buf, " [36m%4d[m %s%c%s %-12.12s %6.6s[36m��%-47.47s[m ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
            else
                sprintf(buf, " %4d %s%c%s %-12.12s %6.6s  %-47.47s", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
        } else {
            if (strcmp(ReadPost, ent->title) == 0)      /* ��ǰ�Ķ����� ��ʶ */
                sprintf(buf, " [33m%4d[m %s%c%s %-12.12s %6.6s[33m���� %-44.44s[m ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
            else
                sprintf(buf, " %4d %s%c%s %-12.12s %6.6s  �� %-44.44s ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
        }
    }
    return buf;
}

int add_author_friend(int ent, struct fileheader *fileinfo, char *direct)
{
    if (!strcmp("guest", currentuser->userid))
        return DONOTHING;;

    if (!strcmp(fileinfo->owner, "Anonymous") || !strcmp(fileinfo->owner, "deliver"))
        return DONOTHING;
    else {
        clear();
        addtooverride(fileinfo->owner);
    }
    return FULLUPDATE;
}
extern int zsend_file(char *filename, char *title);
int zsend_post(int ent, struct fileheader *fileinfo, char *direct)
{
    char *t;
    char buf1[512];

    strcpy(buf1, direct);
    if ((t = strrchr(buf1, '/')) != NULL)
        *t = '\0';
    snprintf(genbuf, 512, "%s/%s", buf1, fileinfo->filename);
    return zsend_file(genbuf, fileinfo->title);
}
int read_post(int ent, struct fileheader *fileinfo, char *direct)
{
    char *t;
    char buf[512];
    int ch;
    int cou;

    clear();
    strcpy(buf, direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
    sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
/*
    strcpy(quote_file, genbuf);
*/
    strcpy(quote_board, currboard);
    strncpy(quote_title, fileinfo->title, 118);
/*    quote_file[119] = fileinfo->filename[STRLEN - 2];
*/
    strncpy(quote_user, fileinfo->owner, IDLEN);
    quote_user[IDLEN] = 0;

#ifndef NOREPLY
    ch = ansimore_withzmodem(genbuf, false, fileinfo->title);   /* ��ʾ�������� */
#else
    ch = ansimore_withzmodem(genbuf, true, fileinfo->title);    /* ��ʾ�������� */
#endif
    brc_add_read(fileinfo->filename);
#ifndef NOREPLY
    move(t_lines - 1, 0);
    clrtoeol();                 /* ��������β */
    if (haspostperm(currentuser, currboard)) {  /* �����Ƿ���POSTȨ ��ʾ����һ�� */
        prints("[44m[31m[�Ķ�����] [33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,���������Ķ� ^X��p [m");
    } else {
        prints("[44m[31m[�Ķ�����]  [33m���� Q,�� ����һ�� ������һ�� <Space>,<Enter>,���������Ķ� ^X �� p [m");
    }

    FFLL = 1;                   /* ReplyPost��ΪReply�����������ReadPostΪȥ��Re:�������� */
    if (!strncmp(fileinfo->title, "Re:", 3)) {
        strcpy(ReplyPost, fileinfo->title);
        for (cou = 0; cou < STRLEN; cou++)
            ReadPost[cou] = ReplyPost[cou + 4];
    } else if (!strncmp(fileinfo->title, "�� ", 3) || !strncmp(fileinfo->title, "�� ", 3)) {
        strcpy(ReplyPost, "Re: ");
        strncat(ReplyPost, fileinfo->title + 3, STRLEN - 4);
        for (cou = 0; cou < STRLEN; cou++)
            ReadPost[cou] = ReplyPost[cou + 4];
    } else {
        strcpy(ReplyPost, "Re: ");
        strncat(ReplyPost, fileinfo->title, STRLEN - 4);
        strncpy(ReadPost, fileinfo->title, STRLEN - 1);
        ReadPost[STRLEN - 1] = 0;
    }

    refresh();
/* sleep(1); *//*
 * * ????? 
 */
    if (!(ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_PGUP))
        ch = igetkey();

    switch (ch) {
    case Ctrl('Z'):
        r_lastmsg();            /* Leeward 98.07.30 support msgX */
        break;
    case Ctrl('Y'):
        zsend_post(ent, fileinfo, direct);
        break;
    case 'N':
    case 'Q':
    case 'n':
    case 'q':
    case KEY_LEFT:
        break;
    case KEY_REFRESH:
        break;
    case ' ':
    case 'j':
    case KEY_RIGHT:
    case KEY_DOWN:
    case KEY_PGDN:
        return READ_NEXT;
    case KEY_UP:
    case KEY_PGUP:
        return READ_PREV;
    case 'Y':
    case 'R':
    case 'y':
    case 'r':
        if ((!strcmp(currboard, "News")) || (!strcmp(currboard, "Original"))) {
            clear();
            move(3, 0);
            clrtobot();
            prints("\n\n                    �ܱ�Ǹ���ð���ܷ�������,���ܻ�����...\n");
            pressreturn();
            break;              /*Haohmaru.98.12.19,���ܻ����µİ� */
        }
        if (fileinfo->accessed[1] & FILE_READ) {        /*Haohmaru.99.01.01.���²���re */
            clear();
            move(3, 0);
            prints("\n\n            �ܱ�Ǹ�������Ѿ�����Ϊ����reģʽ,�벻Ҫ��ͼ���۱���...\n");
            pressreturn();
            break;
        }
        do_reply(fileinfo);
        break;
    case Ctrl('R'):
        post_reply(ent, fileinfo, direct);      /* ������ */
        break;
    case 'g':
        digest_post(ent, fileinfo, direct);     /* ��ժģʽ */
        break;
    case 'M':
        mark_post(ent, fileinfo, direct);       /* Leeward 99.03.02 */
        break;
    case Ctrl('U'):
        sread(0, 1, NULL /*ent */ , 1, fileinfo);       /* Leeward 98.10.03 */
        break;
    case Ctrl('H'):
        sread(-1003, 1, NULL /*ent */ , 1, fileinfo);
        break;
    case Ctrl('N'):
        sread(2, 0, ent, 0, fileinfo);
        sread(3, 0, ent, 0, fileinfo);
        sread(0, 1, ent, 0, fileinfo);
        break;
    case Ctrl('S'):
    case 'p':                  /*Add by SmallPig */
        sread(0, 0, ent, 0, fileinfo);
        break;
    case Ctrl('X'):            /* Leeward 98.10.03 */
        sread(-1003, 0, ent, 0, fileinfo);
        break;
    case Ctrl('Q'):            /*Haohmaru.98.12.05,ϵͳ����Աֱ�Ӳ��������� */
        clear();
        show_authorinfo(0, fileinfo, '\0');
        return READ_NEXT;
        break;
    case Ctrl('W'):            /*cityhunter 00.10.18�쿴������Ϣ */
        clear();
        show_authorBM(0, fileinfo, '\0');
        return READ_NEXT;
        break;
    case Ctrl('O'):
        clear();
        add_author_friend(0, fileinfo, '\0');
        return READ_NEXT;
    case 'Z':
    case 'z':
        if (!HAS_PERM(currentuser, PERM_PAGE))
            break;
        sendmsgtoauthor(0, fileinfo, '\0');
        return READ_NEXT;
        break;
    case Ctrl('A'):            /*Add by SmallPig */
        clear();
        show_author(0, fileinfo, '\0');
        return READ_NEXT;
        break;
    case 'L':
    case 'l':                  /* Luzi 1997.11.1 */
        if (uinfo.mode != LOOKMSGS) {
            show_allmsgs();
            break;
        } else
            return DONOTHING;
    case '!':                  /*Haohmaru 98.09.24 */
        Goodbye();
        break;
    case 'H':                  /* Luzi 1997.11.1 */
        r_lastmsg();
        break;
    case 'w':                  /* Luzi 1997.11.1 */
        if (!HAS_PERM(currentuser, PERM_PAGE))
            break;
        s_msg();
        break;
    case 'O':
    case 'o':                  /* Luzi 1997.11.1 */
        if (!HAS_PERM(currentuser, PERM_BASIC))
            break;
        t_friends();
        break;
    case 'u':                  /* Haohmaru 1999.11.28 */
        clear();
        modify_user_mode(QUERY);
        t_query(NULL);
        break;
    }
#endif
    return FULLUPDATE;
}

int skip_post(int ent, struct fileheader *fileinfo, char *direct)
{
    brc_add_read(fileinfo->filename);
    return GOTO_NEXT;
}

int do_select(int ent, struct fileheader *fileinfo, char *direct)
        /*
         * ������������ ѡ�������� 
         */
{
    char bname[STRLEN], bpath[STRLEN];
    struct stat st;

    move(0, 0);
    clrtoeol();
    prints("ѡ��һ�������� (Ӣ����ĸ��Сд�Կ�)\n");
    prints("������������ (���հ׼��Զ���Ѱ): ");
    clrtoeol();

    make_blist();               /* ��������Board�� �б� */
    namecomplete((char *) NULL, bname); /* ��ʾ���� board �� */
    setbpath(bpath, bname);
    if ((*bname == '\0') || (stat(bpath, &st) == -1)) { /* �ж�board�Ƿ���� */
        move(2, 0);
        prints("����ȷ��������.\n");
        pressreturn();
        return FULLUPDATE;
    }
    if (!(st.st_mode & S_IFDIR)) {
        move(2, 0);
        prints("����ȷ��������.\n");
        pressreturn();
        return FULLUPDATE;
    }

    selboard = 1;
    brc_initial(currentuser->userid, bname);

    move(0, 0);
    clrtoeol();
    move(1, 0);
    clrtoeol();
    if (digestmode != false && digestmode != true)
        digestmode = false;
    setbdir(digestmode, direct, currboard);     /* direct �趨 Ϊ ��ǰboardĿ¼ */
    return NEWDIRECT;
}

int digest_mode()
{                               /* ��ժģʽ �л� */
    if (digestmode == true) {
        digestmode = false;
        setbdir(digestmode, currdirect, currboard);
    } else {
        digestmode = true;
        setbdir(digestmode, currdirect, currboard);
        if (!dashf(currdirect)) {
            digestmode = false;
            setbdir(digestmode, currdirect, currboard);
            return FULLUPDATE;
        }
    }
    return NEWDIRECT;
}

/*stephen : check whether current useris in the list of "jury" 2001.11.1*/
int isJury()
{
    char buf[STRLEN];

    if (!HAS_PERM(currentuser, PERM_JURY))
        return 0;
    makevdir(currboard);
    setvfile(buf, currboard, "jury");
    return seek_in_file(buf, currentuser->userid);
}

int deleted_mode()
{

/* Allow user in file "jury" to see deleted area. stephen 2001.11.1 */
    if (!chk_currBM(currBM, currentuser) && !isJury()) {
        return DONOTHING;
    }
    if (digestmode == 4) {
        digestmode = false;
        setbdir(digestmode, currdirect, currboard);
    } else {
        digestmode = 4;
        setbdir(digestmode, currdirect, currboard);
        if (!dashf(currdirect)) {
            digestmode = false;
            setbdir(digestmode, currdirect, currboard);
            return DONOTHING;
        }
    }
    return NEWDIRECT;
}

int generate_mark()
{
    struct fileheader mkpost;
    struct flock ldata, ldata2;
    int fd, fd2, size = sizeof(fileheader), total, i, count = 0;
    char olddirect[PATHLEN];
    char *ptr, *ptr1;
    struct stat buf;

    digestmode = 0;
    setbdir(digestmode, olddirect, currboard);
    digestmode = 3;
    setbdir(digestmode, currdirect, currboard);
    if ((fd = open(currdirect, O_WRONLY | O_CREAT, 0664)) == -1) {
        report("recopen err");
        return -1;              // �����ļ���������
    }
    ldata.l_type = F_WRLCK;
    ldata.l_whence = 0;
    ldata.l_len = 0;
    ldata.l_start = 0;
    if (fcntl(fd, F_SETLKW, &ldata) == -1) {
        report("reclock err");
        close(fd);
        return -1;              // lock error
    }
    // ��ʼ�������
    if (!setboardmark(currboard, -1)) {
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return -1;
    }

    if ((fd2 = open(olddirect, O_RDONLY, 0664)) == -1) {
        report("recopen err");
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return -1;
    }
    fstat(fd2, &buf);
    ldata2.l_type = F_RDLCK;
    ldata2.l_whence = 0;
    ldata2.l_len = 0;
    ldata2.l_start = 0;
    fcntl(fd2, F_SETLKW, &ldata2);
    total = buf.st_size / size;

    if ((i = safe_mmapfile_handle(fd2, O_RDONLY, PROT_READ, MAP_SHARED, (void **) &ptr, &buf.st_size)) != 1) {
        if (i == 2)
            end_mmapfile((void *) ptr, buf.st_size, -1);
        ldata2.l_type = F_UNLCK;
        fcntl(fd2, F_SETLKW, &ldata2);
        close(fd2);
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return -1;
    }
    ptr1 = ptr;
    for (i = 0; i < total; i++) {
        memcpy(&mkpost, ptr1, size);
        if (mkpost.accessed[0] & FILE_MARKED) {
            write(fd, &mkpost, size);
            count++;
        }
        ptr1 += size;
    }
    end_mmapfile((void *) ptr, buf.st_size, -1);
    ldata2.l_type = F_UNLCK;
    fcntl(fd2, F_SETLKW, &ldata2);
    close(fd2);
    ftruncate(fd, count * size);

    setboardmark(currboard, 0); // ���flag

    ldata.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &ldata);        // �˳���������
    close(fd);
    return 0;
}

static int simple_digest(char* str,int maxlen)
{
	char x[sizeof(int)];
	char *p;
	bzero(x,sizeof(int));
	for (p=str;*p&&((p-str)<maxlen);p++)
		x[(p-str)%sizeof(int)]+=*p;
	return (int)x;
}

int generate_title()
{
    struct fileheader mkpost, *ptr1, *ptr2;
    struct flock ldata, ldata2;
    int fd, fd2, size = sizeof(fileheader), total, i, j, count = 0, hasht;
    char olddirect[PATHLEN];
    char *ptr, *t;
    struct hashstruct {
    	int index, data;
    } *hashtable;
    int *index, *next;
    struct stat buf;

    digestmode = 0;
    setbdir(digestmode, olddirect, currboard);
    digestmode = 2;
    setbdir(digestmode, currdirect, currboard);
    if ((fd = open(currdirect, O_WRONLY | O_CREAT, 0664)) == -1) {
        report("recopen err");
        return -1;              // �����ļ���������
    }
    ldata.l_type = F_WRLCK;
    ldata.l_whence = 0;
    ldata.l_len = 0;
    ldata.l_start = 0;
    if (fcntl(fd, F_SETLKW, &ldata) == -1) {
        report("reclock err");
        close(fd);
        return -1;              // lock error
    }
    // ��ʼ�������
    if (!setboardtitle(currboard, -1)) {
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return -1;
    }

    if ((fd2 = open(olddirect, O_RDONLY, 0664)) == -1) {
        report("recopen err");
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return -1;
    }
    ldata2.l_type = F_RDLCK;
    ldata2.l_whence = 0;
    ldata2.l_len = 0;
    ldata2.l_start = 0;
    fcntl(fd2, F_SETLKW, &ldata2);

    if ((i = safe_mmapfile_handle(fd2, O_RDONLY, PROT_READ, MAP_SHARED, (void **) &ptr, &buf.st_size)) != 1) {
        if (i == 2)
            end_mmapfile((void *) ptr, buf.st_size, -1);
        ldata2.l_type = F_UNLCK;
        fcntl(fd2, F_SETLKW, &ldata2);
        close(fd2);
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return -1;
    }
    total = buf.st_size / size;
    hasht = total*8/5;
    hashtable = (struct hashstruct*) malloc(sizeof(*hashtable)*hasht);
    index = (int*)malloc(sizeof(int)*total);
    next = (int*)malloc(sizeof(int)*total);
    memset(hashtable, 0xFF, sizeof(*hashtable)*hasht);
    memset(index, 0, sizeof(int)*total);
    ptr1 = (struct fileheader*)ptr;
    for(i=0;i<total;i++,ptr1++){
    	int l=0, m;
    	if(ptr1->groupid==ptr1->id)
    		l=i;
    	else {
    		l=ptr1->groupid%hasht;
    		while(hashtable[l].index!=ptr1->groupid&&hashtable[l].index!=-1){
    			l++;
    			if(l>=hasht) l=0;
    		}
    		if(hashtable[l].index==-1) l=i;
    		else l=hashtable[l].data;
    	}
    	if(l==i){
    		l=ptr1->groupid%hasht;
    		while(hashtable[l].index!=-1){
    			l++;
    			if(l>=hasht) l=0;
    		}
    		hashtable[l].index=ptr1->groupid;
    		hashtable[l].data=i;
    		index[i]=i;
    		next[i]=0;
    	}
    	else {
    		m=index[l];
    		next[m]=i;
    		next[i]=0;
    		index[l]=i;
    		index[i]=-1;
    	}
    }
    ptr1 = (struct fileheader*)ptr;
    for(i=0;i<total;i++,ptr1++) 
    if (index[i]!=-1) {
    	int last;
    	write(fd, ptr1, size);
    	count++;
    	j=next[i];
    	while(j!=0){
    		ptr2=(struct fileheader*)(ptr+j*size);
    		memcpy(&mkpost, ptr2, sizeof(mkpost));
    		t = ptr2->title;
    		if(!strncmp(t, "Re:", 3)) t+=4;
		if(next[j]==0) sprintf(mkpost.title, "�� %s", t);
		else sprintf(mkpost.title, "�� %s", t);
    		write(fd, &mkpost, size);
    		count++;
    		j=next[j];
    	}
    }
    free(index);
    free(next);
    free(hashtable);
    end_mmapfile((void *) ptr, buf.st_size, -1);
    ldata2.l_type = F_UNLCK;
    fcntl(fd2, F_SETLKW, &ldata2);
    close(fd2);
    ftruncate(fd, count * size);

    setboardtitle(currboard, 0); // ���flag

    ldata.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &ldata);        // �˳���������
    close(fd);
    return 0;
}

int generate_all_title()
{
    struct fileheader mkpost, *ptr1;
    int fd2, size = sizeof(fileheader), total, i, count = 0;
    int boardt;
    struct boardheader btmp;
    char olddirect[PATHLEN];
    char *ptr, *t, *t2;
    struct search_temp{
    	bool has_pre;
    	int digest;
    	int thread_id;
    	int id;
    	int next; 
    }  *index;
    struct stat buf;
    int gen_threadid;

    if(!HAS_PERM(currentuser, PERM_SYSOP)) return 0;


    for(boardt=1;boardt<=get_boardcount();boardt++){
    	memcpy(&btmp, &bcache[boardt-1], sizeof(struct boardheader));
    	strcpy(currboard, btmp.filename);
    	setbdir(digestmode, currdirect, currboard);

	gen_threadid=1;

    if ((fd2 = open(currdirect, O_RDWR, 0664)) == -1) {
        report("recopen err");
        continue;
    }

    if ((i = safe_mmapfile_handle(fd2, O_RDWR, PROT_READ|PROT_WRITE, MAP_SHARED, (void **) &ptr, &buf.st_size)) != 1) {
        if (i == 2)
            end_mmapfile((void *) ptr, buf.st_size, -1);
        close(fd2);
        continue;
    }
    total = buf.st_size / size;
    index = (int*)malloc(sizeof(*index)*total);
    ptr1 = (struct fileheader*)ptr;
    for (i = 0; i < total; i++, ptr1++) {
    	int j;
        t2 = ptr1->title;
        if (!strncmp(t2, "Re:",3)) {
   			index[i].has_pre=true;
        	t2+=4;
        } else
   			index[i].has_pre=false;
        index[i].thread_id=0;
        index[i].digest=simple_digest(t2,STRLEN);
    	for (j=i-1; j>=0;j--) {
    		struct fileheader* tmppost;
    		if (index[j].digest!=index[i].digest)
    			continue;
    		tmppost = ((struct fileheader*)(ptr+j*size));
    		t = tmppost->title;
    		if (index[j].has_pre) 
    			t+=4;
    		if (!strcmp(t, t2)) {
    			index[j].next=i;
    			index[i].thread_id=index[j].thread_id;
    			index[i].next=0;
    			break;
    		}
    	}
        if (index[i].thread_id==0) {
        	index[i].thread_id=gen_threadid;
        	index[i].id = gen_threadid;
        	index[i].next=0;
        	gen_threadid++;
        }
        else {
        	index[i].id = gen_threadid;
        	gen_threadid++;
        }
    }
    ptr1 = (struct fileheader*)ptr;
    for(i=0;i<total;i++,ptr1++) {
    	ptr1->id = index[i].id;
    	ptr1->groupid = index[i].thread_id;
    	ptr1->reid = index[i].thread_id;
    }
    free(index);
    end_mmapfile((void *) ptr, buf.st_size, -1);
    close(fd2);
    btmp.nowid = gen_threadid+1;
    set_board(boardt, &btmp);
   }
    return 0;
}

int marked_mode()
{
    if (digestmode == 3) {
        digestmode = false;
        setbdir(digestmode, currdirect, currboard);
    } else {
        digestmode = 3;
        if (setboardmark(currboard, -1)) {
            if (generate_mark() == -1) {
                digestmode = false;
                return FULLUPDATE;
            }
        }
        setbdir(digestmode, currdirect, currboard);
        if (!dashf(currdirect)) {
            digestmode = false;
            setbdir(digestmode, currdirect, currboard);
            return FULLUPDATE;
        }
    }
    return NEWDIRECT;
}

int title_mode()
{
    struct stat st;
    
    if (!stat("heavyload", &st)) {
        move(t_lines - 1, 0);
        clrtoeol();
        prints("ϵͳ�������أ���ʱ������Ӧ�����Ķ�������...");
        refresh();
        pressanykey();
        return FULLUPDATE;
    }
    
    digestmode = 2;
    if (setboardtitle(currboard, -1)) {
        if (generate_title() == -1) {
            digestmode = false;
            return FULLUPDATE;
        }
    }
    setbdir(digestmode, currdirect, currboard);
    if (!dashf(currdirect)) {
        digestmode = false;
        setbdir(digestmode, currdirect, currboard);
        return FULLUPDATE;
    }
    return NEWDIRECT;
}

int search_mode(int mode, char *index)
// added by bad 2002.8.8 search mode
{
    struct fileheader mkpost;
    struct flock ldata, ldata2;
    int fd, fd2, size = sizeof(fileheader), total, i, count = 0;
    char olddirect[PATHLEN];
    char *ptr, *ptr1;
    struct stat buf;

    digestmode = 0;
    setbdir(digestmode, olddirect, currboard);
    digestmode = mode;
    setbdir(digestmode, currdirect, currboard);
    if (mode==6&& !setboardorigin(currboard, -1)) {
    	return NEWDIRECT;
    }
    if ((fd = open(currdirect, O_WRONLY | O_CREAT, 0664)) == -1) {
        report("recopen err");
        return FULLUPDATE;      // �����ļ���������
    }
    ldata.l_type = F_WRLCK;
    ldata.l_whence = 0;
    ldata.l_len = 0;
    ldata.l_start = 0;
    if (fcntl(fd, F_SETLKW, &ldata) == -1) {
        report("reclock err");
        close(fd);
        return FULLUPDATE;      // lock error
    }
    // ��ʼ�������
    if (mode==6&& !setboardorigin(currboard, -1)) {
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return -1;
    }

    if ((fd2 = open(olddirect, O_RDONLY, 0664)) == -1) {
        report("recopen err");
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return FULLUPDATE;
    }
    fstat(fd2, &buf);
    ldata2.l_type = F_RDLCK;
    ldata2.l_whence = 0;
    ldata2.l_len = 0;
    ldata2.l_start = 0;
    fcntl(fd2, F_SETLKW, &ldata2);
    total = buf.st_size / size;

    if ((i = safe_mmapfile_handle(fd2, O_RDONLY, PROT_READ, MAP_SHARED, (void **) &ptr, &buf.st_size)) != 1) {
        if (i == 2)
            end_mmapfile((void *) ptr, buf.st_size, -1);
        ldata2.l_type = F_UNLCK;
        fcntl(fd2, F_SETLKW, &ldata2);
        close(fd2);
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return FULLUPDATE;
    }
    ptr1 = ptr;
    for (i = 0; i < total; i++) {
        memcpy(&mkpost, ptr1, size);
        if (mode == 6 && strstr(mkpost.title, "Re:") != mkpost.title || mode == 7 && strcasecmp(mkpost.owner, index) == 0 || mode == 8 && strstr(mkpost.title, index) != NULL) {
            write(fd, &mkpost, size);
            count++;
        }
        ptr1 += size;
    }
    end_mmapfile((void *) ptr, buf.st_size, -1);
    ldata2.l_type = F_UNLCK;
    fcntl(fd2, F_SETLKW, &ldata2);
    close(fd2);
    ftruncate(fd, count * size);

    if(mode==6) setboardorigin(currboard, 0); // ���flag
    
    ldata.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &ldata);        // �˳���������
    close(fd);
    return NEWDIRECT;
}

int change_mode(int ent, struct fileheader *fileinfo, char *direct)
{
    char ans[4];
    char buf[STRLEN], buf2[STRLEN];
    static char title[31] = "";

    if (digestmode > 0) {
        if (digestmode == 7 || digestmode == 8)
            unlink(currdirect);
        digestmode = 0;
        setbdir(digestmode, currdirect, currboard);
        return NEWDIRECT;
    }
    move(t_lines - 1, 0);
    clrtoeol();
    getdata(t_lines - 1, 0, "�л�ģʽ��: 1)��ժ 2)ͬ���� 3)��m���� 4)ԭ�� 5)ͬ���� 6)����ؼ��� [1]: ", ans, 3, DOECHO, NULL, true);
    if (ans[0] == ' ') {
    	 ans[0] = ans[1];
    	 ans[1] = 0;
    }
    if (ans[0] == '5') {
        move(t_lines - 1, 0);
        clrtoeol();
        sprintf(buf, "���������λ���ѵ�����[%s]: ", fileinfo->owner);
        getdata(t_lines - 1, 0, buf, buf2, 13, DOECHO, NULL, true);
        if (buf2[0])
            strcpy(buf, buf2);
        else
            strcpy(buf, fileinfo->owner);
        if (buf[0] == 0)
            return FULLUPDATE;
    } else if (ans[0] == '6') {
        move(t_lines - 1, 0);
        clrtoeol();
        sprintf(buf, "������ҵ����±���ؼ���[%s]: ", title);
        getdata(t_lines - 1, 0, buf, buf2, 30, DOECHO, NULL, true);
        if (buf2[0])
            strcpy(title, buf2);
        strcpy(buf, title);
        if (buf[0] == 0)
            return FULLUPDATE;
    }
    switch (ans[0]) {
    case 0:
    case '1':
        return digest_mode();
        break;
    case '2':
    	 return title_mode();
    	 break;
    case '3':
        return marked_mode();
        break;
   case '4':
        return search_mode(6, buf);
        break;
    case '5':
        return search_mode(7, buf);
        break;
    case '6':
        return search_mode(8, buf);
        break;
    }
    return FULLUPDATE;
}

int junk_mode()
{
    if (!HAS_PERM(currentuser, PERM_SYSOP)) {
        return DONOTHING;
    }

    if (digestmode == 5) {
        digestmode = false;
        setbdir(digestmode, currdirect, currboard);
    } else {
        digestmode = 5;
        setbdir(digestmode, currdirect, currboard);
        if (!dashf(currdirect)) {
            digestmode = false;
            setbdir(digestmode, currdirect, currboard);
            return DONOTHING;
        }
    }
    return NEWDIRECT;
}

int do_thread()
{
    char buf[STRLEN];

    sprintf(buf, "Threading %s", currboard);
    report(buf);
    move(t_lines - 1, 0);
    clrtoeol();
    prints("[5m���Ժ�ϵͳ���������...[m\n");
    refresh();
    /*
     * sprintf(buf,"bin/thread %s&",currboard); 
     */
    sprintf(buf, "bin/thread %s", currboard);
    system(buf);
    return 0;
}

int thread_mode()
{
    struct stat st;

    /*
     * add by Bigman to verrify the thread 
     */
    char buf[STRLEN], ch[4];

    /*
     * add by KCN if  heavyload can't thread 
     */
    if (!stat("heavyload", &st)) {
        move(t_lines - 1, 0);
        clrtoeol();
        prints("ϵͳ�������أ���ʱ������Ӧ�����Ķ�������...");
        refresh();
        pressanykey();
        return FULLUPDATE;
    }

    if (digestmode == 2) {
        digestmode = false;
        setbdir(digestmode, currdirect, currboard);
    } else {
        /*
         * �����ж� Bigman 2000.12.26 
         */
        saveline(t_lines - 2, 0, NULL);
        move(t_lines - 2, 0);
        clrtoeol();
        strcpy(buf, "ȷ��ͬ�����Ķ�? (Y/N) [N]:");
        getdata(t_lines - 2, 0, buf, ch, 3, DOECHO, NULL, true);

        if (ch[0] == 'y' || ch[0] == 'Y') {
            digestmode = 2;
            setbdir(digestmode, currdirect, currboard);
            do_thread();
            /*
             * while(!dashf(currdirect))
             * {
             * sleep(1);
             * if(++i==5)
             * break;
             * } 
             */
            if (!dashf(currdirect)) {
                digestmode = false;
                setbdir(digestmode, currdirect, currboard);
                return PARTUPDATE;
            }
        } else {
            saveline(t_lines - 2, 1, NULL);
            return DONOTHING;
        }
    }
    return NEWDIRECT;
}

int dele_digest(char *dname, char *direc)
{                               /* ɾ����ժ��һƪPOST, dname=post�ļ���,direc=��ժĿ¼�� */
    char digest_name[STRLEN];
    char new_dir[STRLEN];
    char buf[STRLEN];
    char *ptr;
    struct fileheader fh;
    int pos;

    strcpy(digest_name, dname);
    strcpy(new_dir, direc);

    digest_name[0] = 'G';
    ptr = strrchr(new_dir, '/') + 1;
    strcpy(ptr, DIGEST_DIR);
    pos = search_record(new_dir, &fh, sizeof(fh), (RECORD_FUNC_ARG) cmpname, digest_name);      /* ��ժĿ¼�� .DIR�� ���� ��POST */
    if (pos <= 0) {
        return -1;
    }
    delete_record(new_dir, sizeof(struct fileheader), pos, (RECORD_FUNC_ARG) cmpname, digest_name);
    *ptr = '\0';
    sprintf(buf, "%s%s", new_dir, digest_name);
    unlink(buf);
    return 0;
}

int digest_post(int ent, struct fileheader *fhdr, char *direct)
{
    return change_post_flag(ent, fhdr, direct, FILE_DIGEST_FLAG, 1);
}

#ifndef NOREPLY
int do_reply(struct fileheader *fileinfo)
/* reply POST */
{
    char buf[255];

    if (fileinfo->accessed[1] & FILE_READ) {    /*Haohmaru.99.01.01.���²���re */
        clear();
        move(3, 0);
        prints("\n\n            �ܱ�Ǹ�������Ѿ�����Ϊ����reģʽ,�벻Ҫ��ͼ���۱���...\n");
        pressreturn();
        return FULLUPDATE;
    }
    setbfile(buf, currboard, fileinfo->filename);
    strcpy(replytitle, fileinfo->title);
    post_article(buf, fileinfo);
    replytitle[0] = '\0';
    return FULLUPDATE;
}
#endif

int garbage_line(char *str)
{                               /* �жϱ����Ƿ��� ���õ� */
    int qlevel = 0;

    while (*str == ':' || *str == '>') {
        str++;
        if (*str == ' ')
            str++;
        if (qlevel++ >= 0)
            return 1;
    }
    while (*str == ' ' || *str == '\t')
        str++;
    if (qlevel >= 0)
        if (strstr(str, "�ᵽ:\n") || strstr(str, ": ��\n") || strncmp(str, "==>", 3) == 0 || strstr(str, "������ ˵"))
            return 1;
    return (*str == '\n');
}

/* When there is an old article that can be included -jjyang */
void do_quote(char *filepath, char quote_mode, char *q_file, char *q_user)
{                               /* �������£� ȫ�ֱ���quote_file,quote_user, */
    FILE *inf, *outf;
    char *qfile, *quser;
    char buf[256], *ptr;
    char op;
    int bflag;
    int line_count = 0;         /* ��Ӽ���ģʽ���� Bigman: 2000.7.2 */

    qfile = q_file;
    quser = q_user;
    bflag = strncmp(qfile, "mail", 4);  /* �ж����õ������»����� */
    outf = fopen(filepath, "w");
    if (*qfile != '\0' && (inf = fopen(qfile, "r")) != NULL) {  /* �򿪱������ļ� */
        op = quote_mode;
        if (op != 'N') {        /* ����ģʽΪ N ��ʾ ������ */
            fgets(buf, 256, inf);       /* ȡ����һ���� ���������µ� ������Ϣ */
            if ((ptr = strrchr(buf, ')')) != NULL) {    /* ��һ��':'�����һ�� ')' �е��ַ��� */
                ptr[1] = '\0';
                if ((ptr = strchr(buf, ':')) != NULL) {
                    quser = ptr + 1;
                    while (*quser == ' ')
                        quser++;
                }
            }
            /*---	period	2000-10-21	add '\n' at beginning of Re-article	---*/
            if (bflag)
                fprintf(outf, "\n�� �� %s �Ĵ������ᵽ: ��\n", quser);
            else
                fprintf(outf, "\n�� �� %s ���������ᵽ: ��\n", quser);

            if (op == 'A') {    /* ����һ���⣬ȫ������ */
                while (fgets(buf, 256, inf) != NULL) {
                    fprintf(outf, ": %s", buf);
                }
            } else if (op == 'R') {
                while (fgets(buf, 256, inf) != NULL)
                    if (buf[0] == '\n')
                        break;
                while (fgets(buf, 256, inf) != NULL) {
                    if (Origin2(buf))   /* �ж��Ƿ� ������� */
                        continue;
                    fprintf(outf, "%s", buf);

                }
            } else {
                while (fgets(buf, 256, inf) != NULL)
                    if (buf[0] == '\n')
                        break;
                while (fgets(buf, 256, inf) != NULL) {
                    if (strcmp(buf, "--\n") == 0)       /* ���� ��ǩ����Ϊֹ */
                        break;
                    if (buf[250] != '\0')
                        strcpy(buf + 250, "\n");
                    if (!garbage_line(buf)) {   /* �ж��Ƿ��������� */
                        fprintf(outf, ": %s", buf);
                        if (op == 'S') {        /* ����ģʽ,ֻ����ǰ���� Bigman:2000.7.2 */
                            line_count++;
                            if (line_count > 10) {
                                fprintf(outf, ": ...................");
                                break;
                            }
                        }
                    }
                }
            }
        }

        fprintf(outf, "\n");
        fclose(inf);
    }
    /*
     * *q_file = '\0';
     * *q_user = '\0';
     */

    if ((numofsig > 0) && !(currentuser->signature == 0 || Anony == 1)) {       /* ǩ����Ϊ0����� */
        if (currentuser->signature < 0)
            addsignature(outf, currentuser, (rand() % numofsig) + 1);
        else
            addsignature(outf, currentuser, currentuser->signature);
    }
    fclose(outf);
}

int do_post()
{                               /* �û�post */
    *quote_user = '\0';
    return post_article("", NULL);
}

 /*ARGSUSED*/ int post_reply(int ent, struct fileheader *fileinfo, char *direct)
        /*
         * ���Ÿ�POST���� 
         */
{
    char uid[STRLEN];
    char title[STRLEN];
    char *t;
    FILE *fp;
    char q_file[STRLEN];


    if (!HAS_PERM(currentuser, PERM_LOGINOK) || !strcmp(currentuser->userid, "guest"))  /* guest ��Ȩ */
        return 0;
    /*
     * ̫���˰�,����post�Ͳ��û�����
     * if (!HAS_PERM(currentuser,PERM_POST)) return; Haohmaru.99.1.18 
     */

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

    modify_user_mode(SMAIL);

    /*
     * indicate the quote file/user 
     */
    setbfile(q_file, currboard, fileinfo->filename);
    strncpy(quote_user, fileinfo->owner, IDLEN);
    quote_user[IDLEN] = 0;

    /*
     * find the author 
     */
    if (strchr(quote_user, '.')) {
        genbuf[0] = '\0';
        fp = fopen(q_file, "r");
        if (fp != NULL) {
            fgets(genbuf, 255, fp);
            fclose(fp);
        }

        t = strtok(genbuf, ":");
        if (strncmp(t, "������", 6) == 0 || strncmp(t, "Posted By", 9) == 0 || strncmp(t, "��  ��", 6) == 0) {
            t = (char *) strtok(NULL, " \r\t\n");
            strcpy(uid, t);
        } else {
            prints("Error: Cannot find Author ... \n");
            pressreturn();
        }
    } else
        strcpy(uid, quote_user);

    /*
     * make the title 
     */
    if (toupper(fileinfo->title[0]) != 'R' || fileinfo->title[1] != 'e' || fileinfo->title[2] != ':')
        strcpy(title, "Re: ");
    else
        title[0] = '\0';
    strncat(title, fileinfo->title, STRLEN - 5);

    clear();

    /*
     * edit, then send the mail 
     */
    switch (do_send(uid, title, q_file)) {
    case -1:
        prints("ϵͳ�޷�����\n");
        break;
    case -2:
        prints("���Ŷ����Ѿ���ֹ\n");
        break;
    case -3:
        prints("ʹ���� '%s' �޷�����\n", uid);
        break;
    case -4:
        prints("�Է������������޷�����\n");
        break;
    default:
        prints("�ż��ѳɹ��ؼĸ�ԭ���� %s\n", uid);
    }
    pressreturn();
    return FULLUPDATE;
}

int show_board_notes(char bname[30])
{                               /* ��ʾ�����Ļ� */
    char buf[256];

    sprintf(buf, "vote/%s/notes", bname);       /* ��ʾ����İ����Ļ� vote/����/notes */
    if (dashf(buf)) {
        ansimore2(buf, false, 0, 23 /*19 */ );
        return 1;
    } else if (dashf("vote/notes")) {   /* ��ʾϵͳ�Ļ� vote/notes */
        ansimore2("vote/notes", false, 0, 23 /*19 */ );
        return 1;
    }
    return -1;
}

int post_article(char *q_file, struct fileheader *re_file)
{                               /*�û� POST ���� */
    struct fileheader post_file;
    char filepath[STRLEN];
    char buf[256], buf2[256], buf3[STRLEN], buf4[STRLEN];
    int aborted, anonyboard;
    int replymode = 1;          /* Post New UI */
    char ans[4], include_mode = 'S';
    struct boardheader *bp;

    if (true == check_readonly(currboard))      /* Leeward 98.03.28 */
        return FULLUPDATE;

#ifdef AIX_CANCELLED_BY_LEEWARD
    if (true == check_RAM_lack())       /* Leeward 98.06.16 */
        return FULLUPDATE;
#endif

    modify_user_mode(POSTING);
    if (!haspostperm(currentuser, currboard)) { /* POSTȨ�޼�� */
        move(3, 0);
        clrtobot();
        if (digestmode == false) {
            prints("\n\n        ����������Ψ����, ����������Ȩ���ڴ˷�������.\n");
            prints("        �������δע�ᣬ���ڸ��˹���������ϸע�����\n");
            prints("        δͨ�����ע����֤���û���û�з������µ�Ȩ�ޡ�\n");
            prints("        лл������ :-) \n");
        } else {
            prints("\n\n     Ŀǰ����ժ������ģʽ, ���Բ��ܷ�������.(������뿪��ժģʽ)\n");
        }
        pressreturn();
        clear();
        return FULLUPDATE;
    } else if (deny_me(currentuser->userid, currboard) && !HAS_PERM(currentuser, PERM_SYSOP)) { /* ������ֹPOST ��� */
        move(3, 0);
        clrtobot();
        prints("\n\n                     �ܱ�Ǹ���㱻����ֹͣ�� POST ��Ȩ��...\n");
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    memset(&post_file, 0, sizeof(post_file));
    clear();
    show_board_notes(currboard);        /* �����Ļ� */
#ifndef NOREPLY                 /* title�Ƿ���Re: */
    if (replytitle[0] != '\0') {
        buf4[0] = ' ';
        /*
         * if( strncasecmp( replytitle, "Re:", 3 ) == 0 ) Change By KCN:
         * why use strncasecmp? 
         */
        if (strncmp(replytitle, "Re:", 3) == 0)
            strcpy(buf, replytitle);
        else
            sprintf(buf, "Re: %s", replytitle);
        buf[50] = '\0';
    } else
#endif
    {
        buf[0] = '\0';
        buf4[0] = '\0';
        replymode = 0;
    }
    if (currentuser->signature > numofsig)      /*ǩ����No.��� */
        currentuser->signature = 1;
    anonyboard = anonymousboard(currboard);     /* �Ƿ�Ϊ������ */
    /*
     * by zixia: ������ȱʡ��ʹ������ 
     */
    if (!strcmp(currboard, "Announce"))
        Anony = 1;
    else
        Anony = 0;

    while (1) {                 /* ����ǰ�޸Ĳ����� ���Կ������'��ʾǩ����' */
        sprintf(buf3, "����ģʽ [%c]", include_mode);
        move(t_lines - 4, 0);
        clrtoeol();
        prints("[m��������� %s ������     %s\n", currboard, (anonyboard) ? (Anony == 1 ? "[1mҪ[mʹ������" : "[1m��[mʹ������") : "");
        clrtoeol();
        prints("ʹ�ñ���: %-50s\n", (buf[0] == '\0') ? "[�����趨����]" : buf);
        clrtoeol();
        if (currentuser->signature < 0)
            prints("ʹ�����ǩ����     %s", (replymode) ? buf3 : " ");
        else
            prints("ʹ�õ� %d ��ǩ����     %s", currentuser->signature, (replymode) ? buf3 : " ");

        if (buf4[0] == '\0' || buf4[0] == '\n') {
            move(t_lines - 1, 0);
            clrtoeol();
            strcpy(buf4, buf);
            getdata(t_lines - 1, 0, "����: ", buf4, 50, DOECHO, NULL, false);
            if ((buf4[0] == '\0' || buf4[0] == '\n')) {
                if (buf[0] != '\0') {
                    buf4[0] = ' ';
                    continue;
                } else
                    return FULLUPDATE;
            }
            strcpy(buf, buf4);
            continue;
        }
        move(t_lines - 1, 0);
        clrtoeol();
        /*
         * Leeward 98.09.24 add: viewing signature(s) while setting post head 
         */
        sprintf(buf2, "��[1;32m0[m~[1;32m%d/V/L[mѡ/��/���ǩ����%s��[1;32mT[m�ı��⣬%s[1;32mEnter[m���������趨: ", numofsig,
                (replymode) ? "��[1;32mS/Y[m/[1;32mN[m/[1;32mR[m/[1;32mA[m ������ģʽ" : "", (anonyboard) ? "[1;32mM[m ������" : "");
        getdata(t_lines - 1, 0, buf2, ans, 3, DOECHO, NULL, true);
        ans[0] = toupper(ans[0]);       /* Leeward 98.09.24 add; delete below toupper */
        if ((ans[0] - '0') >= 0 && ans[0] - '0' <= 9) {
            if (atoi(ans) <= numofsig)
                currentuser->signature = atoi(ans);
        } else if ((ans[0] == 'S' || ans[0] == 'Y' || ans[0] == 'N' || ans[0] == 'A' || ans[0] == 'R') && replymode) {
            include_mode = ans[0];
        } else if (ans[0] == 'T') {
            buf4[0] = '\0';
        } else if (ans[0] == 'M') {
            Anony = (Anony == 1) ? 0 : 1;
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
            /*
             * Changed by KCN,disable color title 
             */
            {
                unsigned int i;

                for (i = 0; (i < strlen(buf)) && (i < STRLEN - 1); i++)
                    if (buf[i] == 0x1b)
                        post_file.title[i] = ' ';
                    else
                        post_file.title[i] = buf[i];
                post_file.title[i] = 0;
            }
            /*
             * strcpy(post_file.title, buf); 
             */
            strncpy(save_title, post_file.title, STRLEN);
            if (save_title[0] == '\0')
                return FULLUPDATE;
            break;
        }
    }                           /* ������� */

    setbfile(filepath, currboard, "");
    if ((aborted = get_postfilename(post_file.filename, filepath)) != 0) {
        move(3, 0);
        clrtobot();
        prints("\n\n�޷������ļ�:%d...\n", aborted);
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    in_mail = false;

    /*
     * strncpy(post_file.owner,(anonyboard&&Anony)?
     * "Anonymous":currentuser->userid,STRLEN) ;
     */
    strncpy(post_file.owner, (anonyboard && Anony) ? currboard : currentuser->userid, STRLEN);

    /*
     * if ((!strcmp(currboard,"Announce"))&&(!strcmp(post_file.owner,"Anonymous")))
     * strcpy(post_file.owner,"SYSOP");
     */

    if ((!strcmp(currboard, "Announce")) && (!strcmp(post_file.owner, currboard)))
        strcpy(post_file.owner, "SYSOP");

    setbfile(filepath, currboard, post_file.filename);

    if ((bp = getbcache(currboard)) == NULL) {
        unlink(filepath);
        clear();
        return FULLUPDATE;
    }
    if (bp->flag & BOARD_OUTFLAG)
        local_article = 0;
    else
        local_article = 1;
    if (!strcmp(post_file.title, buf) && q_file[0] != '\0')
        if (q_file[119] == 'L')
            local_article = 1;

    modify_user_mode(POSTING);

    do_quote(filepath, include_mode, q_file, quote_user);       /*����ԭ���� */

    strcpy(quote_title, save_title);
    strcpy(quote_board, currboard);
    aborted = vedit(filepath, true);    /* ����༭״̬ */

    add_loginfo(filepath, currentuser, currboard, Anony);       /*������һ�� */

    strncpy(post_file.title, save_title, STRLEN);
    if (aborted == 1 || !(bp->flag & BOARD_OUTFLAG)) {  /* local save */
        post_file.innflag[1] = 'L';
        post_file.innflag[0] = 'L';
    } else {
        post_file.innflag[1] = 'S';
        post_file.innflag[0] = 'S';
        outgo_post(&post_file, currboard, save_title);
    }
    Anony = 0;                  /*Inital For ShowOut Signature */

    if (aborted == -1) {        /* ȡ��POST */
        unlink(filepath);
        clear();
        return FULLUPDATE;
    }
    setbdir(digestmode, buf, currboard);

    /*
     * ��boards����������Զ�������±�� Bigman:2000.8.12 
     */
    if (!strcmp(currboard, "Board") && !HAS_PERM(currentuser, PERM_OBOARDS) && HAS_PERM(currentuser, PERM_BOARDS)) {
        post_file.accessed[0] |= FILE_SIGN;
    }

    after_post(currentuser, &post_file, currboard, re_file);

    if (!junkboard(currboard)) {
        currentuser->numposts++;
    }
    return FULLUPDATE;
}

int add_edit_mark(char *fname, int mode, char *title)
{
    FILE *fp, *out;
    char buf[256];
    time_t now;
    char outname[STRLEN];
    int step = 0;

    if ((fp = fopen(fname, "r")) == NULL)
        return 0;
    sprintf(outname, "tmp/%d.editpost", getpid());
    if ((out = fopen(outname, "w")) == NULL) {
        fclose(fp);
        return 0;
    }

    while ((fgets(buf, 256, fp)) != NULL) {
        if (mode == 1) {
            /*
             * if(step==1)
             * step=2;
             * if(!step && !strncmp(buf,"����վ: ",8))
             * {
             * step=1;
             * } 
             */
            if (!strncmp(buf, "[36m�� �޸�:��", 17))
                continue;
            /*
             * if(step!=3&&(!strncmp(buf,"��  ��: ",8)||!strncmp(buf,"ת��վ: ",8)))
             * step=1; 
             */
            if (Origin2(buf)) {
                now = time(0);
                fprintf(out, "[36m�� �޸�:��%s � %15.15s �޸ı��ġ�[FROM: %15.15s][m\n", currentuser->userid, ctime(&now) + 4, fromhost);
                step = 3;
            }
            fputs(buf, out);
        } else {
            if (step != 3 && !strncmp(buf, "��  ��: ", 8)) {
                step = 3;
                fprintf(out, "��  ��: %s\n", title);
                continue;
            }
            fputs(buf, out);
        }
    }
    fclose(fp);
    fclose(out);

    f_mv(outname, fname);
    pressanykey();

    return 1;
}

 /*ARGSUSED*/ int edit_post(int ent, struct fileheader *fileinfo, char *direct)
        /*
         * POST �༭ 
         */
{
    char buf[512];
    char *t;

    if (!strcmp(currboard, "syssecurity")
        || !strcmp(currboard, "junk")
        || !strcmp(currboard, "deleted"))       /* Leeward : 98.01.22 */
        return DONOTHING;
    if ((digestmode == 4) || (digestmode == 5))
        return DONOTHING;       /* no edit in dustbin as requested by terre */
    if (true == check_readonly(currboard))      /* Leeward 98.03.28 */
        return FULLUPDATE;

#ifdef AIX_CANCELLED_BY_LEEWARD
    if (true == check_RAM_lack())       /* Leeward 98.06.16 */
        return FULLUPDATE;
#endif

    modify_user_mode(EDIT);

    if (!HAS_PERM(currentuser, PERM_SYSOP))     /* SYSOP����ǰ������ԭ������ ���Ա༭ */
        if (!chk_currBM(currBM, currentuser))
            /*
             * change by KCN 1999.10.26
             * if(strcmp( fileinfo->owner, currentuser->userid))
             */
            if (!isowner(currentuser, fileinfo))
                return DONOTHING;

    if (deny_me(currentuser->userid, currboard) && !HAS_PERM(currentuser, PERM_SYSOP)) {        /* ������ֹPOST ��� */
        move(3, 0);
        clrtobot();
        prints("\n\n                     �ܱ�Ǹ���㱻����ֹͣ�� POST ��Ȩ��...\n");
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    clear();
    strcpy(buf, direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
#ifndef LEEWARD_X_FILTER
    sprintf(genbuf, "/bin/cp -f %s/%s tmp/%d.editpost.bak", buf, fileinfo->filename, getpid()); /* Leeward 98.03.29 */
    system(genbuf);
#endif

    /*
     * Leeward 2000.01.23: Cache 
     * sprintf(genbuf, "/board/%s/%s.html", currboard,fileinfo->filename);
     * ca_expire(genbuf); 
     */

    sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
    if (vedit_post(genbuf, false) != -1) {
        if (ADD_EDITMARK)
            add_edit_mark(genbuf, 1, /*NULL*/ fileinfo->title);
    }
    bbslog("1user", "edited post '%s' on %s", fileinfo->title, currboard);
    return FULLUPDATE;
}

int edit_title(int ent, struct fileheader *fileinfo, char *direct)
        /*
         * �༭���±��� 
         */
{
    char buf[STRLEN];

    /*
     * Leeward 99.07.12 added below 2 variables 
     */
    long i;
    struct fileheader xfh;
    int fd;


    if (!strcmp(currboard, "syssecurity")
        || !strcmp(currboard, "junk")
        || !strcmp(currboard, "deleted"))       /* Leeward : 98.01.22 */
        return DONOTHING;

    if ((digestmode == 4) || (digestmode == 5))
        return DONOTHING;
    if (true == check_readonly(currboard))      /* Leeward 98.03.28 */
        return FULLUPDATE;

    if (!HAS_PERM(currentuser, PERM_SYSOP))     /* Ȩ�޼�� */
        if (!chk_currBM(currBM, currentuser))
            /*
             * change by KCN 1999.10.26
             * if(strcmp( fileinfo->owner, currentuser->userid))
             */
            if (!isowner(currentuser, fileinfo)) {
                return DONOTHING;
            }
    getdata(t_lines - 1, 0, "�����±���: ", buf, 50, DOECHO, NULL, true);       /*������� */
    if (buf[0] != '\0') {
        char tmp[STRLEN * 2], *t;
        char tmp2[STRLEN];      /* Leeward 98.03.29 */

        /*
         * Leeward 2000.01.23: Cache 
         * sprintf(genbuf, "/board/%s/%s.html", currboard,fileinfo->filename);
         * ca_expire(genbuf); 
         */

        strcpy(tmp2, fileinfo->title);  /* Do a backup */
        /*
         * Changed by KCN,disable color title 
         */
        {
            unsigned int i;

            for (i = 0; (i < strlen(buf)) && (i < STRLEN - 1); i++)
                if (buf[i] == 0x1b)
                    fileinfo->title[i] = ' ';
                else
                    fileinfo->title[i] = buf[i];
            fileinfo->title[i] = 0;
        }
        /*
         * strcpy(fileinfo->title,buf); 
         */
        strcpy(tmp, direct);
        if ((t = strrchr(tmp, '/')) != NULL)
            *t = '\0';
        sprintf(genbuf, "%s/%s", tmp, fileinfo->filename);

        add_edit_mark(genbuf, 2, buf);
        /*
         * Leeward 99.07.12 added below to fix a big bug 
         */
        setbdir(digestmode, buf, currboard);
        if ((fd = open(buf, O_RDONLY, 0)) != -1) {
            for (i = ent; i > 0; i--) {
                if (0 == get_record_handle(fd, &xfh, sizeof(xfh), i)) {
                    if (0 == strcmp(xfh.filename, fileinfo->filename)) {
                        ent = i;
                        break;
                    }
                }
            }
            close(fd);
        }
        if (0 == i)
            return PARTUPDATE;
        /*
         * Leeward 99.07.12 added above to fix a big bug 
         */

        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);

	 setboardorigin(currboard, 1);
	 setboardtitle(currboard, 1);
    }
    return PARTUPDATE;
}

/* Mark POST */
int mark_post(int ent, struct fileheader *fileinfo, char *direct)
{
    return change_post_flag(ent, fileinfo, direct, FILE_MARK_FLAG, 0);
}

int noreply_post(int ent, struct fileheader *fileinfo, char *direct)
{
    return change_post_flag(ent, fileinfo, direct, FILE_NOREPLY_FLAG, 1);
}

int noreply_post_noprompt(int ent, struct fileheader *fileinfo, char *direct)
{
    return change_post_flag(ent, fileinfo, direct, FILE_NOREPLY_FLAG, 0);
}

int sign_post(int ent, struct fileheader *fileinfo, char *direct)
{
    return change_post_flag(ent, fileinfo, direct, FILE_SIGN_FLAG, 1);
}
int del_range(int ent, struct fileheader *fileinfo, char *direct, int mailmode)
  /*
   * ����ɾ�� 
   */
{
    char del_mode[11], num1[11], num2[11];
    char fullpath[STRLEN];
    int inum1, inum2;
    int result;                 /* Leeward: 97.12.15 */
    int idel_mode;              /*haohmaru.99.4.20 */

    if (!strcmp(currboard, "syssecurity")
        || !strcmp(currboard, "junk")
        || !strcmp(currboard, "deleted")
        || strstr(direct, ".THREAD") /*Haohmaru.98.10.16 */ )   /* Leeward : 98.01.22 */
        return DONOTHING;

    if (uinfo.mode == READING && !HAS_PERM(currentuser, PERM_SYSOP))
        if (!chk_currBM(currBM, currentuser)) {
            return DONOTHING;
        }

    if (digestmode == 2)
        return DONOTHING;
    if (digestmode == 4 || digestmode == 5) {
        return DONOTHING;
    }
    clear();
    prints("����ɾ��\n");
    /*
     * Haohmaru.99.4.20.���ӿ���ǿ��ɾ����mark���µĹ��� 
     */
    getdata(1, 0, "ɾ��ģʽ [0]���ɾ�� [1]��ͨɾ�� [2]ǿ��ɾ��(��mark������һ��ɾ) (0): ", del_mode, 10, DOECHO, NULL, true);
    idel_mode = atoi(del_mode);
    /*
     * if (idel_mode!=0 || idel_mode!=1)
     * {
     * return FULLUPDATE ;
     * } 
     */
    getdata(2, 0, "��ƪ���±��(����0���������Ϊɾ��������): ", num1, 10, DOECHO, NULL, true);
    inum1 = atoi(num1);
    if (inum1 == 0) {
        inum2 = -1;
        goto THERE;
    }
    if (inum1 <= 0) {
        prints("������\n");
        pressreturn();
        return FULLUPDATE;
    }
    getdata(3, 0, "ĩƪ���±��: ", num2, 10, DOECHO, NULL, true);
    inum2 = atoi(num2);
    if (inum2 <= inum1) {
        prints("������\n");
        pressreturn();
        return FULLUPDATE;
    }
  THERE:
    getdata(4, 0, "ȷ��ɾ�� (Y/N)? [N]: ", num1, 10, DOECHO, NULL, true);
    if (*num1 == 'Y' || *num1 == 'y') {
        result = delete_range(direct, inum1, inum2, idel_mode);
        if (inum1 != 0)
            fixkeep(direct, inum1, inum2);
        else
            fixkeep(direct, 1, 1);
        if (uinfo.mode != RMAIL) {
            updatelastpost(currboard);
            sprintf(genbuf, "del %d-%d on %s", inum1, inum2, currboard);
            report(genbuf);     /*bbslog */
        }
        prints("ɾ��%s\n", result ? "ʧ�ܣ�" : "���"); /* Leeward: 97.12.15 */
        if (result) {           /* prints("�������: %d;%s �뱨��վ����лл��", result,direct);
                                 * added by Haohmaru,�޸�����ɾ������,98.9.12 */
            prints("�������: %d;%s", result, direct);
            getdata(8, 0, "����ɾ������,������޸�,��ȷ��[35m�����ڱ���ִ������ɾ����������'Y'[0m (Y/N)? [N]: ", num1, 10, DOECHO, NULL, true);
            if (*num1 == 'Y' || *num1 == 'y') {
                if (!mailmode) {
                    sprintf(fullpath, "mail/%c/%s/.tmpfile", toupper(currentuser->userid[0]), currentuser->userid);
                    unlink(fullpath);
                    sprintf(fullpath, "mail/%c/%s/.deleted", toupper(currentuser->userid[0]), currentuser->userid);
                    unlink(fullpath);
                } else {
                    if (true == checkreadonly(currboard)) {     /*Haohmaru,ֻ������´�����~bbsroot/boards/.����tmpfile �ļ����� */
                        sprintf(fullpath, "boards/.%stmpfile", currboard);
                        unlink(fullpath);
                        sprintf(fullpath, "boards/.%sdeleted", currboard);
                        unlink(fullpath);
                    } else {
                        sprintf(fullpath, "boards/%s/.tmpfile", currboard);
                        unlink(fullpath);
                        sprintf(fullpath, "boards/%s/.deleted", currboard);
                        unlink(fullpath);
                        sprintf(fullpath, "boards/%s/.tmpfilD", currboard);
                        unlink(fullpath);
                        sprintf(fullpath, "boards/%s/.tmpfilJ", currboard);
                        unlink(fullpath);
                    }
                }
                prints("\n�����Ѿ�����,������ִ������ɾ��!");
            } else {
                prints("����ȷ��");
            }
        }
        /*
         * Haohamru.99.5.14.ɾ��.deleted�ļ� 
         */
        if (!mailmode) {
            sprintf(fullpath, "mail/%c/%s/.deleted", toupper(currentuser->userid[0]
                    ), currentuser->userid);
            unlink(fullpath);
        } else {
            sprintf(fullpath, "boards/%s/.deleted", currboard);
            unlink(fullpath);
        }
        pressreturn();
        return DIRCHANGED;
    }
    prints("Delete Aborted\n");
    pressreturn();
    return FULLUPDATE;
}

int del_post(int ent, struct fileheader *fileinfo, char *direct)
{
    char usrid[STRLEN];
    int owned, keep;
    extern int SR_BMDELFLAG;

    if (!strcmp(currboard, "syssecurity")
        || !strcmp(currboard, "junk")
        || !strcmp(currboard, "deleted"))       /* Leeward : 98.01.22 */
        return DONOTHING;

    if (digestmode == 2 || digestmode == 3 || digestmode == 4 || digestmode == 5 || digestmode == 6 || digestmode == 7)
        return DONOTHING;
    keep = sysconf_eval("KEEP_DELETED_HEADER"); /*�Ƿ񱣳ֱ�ɾ����POST�� title */
    if (fileinfo->owner[0] == '-' && keep > 0 && !SR_BMDELFLAG) {
        clear();
        prints("��������ɾ��.\n");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    owned = isowner(currentuser, fileinfo);
    /*
     * change by KCN  ! strcmp( fileinfo->owner, currentuser->userid ); 
     */
    strcpy(usrid, fileinfo->owner);
    if (!(owned) && !HAS_PERM(currentuser, PERM_SYSOP))
        if (!chk_currBM(currBM, currentuser)) {
            return DONOTHING;
        }
    if (!SR_BMDELFLAG) {
        clear();
        prints("ɾ������ '%s'.", fileinfo->title);
        getdata(1, 0, "(Y/N) [N]: ", genbuf, 3, DOECHO, NULL, true);
        if (genbuf[0] != 'Y' && genbuf[0] != 'y') {     /* if not yes quit */
            move(2, 0);
            prints("ȡ��\n");
            pressreturn();
            clear();
            return FULLUPDATE;
        }
    }

    if (do_del_post(currentuser, ent, fileinfo, direct, currboard, digestmode, !B_to_b) != 0) {
        move(2, 0);
        prints("ɾ��ʧ��\n");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    return DIRCHANGED;
}

/* Added by netty to handle post saving into (0)Announce */
int Save_post(int ent, struct fileheader *fileinfo, char *direct)
{
    if (!HAS_PERM(currentuser, PERM_SYSOP))
        if (!chk_currBM(currBM, currentuser))
            return DONOTHING;
    return (a_Save("0Announce", currboard, fileinfo, false, direct, ent));
}

/* Semi_save ���������´浽�ݴ浵��ͬʱɾ�����µ�ͷβ Life 1997.4.6 */
int Semi_save(int ent, struct fileheader *fileinfo, char *direct)
{
    if (!HAS_PERM(currentuser, PERM_SYSOP))
        if (!chk_currBM(currBM, currentuser))
            return DONOTHING;
    return (a_SeSave("0Announce", currboard, fileinfo, false));
}

/* Added by netty to handle post saving into (0)Announce */
int Import_post(int ent, struct fileheader *fileinfo, char *direct)
{
    char szBuf[STRLEN];

    if (!HAS_PERM(currentuser, PERM_SYSOP))
        if (!chk_currBM(currBM, currentuser))
            return DONOTHING;

    if (fileinfo->accessed[0] & FILE_IMPORTED) {        /* Leeward 98.04.15 */
        a_prompt(-1, "������������¼����������. �����ٴ���¼��? (Y/N) [N]: ", szBuf);
        if (szBuf[0] != 'y' && szBuf[0] != 'Y')
            return FULLUPDATE;
    }
    /*
     * Leeward 98.04.15 
     */
    return (a_Import("0Announce", currboard, fileinfo, false, direct, ent));
}

int show_b_note()
{
    clear();
    if (show_board_notes(currboard) == -1) {
        move(3, 30);
        prints("�����������ޡ�����¼����");
    }
    pressanykey();
    return FULLUPDATE;
}

int into_announce()
{
    if (a_menusearch("0Announce", currboard, (HAS_PERM(currentuser, PERM_ANNOUNCE) || HAS_PERM(currentuser, PERM_SYSOP) || HAS_PERM(currentuser, PERM_OBOARDS)) ? PERM_BOARDS : 0))
        return FULLUPDATE;
    return DONOTHING;
}

extern int mainreadhelp();
extern int b_results();
extern int b_vote();
extern int b_vote_maintain();
extern int b_notes_edit();
extern int b_jury_edit();       /*stephen 2001.11.1 */

static int sequent_ent;

int sequent_messages(struct fileheader *fptr, int idc, int *continue_flag)
{
    if (readpost) {
        if (idc < sequent_ent)
            return 0;
        if (!brc_unread(FILENAME2POSTTIME(fptr->filename)))
            return 0;           /*�Ѷ� �� ���� */
        if (*continue_flag != 0) {
            genbuf[0] = 'y';
        } else {
            prints("������: '%s' ����:\n\"%s\" posted by %s.\n", currboard, fptr->title, fptr->owner);
            getdata(3, 0, "��ȡ (Y/N/Quit) [Y]: ", genbuf, 5, DOECHO, NULL, true);
        }
        if (genbuf[0] != 'y' && genbuf[0] != 'Y' && genbuf[0] != '\0') {
            if (genbuf[0] == 'q' || genbuf[0] == 'Q') {
                clear();
                return QUIT;
            }
            clear();
            return 0;
        }
        strncpy(quote_user, fptr->owner, IDLEN);
        quote_user[IDLEN] = 0;
        setbfile(genbuf, currboard, fptr->filename);
        ansimore_withzmodem(genbuf, false, fptr->title);
      redo:
        move(t_lines - 1, 0);
        clrtoeol();
        prints("\033[1;44;31m[��������]  \033[33m���� R �� ���� Q,�� ����һ�� ' ',�� ��^R ���Ÿ�����                \033[m");
        *continue_flag = 0;
        switch (igetkey()) {
        case Ctrl('Y'):
            zsend_post(0, fptr, currdirect);
            clear();
            goto redo;
        case Ctrl('Z'):
            r_lastmsg();        /* Leeward 98.07.30 support msgX */
            break;
        case 'N':
        case 'Q':
        case 'n':
        case 'q':
        case KEY_LEFT:
            break;
        case KEY_REFRESH:
            break;
        case 'Y':
        case 'R':
        case 'y':
        case 'r':
            do_reply(fptr);     /*���� */
        case ' ':
        case '\n':
        case KEY_DOWN:
            *continue_flag = 1;
            break;
        case Ctrl('R'):
            post_reply(0, fptr, (char *) NULL);
            break;
        default:
            break;
        }
        clear();
    }
    setbdir(digestmode, genbuf, currboard);
    brc_add_read(fptr->filename);
    /*
     * return 0;  modified by dong , for clear_new_flag(), 1999.1.20
     * if (strcmp(CurArticleFileName, fptr->filename) == 0)
     * return QUIT;
     * else 
     */
    return 0;

}

int sequential_read(int ent, struct fileheader *fileinfo, char *direct)
{
    readpost = 1;
    clear();
    return sequential_read2(ent);
}
 /*ARGSUSED*/ int sequential_read2(int ent)
{
    char buf[STRLEN];
    int continue_flag;

    sequent_ent = ent;
    continue_flag = 0;
    setbdir(digestmode, buf, currboard);
    apply_record(buf, (APPLY_FUNC_ARG) sequent_messages, sizeof(struct fileheader), &continue_flag, 1, false);
    return FULLUPDATE;
}

int clear_new_flag(int ent, struct fileheader *fileinfo, char *direct)
{
    brc_clear_new_flag(fileinfo->filename);
    return PARTUPDATE;
}

int clear_all_new_flag(int ent, struct fileheader *fileinfo, char *direct)
{
    brc_clear();
    return PARTUPDATE;
}

struct one_key read_comms[] = { /*�Ķ�״̬�������� */
    {'r', read_post},
    {'K', skip_post},
    /*
     * {'u',        skip_post},    rem by Haohmaru.99.11.29 
     */
    {'d', del_post},
    {'D', del_range},
    {'m', mark_post},
    {';', noreply_post},        /*Haohmaru.99.01.01,�趨����reģʽ */
    {'#', sign_post},           /* Bigman: 2000.8.12  �趨���±��ģʽ */
    {'E', edit_post},
    {Ctrl('G'), change_mode},   /* bad : 2002.8.8 add marked mode */
    {'`', digest_mode},
    {'.', deleted_mode},
    {'>', junk_mode},
    {'g', digest_post},
    {'T', edit_title},
    {'s', do_select},
    {Ctrl('C'), do_cross},
    {'Y', UndeleteArticle},     /* Leeward 98.05.18 */
    {Ctrl('P'), do_post},
    {'c', clear_new_flag},
    {'f', clear_all_new_flag},  /* added by dong, 1999.1.25 */
    {'S', sequential_read},
#ifdef INTERNET_EMAIL
    {'F', mail_forward},
    {'U', mail_uforward},
    {Ctrl('R'), post_reply},
#endif
    {'J', Semi_save},
    {'i', Save_post},
    {'I', Import_post},
    {'R', b_results},
    {'V', b_vote},
    {'M', b_vote_maintain},
    {'W', b_notes_edit},
    {'h', mainreadhelp},
    {'X', b_jury_edit},
/*�༭������ٲ�ίԱ����,stephen on 2001.11.1 */
    {KEY_TAB, show_b_note},
    {'x', into_announce},
    {'a', auth_search_down},
    {'A', auth_search_up},
    {'/', t_search_down},
    {'?', t_search_up},
    {'\'', post_search_down},
    {'\"', post_search_up},
    {']', thread_down},
    {'[', thread_up},
    {Ctrl('D'), deny_user},
    {Ctrl('A'), show_author},
    {Ctrl('O'), add_author_friend},
    {Ctrl('Q'), show_authorinfo},       /*Haohmaru.98.12.05 */
    {Ctrl('W'), show_authorBM}, /*cityhunter 00.10.18 */
    {'z', sendmsgtoauthor},     /*Haohmaru.2000.5.19 */
    {'Z', sendmsgtoauthor},     /*Haohmaru.2000.5.19 */
    {Ctrl('N'), SR_first_new},
    {'n', SR_first_new},
    {'\\', SR_last},
    {'=', SR_first},
    {Ctrl('S'), SR_read},
    {'p', SR_read},
    {Ctrl('X'), SR_readX},      /* Leeward 98.10.03 */
    {Ctrl('U'), SR_author},
    {Ctrl('H'), SR_authorX},    /* Leeward 98.10.03 */
    {'b', SR_BMfunc},
    {'B', SR_BMfuncX},          /* Leeward 98.04.16 */
    {Ctrl('T'), title_mode},
    {'t', set_delete_mark},     /*KCN 2001 */
    {'v', i_read_mail},         /* period 2000-11-12 read mail in article list */
    /*
     * {'!',      Goodbye},Haohmaru 98.09.21 
     */
    {Ctrl('Y'), zsend_post},    /* COMMAN 2002 */
    {'\0', NULL},
};

int Read()
{
    char buf[2 * STRLEN];
    char notename[STRLEN];
    time_t usetime;
    struct stat st;

    if (!selboard) {
        move(2, 0);
        prints("����ѡ��������\n");
        pressreturn();
        move(2, 0);
        clrtoeol();
        return -1;
    }
    in_mail = false;
    brc_initial(currentuser->userid, currboard);
    setbdir(digestmode, buf, currboard);

    setvfile(notename, currboard, "notes");
    if (stat(notename, &st) != -1) {
        if (st.st_mtime < (time(NULL) - 7 * 86400)) {
/*            sprintf(genbuf,"touch %s",notename);
	    */
            f_touch(notename);
            setvfile(genbuf, currboard, "noterec");
            unlink(genbuf);
        }
    }
    if (vote_flag(currboard, '\0', 1 /*�������µı���¼û */ ) == 0) {
        if (dashf(notename)) {
            /*
             * period  2000-09-15  disable ActiveBoard while reading notes 
             */
            modify_user_mode(READING);
            /*-	-*/
            ansimore(notename, true);
            vote_flag(currboard, 'R', 1 /*д������µı���¼ */ );
        }
    }
    usetime = time(0);
    i_read(READING, buf, readtitle, (READ_FUNC) readdoent, &read_comms[0], sizeof(struct fileheader));  /*���뱾�� */
    board_usage(currboard, time(0) - usetime);  /*boardʹ��ʱ���¼ */

    return 0;
}

/*Add by SmallPig*/
static int catnotepad(FILE * fp, char *fname)
{
    char inbuf[256];
    FILE *sfp;
    int count;

    count = 0;
    if ((sfp = fopen(fname, "r")) == NULL) {
        fprintf(fp, "[31m[41m�ѩء����������������������������������������������������������������������ء�[m\n\n");
        return -1;
    }
    while (fgets(inbuf, sizeof(inbuf), sfp) != NULL) {
        if (count != 0)
            fputs(inbuf, fp);
        else
            count++;
    }
    fclose(sfp);
    return 0;
}

void notepad()
{
    char tmpname[STRLEN], note1[4];
    char note[3][STRLEN - 4];
    char tmp[STRLEN];
    FILE *in;
    int i, n;
    time_t thetime = time(0);

    clear();
    move(0, 0);
    prints("��ʼ������԰ɣ��������Ŀ�Դ�....\n");
    sprintf(tmpname, "etc/notepad_tmp/%s.notepad", currentuser->userid);
    if ((in = fopen(tmpname, "w")) != NULL) {
        for (i = 0; i < 3; i++)
            memset(note[i], 0, STRLEN - 4);
        while (1) {
            for (i = 0; i < 3; i++) {
                getdata(1 + i, 0, ": ", note[i], STRLEN - 5, DOECHO, NULL, false);
                if (note[i][0] == '\0')
                    break;
            }
            if (i == 0) {
                fclose(in);
                unlink(tmpname);
                return;
            }
            getdata(5, 0, "�Ƿ����Ĵ����������԰� (Y)�ǵ� (N)��Ҫ (E)�ٱ༭ [Y]: ", note1, 3, DOECHO, NULL, true);
            if (note1[0] == 'e' || note1[0] == 'E')
                continue;
            else
                break;
        }
        if (note1[0] != 'N' && note1[0] != 'n') {
            sprintf(tmp, "[32m%s[37m��%.24s��", currentuser->userid, currentuser->username);
            fprintf(in, "[31m[40m�ѩС�����������������������������[37m���������[31m�������������������������������С�[m\n");
            fprintf(in, "[31m����%-43s[33m�� [36m%.19s[33m �뿪ʱ���µĻ�[31m����\n", tmp, Ctime(thetime));
            if (i > 2)
                i = 2;
            for (n = 0; n <= i; n++) {
                if (note[n][0] == '\0')
                    break;
                fprintf(in, "[31m��[m%-74.74s[31m��[m\n", note[n]);
            }
            fprintf(in, "[31m���С����������������������������������������������������������������������С�[m\n");
            catnotepad(in, "etc/notepad");
            fclose(in);
            f_mv(tmpname, "etc/notepad");
        } else {
            fclose(in);
            unlink(tmpname);
        }
    }
    if (talkrequest) {
        talkreply();
    }
    clear();
    return;
}

void record_exit_time()
{                               /* ��¼����ʱ��  Luzi 1998/10/23 */
    currentuser->exittime = time(NULL);
    /*
     * char path[80];
     * FILE *fp;
     * time_t now;
     * sethomefile( path, currentuser->userid , "exit");
     * fp=fopen(path, "wb");
     * if (fp!=NULL)
     * {
     * now=time(NULL);
     * fwrite(&now,sizeof(time_t),1,fp);
     * fclose(fp);
     * }
     */
}

int Goodbye()
{                               /*��վ ѡ�� */
    extern int started;
    time_t stay;
    char fname[STRLEN], notename[STRLEN];
    char sysoplist[20][STRLEN], syswork[20][STRLEN], spbuf[STRLEN], buf[STRLEN];
    int i, num_sysop, choose, logouts, mylogout = false;
    FILE *sysops;
    long Time = 10;             /*Haohmaru */

/*---	��ʾ����¼�Ĺص������Ļ����	2001-07-01	---*/
    modify_user_mode(READING);

    i = 0;
    if ((sysops = fopen("etc/sysops", "r")) != NULL) {
        while (fgets(buf, STRLEN, sysops) != NULL && i < 20) {
            strcpy(sysoplist[i], (char *) strtok(buf, " \n\r\t"));
            strcpy(syswork[i], (char *) strtok(NULL, " \n\r\t"));
            i++;
        }
        fclose(sysops);
    }
    num_sysop = i;
    move(1, 0);
    clear();
    move(0, 0);
    prints("���Ҫ�뿪 %s ���� %s һЩ������\n", BBS_FULL_NAME, BBS_FULL_NAME);
    if (strcmp(currentuser->userid, "guest") != 0)
        prints("[[33m1[m] ���Ÿ�" NAME_BBS_CHINESE "\n");
    prints("[[33m2[m] ����[32m*" NAME_BBS_CHINESE " BBS*[m\n");
    if (strcmp(currentuser->userid, "guest") != 0) {
        if (USE_NOTEPAD == 1)
            prints("[[33m3[m] дд*���԰�*[m\n");
    }
    prints("[[33m4[m] �뿪��BBSվ\n");
    sprintf(spbuf, "���ѡ���� [4]��");
    getdata(7, 0, spbuf, genbuf, 4, DOECHO, NULL, true);
    clear();
    choose = genbuf[0] - '0';
    if (strcmp(currentuser->userid, "guest") && choose == 1) {  /* д�Ÿ�վ�� */
        if (PERM_LOGINOK & currentuser->userlevel) {    /*Haohmaru.98.10.05.ûͨ��ע���ֻ�ܸ�ע��վ������ */
            prints("        ID        �����ְ��\n");
            prints("   ============ =============\n");
            for (i = 1; i <= num_sysop; i++) {
                prints("[[33m%1d[m] [1m%-12s %s[m\n", i, sysoplist[i - 1], syswork[i - 1]);
            }

            prints("[[33m%1d[m] ���������ޣ�\n", num_sysop + 1);      /*���һ��ѡ�� */

            sprintf(spbuf, "���ѡ���� [[32m%1d[m]��", num_sysop + 1);
            getdata(num_sysop + 5, 0, spbuf, genbuf, 4, DOECHO, NULL, true);
            choose = genbuf[0] - '0';
            if (0 != genbuf[1])
                choose = genbuf[1] - '0' + 10;
            if (choose >= 1 && choose <= num_sysop) {
                /*
                 * do_send(sysoplist[choose-1], "ʹ���߼����ĵĽ�����"); 
                 */
                if (choose == 1)        /*modified by Bigman : 2000.8.8 */
                    do_send(sysoplist[0], "��վ���ܹܡ�ʹ���߼����Ľ�����", "");
                else if (choose == 2)
                    do_send(sysoplist[1], "��ϵͳά����ʹ���߼����Ľ�����", "");
                else if (choose == 3)
                    do_send(sysoplist[2], "���������ʹ���߼����Ľ�����", "");
                else if (choose == 4)
                    do_send(sysoplist[3], "�����ȷ�ϡ�ʹ���߼����Ľ�����", "");
                else if (choose == 5)
                    do_send(sysoplist[4], "���ٲ����ˡ�ʹ���߼����Ľ�����", "");
            }
/* added by stephen 11/13/01 */
            choose = -1;
        } else {
            /*
             * ����ע�����ʾ��Ϣ Bigman:2000.10.31 
             */
            prints("\n    �����һֱδ�õ������֤,��ȷ�����Ƿ񵽸��˹�������д��ע�ᵥ,\n");
            prints("    ������յ����ȷ����,��û�з��������Ȩ��,����������дһ��ע�ᵥ\n\n");
            prints("     վ���� ID   �����ְ��\n");
            prints("   ============ =============\n");

            /*
             * added by Bigman: 2000.8.8  �޸���վ 
             */
            prints("[[33m%1d[m] [1m%-12s %s[m\n", 1, sysoplist[3], syswork[3]);
            prints("[[33m%1d[m] ���������ޣ�\n", 2);  /*���һ��ѡ�� */

            sprintf(spbuf, "���ѡ���� %1d��", 2);
            getdata(num_sysop + 6, 0, spbuf, genbuf, 4, DOECHO, NULL, true);
            choose = genbuf[0] - '0';
            if (choose == 1)    /*modified by Bigman : 2000.8.8 */
                do_send(sysoplist[3], "�����ȷ�ϡ�ʹ���߼����Ľ�����", "");
            choose = -1;

            /*
             * for(i=0;i<=3;i++)
             * prints("[[33m%1d[m] [1m%-12s %s[m\n",
             * i,sysoplist[i+4],syswork[i+4]);
             * prints("[[33m%1d[m] ���������ޣ�\n",4); 
 *//*
 * * ���һ��ѡ�� 
 */
            /*
             * sprintf(spbuf,"���ѡ���� [[32m%1d[m]��",4);
             * getdata(num_sysop+6,0, spbuf,genbuf, 4, DOECHO, NULL ,true);
             * choose=genbuf[0]-'0';
             * if(choose==1)
             * do_send(sysoplist[5], "ʹ���߼����ĵĽ�����");
             * else if(choose==2)
             * do_send(sysoplist[6], "ʹ���߼����ĵĽ�����");
             * else if(choose==3)
             * do_send(sysoplist[7], "ʹ���߼����ĵĽ�����");
             * else if(choose==0)
             * do_send(sysoplist[4], "ʹ���߼����ĵĽ�����");
             * choose=-1; 
             */
        }
    }
    if (choose == 2)            /*����BBS */
        return 0;
    if (strcmp(currentuser->userid, "guest") != 0) {
        if (choose == 3)        /*���Բ� */
            if (USE_NOTEPAD == 1 && HAS_PERM(currentuser, PERM_POST))
                notepad();
    }

    clear();
    prints("\n\n\n\n");
    stay = time(NULL) - login_start_time;       /*��������ʱ�� */

    currentuser->stay += stay;

    if (DEFINE(currentuser, DEF_OUTNOTE /*�˳�ʱ��ʾ�û�����¼ */ )) {
        sethomefile(notename, currentuser->userid, "notes");
        if (dashf(notename))
            ansimore(notename, true);
    }

    /*
     * Leeward 98.09.24 Use SHARE MEM and disable the old code 
     */
    if (DEFINE(currentuser, DEF_LOGOUT)) {      /* ʹ���Լ�����վ���� */
        sethomefile(fname, currentuser->userid, "logout");
        if (dashf(fname))
            mylogout = true;
    }
    if (mylogout) {
        logouts = countlogouts(fname);  /* logouts Ϊ ��վ���� ���� */
        if (logouts >= 1) {
            user_display(fname, (logouts == 1) ? 1 : (currentuser->numlogins % (logouts)) + 1, true);
        }
    } else {
        logouts = countlogouts("etc/logout");   /* logouts Ϊ ��վ���� ���� */
        user_display("etc/logout", rand() % logouts + 1, true);
    }

    /*
     * if(DEFINE(currentuser,DEF_LOGOUT\*ʹ���Լ�����վ����*\)) Leeward: disable the old code
     * {
     * sethomefile( fname,currentuser->userid, "logout" );
     * if(!dashf(fname))
     * strcpy(fname,"etc/logout");
     * }else
     * strcpy(fname,"etc/logout");
     * if(dashf(fname))
     * {
     * logouts=countlogouts(fname);      \* logouts Ϊ ��վ���� ���� *\
     * if(logouts>=1)
     * {
     * user_display(fname,(logouts==1)?1:
     * (currentuser->numlogins%(logouts))+1,true);
     * }
     * } 
     */
    report("exit");

    /*
     * stay = time(NULL) - login_start_time;    ��������ʱ�� 
     */
    /*
     * Haohmaru.98.11.10.���ж��Ƿ�����վ�� 
     */
    if ( /*strcmp(currentuser->username,"guest")&& */ stay <= Time) {
        char lbuf[256];
        char tmpfile[256];
        FILE *fp;

        strcpy(lbuf, "����-");
        strftime(lbuf + 5, 30, "%Y-%m-%d%Y:%H:%M", localtime(&login_start_time));
        sprintf(tmpfile, "tmp/.tmp%d", getpid());
        fp = fopen(tmpfile, "w");
        if (fp) {
            fputs(lbuf, fp);
            fclose(fp);
            mail_file(currentuser->userid, tmpfile, "surr", "����", 1);
        }
    }
    /*
     * stephen on 2001.11.1: ��վ����5���Ӳ�������վ���� 
     */
    if (stay <= 300 && currentuser->numlogins > 5) {
        currentuser->numlogins--;
        if (currentuser->stay > stay)
            currentuser->stay -= stay;
    }
    if (started) {
        record_exit_time();     /* ��¼�û����˳�ʱ�� Luzi 1998.10.23 */
        /*---	period	2000-10-19	4 debug	---*/
        /*
         * sprintf( genbuf, "Stay:%3ld (%s)", stay / 60, currentuser->username ); 
         */
        bbslog("1system", "EXIT: Stay:%3ld (%s)[%d %d]", stay / 60, currentuser->username, utmpent, usernum);
        u_exit();
        started = 0;
    }

    if (num_user_logins(currentuser->userid) == 0 || !strcmp(currentuser->userid, "guest")) {   /*��黹��û���������� */
        FILE *fp;
        char buf[STRLEN], *ptr;

        sethomefile(fname, currentuser->userid, "msgindex");
        unlink(fname);
        sethomefile(fname, currentuser->userid, "msgfile");
        if (DEFINE(currentuser, DEF_MAILMSG /*��վʱ�Ļ�������Ϣ */ ) && dashf(fname)) {
            char title[STRLEN];
            time_t now;

            now = time(0);
            sprintf(title, "[%12.12s] ����ѶϢ����", ctime(&now) + 4);
            mail_file(currentuser->userid, fname, currentuser->userid, title, 1);
        } else
            unlink(fname);
        fp = fopen("friendbook", "r");  /*����ϵͳ Ѱ������ */
        while (fp != NULL && fgets(buf, sizeof(buf), fp) != NULL) {
            char uid[14];

            ptr = strstr(buf, "@");
            if (ptr == NULL) {
                del_from_file("friendbook", buf);
                continue;
            }
            ptr++;
            strcpy(uid, ptr);
            ptr = strstr(uid, "\n");
            *ptr = '\0';
            if (!strcmp(uid, currentuser->userid))      /*ɾ�����û��� Ѱ������ */
                del_from_file("friendbook", buf);       /*Ѱ������ֻ�ڱ���������Ч */
        }
        if (fp)                                                    /*---	add by period 2000-11-11 fix null hd bug	---*/
            fclose(fp);
    }
    sleep(1);
    pressreturn();              /*Haohmaru.98.10.18 */
    shutdown(0, 2);
    close(0);
    exit(0);
    return -1;
}



int Info()
{                               /* ��ʾ�汾��ϢVersion.Info */
    modify_user_mode(XMENU);
    ansimore("Version.Info", true);
    clear();
    return 0;
}

int Conditions()
{                               /* ��ʾ��Ȩ��ϢCOPYING */
    modify_user_mode(XMENU);
    ansimore("COPYING", true);
    clear();
    return 0;
}

int ShowWeather()
{                               /* ��ʾ�汾��ϢVersion.Info */
    modify_user_mode(XMENU);
    ansimore("WEATHER", true);
    clear();
    return 0;
}

int Welcome()
{                               /* ��ʾ��ӭ���� Welcome */
    modify_user_mode(XMENU);
    ansimore("Welcome", true);
    clear();
    return 0;
}

int cmpbnames(char *bname, struct fileheader *brec)
{
    if (!strncasecmp(bname, brec->filename, sizeof(brec->filename)))
        return 1;
    else
        return 0;
}

void RemoveAppendedSpace(char *ptr)
{                               /* Leeward 98.02.13 */
    int Offset;

    /*
     * Below block removing extra appended ' ' in article titles 
     */
    Offset = strlen(ptr);
    for (--Offset; Offset > 0; Offset--) {
        if (' ' != ptr[Offset])
            break;
        else
            ptr[Offset] = 0;
    }
}

int i_read_mail()
{
    char savedir[STRLEN];

    /*
     * should set digestmode to false while read mail. or i_read may cause error 
     */
    int savemode;
    int mode;

    strcpy(savedir, currdirect);
    savemode = digestmode;
    digestmode = false;
    mode = m_read();
    digestmode = savemode;
    strcpy(currdirect, savedir);
    return mode;
}

int set_delete_mark(int ent, struct fileheader *fileinfo, char *direct)
{
    change_post_flag(ent, fileinfo, direct, FILE_DELETE_FLAG, 1);
}

int change_post_flag(int ent, struct fileheader *fileinfo, char *direct, int flag, int prompt)
{
    /*---	---*/
    int newent = 0, ret = 1;
    char *ptr, buf[STRLEN];
    char ans[256];
    struct fileheader mkpost;
    struct flock ldata;
    int fd, size = sizeof(fileheader);

    /*---	---*/

    if (!chk_currBM(currBM, currentuser))
        return DONOTHING;

    if (flag == FILE_DIGEST_FLAG && (digestmode == 1 || digestmode == 2 || digestmode == 3 || digestmode == 4 || digestmode == 5))
        /*
         * ��ժģʽ�� ���� �����ժ, ���պ�ֽ¨ģʽҲ���� 
         */
        return DONOTHING;
    if (flag == FILE_MARK_FLAG && (digestmode == 1 || digestmode == 2 || digestmode == 3 || digestmode == 4 || digestmode == 5))
        /*
         * ��ժģʽ�� ���� �����ժ, ���պ�ֽ¨ģʽҲ���� 
         */
        return DONOTHING;
    if (flag == FILE_DELETE_FLAG && digestmode != 0 && digestmode != 1 && digestmode != 3)
        return DONOTHING;
    if ((flag == FILE_MARK_FLAG || flag == FILE_DELETE_FLAG) && (!strcmp(currboard, "syssecurity")
                                                                 || !strcmp(currboard, "Filter")))
        return DONOTHING;       /* Leeward 98.03.29 */
    /*
     * Haohmaru.98.10.12.����ģʽ�²�����mark���� 
     */
    if ((flag <= FILE_DELETE_FLAG) && strstr(direct, "/.THREAD"))
        return DONOTHING;

    strcpy(buf, direct);
    ptr = strrchr(buf, '/') + 1;
    ptr[0] = '\0';
    sprintf(&genbuf[512], "%s%s", buf, fileinfo->filename);
    if (!dashf(&genbuf[512]))
        ret = 0;                /* ����һ��newent :PP   */

    if (ret)
        if ((fd = open(direct, O_RDWR | O_CREAT, 0644)) == -1)
            ret = 0;
    if (ret) {
        ldata.l_type = F_RDLCK;
        ldata.l_whence = 0;
        ldata.l_len = size;
        ldata.l_start = size * (ent - 1);
        if (fcntl(fd, F_SETLKW, &ldata) == -1) {
            report("reclock error");
            close(fd);
                                /*---	period	2000-10-20	file should be closed	---*/
            ret = 0;
        }
    }
    if (ret) {
        if (lseek(fd, size * (ent - 1), SEEK_SET) == -1) {
            report("subrec seek err");
            /*---	period	2000-10-24	---*/
            ldata.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &ldata);
            close(fd);
            ret = 0;
        }
    }
    if (ret) {
        if (get_record_handle(fd, &mkpost, sizeof(mkpost), ent) == -1) {
            report("subrec read err");
            ret = 0;
        }
        if (ret)
            if (strcmp(mkpost.filename, fileinfo->filename))
                ret = 0;
        if (!ret) {
            newent = search_record_back(fd, sizeof(struct fileheader), ent, (RECORD_FUNC_ARG) cmpfileinfoname, fileinfo->filename, &mkpost, 1);
            ret = (newent > 0);
            if (ret)
                memcpy(fileinfo, &mkpost, sizeof(mkpost));
            else {
                ldata.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &ldata);
                close(fd);
            }
            ent = newent;
        }
    }
    if (!ret) {
        move(2, 0);
        prints(" �����б����䶯������[%s]�����ѱ�ɾ����\n", fileinfo->title);
        clrtobot();
        pressreturn();
        return DIRCHANGED;
    }
    switch (flag) {
    case FILE_MARK_FLAG:
        if (fileinfo->accessed[0] & FILE_MARKED)        //added by bad 2002.8.7 mark file mode added
            fileinfo->accessed[0] = (fileinfo->accessed[0] & ~FILE_MARKED);
        else
            fileinfo->accessed[0] = fileinfo->accessed[0] | FILE_MARKED;
        setboardmark(currboard, 1);
        break;
    case FILE_NOREPLY_FLAG:
        if (fileinfo->accessed[1] & FILE_READ) {
            fileinfo->accessed[1] &= ~FILE_READ;
            if (prompt)
                a_prompt(-1, " ��������ȡ������reģʽ, �밴 Enter ���� << ", ans);
        } else {
            fileinfo->accessed[1] |= FILE_READ;
            if (prompt)
                a_prompt(-1, " ����������Ϊ����reģʽ, �밴 Enter ���� << ", ans);
            /*
             * Bigman:2000.8.29 sysmail�洦����Ӱ������� 
             */
            if (!strcmp(currboard, "sysmail")) {
                sprintf(ans, "��%s�� ����: %s", currentuser->userid, fileinfo->title);
                strncpy(fileinfo->title, ans, STRLEN);
                fileinfo->title[STRLEN - 1] = 0;
            }
        }
        break;
    case FILE_SIGN_FLAG:
        if (fileinfo->accessed[0] & FILE_SIGN) {
            fileinfo->accessed[0] &= ~FILE_SIGN;
            if (prompt)
                a_prompt(-1, " �������ѳ������ģʽ, �밴 Enter ���� << ", ans);
        } else {
            fileinfo->accessed[0] |= FILE_SIGN;
            if (prompt)
                a_prompt(-1, " ����������Ϊ���ģʽ, �밴 Enter ���� << ", ans);
        }
        break;
    case FILE_DELETE_FLAG:
        if (fileinfo->accessed[1] & FILE_DEL)
            fileinfo->accessed[1] &= ~FILE_DEL;
        else
            fileinfo->accessed[1] |= FILE_DEL;
        break;
    case FILE_DIGEST_FLAG:
        if (fileinfo->accessed[0] & FILE_DIGEST) {      /* ����Ѿ�����ժ�Ļ��������ժ��ɾ����post */
            fileinfo->accessed[0] = (fileinfo->accessed[0] & ~FILE_DIGEST);
            dele_digest(fileinfo->filename, direct);
        } else {
            struct fileheader digest;
            char *ptr, buf[64];

            memcpy(&digest, fileinfo, sizeof(digest));
            digest.filename[0] = 'G';
            strcpy(buf, direct);
            ptr = strrchr(buf, '/') + 1;
            ptr[0] = '\0';
            sprintf(genbuf, "%s%s", buf, digest.filename);
            if (dashf(genbuf)) {
                fileinfo->accessed[0] = fileinfo->accessed[0] | FILE_DIGEST;
            } else {
                digest.accessed[0] = 0;
                sprintf(&genbuf[512], "%s%s", buf, fileinfo->filename);
                link(&genbuf[512], genbuf);
                strcpy(ptr, DIGEST_DIR);
                if (get_num_records(buf, sizeof(digest)) > MAX_DIGEST) {
                    ldata.l_type = F_UNLCK;
                    fcntl(fd, F_SETLK, &ldata);
                    close(fd);
                    move(3, 0);
                    clrtobot();
                    move(4, 10);
                    prints("��Ǹ�������ժ�����Ѿ����� %d ƪ���޷��ټ���...\n", MAX_DIGEST);
                    pressanykey();
                    return PARTUPDATE;
                }
                append_record(buf, &digest, sizeof(digest));    /* ��ժĿ¼����� .DIR */
                fileinfo->accessed[0] = fileinfo->accessed[0] | FILE_DIGEST;
            }
        }
        break;
    }

    if (lseek(fd, size * (ent - 1), SEEK_SET) == -1) {
        report("subrec seek err");
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &ldata);
        close(fd);
        return DONOTHING;
    }
    if (safewrite(fd, fileinfo, size) != size) {
        report("subrec write err");
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &ldata);
        close(fd);
        return DONOTHING;
    }

    ldata.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &ldata);
    close(fd);

    return newent ? DIRCHANGED : PARTUPDATE;
}
