/* user cache ����
   ��hask table ��������ID��id��,�ӿ�����ٶ�
   2001.5.4 KCN
*/


#include "bbs.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include <signal.h>

#include "uhashgen.h"

struct UCACHE {
    ucache_hashtable hashtable;
    ucache_hashtable hashusage;
    int hashhead[UCACHE_HASHSIZE + 1];
    int next[MAXUSERS];
    time_t uptime;
    int number;
};

static struct userec *passwd = NULL;
static struct UCACHE *uidshm = NULL;

static int ucache_lock()
{
    int lockfd;

    lockfd = open(ULIST, O_RDWR | O_CREAT, 0600);
    if (lockfd < 0) {
        bbslog("3system", "CACHE:lock ucache:%s", strerror(errno));
        return -1;
    }
    flock(lockfd, LOCK_EX);
    return lockfd;
}

static void ucache_unlock(int fd)
{
    flock(fd, LOCK_UN);
    close(fd);
}

/* do init hashtable , read from uhashgen.dat -- wwj*/
static void ucache_hashinit()
{
    FILE *fp;
    char line[256];
    int i, j, ptr, data;


    fp = fopen("uhashgen.dat", "rt");
    if (!fp) {
        bbslog("3system", "UCACHE:load uhashgen.dat fail");
        exit(0);
    }
    i = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#')
            continue;
        j = 0;
        ptr = 0;
        while ((line[ptr] >= '0' && line[ptr] <= '9') || line[ptr] == '-') {
            data = ptr;
            while ((line[ptr] >= '0' && line[ptr] <= '9') || line[ptr] == '-')
                ptr++;
            line[ptr++] = 0;
            if (i == 0) {
                if (j >= 26) {
                    bbslog("3system", "UCACHE:hash0>26");
                    exit(0);
                }
                uidshm->hashtable.hash0[j++] = atoi(&line[data]);
            } else {
                if (j >= 36) {
                    bbslog("3system", "UCACHE:hash0>26");
                    exit(0);
                }
                uidshm->hashtable.hash[i - 1][j++] = atoi(&line[data]);
            }
        }
        i++;
        if (i > (int) sizeof(uidshm->hashtable.hash) / 36) {
            bbslog("3system", "hashline %d exceed", i);
            exit(0);
        }
    }
    fclose(fp);

    bbslog("1system", "HASH load");
}

/*
   CaseInsensitive ucache_hash, assume
   isalpha(userid[0])
   isahpha(userid[n]) || isnumber(userid[n]) n>0
   01/5/5 wwj
 */
unsigned int ucache_hash_deep(const char *userid)
{
    int n1, n2, n;
    ucache_hashtable *hash;
    ucache_hashtable *usage;

    if (!*userid)
        return 0;
    hash = &uidshm->hashtable;
    usage = &uidshm->hashusage;

    n1 = *userid++;
    if (n1 >= 'a' && n1 <= 'z')
        n1 &= 0xdf;
    n1 -= 'A';
    if (n1 < 0 || n1 >= 26)
        return 0;

    n1 = hash->hash0[n1];

    n = 1;
    while (n1 < 0) {
        n1 = -n1 - 1;
        if (!*userid) {
/* disable it            usage->hash[n1][0]++; */
            n1 = hash->hash[n1][0];
        } else {
            n2 = *userid++;
            if (n2 >= 'a' && n2 <= 'z') {
                n2 -= 'a' - 10;
            } else if (n2 >= 'A' && n2 <= 'Z') {
                n2 -= 'A' - 10;
            } else {
                n2 -= '0';
            }
            if (n2 < 0 || n2 >= 36)
                return 0;
            n1 = hash->hash[n1][n2];
        }
        n++;
    }
    return n;
}


/*
   CaseInsensitive ucache_hash, assume 
   isalpha(userid[0]) 
   isahpha(userid[n]) || isnumber(userid[n]) n>0
   01/5/5 wwj
 */
