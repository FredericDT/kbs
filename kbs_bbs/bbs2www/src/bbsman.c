/*
 * $Id$
 */
#include "bbslib.h"

char genbuf[1024];
char currfile[STRLEN];

int main()
{
    int i, total = 0, mode;
    char board[80], *ptr;
    char buf[STRLEN];
    bcache_t *brd;

    init_all();
    if (!loginok)
        http_fatal("���ȵ�¼");
    strsncpy(board, getparm("board"), 60);
    mode = atoi(getparm("mode"));
    brd = getbcache(board);
    if (brd == 0)
        http_fatal("�����������");
    strcpy(board, brd->filename);
    if (!has_BM_perm(currentuser, board))
        http_fatal("����Ȩ���ʱ�ҳ");
    if (mode <= 0 || mode > 5)
        http_fatal("����Ĳ���");
    printf("<table>");
    for (i = 0; i < parm_num && i < 40; i++) {
        if (!strncmp(parm_name[i], "box", 3)) {
            total++;
            if (mode == 1)
                do_del(board, atoi(parm_name[i] + 3));
            if (mode == 2)
                do_set(board, atoi(parm_name[i] + 3), FILE_MARK_FLAG);
            if (mode == 3)
                do_set(board, atoi(parm_name[i] + 3), FILE_DIGEST_FLAG);
            if (mode==4)
                do_set(board, atoi(parm_name[i] + 3), FILE_NOREPLY_FLAG);
            if (mode == 5)
                do_set(board, atoi(parm_name[i] + 3), FILE_DING_FLAG);
        }
       	else if (!strncmp(parm_name[i], "boz", 3)) {
            total++;
            if (mode == 1)
                do_del_zd(board, atoi(parm_name[i] + 3));
            if (mode == 2)
                do_set_zd(board, atoi(parm_name[i] + 3), FILE_MARK_FLAG);
            if (mode == 3)
                do_set_zd(board, atoi(parm_name[i] + 3), FILE_DIGEST_FLAG);
            if (mode==4)
                do_set_zd(board, atoi(parm_name[i] + 3), FILE_NOREPLY_FLAG);
            if (mode == 5)
                do_del_zd(board, atoi(parm_name[i] + 3));
        }
    }
    printf("</table>");
    if (total <= 0)
        printf("����ѡ������<br>\n");
    printf("<br><a href=\"/bbsmdoc.php?board=%s\">���ع���ģʽ</a>", encode_url(buf, board, sizeof(buf)));
    http_quit();
}

/* modified by stiger,20030414 */
int do_del(char *board, int id)
{
    int fd;
    int ent;
    char dir[256];
    struct fileheader f;

    sprintf(dir, "boards/%s/.DIR", board);
    fd = open(dir, O_RDWR, 0644);
    if (fd < 0)
        http_fatal("����Ĳ���");
    if( get_records_from_id( fd, id, &f, 1, &ent) ){
	close(fd);
        switch (del_post(ent, &f, dir, board)) {
        case DONOTHING:
            http_fatal("����Ȩɾ������");
            break;
        default:
            printf("<tr><td>%s  </td><td>����:%s </td><td>ɾ���ɹ�.</td></tr>\n", f.owner, nohtml(f.title));
	}
	return;
    }

    printf("<tr><td></td><td></td><td>�ļ�������.</td></tr>\n");
}

int do_del_zd(char *board, int id)
{
    FILE *fp;
	int ffind=0;
    int ent=1;
    char dir[256];
    struct fileheader f;

    sprintf(dir, "boards/%s/.DINGDIR", board);

	fp = fopen(dir, "r+");
    if (fp == 0) 
        http_fatal("����Ĳ���"); 
    while (1) {
	    if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
		    break;               
	    if (f.id==id) {
		    ffind=1;
		    break;
	    }
	    ent++;
    }
    fclose(fp);

    if( ffind ){
        switch (del_post(ent, &f, dir, board)) {
        case DONOTHING:
            http_fatal("����Ȩɾ������");
            break;
        default:
            printf("<tr><td>%s  </td><td>����:%s </td><td>ɾ���ɹ�.</td></tr>\n", f.owner, nohtml(f.title));
	}
	return;
    }

    printf("<tr><td></td><td></td><td>�ļ�������.</td></tr>\n");
}

/* �� G ʱ��û�� post ����ժ�� */
/* modified by stiger,20030414 */
int do_set(char *board, int id, int flag)
{
    int fd;
    char dir[256];
    struct fileheader f;
    int ent;

	setbdir(DIR_MODE_NORMAL, dir, board);
    fd = open(dir, O_RDWR, 0644);
    if( fd >= 0 && get_records_from_id( fd, id, &f, 1, &ent) )
	{
        close(fd);
		if(change_post_flag(NULL, currentuser, 0, board, ent, &f, dir, flag, 0)!=DONOTHING)
			printf("<tr><td>%s</td><td>����:%s</td><td>��ǳɹ�.</td></tr>\n", f.owner, nohtml(f.title));
		else
			printf("<tr><td>%s</td><td>����:%s</td><td>��ǲ��ɹ�.</td></tr>\n", f.owner, nohtml(f.title));
    }else{
        close(fd);
        printf("<tr><td></td><td></td><td></td><td>�ļ�������.</td></tr>\n");
    }
    
}


int do_set_zd(char *board, int id, int flag)
{
    FILE *fp;
    char dir[256];
    struct fileheader f;
    int ent=1;
    int ffind=0;

    sprintf(dir,"boards/%s/.DINGDIR",board);
    
	fp = fopen(dir, "r+");
    if (fp == 0) 
        http_fatal("����Ĳ���"); 
    while (1) {
	    if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
		    break;               
	    if (f.id==id) {
		    ffind=1;
		    break;
	    }
	    ent++;
    }
    fclose(fp);

	if(ffind){
		if(change_post_flag(NULL, currentuser, 0, board, ent, &f, dir, flag, 0)!=DONOTHING)
			printf("<tr><td>%s</td><td>����:%s</td><td>��ǳɹ�.</td></tr>\n", f.owner, nohtml(f.title));
		else
			printf("<tr><td>%s</td><td>����:%s</td><td>��ǲ��ɹ�.</td></tr>\n", f.owner, nohtml(f.title));
    }else{
        printf("<tr><td></td><td></td><td></td><td>�ļ�������.</td></tr>\n");
    }
}
