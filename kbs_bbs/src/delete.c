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

int
d_board()
{
    struct boardheader binfo ;
    int bid ;
    char bname[STRLEN];
    extern char lookgrp[];
    extern int numboards ;

    modify_user_mode( ADMIN );
    if(!check_systempasswd())
    {
        return;
    }
    clear();
    stand_title( "ɾ��������" );
    make_blist() ;
    move(1,0) ;
    namecomplete( "������������: ",genbuf) ;
    if( genbuf[0] == '\0' )
        return 0;
    bid = getbnum(genbuf) ;
    if( get_record(BOARDS,&binfo,sizeof(binfo),bid) == -1 ) {
        move(2,0) ;
        prints("����ȷ��������\n") ;
        pressreturn() ;
        clear() ;
        return 0 ;
    }
    strcpy(bname,genbuf);
    move(1,0) ;
    prints( "ɾ�������� '%s'.", binfo.filename );
    clrtoeol();
    getdata(2,0,"(Yes, or No) [N]: ",genbuf,4,DOECHO,NULL,YEA) ;
    if( genbuf[0] != 'Y' && genbuf[0] != 'y') { /* if not yes quit */
        move(2,0) ;
        prints("ȡ��ɾ��....\n") ;
        pressreturn() ;
        clear() ;
        return 0 ;
    }
    {
        char        secu[STRLEN];
        sprintf(secu,"ɾ����������%s",binfo.filename);
        securityreport(secu,NULL);
    }
    if(seek_in_file("0Announce/.Search",bname))
    {
        getdata(3,0,"�Ƴ������� (Yes, or No) [Y]: ",genbuf,4,DOECHO,NULL,YEA) ;
        if( genbuf[0] != 'N' && genbuf[0] != 'n')
        {
            get_grp(binfo.filename);
            del_grp(lookgrp,binfo.filename,binfo.title+13);
        }
    }
    if(seek_in_file("etc/anonymous",bname))
        del_from_file("etc/anonymous",bname);
    if(seek_in_file("0Announce/.Search",bname))
        del_from_file("0Announce/.Search",bname);

    if( binfo.filename[0] == '\0' ) return -1; /* rrr - precaution */
    sprintf(genbuf, "deleted board %s", binfo.filename);
    report(genbuf);
    sprintf(genbuf,"/bin/rm -fr boards/%s",binfo.filename) ;
    system(genbuf) ;
    sprintf(genbuf,"/bin/rm -fr vote/%s",binfo.filename) ;
    system(genbuf) ;

    sprintf( genbuf, " << '%s'�� %s ɾ�� >>",
             binfo.filename, currentuser->userid );
    memset( &binfo, 0, sizeof( binfo ) );
    strcpy( binfo.title, genbuf );
    binfo.level = PERM_SYSOP;
    substitute_record( BOARDS, &binfo, sizeof( binfo ), bid );
    reload_boards();
    move(4,0) ;
    prints("���������Ѿ�ɾ��...\n") ;
    pressreturn() ;
    numboards = -1 ;
    clear() ;
    return 0 ;
}