unsigned int ucache_hash(const char *userid)
{
    int n1, n2, n, len;
    ucache_hashtable *hash;
    ucache_hashtable *usage;

    if (!*userid)
        return 0;
    hash = &uidshm->hashtable;
    usage = &uidshm->hashusage;

    n1 = *userid++;
    if (n1 >= 'a' && n1 <= 'z')
        n1 &= 0xdf;
    n1 -= 'A';
    if (n1 < 0 || n1 >= 26)
        return 0;

/* disable it    usage->hash0[n1]++;*/
    n1 = hash->hash0[n1];

    while (n1 < 0) {
        n1 = -n1 - 1;
        if (!*userid) {
/* disable it            usage->hash[n1][0]++; */
            n1 = hash->hash[n1][0];
        } else {
            n2 = *userid++;
            if (n2 >= 'a' && n2 <= 'z') {
                n2 -= 'a' - 10;
            } else if (n2 >= 'A' && n2 <= 'Z') {
                n2 -= 'A' - 10;
            } else {
                n2 -= '0';
            }
            if (n2 < 0 || n2 >= 36)
                return 0;
/* disable it            usage->hash[n1][n2]++; */
            n1 = hash->hash[n1][n2];
        }
    }
    n1 = (n1 * UCACHE_HASHBSIZE) % UCACHE_HASHSIZE + 1;
    if (!*userid)
        return n1;

    n2 = 0;
    len = strlen(userid);
    while (*userid) {
        n = *userid++;
        if (n >= 'a' && n <= 'z') {
            n -= 32;
        }
        n2 += (n - 47) * len;
        len--;
    }
    n1 = (n1 + n2 % UCACHE_HASHBSIZE) % UCACHE_HASHSIZE + 1;
    return n1;
}

static int fillucache(struct userec *uentp, int *number, int *prev)
{
    if (*number < MAXUSERS) {
        int hashkey;

        if (id_invalid(uentp->userid))
            hashkey = 0;
        else
            hashkey = ucache_hash(uentp->userid);
        if (hashkey < 0 || hashkey > UCACHE_HASHSIZE) {
            bbslog("3system", "UCACHE:hash(%s) %d error", uentp->userid, hashkey);
            exit(0);
        }
      addempty:
        if (hashkey == 0) {     /* empty user add in recurise sort 
                                   int i=uidshm->hashhead[0];
                                   uidshm->next[*number] = 0;
                                   if (i==0) uidshm->hashhead[0]=++(*number);
                                   else {
                                   int prev;
                                   while (i) {
                                   prev=i;
                                   i=uidshm->next[i-1];
                                   };
                                   uidshm->next[prev-1]=++(*number);
                                   } */
            uidshm->next[*number] = 0;
            (*number)++;
            if (!(*prev)) {
                uidshm->hashhead[0] = *number;
            } else {
                uidshm->next[(*prev) - 1] = *number;
            }
            *prev = *number;
        } else {
/* check multi-entry of user */
            int i, prev;

            i = uidshm->hashhead[hashkey];
            prev = -1;
            while (i != 0) {
                struct userec *uentp;

                uentp = &passwd[i - 1];
                if (!strcasecmp(passwd[*number].userid, uentp->userid)) {
                    if (passwd[*number].numlogins > uentp->numlogins) {
                        bbslog("3passwd", "deleted %s in %d", uentp->userid, i - 1);
                        if (prev == -1)
                            uidshm->hashhead[hashkey] = uidshm->next[i - 1];
                        else
                            uidshm->next[prev - 1] = uidshm->next[i - 1];
                        uentp->userid[0] = 0;
                        uidshm->next[i - 1] = 0;
                        uidshm->hashhead[0] = i;
                    } else {
                        bbslog("3passwd", "deleted %s in %d", passwd[*number].userid, *number);
                        passwd[*number].userid[0] = 0;
                        hashkey = 0;
                        goto addempty;
                    }
                }
                prev = i;
                i = uidshm->next[i - 1];
            }
            uidshm->next[*number] = uidshm->hashhead[hashkey];
            uidshm->hashhead[hashkey] = ++(*number);
        }
    }
    return 0;
}

int flush_ucache()
{
    return substitute_record(PASSFILE, passwd, MAXUSERS * sizeof(struct userec), 1);
}

