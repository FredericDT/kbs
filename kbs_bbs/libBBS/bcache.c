/* share memory ����
                            bcache:�� , ucache:����ע���û�, utmp cache:����user 
*/

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
#include <sys/ipc.h>
#include <sys/shm.h>
#define chartoupper(c)  ((c >= 'a' && c <= 'z') ? c+'A'-'a' : c)


struct BCACHE   *brdshm;
struct UTMPFILE *utmpshm;
struct userec lookupuser;
struct shortfile *bcache;
int     numboards = -1 ;

void
attach_err( shmkey, name )
int     shmkey;
char    *name;
{
    sprintf( genbuf, "Error! %s error! key = %x.\n", name, shmkey );
    write( 1, genbuf, strlen( genbuf ) );
    exit( 1 );
}

void *
attach_shm( shmstr, defaultkey, shmsize )  /* attach share memory */
char    *shmstr;
int     defaultkey, shmsize;
{
    void        *shmptr;
    int         shmkey, shmid;

    shmkey = sysconf_eval( shmstr );
    if( shmkey < 1024 )
        shmkey = defaultkey;
    shmid = shmget( shmkey, shmsize, 0 );
    if( shmid < 0 ) {
        shmid = shmget( shmkey, shmsize, IPC_CREAT | 0660 ); /* modified by dong , for web application , 1998.12.1 */
        if( shmid < 0 )
            attach_err( shmkey, "shmget" );
        shmptr = (void *) shmat( shmid, NULL, 0 );
        if( shmptr == (void *)-1 )
            attach_err( shmkey, "shmat" );
        memset( shmptr, 0, shmsize );
    } else {
        shmptr = (void *) shmat( shmid, NULL, 0 );
        if( shmptr == (void *)-1 )
            attach_err( shmkey, "shmat" );
    }
    return shmptr;
}

int
fillbcache(fptr)/*fill board cache */
struct boardheader *fptr ;
{
    struct shortfile *bptr;

    if( numboards >= MAXBOARD )
        return 0;
    bptr = &bcache[ numboards++ ];
    memcpy( bptr, fptr, sizeof( struct shortfile ) );
    return 0 ;
}

/* Ϊ�����������ʱ�����flock, �ض�BOARDS, ��ԭ����resolve_boards��Ϊreload_boards,
   ����д��resolve_boards. ylsdd 2001.4.26 */
void
reload_boards()
{
    struct stat st;
    time_t      now;
    int lockfd;

    if( brdshm == NULL ) {
        brdshm = attach_shm( "BCACHE_SHMKEY", 3693, sizeof( *brdshm ) ); /* attach board share memory*/
    }
    numboards = brdshm->number;
    bcache = brdshm->bcache;
    now = time( NULL );

    lockfd = open( "bcache.lock", O_RDWR|O_CREAT, 0600 );

    if( lockfd < 0 ) {
        log_usies( "CACHE", "reload bcache lock error!!!!" );
        return;
    }

    flock(lockfd,LOCK_EX);

    if( stat( BOARDS, &st ) < 0 ) {
        st.st_mtime = now - 3600;
    }
    if( brdshm->uptime < st.st_mtime || brdshm->uptime < now - 3600 ) { /* ����update board share memory*/
        log_usies( "CACHE", "reload bcache" );
        /*brdshm->uptime = now;*/
        numboards = 0;
        apply_record( BOARDS, fillbcache, sizeof(struct boardheader) );
        brdshm->number = numboards;
        brdshm->uptime = now;
    }
    flock(lockfd,LOCK_UN);
    close(lockfd);
}

void
resolve_boards()
{
    struct stat st;
    time_t      now;
    int lockfd;
    static int n=0;

    if( brdshm == NULL ) {
        brdshm = attach_shm( "BCACHE_SHMKEY", 3693, sizeof( *brdshm ) ); /* attach board share memory*/
    }
    numboards = brdshm->number;
    bcache = brdshm->bcache;
    
    if( brdshm->uptime < 999999 ) {
        reload_boards();
        return;
    }

    n++;
    if(n%50) return;
    
    now = time( NULL );
    if( stat( BOARDS, &st ) < 0 ) {
        st.st_mtime = now - 3600;
    }
    if( brdshm->uptime < st.st_mtime || brdshm->uptime < now - 3600 )
        reload_boards();
}

int
apply_boards(func) /* �����а� Ӧ�� func����*/
int (*func)() ;
{
    register int i ;

    resolve_boards();
    for(i=0;i<numboards;i++)
        if( bcache[i].level & PERM_POSTMASK || HAS_PERM( bcache[i].level ) || (bcache[i].level&PERM_NOZAP))
            if((*func)(&bcache[i]) == QUIT)
                return QUIT;
    return 0;
}

struct shortfile *
            getbcache( bname ) /* get board cache, ͨ��board name */
            char *bname ;
{
    register int i ;

    resolve_boards();
    for(i=0;i<numboards;i++)
        if( bcache[i].level & PERM_POSTMASK || HAS_PERM( bcache[i].level ) || (bcache[i].level&PERM_NOZAP))
            if( !strncasecmp( bname, bcache[i].filename, STRLEN ) )
                return &bcache[i];
    return NULL;
}