suicide()
{
    char buf[STRLEN];
    FILE *fn;
    time_t now;
    char filename[STRLEN];
#define XPERMSTR "bTCPRp#@XWBA$VS!DEM1234567890%"
    char XPERM[48];
    int  oldXPERM;
    int  num;

    modify_user_mode( OFFLINE );
    if(HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_BOARDS)||HAS_PERM(PERM_OBOARDS)||HAS_PERM(PERM_ACCOUNTS)||HAS_PERM(PERM_ANNOUNCE)||HAS_PERM(PERM_SUICIDE)||HAS_PERM(PERM_CHATOP)||HAS_PERM(PERM_DENYPOST)||HAS_PERM(PERM_DENYMAIL))
    {
        clear();
        move(11,28);
        prints("[1m[33m������������������ɱ��[m");
        pressanykey();
        return;
    }

    clear();
    move(1,0);
    prints("ѡ����ɱ��ʹ�������������ٵ�14�죬14��������ʺ��Զ���ʧ��");
    move(3,0);
    prints("����14�������ı�����Ļ��������ͨ����¼��վһ�λָ�ԭ������");
    move(5,0);
    prints("��ɱ�û�����ʧ����[33m����Ȩ��[0m������");
    move(7,0);
    /*
        clear();
        move(1,0);
        prints("ѡ����ɱ��ʹ�����ʺ��Զ�������ʧ!");
        move(3,0);
        prints("�����ʺŽ����ϴ�ϵͳ��ɾ��");
        */

    if(askyn("��ȷ��Ҫ��ɱ��",0)==1)
    {
        clear();
        getdata(0,0,"������ԭ����(������ȷ�Ļ������̶���): ",buf,39,NOECHO,NULL,YEA);/*Haohmaru,98.10.12,check the passwds*/
        if( *buf == '\0' || !checkpasswd2( buf,&currentuser )) {
            prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
            pressanykey();
            return;
        }

        oldXPERM=currentuser->userlevel;
        strcpy(XPERM, XPERMSTR);
        for ( num = 0; num < strlen(XPERM); num++ )
            if ( !(oldXPERM & (1 << num)) )
                XPERM[num] = ' ';
        XPERM[num] = '\0';
        currentuser->userlevel&=0x3F;/*Haohmaru,99.3.20.��ɱ��ֻ��������Ȩ��*/
        currentuser->userlevel^=PERM_SUICIDE;
        substitute_record(PASSFILE,&currentuser,sizeof(currentuser),usernum);
        /*Haohmaru.99.3.20.��ɱ֪ͨ*/
        now=time(0);
        sprintf(filename,"etc/%s.tmp",currentuser->userid);
        fn=fopen(filename,"w");
        fprintf(fn,"[1m%s[m �Ѿ��� [1m%24.24s[m ��ɱ�ˣ��������������ϣ��뱣��...",currentuser->userid,ctime(&now));
        fprintf(fn,"\n\n��������ɱ��ԭ����Ȩ��\n\033[1m\033[33m%s\n[0m", XPERM);
        getuinfo(fn, &currentuser);
        fprintf(fn,"\n                      [1m ϵͳ�Զ�����ϵͳ��[m\n");
        fclose(fn);
        sprintf(buf,"%s ����ɱ֪ͨ",currentuser->userid);
        postfile(filename,"Goodbye",buf,1);
        unlink(filename);

        /*kick_user(&uinfo);
        exit(0);*/
        abort_bbs();
    }
}


offline()
{
    char buf[STRLEN];

    modify_user_mode( OFFLINE );

    if(HAS_PERM(PERM_SYSOP))
        return;
    clear();
    move(1,0);
    prints("[32m���ѹ��.....[m");
    move(3,0);
    if(askyn("��ȷ��Ҫ�뿪������ͥ",0)==1)
    {
        clear();
        if(d_user(currentuser->userid)==1)
        {
            mail_info();
            kick_user(&uinfo);
            exit(0);
        }
    }
}

getuinfo(fn, ptr_urec)
FILE *fn;
struct userec *ptr_urec;
{
    fprintf(fn,"\n\n���Ĵ���     : %s\n", ptr_urec->userid);
    fprintf(fn,"�����ǳ�     : %s\n", ptr_urec->username);
    fprintf(fn,"��ʵ����     : %s\n", ptr_urec->realname);
    fprintf(fn,"��ססַ     : %s\n", ptr_urec->address);
    fprintf(fn,"�����ʼ����� : %s\n", ptr_urec->email);
    fprintf(fn,"��ʵ E-mail  : %s\n", ptr_urec->realemail);
    fprintf(fn,"Ident ����   : %s\n", ptr_urec->ident);
    fprintf(fn,"ע������     : %s", ctime( &ptr_urec->firstlogin));
    fprintf(fn,"����������� : %s", ctime( &ptr_urec->lastlogin));
    fprintf(fn,"������ٻ��� : %s\n", ptr_urec->lasthost );
    fprintf(fn,"��վ����     : %d ��\n", ptr_urec->numlogins);
    fprintf(fn,"������Ŀ     : %d / %d (Board/1Discuss)\n",
            ptr_urec->numposts, post_in_tin( ptr_urec->userid ));
}

