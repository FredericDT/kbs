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
/* ��.BOARDS��cache ����
                    KCN 2001.05.16 */

#include "bbs.h"
#include <sys/ipc.h>
#include <sys/shm.h>

struct boardheader *bcache;
struct BCACHE {
	int numboards;
}*brdshm;

static int bcache_lock()
{
    int lockfd;
    lockfd = creat( "bcache.lock", 0600 );
    if( lockfd < 0 ) {
        log( "3system", "CACHE:lock bcache:%s", strerror(errno) );
        return -1;
    }
    flock(lockfd,LOCK_EX);
    return lockfd;
}

static int bcache_unlock(int fd)
{
    flock(fd,LOCK_UN);
    close(fd);
}

void
reload_boards()
{
}

void resolve_boards()
{
	int boardfd;
	struct stat st;
	int iscreate;
	
    if( brdshm == NULL ) {
        brdshm = attach_shm( "BCACHE_SHMKEY", 3693, sizeof( *brdshm ) ,&iscreate); /* attach board share memory*/
    }
    
	if ((boardfd=open(BOARDS,O_RDWR|O_CREAT,0644)) == -1) {
		log("3system","Can't open " BOARDS "file %s",strerror(errno));
       	exit(-1);
	}
   	bcache = (struct boardheader*) mmap(NULL,
   			MAXBOARD*sizeof(struct boardheader),
   			PROT_READ|PROT_WRITE,MAP_SHARED,boardfd,0);
   	if (bcache==(struct boardheader*)-1) {
		log("4system","Can't map " BOARDS "file %s",strerror(errno));
		close(boardfd);
       	exit(-1);
   	}
	if (iscreate) {
		int i;
		int fd;
		fd = bcache_lock();
		ftruncate(boardfd,MAXBOARD*sizeof(struct boardheader));
		for (i=0;i<MAXBOARD;i++)
			if (bcache[i].filename[0])
				brdshm->numboards=i+1;
		bcache_unlock(fd);
	}
   	close(boardfd);
}

int apply_boards(int (*func)()) /* �����а� Ӧ�� func����*/
{
    register int i ;

    for(i=0;i<brdshm->numboards;i++)
        if( bcache[i].level & PERM_POSTMASK || HAS_PERM( bcache[i].level ) || (bcache[i].level&PERM_NOZAP))
            if (bcache[i].filename[0])
            if((*func)(&bcache[i]) == QUIT)
                return QUIT;
    return 0;
}

int
getbnum( bname ) /* board name --> board No. */
char    *bname;
{
    register int i;

    for(i=0;i<brdshm->numboards;i++)
        if( bcache[i].level & PERM_POSTMASK || HAS_PERM( bcache[i].level )|| (bcache[i].level&PERM_NOZAP))
            if(!strncasecmp( bname, bcache[i].filename, STRLEN ) )
                return i+1 ;
    return 0 ;
}
/*---	added by period		2000-11-07	to be used in postfile	---*/
int getboardnum(char*  bname ,struct boardheader* bh) /* board name --> board No. & not check level */
{
    register int i;

    for(i=0;i<brdshm->numboards;i++)
        if(!strncasecmp( bname, bcache[i].filename, STRLEN ) ) {
        	if (bh)
        		*bh=bcache[i];
            return i+1 ;
        }
    return 0 ;
} /*---	---*/

int haspostperm(char *bname) /* �ж��� bname�� �Ƿ���postȨ */
{
    register int i;

#ifdef BBSMAIN
    if(digestmode)
        return 0;
#endif
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

int normal_board(char *bname)
{
    register int i;

    if( strcmp( bname, DEFAULTBOARD ) == 0 )  return 1;
    if ((i = getbnum(bname)) == 0) return 0;
    return (bcache[i-1].level==0);
}

struct boardheader* getbcache()
{
	return bcache;
}

int get_boardcount()
{
	return brdshm->numboards;
}

struct boardheader const* getboard(int num)
{
	if (num>0&&num<=MAXBOARD) {
		return &bcache[num-1];
	}
	return 0;
}

int delete_board(char* boardname,char* title)
{
	int bid,i;
	char buf[1024];
	int fd;
    
    bid = getbnum(boardname) ;
    if (bid==0) {
#ifdef BBSMAIN
        move(2,0) ;
        prints("����ȷ��������\n") ;
        pressreturn() ;
        clear() ;
#endif
    	return -1;
    }
    bid--;
    strcpy(boardname,bcache[bid].filename);
    strcpy(title,bcache[bid].title);
    
#ifdef BBSMAIN
    move(1,0) ;
    prints( "ɾ�������� '%s'.", bcache[bid].filename );
    clrtoeol();
    getdata(2,0,"(Yes, or No) [N]: ",genbuf,4,DOECHO,NULL,YEA) ;
    if( genbuf[0] != 'Y' && genbuf[0] != 'y') { /* if not yes quit */
        move(2,0) ;
        prints("ȡ��ɾ��....\n") ;
        pressreturn() ;
        clear() ;
        return -1;
    }
    sprintf(buf,"ɾ����������%s",bcache[bid].filename);
    securityreport(buf,NULL);
#endif

    sprintf( buf, " << '%s'�� %s ɾ�� >>",
             bcache[bid].filename, currentuser->userid );
    fd = bcache_lock();
    bid = getbnum(boardname) ;
    if (bid==0) return -1; /* maybe delete by other people */
    bid--;
	if (brdshm->numboards==bid+1)
		if (bid==0) brdshm->numboards=0;
		else
			for (i=bid-1;i>=0;i--)
				if (!bcache[i].filename[0]) {
					brdshm->numboards=i+1;
					break;
				}
    memset( &bcache[bid], 0, sizeof( struct boardheader ) );
    strcpy( bcache[bid].title, buf );
    bcache[bid].level = PERM_SYSOP;
    bcache_unlock(fd);
    return 0;
}

int add_board(struct boardheader* newboard)
{
	int bid=0;
	int fd;
	fd = bcache_lock();
    if ((bid = getbnum("")) <= 0)
    	if (brdshm->numboards<MAXBOARD) bid = brdshm->numboards+1;

    if (bid>0) {
        memcpy(&bcache[bid-1], newboard, sizeof(struct boardheader));
        if (bid>brdshm->numboards)
        	brdshm->numboards= bid;
        bcache_unlock(fd);
        return 0;
    }
    bcache_unlock(fd);
    return -1;
}

int set_board(int bid,struct boardheader* board)
{
    memcpy(&bcache[bid-1], board, sizeof(struct boardheader));
}