int load_ucache()
{
    int iscreate;
    int usernumber, i;
    int passwdfd;
    int prev;

    uidshm = (struct UCACHE *) attach_shm("UCACHE_SHMKEY", 3696, sizeof(*uidshm), &iscreate);   /*attach to user shm */

    if (!iscreate) {
        bbslog("4system", "load a exitist ucache shm!");
/*        return -1; */
    }

    passwd = (struct userec *) attach_shm("PASSWDCACHE_SHMKEY", 3697, MAXUSERS * sizeof(struct userec), &iscreate);     /*attach to user shm */
    if (!iscreate) {            /* shouldn't load passwd file in this place */
        bbslog("4system", "load a exitist ucache shm!");
        return 0;
    }

    if ((passwdfd = open(PASSFILE, O_RDWR | O_CREAT, 0644)) == -1) {
        bbslog("3system", "Can't open " PASSFILE "file %s", strerror(errno));
        exit(-1);
    }
    ftruncate(passwdfd, MAXUSERS * sizeof(struct userec));
    if (get_records(PASSFILE, passwd, sizeof(struct userec), 1, MAXUSERS) != MAXUSERS) {
        bbslog("4system", "PASS file!");
        return -1;
    }

    bzero(uidshm->hashhead, UCACHE_HASHSIZE * sizeof(int));
    usernumber = 0;

    ucache_hashinit();

    prev = 0;
    for (i = 0; i < MAXUSERS; i++)
        fillucache(&passwd[i], &usernumber, &prev);

    bbslog("1system", "CACHE:reload ucache for %d users", usernumber);
    uidshm->number = usernumber;

    return 0;
}

void resolve_ucache()
{
    int iscreate;

    iscreate = 0;
    if (uidshm == NULL) {
        uidshm = (struct UCACHE *) attach_shm1("UCACHE_SHMKEY", 3696, sizeof(*uidshm), &iscreate, 1, NULL);
        /*attach to user shm,readonly */
        if (iscreate) {         /* shouldn't load passwd file in this place */
            bbslog("4system", "passwd daemon havn't startup");
            exit(-1);
        }
    }

    if (passwd == NULL) {
        passwd = (struct userec *) attach_shm1("PASSWDCACHE_SHMKEY", 3697, MAXUSERS * sizeof(struct userec), &iscreate, 0, NULL);       /*attach to user shm */
        if (iscreate) {         /* shouldn't load passwd file in this place */
            bbslog("4system", "passwd daemon havn't startup");
            exit(-1);
        }
    }
}

/*---	period	2000-10-20	---*/
int getuserid(char *userid, int uid)
{
    if (uid > uidshm->number || uid <= 0)
        return 0;
    strncpy(userid, (char *) passwd[uid - 1].userid, IDLEN + 1);
    userid[IDLEN] = 0;
    return uid;
}

static int setuserid_internal(int num, const char *userid)
{                               /* ����user num��idΪuser id */
    if (num > 0 && num <= MAXUSERS) {
        int oldkey, newkey, find;

        if (num > uidshm->number)
            uidshm->number = num;
        oldkey = ucache_hash((char *) passwd[num - 1].userid);
        newkey = ucache_hash(userid);
/*        if (oldkey!=newkey) { disable,Ϊ�˼�ǿ������*/
        find = uidshm->hashhead[oldkey];

        if (find == num)
            uidshm->hashhead[oldkey] = uidshm->next[find - 1];
        else {                  /* find and remove the hash node */
            int i = 0;

            while (uidshm->next[find - 1] && uidshm->next[find - 1] != num) {
                find = uidshm->next[find - 1];
                i++;
                if (i > MAXUSERS) {
                    bbslog("3system", "UCACHE:uhash loop! find %d,%s", num, userid);
                    exit(0);
                }
            }
            if (!uidshm->next[find - 1]) {
                if (oldkey != 0) {
                    bbslog("3system", "UCACHE:can't find %s in hash table", passwd[num - 1].userid);
/*		          	exit(0);*/
                }
                return -1;
            } else
                uidshm->next[find - 1] = uidshm->next[num - 1];
        }

        uidshm->next[num - 1] = uidshm->hashhead[newkey];
        uidshm->hashhead[newkey] = num;
/*        }	        */
        strncpy(passwd[num - 1].userid, userid, IDLEN + 1);
    }
    return 0;
}

int setuserid2(int num, const char *userid)
{
    int lockfd, ret;

    lockfd = ucache_lock();
    ret = setuserid_internal(num, userid);
    ucache_unlock(lockfd);
    return ret;
}