mail_info()
{
    FILE *fn;
    time_t now;
    char filename[STRLEN];

    now=time(0);
    sprintf(filename,"etc/%s.tmp",currentuser->userid);
    fn=fopen(filename,"w");
    fprintf(fn,"[1m%s[m �Ѿ��� [1m%24.24s[m ��ɱ�ˣ���������(��)�����ϣ��뱣��...",currentuser->userid
            ,ctime(&now));
    getuinfo(fn, &currentuser);
    fprintf(fn,"\n                      [1m ϵͳ�Զ�����ϵͳ��[m\n");
    fclose(fn);
    mail_file(filename,"acmanager","��ɱ֪ͨ....");
    unlink(filename);
}


int
d_user(cid)
char cid[IDLEN];
{
    int id,fd ;
    char tmpbuf [30];
    struct userec* lookupuser;

    if(uinfo.mode!=OFFLINE)
    {
        modify_user_mode( ADMIN );
        if(!check_systempasswd())
        {
            return;
        }
        clear();
        stand_title( "ɾ��ʹ�����ʺ�" );
        move(1,0) ;
        usercomplete("��������ɾ����ʹ���ߴ���: ",genbuf);
        if(*genbuf == '\0') {
            clear() ;
            return 0 ;
        }
    }else
        strcpy(genbuf,cid);
    if(!(id = getuser(genbuf,&lookupuser))) {
        move(3,0) ;
        prints("�����ʹ���ߴ���...") ;
        clrtoeol() ;
        pressreturn() ;
        clear() ;
        return 0 ;
    }
    /*    if (!isalpha(lookupuser->userid[0])) return 0;*/
    /* rrr - don't know how...*/
    move(1,0) ;
    if(uinfo.mode!=OFFLINE)
        prints("ɾ��ʹ���� '%s'.",genbuf) ;
    else
        prints(" %s ���뿪����",cid);
    clrtoeol();
    getdata(2,0,"(Yes, or No) [N]: ",genbuf,4,DOECHO,NULL,YEA) ;
    if(genbuf[0] != 'Y' && genbuf[0] != 'y') { /* if not yes quit */
        move(2,0) ;
        if(uinfo.mode!=OFFLINE)
            prints("ȡ��ɾ��ʹ����...\n") ;
        else
            prints("����춻���ת���ˣ��ø����...");
        pressreturn() ;
        clear() ;
        return 0 ;
    }
    if(uinfo.mode!=OFFLINE)
    {
        char        secu[STRLEN];
        sprintf(secu,"ɾ��ʹ���ߣ�%s",lookupuser->userid);
        securityreport(secu,lookupuser);
    }
    sprintf(genbuf, "%s deleted user %s", currentuser->userid,lookupuser->userid);
    report(genbuf);
    /*Haohmaru.99.12.23.��ɾIDһ�����ڲ���ע��*/
    if((fd = open(".badname",O_WRONLY|O_CREAT,0644)) != -1 ) {
        char buf[STRLEN] ;
        char thtime[40];
        time_t dtime;
        dtime = time(0);
        sprintf(thtime,"%d",dtime);
        flock(fd,LOCK_EX) ;
        lseek(fd,0,SEEK_END) ;
        sprintf(buf,"%-12.12s %-66.66s\n",lookupuser->userid, thtime) ;
        write(fd,buf,strlen(buf)) ;
        flock(fd,LOCK_UN) ;
        close(fd) ;
    }
    else{
        printf("�����뱨�����");
        pressanykey();}
    setmailpath(tmpbuf, lookupuser->userid);
    sprintf(genbuf,"/bin/rm -fr %s", tmpbuf) ;
    system(genbuf) ;
    sethomepath(tmpbuf, lookupuser->userid);
    sprintf(genbuf,"/bin/rm -fr %s", tmpbuf) ;
    system(genbuf) ;
    sprintf(genbuf,"/bin/rm -fr tmp/email_%s", lookupuser->userid) ;
    system(genbuf) ;
    lookupuser->userlevel = 0;
    strcpy(lookupuser->address, "");
    strcpy(lookupuser->username, "");
    strcpy(lookupuser->realname, "");
    lookupuser->userid[0] = '\0' ;
    substitute_record(PASSFILE,&lookupuser,sizeof(lookupuser),id) ;
    setuserid( id, lookupuser->userid );
    move(2,0) ;
    prints("%s �Ѿ��Ѿ��ͱ���ͥʧȥ����....\n",lookupuser->userid) ;
    pressreturn() ;

    clear() ;
    return 1 ;
}