int
getbnum( bname ) /* board name --> board No. */
char    *bname;
{
    register int i;

    resolve_boards();/* attach shm*/
    for(i=0;i<numboards;i++)
        if( bcache[i].level & PERM_POSTMASK || HAS_PERM( bcache[i].level )|| (bcache[i].level&PERM_NOZAP))
            if(!strncasecmp( bname, bcache[i].filename, STRLEN ) )
                return i+1 ;
    return 0 ;
}
/*---	added by period		2000-11-07	to be used in postfile	---*/
int
getboardnum( bname ) /* board name --> board No. & not check level */
char    *bname;
{
    register int i;

    resolve_boards();/* attach shm*/
    for(i=0;i<numboards;i++)
        if(!strncasecmp( bname, bcache[i].filename, STRLEN ) )
            return i+1 ;
    return 0 ;
} /*---	---*/

int
haspostperm(bname) /* �ж��� bname�� �Ƿ���postȨ */
char *bname;
{
    register int i;

    if(digestmode)
        return 0;
    /*    if( strcmp( bname, DEFAULTBOARD ) == 0 )  return 1; change by KCN 2000.09.01 */
    if ((i = getbnum(bname)) == 0) return 0;
    if (HAS_PERM(PERM_DENYPOST))
        /*if(!strcmp(bname, "sysop"))
               return 1;*/ /* Leeward 98.05.21 revised by stephen 2000.10.27*/ 
        /* let user denied post right post at Complain*/
    {if (!strcmp(bname, "Complain")) return 1;/* added by stephen 2000.10.27*/
        else if(!strcmp(bname, "sysop"))
            return 1;} /* stephen 2000.10.27 */
    if (!HAS_PERM(PERM_POST)) return 0;
    return (HAS_PERM((bcache[i-1].level&~PERM_NOZAP) & ~PERM_POSTMASK));
}

int
normal_board(bname) /* bname�� �Ƿ��� normal level */
char *bname;
{
    register int i;

    if( strcmp( bname, DEFAULTBOARD ) == 0 )  return 1;
    if ((i = getbnum(bname)) == 0) return 0;
    return (bcache[i-1].level==0);
}

void
resolve_utmp()
{
    if( utmpshm == NULL ) {
        utmpshm = attach_shm( "UTMP_SHMKEY", 3699, sizeof( *utmpshm ) );/*attach user tmp cache */
    }
}

int
getnewutmpent( up )
struct user_info *up;
{
    static int          utmpfd=0;
    struct user_info    *uentp;
    time_t      now;
    int         i, n;
    static int tmpfd;

    if( utmpfd == 0 ) {
        utmpfd = open( ULIST, O_RDWR|O_CREAT, 0600 );
        if( utmpfd < 0 )
            return -1;
    }
    /*    if (tmpfd ==0){  by dong 1998.7.6 */
    /*       tmpfd = open( ".tmpfile",  O_RDWR|O_CREAT, 0600 );
           if (tmpfd < 0)
              return -1;
        }
    */
    resolve_utmp();

    while (flock( utmpfd, LOCK_EX ) ==-1)  sleep(2); /* lock it, 1999.9.17, change to while, by dong */
    /*    while (flock( tmpfd, LOCK_EX ) == -1); * lock it*/

    for( i = 0; i < USHM_SIZE; i++ ) { /* �ҿյ�user tmp shm */
        uentp = &(utmpshm->uinfo[ i ]);
        if( !uentp->active || !uentp->pid ) break ;
    }
    if( i >= USHM_SIZE ) {
        flock( utmpfd, LOCK_UN );
        close(utmpfd);
        return -1;
    }
    utmpshm->uinfo[i] = *up;

    now = time( NULL );
    if(( now > utmpshm->uptime + 120 )||(now < utmpshm->uptime-120)) {
        utmpshm->uptime = now;
        log_usies( "CACHE", "Clean user utmp cache");
        for( n = 0; n < USHM_SIZE; n++ ) {
            utmpshm->uptime = now;
            uentp = &(utmpshm->uinfo[ n ]);
            if( uentp->active && uentp->pid && kill( uentp->pid, 0 ) == -1 ) /*uentp���*/
            {
                char buf[STRLEN];
                strncpy(buf, uentp->userid, IDLEN+2);
                memset( uentp, 0, sizeof( struct user_info ) );/*uentp�д���������*/
                RemoveMsgCountFile2(buf);
            }
        }
        n = USHM_SIZE - 1;
        while( n > 0 && utmpshm->uinfo[ n ].active == 0 )
            n--;
        ftruncate( utmpfd, 0 );
        write( utmpfd, utmpshm->uinfo, (n+1) * sizeof(struct user_info) );
    }
    /*   flock( tmpfd, LOCK_UN );* unlock */
    flock( utmpfd, LOCK_UN );/* unlock */
    close(utmpfd); /* add by dong , 1999.9.12 */
    utmpfd = 0;
    return i+1 ;
}