void setuserid(int num, const char *userid)
{

    int m_socket;
    char cmdbuf[255];
    struct sockaddr_in sin;
    fd_set rfds;
    int result;
    struct timeval tv;

    m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket < 0)
        return;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(60001);
    inet_aton("127.0.0.1", &sin.sin_addr);
    if (connect(m_socket, (struct sockaddr *) &sin, sizeof(sin)) != 0) {
        close(m_socket);
        return;
    }
    if (userid[0] == 0)
        sprintf(cmdbuf, "DEL %s %d", userid, num);
    else
        sprintf(cmdbuf, "SET %s %d", userid, num);
    write(m_socket, cmdbuf, strlen(cmdbuf));
    FD_ZERO(&rfds);
    FD_SET(m_socket, &rfds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    result = select(m_socket + 1, &rfds, NULL, NULL, &tv);
    if (result) {
        int len = read(m_socket, &result, sizeof(result));

        close(m_socket);
        if (len != sizeof(result))
            return;
        return;
    }
    close(m_socket);
    return;
}

static int searchnewuser()
{                               /* ��cache�� ���е� user num */
    if (uidshm->hashhead[0])
        return uidshm->hashhead[0];
    if (uidshm->number < MAXUSERS)
        return uidshm->number + 1;
    return 0;
}
int searchuser(const char *userid)
{
    register int i;

    i = uidshm->hashhead[ucache_hash(userid)];
    while (i)
        if (!strcasecmp(userid, passwd[i - 1].userid))
            return i;
        else
            i = uidshm->next[i - 1];
    return 0;
}

int getuser(const char *userid, struct userec **user)
{                               /* ȡ�û���Ϣ */
    int uid = searchuser(userid);

    if (uid == 0) {
        if (user)
            *user = NULL;
        return 0;
    }
    if (user)
        *user = &passwd[uid - 1];
    return uid;
}

char *getuserid2(int uid)
{
    if (uid > uidshm->number || uid <= 0)
        return NULL;
    return passwd[uid - 1].userid;
}

char *u_namearray(char buf[][IDLEN + 1], int *pnum, char *tag)
/* ����tag ,���� ƥ���user id �б� (�������ע���û�)*/
{
    register struct UCACHE *reg_ushm = uidshm;
    register int n, num, i;
    int hash, len, ksz;
    char tagv[IDLEN + 1];

    *pnum = 0;

    len = strlen(tag);
    if (len > IDLEN)
        return NULL;
    if (!len) {
        return NULL;
    }
    ksz = ucache_hash_deep(tag);

    strcpy(tagv, tag);


    if (len >= ksz) {
        tagv[ksz] = 0;
        hash = ucache_hash(tagv) - 1;
        for (n = 0; n < UCACHE_HASHBSIZE; n++) {
            num = reg_ushm->hashhead[(hash + n % UCACHE_HASHBSIZE) % UCACHE_HASHSIZE + 1];
            while (num) {
                if (!strncasecmp(passwd[num - 1].userid, tag, len)) {
                    strcpy(buf[(*pnum)++], passwd[num - 1].userid);     /*���ƥ��, add into buf */
                }
                num = reg_ushm->next[num - 1];
            }
        }
    } else {
        for (i = len; i < ksz; i++)
            tagv[i] = '0';
        tagv[ksz] = 0;

        while (1) {
            hash = ucache_hash(tagv) - 1;

            for (n = 0; n < UCACHE_HASHBSIZE; n++) {
                num = reg_ushm->hashhead[(hash + n % UCACHE_HASHBSIZE) % UCACHE_HASHSIZE + 1];  /* see hash() */
                while (num) {
                    if (!strncasecmp(passwd[num - 1].userid, tagv, ksz)) {
                        strcpy(buf[(*pnum)++], passwd[num - 1].userid); /*���ƥ��, add into buf */
                    }
                    num = reg_ushm->next[num - 1];
                }
            }

            i = ksz - 1;
            while (i >= len) {
                if (tagv[i] == 'Z') {
                    tagv[i] = '0';
                    i--;
                } else if (tagv[i] == '9') {
                    tagv[i] = 'A';
                    break;
                } else {
                    tagv[i]++;
                    break;
                }
            }
            if (i < len)
                break;
        }

    }
    return buf[0];
}

/* disable by KCN 
int
set_safe_record()
{
    struct userec tmp;
    extern int ERROR_READ_SYSTEM_FILE;

    if(get_record(PASSFILE,&tmp,sizeof(struct userec),usernum)==-1)
    {
        char buf[STRLEN];

        sprintf(buf,"Error:Read Passfile %4d %12.12s",usernum,currentuser->userid);
        report(buf);
        ERROR_READ_SYSTEM_FILE=true;
        abort_bbs(0);
        return -1;
    }
    currentuser->numposts=tmp.numposts;
    currentuser->numlogins=tmp.numlogins;
    currentuser->stay=tmp.stay;
    currentuser->userlevel=tmp.userlevel;
}
*/

