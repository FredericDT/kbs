/* define all function that need to by used in whole project */
#ifndef __FUNC_H__
#define __FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* defined in pass.c */
    int checkpasswd(const char *passwd, const char *test);
    int checkpasswd2(const char *passwd, const struct userec *user);
    int setpasswd(const char *passwd, struct userec *user);


    char *Cdate(time_t clock);  /* ʱ�� --> Ӣ�� */
    char *Ctime(time_t clock);  /* ʱ�� ת�� �� Ӣ�� */
    char *nextword(const char **str, char *buf, int sz);


/* defined in ucache.c */
    struct userec *getuserbynum(int num);
    int getnewuserid2(char *userid);
    unsigned int ucache_hash(const char *userid);       /* hash function export for utmp usage */

    void resolve_ucache();

    int getuserid(char *userid, int uid);
    void setuserid(int num, const char *userid);        /* ����user num��idΪuser id */
    int setuserid2(int num, const char *userid);        /* ����user num��idΪuser id,userdʹ�� */
    int searchuser(const char *userid);
    int getuser(const char *userid, struct userec **lookupuser);        /* ȡ�û���Ϣ ������ֵ��lookupuser���� */
    char *u_namearray(char buf[][IDLEN + 1], int *pnum, char *tag);
    char *getuserid2(int uid);
    int update_user(struct userec *user, int num, int all);
    int apply_users(int (*fptr) (struct userec *, char *), char *arg);
    int getnewuserid(char *userid);

    int flush_ucache();
    int load_ucache();
    int get_giveupinfo(char* userid,int* basicperm,int s[10][2]);
	void save_giveupinfo(struct userec* lookupuser,int lcount,int s[10][2]);
/* ����tag ,���� ƥ���user id �б� (�������ע���û�)*/


/* 
   in bbslog.c 
   
   ���from[0]��һ�����֣���ô�������log�����ȼ���ȱʡ���ȼ��� 0

*/
    int bbslog(const char *from, const char *fmt, ...);
    int bmlog(char* id, char* boardname, int type, int value);

/* defined in utmp.c */
    typedef int (*APPLY_UTMP_FUNC) (struct user_info *, void *, int pos);
    void resolve_utmp();
    void detach_utmp();
    int getnewutmpent(struct user_info *up);
    int real_getnewutmpent(struct user_info *up);
#define CHECK_UENT(uident) ((utmpent > 0 && utmpent <= USHM_SIZE ) && \
			(utmpshm->uinfo[ utmpent - 1 ].uid==(uident)))
#define UPDATE_UTMP(field,entp) { if (CHECK_UENT((entp).uid)) \
			utmpshm->uinfo[ utmpent- 1 ].field=(entp).field; }
#define UPDATE_UTMP_STR(field,entp) { if (CHECK_UENT((entp).uid)) \
			strcpy(utmpshm->uinfo[ utmpent- 1 ].field,(entp).field); }
    int search_ulist(struct user_info *uentp, int (*fptr) (int, struct user_info *), int farg); /* ulist �� search ����fptr������ user */
#if 0
    void clear_utmp2(struct user_info *uentp);
#endif
    void clear_utmp2(int uentp);
    void clear_utmp(int uentp, int useridx, int pid);   /*����סutmp����clear_utmp2,��У��useridx */
    int apply_ulist(APPLY_UTMP_FUNC fptr, void *arg);
    int apply_utmpuid(APPLY_UTMP_FUNC fptr, int uid, void *arg);
    int apply_ulist_addr(APPLY_UTMP_FUNC fptr, void *arg);      /* apply func on user list */
    int get_utmp_number();      /* return active user */
    struct user_info *get_utmpent(int utmpnum);
    int get_utmpent_num(struct user_info *uent);        /* return utmp number */

    int t_cmpuids(int uid, struct user_info *up);
    int apply_utmp(APPLY_UTMP_FUNC fptr, int maxcount, char *userid, void *arg);

/* defined in list.c */
    int myfriend(int uid, char *fexp);

/* defined in newio.c */
    void output(char *s, int len);