/* to be Continue fix kick user problem */
int
kick_user(struct user_info *userinfo)
{
    int id, ind ;
    struct user_info uin;
    struct userec kuinfo;
    char kickuser[40], buffer [40];

    if(uinfo.mode!=LUSERS&&uinfo.mode!=OFFLINE&&uinfo.mode!=FRIEND)
    {
        modify_user_mode( ADMIN );
        stand_title( "Kick User" );
        move(1,0) ;
        usercomplete("Enter userid to be kicked: ",kickuser) ;
        if(*kickuser == '\0') {
            clear() ;
            return 0 ;
        }
        if(!(id = searchuser(kickuser))) { /* change getuser -> searchuser, by dong, 1999.10.26 */
            move(3,0) ;
            prints("Invalid User Id") ;
            clrtoeol() ;
            pressreturn() ;
            clear() ;
            return 0 ;
        }
        move(1,0) ;
        prints("Kick User '%s'.",kickuser) ;
        clrtoeol();
        getdata(2,0,"(Yes, or No) [N]: ",genbuf,4,DOECHO,NULL,YEA) ;
        if(genbuf[0] != 'Y' && genbuf[0] != 'y') { /* if not yes quit */
            move(2,0) ;
            prints("Aborting Kick User\n") ;
            pressreturn() ;
            clear() ;
            return 0 ;
        }
        /*search_record(PASSFILE, &kuinfo, sizeof(kuinfo), cmpuids, kickuser);modified by dong, 1998.11.2 ������*/
        ind = search_ulist( &uin, t_cmpuids, id );
    }else
    {
        uin=*userinfo;
        strcpy(kickuser,uin.userid);
        /*        id = getuser(kickuser);
                search_record(PASSFILE, &kuinfo, sizeof(kuinfo), cmpuids, kickuser);
                ind = search_ulist( &uin, t_cmpuids, id );*/
        ind=YEA;
    }
    if (!ind || !uin.active || (kill(uin.pid,0) == -1)) {
        if(uinfo.mode!=LUSERS&&uinfo.mode!=OFFLINE&&uinfo.mode!=FRIEND)
        {
            move(3,0) ;
            prints("User Has Logged Out") ;
            clrtoeol() ;
            pressreturn() ;
            clear() ;
        }
        return 0 ;
    }
    if (kill(uin.pid,SIGHUP) == -1)
    {
    	clear_utmp2(userinfo);
    }
    sprintf(buffer, "kicked %s", kickuser);
    report(buffer);
    /*sprintf( genbuf, "%s (%s)", kuinfo.userid, kuinfo.username );modified by dong, 1998.11.2 */
    /*log( "1system", "KICK %s (%s)", uin.userid, uin.username );*/
    /*    uin.active = NA;
        uin.pid = 0;
        uin.invisible = YEA;
        uin.sockactive = 0;
        uin.sockaddr = 0;
        uin.destuid = 0;
        update_ulist( &uin, ind ); ��������Ҳ����д�����ע�͵� dong 1998.7.7*/
    move(2,0) ;
    if(uinfo.mode!=LUSERS&&uinfo.mode!=OFFLINE&&uinfo.mode!=FRIEND)
    {
        prints("User has been Kicked\n") ;
        pressreturn() ;
        clear() ;
    }
    return 1 ;
}