int getnewuserid3(char *userid)
{

    int m_socket;
    char cmdbuf[255];
    struct sockaddr_in sin;
    fd_set rfds;
    int result;
    struct timeval tv;

    m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket < 0)
        return -1;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(60001);
    inet_aton("127.0.0.1", &sin.sin_addr);
    if (connect(m_socket, (struct sockaddr *) &sin, sizeof(sin)) != 0) {
        close(m_socket);
        return -1;
    }
    sprintf(cmdbuf, "NEW %s", userid);
    write(m_socket, cmdbuf, strlen(cmdbuf));
    FD_ZERO(&rfds);
    FD_SET(m_socket, &rfds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    result = select(m_socket + 1, &rfds, NULL, NULL, &tv);
    if (result) {
        int len = read(m_socket, &result, sizeof(result));

        close(m_socket);
        if (len != sizeof(result))
            return -1;
        return result;
    }
    close(m_socket);
    return -1;
}

int getnewuserid2(char *userid)
{
    int result = getnewuserid3(userid);

    if (result >= 0)
        return result;
    return -1;
}

struct userec *getuserbynum(int num)
{
    if (num <= 0 || num >= MAXUSERS)
        return NULL;
    return &passwd[num - 1];
}

int getnewuserid(char *userid)
{
    struct userec utmp;
    int fd, i;
    time_t system_time;

    system_time = time(NULL);
/*
    if( (fd = open( PASSFILE, O_RDWR|O_CREAT, 0600 )) == -1 )
        return -1;
    flock( fd, LOCK_EX );
*/
    fd = ucache_lock();

    while (1) {
        int ret;

        i = searchnewuser();
        if (i <= 0 || i > MAXUSERS) {
            ucache_unlock(fd);
            return -1;
        }
        memset(&utmp, 0, sizeof(utmp));
        strcpy(utmp.userid, userid);
        utmp.lastlogin = time(NULL);
        ret = setuserid_internal(i, userid);    /* added by dong, 1998.12.2 */
        if (ret == 0)
            break;
        passwd[i - 1].userid[0] = 0;
    }
    update_user(&utmp, i, 0);
    ucache_unlock(fd);
    return i;
}

int update_user(struct userec *user, int num, int all)
{
    struct userec tmpuser;

    if (!all) {
        if (strncasecmp(user->userid, passwd[num - 1].userid, IDLEN))
            return -1;
        tmpuser = *user;
#ifdef CONV_PASS
        memcpy(tmpuser.passwd, passwd[num - 1].passwd, IDLEN + 2);
#endif
        memcpy(tmpuser.md5passwd, passwd[num - 1].md5passwd, IDLEN + 2);
    } else {
        tmpuser = *user;
        memcpy(tmpuser.userid, passwd[num - 1].userid, IDLEN + 2);
    }
    memcpy(&passwd[num - 1], &tmpuser, sizeof(struct userec));
    return 0;
}

int apply_users(int (*fptr) (struct userec *, char *), char *arg)
{
    int i;
    int count;

    count = 0;
    for (i = 0; i < uidshm->number; i++)
        if (fptr) {
            int ret;

            ret = (*fptr) (&passwd[i], arg);
            if (ret == QUIT)
                break;
            if (ret == COUNT)
                count++;
        } else
            count++;
    return count;
}

int get_giveupinfo(char* userid,int* basicperm,int s[10][2])
{
	int lcount;
	FILE* fn;
	char buf[255];
    basicperm = 0;
    sethomefile(buf, userid, "giveup");
    fn = fopen(buf, "rt");
    if (fn) {
        while (!feof(fn)) {
            if (fscanf(fn, "%d %d", &i, &j) <= 0)
                break;
            s[lcount][0] = i;
            s[lcount][1] = j;
            switch (i) {
            case 1:
                basicperm |= PERM_BASIC;
                break;
            case 2:
                basicperm |= PERM_POST;
                break;
            case 3:
                basicperm |= PERM_CHAT;
                break;
            case 4:
                basicperm |= PERM_PAGE;
                break;
            case 5:
                basicperm |= PERM_DENYMAIL;
                break;
            }
            lcount++;
        }
        fclose(fn);
    }
    return lcount;
}