/* defined in stuff.c */
    int multilogin_user(struct userec *user, int usernum);
    time_t get_exit_time(char *id, char *exittime);
    int dashf(char *fname);
    int dashd(char *fname);
    int seek_in_file(char filename[STRLEN], char seekstr[STRLEN]);
    char *setbdir(int digestmode, char *buf, char *boardname);
    int my_system(const char *cmdstring);
    char *modestring(int mode, int towho, int complete, char *chatid);
    int countexp(struct userec *udata);
    int countperf(struct userec *udata);
    int compute_user_value(struct userec *urec);
    void *attach_shm(char *shmstr, int defaultkey, int shmsize, int *iscreate);
    void *attach_shm1(char *shmstr, int defaultkey, int shmsize, int *iscreate, int readonly, void *shmaddr);
    void attach_err(int shmkey, char *name);
    int id_invalid(char *userid);
    int addtofile(char filename[STRLEN], char str[STRLEN]);
    void bbssettime(time_t t);
    time_t bbstime(time_t * t);
    int getwwwguestcount();
    int del_from_file(char filename[STRLEN], char str[STRLEN]);
    int setpublicshmreadonly(int readonly);
    struct public_data *get_publicshm();        /*���public shmָ�� */
    char *sethomefile(char *buf, char *userid, char *filename); /*ȡĳ�û��ļ� ·�� */
    char *sethomepath(char *buf, char *userid); /* ȡ ĳ�û� ��home */

    char *setmailfile(char *buf, char *userid, char *filename); /* ȡĳ�û�mail�ļ� ·�� */
    char *setmailpath(char *buf, char *userid); /* ȡ ĳ�û� ��mail */
    char *setbpath(char *buf, char *boardname); /* ȡĳ�� ·�� */
    char *setbfile(char *buf, char *boardname, char *filename); /* ȡĳ�����ļ� */
    void RemoveMsgCountFile(char *userID);
    int bad_user_id(char *userid);      /* ���.badname�Ƿ�����ע��� */
    int valid_ident(char *ident);       /* ���Ϸ���ident */
    struct user_info *t_search();
    int getunifopid();
    int cmpinames(const char *userid, const char *uv);
    int cmpfnames(char *userid, struct friends *uv);
    int cmpfileinfoname(char *filename, struct fileheader *fi);

    int canIsend2(char *userid);
    void sigbus(int signo);
    void encodestr(register char *str);
    int Isspace(char ch);
    char *idle_str(struct user_info *uent);
    void getuinfo(FILE * fn, struct userec *ptr_urec);
    int simplepasswd(char *str);
    void logattempt(char *uid, char *frm);
    int check_ban_IP(char *IP, char *buf);

#define time(x) bbstime(x)