int
apply_ulist( fptr ) /* apply func on user list */
int (*fptr)();
{
    struct user_info    *uentp, utmp;
    int         i, max;

    resolve_utmp();
    max = USHM_SIZE - 1;
    while( max > 0 && utmpshm->uinfo[ max ].active == 0 ) /*������� ��active��user*/
        max--;
    for( i = 0; i <= max; i++ ) {
        uentp = &(utmpshm->uinfo[ i ]);
        utmp = *uentp;
        if( (*fptr)( &utmp ) == QUIT )
            return QUIT;
    }
    return 0;
}

int
apply_ulist_address( fptr ) /* apply func on user list address*/
int (*fptr)();
{
    struct user_info    *uentp, utmp;
    int         i, max;

    resolve_utmp();
    max = USHM_SIZE - 1;
    while( max > 0 && utmpshm->uinfo[ max ].active == 0 )
        max--;
    for( i = 0; i <= max; i++ ) {
        /*        uentp = &(utmpshm->uinfo[ i ]);
                utmp = *uentp;*/
        if( (*fptr)( &(utmpshm->uinfo[ i ]) ) == QUIT )
            return QUIT;
    }
    return 0;
}

int
search_ulist( uentp, fptr, farg ) /* ulist �� search ����fptr������ user */
struct user_info *uentp;
int (*fptr)();
int farg;
{
    int         i;

    resolve_utmp();
    for( i = 0; i < USHM_SIZE; i++ ) {
        *uentp = utmpshm->uinfo[ i ];
        if( (*fptr)( farg, uentp ) )
            return i+1;
    }
    return 0;
}

int
search_ulistn( uentp, fptr, farg, unum ) /* ����fptr������user ����ͳ�� */
struct user_info *uentp;
int (*fptr)();
int farg;
int unum;
{
    int         i, j;
    j = 1;
    resolve_utmp();
    for( i = 0; i < USHM_SIZE; i++ ) {
        *uentp = utmpshm->uinfo[ i ];
        if( (*fptr)( farg, uentp ) ) {
            if (j == unum) return i+1;
            else j++; }

    }
    return 0;
}

/*Function Add by SmallPig*/
int
count_logins( uentp, fptr, farg, show ) /* �û�����login����������ʾ״̬*/
struct user_info *uentp;
int (*fptr)();
int farg;
int show;
{
    int         i, j;

    j = 0;
    resolve_utmp();
    for( i = 0; i < USHM_SIZE; i++ ) {
        *uentp = utmpshm->uinfo[ i ];
        if( (*fptr)( farg, uentp ) )
            if( (*fptr)( farg, uentp ) )
                if(show == 0) j++;
                else if(show == 1) {
                    j++;
                    prints("(%d) Ŀǰ״̬: %s, ����: %s \n", j,
                           modestring(uentp->mode, uentp->destuid, 0, /* 1->0 ����ʾ�������� modified by dong 1996.10.26 */
                                      uentp->in_chat ? uentp->chatid : NULL), uentp->from );

                }
    }
    return j;
}



/* Written By Excellent *//*Modify by SmallPig*/
int
t_search_ulist( uentp, fptr, farg ) /* ��ʾ����fptr���û� ״̬*/
struct user_info *uentp;
int (*fptr)();
int farg;
{
    int         i,num;

    resolve_utmp();
    num=0;
    for( i = 0; i < USHM_SIZE; i++ ) {
        *uentp = utmpshm->uinfo[ i ];
        if( (*fptr)( farg, uentp ) )
        {
            if( !uentp->active || !uentp->pid )
                continue;
            if(uentp->invisible==1)
            {
                if(HAS_PERM(PERM_SEECLOAK))
                {
                    prints("[32m������   [m");
                    continue;
                }
                else
                    continue;
            }
            num++;
            if(num==1)
                prints("Ŀǰ��վ�ϣ�״̬���£�\n");
            prints("[1m%s[m ", modestring(uentp->mode,
                                            uentp->destuid, 0,/* 1->0 ����ʾ�������� modified by dong 1996.10.26 */
                                            (uentp->in_chat ? uentp->chatid : NULL)));
            if((num)%8==0)
                prints("\n");
        }
    }
    prints("\n");
    /*return 0;*/
    return num; /* Leeward 98.09.20 */
}

void
update_ulist( uentp, uent )
struct user_info *uentp;
int     uent;
{
    int utmpfd;
    int ret;

    resolve_utmp();
    utmpfd = open( ULIST, O_RDWR|O_CREAT, 0600 );
    if( utmpfd < 0 )
        return;

    if (flock( utmpfd, LOCK_EX ) ==-1)  return; /* lock it, 1999.9.17, change to while, by dong */
    if( uent > 0 && uent <= USHM_SIZE ) {
        utmpshm->uinfo[ uent - 1 ] = *uentp;
    }
    flock( utmpfd, LOCK_UN );/* unlock */
    close(utmpfd);
}

void
update_utmp()
{
    extern time_t old;
    memcpy(&old,uinfo.tty+1,sizeof(time_t));
    update_ulist( &uinfo, utmpent );
}