#define BBS_TRY \
    	if (!sigsetjmp(bus_jump, 1)) { \
        	signal(SIGBUS, sigbus);

#define BBS_CATCH \
	} \
	else { \

#define BBS_END } \
	signal(SIGBUS, SIG_IGN);

#define BBS_RETURN(x) {signal(SIGBUS, SIG_IGN);return (x);}
#define BBS_RETURN_VOID {signal(SIGBUS, SIG_IGN);return;}

    int safe_mmapfile(char *filename, int openflag, int prot, int flag, void **ret_ptr, size_t * size, int *ret_fd);
    int safe_mmapfile_handle(int fd, int openflag, int prot, int flag, void **ret_ptr, size_t * size);
    void end_mmapfile(void *ptr, int size, int fd);

/* define in bcache.c */
    int getbnum(char *bname);
    void resolve_boards();
    int get_boardcount();
    struct boardheader *getbcache(char *bname);
    int normal_board(char *bname);
    int getboardnum(char *bname, struct boardheader *bh);       /* board name --> board No. & not check level */

    int add_board(struct boardheader *newboard);
    int apply_boards(int (*func) ());   /* �����а� Ӧ�� func���� */
    int delete_board(char *boardname, char *title);     /* delete board entry */
    struct boardheader const *getboard(int num);
    int set_board(int bid, struct boardheader *board);
    struct BoardStatus *getbstatus(int index);  /* ��ð���ķ���״̬�ṹ */
    int updatelastpost(char *board);
    int setboardmark(char *board, int i);
    int setboardorigin(char *board, int i);
    int setboardtitle(char *board, int i);
    int board_setreadonly(char *board, int readonly);   /* ���ð���ֻ������ */
    int get_nextid(char *boardname);    /*�������������Ų��Զ���һ */

/* define in boards.c */
    int anonymousboard(char *board);
    int load_boards(char *boardprefix);
    void brc_clear_new_flag(unsigned fid);    /* �������ĵ���ƪ����δ����� */

    int getfavnum();
    void save_zapbuf();
    void addFavBoard(int);
    void addFavBoardDir(int, char *);
    void release_favboard();
    int changeFavBoardDir(int i, char *s);
    int ExistFavBoard(int idx);
    void load_favboard(int dohelp);
    void save_favboard();
    void save_userfile(char *fname, int blknum, char *buf);
    int IsFavBoard(int idx);
    void MoveFavBoard(int p, int q);
    int DelFavBoard(int i);
    int SetFav(int i);
    int cmpboard(struct newpostdata *brd, struct newpostdata *tmp);

    int brc_initial(char *userid, char *boardname);
    char *brc_putrecord(char *ptr, char *name, int num, int *list);
    /*
     * ����һ�����brclist 
     */
    void brc_update(char *userid);      /* ���浱ǰ��brclist���û���.boardrc */
    void brc_add_read(unsigned int fid);
    void brc_addreaddirectly(char *userid, int bnum, unsigned int fid);
    void brc_clear();
    int brc_unread(unsigned int fid);
    int junkboard(char *currboard);     /* �ж��Ƿ�Ϊ junkboards */
    int checkreadonly(char *board);     /* �ж��ǲ���ֻ������ */
    int deny_me(char *user, char *board);       /* �ж��û� �Ƿ񱻽�ֹ�ڵ�ǰ�淢���� */
    int haspostperm(struct userec *user, char *bname);  /* �ж��� bname�� �Ƿ���postȨ */
    int chk_BM_instr(const char BMstr[STRLEN - 1], const char bmname[IDLEN + 2]);       /*��ѯ�ַ������Ƿ���� bmname */
    int chk_currBM(const char BMstr[STRLEN - 1], struct userec *user);  /* ��������İ������� �ж�user�Ƿ��а��� Ȩ�� */
    int deldeny(struct userec *user, char *board, char *uident, int notice_only);       /* ɾ�� ��ֹPOST�û� */
    int check_read_perm(struct userec *user, int num);

/* define in article.c */
    /* mmap_search_dir_apply
     ����:mmap struct fileheader�ṹ���ļ����ҵ���Ҫ�ļ�¼,
     �ҵ������func,����start�����ļ�mmap�Ŀ�ʼ��entΪ��¼��
     ��������ID�����ֲ���
     ���õ�ʱ��
     match=true,��ȫƥ��
     match=false,�ҵ��ò����ǰһ����¼��
     */
    typedef int (*DIR_APPLY_FUNC)(int fd,struct fileheader* start,int ent,int total,struct fileheader* data,bool match);

    int get_postfilename(char *filename, char *direct);
    int mail_file(char *fromid, char *tmpfile, char *userid, char *title, int unlink);
    int isowner(struct userec *user, struct fileheader *fileinfo);
    int do_del_post(struct userec *user, int ent, struct fileheader *fileinfo, char *direct, char *board, int digestmode, int decpost);
    /*
     * ɾ�����£�digestmode�����Ķ�ģʽ��decpost��ʾ����ɾ���Ƿ�������� 
     */

    int cmpname(struct fileheader *fhdr, char name[STRLEN]);
/* Haohmaru.99.3.30.�Ƚ� ĳ�ļ����Ƿ�� ��ǰ�ļ� ��ͬ */

    void addsignature(FILE * fp, struct userec *user, int sig); /*����User��ǩ���� */

/*���  POST ���һ�� ����Դ*/
    void add_loginfo(char *filepath, struct userec *user, char *currboard, int Anony);

/* ��quote_file���Ƶ�filepath (ת�����Զ�����)*/
    void getcross(char *filepath, char *quote_file, struct userec *user, int in_mail, char *board, char *title, int Anony, int mode, char *sourceboard);

    void write_header(FILE * fp, struct userec *user, int in_mail, char *board, char *title, int Anony, int mode);

/*д��.post�ļ�������*/
    int write_posts(char *id, char *board, char *title);
    void cancelpost(char *board, char *userid, struct fileheader *fh, int owned, int autoappend);
    int outgo_post(struct fileheader *fh, char *board, char *title);
    int after_post(struct userec *user, struct fileheader *fh, char *boardname, struct fileheader *re);
    int post_file(struct userec *user, char *fromboard, char *filename, char *nboard, char *posttitle, int Anony, int mode);
    int post_cross(struct userec *user, char *toboard, char *fromboard, char *title, char *filename, int Anony, int in_mail, char islocal, int mode);   /* (�Զ������ļ���) ת�����Զ����� */

/* bad 2002.8.16 */
    int dele_digest(char *dname, char *direc);
    int change_post_flag(char* currBM, struct userec* currentuser, int digestmode, char* currboard, int ent, struct fileheader *fileinfo, char *direct, int flag, int prompt);

/**
 * A function return flag character of an article.
 * 
 * @param ent pointer to fileheader structure of the article
 * @param user pointer to userec structure of the user
 * @param is_bm nonzero for board manager, zero for others
 * @return flag character of the article
 * @author flyriver
 */
    char get_article_flag(struct fileheader *ent, struct userec *user, int is_bm);

/* define in record.c */
    int safewrite(int fd, char *buf, int size);
    typedef int (*RECORD_FUNC_ARG) (void *, void *);
    typedef int (*APPLY_FUNC_ARG) (void *, int, void *);
    int delete_record(char *dirname, int size, int ent, RECORD_FUNC_ARG filecheck, void *arg);
    int apply_record(char *filename, APPLY_FUNC_ARG fptr, int size, void *arg, int applycopy, bool reverse);
    int append_record(char *filename, void *record, int size);
    int substitute_record(char *filename, void *rptr, int size, int id);
    int search_record(char *filename, void *rptr, int size, RECORD_FUNC_ARG fptr, void *farg);
    long get_num_records(char *filename, int size);
    long get_sum_records(char *fpath, int size);        /*���.DIR�����ļ���С���� */
    int get_record_handle(int fd, void *rptr, int size, int id);
    int get_record(char *filename, void *rptr, int size, int id);
    int delete_range(char *filename, int id1, int id2, int del_mode);
    int get_records(char *filename, void *rptr, int size, int id, int number);
    int search_record_back(int fd,      /* idx file handle */
                           int size,    /* record size */
                           int start,   /* where to start reverse search */
                           RECORD_FUNC_ARG fptr,        /* compare function */
                           void *farg,  /* additional param to call fptr() / original record */
                           void *rptr,  /* record data buffer to be used for reading idx file */
                           int sorted); /* if records in file are sorted */
    int del_range(int ent, struct fileheader *fileinfo, char *direct, int mailmode);


/* define in sysconf.c */
    char *sysconf_str(char *key);
    int sysconf_eval(char *key,int defaultval);
    struct smenuitem *sysconf_getmenu(char *menu_name);
    void build_sysconf(char *configfile, char *imgfile);
    void load_sysconf();
    char *sysconf_relocate(char *data); /*���¶�λmenuitem������ַ��� */
    int check_sysconf();

/* libmsg.c */
    int addto_msglist(int utmpnum, char *userid);
    int sendmsgfunc(struct user_info *uentp, const char *msgstr, int mode);
    int canmsg(struct userec *fromuser, struct user_info *uin);
    int can_override(char *userid, char *whoasks);
    int delfrom_msglist(int utmpnum, char *userid);

/* site.c */
    char *ModeType(int mode);
    char *email_domain();
    int get_shmkey(char *s);
    int uleveltochar(char *buf, struct userec *lookupuser);

    int safe_kill(int x, int y);

    void main_bbs(int convit, char *argv);

/* bbs_sendmail.c */
    int chkusermail(struct userec *user);
    int chkreceiver(struct userec *fromuser, struct userec *touser);
    int bbs_sendmail(char *fname, char *title, char *receiver, int isuu, int isbig5, int noansi);
    int check_query_mail(char qry_mail_dir[STRLEN]);
/* convcode.c */
    void conv_init();

/* libann.c */
    void ann_add_item(MENU * pm, ITEM * it);
    int ann_load_directory(MENU * pm);
    ITEM *ann_alloc_items(size_t num);
    void ann_free_items(ITEM * it, size_t num);
    void ann_set_items(MENU * pm, ITEM * it, size_t num);
    int ann_get_board(char *path, char *board, size_t len);
    int ann_get_path(char *board, char *path, size_t len);

/* check the user's access for the path
  return < 0 deny access
  return ==0 has access and it can be access by any body
  return >0 need some extra permission to access it
*/
    int ann_traverse_check(char *path, struct userec *user);


/* zmodem */
    int zsend_file(char *filename, char *title);
#define FILENAME2POSTTIME(x) (atoi(((char*)x)+2))

#ifdef __cplusplus
}
#endif
#endif
